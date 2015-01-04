#include "pch.h"
#include "Renderer.h"
#include "DX11\DisplayTexture\DX11SwapChainPanelDisplayTexture.h"

#include <memory>

using namespace DX11::DisplayTexture;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace concurrency;

namespace RendererWinRT{

	Renderer::Renderer(){
	}

	Renderer::~Renderer(){
		this->coreInput->Dispatcher->StopProcessEvents();
	}

	void Renderer::Initialize(SwapChainPanel ^panel){
		this->InitVisualEventHandlers(panel);
		this->InitInputEventHandlers(panel);
		this->InitAppEventHandlers();

		this->dxDev = std::make_shared<DX11::DeviceResources>();

		DisplayInformation ^currentDisplayInformation = DisplayInformation::GetForCurrentView();
		std::unique_ptr<DisplayTexture> dispTex = std::unique_ptr<DisplayTexture>(
			new SwapChainPanelDisplayTexture(this->dxDev.get(), panel, currentDisplayInformation));

		this->dxDev->Initialize(std::move(dispTex));

		this->StartRenderLoop();
	}

	void Renderer::InitVisualEventHandlers(SwapChainPanel ^panel){
		auto window = Windows::UI::Xaml::Window::Current->CoreWindow;
		auto currentDisplayInformation = DisplayInformation::GetForCurrentView();

		window->VisibilityChanged +=
			ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &Renderer::OnVisibilityChanged);

		currentDisplayInformation->DpiChanged +=
			ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Renderer::OnDpiChanged);

		currentDisplayInformation->OrientationChanged +=
			ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Renderer::OnOrientationChanged);

		DisplayInformation::DisplayContentsInvalidated +=
			ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Renderer::OnDisplayContentsInvalidated);

		panel->CompositionScaleChanged +=
			ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &Renderer::OnCompositionScaleChanged);

		panel->SizeChanged +=
			ref new SizeChangedEventHandler(this, &Renderer::OnSwapChainPanelSizeChanged);
	}

	void Renderer::InitInputEventHandlers(SwapChainPanel ^panel){
		// Disable all pointer visual feedback for better performance when touching.
		auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
		pointerVisualizationSettings->IsContactFeedbackEnabled = false;
		pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;

		// Register our SwapChainPanel to get independent input pointer events
		auto workItemHandler = ref new WorkItemHandler([=](IAsyncAction ^)
		{
			// The CoreIndependentInputSource will raise pointer events for the specified device types on whichever thread it's created on.
			this->coreInput = panel->CreateCoreIndependentInputSource(
				Windows::UI::Core::CoreInputDeviceTypes::Mouse |
				Windows::UI::Core::CoreInputDeviceTypes::Touch |
				Windows::UI::Core::CoreInputDeviceTypes::Pen
				);

			// Register for pointer events, which will be raised on the background thread.
			this->coreInput->PointerPressed += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &Renderer::OnPointerPressed);
			this->coreInput->PointerMoved += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &Renderer::OnPointerMoved);
			this->coreInput->PointerReleased += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &Renderer::OnPointerReleased);

			// Begin processing input messages as they're delivered.
			this->coreInput->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
		});

		// Run task on a dedicated high priority background thread.
		this->inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
	}

	void Renderer::InitAppEventHandlers(){
		auto app = Windows::UI::Xaml::Application::Current;

		app->Suspending += ref new SuspendingEventHandler(this, &Renderer::OnSuspending);
		app->Resuming += ref new EventHandler<Object^>(this, &Renderer::OnResuming);
	}

	// Window event handlers.
	void Renderer::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args){
		if (args->Visible){
			this->StartRenderLoop();
		}
		else{
			this->StopRenderLoop();
		}
	}

	// Display event handlers.
	void Renderer::OnDpiChanged(DisplayInformation^ sender, Object^ args){
		critical_section::scoped_lock lock(this->criticalSection);
		this->dxDev->SetDpi(sender->LogicalDpi);
	}

	void Renderer::OnOrientationChanged(DisplayInformation^ sender, Object^ args){
		critical_section::scoped_lock lock(this->criticalSection);
		DX11::DisplayTexture::Orientation currentOrientation;

		SwapChainPanelDisplayTexture::ConvOrientations(sender->CurrentOrientation, currentOrientation);
		this->dxDev->SetCurrentOrientation(currentOrientation);
	}

	void Renderer::OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ args){
		critical_section::scoped_lock lock(this->criticalSection);
		this->dxDev->SetScale(DirectX::XMFLOAT2(sender->CompositionScaleX, sender->CompositionScaleY));
	}

	void Renderer::OnSwapChainPanelSizeChanged(Object^ sender, SizeChangedEventArgs^ e){
		critical_section::scoped_lock lock(this->criticalSection);
		this->dxDev->SetLogicalSize(DirectX::XMFLOAT2(e->NewSize.Width, e->NewSize.Height));
	}

	void Renderer::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args){
		critical_section::scoped_lock lock(this->criticalSection);
		this->dxDev->ValidateDevice();
	}

	// Independent input handling functions.
	void Renderer::OnPointerPressed(Object^ sender, PointerEventArgs^ e){
		
	}

	void Renderer::OnPointerMoved(Object^ sender, PointerEventArgs^ e){
		
	}

	void Renderer::OnPointerReleased(Object^ sender, PointerEventArgs^ e){

	}

	// App events
	void Renderer::OnSuspending(Object^ sender, SuspendingEventArgs^ e){
		critical_section::scoped_lock lock(this->criticalSection);
		this->dxDev->Trim();

		this->StopRenderLoop();
	}

	void Renderer::OnResuming(Object ^sender, Object ^args){
		this->StartRenderLoop();
	}

	void Renderer::StartRenderLoop(){
		// If the animation render loop is already running then do not start another thread.
		if (this->renderLoopWorker != nullptr 
			&& this->renderLoopWorker->Status == AsyncStatus::Started)
		{
			return;
		}

		// Create a task that will be run on a background thread.
		auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^ action){
			// Calculate the updated frame and render once per vertical blanking interval.
			while (action->Status == AsyncStatus::Started){
				critical_section::scoped_lock lock(this->criticalSection);
				/*Update();
				if (Render())
				{
					m_deviceResources->Present();
				}*/

				auto d3dCtx = this->dxDev->GetD3DContext();
				auto d3dRT = this->dxDev->GetD3DDisplayTarget();

				this->dxDev->SetD3DDisplayTarget();

				float clearColor[] = { 0.5f, 0.5f, 1.0f, 1.0f };

				d3dCtx->ClearRenderTargetView(d3dRT.Get(), clearColor);

				this->dxDev->Present();
			}
		});

		// Run task on a dedicated high priority background thread.
		this->renderLoopWorker = ThreadPool::RunAsync(
			workItemHandler, 
			WorkItemPriority::High, 
			WorkItemOptions::TimeSliced);

	}

	void Renderer::StopRenderLoop(){
		this->renderLoopWorker->Cancel();
	}

}