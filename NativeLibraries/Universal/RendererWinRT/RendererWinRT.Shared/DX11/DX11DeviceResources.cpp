#include "pch.h"
#include "DX11DeviceResources.h"
#include "Helpers\DX11H.h"
#include "Exceptions\DX11Except.h"

namespace DX11{

	DeviceResources::DeviceResources()
		: d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1){
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
		H::ThrowIfFailed(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			__uuidof(ID2D1Factory2),
			&options,
			&this->d2dFactory
			));

		// Initialize the DirectWrite Factory.
		H::ThrowIfFailed(DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory2),
			&this->dwriteFactory
			));

		// Initialize the Windows Imaging Component (WIC) Factory.
		H::ThrowIfFailed(CoCreateInstance(
			CLSID_WICImagingFactory2,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(this->wicFactory.ReleaseAndGetAddressOf())
			));
	}

	void DeviceResources::CreateDeviceResources(){
		// This flag adds support for surfaces with a different color channel ordering
		// than the API default. It is required for compatibility with Direct2D.
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
		if (H::SdkLayersAvailable()){
			// If the project is in a debug build, enable debugging via SDK Layers with this flag.
			creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
		}
#endif
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		Microsoft::WRL::ComPtr<ID3D11Device> device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

		HRESULT hr = D3D11CreateDevice(
			nullptr,					// Specify nullptr to use the default adapter.
			D3D_DRIVER_TYPE_HARDWARE,	// Create a device using the hardware graphics driver.
			0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
			creationFlags,				// Set debug and Direct2D compatibility flags.
			featureLevels,				// List of feature levels this app can support.
			ARRAYSIZE(featureLevels),	// Size of the list above.
			D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Windows Store apps.
			&device,					// Returns the Direct3D device created.
			&this->d3dFeatureLevel,			// Returns feature level of device created.
			&context					// Returns the device immediate context.
			);

		if (FAILED(hr)){
			// If the initialization fails, fall back to the WARP device.
			// For more information on WARP, see: 
			// http://go.microsoft.com/fwlink/?LinkId=286690
			H::ThrowIfFailed(D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
				0,
				creationFlags,
				featureLevels,
				ARRAYSIZE(featureLevels),
				D3D11_SDK_VERSION,
				&device,
				&this->d3dFeatureLevel,
				&context
				));
		}

		// Store pointers to the Direct3D 11.1 API device and immediate context.
		H::ThrowIfFailed(device.As(&this->d3dDevice));
		H::ThrowIfFailed(context.As(&this->d3dContext));

		// Create the Direct2D device object and a corresponding context.
		Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;

		H::ThrowIfFailed(this->d3dDevice.As(&dxgiDevice));
		H::ThrowIfFailed(this->d2dFactory->CreateDevice(dxgiDevice.Get(), &this->d2dDevice));
		H::ThrowIfFailed(this->d2dDevice->CreateDeviceContext(
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			&this->d2dContext
			));
	}

	void DeviceResources::CreateSizeDependentResources(){
		HRESULT hr = S_OK;
		// Clear the previous window size specific context.
		ID3D11RenderTargetView* nullViews[] = { nullptr };
		this->d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
		this->d3dRenderTargetView = nullptr;
		this->d2dContext->SetTarget(nullptr);
		this->d2dTargetBitmap = nullptr;
		this->d3dContext->Flush();

		hr = this->displayTexture->Resize();

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET){
			throw DX11::Exception::DeviceLost();
		}
		else{
			H::ThrowIfFailed(hr);
		}
	}

}