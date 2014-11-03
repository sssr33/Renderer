#pragma once
#include "ServiceDataPolicyBase.h"

#include <Windows.h>
#include <mutex>

template<class Data>
class OperationDataPolicyWindows{
public:
	OperationDataPolicyWindows(){
		InitializeCriticalSectionEx(&this->taskDataCS, 3000, 0);
	}

	Data &AcquireTaskData(){
		EnterCriticalSection(&this->taskDataCS);
		return this->taskData;
	}

	void ReturnTaskData(){
		LeaveCriticalSection(&this->taskDataCS);
	}
//protected:
	// protected because don't need vtable overhead or possible memory leaks
	~OperationDataPolicyWindows(){
		DeleteCriticalSection(&this->taskDataCS);
	}
private:
	CRITICAL_SECTION taskDataCS;
	Data taskData;
};




template<class Data>
class OperationDataPolicyStd{
public:
	OperationDataPolicyStd(){}

	Data &AcquireTaskData(){
		this->taskDataMtx.lock();
		return this->taskData;
	}

	void ReturnTaskData(){
		this->taskDataMtx.unlock();
	}
//protected:
	// protected because don't need vtable overhead or possible memory leaks
	~OperationDataPolicyStd(){}
private:
	std::mutex taskDataMtx;
	Data taskData;
};




template<class Data>
class OperationDataPolicyWindows2 : public ServiceDataPolicyBase<OperationDataPolicyWindows2<Data>, Data>{
public:
	OperationDataPolicyWindows2(){
		InitializeCriticalSectionEx(&this->taskDataCS, 3000, 0);
	}

	~OperationDataPolicyWindows2(){
		DeleteCriticalSection(&this->taskDataCS);
	}

	Data &AcquireTaskData(){
		EnterCriticalSection(&this->taskDataCS);
		return this->taskData;
	}

	void ReturnTaskData(){
		LeaveCriticalSection(&this->taskDataCS);
	}
private:
	CRITICAL_SECTION taskDataCS;
	Data taskData;
};




template<class Data>
class OperationDataPolicyStd2 : public ServiceDataPolicyBase<OperationDataPolicyStd2<Data>, Data>{
public:
	OperationDataPolicyStd2(){}

	~OperationDataPolicyStd2(){}

	Data &AcquireTaskData(){
		this->taskDataMtx.lock();
		return this->taskData;
	}

	void ReturnTaskData(){
		this->taskDataMtx.unlock();
	}
private:
	std::mutex taskDataMtx;
	Data taskData;
};