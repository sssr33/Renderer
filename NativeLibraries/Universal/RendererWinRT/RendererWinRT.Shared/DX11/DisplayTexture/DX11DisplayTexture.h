#pragma once

#include <wrl.h>
#include <DirectXMath.h>
#include <d2d1_2.h>

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
			// the 3D matrix is specified explicitly to avoid rounding errors.
			static const DirectX::XMFLOAT4X4 Rotation0;
			static const DirectX::XMFLOAT4X4 Rotation90;
			static const DirectX::XMFLOAT4X4 Rotation180;
			static const DirectX::XMFLOAT4X4 Rotation270;

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

			const DirectX::XMFLOAT2 &GetOutputSize() const;
			const DirectX::XMFLOAT2 &GetPhysicalSize() const;
			const D2D1::Matrix3x2F &GetOrientationTransform2D() const;
			const DirectX::XMFLOAT4X4 &GetOrientationTransform3D() const;
			DXGI_FORMAT GetFormat() const;

			DeviceResources *GetDeviceResources();

			virtual void GetBuffer(uint32_t index, const IID &iid, void **surface) = 0;
			virtual void Resize() = 0;
			virtual void Present() = 0;
		protected:
			void SetOutputSize(const DirectX::XMFLOAT2 &v);
			void SetPhysicalSize(const DirectX::XMFLOAT2 &v);
			void SetOrientationTransform2D(const D2D1::Matrix3x2F &v);
			void SetOrientationTransform3D(const DirectX::XMFLOAT4X4 &v);
			void SetFormat(DXGI_FORMAT v);
		private:
			Orientation defaultOrientation;
			Orientation currentOrientation;
			float logicalDpi;
			DirectX::XMFLOAT2 scale;
			DirectX::XMFLOAT2 logicalSize;
			DirectX::XMFLOAT2 outputSize; // scaled logical size
			DirectX::XMFLOAT2 physicalSize; // scaled and rotated logical size
			D2D1::Matrix3x2F orientationTransform2D;
			DirectX::XMFLOAT4X4 orientationTransform3D;
			DXGI_FORMAT textureFormat;
			DeviceResources *devRes;
		};

	}
}