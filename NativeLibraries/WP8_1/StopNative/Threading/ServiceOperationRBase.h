#pragma once
#include "ServiceOperation.h"
#include "ServiceOperationReturnPolicy.h"

template<class ServiceOperationBase,
class R = void, template <class T> class ReturnPolicy = StdReturnPolicy>
class ServiceOperationRBase : 
	public ServiceOperationBase,
	public ReturnPolicy<R>
{
public:

	typedef ServiceOperationBase Base;

	ServiceOperationRBase() {}

	virtual ~ServiceOperationRBase() {}
};