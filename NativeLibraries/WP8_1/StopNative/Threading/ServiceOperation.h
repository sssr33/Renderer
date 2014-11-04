#pragma once
#include "ServiceOperationReturnPolicyBase.h"
#include "ServiceDataPolicy.h"
#include "ServicePolicyBase.h"
#include "ServiceUnhandledExceptionHandler.h"

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

template<
	class Impl,
	class Data, template<class D> class DataPolicyImpl,
		template <class T> class OperationWrapper,
	class ExceptionHandlerImpl,
	class DerefPolicy = StdDerefPolicy>
class _ServiceOperation2Base{
public:
	typedef ServicePolicyBase<Impl, Data, DataPolicyImpl, OperationWrapper, ExceptionHandlerImpl, DerefPolicy> ServiceType;

	virtual ~_ServiceOperation2Base() {}

	void SetExceptionHandler(ServiceUnhandledExceptionHandler<ExceptionHandlerImpl> *exceptionHandler){
		this->exceptionHandler = exceptionHandler;
	}

	bool IsCancelled() const{
		return this->cancelled;
	}

	void Cancel(){
		this->cancelled = true;
	}

	virtual void RunVirtual(ServiceType &service) = 0;
protected:
	ServiceUnhandledExceptionHandler<ExceptionHandlerImpl> *exceptionHandler;

	_ServiceOperation2Base()
		: cancelled(false){}
private:
	bool cancelled;
};




template<
	class Impl, class ServImpl,
	class Data, template<class D> class DataPolicyImpl,
	class Ret, template <class Ret> class RetPolicyContainerImpl, template <class Ret, class RetPolicyContainerImpl> class RetPolicyImpl,
	class ExceptionHandlerImpl,
		template <class T> class OperationWrapper,
	class DerefPolicy = StdDerefPolicy>
class ServiceOperation2Base : public _ServiceOperation2Base < ServImpl, Data, DataPolicyImpl, OperationWrapper, ExceptionHandlerImpl, DerefPolicy > {
public:
	virtual ~ServiceOperation2Base() {}

	ReturnContainerBase<RetPolicyContainerImpl<Ret>, Ret> GetResult(){
		return this->retPolicy.GetResult();
	}

	virtual void RunVirtual(ServiceType &service) override{
		try{
			this->retPolicy.SetResult(this->Run(service));
		}
		catch (...){
			this->retPolicy.SetException(std::current_exception());
			if (this->exceptionHandler){
				this->exceptionHandler->HandleException(std::current_exception());
			}
		}
	}

	Ret Run(ServiceType &service){
		return static_cast<Impl*>(this)->Run(service);
	}
protected:
	ServiceOperation2Base()
		: retPolicy(_retPolicy){}
private:
	typedef RetPolicyImpl<Ret, RetPolicyContainerImpl<Ret>> RetPolicyImplType;
	RetPolicyImplType _retPolicy;
	ServiceOperationReturnPolicyBase<RetPolicyImplType, RetPolicyContainerImpl<Ret>, Ret> &retPolicy;
};

// void specialization
template<
	class Impl, class ServImpl,
	class Data, template<class D> class DataPolicyImpl,
	template <class Ret> class RetPolicyContainerImpl, template <class Ret, class RetPolicyContainerImpl> class RetPolicyImpl,
	class ExceptionHandlerImpl,
	template <class T> class OperationWrapper,
	class DerefPolicy>
class ServiceOperation2Base<
	Impl, ServImpl, 
	Data, DataPolicyImpl, 
	void, RetPolicyContainerImpl, RetPolicyImpl, 
	ExceptionHandlerImpl,
	OperationWrapper, 
	DerefPolicy> : public _ServiceOperation2Base < ServImpl, Data, DataPolicyImpl, OperationWrapper, DerefPolicy >{
public:
	virtual ~ServiceOperation2Base() {}

	ReturnContainerBase<RetPolicyContainerImpl<void>, void> GetResult(){
		return this->retPolicy.GetResult();
	}

	void SetExceptionHandler(ServiceUnhandledExceptionHandler<ExceptionHandlerImpl> *exceptionHandler){
		this->exceptionHandler = exceptionHandler;
	}

	virtual void RunVirtual(ServiceType &service) override{
		try{
			this->Run(service);
			this->retPolicy.SetResult();
		}
		catch (...){
			this->retPolicy.SetException(std::current_exception());
			if (this->exceptionHandler){
				this->exceptionHandler->HandleException(std::current_exception());
			}
		}
	}

	void Run(ServiceType &service){
		static_cast<Impl*>(this)->Run(service);
	}
protected:
	ServiceUnhandledExceptionHandler<ExceptionHandlerImpl> *exceptionHandler;

	ServiceOperation2Base()
		: retPolicy(_retPolicy), exceptionHandler(nullptr){}
private:
	typedef RetPolicyImpl<void, RetPolicyContainerImpl<void>> RetPolicyImplType;
	RetPolicyImplType _retPolicy;
	ServiceOperationReturnPolicyBase<RetPolicyImplType, RetPolicyContainerImpl<void>, void> &retPolicy;
};