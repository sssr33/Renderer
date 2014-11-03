#pragma once
#include "ServiceOperation.h"
#include "ServiceDataPolicyBase.h"
#include "ServicePolicyHelpers.h"

#include <memory>

template<class Impl, class Data, template<class D> class DataPolicyImpl, template <class T> class OperationWrapper, class DerefPolicy>
class _ServiceOperation2Base;

template<class Impl, class ServImpl, class Data, template<class D> class DataPolicyImpl, template <class T> class OperationWrapper, class DerefPolicy>
class ServiceOperation2Base;

template<class Impl, class Data, template<class D> class DataPolicyImpl, template <class T> class OperationWrapper, class DerefPolicy = StdDerefPolicy>
class ServicePolicyBase{
public:
	typedef Impl Impl;
	typedef OperationWrapper<_ServiceOperation2Base<Impl, Data, DataPolicyImpl, OperationWrapper, DerefPolicy>> OperationType;
	template<class OpImpl>
	struct OperationBase{
		typedef ServiceOperation2Base<OpImpl, Impl, Data, DataPolicyImpl, OperationWrapper, DerefPolicy> Type;
	};

	ServicePolicyBase()
		: serviceData(_serviceData){}

	void AddOperation(OperationType &op){
		static_cast<Impl*>(this)->AddOperation(op);
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
	~ServicePolicyBase(){}
};