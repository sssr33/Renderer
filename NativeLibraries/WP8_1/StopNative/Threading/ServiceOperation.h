#pragma once
#include "ServiceDataPolicy.h"

template<class ServiceDataPolicy>
class ServiceOperation
{
public:

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