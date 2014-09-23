#include "pch.h"
#include "RendererProxy.h"
#include "..\Helpers\H.h"
#include "D3D\InitParams\D3DSwapChainPanelInitParams.h"

#include <ppltasks.h>
#include <directxmath.h>

RendererProxy::RendererProxy()
{
}

RendererProxy::~RendererProxy()
{
}

void RendererProxy::Initialize(std::map<std::string, std::shared_ptr<void>> params)
{
	concurrency::create_task(
		[=]()
	{
		this->InitializeInternal(params);
	});
}

void RendererProxy::InitializeInternal(std::map<std::string, std::shared_ptr<void>> params)
{
	auto finded = params.find(D3DSwapChainPanelInitParams::ParameterKey);

	if (finded != params.end())
	{
		float dpi;
		DirectX::XMFLOAT2 logicalSize, compositionScale;
		Windows::Graphics::Display::DisplayOrientations currentOrientation, nativeOrientation;
		std::shared_ptr<D3DSwapChainPanelInitParams> p = std::static_pointer_cast<D3DSwapChainPanelInitParams>(finded->second);

		auto swapChainPanel = p->GetPanel();

		H::Task::RunSync(
			[&]()
		{
			Windows::Graphics::Display::DisplayInformation^ currentDisplayInformation = Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

			logicalSize.x = static_cast<float>(swapChainPanel->ActualWidth);
			logicalSize.y = static_cast<float>(swapChainPanel->ActualHeight);

			compositionScale.x = swapChainPanel->CompositionScaleX;
			compositionScale.y = swapChainPanel->CompositionScaleY;

			dpi = currentDisplayInformation->LogicalDpi;

			currentOrientation = currentDisplayInformation->CurrentOrientation;
			nativeOrientation = currentDisplayInformation->NativeOrientation;

		}, swapChainPanel->Dispatcher);

		int stop = 324;
	}
}