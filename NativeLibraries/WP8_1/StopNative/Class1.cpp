// Class1.cpp
#include "pch.h"
#include "Class1.h"

#include "Threading\ServiceDataPolicy.h"
#include "Threading\ServiceOperation.h"
#include "Threading\Service.h"

#include <memory>
#include <vector>

typedef OperationDataPolicyWindows<int> IntDataPolicy; 
typedef ServiceOperation<IntDataPolicy, ServiceOperationRunPolicy> IntServiceOperation;
typedef ServiceStdThreadPolicy <IntDataPolicy, std::unique_ptr<IntServiceOperation>> IntService;
//typedef ServiceSimplePolicy <IntDataPolicy, std::unique_ptr<IntServiceOperation>> IntService;
//typedef ServiceSimplePolicy <IntDataPolicy, std::shared_ptr<IntServiceOperation>> IntService;
//typedef ServiceSimplePolicy <IntDataPolicy, IntServiceOperation> IntService;
//typedef ServiceSimplePolicy <IntDataPolicy, IntServiceOperation *> IntService;

// data -> operation(data) -> service

using namespace StopNative;
using namespace Platform;

Class1::Class1()
{
	IntDataPolicy idp;
	std::unique_ptr<IntServiceOperation> dd = std::unique_ptr<IntServiceOperation>(new IntServiceOperation);
	//std::shared_ptr<IntServiceOperation> dd = std::make_shared<IntServiceOperation>();
	/*IntServiceOperation dd;
	IntServiceOperation *p = &dd;*/
	//IntServiceOperation *dd = new IntServiceOperation;

	/*std::vector<std::unique_ptr<IntServiceOperation>> ops;

	ops.push_back(StdCopyPolicy::Copy(dd));*/

	IntService intServ;

	auto tmpRef = StdDerefPolicy::Deref(dd);

	intServ.AddOperation(dd);

}
