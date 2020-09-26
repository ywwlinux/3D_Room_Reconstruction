#include "SceneObjectManager.h"

CSceneObjectManager::CSceneObjectManager()
{
	
}

CSceneObjectManager::~CSceneObjectManager()
{
	DestroyManager();
}

void CSceneObjectManager::Set3DModel(CMesh *pModel)
{
	if (_p3DModel != 0)
		delete _p3DModel;

	_p3DModel = pModel;
}

CMesh* CSceneObjectManager::Get3DModel()
{
	return _p3DModel;
}

void CSceneObjectManager::SetDataDir(const QString &dir)
{
	_dataDir = dir;
}

const QString& CSceneObjectManager::GetDataDir() const
{
	return _dataDir;
}

void CSceneObjectManager::SetPhotoDir(const QString &dir)
{
	_photoDir = dir;
}

const QString& CSceneObjectManager::GetPhotoDir() const
{
	return _photoDir;
}

void CSceneObjectManager::SetModelPath(const QString &modelPath)
{
	_modelPath = modelPath;
}

const QString& CSceneObjectManager::GetModelPath() const
{
	return _modelPath;
}

void CSceneObjectManager::DestroyManager()
{
	if (_p3DModel != 0) {
		delete _p3DModel;
		_p3DModel = 0;
	}
}