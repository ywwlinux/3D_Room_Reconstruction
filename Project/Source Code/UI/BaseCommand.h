/*******************************************************************
* Author	: wwyang
* Date		: 2007.3.23
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : 
* Version   : 1.0
*********************************************************************/
// .NAME CCmdBase - Super class of a command

// .SECTION Description
// Each command corresponds to an operation, which does something on
// the data and produces something desired.
// One command has three actions done specifically when begin, stop 
// and message comes.
// To put the command into undo/redo scheme, please refer to the document
// of program architecture.
// It should be noted the message is passed to the command only when
// it is alive which is true if it begins but not stops
// If the command changes window content, the influenced windows should be 
// updated.

// .SECTION See also
// CCmdFactory

#ifndef _BASECOMMAND_H_
#define _BASECOMMAND_H_

#include <QStringList>

#include "BaseWindow.h" // For the basic interactable window
#include "../base/Vec2.h" // For vec2
#include <string>

// -----------------------------------------------------------------------
// class of message to be passed to command for handling
class CMsg{
public:
	enum MSG_TYPE{ KEY_DOWN, KEY_UP, LBUTTONDOWN, LBUTTONUP, MBUTTONDOWN, MBUTTONUP, RBUTTONDOWN, RBUTTONUP, MOUSEMOVE, MOUSEWHEEL, CHAR, CUSTOM };

	// message data
public:
	CBaseWindow *_pWnd;  // refer to message-invoked window
	// Note: the _pWnd is dependent on current view class only for
	// its render() function
	MSG_TYPE _msgType;  // message type
	Vec2 _point;      // mouse position
	short _zDelta;      // mouse wheel

	int _nChar, _nRepCnt, _nFlags; // key&char message
	bool _bCtlPress, _bShiftPress, _bAltPress;

public:
	CMsg()
	{
		_bAltPress = _bShiftPress = _bCtlPress = false;
	}
};

class CCmdLogBase;

class CCmdBase{
public:
	// Description:
	// constructor and destructor
	CCmdBase(){}
	virtual ~CCmdBase(){}

public:
	// Description:
	// Begin and end a command. Their behaviors are mainly determined
	// by BeginCmdImplement() and EndCmdImplement();
	void BeginCmd();
	void EndCmd();

	// Description:
	// Handle messages passed to the cmd when it's running
	// Note: only one active view can passed to the active command
	// to avoid message melt together.
	virtual void HandleMsg(CMsg& msg){}

	// Description:
	// Undo/Redo operation
	virtual void Undo(CCmdLogBase *pCmdLog) = 0;
	virtual void Redo(CCmdLogBase *pCmdLog) = 0;

	// Description:
	// Check current command state.
	bool IsAlive() const
	{ return _bIsAlive; }

	// Description:
	// Render the temporary data in the active command operation
	virtual void Render(CBaseWindow *parentWnd){}

protected:
	// Description:
	// When a command is invoked, it should do something for other 
	// currently alive commands in this function.
	// In default, it deactivates currently alive commands   
	virtual void ModifyOtherCmdState();

	// Description:
	// The following functions should be overridden for derived
	// command class to handle its process when it starts or ends 
	virtual void BeginCmdImplement(){}
	virtual void EndCmdImplement(){}

protected:
	bool _bIsAlive;

	// For debugging information
public:
	static void Print(const QString& info);
	static const QStringList& GetDebugInfo();
	static bool IsDebugInfoDirt() ;
private:
	static QStringList _printInfo; 
	static bool _isDebugInfoDirt;
};

using namespace std;
// base class for logging the command, used for redo/undo
class CCmdLogBase{
public:
	CCmdLogBase() : _pLogCmd(NULL){}
	virtual ~CCmdLogBase(){}

	// Description:
	// Set logged command
	void SetLogCommand(CCmdBase* pLogCmd)
	{
		_pLogCmd = pLogCmd;
	}

	// Description:
	// Undo/Redo the logged command
	void Redo()
	{
		_pLogCmd->Redo(this);
	}
	void Undo()
	{
		_pLogCmd->Undo(this);
	}

	// Description:
	// Set/get the name of logged name
	void SetCommandName(const string& name)
	{
		_logCmdName = name;
	}
	const string& GetCommandName() const
	{
		return _logCmdName;
	}

private:
	CCmdBase *_pLogCmd;
	string _logCmdName;
};

#endif