#include "pch.h"
#include "Renderer.h"

using namespace Windows::UI::Xaml::Controls;

namespace RendererWinRT{

	Renderer::Renderer(){
	}

	Renderer::~Renderer(){
	}

	void Renderer::Initialize(SwapChainPanel ^panel){
		auto window = Windows::UI::Xaml::Window::Current->CoreWindow;
		auto app = Windows::UI::Xaml::Application::Current;
	}

}