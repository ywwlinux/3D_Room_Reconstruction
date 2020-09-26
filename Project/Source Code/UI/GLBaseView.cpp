#include <QtOpenGL>
#include <gl/GLU.h>

#include "../scene/ObjectManagerFactory.h" // For anything manager's factory

#include "GLBaseView.h"


CGLBaseView::CGLBaseView(QWidget *parent /* = 0 */, const QGLWidget * shareWidget /* = 0 */)
: QGLWidget(QGLFormat(/*QGL::SampleBuffers | QGL::DoubleBuffer*/ QGL::SampleBuffers | QGL::Rgba | QGL::DoubleBuffer | QGL::DirectRendering |
			QGL::AlphaChannel ), parent, shareWidget)
{
	// User interaction
	m_bLButtonDown = m_bRButtonDown = m_bMButtonDown = false;
	m_fTranslate[0] = m_fTranslate[1] = .0f;
	m_listRotateAxis.clear();
	m_listRotateAngle.clear();
	m_fScale = 1.f;

	setFocusPolicy(Qt::StrongFocus);

	_bHasFocus = false;
	this->setMouseTracking(false);
}

void CGLBaseView::initializeGL()
{	
	// Initialize the opengl data
	m_fTranslate[0] = m_fTranslate[1] = .0; // Translation in xy plane
	m_fScale = 1.0;

	return;
}

void CGLBaseView::paintGL()
{
	Render();
}

void CGLBaseView::Render()
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
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Render the focused frame
	if (_bHasFocus){
		RenderActiveFrame();
	}

	// Render all the renderers inserted into the window,  
	// e.g., renderer of active command; renderer of current key frame.
	CBaseWindow::Render();

	swapBuffers();

	// Stop rendering
	EndScene();
}

void CGLBaseView::RenderActiveFrame()
{
	GLint viewport[4], cx, cy;
	glGetIntegerv(GL_VIEWPORT, viewport);

	cx = viewport[2] * .5;
	cy = viewport[3] * .5;

	glMatrixMode(GL_PROJECTION);						
	glPushMatrix();

	glLoadIdentity();									
	glOrtho(-cx, cx, -cy, cy, .1, 20);

	// Draw the border frame
	glColor3f(.0, 1.0, 1.0);
	glLineWidth(3);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBegin(GL_LINES);
	glVertex3f(-cx, cy, -.12);
	glVertex3f(cx, cy, -.12);
	glVertex3f(cx, cy, -.12);
	glVertex3f(cx, -cy, -.12);
	glVertex3f(cx, -cy, -.12);
	glVertex3f(-cx, -cy, -.12);
	glVertex3f(-cx, -cy, -.12);
	glVertex3f(-cx, cy, -.12);
	glEnd();

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}

void CGLBaseView::BeginScene()
{
	// Make this widget the current widget for opengl operation, i.e., makes the widget's rendering context
	// the current opengl  rendering context
	makeCurrent();

	return;
}

void CGLBaseView::SetProjection()
{
	// Set the opengl projection matrix
	GLsizei width, height;
	GLfloat aspect ;
	double scale = m_fScale;
	
	width = QWidget::width();
	height = QWidget::height();

	if (height == 0)									// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}
	aspect = (GLfloat)width/(GLfloat)height;

	glViewport(0, 0, width, height);					// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// glFrustum(-1.0 * aspect , 1.0f * aspect	, -1.0f, 1.0f, 3.0f, 100.0f);  
	// gluPerspective(45, (double)_cx / (double)_cy, .1, 20);
	double fHalfHeight = (height*.5)/(scale);
	glOrtho(-fHalfHeight*aspect, fHalfHeight*aspect, -fHalfHeight, fHalfHeight, -1, 1);

	m_viewport[0] = m_viewport[1] = 0;
	m_viewport[2] = width;
	m_viewport[3] = height;
	m_fScreenSize[0] = 2*fHalfHeight*aspect;
	m_fScreenSize[1] = 2*fHalfHeight;

	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
}

void CGLBaseView::TransformScene()
{
	// Transform the scene
	// Initialize model view 
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix

	// Translate and rotate
	// Actually it will only rotate by z axis in 2D case
	glTranslatef(m_fTranslate[0], m_fTranslate[1], .0f);

	if(m_listRotateAngle.size() > 0){
		std::list<double>::iterator itAngle = m_listRotateAngle.end();
		std::list<int>::iterator itAxis = m_listRotateAxis.end();
		int i = 0;

		for(i = 0; i < m_listRotateAngle.size(); i ++){
			itAngle --;
			itAxis --;

			double angle = *itAngle;
			int axis = *itAxis;

			if(axis == 0){
				glRotatef(angle, 1.0f, .0f, .0f); // Rotate by x
			}
			else if(axis == 1){
				glRotatef(angle, .0f, 1.0f, .0f); // Rotate by y
			}
			else{
				glRotatef(angle, .0f, .0f, 1.0f); // Rotate by z
			}
		}
	}
	
	return;
}

void CGLBaseView::EndScene()
{
	// make context to null
	doneCurrent();
}

void CGLBaseView::BeginPicking(const QPoint& p, double size)
{
	BeginScene();
	SetProjection();

	GLfloat pMatrix[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pMatrix);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix(p.x(), viewport[3] - p.y(), size, size, viewport);
	glMultMatrixf(pMatrix);

	// Initialize the render mode to select mode
	glSelectBuffer(1024, m_pickbuf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(30000);

	glMatrixMode( GL_MODELVIEW ); 
	TransformScene();
}

int CGLBaseView::EndPicking()
{
	int i;

	int hits = glRenderMode(GL_RENDER);

	// Pick
	int selected = -1, names;
	GLuint *ptr = m_pickbuf;
	GLuint min = 0xffffffff;
	for (i = 0; i < hits; i++) {
		names = *ptr++;
		GLuint z = *ptr;  //int
		ptr++;
		ptr++;
		//for (j = 0; j < names; j++) {

		if (z < min) {
			min = z;
			selected = (int)*ptr;
		}
		ptr++;
		//}
	}

	return selected;
}

void CGLBaseView::PickPoint(const QPoint&p, double sizeWidth, double sizeHeight, const varray<Vec2>& arrayPoint, varray<int>& arraySelPoint)
{
	if(sizeWidth < 1){
		sizeWidth = 1;
	}
	if(sizeHeight < 1){
		sizeHeight = 1;
	}
	
	// Begin picking
	BeginScene();
	SetProjection();

	GLfloat pMatrix[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pMatrix);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix(p.x(), viewport[3] - p.y(), sizeWidth, sizeHeight, viewport);
	glMultMatrixf(pMatrix);

	// Initialize the render mode to select mode
	glSelectBuffer(1024, m_pickbuf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(30000);

	glMatrixMode( GL_MODELVIEW ); 
	TransformScene();

	// Render points
	int i = 0, nPoints = arrayPoint.size();

	for(; i < nPoints; i ++){
		const Vec2& point = arrayPoint[i];
		glLoadName(i);

		glBegin(GL_POINTS);
		glVertex2f(point.x(), point.y());
		glEnd();

	}

	// Picking
	arraySelPoint.clear();
	int hits = glRenderMode(GL_RENDER);
	GLuint min = 0xffffffff;
	for (i = 0; i < hits; i++) {
		GLuint iPickPoint = m_pickbuf[4*i+3];
		arraySelPoint.push_back(iPickPoint);
	}

	return;
}

void CGLBaseView::PickPoint(const QPoint& p, double size, const varray<Vec2>& arrayPoint, varray<int>& arraySelPoint)
{
	// Given a position on the screen, pick points from an array of points

	// Prepare picking
	BeginPicking(p, size);

	// Render points
	int i = 0, nPoints = arrayPoint.size();

	for(; i < nPoints; i ++){
		const Vec2& point = arrayPoint[i];
		glLoadName(i);

		glBegin(GL_POINTS);
		glVertex2f(point.x(), point.y());
		glEnd();

	}

	// Picking
	arraySelPoint.clear();
	int hits = glRenderMode(GL_RENDER);
	GLuint min = 0xffffffff;
	for (i = 0; i < hits; i++) {
		GLuint iPickPoint = m_pickbuf[4*i+3];
		arraySelPoint.push_back(iPickPoint);
	}

	return;
}

void CGLBaseView::PickPoint(const QPoint& p, double size, const std::list<Vec2>& listPoint, varray<int>& arraySelPoint)
{
	// Given a position on the screen, pick points from an array of points

	// Prepare picking
	BeginPicking(p, size);

	// Render points
	int i = 0;

	std::list<Vec2>::const_iterator itPoint = listPoint.begin();
	while(itPoint != listPoint.end()){
		const Vec2& point = *itPoint;
		glLoadName(i);

		glBegin(GL_POINTS);
		glVertex2f(point.x(), point.y());
		glEnd();

		itPoint ++; i ++;
	}

	// Picking
	arraySelPoint.clear();
	int hits = glRenderMode(GL_RENDER);
	GLuint min = 0xffffffff;
	for (i = 0; i < hits; i++) {
		GLuint iPickPoint = m_pickbuf[4*i+3];
		arraySelPoint.push_back(iPickPoint);
	}

	return;
}

void CGLBaseView::PickPoint(const QPoint&p, double sizeWidth, double sizeHeight, const std::list<Vec2>& listPoint, varray<int>& arraySelPoint)
{
	if(sizeWidth < 1){
		sizeWidth = 1;
	}
	if(sizeHeight < 1){
		sizeHeight = 1;
	}

	// Begin picking
	BeginScene();
	SetProjection();

	GLfloat pMatrix[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pMatrix);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix(p.x(), viewport[3] - p.y(), sizeWidth, sizeHeight, viewport);
	glMultMatrixf(pMatrix);

	// Initialize the render mode to select mode
	glSelectBuffer(1024, m_pickbuf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(30000);

	glMatrixMode( GL_MODELVIEW ); 
	TransformScene();

	// Render points
	int i = 0;

	std::list<Vec2>::const_iterator itPoint = listPoint.begin();
	while(itPoint != listPoint.end()){
		const Vec2& point = *itPoint;
		glLoadName(i);

		glBegin(GL_POINTS);
		glVertex2f(point.x(), point.y());
		glEnd();

		itPoint ++; i ++;
	}

	// Picking
	arraySelPoint.clear();
	int hits = glRenderMode(GL_RENDER);
	GLuint min = 0xffffffff;
	for (i = 0; i < hits; i++) {
		GLuint iPickPoint = m_pickbuf[4*i+3];
		arraySelPoint.push_back(iPickPoint);
	}

	return;
}

bool CGLBaseView::PickPolygon(const QPoint&p, double sizeWidth, double sizeHeight, const varray<Vec2>& arrayPolygonVertex, bool bClose /* = true */)
{
	// Begin picking
	BeginScene();
	SetProjection();

	GLfloat pMatrix[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pMatrix);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix(p.x(), viewport[3] - p.y(), sizeWidth, sizeHeight, viewport);
	glMultMatrixf(pMatrix);

	// Initialize the render mode to select mode
	glSelectBuffer(1024, m_pickbuf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(30000);

	glMatrixMode( GL_MODELVIEW ); 
	TransformScene();

	// Render polygon
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glLoadName(0);

	if(bClose)
		glBegin(GL_LINE_LOOP);
	else
		glBegin(GL_LINE_STRIP);

	int nPoint = arrayPolygonVertex.size(), iPoint = 0;
	for(; iPoint < nPoint; iPoint ++){
		const Vec2& point = arrayPolygonVertex[iPoint];
		glVertex2f(point.x(), point.y());	
	}

	glEnd();

	// Picking
	int hits = glRenderMode(GL_RENDER);

	if(hits > 0)
		return true;
	else
		return false;
}

void CGLBaseView::PickTriangle(const QPoint&p, double size, const CMesh *pMesh, varray<int>& arraySelTriangle)
{
	// Begin picking
	BeginScene();
	SetProjection();

	GLfloat pMatrix[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pMatrix);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix(p.x(), viewport[3] - p.y(), size, size, viewport);
	glMultMatrixf(pMatrix);

	// Initialize the render mode to select mode
	glSelectBuffer(1024, m_pickbuf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(30000);

	glMatrixMode( GL_MODELVIEW ); 
	TransformScene();

	// Render triangles
	int nFace = pMesh->GetFSize(), iFace = 0;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for(; iFace < nFace; iFace ++){
		glLoadName(iFace);
		const CFace &face = pMesh->GetF(iFace);
		const Vec3 &v1 = pMesh->GetV(face.p(0)).Pos(), &v2 = pMesh->GetV(face.p(1)).Pos(), &v3 = pMesh->GetV(face.p(2)).Pos();

		glBegin(GL_TRIANGLES);
			glVertex2d(v1.x(), v1.y());
			glVertex2d(v2.x(), v2.y());
			glVertex2d(v3.x(), v3.y());
		glEnd();
	}


	// Picking
	arraySelTriangle.clear();
	int hits = glRenderMode(GL_RENDER);
	GLuint min = 0xffffffff;
	int i = 0;
	for (; i < hits; i++) {
		GLuint iPickTriangle = m_pickbuf[4*i+3];
		arraySelTriangle.push_back(iPickTriangle);
	}

	return;
}

Vec3 CGLBaseView::UnProjectPoint(const Vec3& screenpoint)
{
	// Set render dc and initialize the model view matrix
	BeginScene();

	// Initialize the projection matrix
	SetProjection();

	// Transform the scene to desired location
	TransformScene();

	double modelMatrix[16], projMatrix[16];
	int viewport[4];
	double objx,objy, objz;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluUnProject(screenpoint.x(), viewport[3]-screenpoint.y(), screenpoint.z(),
		modelMatrix, projMatrix, viewport, &objx, &objy, &objz);

	// Set render dc to NULL
	EndScene();

	return Vec3(objx, objy, objz);
}

Vec3 CGLBaseView::UnProjectPointForRender(const Vec3& screenpoint)
{
	double modelMatrix[16], projMatrix[16];
	int viewport[4];
	double objx,objy, objz;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluUnProject(screenpoint.x(), viewport[3]-screenpoint.y(), screenpoint.z(),
		modelMatrix, projMatrix, viewport, &objx, &objy, &objz);

	return Vec3(objx, objy, objz);
}

Vec3 CGLBaseView::ProjectPoint(const Vec3& spacepoint)
{
	// Set render dc and initialize the model view matrix
	BeginScene();

	// Initialize the projection matrix
	SetProjection();

	// Transform the scene to desired location
	TransformScene();

	double modelMatrix[16], projMatrix[16];
	int viewport[4];
	double screenx,screeny, screenz;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluProject(spacepoint.x(), spacepoint.y(), spacepoint.z(), modelMatrix, projMatrix, viewport, 
		&screenx, &screeny, &screenz);

	screeny = viewport[3]-screeny;

	// Set render dc to NULL
	EndScene();

	return Vec3(screenx, screeny, screenz);
}

Vec3 CGLBaseView::ProjectPointForRender(const Vec3& spacepoint)
{
	double modelMatrix[16], projMatrix[16];
	int viewport[4];
	double screenx,screeny, screenz;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluProject(spacepoint.x(), spacepoint.y(), spacepoint.z(), modelMatrix, projMatrix, viewport, 
		&screenx, &screeny, &screenz);

	screeny = viewport[3]-screeny;

	return Vec3(screenx, screeny, screenz);
}

void CGLBaseView::mousePressEvent( QMouseEvent * event )
{
	if(event->button() == Qt::LeftButton){
		mouseLButtonDown(event);
	}
	else if(event->button() == Qt::RightButton){
		mouseRButtonDown(event);
	}

	QWidget::mousePressEvent(event);

	return;
}

void CGLBaseView::mouseLButtonDown(QMouseEvent * event )
{
	m_bLButtonDown = true;
	m_lastPointPos = event->pos();

	// Pass to alive commands
	if( !GetTransformState() && ! event->modifiers().testFlag(Qt::AltModifier) ){
		// Transform 

		CMsg msg;
		msg._pWnd = this;
		msg._msgType = CMsg::LBUTTONDOWN;

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		// Transform the view coordinate to model coordinate in opengl coordinate with z = 0
		Vec3 unProjectPoint = UnProjectPoint(Vec3(event->pos().x(), event->pos().y(), 0));
		msg._point.Set(unProjectPoint.x(), unProjectPoint.y());

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);
	}

	return;
}

void CGLBaseView::mouseRButtonDown(QMouseEvent * event )
{
	m_bRButtonDown = true;
	m_lastPointPos = event->pos();

	// Pass to alive commands
	if(!GetTransformState() && ! event->modifiers().testFlag(Qt::AltModifier) ){
		CMsg msg;
		msg._pWnd = this;
		msg._msgType = CMsg::RBUTTONDOWN;

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		// Transform the view coordinate to model coordinate in opengl coordinate with z = 0
		Vec3 unProjectPoint = UnProjectPoint(Vec3(event->pos().x(), event->pos().y(), 0));
		msg._point.Set(unProjectPoint.x(), unProjectPoint.y());

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);
	}

	return;
}

void CGLBaseView::mouseReleaseEvent( QMouseEvent * event )
{
	if(event->button() == Qt::LeftButton){
		mouseLButtonUp(event);
	}
	else if(event->button() == Qt::RightButton){
		mouseRButtonUp(event);
	}

	QWidget::mouseReleaseEvent(event);

	return;
}

void CGLBaseView::mouseLButtonUp(QMouseEvent * event)
{
	// Pass to alive commands
	if(!GetTransformState() && ! event->modifiers().testFlag(Qt::AltModifier) ){
		CMsg msg;
		msg._pWnd= this;
		msg._msgType = CMsg::LBUTTONUP;

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		// Transform the view coordinate to model coordinate in opengl coordinate with z = 0
		Vec3 unProjectPoint = UnProjectPoint(Vec3(event->pos().x(), event->pos().y(), 0));
		msg._point.Set(unProjectPoint.x(), unProjectPoint.y());

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);
	}

	m_bLButtonDown = false;

	return;
}

void CGLBaseView::mouseRButtonUp(QMouseEvent * event)
{
	// Pass to alive commands
	if(!GetTransformState() && ! event->modifiers().testFlag(Qt::AltModifier) ){
		CMsg msg;
		msg._pWnd = this;
		msg._msgType = CMsg::RBUTTONUP;

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		// Transform the view coordinate to model coordinate in opengl coordinate with z = 0
		Vec3 unProjectPoint = UnProjectPoint(Vec3(event->pos().x(), event->pos().y(), 0));
		msg._point.Set(unProjectPoint.x(), unProjectPoint.y());

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);
	}

	m_bRButtonDown = false;

	return;
}

void CGLBaseView::mouseMoveEvent ( QMouseEvent * event )
{
	if(GetTransformState() || event->modifiers().testFlag(Qt::AltModifier) ){
		double dx = double(event->pos().x() - m_lastPointPos.x()), dy = double(event->pos().y() - m_lastPointPos.y());
		if(m_viewport[2] == 0){
			m_viewport[2] = 1;
		}
		if(m_viewport[3] == 0){
			m_viewport[3] = 1;
		}
		if(m_bLButtonDown){ // Rotate
			dx /= 5.f;
			dy /= 5.f;
			if(fabs(dx) > fabs(dy) ){
				m_listRotateAngle.push_back(-dx);
				m_listRotateAxis.push_back(2);
			}
			else{
				m_listRotateAngle.push_back(-dy);
				m_listRotateAxis.push_back(2);
			}

			// Render scene
			Render();
		}	
		else if(m_bRButtonDown){ // Translate
			dx = dx / (double)m_viewport[2] * m_fScreenSize[0];
			dy = -dy / (double)m_viewport[3] * m_fScreenSize[1];
			m_fTranslate[0] += dx;
			m_fTranslate[1] += dy;

			// Render scene
			Render();
		}
		else if(m_bMButtonDown){ // Scale
			dx /= 10.f;
			dy /= 10.f;
			if(dx > 0){
				m_fScale += fabs(dx*dy * .01);
			}
			else{
				m_fScale -= fabs(dx*dy * .01);
			}

			if(m_fScale <= 0){
				m_fScale = .01;
			}
			else if(m_fScale > 20){
				m_fScale = 20; // To avoid the unit object exceeds frustum in z direction
				// for solving depth test or polygon offset problem
				// The z range is about [-25, 25]
			}

			// Render scene
			Render();
		}
	}
	else{
		// Pass to alive commands
		CMsg msg;
		msg._pWnd = this;
		msg._msgType = CMsg::MOUSEMOVE;

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		// Transform the view coordinate to model coordinate in opengl coordinate with z = 0
		Vec3 unProjectPoint = UnProjectPoint(Vec3(event->pos().x(), event->pos().y(), 0));
		msg._point.Set(unProjectPoint.x(), unProjectPoint.y());

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);
	}

	m_lastPointPos = event->pos();

	QWidget::mouseMoveEvent(event);

	return;
}

void CGLBaseView::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Space){
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
	else{
		// Pass to alive commands
		CMsg msg;
		msg._pWnd = (this);
		msg._msgType = CMsg::KEY_DOWN;
		msg._nChar = event->key();

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);
	}

	QWidget::keyPressEvent(event);
}

void CGLBaseView::wheelEvent(QWheelEvent *event)
{
	if(!GetTransformState() && !event->modifiers().testFlag(Qt::AltModifier) ){
		// Pass to alive commands
		CMsg msg;
		msg._pWnd= this;
		msg._msgType = CMsg::MOUSEWHEEL;
		msg._zDelta = event->delta();

		msg._bCtlPress = event->modifiers().testFlag(Qt::ControlModifier);
		msg._bAltPress = event->modifiers().testFlag(Qt::AltModifier);
		msg._bShiftPress = event->modifiers().testFlag(Qt::ShiftModifier);

		// Transform the view coordinate to model coordinate in opengl coordinate with z = 0
		Vec3 unProjectPoint = UnProjectPoint(Vec3(event->pos().x(), event->pos().y(), 0));
		msg._point.Set(unProjectPoint.x(), unProjectPoint.y());

		CObjectManagerFactory::GetCommandManager().PassMsgToLiveCmd(msg);
	}
	else{
		// Scale
		m_fScale -= (event->delta()) / 3000.0f;;
		if(m_fScale <= 0){
			m_fScale = .01;
		}
		else if(m_fScale > 25){
			m_fScale = 25; // To avoid the unit object exceeds frustum in z direction
			// for solving depth test or polygon offset problem
			// The z range is about [-25, 25]
		}

		Render();
	}

	QWidget::wheelEvent(event);
}

void CGLBaseView::focusOutEvent( QFocusEvent * event )
{
	if(_bHasFocus){
		this->setMouseTracking(false);
		_bHasFocus = false;
		Render();
	}
}

void CGLBaseView::focusInEvent( QFocusEvent * event )
{
	if(!_bHasFocus){
		this->setMouseTracking(true);
		_bHasFocus = true;
		Render();
	}
}

void CGLBaseView::ResetTransform()
{
	m_fTranslate[0] = m_fTranslate[1] = .0f;
	m_listRotateAxis.clear();
	m_listRotateAngle.clear();
	m_fScale = 1.f;
}

double CGLBaseView::GetTransformScale()
{
	return m_fScale;
}

void CGLBaseView::SaveViewContentToImage(const QString &filePath)
{
	BeginScene();

	GLint viewPort[4] = {0}; 
	glGetIntegerv(GL_VIEWPORT, viewPort); 

	QImage *pSaveImage;
	pSaveImage = new QImage(viewPort[2], viewPort[3], QImage::Format_RGB888);
	
	glReadPixels(viewPort[0], viewPort[1], viewPort[2], viewPort[3], GL_RGB, GL_UNSIGNED_BYTE, pSaveImage->bits()); 

	QImage mirImage = pSaveImage->mirrored(false, true);
	
	mirImage.save(filePath);

	delete pSaveImage;

	EndScene();
}