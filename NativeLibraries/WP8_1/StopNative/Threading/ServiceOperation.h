#pragma once
#include "ServiceDataPolicy.h"

template<class ServiceDataPolicy>
class ServiceOperationRunPolicy
{
public:

	void Run(const ServiceDataPolicy &serviceData)
	{
		int stop = 324;
	}

protected:

	~ServiceOperationRunPolicy() {}

};

template<class ServiceDataPolicy, template <class T> class ServiceOperationDoPolicy>
class ServiceOperation : public ServiceOperationRunPolicy<ServiceDataPolicy>
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