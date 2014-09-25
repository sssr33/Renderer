#pragma once

#include <exception>
#include <future>

template<class R>
class StdReturnContainer
{
public:

	StdReturnContainer(std::shared_future<R> &fut)
		: fut(fut)
	{}

	const R &GetValue() const
	{
		return this->fut.get();
	}

private:

	std::shared_future<R> fut;

};

template<class R = void>
class StdReturnPolicy
{
public:

	StdReturnPolicy()
	{
		this->fut = this->prom.get_future();
	}

	StdReturnContainer<R> GetResult()
	{
		StdReturnContainer<R> res(this->fut);
		return res;
	}

protected:

	void SetResult(R &&v)
	{
		this->prom.set_value(v);
	}

	void SetException(std::exception_ptr e)
	{
		this->prom.set_exception(e);
	}

	~StdReturnPolicy(){}

private:

	std::promise<R> prom;
	std::shared_future<R> fut;
};