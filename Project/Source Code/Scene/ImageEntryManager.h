/*******************************************************************
* Author	: wwyang
* Date		: 2007.8.17
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : 
* Version   : 1.0
*********************************************************************/
// .NAME ImageEntryManager - Manager of images in this application

// .SECTION Description
// This class is used to manage images in the application. In this manager, 
// the pixel data of all image that used in the application is stored here.
// Then, the image here will be used in the other places by referring to the 
// image id, and the image reference will increase accordingly.
// Note: the path name of the image identifies an image; thus, an image will
// only have one copy

// .SECTION See also
// CObjectManager CObjectManagerFactory

#ifndef _IMAGEENTRYMANAGER_H_
#define _IMAGEENTRYMANAGER_H_

#include <QImage>
#include <QString>
#include <list>

#include "ObjectManager.h"

class CImageEntryManager : public CObjectManager{
public:
	class CImageEntry{
	public:
		QString pathName;
		QImage *pImageData;
		int refCount;

		CImageEntry()
		{
			pImageData = 0;
			refCount = 0;
		}
	};

	CImageEntryManager(){ }
	~CImageEntryManager()
	{
		DestroyManager();
	}
	
	// Description:
	// Insert image object
	bool InsertImageObject(const QString &imagePathName);
	bool ReInsertImageObject(const QString &oldImagePathName, const QString &imagePathName);

	// Description:
	// Delete Image object
	void DeleteImageObject(const QString &imagePathName);

	// Description:
	// Retrieve image object
	QImage* GetImageObject(const QString &imagePathName);

	// Description:
	// Destroy the manager -- free all memory
	void DestroyManager();

protected:
	CImageEntry* GetImageEntry(const QString &imagePathName);

private:
	// Image objects
	std::list<CImageEntry> _listImage;
};

#endif