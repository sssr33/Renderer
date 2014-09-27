#pragma once
#include "ServiceOperationRBase.h"

#include <type_traits>

template<class Callable, class ServiceOperationBase,
class R = void, template <class T> class ReturnPolicy = StdReturnPolicy>
class ServiceOperationRBaseGeneric : public ServiceOperationRBase<ServiceOperationBase, R, ReturnPolicy>
{
public:

	ServiceOperationRBaseGeneric(Callable &func)
		: func(func)
	{}

	virtual ~ServiceOperationRBaseGeneric() {}

	virtual void Run(typename ServiceOperationBase::DataPolicy &serviceState) override
	{
		this->SetResult(this->func(serviceState));
	}

private:

	Callable func;

};

template<class ServiceOperationBase, template <class T> class ReturnPolicy = StdReturnPolicy>
struct ServiceOperationRBaseGenericFactory
{
	template<class Callable>
	static ServiceOperationRBaseGeneric<
		Callable,
		ServiceOperationBase,
		typename std::result_of<Callable(typename ServiceOperationBase::DataPolicy &)>::type,
		ReturnPolicy> *Create(Callable func)
	{
		typedef typename std::result_of<Callable(typename ServiceOperationBase::DataPolicy &)>::type RetType;
		typedef ServiceOperationRBaseGeneric <Callable, ServiceOperationBase, RetType, ReturnPolicy> OpType;

		OpType *op = new OpType(func);

		return op;
	}

	/*template<class Callable>
	static ServiceOperationRBaseGeneric<
		Callable,
		ServiceOperationBase,
		typename std::result_of<Callable(typename ServiceOperationBase::DataPolicy &)>::type,
		ReturnPolicy> *Create(Callable &&func)
	{
		typedef typename std::result_of<Callable(typename ServiceOperationBase::DataPolicy &)>::type RetType;
		typedef ServiceOperationRBaseGeneric <Callable, ServiceOperationBase, RetType, ReturnPolicy> OpType;

		OpType *op = new OpType(func);

		return op;
	}*/
};