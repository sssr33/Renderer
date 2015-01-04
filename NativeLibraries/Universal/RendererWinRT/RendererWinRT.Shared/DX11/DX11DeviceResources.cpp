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

	Microsoft::WRL::ComPtr<ID3D11Device2> DeviceResources::GetD3DDevice() const{
		return this->d3dDevice;
	}

	Microsoft::WRL::ComPtr<ID3D11DeviceContext2> DeviceResources::GetD3DContext() const{
		return this->d3dContext;
	}

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> DeviceResources::GetD3DDisplayTarget() const{
		return this->d3dRenderTargetView;
	}

	const D3D11_VIEWPORT &DeviceResources::GetD3DDisplayViewport() const{
		return this->screenViewport;
	}

	D3D_FEATURE_LEVEL DeviceResources::GetD3DFeatureLevel() const{
		return this->d3dFeatureLevel;
	}

	Microsoft::WRL::ComPtr<ID2D1Device1> DeviceResources::GetD2DDevice() const{
		return this->d2dDevice;
	}

	Microsoft::WRL::ComPtr<ID2D1DeviceContext1> DeviceResources::GetD2DContext() const{
		return this->d2dContext;
	}

	Microsoft::WRL::ComPtr<ID2D1Bitmap1> DeviceResources::GetD2DDisplayTarget() const{
		return this->d2dTargetBitmap;
	}

	Microsoft::WRL::ComPtr<ID2D1Factory2> DeviceResources::GetD2DFactory() const{
		return this->d2dFactory;
	}

	Microsoft::WRL::ComPtr<IDWriteFactory2> DeviceResources::GetDWriteFactory() const{
		return this->dwriteFactory;
	}

	Microsoft::WRL::ComPtr<IWICImagingFactory2> DeviceResources::GetWICFactory() const{
		return this->wicFactory;
	}

	void DeviceResources::SetDpi(float v){
		if (v != this->displayTexture->GetLogicalDpi()){
			this->displayTexture->SetLogicalDpi(v);
			this->d2dContext->SetDpi(v, v);
			this->CreateSizeDependentResources();
		}
	}

	void DeviceResources::SetScale(const DirectX::XMFLOAT2 &v){
		if (v != this->displayTexture->GetScale()){
			this->displayTexture->SetScale(v);
			this->CreateSizeDependentResources();
		}
	}

	void DeviceResources::SetLogicalSize(const DirectX::XMFLOAT2 &v){
		if (v != this->displayTexture->GetLogicalSize()){
			this->displayTexture->SetLogicalSize(v);
			this->CreateSizeDependentResources();
		}
	}

	void DeviceResources::SetCurrentOrientation(DisplayTexture::Orientation v){
		if (v != this->displayTexture->GetCurrentOrientation()){
			this->displayTexture->SetCurrentOrientation(v);
			this->CreateSizeDependentResources();
		}
	}

	void DeviceResources::Initialize(std::unique_ptr<DisplayTexture::DisplayTexture> &&displayTexture){
		this->displayTexture = std::move(displayTexture);

		this->CreateDeviceIndependentResources();
		this->CreateDeviceResources();
		this->CreateSizeDependentResources();
	}

	void DeviceResources::SetD3DDisplayTarget(ID3D11DepthStencilView *depthStencil){
		this->d3dContext->OMSetRenderTargets(1, this->d3dRenderTargetView.GetAddressOf(), depthStencil);
		this->d3dContext->RSSetViewports(1, &this->screenViewport);
	}

	void DeviceResources::SetD2DDisplayTarget(){
		this->d2dContext->SetTarget(this->d2dTargetBitmap.Get());
	}

	void DeviceResources::Present(){
		this->displayTexture->Present();

		this->d3dContext->DiscardView(this->d3dRenderTargetView.Get());
	}

	void DeviceResources::Trim(){
		Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;

		this->d3dDevice.As(&dxgiDevice);
		dxgiDevice->Trim();
	}

	void DeviceResources::ValidateDevice(){
		// The D3D Device is no longer valid if the default adapter changed since the device
		// was created or if the device has been removed.
		DXGI_ADAPTER_DESC previousDesc, currentDesc;
		Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
		Microsoft::WRL::ComPtr<IDXGIAdapter> deviceAdapter;
		Microsoft::WRL::ComPtr<IDXGIFactory2> deviceFactory, currentFactory;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> previousDefaultAdapter, currentDefaultAdapter;

		// First, get the information for the default adapter from when the device was created.
		H::ThrowIfFailed(this->d3dDevice.As(&dxgiDevice));
		H::ThrowIfFailed(dxgiDevice->GetAdapter(&deviceAdapter));
		H::ThrowIfFailed(deviceAdapter->GetParent(IID_PPV_ARGS(&deviceFactory)));
		H::ThrowIfFailed(deviceFactory->EnumAdapters1(0, &previousDefaultAdapter));
		H::ThrowIfFailed(previousDefaultAdapter->GetDesc(&previousDesc));

		// Next, get the information for the current default adapter.
		H::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));
		H::ThrowIfFailed(currentFactory->EnumAdapters1(0, &currentDefaultAdapter));
		H::ThrowIfFailed(currentDefaultAdapter->GetDesc(&currentDesc));

		// If the adapter LUIDs don't match, or if the device reports that it has been removed,
		// a new D3D device must be created.
		if (previousDesc.AdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
			previousDesc.AdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart ||
			FAILED(this->d3dDevice->GetDeviceRemovedReason()))
		{
			// Release references to resources related to the old device.
			dxgiDevice = nullptr;
			deviceAdapter = nullptr;
			deviceFactory = nullptr;
			previousDefaultAdapter = nullptr;

			throw DX11::Exception::DeviceLost();
		}
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

		this->displayTexture->Resize();

		this->CreateRenderTargets();
	}

	void DeviceResources::CreateRenderTargets(){
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		Microsoft::WRL::ComPtr<IDXGISurface2> dxgiBackBuffer;
		auto renderTargetSize = this->displayTexture->GetPhysicalSize();
		D2D1_BITMAP_PROPERTIES1 bitmapProperties =
			D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(this->displayTexture->GetFormat(), D2D1_ALPHA_MODE_PREMULTIPLIED),
			this->displayTexture->GetLogicalDpi(),
			this->displayTexture->GetLogicalDpi()
			);

		this->displayTexture->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
		this->displayTexture->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));

		H::ThrowIfFailed(this->d3dDevice->CreateRenderTargetView(
			backBuffer.Get(), 
			nullptr, 
			this->d3dRenderTargetView.ReleaseAndGetAddressOf()
			));

		H::ThrowIfFailed(this->d2dContext->CreateBitmapFromDxgiSurface(
			dxgiBackBuffer.Get(), 
			&bitmapProperties, 
			this->d2dTargetBitmap.ReleaseAndGetAddressOf()
			));

		this->screenViewport = CD3D11_VIEWPORT(
			0.0f,
			0.0f,
			renderTargetSize.x,
			renderTargetSize.y
			);
	}

}