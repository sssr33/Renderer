#include "pch.h"
#include "DX11ExceptDeviceLost.h"

namespace DX11{
	namespace Exception{

		DeviceLost::DeviceLost()
			: std::runtime_error("DeviceLost"){
		}

		DeviceLost::~DeviceLost(){
		}

	}
}