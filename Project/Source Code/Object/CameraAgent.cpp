#include "CameraAgent.h"

CCameraAgent::CCameraAgent()
{
	_pCameraPhoto = 0;
}

CCameraAgent::~CCameraAgent()
{
	if (_pCameraPhoto != 0) {
		delete _pCameraPhoto;
		_pCameraPhoto = 0;
	}
}

Object* CCameraAgent::New()
{
	return new CCameraAgent;
}

void CCameraAgent::SetIntrinsicParameter(const CCameraAgent::S_CameraIntrinsicParameter &intrinsicParameter)
{
	_intrinsicParameter = intrinsicParameter;
}

const CCameraAgent::S_CameraIntrinsicParameter& CCameraAgent::GetIntrinsicParameter() const
{
	return _intrinsicParameter;
}

void CCameraAgent::SetExtrinsicParameter(const S_CameraExtrinsicParameter &extrinsicParam)
{
	_extrinsicParameter = extrinsicParam;
}

const CCameraAgent::S_CameraExtrinsicParameter& CCameraAgent::GetExtrinsicParameter() const
{
	return _extrinsicParameter;
}

void CCameraAgent::SetCheckboardExtrinsicParameter(const std::vector<CCameraAgent::S_CameraCheckboardExtrinsicParameter>& arrayExtrinsicParam)
{
	_arrayCheckboardExtrinsicParameter = arrayExtrinsicParam;
}

const std::vector<CCameraAgent::S_CameraCheckboardExtrinsicParameter>& CCameraAgent::GetCheckboardExtrinsicParameter() const
{
	return _arrayCheckboardExtrinsicParameter;
}

void CCameraAgent::SetPhotoImage(CImageObject *pBkImg)
{
	if (_pCameraPhoto != 0) {
		delete _pCameraPhoto;
	}

	_pCameraPhoto = pBkImg;
}

const CImageObject* CCameraAgent::GetPhotoImage() const
{
	return _pCameraPhoto;
}

CImageObject* CCameraAgent::GetPhotoImage() 
{
	return _pCameraPhoto;
}