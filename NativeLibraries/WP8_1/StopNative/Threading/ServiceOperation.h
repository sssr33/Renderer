#pragma once
#include "ServiceDataPolicy.h"
#include "ServiceOperationReturnPolicy.h"

#include <future>

template<class ServiceDataPolicy>
class ServiceOperationTestRunPolicy
{
public:

	void Run(const ServiceDataPolicy &serviceData)
	{
		int stop = 324;
	}

protected:

	~ServiceOperationTestRunPolicy() {}

};

template<class ServiceDataPolicy, template <class T> class ServiceOperationRunPolicy, 
class R = void, template <class T> class ReturnPolicy = StdReturnPolicy>
class ServiceOperation : public ServiceOperationRunPolicy<ServiceDataPolicy>, public ReturnPolicy<R>
{
public:

	ServiceOperation()
		: cancelled(false)
	{}

	bool IsCancelled() const
	{
		return this->cancelled;
	}

	void Cancel()
	{
		this->cancelled = true;
	}

	void Run(const ServiceDataPolicy &serviceData)
	{
		ServiceOperationRunPolicy<ServiceDataPolicy>::Run(serviceData);
	}

//protected:

	~ServiceOperation() {}

private:

	bool cancelled;
};