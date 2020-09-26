/*******************************************************************
* Author	: wwyang
* Date		: 2017.11.23
* Copyright : Zhejiang Gongshang University
* Head File :
* Version   : 1.0
*********************************************************************/
// .NAME CCameraManager - a class to manage all cameras

// .SECTION Description
// This class is used to manage the camera objects. In the manager, each camera is assumed to have a unique ID or name.
// Then, through the ID or name, we can find the desired camera.

// .SECTION See also
// CObjectManagerFactory

#ifndef _CAMERAMANAGER_H_
#define _CAMERAMANAGER_H_

#include "ObjectManager.h"
#include "../Object/CameraAgent.h"
#include <list>

class CCameraManager : public CObjectManager {
public:
	CCameraManager();
	~CCameraManager();

	// Description:
	// Insert a camera
	void InsertACamera(CCameraAgent *pCamera);

	// Description:
	// Get a camera agent using its ID
	const CCameraAgent* GetACamera(int iCameraID) const;
	CCameraAgent* GetACamera(int iCameraID);

	// Description:
	// Get a camera agent using its name
	const CCameraAgent* GetACamera(const std::string &cameraName) const;
	CCameraAgent* GetACamera(const std::string &cameraName);

	// Description:
	// Get all cameras
	const std::list<CCameraAgent *> &GetCameras() const;

	// Description:
	// Delete a specified camera agent
	void DeleteACamera(int iCameraID);
	void DeleteACamera(const std::string &cameraName);

	// Description:
	// Destroy the manager, i.e., deleting all of the cameras
	void DestroyManager();

private:
	std::list<CCameraAgent *> _listCamera;
};


#endif
