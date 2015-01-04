#include "pch.h"
#include "DX11DisplayTexture.h"

namespace DX11{
	namespace DisplayTexture{

		const DirectX::XMFLOAT4X4 DisplayTexture::Rotation0 = DirectX::XMFLOAT4X4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);

		const DirectX::XMFLOAT4X4 DisplayTexture::Rotation90 = DirectX::XMFLOAT4X4(
			0.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);

		const DirectX::XMFLOAT4X4 DisplayTexture::Rotation180 = DirectX::XMFLOAT4X4(
			-1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);

		const DirectX::XMFLOAT4X4 DisplayTexture::Rotation270 = DirectX::XMFLOAT4X4(
			0.0f, -1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);

		DisplayTexture::DisplayTexture(DeviceResources *devRes)
			: defaultOrientation(Orientation::None), currentOrientation(Orientation::None),
			logicalDpi(0.0f), scale(0.0f, 0.0f), logicalSize(0.0f, 0.0f),
			outputSize(0.0f, 0.0f), physicalSize(0.0f, 0.0f), devRes(devRes),
			orientationTransform2D(D2D1::Matrix3x2F::Identity()),
			orientationTransform3D(DisplayTexture::Rotation0),
			textureFormat(DXGI_FORMAT::DXGI_FORMAT_UNKNOWN){
		}

		DisplayTexture::~DisplayTexture(){
		}

		Orientation DisplayTexture::GetDefaultOrientation() const{
			return this->defaultOrientation;
		}

		void DisplayTexture::SetDefaultOrientation(Orientation v){
			this->defaultOrientation = v;
		}

		Orientation DisplayTexture::GetCurrentOrientation() const{
			return this->currentOrientation;
		}

		void DisplayTexture::SetCurrentOrientation(Orientation v){
			this->currentOrientation = v;
		}

		float DisplayTexture::GetLogicalDpi() const{
			return this->logicalDpi;
		}

		void DisplayTexture::SetLogicalDpi(float v){
			this->logicalDpi = v;
		}

		const DirectX::XMFLOAT2 &DisplayTexture::GetScale() const{
			return this->scale;
		}

		void DisplayTexture::SetScale(const DirectX::XMFLOAT2 &v){
			this->scale = v;
		}

		const DirectX::XMFLOAT2 &DisplayTexture::GetLogicalSize() const{
			return this->logicalSize;
		}

		void DisplayTexture::SetLogicalSize(const DirectX::XMFLOAT2 &v){
			this->logicalSize = v;
		}

		const DirectX::XMFLOAT2 &DisplayTexture::GetOutputSize() const{
			return this->outputSize;
		}

		const DirectX::XMFLOAT2 &DisplayTexture::GetPhysicalSize() const{
			return this->physicalSize;
		}

		const D2D1::Matrix3x2F &DisplayTexture::GetOrientationTransform2D() const{
			return this->orientationTransform2D;
		}

		const DirectX::XMFLOAT4X4 &DisplayTexture::GetOrientationTransform3D() const{
			return this->orientationTransform3D;
		}

		DXGI_FORMAT DisplayTexture::GetFormat() const{
			return this->textureFormat;
		}

		DeviceResources *DisplayTexture::GetDeviceResources(){
			return this->devRes;
		}

		void DisplayTexture::SetOutputSize(const DirectX::XMFLOAT2 &v){
			this->outputSize = v;
		}

		void DisplayTexture::SetPhysicalSize(const DirectX::XMFLOAT2 &v){
			this->physicalSize = v;
		}

		void DisplayTexture::SetOrientationTransform2D(const D2D1::Matrix3x2F &v){
			this->orientationTransform2D = v;
		}

		void DisplayTexture::SetOrientationTransform3D(const DirectX::XMFLOAT4X4 &v){
			this->orientationTransform3D = v;
		}

		void DisplayTexture::SetFormat(DXGI_FORMAT v){
			this->textureFormat = v;
		}

	}
}