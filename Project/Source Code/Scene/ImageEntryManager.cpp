
#include "ImageEntryManager.h"

#include <QImageReader>

void CImageEntryManager::DestroyManager()
{
	std::list<CImageEntry>::iterator it = _listImage.begin();
	while(it != _listImage.end()){
		if((*it).pImageData != 0){
			delete (*it).pImageData;
		}
		it ++;
	}

	_listImage.clear();
}

bool CImageEntryManager::InsertImageObject(const QString &imagePathName)
{
	// Try to find the image in the list
	CImageEntry *pImageEntry = GetImageEntry(imagePathName);

	if(pImageEntry != 0){
		pImageEntry->refCount ++;

		return true;
	}
	
	// Try to load the image
	QImage *pImage = new QImage;

	if(!pImage->load(imagePathName)){
		delete pImage;

		return false;
	}
	
	// Insert the image
	CImageEntry newImageObject;
	
	newImageObject.pathName = imagePathName;
	newImageObject.pImageData = pImage;
	newImageObject.refCount = 1;
	
	_listImage.push_back(newImageObject);

	return true;
}

bool CImageEntryManager::ReInsertImageObject(const QString &oldImagePathName, const QString &imagePathName)
{
	CImageEntry *pImageEntry = GetImageEntry(oldImagePathName);

	if (pImageEntry == 0) {
		return InsertImageObject(imagePathName);
	}

	if (pImageEntry->refCount > 1) {
		pImageEntry->refCount--;

		return InsertImageObject(imagePathName);
	}

	// replace the image entry with the new image
	if (!pImageEntry->pImageData->load(imagePathName)) {
		return false;
	}

	pImageEntry->pathName = imagePathName;
	
	return true;
}

void CImageEntryManager::DeleteImageObject(const QString &imagePathName)
{
	std::list<CImageEntry>::iterator itImageObject = _listImage.begin();
	while(itImageObject != _listImage.end()){
		CImageEntry &imageObject = *itImageObject;
		
		if(imageObject.pathName == imagePathName){
			imageObject.refCount --;
			if(imageObject.refCount == 0){
				delete imageObject.pImageData;
				_listImage.erase(itImageObject);
			}

			return;
		}
	
		itImageObject ++;
	}
}

QImage* CImageEntryManager::GetImageObject(const QString &imagePathName)
{
	std::list<CImageEntry>::iterator itImageObject = _listImage.begin();
	while(itImageObject != _listImage.end()){
		CImageEntry &imageObject = *itImageObject;

		if(imageObject.pathName == imagePathName) {
			return imageObject.pImageData;
		}

		itImageObject ++;
	}

	return 0;
}

CImageEntryManager::CImageEntry* CImageEntryManager::GetImageEntry(const QString &imagePathName)
{
	std::list<CImageEntry>::iterator itImageObject = _listImage.begin();
	while(itImageObject != _listImage.end()){
		CImageEntry &imageObject = *itImageObject;

		if(imageObject.pathName == imagePathName) {
			return &imageObject;
		}

		itImageObject ++;
	}

	return 0;
}


