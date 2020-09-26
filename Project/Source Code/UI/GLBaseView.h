/*******************************************************************
* Author	: wwyang
* Date		: 2011.3.25
* Copyright : Multimedia Lab, Zhejiang Jongshang University 
* Head File : Opengl-based Qt window 
* Version   : 1.0
*********************************************************************/

// .NAME CGLBaseView -- An opengl view derived from the Qt opengl widget 

// .SECTION Description
// This class provides an opengl view that can be used to display & interact 
// the 3D scene.
// It can be used just as a qt widget

#ifndef _GLBASEVIEW_H_
#define _GLBASEVIEW_H_

#include <QGLWidget>
#include <QPoint>

//#include <gl/GL.h>

#include "BaseWindow.h"
#include "../Base/Vec3.h"
#include "../Base/Vec2.h"
#include "../Base/varray.h"
#include "../Base/Mesh.h"
#include <list>

class CGLBaseView : public QGLWidget, public CBaseWindow
{
	Q_OBJECT

public:
	CGLBaseView(QWidget *parent = 0, const QGLWidget * shareWidget = 0);
	~CGLBaseView(){};

protected:
	// Override the Qt implementation to perform traditional opengl tasks
	void initializeGL();
	void paintGL();

	//void paintEvent(QPaintEvent* event);
	//void resizeEvent(QResizeEvent *event);

	// Message Handler
	void mousePressEvent( QMouseEvent * event );
	void mouseLButtonDown(QMouseEvent * event );
	void mouseRButtonDown(QMouseEvent * event );
	void mouseReleaseEvent ( QMouseEvent * event ); 
	void mouseLButtonUp(QMouseEvent * event);
	void mouseRButtonUp(QMouseEvent * event);
	void mouseMoveEvent ( QMouseEvent * event ) ;
	void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);

	virtual void focusOutEvent ( QFocusEvent * event );
	virtual void focusInEvent ( QFocusEvent * event );
	bool _bHasFocus;

public:
	// Window render
	virtual void Render();

protected:
	// Description:
	// Render the border frame for activated view
	void RenderActiveFrame();   

public:
	void BeginScene();
	void SetProjection();
	void TransformScene();
	virtual void SceneGLSetting(){};
	void EndScene();

	void ResetTransform();
	double GetTransformScale();

	void SaveViewContentToImage(const QString &filePath);

public:
	// Description:
	// Select objects using open gl picking scheme
	void BeginPicking(const QPoint& p, double size);
	int EndPicking();
	void PickPoint(const QPoint& p, double size, const varray<Vec2>& arrayPoint, varray<int>& arraySelPoint);
	void PickPoint(const QPoint& p, double size, const std::list<Vec2>& listPoint, varray<int>& arraySelPoint);
	void PickPoint(const QPoint&p, double sizeWidth, double sizeHeight, const std::list<Vec2>& listPoint, varray<int>& arraySelPoint);
	void PickPoint(const QPoint&p, double sizeWidth, double sizeHeight, const varray<Vec2>& arrayPoint, varray<int>& arraySelPoint);
	bool PickPolygon(const QPoint&p, double sizeWidth, double sizeHeight, const varray<Vec2>& arrayPolygonVertex, bool bClose = true);
	void PickTriangle(const QPoint&p, double size, const CMesh *pMesh, varray<int>& arraySelTriangle);

	// Description:
	// Un-project screen point into model space
	Vec3 UnProjectPoint(const Vec3& screenpoint);
	Vec3 UnProjectPointForRender(const Vec3& screenpoint);
	// Project space point into screen space
	Vec3 ProjectPoint(const Vec3& spacepoint);
	Vec3 ProjectPointForRender(const Vec3& spacepoint);

protected:

	// Opengl scene space and its associated transformation
	GLint m_viewport[4];
	double m_fScreenSize[2];
	GLuint m_pickbuf [1024]; // For gl picking

	float m_fTranslate[2]; // Translation in xy plane
	std::list<double> m_listRotateAngle;
	std::list<int> m_listRotateAxis;
	float m_fScale;		   // Scaling ratio

	// User interaction variables
	bool m_bLButtonDown, m_bRButtonDown, m_bMButtonDown;
	QPoint m_lastPointPos;

};


#endif