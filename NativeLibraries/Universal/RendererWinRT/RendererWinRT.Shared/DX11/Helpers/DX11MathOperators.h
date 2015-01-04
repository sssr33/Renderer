#pragma once

#include <DirectXMath.h>

namespace DX11{

	DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2 &a, const DirectX::XMFLOAT2 &b);
	bool operator==(const DirectX::XMFLOAT2 &a, const DirectX::XMFLOAT2 &b);
	bool operator!=(const DirectX::XMFLOAT2 &a, const DirectX::XMFLOAT2 &b);

	class MathOperators{
	public:
		static DirectX::XMFLOAT2 Max(const DirectX::XMFLOAT2 &a, float b);
	};

}