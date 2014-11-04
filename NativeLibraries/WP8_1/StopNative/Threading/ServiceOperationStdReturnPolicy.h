#pragma once
#include "ServiceOperationReturnPolicyBase.h"

#include <future>

template<class R>
class StdReturnContainer2{
public:
	void SetFuture(std::shared_future<R> &fut){
		this->fut = fut;
	}

	R GetValue(){
		return this->fut.get();
	}
private:
	std::shared_future<R> fut;
};

template<>
class StdReturnContainer2<void>{
public:
	void SetFuture(std::shared_future<void> &fut){
		this->fut = fut;
	}

	void GetValue(){
		this->fut.get();
	}
private:
	std::shared_future<void> fut;
};




template<class R, class ContainerImpl = StdReturnContainer2<R>>
class StdReturnPolicy2 : public ServiceOperationReturnPolicyBase<StdReturnPolicy2<R, ContainerImpl>, ContainerImpl, R>{
public:
	StdReturnPolicy2(){
		this->fut = this->prom.get_future();
	}

	~StdReturnPolicy2(){}

	ReturnContainerBase<ContainerImpl, R> GetResult(){
		ReturnContainerBase<ContainerImpl, R> res;
		res.impl.SetFuture(this->fut);
		return res;
	}

	void SetResult(R &&v){
		this->prom.set_value(v);
	}

	void SetException(std::exception_ptr e){
		this->prom.set_exception(e);
	}
private:
	std::promise<R> prom;
	std::shared_future<R> fut;
};

template<class ContainerImpl>
class StdReturnPolicy2<void, ContainerImpl> : public ServiceOperationReturnPolicyBase<StdReturnPolicy2<void, ContainerImpl>, ContainerImpl, void>{
public:
	StdReturnPolicy2(){
		this->fut = this->prom.get_future();
	}

	~StdReturnPolicy2(){}

	ReturnContainerBase<ContainerImpl, void> GetResult(){
		ReturnContainerBase<ContainerImpl, void> res;
		res.impl.SetFuture(this->fut);
		return res;
	}

	void SetResult(){
		this->prom.set_value();
	}

	void SetException(std::exception_ptr e){
		this->prom.set_exception(e);
	}
private:
	std::promise<void> prom;
	std::shared_future<void> fut;
};