#pragma once

#include <QtWidgets/QMainWindow>
#include "Source Code\UI\GLCameraView.h"
#include "Source Code\UI\GLAllCameraView.h"

#include <vector>

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CMainWindow(QWidget *parent = Q_NULLPTR);

protected:
	void closeEvent(QCloseEvent *event);  // release resources of the application
	void resizeEvent(QResizeEvent *event); // re-paint the review when resizing the window

private: // UI elements
	void CreateAction();
	void CreateMenus();
	void AddRenders();

	QAction *newFileAct;
	QAction *openFileAct;
	QAction *saveFileAct;
	QAction *closeAct;

	QAction *_pActInputCameraCalibration;
	QAction *_pActInputObjectModel;

private slots:
	void newFile();
	void openFile();
	void saveFile();
	void updateSaveFile();

	void commandScrollUp();
	void commandScrollDown();

	void commandInputCameraCalibration();
	void updateCommandInputCameraCalibration();
	void commandInputObjectModel();
	void updateCommandInputObjectModel();

private:
	// Widgets for sub-windows
	QWidget *_pSubWnd_CameraList, *_pSubWnd_CameraMain, *_pSubWnd_CameraShowOne, *_pSubWnd_CameraShowTwo;

	void CreateWidgetsInSubWnd_CameraList();
	void CreateWidgetsInSubWnd_CameraMain();
	void CreateWidgetsInSubWnd_CameraShowOne();
	void CreateWidgetsInSubWnd_CameraShowTwo();

public:
	enum E_CameraView{MAIN,SHOW_ONE, SHOW_TWO, ALLVIEW};
	CGLCameraView* GetCameraView(E_CameraView whichOne) const
	{
		switch (whichOne) {
		case MAIN:
			return _pCameraView_Main;
		case SHOW_ONE:
			return _pCameraView_ShowOne;
		case SHOW_TWO:
			return _pCameraView_ShowTwo;
		default:
			return 0;
		}
	}
	CGLAllCameraView* GetCameraListView() const
	{
		return _pCameraView_List;
	}

	void UpdateCameraView(E_CameraView whichOne)
	{
		CGLCameraView *pCamera = GetCameraView(whichOne);

		if (pCamera != 0) {
			pCamera->Render();
		}
		else if (whichOne == ALLVIEW) {
			_pCameraView_Main->Render();
			_pCameraView_ShowOne->Render();
			_pCameraView_ShowTwo->Render();
		}
	}

protected:
	void UpdateCameraConfig();

private:
	// Views for the camera simulation
	CGLCameraView *_pCameraView_Main, *_pCameraView_ShowOne, *_pCameraView_ShowTwo;
	CGLAllCameraView *_pCameraView_List;
};
