/*******************************************************************
* Author	: wwyang
* Date		: 2017.11.23
* Copyright : Zhejiang Gongshang University
* Head File :
* Version   : 1.0
*********************************************************************/
// .NAME CSceneObjectManager - a class to manage all kinds of geometric objects

// .SECTION Description
// This class is used to manage all kinds of geometric objects 

// .SECTION See also
// CObjectManagerFactory

#ifndef _SCENEOBJECTMANAGER_H_
#define _SCENEOBJECTMANAGER_H_

#include "ObjectManager.h"
#include "../Base/Mesh.h"

#include <QString>

class CSceneObjectManager : public CObjectManager {
public:
	CSceneObjectManager();
	~CSceneObjectManager();

	void Set3DModel(CMesh *pModel);
	CMesh* Get3DModel();

	void SetDataDir(const QString &dir);
	const QString& GetDataDir() const;

	void SetPhotoDir(const QString &dir);
	const QString& GetPhotoDir() const;

	void SetModelPath(const QString &modelPath);
	const QString& GetModelPath() const;

	void DestroyManager();

private:

	CMesh *_p3DModel;

	QString _dataDir, _photoDir, _modelPath;
};

#endif
