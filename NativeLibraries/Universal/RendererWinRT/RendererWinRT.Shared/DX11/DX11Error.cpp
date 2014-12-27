#include "pch.h"
#include "DX11Error.h"

namespace DX11{

	void ThrowIfFailed(HRESULT hr){
		if (FAILED(hr)){
			// Set a breakpoint on this line to catch Win32 API errors.
			throw Platform::Exception::CreateException(hr);
		}
	}

}