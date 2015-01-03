#include "pch.h"
#include "DX11H.h"
#include "..\Exceptions\DX11ExceptHresultException.h"

#include <d3d11_2.h>

namespace DX11{

	void H::ThrowIfFailed(HRESULT hr){
		if (FAILED(hr)){
			// Set a breakpoint on this line to catch Win32 API errors.
			/*throw Platform::Exception::CreateException(hr);*/
			throw DX11::Exception::HresultException(hr);
		}
	}

	bool H::SdkLayersAvailable(){
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
			0,
			D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
			nullptr,                    // Any feature level will do.
			0,
			D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
			nullptr,                    // No need to keep the D3D device reference.
			nullptr,                    // No need to know the feature level.
			nullptr                     // No need to keep the D3D device context reference.
			);

		return SUCCEEDED(hr);
	}

}