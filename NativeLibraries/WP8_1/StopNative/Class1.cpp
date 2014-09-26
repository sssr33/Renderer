// Class1.cpp
#include "pch.h"
#include "Class1.h"

#include "Threading\ServiceDataPolicy.h"
#include "Threading\ServiceOperation.h"
#include "Threading\ServiceOperationRBase.h"
#include "Threading\Service.h"

#include <memory>
#include <vector>
#include <functional>
#include <type_traits>

typedef OperationDataPolicyWindows<int> IntDataPolicy; 
typedef ServiceOperation<IntDataPolicy> IntOperation;

template<class R>
struct IntRServiceOperation
{
	typedef ServiceOperationRBase<IntOperation, R> Type;
};

typedef ServiceStdThreadPolicy <IntDataPolicy, std::unique_ptr<IntOperation>> IntService;
//typedef ServiceSimplePolicy <IntDataPolicy, std::unique_ptr<IntServiceOperation>> IntService;
//typedef ServiceSimplePolicy <IntDataPolicy, std::shared_ptr<IntServiceOperation>> IntService;
//typedef ServiceSimplePolicy <IntDataPolicy, IntServiceOperation> IntService;
//typedef ServiceSimplePolicy <IntDataPolicy, IntServiceOperation *> IntService;

// data -> operation(data) -> service

class SimpleRCOperation : public IntRServiceOperation<float>::Type
{
public:

	SimpleRCOperation(){}
	virtual ~SimpleRCOperation(){}

	virtual void Run(IntDataPolicy &serviceState) override
	{
		this->SetResult(3.14f);
	}

};

template<class R, class Callable>
class GenericRCOperation : public IntRServiceOperation<R>::Type
{
public:

	GenericRCOperation(Callable &func)
		: func(func)
	{}
	virtual ~GenericRCOperation(){}

	virtual void Run(IntDataPolicy &serviceState) override
	{
		this->SetResult(this->func(serviceState));
	}

private:

	Callable func;

};

template<class Callable>
typename GenericRCOperation<typename std::result_of<Callable(IntDataPolicy &)>::type, Callable>::ReturnType AddGenericIntOperation(Callable func, IntService *serv)
{
	typedef typename std::result_of<Callable(IntDataPolicy &)>::type R;
	GenericRCOperation<R, Callable> *opTmp = new GenericRCOperation<R, Callable>(func);
	GenericRCOperation<R, Callable>::ReturnType result = opTmp->GetResult();
	std::unique_ptr<IntOperation> opUnique = std::unique_ptr<IntOperation>(opTmp);

	serv->AddOperation(opUnique);

	return result;
}

using namespace StopNative;
using namespace Platform;

Class1::Class1()
{
	IntDataPolicy idp;
	auto opTmp = new SimpleRCOperation;
	auto opResult = opTmp->GetResult();
	std::unique_ptr<IntOperation> dd = std::unique_ptr<IntOperation>(opTmp);
	//std::shared_ptr<IntServiceOperation> dd = std::make_shared<IntServiceOperation>();
	/*IntServiceOperation dd;
	IntServiceOperation *p = &dd;*/
	//IntServiceOperation *dd = new IntServiceOperation;

	/*std::vector<std::unique_ptr<IntServiceOperation>> ops;

	ops.push_back(StdCopyPolicy::Copy(dd));*/

	IntService intServ;

	auto tmpRef = StdDerefPolicy::Deref(dd);

	intServ.AddOperation(dd);

	auto ttt = 
		[&](IntDataPolicy &serviceState)
	{
		return 6.28; 
	};

	auto res222 = AddGenericIntOperation(ttt, &intServ);

	auto r2 = res222.GetValue();
	auto res = opResult.GetValue();


	int stop = 324;

}
