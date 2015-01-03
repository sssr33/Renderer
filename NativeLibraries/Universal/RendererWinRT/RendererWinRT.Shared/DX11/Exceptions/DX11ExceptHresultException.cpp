#include "pch.h"
#include "DX11ExceptHresultException.h"

#include <comdef.h>

namespace DX11{
	namespace Exception{

		HresultException::HresultException(HRESULT hr)
			: std::runtime_error("HresultException"), hr(hr){
		}

		HresultException::~HresultException(){
		}

		HRESULT HresultException::GetHResult() const{
			return this->hr;
		}

	}
}