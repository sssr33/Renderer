#pragma once
#include "ServiceDataPolicy.h"
#include "ServicePolicyBase.h"

template<class ServiceDataPolicy>
class ServiceOperation
{
public:

	typedef ServiceDataPolicy DataPolicy;

	ServiceOperation()
		: cancelled(false)
	{}

	virtual ~ServiceOperation() {}

	bool IsCancelled() const
	{
		return this->cancelled;
	}

	void Cancel()
	{
		this->cancelled = true;
	}

	virtual void Run(ServiceDataPolicy &serviceData) = 0;

private:

	bool cancelled;
};

template<class Impl, class Data, template<class D> class DataPolicyImpl, template <class T> class OperationWrapper, class DerefPolicy = StdDerefPolicy>
class _ServiceOperation2Base{
public:
	typedef ServicePolicyBase<Impl, Data, DataPolicyImpl, OperationWrapper, DerefPolicy> ServiceType;

	virtual ~_ServiceOperation2Base() {}

	bool IsCancelled() const{
		return this->cancelled;
	}

	void Cancel(){
		this->cancelled = true;
	}

	virtual void RunVirtual(ServiceType &service) = 0;
protected:
	_ServiceOperation2Base()
		: cancelled(false){}
private:
	bool cancelled;
};

template<class Impl, class ServImpl, class Data, template<class D> class DataPolicyImpl, template <class T> class OperationWrapper, class DerefPolicy = StdDerefPolicy>
class ServiceOperation2Base : public _ServiceOperation2Base<ServImpl, Data, DataPolicyImpl, OperationWrapper, DerefPolicy>{
public:
	virtual ~ServiceOperation2Base() {}

	bool IsCancelled() const{
		return this->cancelled;
	}

	void Cancel(){
		this->cancelled = true;
	}

	virtual void RunVirtual(ServiceType &service) override{
		this->Run(service);
	}

	void Run(ServiceType &service){
		static_cast<Impl*>(this)->Run(service);
	}
protected:
	ServiceOperation2Base(){}
private:
	bool cancelled;
};