#pragma once
#include "DisplayTexture\DX11DisplayTexture.h"

#include <wrl.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <dwrite_2.h>
#include <wincodec.h>
#include <DirectXMath.h>
#include <memory>

namespace DX11{

	class DeviceResources{
	public:
		DeviceResources();
		~DeviceResources();

		void Initialize(std::unique_ptr<DisplayTexture::DisplayTexture> &&displayTexture);
	private:
		// D3D
		Microsoft::WRL::ComPtr<ID3D11Device2>			d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext2>	d3dContext;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	d3dRenderTargetView;
		D3D11_VIEWPORT									screenViewport;

		// D2D
		Microsoft::WRL::ComPtr<ID2D1Factory2>			d2dFactory;
		Microsoft::WRL::ComPtr<ID2D1Device1>			d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext1>		d2dContext;
		Microsoft::WRL::ComPtr<ID2D1Bitmap1>			d2dTargetBitmap;

		// DirectWrite drawing components.
		Microsoft::WRL::ComPtr<IDWriteFactory2>			dwriteFactory;
		Microsoft::WRL::ComPtr<IWICImagingFactory2>		wicFactory;

		std::unique_ptr<DisplayTexture::DisplayTexture> displayTexture;

		void CreateDeviceIndependentResources();
		void CreateDeviceResources();
		void CreateSizeDependentResources();
	};

}