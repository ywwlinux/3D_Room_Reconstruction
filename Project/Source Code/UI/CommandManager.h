/*******************************************************************
* Author	: wwyang
* Date		: 2007.3.31
* Copyright : State key CAD&CG Lab, Group of Animation 
* Version   : 1.0
*********************************************************************/
// .NAME CCommandManager - class for managing all commands

// .SECTION Description
// All commands have an instance in the manager, and have an function 
// to access it.

// .SECTION See also
// CCmdBase CObjectManager

#ifndef _COMMANDMANGER_H_
#define _COMMANDMANGER_H_

#include <list> // For list

#include "BaseWindow.h"
#include "BaseCommand.h" // For base class of command
#include "../Scene/ObjectManager.h"

#include "../../MainWindow.h" // For main window 
#include "Command\LoadCameraCalibrationCommand.h"
#include "Command\LoadObjModelCommand.h"
#include "Command\OpenFileCommand.h"
#include "Command\SaveFileCommand.h"

using namespace std;

void SetTransformState(bool bTransformSwitch);
bool GetTransformState();

class CCommandManager : public CObjectManager{
public:
	typedef std::list<CCmdBase*> CmdList;

	friend class CCmdBase;

public:
	// Description:
	// Constructor and destructor
	CCommandManager();
	~CCommandManager();

	// Description:
	// Destroy the manager -- free all memory
	void DestroyManager();

	// Description:
	// Pass the main window handle
	void PassMainWindowHandle(CMainWindow *pMainWindow);
	CMainWindow* GetMainWindowHandle();

public:
	COpenFileCommand& GetOpenFileCommand()
	{
		return _cmdOpenFile;
	}

	CSaveFileCommand& GetSaveFileCommand()
	{
		return _cmdSaveFile;
	}

	CLoadObjModelCommand& GetLoadObjModelCommand()
	{
		return _cmdLoadObjModel;
	}

	CLoadCameraCalibrationCommand& GetLoadCameraCalibrationCommand()
	{
		return _cmdLoadCameraCalibration;
	}

public:
	// Description:
	// Pass message to alive commands for handling 
	void PassMsgToLiveCmd(CMsg& msg);

	// Description:
	// Render all live commands
	void RenderLiveCmd(CBaseWindow *parentWnd);

	// Description:
	// Stop all alive commands
	void StopAllLiveCmd();

	// Description:
	// Set the max number of operations for redo/undo respectively
	void SetMaxNumberOfLogCommand(int nMaxNumber);

	// Description:
	// Get the number of logged commands
	int GetNumberOfLogRedoCommand() const;
	int GetNumberOfLogUndoCommand() const;

	// Description:
	// Get the name of logged commands
	const std::string& GetNameOfLogRedoCommand(int iCommand) const;
	const std::string& GetNameOfLogUndoCommand(int iCommand) const;

	// Description:
	// Redo/undo operations
	void Redo(int nCommand);
	void Undo(int nCommand);

	// Description:
	// Push an undo/redo operation
	void PushUndo(CCmdLogBase *pLogCmd, bool bClearRedo = true);
	void PushRedo(CCmdLogBase *pLogCmd);

	// Description:
	// Delete the logged operations
	void DeleteRedo(int nDel);
	void DeleteUndo(int nDel);

	// Description:
	// Get number of alive command
	int GetNumberofLiveCommand() const;

private:
	// Description:
	// Manage command instances in the factory. e.g.
	// add, del and find command object in the list
	void AddToLiveList(CCmdBase *pCmd);
	void DelLiveCmd(CCmdBase *pCmd);
	CmdList::iterator FindCmdInLiveList(CCmdBase *pCmd);

private:
	// live commands list
	CmdList _liveCmdList; 
	// Logged commands for redo/undo scheme
	// Note: the most eary command is at the first in redo list 
	list<CCmdLogBase *> _listLogRedoCmd;
	// Note: the most last command is at the first in undo list
	list<CCmdLogBase *> _listLogUndoCmd;
	// Maximum redo/undo operations
	int _nMaxLogCommand;

	// following instances vars are global objects
	// for different commands
	//CLoadCameraBKImage _cmdLoadCameraImage;
	COpenFileCommand _cmdOpenFile;
	CSaveFileCommand _cmdSaveFile;
	CLoadObjModelCommand _cmdLoadObjModel;
	CLoadCameraCalibrationCommand _cmdLoadCameraCalibration;


	// commands for demo figures

	// Pointer to main window
	CMainWindow *_pMainWindow; // used for updating widgets in the main window at the running of command
};

#endif
