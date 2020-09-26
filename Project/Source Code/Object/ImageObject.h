/*******************************************************************
* Author	: wwyang
* Date		: 2007.3.23
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : 
* Version   : 1.0
*********************************************************************/
// .NAME CImageObject - class for representing an image element in an 
// animation frame

// .SECTION Description
// This class is user for representing an image element in an animation frame.
// The image data is actually stored in the image object manager for 
// storage saving; thus, different image element may use a same image data copy.
// The path name of image uniquely identify an image

// .SECTION See also
// CElementObject

#ifndef _IMAGEOBJECT_H_
#define _IMAGEOBJECT_H_

#include <QImage>
#include <QString>

#include "ElementObject.h"

class CImageObject : public CElementObject{
public:
	CImageObject();
	virtual ~CImageObject();

	// Description:
	// New itself
	virtual Object *New();

	// Description:
	// Copy itself from another
	virtual void Copy(const Object *other); 

	// Description:
	// Associate an image data
	bool LoadImageFromFile(const QString &filePathName);
	bool ReLoadImageFromFile(const QString &filePathName);  // memory-efficient, re-use memory and texture

	// Description:
	// Get the image associated with the object
	QImage* GetImage();

	// Description:
	// Get the texture associated with the object
	int GetTextureBuddy();

	// Description:
	// Get the name of the file associated with the image
	QString GetImageFileName() const;
	//void UpdateImageFileName(const QString &filename);


private:
	// Refer to an image
	QString _imagePathName;
};

#endif