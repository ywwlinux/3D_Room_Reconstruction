
#include "ImageObject.h"

#include "../Scene/ObjectManagerFactory.h"

CImageObject::CImageObject()
{
	_elementType = E_IMAGE;
}

CImageObject::~CImageObject()
{
	// Decrease the reference of the associated image in the image manager
	CObjectManagerFactory::GetImageEntryManager().DeleteImageObject(_imagePathName);

	// Decrease the reference of the associated texture in the texture manager
	CObjectManagerFactory::GetTextureManager().DeleteTextureObject(_imagePathName);
}

 Object *CImageObject::New()
{
	return new CImageObject;
}

void CImageObject::Copy(const Object *other)
{
	CElementObject::Copy(other);

	const CImageObject *pOtherImageObject = static_cast<const CImageObject *>(other);

	_imagePathName = pOtherImageObject->_imagePathName;

	LoadImageFromFile(_imagePathName);
}

bool CImageObject::LoadImageFromFile(const QString &filePathName)
{
	// Insert the image into the image enetry manager
	if(CObjectManagerFactory::GetImageEntryManager().InsertImageObject(filePathName)){
		_imagePathName = filePathName;

		// Create a texture for this image
		QImage *pImage = CObjectManagerFactory::GetImageEntryManager().GetImageObject(filePathName);
		CObjectManagerFactory::GetTextureManager().InsertTextureObject(pImage, filePathName);

		return true;
	}
	else{
		_imagePathName = QString();

		return false;
	}
}

bool CImageObject::ReLoadImageFromFile(const QString &filePathName)
{
	// Replace an image in the image enetry manager with a new image
	if (CObjectManagerFactory::GetImageEntryManager().ReInsertImageObject(_imagePathName, filePathName)) {
		// Create a texture for this image
		QImage *pImage = CObjectManagerFactory::GetImageEntryManager().GetImageObject(filePathName);
		CObjectManagerFactory::GetTextureManager().ReInsertTextureObject(pImage, _imagePathName, filePathName);

		_imagePathName = filePathName;

		return true;
	}
	else {
		_imagePathName = QString();

		return false;
	}
}

QImage* CImageObject::GetImage()
{
	return CObjectManagerFactory::GetImageEntryManager().GetImageObject(_imagePathName);
}

QString CImageObject::GetImageFileName() const
{
	return _imagePathName;
}

//void CImageObject::UpdateImageFileName(const QString &filename)
//{
//	_imagePathName = filename;
//}

int CImageObject::GetTextureBuddy()
{
	return CObjectManagerFactory::GetTextureManager().GetTextureID(_imagePathName);
}


