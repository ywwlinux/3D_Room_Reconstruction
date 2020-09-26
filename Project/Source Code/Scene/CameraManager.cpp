#include "CameraManager.h"

CCameraManager::CCameraManager()
{

}

CCameraManager::~CCameraManager()
{
	DestroyManager();
}

void CCameraManager::InsertACamera(CCameraAgent *pCamera)
{
	_listCamera.push_back(pCamera);
}

const CCameraAgent* CCameraManager::GetACamera(int iCameraID) const
{
	std::list<CCameraAgent *>::const_iterator itCamera = _listCamera.begin();
	while (itCamera != _listCamera.end()) {
		const CCameraAgent* pAgent = *itCamera;

		if (pAgent->GetID() == iCameraID)
			return pAgent;

		itCamera++;
	}

	return 0;
}

CCameraAgent* CCameraManager::GetACamera(int iCameraID)
{
	std::list<CCameraAgent *>::iterator itCamera = _listCamera.begin();
	while (itCamera != _listCamera.end()) {
		CCameraAgent* pAgent = *itCamera;

		if (pAgent->GetID() == iCameraID)
			return pAgent;

		itCamera++;
	}

	return 0;
}

CCameraAgent* CCameraManager::GetACamera(const std::string &cameraName) 
{
	std::list<CCameraAgent *>::iterator itCamera = _listCamera.begin();
	while (itCamera != _listCamera.end()) {
		CCameraAgent* pAgent = *itCamera;

		if (std::strcmp( cameraName.c_str(), pAgent->GetName().c_str() ) == 0)
			return pAgent;

		itCamera++;
	}

	return 0;
}

const CCameraAgent* CCameraManager::GetACamera(const std::string &cameraName) const
{
	std::list<CCameraAgent *>::const_iterator itCamera = _listCamera.begin();
	while (itCamera != _listCamera.end()) {
		const CCameraAgent* pAgent = *itCamera;

		if (std::strcmp(cameraName.c_str(), pAgent->GetName().c_str()) == 0)
			return pAgent;

		itCamera++;
	}

	return 0;
}

void CCameraManager::DeleteACamera(int iCameraID)
{
	std::list<CCameraAgent *>::iterator itCamera = _listCamera.begin();
	while (itCamera != _listCamera.end()) {
		CCameraAgent* pAgent = *itCamera;

		if (pAgent->GetID() == iCameraID) { // delete the camera and remove it from the list
			_listCamera.erase(itCamera);
			delete pAgent;

			return;
		}

		itCamera++;
	}

	return;
}

const std::list<CCameraAgent *>& CCameraManager::GetCameras() const
{
	return _listCamera;
}

void CCameraManager::DeleteACamera(const std::string &cameraName)
{
	std::list<CCameraAgent *>::iterator itCamera = _listCamera.begin();
	while (itCamera != _listCamera.end()) {
		CCameraAgent* pAgent = *itCamera;

		if (std::strcmp(cameraName.c_str(), pAgent->GetName().c_str()) == 0) {
			_listCamera.erase(itCamera);
			delete pAgent;

			return;
		}

		itCamera++;
	}

	return;
}

void CCameraManager::DestroyManager()
{
	std::list<CCameraAgent *>::iterator itCamera = _listCamera.begin();
	while (itCamera != _listCamera.end()) {
		CCameraAgent* pAgent = *itCamera;

		delete pAgent;

		itCamera++;
	}

	_listCamera.clear();

	return;
}