#pragma once
#include "ServiceOperation.h"
#include "ServicePolicyBase.h"

#include <memory>

template<class ServiceDataPolicy, class OperationType, class DerefPolicy = StdDerefPolicy>
class ServiceSimplePolicy
{
public:

	ServiceSimplePolicy()
	{}

	void AddOperation(OperationType &op)
	{
		DerefPolicy::Deref(op)->Run(this->serviceData);
	}

//protected:

	~ServiceSimplePolicy()
	{}

private:

	ServiceDataPolicy serviceData;

};

template<class Data, template<class D> class DataPolicyImpl, template<class T> class OperationWrapper, class DerefPolicy = StdDerefPolicy>
class ServiceSimplePolicy2 : public ServicePolicyBase<ServiceSimplePolicy2<Data, DataPolicyImpl, OperationWrapper, DerefPolicy>, Data, DataPolicyImpl, OperationWrapper, DerefPolicy>{
public:
	ServiceSimplePolicy2(){}

	void AddOperation(typename ServicePolicyBase::OperationType &op){
		DerefPolicy::Deref(op)->RunVirtual(*this);
	}

	~ServiceSimplePolicy2(){}
};