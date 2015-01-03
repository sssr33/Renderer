#include "pch.h"
#include "DX11SwapChainPanelDisplayTexture.h"
#include "..\DX11DeviceResources.h"

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

		HRESULT SwapChainPanelDisplayTexture::Resize(){
			HRESULT hr = S_OK;
			return hr;
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

	}
}