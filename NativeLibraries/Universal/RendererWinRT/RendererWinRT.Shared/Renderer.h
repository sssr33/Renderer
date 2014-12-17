#pragma once

namespace RendererWinRT{

	[Windows::Foundation::Metadata::WebHostHiddenAttribute]
	public ref class Renderer sealed{
	public:
		Renderer();
		virtual ~Renderer();

		void Initialize(Windows::UI::Xaml::Controls::SwapChainPanel ^panel);
	};

}