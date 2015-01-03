#pragma once

#include <stdexcept>
#include <Windows.h>

namespace DX11{
	namespace Exception{

		class HresultException : public std::runtime_error{
		public:
			HresultException(HRESULT hr);
			virtual ~HresultException();

			HRESULT GetHResult() const;

		private:
			HRESULT hr;
		};

	}
}