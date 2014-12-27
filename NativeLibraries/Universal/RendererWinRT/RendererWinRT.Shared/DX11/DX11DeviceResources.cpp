#include "pch.h"
#include "DX11DeviceResources.h"
#include "DX11Error.h"

namespace DX11{

	DeviceResources::DeviceResources(){
	}

	DeviceResources::~DeviceResources(){
	}

	void DeviceResources::Initialize(std::unique_ptr<DisplayTexture::DisplayTexture> &&displayTexture){
		this->displayTexture = std::move(displayTexture);

		this->CreateDeviceIndependentResources();
		this->CreateDeviceResources();
		this->CreateSizeDependentResources();
	}

	void DeviceResources::CreateDeviceIndependentResources(){
		D2D1_FACTORY_OPTIONS options = {};

#if defined(_DEBUG)
		// If the project is in a debug build, enable Direct2D debugging via SDK Layers.
		options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

		// Initialize the Direct2D Factory.
		ThrowIfFailed(
			D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			__uuidof(ID2D1Factory2),
			&options,
			&this->d2dFactory
			)
			);

		// Initialize the DirectWrite Factory.
		ThrowIfFailed(
			DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory2),
			&this->dwriteFactory
			)
			);

		// Initialize the Windows Imaging Component (WIC) Factory.
		ThrowIfFailed(
			CoCreateInstance(
			CLSID_WICImagingFactory2,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(this->wicFactory.ReleaseAndGetAddressOf())
			)
			);
	}

	void DeviceResources::CreateDeviceResources(){

	}

	void DeviceResources::CreateSizeDependentResources(){

	}

}