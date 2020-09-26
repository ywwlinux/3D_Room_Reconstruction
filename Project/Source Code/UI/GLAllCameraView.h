/*******************************************************************
* Author	: wwyang
* Date		: 2020.4.20
* Copyright : Multimedia Lab, Zhejiang Jongshang University
* Head File : Opengl-based Qt window
* Version   : 1.0
*********************************************************************/

// .NAME CGLAllCameraView -- An opengl view derived from the Qt opengl widget 

// .SECTION Description
// This class is used for the display of all camera views

#ifndef _GLALLCAMERAVIEW_H_
#define _GLALLCAMERAVIEW_H_

#include <QGLWidget>

#include "BaseWindow.h"
#include "../Object/ImageObject.h"
#include "../Object/CameraAgent.h"

class CGLAllCameraView : public QGLWidget
{
	Q_OBJECT

public:
	CGLAllCameraView(QWidget *parent = 0, const QGLWidget * shareWidget = 0);
	~CGLAllCameraView();

protected:
	// Override the Qt implementation to perform traditional opengl tasks
	void initializeGL();
	void paintGL();
	//void paintEvent(QPaintEvent* event);
	//void resizeEvent(QResizeEvent* event);

protected:
	void BeginScene();
	void SetProjection();
	void TransformScene();
	virtual void SceneGLSetting() {};
	void EndScene();

public:
	// Description:
	// Window render
	virtual void Render();

	// Description:
	// Render all camera views
	void RenderCameraViews();
	void RenderACamera(CCameraAgent *pCamera, int iLeft, int iRight, int iBottom, int iTop);
public:
	// Description:
	// Interface for scrolling
	void ScrollUpDown(bool bScrollUp);
	void ResetScrollUp();

protected:
	int _iScrollUp;
};


#endif
