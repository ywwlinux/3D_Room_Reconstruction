/*******************************************************************
* Author	: wwyang
* Date		: 2007.3.22
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : 
* Version   : 1.0
*********************************************************************/
// .NAME ObjectManagerFactory - a factory class to create instance of 
// object manager

// .SECTION Description
// This class is used to store and access the instance of object manager
// It only provide functions to access the manager instances, and will 
// not be instanced.
// Each object manager should have an instance in the class, and have 
// an function to access it.

// .SECTION See also
// CObjectManager CCommandManager

#ifndef _OBJECTMANAGERFACTORY_H_
#define _OBJECTMANAGERFACTORY_H_

#include "../ui/CommandManager.h" // For manager of command
#include "ImageEntryManager.h" // For manager of Image object
#include "TextureManager.h" // For manager of texture object
#include "CameraManager.h" // For manager of cameras
#include "SceneObjectManager.h" // For manager of all kinds geometry objects

class CObjectManagerFactory{
private:
	// Description:
	// Constructor and destructor
	CObjectManagerFactory();
	~CObjectManagerFactory();

public:
	// Description:
	// Get the manager of commands
	static CCommandManager& GetCommandManager()
	{
		return _commandManager;
	}

	static CImageEntryManager& GetImageEntryManager()
	{
		return _imageEntryManager;
	}
	static CTextureManager& GetTextureManager()
	{
		return _textureManager;
	}
	static CCameraManager& GetCameraManager()
	{
		return _cameraManager;
	}
	static CSceneObjectManager& GetSceneObjectManager()
	{
		return _sceneObjectManager;
	}

	// Description:
	// Free all managers in the factory
	static void DestroyAllManagers();

private:
	// Instance of command manager
	static CCommandManager _commandManager;

	static CImageEntryManager _imageEntryManager;
	static CTextureManager _textureManager;
	static CCameraManager _cameraManager;
	static CSceneObjectManager _sceneObjectManager;
	
};

#endif