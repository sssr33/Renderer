﻿// Class1.cpp
#include "pch.h"
#include "Class1.h"

#include "Threading\ServiceDataPolicy.h"
#include "Threading\ServiceOperation.h"
#include "Threading\ServiceOperationRBase.h"
#include "Threading\ServiceOperationRBaseGeneric.h"
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

int OperationFn(IntDataPolicy &serviceState)
{
	return 123;
}

struct OperationCallableObj
{
	short operator()(IntDataPolicy &serviceState)
	{
		return 12345;
	}
};

auto operationLambda =
[&](IntDataPolicy &serviceState)
{
	return 6.28;
};

std::function<float(IntDataPolicy &serviceState)> operationFunctionObj = 
[&](IntDataPolicy &serviceState)
{
	float res = static_cast<float>(operationLambda(serviceState) * 2.0);
	return res;
};

using namespace StopNative;
using namespace Platform;

Class1::Class1()
{
	IntService intServ;

	auto opTmp = new SimpleRCOperation;
	auto opResult = opTmp->GetResult();
	std::unique_ptr<IntOperation> simpleIntOp = std::unique_ptr<IntOperation>(opTmp);
	//std::shared_ptr<IntServiceOperation> dd = std::make_shared<IntServiceOperation>();
	/*IntServiceOperation dd;
	IntServiceOperation *p = &dd;*/
	//IntServiceOperation *dd = new IntServiceOperation;

	/*std::vector<std::unique_ptr<IntServiceOperation>> ops;

	ops.push_back(StdCopyPolicy::Copy(dd));*/

	auto tmpRef = StdDerefPolicy::Deref(simpleIntOp);
	intServ.AddOperation(simpleIntOp);

	auto res = opResult.GetValue();

	auto fnBasedOp = ServiceOperationRBaseGenericFactory<IntOperation>::Create(OperationFn);
	auto objBasedOp = ServiceOperationRBaseGenericFactory<IntOperation>::Create(OperationCallableObj());
	auto lambdaBasedOp = ServiceOperationRBaseGenericFactory<IntOperation>::Create(operationLambda);
	auto funcObjBasedOp = ServiceOperationRBaseGenericFactory<IntOperation>::Create(operationFunctionObj);

	auto fnBasedOpRes = fnBasedOp->GetResult();
	auto objBasedOpRes = objBasedOp->GetResult();
	auto lambdaBasedOpRes = lambdaBasedOp->GetResult();
	auto funcObjBasedOpRes = funcObjBasedOp->GetResult();

	intServ.AddOperation(std::unique_ptr<IntOperation>(fnBasedOp));
	intServ.AddOperation(std::unique_ptr<IntOperation>(objBasedOp));
	intServ.AddOperation(std::unique_ptr<IntOperation>(lambdaBasedOp));
	intServ.AddOperation(std::unique_ptr<IntOperation>(funcObjBasedOp));

	auto fnRes = fnBasedOpRes.GetValue();
	auto objRes = objBasedOpRes.GetValue();
	auto lambdaRes = lambdaBasedOpRes.GetValue();
	auto fnObjRes = funcObjBasedOpRes.GetValue();

	int stop = 324;

}
