#pragma once

#include <wrl.h>
#include <DirectXMath.h>

namespace DX11{

	class DeviceResources;

	namespace DisplayTexture{

		enum class Orientation{
			None,
			Landscape,
			Portrait,
			LandscapeFlipped,
			PortraitFlipped
		};

		class DisplayTexture{
		public:
			DisplayTexture(DeviceResources *devRes);
			virtual ~DisplayTexture();

			Orientation GetDefaultOrientation() const;
			void SetDefaultOrientation(Orientation v);

			Orientation GetCurrentOrientation() const;
			void SetCurrentOrientation(Orientation v);

			float GetLogicalDpi() const;
			void SetLogicalDpi(float v);

			const DirectX::XMFLOAT2 &GetScale() const;
			void SetScale(const DirectX::XMFLOAT2 &v);

			const DirectX::XMFLOAT2 &GetLogicalSize() const;
			void SetLogicalSize(const DirectX::XMFLOAT2 &v);

			const DirectX::XMFLOAT2 &GetPhysicalSize() const;

			DeviceResources *GetDeviceResources();

			virtual HRESULT Resize() = 0;
		protected:
			void SetPhysicalSize(const DirectX::XMFLOAT2 &v);
		private:
			Orientation defaultOrientation;
			Orientation currentOrientation;
			float logicalDpi;
			DirectX::XMFLOAT2 scale;
			DirectX::XMFLOAT2 logicalSize;
			DirectX::XMFLOAT2 physicalSize;
			DeviceResources *devRes;
		};

	}
}