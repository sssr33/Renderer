#pragma once
#include "DX11MathOperators.h"

#include <Windows.h>

namespace DX11{

	class H{
	public:
		typedef MathOperators MathOp;

		static void ThrowIfFailed(HRESULT hr);
		static bool SdkLayersAvailable();
	};

}