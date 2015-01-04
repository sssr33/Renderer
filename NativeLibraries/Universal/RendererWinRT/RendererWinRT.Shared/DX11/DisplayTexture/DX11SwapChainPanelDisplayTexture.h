#pragma once
#include "DX11DisplayTexture.h"

#include <dxgi1_3.h>

namespace DX11{
	namespace DisplayTexture{

		class SwapChainPanelDisplayTexture : public DisplayTexture{
			static const uint32_t BufferCount = 2; // Double-buffered swap chain to minimize latency.
			static const DXGI_FORMAT BufferFormat = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
		public:
			SwapChainPanelDisplayTexture(
				DeviceResources *devRes,
				Windows::UI::Xaml::Controls::SwapChainPanel ^swapChainPanel, 
				Windows::Graphics::Display::DisplayInformation ^currentDisplayInformation);

			virtual ~SwapChainPanelDisplayTexture();

			// Can throw DX11::Exception::HresultException
			virtual void GetBuffer(uint32_t index, const IID &iid, void **surface) override;

			// Can throw DX11::Exception::DeviceLost, DX11::Exception::HresultException
			virtual void Resize() override;
			virtual void Present() override;

			static void ConvOrientations(Windows::Graphics::Display::DisplayOrientations from, Orientation &to);
		private:
			// Cached reference to the XAML panel.
			Windows::UI::Xaml::Controls::SwapChainPanel ^swapChainPanel;
			Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;

			DXGI_MODE_ROTATION ComputeDisplayRotation();
			DXGI_MODE_ROTATION ComputeSizeAndRotation();
			HRESULT ResizeSwapChain();
			void CreateSwapChain();
			void ComputeInverseScale();
			
			static void CheckDeviceLost(HRESULT hr);
		};

	}
}