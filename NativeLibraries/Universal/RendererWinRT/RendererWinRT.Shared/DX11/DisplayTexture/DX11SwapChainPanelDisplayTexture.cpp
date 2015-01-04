#include "pch.h"
#include "DX11SwapChainPanelDisplayTexture.h"
#include "..\DX11DeviceResources.h"
#include "..\Helpers\DX11H.h"
#include "..\Exceptions\DX11Except.h"

#include <windows.ui.xaml.media.dxinterop.h>

using namespace Windows::Graphics::Display;

namespace DX11{
	namespace DisplayTexture{

		SwapChainPanelDisplayTexture::SwapChainPanelDisplayTexture(
			DeviceResources *devRes,
			Windows::UI::Xaml::Controls::SwapChainPanel ^swapChainPanel,
			Windows::Graphics::Display::DisplayInformation ^currentDisplayInformation)
			: DisplayTexture(devRes), swapChainPanel(swapChainPanel)
		{
			Orientation defaultOrientation, currentOrientation;

			SwapChainPanelDisplayTexture::ConvOrientations(currentDisplayInformation->NativeOrientation, defaultOrientation);
			SwapChainPanelDisplayTexture::ConvOrientations(currentDisplayInformation->CurrentOrientation, currentOrientation);

			this->SetDefaultOrientation(defaultOrientation);
			this->SetCurrentOrientation(currentOrientation);

			DirectX::XMFLOAT2 scale(this->swapChainPanel->CompositionScaleX, this->swapChainPanel->CompositionScaleY);
			DirectX::XMFLOAT2 logicalSize(static_cast<float>(this->swapChainPanel->ActualWidth), static_cast<float>(this->swapChainPanel->ActualHeight));

			this->SetLogicalDpi(currentDisplayInformation->LogicalDpi);
			this->SetScale(scale);
			this->SetLogicalSize(logicalSize);
		}

		SwapChainPanelDisplayTexture::~SwapChainPanelDisplayTexture(){
		}

		void SwapChainPanelDisplayTexture::GetBuffer(uint32_t index, const IID &iid, void **surface){
			H::ThrowIfFailed(this->swapChain->GetBuffer(0, iid, surface));
		}

		void SwapChainPanelDisplayTexture::Resize(){
			HRESULT hr = S_OK;
			auto displayRotation = this->ComputeSizeAndRotation();

			if (this->swapChain != nullptr){
				hr = this->ResizeSwapChain();
				SwapChainPanelDisplayTexture::CheckDeviceLost(hr);
			}
			else{
				this->CreateSwapChain();
			}

			this->SetFormat(SwapChainPanelDisplayTexture::BufferFormat);

			H::ThrowIfFailed(this->swapChain->SetRotation(displayRotation));

			this->ComputeInverseScale();
		}

		void SwapChainPanelDisplayTexture::Present(){
			// The first argument instructs DXGI to block until VSync, putting the application
			// to sleep until the next VSync. This ensures we don't waste any cycles rendering
			// frames that will never be displayed to the screen.
			HRESULT hr = this->swapChain->Present(1, 0);
			SwapChainPanelDisplayTexture::CheckDeviceLost(hr);
		}

		void SwapChainPanelDisplayTexture::ConvOrientations(DisplayOrientations from, Orientation &to){
			switch (from){
			case DisplayOrientations::Landscape:
				to = Orientation::Landscape;
				break;
			case DisplayOrientations::Portrait:
				to = Orientation::Portrait;
				break;
			case DisplayOrientations::LandscapeFlipped:
				to = Orientation::LandscapeFlipped;
				break;
			case DisplayOrientations::PortraitFlipped:
				to = Orientation::PortraitFlipped;
				break;
			case DisplayOrientations::None:
			default:
				to = Orientation::None;
				break;
			}
		}

		DXGI_MODE_ROTATION SwapChainPanelDisplayTexture::ComputeDisplayRotation(){
			DXGI_MODE_ROTATION rotationMode = DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_UNSPECIFIED;

			// Note: NativeOrientation can only be Landscape or Portrait even though
			// the DisplayOrientations enum has other values.
			switch (this->GetDefaultOrientation()){
			case Orientation::Landscape:
				switch (this->GetCurrentOrientation()){
				case Orientation::Landscape:
					rotationMode = DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_IDENTITY;
					break;
				case Orientation::Portrait:
					rotationMode = DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE270;
					break;
				case Orientation::LandscapeFlipped:
					rotationMode = DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE180;
					break;
				case Orientation::PortraitFlipped:
					rotationMode = DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE90;
					break;
				}
				break;
			case Orientation::Portrait:
				switch (this->GetCurrentOrientation()){
				case Orientation::Landscape:
					rotationMode = DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE90;
					break;
				case Orientation::Portrait:
					rotationMode = DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_IDENTITY;
					break;
				case Orientation::LandscapeFlipped:
					rotationMode = DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE270;
					break;
				case Orientation::PortraitFlipped:
					rotationMode = DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE180;
					break;
				}
				break;
			}

			return rotationMode;
		}

		DXGI_MODE_ROTATION SwapChainPanelDisplayTexture::ComputeSizeAndRotation(){
			D2D1::Matrix3x2F orientationTransform2D;
			DirectX::XMFLOAT4X4 orientationTransform3D;
			DirectX::XMFLOAT2 outputSize, renderTargetSize;

			outputSize = this->GetLogicalSize() * this->GetScale();
			outputSize = H::MathOp::Max(outputSize, 1.0f);

			auto displayRotation = this->ComputeDisplayRotation();
			auto logicalSize = this->GetLogicalSize();

			bool swapDimensions = displayRotation == DXGI_MODE_ROTATION_ROTATE90 ||
				displayRotation == DXGI_MODE_ROTATION_ROTATE270;
			renderTargetSize.x = swapDimensions ? outputSize.y : outputSize.x;
			renderTargetSize.y = swapDimensions ? outputSize.x : outputSize.y;

			this->SetOutputSize(outputSize);
			this->SetPhysicalSize(renderTargetSize);

			switch (displayRotation){
			case DXGI_MODE_ROTATION_IDENTITY:
				orientationTransform2D = D2D1::Matrix3x2F::Identity();
				orientationTransform3D = DisplayTexture::Rotation0;
				break;

			case DXGI_MODE_ROTATION_ROTATE90:
				orientationTransform2D =
					D2D1::Matrix3x2F::Rotation(90.0f) *
					D2D1::Matrix3x2F::Translation(logicalSize.y, 0.0f);
				orientationTransform3D = DisplayTexture::Rotation270;
				break;

			case DXGI_MODE_ROTATION_ROTATE180:
				orientationTransform2D =
					D2D1::Matrix3x2F::Rotation(180.0f) *
					D2D1::Matrix3x2F::Translation(logicalSize.x, logicalSize.y);
				orientationTransform3D = DisplayTexture::Rotation180;
				break;

			case DXGI_MODE_ROTATION_ROTATE270:
				orientationTransform2D =
					D2D1::Matrix3x2F::Rotation(270.0f) *
					D2D1::Matrix3x2F::Translation(0.0f, logicalSize.x);
				orientationTransform3D = DisplayTexture::Rotation90;
				break;

			default:
				H::ThrowIfFailed(E_FAIL);
			}

			return displayRotation;
		}

		HRESULT SwapChainPanelDisplayTexture::ResizeSwapChain(){
			auto renderTargetSize = this->GetPhysicalSize();
			HRESULT hr = this->swapChain->ResizeBuffers(
				SwapChainPanelDisplayTexture::BufferCount,
				lround(renderTargetSize.x),
				lround(renderTargetSize.y),
				SwapChainPanelDisplayTexture::BufferFormat,
				0);

			return hr;
		}

		void SwapChainPanelDisplayTexture::CreateSwapChain(){
			auto renderTargetSize = this->GetPhysicalSize();
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
			Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
			Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
			Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
			auto d3dDev = this->GetDeviceResources()->GetD3DDevice();

			swapChainDesc.Width = lround(renderTargetSize.x); // Match the size of the window.
			swapChainDesc.Height = lround(renderTargetSize.y);
			swapChainDesc.Format = SwapChainPanelDisplayTexture::BufferFormat; // This is the most common swap chain format.
			swapChainDesc.Stereo = false;
			swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = SwapChainPanelDisplayTexture::BufferCount;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
			swapChainDesc.Flags = 0;
			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

			H::ThrowIfFailed(d3dDev.As(&dxgiDevice));
			H::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));
			H::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

			H::ThrowIfFailed(dxgiFactory->CreateSwapChainForComposition(
				d3dDev.Get(),
				&swapChainDesc,
				nullptr,
				this->swapChain.ReleaseAndGetAddressOf()
				));

			H::ThrowIfFailed(dxgiDevice->SetMaximumFrameLatency(1));

			this->swapChainPanel->Dispatcher->RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::High,
				ref new Windows::UI::Core::DispatchedHandler([=]()
			{
				// Get backing native interface for SwapChainPanel
				Microsoft::WRL::ComPtr<ISwapChainPanelNative> panelNative;
				H::ThrowIfFailed(
					reinterpret_cast<IUnknown*>(this->swapChainPanel)->QueryInterface(
					IID_PPV_ARGS(&panelNative))
					);

				H::ThrowIfFailed(panelNative->SetSwapChain(this->swapChain.Get()));
			}, Platform::CallbackContext::Any));
		}

		void SwapChainPanelDisplayTexture::ComputeInverseScale(){
			DXGI_MATRIX_3X2_F inverseScale = { 0 };
			auto compositionScale = this->GetScale();
			Microsoft::WRL::ComPtr<IDXGISwapChain2> spSwapChain2;

			inverseScale._11 = 1.0f / compositionScale.x;
			inverseScale._22 = 1.0f / compositionScale.y;

			H::ThrowIfFailed(this->swapChain.As<IDXGISwapChain2>(&spSwapChain2));
			H::ThrowIfFailed(spSwapChain2->SetMatrixTransform(&inverseScale));
		}

		void SwapChainPanelDisplayTexture::CheckDeviceLost(HRESULT hr){
			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET){
				throw DX11::Exception::DeviceLost();
			}
			else{
				H::ThrowIfFailed(hr);
			}
		}

	}
}