#include "MainWindow.h"

#include <QFrame>
#include <QGridLayout>
#include <QAction>
#include <QMenuBar>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QLabel>
#include <QPushButton>

#include "Source Code/Scene/ObjectManagerFactory.h"

CMainWindow::CMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	// (1) Initialize main window
	const int minw = 800, minh = 600;
	resize(minw, minh);

	setWindowIcon(QIcon(":/Resources/images/window_icon.png"));
	setWindowTitle(tr("Toolkit for Camera Calibration Vertification"));

	// (2) Create UI layout
	QFrame *pCentralWidget = new QFrame(this);
	QGridLayout *pLayout = new QGridLayout;

	pCentralWidget->setFrameShape(QFrame::StyledPanel);
	pCentralWidget->setLayout(pLayout);

	pLayout->setContentsMargins(1, 0, 1, 0);
	pLayout->setSpacing(0);

	_pSubWnd_CameraList = new QWidget(pCentralWidget);
	auto pSubWnd_CameraShow = new QWidget(pCentralWidget);

	pLayout->addWidget(_pSubWnd_CameraList, 0, 0);
	pLayout->addWidget(pSubWnd_CameraShow, 0, 1);

	pLayout->setColumnStretch(0, 30);
	pLayout->setColumnStretch(1, 70);

	// split further the sub-window of camerashow into 3 sub-windows
	QVBoxLayout *pCameraShowLayout = new QVBoxLayout;

	pSubWnd_CameraShow->setLayout(pCameraShowLayout);

	pCameraShowLayout->setContentsMargins(1, 1, 1, 1);
	pCameraShowLayout->setSpacing(0);

	_pSubWnd_CameraMain = new QWidget(pSubWnd_CameraShow);
	auto pSubWnd_CameraSide = new QWidget(pSubWnd_CameraShow);

	pCameraShowLayout->addWidget(pSubWnd_CameraSide, 45);
	pCameraShowLayout->addWidget(_pSubWnd_CameraMain, 55);

	QHBoxLayout *pCameraSideLayout = new QHBoxLayout;

	pSubWnd_CameraSide->setLayout(pCameraSideLayout);

	pCameraSideLayout->setContentsMargins(1, 1, 1, 1);
	pCameraSideLayout->setSpacing(0);

	_pSubWnd_CameraShowOne = new QWidget(pSubWnd_CameraSide);
	_pSubWnd_CameraShowTwo = new QWidget(pSubWnd_CameraSide);

	pCameraSideLayout->addWidget(_pSubWnd_CameraShowOne);
	pCameraSideLayout->addWidget(_pSubWnd_CameraShowTwo);

	// Create the widgets in the sub-windows
	CreateWidgetsInSubWnd_CameraMain();
	CreateWidgetsInSubWnd_CameraShowOne();
	CreateWidgetsInSubWnd_CameraShowTwo();
	CreateWidgetsInSubWnd_CameraList();

	this->setContentsMargins(3, 3, 3, 3);
	setCentralWidget(pCentralWidget);

	// (3) Pass window handles to the related objects
	CObjectManagerFactory::GetCommandManager().PassMainWindowHandle(this);
	CObjectManagerFactory::GetTextureManager().SetMainGLWidget(_pCameraView_Main);

	// (4) Create UI sub-elements	
	CreateAction(); 		
	CreateMenus();
	AddRenders();

	return;
}

void CMainWindow::CreateWidgetsInSubWnd_CameraList()
{
	// Add a view for this sub-window to render all the cameras 
	QVBoxLayout *pLayout;

	pLayout = new QVBoxLayout;
	pLayout->setContentsMargins(2, 2, 2, 2);
	_pSubWnd_CameraList->setLayout(pLayout);

	QLabel *pLabel = new QLabel(tr("Camera Array"), _pSubWnd_CameraList);
	pLabel->setAlignment(Qt::AlignCenter);

	//QLabel *pCameraList = new QLabel(tr("Render all cameras here"), _pSubWnd_CameraList);

	QWidget *pListSubWnd = new QWidget(_pSubWnd_CameraList);
	QVBoxLayout *pListLayout = new QVBoxLayout;
	QPushButton *pUp, *pDown;

	pListLayout->setContentsMargins(2, 2, 2, 2);
	pListSubWnd->setLayout(pListLayout);

	pUp = new QPushButton(pListSubWnd);
	pDown = new QPushButton(pListSubWnd);
	_pCameraView_List = new CGLAllCameraView(pListSubWnd, _pCameraView_Main); // texture shared with view-camera main

	pUp->setText(tr("UP"));
	pDown->setText(tr("DOWN"));

	connect(pUp, SIGNAL(clicked()), this, SLOT(commandScrollUp()));
	connect(pDown, SIGNAL(clicked()), this, SLOT(commandScrollDown()));

	pListLayout->addWidget(pUp, 5);
	pListLayout->addWidget(_pCameraView_List, 90);
	pListLayout->addWidget(pDown, 5);

	pLayout->addWidget(pLabel, 5);
	pLayout->addWidget(pListSubWnd, 95);

	return;
}

void CMainWindow::CreateWidgetsInSubWnd_CameraMain()
{
	// Add a camera view for this sub-window to render the main camera
	QVBoxLayout *pLayout;

	pLayout = new QVBoxLayout;
	pLayout->setContentsMargins(2, 2, 2, 2);
	_pSubWnd_CameraMain->setLayout(pLayout);

	_pCameraView_Main = new CGLCameraView(_pSubWnd_CameraMain); 

	pLayout->addWidget(_pCameraView_Main);
}

void CMainWindow::CreateWidgetsInSubWnd_CameraShowOne()
{
	// Add a camera view for this sub-window to render the a secondary camera
	QVBoxLayout *pLayout;

	pLayout = new QVBoxLayout;
	pLayout->setContentsMargins(2, 2, 2, 2);
	_pSubWnd_CameraShowOne->setLayout(pLayout);

	// add a selection box
	QLabel *pLabel = new QLabel(tr("Select Camera..."), _pSubWnd_CameraShowOne);

	_pCameraView_ShowOne = new CGLCameraView(_pSubWnd_CameraShowOne, _pCameraView_Main); // texture shared with view-camera main

	pLayout->addWidget(pLabel, 10);
	pLayout->addWidget(_pCameraView_ShowOne, 90);
}

void CMainWindow::CreateWidgetsInSubWnd_CameraShowTwo()
{
	// Add a camera view for this sub-window to render the a secondary camera
	QVBoxLayout *pLayout;

	pLayout = new QVBoxLayout;
	pLayout->setContentsMargins(2, 2, 2, 2);
	_pSubWnd_CameraShowTwo->setLayout(pLayout);

	// add a selection box
	QLabel *pLabel = new QLabel(tr("Select Camera..."), _pSubWnd_CameraShowTwo);

	_pCameraView_ShowTwo = new CGLCameraView(_pSubWnd_CameraShowTwo, _pCameraView_Main); // texture shared with view-camera main

	pLayout->addWidget(pLabel, 10);
	pLayout->addWidget(_pCameraView_ShowTwo, 90);
}

void CMainWindow::CreateAction()
{
	newFileAct = new QAction(tr("&New"), this);
	newFileAct->setShortcuts(QKeySequence::New);
	newFileAct->setStatusTip(tr("Create a new file."));
	connect(newFileAct, SIGNAL(triggered()), this, SLOT(newFile()));

	openFileAct = new QAction(tr("&Open"), this);
	openFileAct->setShortcuts(QKeySequence::Open);
	openFileAct->setStatusTip(tr("Open an existing file."));
	connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));

	saveFileAct = new QAction(tr("&Save"), this);
	saveFileAct->setShortcuts(QKeySequence::Save);
	saveFileAct->setStatusTip(tr("Save the document to disk."));
	connect(saveFileAct, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(saveFileAct, SIGNAL(hovered()), this, SLOT(updateSaveFile()));

	closeAct = new QAction(tr("&Exit"), this);
	closeAct->setShortcut(QKeySequence::Close);
	closeAct->setStatusTip(tr("Close the application."));
	connect(closeAct, SIGNAL(triggered()), this, SLOT(close()));

	_pActInputCameraCalibration = new  QAction(tr("&Calibrations"), this);
	connect(_pActInputCameraCalibration, SIGNAL(triggered()), this, SLOT(commandInputCameraCalibration()));
	connect(_pActInputCameraCalibration, SIGNAL(hovered()), this, SLOT(updateCommandInputCameraCalibration()));

	_pActInputObjectModel = new  QAction(tr("&Obj Model"), this);
	connect(_pActInputObjectModel, SIGNAL(triggered()), this, SLOT(commandInputObjectModel()));
	connect(_pActInputObjectModel, SIGNAL(hovered()), this, SLOT(updateCommandInputObjectModel()));

}

void CMainWindow::CreateMenus()
{
	QMenu *fileMenu, *toolMenu, *helpMenu;
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newFileAct);
	fileMenu->addSeparator();
	fileMenu->addAction(openFileAct);
	fileMenu->addAction(saveFileAct);
	fileMenu->addSeparator();
	fileMenu->addAction(closeAct);

	toolMenu = menuBar()->addMenu(tr("&Input"));
	toolMenu->addAction(_pActInputCameraCalibration);
	toolMenu->addSeparator();
	toolMenu->addAction(_pActInputObjectModel);

	helpMenu = menuBar()->addMenu(tr("&Help"));
}

void CMainWindow::AddRenders()
{

}

void CMainWindow::resizeEvent(QResizeEvent *event)
{
	UpdateCameraView(ALLVIEW);

	event->accept();
}

void CMainWindow::closeEvent(QCloseEvent *event)
{
	// Important !!!
	// Some objects will call the CObjectmanagerFactory's manager, so
	// we should destroy these objects before the CObjectmanagerFactory is destroyed.
	CObjectManagerFactory::DestroyAllManagers();

	event->accept();
}

void CMainWindow::newFile()
{
	CObjectManagerFactory::DestroyAllManagers();

	UpdateCameraConfig();
}

void CMainWindow::openFile()
{
	// Open a stored calibration configuration file
	CObjectManagerFactory::GetCommandManager().GetOpenFileCommand().BeginCmd();
	CObjectManagerFactory::GetCommandManager().GetOpenFileCommand().EndCmd();

	UpdateCameraConfig();
}

void CMainWindow::saveFile()
{
	// Save the currently verified calibration setup into a configuration file
	CObjectManagerFactory::GetCommandManager().GetSaveFileCommand().SetSaveMode(CSaveFileCommand::SAVE);
	CObjectManagerFactory::GetCommandManager().GetSaveFileCommand().BeginCmd();
	CObjectManagerFactory::GetCommandManager().GetSaveFileCommand().EndCmd();
}

void CMainWindow::updateSaveFile()
{
	saveFileAct->setEnabled(CObjectManagerFactory::GetCameraManager().GetCameras().size() > 0);
}

void CMainWindow::commandInputCameraCalibration()
{
	CObjectManagerFactory::GetCommandManager().GetLoadCameraCalibrationCommand().BeginCmd();
	CObjectManagerFactory::GetCommandManager().GetLoadCameraCalibrationCommand().EndCmd();
	
	UpdateCameraConfig();
}

void CMainWindow::UpdateCameraConfig()
{
	// 1. Associate the cameras to the show views
	CCameraAgent *pMainCamera = 0, *pCameraOne = 0, *pCameraTwo = 0;
	const std::list<CCameraAgent *> &listCamera = CObjectManagerFactory::GetCameraManager().GetCameras();
	auto itCamera = listCamera.begin();

	for (; itCamera != listCamera.end(); itCamera++) {
		CCameraAgent *pCamera = *itCamera;
		if (pCamera->GetName() == "camera_main") {
			pMainCamera = pCamera;
		}
		else {
			if (pCameraOne == 0) {
				pCameraOne = pCamera;
			}
			else if (pCameraTwo == 0) {
				pCameraTwo = pCamera;
			}
		}

		if (pCameraOne != 0 && pCameraTwo != 0 && pMainCamera != 0) break;
	}

	_pCameraView_Main->SetCamera(pMainCamera->GetName());
	_pCameraView_ShowOne->SetCamera(pCameraOne->GetName());
	_pCameraView_ShowTwo->SetCamera(pCameraTwo->GetName());

	UpdateCameraView(ALLVIEW);

	// 2. 
	_pCameraView_List->ResetScrollUp();

	// 3. Update the box of camera selection

}

void CMainWindow::updateCommandInputCameraCalibration()
{

}

void CMainWindow::commandInputObjectModel()
{
	CObjectManagerFactory::GetCommandManager().GetLoadObjModelCommand().BeginCmd();
	CObjectManagerFactory::GetCommandManager().GetLoadObjModelCommand().EndCmd();

	UpdateCameraView(ALLVIEW);
	_pCameraView_List->Render();
}

void CMainWindow::updateCommandInputObjectModel()
{
	_pActInputObjectModel->setEnabled(CObjectManagerFactory::GetCameraManager().GetCameras().size() > 0);
}

void CMainWindow::commandScrollUp()
{
	_pCameraView_List->ScrollUpDown(true);
}

void CMainWindow::commandScrollDown()
{
	_pCameraView_List->ScrollUpDown(false);
}
