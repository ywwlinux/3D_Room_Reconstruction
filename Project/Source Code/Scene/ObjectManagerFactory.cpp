
#include "ObjectManagerFactory.h"

CObjectManagerFactory::CObjectManagerFactory()
{

}

CObjectManagerFactory::~CObjectManagerFactory()
{
	DestroyAllManagers();
}

void CObjectManagerFactory::DestroyAllManagers()
{
	_commandManager.DestroyManager();

	_imageEntryManager.DestroyManager();
	_textureManager.DestroyManager();
	_cameraManager.DestroyManager();
	_sceneObjectManager.DestroyManager();
}

// Initialize the static instance of object manager in the 
// factory
CCommandManager CObjectManagerFactory::_commandManager;

CImageEntryManager CObjectManagerFactory::_imageEntryManager;
CTextureManager CObjectManagerFactory::_textureManager;
CCameraManager CObjectManagerFactory::_cameraManager;
CSceneObjectManager CObjectManagerFactory::_sceneObjectManager;
