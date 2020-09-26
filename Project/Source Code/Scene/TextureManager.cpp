#include <QtOpenGL>

#include "TextureManager.h"
#include "assert.h"

//#include "gl\gl.h"

void CTextureManager::SetMainGLWidget(QGLWidget *pMainGLWidget)
{
	_pMainGLWidget = pMainGLWidget;
}

void CTextureManager::DestroyManager()
{
	if(_pMainGLWidget == 0){
		return;
	}

	// Free all video memory of the textures
	// Very important to check if size > 0, because the widget is destroyed before CTextureManager instance
	// However, the texture manager will be empty before the widget is destroyed.
	if(_listTexture.size() > 0 && _pMainGLWidget != 0 && _pMainGLWidget->isValid()){ // Use the context of this widget to delete textures, because all the textures are shared by this widget
		_pMainGLWidget->makeCurrent();
		
		std::list<CTextureEntry>::iterator it = _listTexture.begin();
		while(it != _listTexture.end()){
			CTextureEntry &textureEntry = *it;

			GLuint id[1] = {textureEntry.iTextureID};
			glDeleteTextures(1, id);

			it ++;
		}
	}
	
	_listTexture.clear();
}

int CTextureManager::InsertTextureObject(QImage *pTextureData, const QString &imageDataFileName)
{
	// Try to find the texture in the list
	std::list<CTextureEntry>::iterator itTextureEntry = _listTexture.begin();
	CTextureEntry *pTextureEntry = 0;
	int iD = -1;
	while(itTextureEntry != _listTexture.end()){
		pTextureEntry = &(*itTextureEntry);

		if(pTextureEntry->pathName== imageDataFileName){
			iD = pTextureEntry->iTextureID;

			break;
		}

		itTextureEntry ++;
	}

	if(iD >= 0){
		pTextureEntry->refCount ++;
		return iD;
	}

	// Create a texture for this image data
	if(_pMainGLWidget == 0){
		return -1;
	}
	_pMainGLWidget->makeCurrent();

	// Get a texture ID
	GLuint iTextureID;
	glGenTextures(1, &iTextureID);
	assert(iTextureID >= 0);

	// select current texture
	glBindTexture( GL_TEXTURE_2D, iTextureID);

	// select modulate to mix texture with color for shading
	//glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	
	// select modulate to mix texture without color for shading
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

	// when texture area is small, bilinear filter the closest mipmap
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// when texture area is large, bilinear filter the original
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR  );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// the texture clamps at the edges (not repeat)
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// build our texture -- upload image data to video memory
	QImage upTextureData;
	upTextureData = QGLWidget::convertToGLFormat( *pTextureData );

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, upTextureData.width(), upTextureData.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, upTextureData.bits());

	// Store this texture enetry
	CTextureEntry textureEntry;

	textureEntry.iTextureID = iTextureID;
	textureEntry.pathName = imageDataFileName;
	textureEntry.refCount = 1;

	_listTexture.push_back(textureEntry);

	//_pMainGLWidget->doneCurrent();

	return iTextureID;
}

int CTextureManager::ReInsertTextureObject(QImage *pTextureData, const QString &oldImageDataFileName, const QString &imageDataFileName)
{
	DeleteTextureObject(oldImageDataFileName);
	return InsertTextureObject(pTextureData, imageDataFileName);
}

void CTextureManager::DeleteTextureObject(const QString &imageDataFileName)
{
	std::list<CTextureEntry>::iterator itTextureEntry = _listTexture.begin();
	while(itTextureEntry != _listTexture.end()){
		CTextureEntry &textureEntry = *itTextureEntry;

		if(textureEntry.pathName== imageDataFileName){
			textureEntry.refCount --;
			if(textureEntry.refCount == 0){
				if(_pMainGLWidget != 0){
					_pMainGLWidget->makeCurrent();

					GLuint id[1] = {textureEntry.iTextureID};
					glDeleteTextures(1, id);
				}
				
				_listTexture.erase(itTextureEntry);
			}

			return;
		}

		itTextureEntry ++;
	}
}

int CTextureManager::GetTextureID(const QString &imageDataFileName)
{
	std::list<CTextureEntry>::iterator itTextureEntry = _listTexture.begin();
	while(itTextureEntry != _listTexture.end()){
		CTextureEntry &textureEntry = *itTextureEntry;

		if(textureEntry.pathName== imageDataFileName){
			return textureEntry.iTextureID;
		}

		itTextureEntry ++;
	}

	return -1;
}




