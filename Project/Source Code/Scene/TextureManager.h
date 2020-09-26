/*******************************************************************
* Author	: wwyang
* Date		: 2007.8.17
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : 
* Version   : 1.0
*********************************************************************/
// .NAME TextureManager - Manager of textures in this application

// .SECTION Description
// This class is used to manage textures in the application. 
// This class takes the responsibility of loading image data to the video memory.
// It ensures that only one texture is created for the same image file, and that
// the video memory of a texture will be free if it is not used.

// .SECTION See also
// CObjectManager CObjectManagerFactory

#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include <QImage>
#include <QString>
#include <list>
#include <QGLWidget>

#include "ObjectManager.h"

class CTextureManager : public CObjectManager{
public:
	class CTextureEntry{
	public:
		QString pathName;
		int iTextureID;
		int refCount;

		CTextureEntry()
		{
			iTextureID = -1;
			refCount = 0;
		}
	};

	CTextureManager()
	{
		_pMainGLWidget = 0;
	}
	~CTextureManager()
	{
		DestroyManager();
	}

	// Description:
	// Set the main gl widget, where the textures will be created
	void SetMainGLWidget(QGLWidget *pMainGLWidget);

	// Description:
	// Insert texture object: return texture ID
	int InsertTextureObject(QImage *pTextureData, const QString &imageDataFileName);
	int ReInsertTextureObject(QImage *pTextureData, const QString &oldImageDataFileName, const QString &imageDataFileName);

	// Description:
	// Delete texture object
	void DeleteTextureObject(const QString &imageDataFileName);

	// Description:
	// Retrieve texture 
	int GetTextureID(const QString &imageDataFileName);

	// Description:
	// Destroy the manager -- free all video memory
	void DestroyManager();

private:
	// Image objects
	std::list<CTextureEntry> _listTexture;
	QGLWidget *_pMainGLWidget;
};

#endif