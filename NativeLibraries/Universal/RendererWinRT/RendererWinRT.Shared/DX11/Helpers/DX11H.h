#pragma once

#include <Windows.h>

namespace DX11{

	class H{
	public:
		static void ThrowIfFailed(HRESULT hr);
		static bool SdkLayersAvailable();
	};

}