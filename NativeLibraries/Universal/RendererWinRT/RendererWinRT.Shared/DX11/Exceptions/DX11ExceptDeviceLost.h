#pragma once

#include <stdexcept>

namespace DX11{
	namespace Exception{

		class DeviceLost : public std::runtime_error{
		public:
			DeviceLost();
			virtual ~DeviceLost();

			/*virtual const char* what() const throw();*/
		};

	}
}