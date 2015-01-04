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

		Microsoft::WRL::ComPtr<ID3D11Device2> GetD3DDevice() const;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext2> GetD3DContext() const;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetD3DDisplayTarget() const;
		const D3D11_VIEWPORT &GetD3DDisplayViewport() const;
		D3D_FEATURE_LEVEL GetD3DFeatureLevel() const;

		Microsoft::WRL::ComPtr<ID2D1Device1> GetD2DDevice() const;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext1> GetD2DContext() const;
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> GetD2DDisplayTarget() const;
		Microsoft::WRL::ComPtr<ID2D1Factory2> GetD2DFactory() const;

		Microsoft::WRL::ComPtr<IDWriteFactory2> GetDWriteFactory() const;
		Microsoft::WRL::ComPtr<IWICImagingFactory2> GetWICFactory() const;

		void SetDpi(float v);
		void SetScale(const DirectX::XMFLOAT2 &v);
		void SetLogicalSize(const DirectX::XMFLOAT2 &v);
		void SetCurrentOrientation(DisplayTexture::Orientation v);

		void Initialize(std::unique_ptr<DisplayTexture::DisplayTexture> &&displayTexture);

		void SetD3DDisplayTarget(ID3D11DepthStencilView *depthStencil = nullptr);
		void SetD2DDisplayTarget();
		void Present();
		void Trim();
		// This method is called in the event handler for the DisplayContentsInvalidated event.
		void ValidateDevice();
	private:
		// D3D
		Microsoft::WRL::ComPtr<ID3D11Device2>			d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext2>	d3dContext;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	d3dRenderTargetView;
		D3D11_VIEWPORT									screenViewport;
		D3D_FEATURE_LEVEL								d3dFeatureLevel;

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
		void CreateRenderTargets();
	};

}