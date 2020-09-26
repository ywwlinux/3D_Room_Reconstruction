#include "GLAllCameraView.h"

#include <QtOpenGL>

#include "../Scene/ObjectManagerFactory.h"

CGLAllCameraView::CGLAllCameraView(QWidget *parent /* = 0 */, const QGLWidget * shareWidget /* = 0 */)
	: QGLWidget(QGLFormat(/*QGL::SampleBuffers | QGL::DoubleBuffer*/ QGL::SampleBuffers | QGL::Rgba | QGL::DoubleBuffer | QGL::DirectRendering |
		QGL::AlphaChannel), parent, shareWidget)
{

	setFocusPolicy(Qt::StrongFocus); // Enable to accept the input of keyboard
	this->setMouseTracking(false);

	_iScrollUp = 0;
}

CGLAllCameraView::~CGLAllCameraView()
{

}

void CGLAllCameraView::BeginScene()
{
	// Make this widget the current widget for opengl operation, i.e., makes the widget's rendering context
	// the current opengl  rendering context
	makeCurrent();

	return;
}

void CGLAllCameraView::SetProjection()
{
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

	double fHalfHeight = .5*height;
	glOrtho(-fHalfHeight*aspect, fHalfHeight*aspect, -fHalfHeight, fHalfHeight, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CGLAllCameraView::TransformScene()
{
	// Do nothing

	return;
}

void CGLAllCameraView::EndScene()
{
	// make no opengl context current context
	doneCurrent();

	return;
}

void CGLAllCameraView::initializeGL()
{
	// Initialize the opengl data

	return;
}

void CGLAllCameraView::paintGL()
{
	Render();
}

//void CGLAllCameraView::resizeEvent(QResizeEvent *event)
//{
//	Render();
//
//	event->accept();
//}

void CGLAllCameraView::Render()
{
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

	RenderCameraViews();

	swapBuffers();

	// Stop rendering
	EndScene();
}

void CGLAllCameraView::RenderCameraViews()
{
	// Render the camera views
	const std::list<CCameraAgent *> &listCamera = CObjectManagerFactory::GetCameraManager().GetCameras();

	if (listCamera.size() == 0) return;

	int width = QWidget::width(), height = QWidget::height();
	CImageObject *pImage = listCamera.front()->GetPhotoImage();
	int imageWidth = pImage->GetImage()->width(), imageHeight = pImage->GetImage()->height();
	int dx = 5, dy = 5;
	double scale = double(width - 2 * dx) / imageWidth;
	int rectWidth = width - 2 * dx, rectHeight = imageHeight*scale;
	int nShowCamera = (height-dy) / (rectHeight + dy);
	int resid = height - nShowCamera*(rectHeight + dy);

	if (resid >= (dy + rectHeight/2)) nShowCamera++;

	// Render all camera views
	auto itCamera = listCamera.begin();
	for (int iSkipCamera = 0; iSkipCamera < _iScrollUp; iSkipCamera++) {
		itCamera++;
	}

	int iStX = -width / 2 + dx, iStY = height / 2 - dy;
	for (int iShowCamera = 0; iShowCamera < nShowCamera && itCamera != listCamera.end(); iShowCamera++, itCamera++) {
		// Render a camera view
		CCameraAgent *pCamera = *itCamera;

		RenderACamera(pCamera, iStX, iStX + rectWidth, iStY - rectHeight, iStY);

		iStY -= (rectHeight + dy);
	}

	return;
}

void CGLAllCameraView::RenderACamera(CCameraAgent *pCamera, int iLeft, int iRight, int iBottom, int iTop)
{
	CImageObject *pCameraImage = pCamera->GetPhotoImage();

	if (pCameraImage != 0) {
		CImageObject *pTexture = pCameraImage;

		// Render the image using texture
		int iTextureID = pTexture->GetTextureBuddy();
		if (iTextureID >= 0) {
			assert(glIsTexture(iTextureID));

			// Bind this texture
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (GLuint)(iTextureID));

			// Film size
			double xLeft = iLeft, xRight = iRight;
			double yUp = iTop, yDown = iBottom;

			// Image size
			QImage *pImageData = pTexture->GetImage();
			double filmEdgeWidth = 2;
			double w = pImageData->width(), h = pImageData->height();
			double frameWidth = xRight - xLeft - filmEdgeWidth * 2, frameHeight = yUp - yDown - filmEdgeWidth * 2;
			double scaleX = frameWidth / w, scaleY = frameHeight / h, scale = (scaleX < scaleY ? scaleX : scaleY);

			if (scale > 1) {
				scale = 1;
			}

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			// Translate the origin to the frame center
			glTranslatef((xLeft + xRight) / 2, (yDown + yUp) / 2, .0);

			// Scale the image surround by the frame
			glScalef(scale, scale, 1.);

			// Draw the image geometry
			double halfImageWidth = pImageData->width()*.5, halfImageHeight = pImageData->height()*.5;

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

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_QUADS);
			glColor4f(1.0, 1.0, 1.0, 1.0);

			for (i = 0; i < 4; i++) {
				glTexCoord2d(textCoordV[i].x(), textCoordV[i].y());
				glVertex2d(rectV[i].x(), rectV[i].y());
			}
			glEnd();

			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);

			// render for the main camera
			if (pCamera->GetName().compare("camera_main") == 0) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glLineWidth(3);
				glColor4f(1.0, 42. / 255., .0, 1.0);

				glBegin(GL_QUADS);
				for (i = 0; i < 4; i++) {
					glVertex2d(rectV[i].x(), rectV[i].y());
				}
				glEnd();
			}

			glPopMatrix();
		}
	}

	return;
}

void CGLAllCameraView::ScrollUpDown(bool bScrollUp)
{
	if (bScrollUp) {
		_iScrollUp++;

		if (_iScrollUp >= CObjectManagerFactory::GetCameraManager().GetCameras().size()) {
			_iScrollUp--;
		}
	}
	else {
		_iScrollUp--;

		if (_iScrollUp < 0) _iScrollUp = 0;
	}

	Render();
}

void CGLAllCameraView::ResetScrollUp()
{
	_iScrollUp = 0;

	Render();
}
