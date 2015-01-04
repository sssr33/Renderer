#include "pch.h"
#include "DX11MathOperators.h"

#include <algorithm>

namespace DX11{

	DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2 &a, const DirectX::XMFLOAT2 &b){
		DirectX::XMFLOAT2 res;

		res.x = a.x * b.x;
		res.y = a.y * b.y;

		return res;
	}

	bool operator==(const DirectX::XMFLOAT2 &a, const DirectX::XMFLOAT2 &b){
		bool res = a.x == b.x && a.y == b.y;
		return res;
	}

	bool operator!=(const DirectX::XMFLOAT2 &a, const DirectX::XMFLOAT2 &b){
		return !operator==(a, b);
	}

	DirectX::XMFLOAT2 MathOperators::Max(const DirectX::XMFLOAT2 &a, float b){
		DirectX::XMFLOAT2 res;

		res.x = (std::max)(a.x, b);
		res.y = (std::max)(a.y, b);

		return res;
	}

}