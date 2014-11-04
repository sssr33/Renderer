#pragma once
#include "ServiceOperation.h"
#include "ServiceDataPolicyBase.h"
#include "ServicePolicyHelpers.h"
#include "ServiceOperationStdReturnPolicy.h"
#include "ServiceUnhandledExceptionHandler.h"

#include <memory>

template<
	class Impl,
	class Data, template<class D> class DataPolicyImpl,
		template <class T> class OperationWrapper,
	class ExceptionHandlerImpl,
	class DerefPolicy>
class _ServiceOperation2Base;

template<
	class Impl, class ServImpl,
	class Data, template<class D> class DataPolicyImpl,
	class Ret, template <class Ret> class RetPolicyContainerImpl, template <class Ret, class RetPolicyContainerImpl> class RetPolicyImpl,
	class ExceptionHandlerImpl,
		template <class T> class OperationWrapper,
	class DerefPolicy>
class ServiceOperation2Base;

template<class Impl, class Data, template<class D> class DataPolicyImpl, template <class T> class OperationWrapper, class ExceptionHandlerImpl = EmptyServiceUnhandledExceptionHandler, class DerefPolicy = StdDerefPolicy>
class ServicePolicyBase{
public:
	typedef Impl Impl;
	typedef ServiceUnhandledExceptionHandler<ExceptionHandlerImpl> ExceptionHandlerType;
	typedef OperationWrapper<_ServiceOperation2Base<Impl, Data, DataPolicyImpl, OperationWrapper, ExceptionHandlerImpl, DerefPolicy>> OperationType;

	template<
		class OpImpl, 
		class Ret, template <class Ret> class RetPolicyContainerImpl, template <class Ret, class RetPolicyContainerImpl> class RetPolicyImpl>
	struct OperationBase{
		typedef ServiceOperation2Base<
			OpImpl, Impl, 
			Data, DataPolicyImpl, 
			Ret, RetPolicyContainerImpl, RetPolicyImpl,
			ExceptionHandlerImpl,
			OperationWrapper,
			DerefPolicy> Type;
	};

	template<
		class OpImpl,
		class Ret>
	struct OperationBaseStd{
		typedef ServiceOperation2Base<
			OpImpl, Impl,
			Data, DataPolicyImpl,
			Ret, StdReturnContainer2, StdReturnPolicy2,
			ExceptionHandlerImpl,
			OperationWrapper,
			DerefPolicy> Type;
	};

	ServicePolicyBase()
		: serviceData(_serviceData){}

	void SetExceptionHandler(std::unique_ptr<ExceptionHandlerType> &&handler){
		this->exceptionHandler = std::move(handler);
	}

	void AddOperation(OperationType &op){
		DerefPolicy::Deref(op)->SetExceptionHandler(this->exceptionHandler.get());
		static_cast<Impl*>(this)->AddOperationImpl(op);
	}

	Data &AcquireTaskData(){
		return this->serviceData.AcquireTaskData();
	}

	void ReturnTaskData(){
		this->serviceData.ReturnTaskData();
	}

protected:
	DataPolicyImpl<Data> _serviceData;
	ServiceDataPolicyBase<DataPolicyImpl<Data>, Data> &serviceData;
	std::unique_ptr<ExceptionHandlerType> exceptionHandler;

	~ServicePolicyBase(){}
};