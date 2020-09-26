#include "GLCameraView.h"

#include <QtOpenGL>
#include <QtCore/QString>
#include <gl/GLU.h>

#include "../Scene/ObjectManagerFactory.h"
#include "../Base/ArcBall.h"

CGLCameraView::CGLCameraView(QWidget *parent /* = 0 */, const QGLWidget * shareWidget /* = 0 */)
	: QGLWidget(QGLFormat(/*QGL::SampleBuffers | QGL::DoubleBuffer*/ QGL::SampleBuffers | QGL::Rgba | QGL::DoubleBuffer | QGL::DirectRendering |
		QGL::AlphaChannel), parent, shareWidget)
	//:QGLWidget(QGLFormat(QGL::SampleBuffers | QGL::DoubleBuffer | QGL::DepthBuffer
	//		   | QGL::Rgba | QGL::AlphaChannel | QGL::AccumBuffer | QGL::StencilBuffer) ,parent)
{

	setFocusPolicy(Qt::StrongFocus); // Enable to accept the input of keyboard
	this->setMouseTracking(false);
	_bHasFocus = false;


	m_bLButtonDown = m_bMButtonDown = m_bRButtonDown = false;

	m_imageScale = 1.;

	m_pAssociateCamera = 0;

	// 2019-7-16 for 3d-printed hand
	m_modelTransform.m_ModelPos.Set(638.14, 459.54, 2901.81);
	m_modelTransform.m_ModelOrientation.Set(90, 0, 0);

	return;
}

void CGLCameraView::SetCamera(const std::string &cameraName)
{
	m_assoicateCameraName = cameraName;
}

void CGLCameraView::initializeGL()
{
	// Initialize the gl setting, e.g., transformation.

	return;
}

void CGLCameraView::paintGL()
{
	Render();
}

void CGLCameraView::Render()
{
	m_pAssociateCamera = CObjectManagerFactory::GetCameraManager().GetACamera(m_assoicateCameraName);

	// Set current opengl render context 
	BeginScene();

	// Initialize the projection matrix
	SetProjection();

	// Transform the scene to desired location
	TransformScene();

	// Opengl setting
	SceneGLSetting();

	// Clear the window
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render the photo taken by the camera
	RenderBkImage();

	// Render a 3D model using the camera setting
	RenderModel();

	// Render all the renderers inserted into the window,  
	// e.g., renderer of active command; renderer of current key frame.
	CBaseWindow::Render();

	swapBuffers();

	// Stop rendering
	EndScene();
}

void CGLCameraView::BeginScene()
{
	// Make this widget the current widget for opengl operation, i.e., makes the widget's rendering context
	// the current opengl  rendering context
	makeCurrent();

	return;
}

void CGLCameraView::SetProjection()
{
	// Set the default opengl projection matrix
	GLsizei width, height;
	GLfloat aspect;

	width = QWidget::width();
	height = QWidget::height();

	if (height == 0)									// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}
	aspect = (GLfloat)width / (GLfloat)height;

	glViewport(0, 0, width, height);					// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	double fHalfHeight = (height*.5);
	glOrtho(-fHalfHeight*aspect, fHalfHeight*aspect, -fHalfHeight, fHalfHeight, -1, 1);

	m_viewport[0] = m_viewport[1] = 0;
	m_viewport[2] = width;
	m_viewport[3] = height;
	m_fScreenSize[0] = 2 * fHalfHeight*aspect;
	m_fScreenSize[1] = 2 * fHalfHeight;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CGLCameraView::TransformScene()
{
	// Transform the scene
	// Initialize model view 
	glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix

	return;
}

void CGLCameraView::ResetTransform()
{
	m_modelTransform.m_Translate.Set(0, 0, 0);
	m_modelTransform.m_RotTransform.setIdentity();

	m_imageScale = 1.;
	m_imageTranslate.Set(.0, .0);
}

void CGLCameraView::EndScene()
{
	// make context to null
	doneCurrent();
}

void CGLCameraView::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton) {
		mouseLButtonDown(event);
	}
	else if (event->button() == Qt::RightButton) {
		mouseRButtonDown(event);
	}

	QWidget::mousePressEvent(event);

	return;
}

void CGLCameraView::mouseLButtonDown(QMouseEvent * event)
{
	m_bLButtonDown = true;
	m_lastPointPos = event->pos();

	// Pass to alive commands
	if (!GetTransformState() && !event->modifiers().testFlag(Qt::AltModifier)) {
		// Transform 

		CMsg msg;
		msg._pWnd = this;
		msg._msgType = CMsg::LBUTTONDOWN;

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		msg._point.Set(event->pos().x(), event->pos().y());

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);
	}
	else if(m_assoicateCameraName == "camera_main"){
		m_bRotate = true;

		Point mousePt;
		mousePt.x = event->pos().x();
		mousePt.y = event->pos().y();

		m_modelTransform.m_LastRot.set(m_modelTransform.m_RotTransform);   // Set Last Static Rotation To Current One   
		m_modelTransform.m_ArcRotation.setBounds(m_fScreenSize[0], m_fScreenSize[1]);
		m_modelTransform.m_ArcRotation.click(mousePt);    // Update Start Vector And Prepare For Dragging
	}

	return;
}

void CGLCameraView::mouseRButtonDown(QMouseEvent * event)
{
	m_bRButtonDown = true;
	m_lastPointPos = event->pos();

	// Pass to alive commands
	if (!GetTransformState() && !event->modifiers().testFlag(Qt::AltModifier)) {
		CMsg msg;
		msg._pWnd = this;
		msg._msgType = CMsg::RBUTTONDOWN;

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		msg._point.Set(event->pos().x(), event->pos().y());

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);
	}
	else if (m_assoicateCameraName == "camera_main") {
		m_bTranslate = true;
		m_ActiveTranslateSwitch = m_TranslateSwitch;
	}

	return;
}

void CGLCameraView::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton) {
		mouseLButtonUp(event);
	}
	else if (event->button() == Qt::RightButton) {
		mouseRButtonUp(event);
	}

	QWidget::mouseReleaseEvent(event);

	return;
}

void CGLCameraView::mouseLButtonUp(QMouseEvent * event)
{
	// Pass to alive commands
	if (!GetTransformState() && !event->modifiers().testFlag(Qt::AltModifier)) {
		CMsg msg;
		msg._pWnd = this;
		msg._msgType = CMsg::LBUTTONUP;

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		msg._point.Set(event->pos().x(), event->pos().y());

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);
	}

	m_bLButtonDown = false;
	m_bRotate = false;

	return;
}

void CGLCameraView::mouseRButtonUp(QMouseEvent * event)
{
	// Pass to alive commands
	if (!GetTransformState() && !event->modifiers().testFlag(Qt::AltModifier)) {
		CMsg msg;
		msg._pWnd = this;
		msg._msgType = CMsg::RBUTTONUP;

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		msg._point.Set(event->pos().x(), event->pos().y());

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);
	}

	m_bRButtonDown = false;
	m_bTranslate = false;

	return;
}

void CGLCameraView::mouseMoveEvent(QMouseEvent * event)
{
	if (GetTransformState() /*|| event->modifiers().testFlag(Qt::AltModifier)*/) {
		double dx = double(event->pos().x() - m_lastPointPos.x()), dy = double(event->pos().y() - m_lastPointPos.y());
		if (m_viewport[2] == 0) {
			m_viewport[2] = 1;
		}
		if (m_viewport[3] == 0) {
			m_viewport[3] = 1;
		}
		if (m_bLButtonDown & m_bRotate) { // Rotate

			Quat4f ThisQuat;
			Point mousePt;

			mousePt.x = event->pos().x();
			mousePt.y = event->pos().y();

			m_modelTransform.m_ArcRotation.drag(mousePt, ThisQuat); // Update End Vector And Get Rotation As Quaternion
			m_modelTransform.m_ThisRot.setRotation(ThisQuat); // Convert Quaternion Into Matrix
			m_modelTransform.m_RotTransform.mul(m_modelTransform.m_ThisRot, m_modelTransform.m_LastRot); // Accumulate Last Rotation Into This One

													  // Render scene
			CObjectManagerFactory::GetCommandManager().GetMainWindowHandle()->UpdateCameraView(CMainWindow::ALLVIEW);
		}
		else if (m_bRButtonDown) { // Translate
			if (event->modifiers().testFlag(Qt::AltModifier)) {
				// Move the image
				m_imageTranslate += Vec2(dx, -dy);

				// Render scene
				Render();
			}
			else if(m_assoicateCameraName == "camera_main"){ // Move the model
				dx = dx / (double)m_viewport[2] * m_fScreenSize[0];
				dy = dy / (double)m_viewport[3] * m_fScreenSize[1];

				Vec2 moveVec(dx, dy);
				double xAxisAngle, yAxisAngle, zAxisAngle;
				double angle1, angle2;

				// x - axis
				angle1 = ComputeAngle2D(Vec2(1, 0), moveVec);
				angle2 = ComputeAngle2D(Vec2(-1, 0), moveVec);
				xAxisAngle = (angle1 < angle2 ? angle1 : angle2);

				// y - axis
				angle1 = ComputeAngle2D(Vec2(0, 1), moveVec);
				angle2 = ComputeAngle2D(Vec2(0, -1), moveVec);
				yAxisAngle = (angle1 < angle2 ? angle1 : angle2);

				// z - axis
				angle1 = ComputeAngle2D(Vec2(1, 1), moveVec);
				angle2 = ComputeAngle2D(Vec2(-1, -1), moveVec);
				zAxisAngle = (angle1 < angle2 ? angle1 : angle2);

				if (xAxisAngle <= yAxisAngle && xAxisAngle <= zAxisAngle) {
					m_ActiveTranslateSwitch = 0;
				}
				else if (yAxisAngle <= xAxisAngle && yAxisAngle <= zAxisAngle) {
					m_ActiveTranslateSwitch = 1;
				}
				else {
					assert(zAxisAngle <= xAxisAngle && zAxisAngle <= yAxisAngle);
					m_ActiveTranslateSwitch = 2;
				}

				double moveLen = sqrt(dx*dx + dy*dy);
				//moveLen = 0;
				if (m_ActiveTranslateSwitch == 0) { // move along x
					if (dx > 0) {
						m_modelTransform.m_Translate.x() += moveLen;
					}
					else {
						m_modelTransform.m_Translate.x() -= moveLen;
					}
				}
				else if (m_ActiveTranslateSwitch == 1) { // move along y
					if (dy > 0) {
						m_modelTransform.m_Translate.y() += moveLen;
					}
					else {
						m_modelTransform.m_Translate.y() -= moveLen;
					}
				}
				else { // move along z
					assert(m_ActiveTranslateSwitch == 2);
					if (dy > 0) {
						m_modelTransform.m_Translate.z() -= moveLen;
					}
					else {
						m_modelTransform.m_Translate.z() += moveLen;
					}
				}

				// Render scene
				CObjectManagerFactory::GetCommandManager().GetMainWindowHandle()->UpdateCameraView(CMainWindow::ALLVIEW);
			}
		}
	}
	else {
		// Pass to alive commands
		CMsg msg;
		msg._pWnd = this;
		msg._msgType = CMsg::MOUSEMOVE;

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		msg._point.Set(event->pos().x(), event->pos().y());

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);
	}

	m_lastPointPos = event->pos();

	QWidget::mouseMoveEvent(event);

	return;
}

void CGLCameraView::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Space) {
		ResetTransform();

		// Pass to alive commands
		CMsg msg;
		msg._pWnd = (this);
		msg._msgType = CMsg::KEY_DOWN;
		msg._nChar = event->key();

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);

		Render();
	}
	else {
		// Pass to alive commands
		CMsg msg;
		msg._pWnd = (this);
		msg._msgType = CMsg::KEY_DOWN;
		msg._nChar = event->key();

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);

		if (m_assoicateCameraName == "camera_main") {
			double moveLen = 10;
			if (msg._nChar == 'a' || msg._nChar == 'A') {
				if (msg._bAltPress)
					m_modelTransform.m_Translate.x() -= moveLen;
				else
					m_modelTransform.m_Translate.x() += moveLen;
			}
			else if (msg._nChar == 'd' || msg._nChar == 'D') {
				if (msg._bAltPress)
					m_modelTransform.m_Translate.y() -= moveLen;
				else
					m_modelTransform.m_Translate.y() += moveLen;
			}
			else if (msg._nChar == 's' || msg._nChar == 'S') {
				if (msg._bAltPress)
					m_modelTransform.m_Translate.z() -= moveLen;
				else
					m_modelTransform.m_Translate.z() += moveLen;
			}

			CObjectManagerFactory::GetCommandManager().GetMainWindowHandle()->UpdateCameraView(CMainWindow::ALLVIEW);
		}
	}

	QWidget::keyPressEvent(event);
}

void CGLCameraView::wheelEvent(QWheelEvent *event)
{
	if (!GetTransformState() && !event->modifiers().testFlag(Qt::AltModifier)) {
		// Pass to alive commands
		CMsg msg;
		msg._pWnd = this;
		msg._msgType = CMsg::MOUSEWHEEL;
		msg._zDelta = event->delta();

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		msg._point.Set(event->pos().x(), event->pos().y());

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);
	}
	else {
		CCameraAgent *pCamera = m_pAssociateCamera;
		CImageObject *pBkImage = (pCamera != 0 ? pCamera->GetPhotoImage() : 0);
		if (/*event->modifiers().testFlag(Qt::AltModifier) && */pBkImage != 0) { // Scale the image
			double tempScale = m_imageScale;
			tempScale -= (event->delta()) / 3000.0f;;
			if (tempScale <= 0) {
				tempScale = .01;
			}
			else {
				// test if the size of the image is greater than its original size
				double winX = QWidget::width(), winY = QWidget::height();
				double imgX = pBkImage->GetImage()->width(), imgY = pBkImage->GetImage()->height();
				double scaleX = 1, scaleY = 1, scaleImg;

				if (imgX > winX) {
					scaleX = winX / imgX;
				}
				if (imgY > winY) {
					scaleY = winY / imgY;
				}
				scaleImg = (scaleX < scaleY ? scaleX : scaleY);

				if (tempScale*scaleImg > 1) {
					tempScale = 1. / scaleImg;
				}
			}

			m_imageScale = tempScale;
		}
		
		Render();
	}

	QWidget::wheelEvent(event);
}

void CGLCameraView::focusOutEvent(QFocusEvent * event)
{
	if (_bHasFocus) {
		this->setMouseTracking(false);
		_bHasFocus = false;
		Render();
	}
}

void CGLCameraView::focusInEvent(QFocusEvent * event)
{
	if (!_bHasFocus) {
		this->setMouseTracking(true);
		_bHasFocus = true;
		Render();
	}
}

void CGLCameraView::SetCameraForRenderBkImage()
{
	// Push back the projection matrix
	glMatrixMode(GL_PROJECTION);

	glPushMatrix();

	// Set the opengl projection matrix
	GLsizei width, height;
	GLfloat aspect;

	width = QWidget::width();
	height = QWidget::height();

	if (height == 0)									// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}
	aspect = (GLfloat)width / (GLfloat)height;

	glViewport(0, 0, width, height);					// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	double fHalfHeight = (height*.5);
	glOrtho(-fHalfHeight*aspect, fHalfHeight*aspect, -fHalfHeight, fHalfHeight, -1, 1);

	m_viewport[0] = m_viewport[1] = 0;
	m_viewport[2] = width;
	m_viewport[3] = height;
	m_fScreenSize[0] = 2 * fHalfHeight*aspect;
	m_fScreenSize[1] = 2 * fHalfHeight;

	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();

	glLoadIdentity();

	return;
}

void CGLCameraView::RestoreCamera()
{
	glMatrixMode(GL_PROJECTION);

	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	glPopMatrix();

	return;
}

void CGLCameraView::RenderBkImage()
{
	CCameraAgent *pCamera = m_pAssociateCamera;
	if (pCamera == 0) return;

	CImageObject *pBkImage = pCamera->GetPhotoImage();
	if (pBkImage == 0) return;

	// Set the camera for the rendering
	SetCameraForRenderBkImage();

	// Render the image
	double winX = QWidget::width(), winY = QWidget::height();
	double imgX = pBkImage->GetImage()->width(), imgY = pBkImage->GetImage()->height();
	double scaleX = 1, scaleY = 1, scaleImg;

	if (imgX > winX) {
		scaleX = winX / imgX;
	}
	if (imgY > winY) {
		scaleY = winY / imgY;
	}
	scaleImg = (scaleX < scaleY ? scaleX : scaleY);
	scaleImg = scaleImg * m_imageScale;

	CImageObject *pTexture = pBkImage;

	// Render the character using texture
	int iTextureID = pTexture->GetTextureBuddy();
	if (iTextureID >= 0) {
		assert(glIsTexture(iTextureID));

		// Bind this texture
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, (GLuint)(iTextureID));

		// Draw the image geometry
		QImage *pImageData = pTexture->GetImage();
		double halfImageWidth = imgX*.5*scaleImg, halfImageHeight = imgY*.5*scaleImg;

		// Create an rectangle to cover the image
		Vec2 rectV[4], textCoordV[4];
		rectV[0].Set(-halfImageWidth, -halfImageHeight);
		rectV[1].Set(halfImageWidth, -halfImageHeight);
		rectV[2].Set(halfImageWidth, halfImageHeight);
		rectV[3].Set(-halfImageWidth, halfImageHeight);

		int i = 0;
		for (i = 0; i < 4; i++) {
			textCoordV[i].Set((double)(rectV[i].x() + halfImageWidth) / (double)(2 * halfImageWidth), (double)(rectV[i].y() + halfImageHeight) / (double)(2 * halfImageHeight));
		}

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_QUADS);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		double dx = m_imageTranslate.x(), dy = m_imageTranslate.y();

		for (i = 0; i < 4; i++) {
			glTexCoord2d(textCoordV[i].x(), textCoordV[i].y());
			glVertex2d(rectV[i].x() + dx, rectV[i].y() + dy);
		}
		glEnd();

		//glDisable(GL_BLEND);

		glDisable(GL_TEXTURE_2D);
	}

	// Restore the camera
	RestoreCamera();

	return;
}

void CGLCameraView::RenderModel()
{
	CMesh *pModel = CObjectManagerFactory::GetSceneObjectManager().Get3DModel();
	if (pModel == 0) return;

	if (m_pAssociateCamera == 0) return;

	// Set the camera for the rendering
	SetCameraForRenderModel();

	TurnOnLightMaterial();

	// Transform the teeth
	TransformModel();

	// Render a model
	RenderAModel(pModel);

	TurnOffLightMaterial();

	// Restore the camera
	RestoreCamera();

	return;
}

void CGLCameraView::TurnOnLightMaterial()
{
	// Set and turn on the lights
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Create light components
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.f };
	GLfloat diffuseLight[] = { 0.8f, 0.2f, 0.8, 1.f };
	GLfloat specularLight[] = { .8f, .8f, .8f, 1.f };
	GLfloat position[] = { 15.f, 20.f, -4.0f, 1.0f };

	if (m_assoicateCameraName != "camera_main") {
		diffuseLight[1] = 0.8;
	}

	position[0] = 0;
	position[1] = -10000;
	position[2] = 20000;

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	float colorBlue[] = { 0.0f, 0.0f, 1.0f, 0.5f };
	float shiness[] = { 100 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ambientLight);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shiness);

	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	
	// Set and turn on the lights
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	return;
}

void CGLCameraView::TurnOffLightMaterial()
{
	// Turn off the lighting
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
}

void CGLCameraView::TransformCoordinateFromWorldToCamera()
{
	CCameraAgent *pCamera = m_pAssociateCamera; 

	// Transform to the camera's coordinate system
	const CCameraAgent::S_CameraExtrinsicParameter &extrinsicParam = pCamera->GetExtrinsicParameter();
	double transposeR[9];

	TransposeMatrix(extrinsicParam.R, transposeR);

	GLdouble rotMatrix[16];
	double a00 = transposeR[0], a01 = transposeR[1], a02 = transposeR[2],
		a10 = transposeR[3], a11 = transposeR[4], a12 = transposeR[5],
		a20 = transposeR[6], a21 = transposeR[7], a22 = transposeR[8];

	rotMatrix[0] = a00;
	rotMatrix[1] = a10;
	rotMatrix[2] = a20;
	rotMatrix[3] = 0;

	rotMatrix[4] = a01;
	rotMatrix[5] = a11;
	rotMatrix[6] = a21;
	rotMatrix[7] = 0;

	rotMatrix[8] = a02;
	rotMatrix[9] = a12;
	rotMatrix[10] = a22;
	rotMatrix[11] = 0;

	rotMatrix[12] = 0;
	rotMatrix[13] = 0;
	rotMatrix[14] = 0;
	rotMatrix[15] = 1;

	glMultMatrixd(rotMatrix);

	glTranslated(-extrinsicParam.tx, -extrinsicParam.ty, -extrinsicParam.tz);

	return;
}

void CGLCameraView::TransformModel()
{
	// Transform the box into the right camera coordinate system
	glMatrixMode(GL_MODELVIEW);

	// Transform to the right camera's coordinates
	TransformCoordinateFromWorldToCamera();

	// Scale and rotate around the center of the teeth model
	CMesh *pModel = CObjectManagerFactory::GetSceneObjectManager().Get3DModel();
	if (pModel == 0) return;

	const Vec3 &modelCenter = pModel->GetCenter();

	// Translate the model
	glTranslated(m_modelTransform.m_Translate.x(), m_modelTransform.m_Translate.y(), m_modelTransform.m_Translate.z());

	// Translate to the model's position
	glTranslated(m_modelTransform.m_ModelPos.x(), m_modelTransform.m_ModelPos.y(), m_modelTransform.m_ModelPos.z());

	// Move back to the model center
	glTranslated(modelCenter.x(), modelCenter.y(), modelCenter.z());

	// Scale the model -- no scale for the model												 
	glScaled(1, 1, 1);

	// Rotate the box
	GLdouble rotMatrix[16];
	m_modelTransform.m_RotTransform.get(rotMatrix);
	glMultMatrixd(rotMatrix);

	// Rotate to the model's initial orientation
	glRotated(m_modelTransform.m_ModelOrientation.x(), 1, 0, 0);
	glRotated(m_modelTransform.m_ModelOrientation.y(), 0, 1, 0);
	glRotated(m_modelTransform.m_ModelOrientation.z(), 0, 0, 1);

	// Move the box center to origin
	glTranslated(-modelCenter.x(), -modelCenter.y(), -modelCenter.z());

	return;
}

void CGLCameraView::RenderAModel(CMesh *pModel)
{
	//glLineWidth(1);

	// Turn on blending
	glColor4f(1., 1., 1., .4); // for transparency blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	RenderA3DMesh(pModel);

	// Turn off the blending
	glDisable(GL_BLEND);

	return;
}

void CGLCameraView::RenderA3DMesh(CMesh *pMesh)
{
	CMesh &mesh = *pMesh;

	// Render the input mesh
	int i = 0;
	int nFSize = mesh.GetFSize();

	if (nFSize <= 0) {
		return;
	}

	glPushMatrix();

	// Set draw attribute
	glEnable(GL_DEPTH_TEST);

	glPolygonOffset(1.0, 1.0);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	GLint polygonMode[2];
	glGetIntegerv(GL_POLYGON_MODE, polygonMode);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Draw depth-coving face
	glBegin(GL_TRIANGLES);
	for (i = nFSize - 1; i >= 0; i--) {
		const CFace& face = mesh.GetF(i);
		Vec3 v0, v1, v2;
		v0 = mesh.GetV(face.p(0)).Pos();
		v1 = mesh.GetV(face.p(1)).Pos();
		v2 = mesh.GetV(face.p(2)).Pos();

		glVertex3f(v0.x(), v0.y(), v0.z());
		glVertex3f(v1.x(), v1.y(), v1.z());
		glVertex3f(v2.x(), v2.y(), v2.z());
	}
	glEnd();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDisable(GL_POLYGON_OFFSET_FILL);

	// Draw mesh
	glPolygonMode(GL_FRONT, polygonMode[0]);
	glPolygonMode(GL_BACK, polygonMode[1]);

	glBegin(GL_TRIANGLES);

	for (i = 0; i < nFSize; i++) {
		const CFace& face = pMesh->GetF(i);
		const CVertex &V1 = pMesh->GetV(face.p(0)), &V2 = pMesh->GetV(face.p(1)), &V3 = pMesh->GetV(face.p(2));
		const Vec3 &v1 = V1.Pos(), &v2 = V2.Pos(), &v3 = V3.Pos();
		const Vec3 &n1 = V1.Normal(), &n2 = V2.Normal(), &n3 = V3.Normal();

		glNormal3d(n1.x(), n1.y(), n1.z());
		glVertex3d(v1.x(), v1.y(), v1.z());

		glNormal3d(n2.x(), n2.y(), n2.z());
		glVertex3d(v2.x(), v2.y(), v2.z());

		glNormal3d(n3.x(), n3.y(), n3.z());
		glVertex3d(v3.x(), v3.y(), v3.z());
	}

	glEnd();

	glDisable(GL_DEPTH_TEST);

	glPopMatrix();

	return;
}

void CGLCameraView::SetCameraForRenderModel()
{
	// Push back the projection matrix
	glMatrixMode(GL_PROJECTION);

	glPushMatrix();

	// Set the opengl projection matrix
	GLsizei width, height;
	GLfloat aspect;

	width = QWidget::width();
	height = QWidget::height();

	if (height == 0)									// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}
	aspect = (GLfloat)width / (GLfloat)height;

	CCameraAgent *pCamera = m_pAssociateCamera;
	CImageObject *pBkImage = pCamera->GetPhotoImage();
	if (pBkImage != 0) {
		double winX = QWidget::width(), winY = QWidget::height();
		double imgX = pBkImage->GetImage()->width(), imgY = pBkImage->GetImage()->height();
		double scaleX = 1, scaleY = 1, scaleImg;

		if (imgX > winX) {
			scaleX = winX / imgX;
		}
		if (imgY > winY) {
			scaleY = winY / imgY;
		}
		scaleImg = (scaleX < scaleY ? scaleX : scaleY);
		scaleImg = scaleImg * m_imageScale;

		int drawImgX = (int)(imgX * scaleImg), drawImgY = (int)(imgY * scaleImg);
		int dx = width - drawImgX, dy = height - drawImgY;

		const CCameraAgent::S_CameraIntrinsicParameter &intrinsicParam = pCamera->GetIntrinsicParameter();
		double pinHoleDx = intrinsicParam.cx - imgX / 2, pinHoleDy = intrinsicParam.cy - imgY / 2;

		pinHoleDx *= scaleImg;
		pinHoleDy *= scaleImg;

		glViewport(dx / 2 + pinHoleDx + m_imageTranslate.x(), dy / 2 - pinHoleDy + m_imageTranslate.y(), drawImgX, drawImgY);

	}
	else {
		glViewport(0, 0, width, height);					// Reset The Current Viewport
	}

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	double fHalfHeight = (height*.5);

	m_viewport[0] = m_viewport[1] = 0;
	m_viewport[2] = width;
	m_viewport[3] = height;
	m_fScreenSize[0] = 2 * fHalfHeight*aspect;
	m_fScreenSize[1] = 2 * fHalfHeight;

	const CCameraAgent::S_CameraIntrinsicParameter &intrinsicParam = pCamera->GetIntrinsicParameter();
	double sensorHalfHeight = pCamera->GetPhotoImage()->GetImage()->height()*.5, sensorHalfWidth = pCamera->GetPhotoImage()->GetImage()->width()*.5;
	double fx = intrinsicParam.fx;
	double FOV = 2 * atan(sensorHalfHeight / fx);

	FOV = FOV / 3.14159 * 180.;
	aspect = sensorHalfWidth / sensorHalfHeight;

	double nearPlane = .1, farPlane = 1.0e3 * 100; // 100 meters
	//nearPlane = nearPlane * fx / pCamera->GetIntrinsicParameter().fx;
	gluPerspective(FOV, aspect, nearPlane, farPlane);

	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();

	glLoadIdentity();

	Vec3 lookAt(0, 0, 1), up(0, -1, 0);
	Vec3 eyePoint(0, 0, 0), refPoint;

	refPoint = eyePoint + lookAt;

	gluLookAt(eyePoint.x(), eyePoint.y(), eyePoint.z(), refPoint.x(), refPoint.y(), refPoint.z(), up.x(),
		up.y(), up.z());

	return;
}

Vec3 CGLCameraView::TransposeMultiple(const double R[9], const Vec3& vec)
{
	Vec3 result;
	double x = vec.x(), y = vec.y(), z = vec.z();

	result.x() = R[0] * x + R[3] * y + R[6] * z;
	result.y() = R[1] * x + R[4] * y + R[7] * z;
	result.z() = R[2] * x + R[5] * y + R[8] * z;

	return result;
}

Vec3 CGLCameraView::MatrixMultiple(const double R[9], const Vec3& vec)
{
	Vec3 result;
	double x = vec.x(), y = vec.y(), z = vec.z();

	result.x() = R[0] * x + R[1] * y + R[2] * z;
	result.y() = R[3] * x + R[4] * y + R[5] * z;
	result.z() = R[6] * x + R[7] * y + R[8] * z;

	return result;
}

Vec3 CGLCameraView::RotateVector(const double matrix[16], const Vec3 &vec)
{
	double x = vec.x(), y = vec.y(), z = vec.z(), rotX, rotY, rotZ;

	rotX = matrix[0] * x + matrix[4] * y + matrix[8] * z + matrix[12] * 1.;
	rotY = matrix[1] * x + matrix[5] * y + matrix[9] * z + matrix[13] * 1.;
	rotZ = matrix[2] * x + matrix[6] * y + matrix[10] * z + matrix[14] * 1.;

	return Vec3(rotX, rotY, rotZ);
}

void CGLCameraView::TransposeMatrix(const double matrix[9], double transM[9])
{
	transM[0] = matrix[0];
	transM[1] = matrix[3];
	transM[2] = matrix[6];

	transM[3] = matrix[1];
	transM[4] = matrix[4];
	transM[5] = matrix[7];

	transM[6] = matrix[2];
	transM[7] = matrix[5];
	transM[8] = matrix[8];

	return;
}

S_ModelTransform CGLCameraView::m_modelTransform;