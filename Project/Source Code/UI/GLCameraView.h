/*******************************************************************
* Author	: wwyang
* Date		: 2019.12.5
* Copyright : Multimedia Lab, Zhejiang Gongshang University
* Head File : Opengl-based Qt window
* Version   : 1.0
*********************************************************************/

// .NAME CGLCameraView -- An opengl view that is used to mimic camera projection 

// .SECTION Description
// This class is defined to simulate the projection of a camera in the stereo camera system

#ifndef _GLCAMERAVIEW_H_
#define _GLCAMERAVIEW_H_

#include <QGLWidget>
#include <QPoint>
#include "../Base/Mesh.h"
#include "../Base/Vec3.h"
#include "../Base/ArcBall.h"
#include "BaseWindow.h"

#include "../Object/CameraAgent.h"
#include <string>

struct S_ModelTransform {
	// Transformation for the 3D model
	Vec3 m_Translate; // Translate vector
	CArcBall m_ArcRotation; // Rotation arc-ball
	Matrix4f m_RotTransform, m_LastRot, m_ThisRot; // Log rotation matrices

	// Initial model position and orientation											   
	Vec3 m_ModelPos;
	Vec3 m_ModelOrientation; // rotation angles around the x/y/z axis
}; // Transformation for the 3D model

class CGLCameraView : public QGLWidget, public CBaseWindow
{
	Q_OBJECT

public:
	CGLCameraView(QWidget *parent = 0, const QGLWidget * shareWidget = 0);
	~CGLCameraView() {}

protected:
	// Override the Qt implementation to perform traditional opengl tasks
	void initializeGL();
	void paintGL();

	// Message Handler
	void mousePressEvent(QMouseEvent * event);
	void mouseLButtonDown(QMouseEvent * event);
	void mouseRButtonDown(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void mouseLButtonUp(QMouseEvent * event);
	void mouseRButtonUp(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);

	virtual void focusOutEvent(QFocusEvent * event);
	virtual void focusInEvent(QFocusEvent * event);
	bool _bHasFocus;

public:
	// Description:
	// Window render
	virtual void Render();

protected:
	void ResetTransform();

	// Description:
	// Render the background image of the camera
	void RenderBkImage();
	void SetCameraForRenderBkImage();
	void RestoreCamera();

	// Description:
	// Render the model via camera's projection
	void RenderModel();
	void RenderAModel(CMesh *pModel);
	void RenderA3DMesh(CMesh *pMesh);
	void SetCameraForRenderModel();
	void TransformModel();
	void TurnOnLightMaterial();
	void TurnOffLightMaterial();

public:
	void BeginScene();
	void SetProjection();
	void TransformScene();
	virtual void SceneGLSetting() {};
	void EndScene();

	// Description:
	// Associate a camera to the view
	void SetCamera(const std::string &cameraName);

	// Description:
	// Transform the coordinate from world coordinate to camera coordinate
	void TransformCoordinateFromWorldToCamera();

	// Description:
	// Matrix operation
	void TransposeMatrix(const double matrix[9], double transM[9]);

	// Description:
	// Multiple the transpose of a matrix with a vector
	Vec3 TransposeMultiple(const double matrix[9], const Vec3& vec);
	Vec3 MatrixMultiple(const double matrix[9], const Vec3& vec);
	Vec3 RotateVector(const double matrix[16], const Vec3 &vec);

	// Description:
	// Get the model transform
	S_ModelTransform& GetModelTransform()
	{
		return m_modelTransform;
	}

protected:
	// Camera that is associated with the view
	CCameraAgent *m_pAssociateCamera; 
	std::string m_assoicateCameraName;

	// Transformation for the 3D model
	static S_ModelTransform m_modelTransform; // The model can be manipulated and recorded in the main camera, but used by all cameras
						   
	// Cameara-Image translation and scale
	Vec2 m_imageTranslate;
	double m_imageScale;

	// Opengl scene space and its associated transformation
	GLint m_viewport[4];
	double m_fScreenSize[2];
	GLuint m_pickbuf[1024]; // For gl picking

	// User interaction variables
	bool m_bLButtonDown, m_bRButtonDown, m_bMButtonDown, m_bCtrlButtonDown;
	QPoint m_lastPointPos;
	bool m_bRotate, m_bTranslate;
	int m_TranslateSwitch, m_ActiveTranslateSwitch; // 0 - translate along x axis; 1 - translate along y axis; 2 - translate along z axis

protected:
	double ComputeAngle2D(const Vec2 &vec1, const Vec2 &vec2)
	{
		double dot = Dot(vec1, vec2) / (vec1.Magnitude()*vec2.Magnitude());

		if (fabs(dot) >= 1.0) {  // it's very important !!!!!!!
			if (dot < 0)
				dot = -.999999;
			else
				dot = .999999;
		}

		return acos(dot);
	}
};


#endif