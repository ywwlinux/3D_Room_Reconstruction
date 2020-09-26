
#include "CommandManager.h"
#include "assert.h"

#pragma warning(disable: 4267)

// -----------------------------------------------------------------------
// Implementation of command factory

////////////////////////////////////////////////////////////////////////// 
// Transform operation for view

bool g_bTransformSwitch = true;
void SetTransformState(bool bTransformSwitch)
{ g_bTransformSwitch = bTransformSwitch; }
bool GetTransformState()
{ 
	return g_bTransformSwitch;
}

CCommandManager::CCommandManager()
{
	_nMaxLogCommand = 20;
	_pMainWindow = 0;
}

CCommandManager::~CCommandManager()
{
	DestroyManager();
	_pMainWindow = 0;
}

void CCommandManager::PassMainWindowHandle(CMainWindow *pMainWindow)
{
	_pMainWindow = pMainWindow;
}

CMainWindow* CCommandManager::GetMainWindowHandle()
{
	return _pMainWindow;
}

void CCommandManager::AddToLiveList(CCmdBase *pCmd)
{
	if(FindCmdInLiveList(pCmd) == _liveCmdList.end()){
		_liveCmdList.push_back(pCmd);
	}
}

void CCommandManager::DelLiveCmd(CCmdBase *pCmd)
{
	CmdList::iterator find = FindCmdInLiveList(pCmd);
	if(find != _liveCmdList.end()){
		_liveCmdList.erase(find);
	}
}

void CCommandManager::StopAllLiveCmd()
{
	CmdList::iterator it = _liveCmdList.begin(), nextIt;
	int nListSize = _liveCmdList.size();
	for(; it != _liveCmdList.end(); ){
		nextIt = it;
		nextIt ++;
		(*it)->EndCmd();
		it = nextIt;
	}
}

CCommandManager::CmdList::iterator CCommandManager::FindCmdInLiveList(CCmdBase *pCmd)
{
	CmdList::iterator it = _liveCmdList.begin();
	for(; it != _liveCmdList.end(); it ++){
		if(*it == pCmd){
			return it;
		}
	}

	return it;
}

void CCommandManager::PassMsgToLiveCmd(CMsg& msg)
{
	// Note: the active command may be deleted from list during handling
	CmdList::iterator it = _liveCmdList.begin(), nextIt;
	for(; it != _liveCmdList.end();){
		nextIt = it;
		nextIt ++;
		(*it)->HandleMsg(msg);
		it = nextIt;
	}

	return;
}

void CCommandManager::RenderLiveCmd(CBaseWindow *parentWnd)
{
	CmdList::iterator it = _liveCmdList.begin(), nextIt;
	for(; it != _liveCmdList.end();){
		nextIt = it;
		nextIt ++;
		(*it)->Render(parentWnd);
		it = nextIt;
	}
}

int CCommandManager::GetNumberOfLogRedoCommand() const
{
	return _listLogRedoCmd.size();
}

int CCommandManager::GetNumberOfLogUndoCommand() const
{
	return _listLogUndoCmd.size();
}

const std::string& CCommandManager::GetNameOfLogRedoCommand(int iCommand) const
{
	assert(iCommand >= 0 && iCommand < _listLogRedoCmd.size());

	list<CCmdLogBase *>::const_iterator it = _listLogRedoCmd.begin();
	int i = 0;

	while(i != iCommand && it != _listLogRedoCmd.end()){
		it ++;
	}

	return (*it)->GetCommandName();
}

const std::string& CCommandManager::GetNameOfLogUndoCommand(int iCommand) const
{
	assert(iCommand >= 0 && iCommand < _listLogUndoCmd.size());

	list<CCmdLogBase *>::const_iterator it = _listLogUndoCmd.begin();
	int i = 0;

	while(i != iCommand && it != _listLogUndoCmd.end()){
		it ++;
	}

	return (*it)->GetCommandName();
} 

int CCommandManager::GetNumberofLiveCommand() const
{
	return _liveCmdList.size();
}

void CCommandManager::Redo(int nCommand)
{
	list<CCmdLogBase *>::iterator it = _listLogRedoCmd.begin(), it2;
	int i = 0;

	while(i < nCommand && it != _listLogRedoCmd.end()){
		(*it)->Redo();

		it2 = it; it2 ++;	
		PushUndo(*it, false);
		_listLogRedoCmd.erase(it);

		it = it2; i++;
	}

	return;
}

void CCommandManager::Undo(int nCommand)
{
	list<CCmdLogBase *>::iterator it = _listLogUndoCmd.begin(), it2;
	int i = 0;

	while(i < nCommand && it != _listLogUndoCmd.end()){
		(*it)->Undo();

		it2 = it; it2 ++;
		PushRedo(*it);
		_listLogUndoCmd.erase(it);
		
		it = it2; i++;
	}

	return;
}

void CCommandManager::PushRedo(CCmdLogBase *pLogCmd)
{
	_listLogRedoCmd.push_front(pLogCmd);
	if(_listLogRedoCmd.size() > _nMaxLogCommand){
		DeleteRedo(1);
	}
}

void CCommandManager::PushUndo(CCmdLogBase *pLogCmd, bool bClearRedo /* = true */)
{
	_listLogUndoCmd.push_front(pLogCmd);
	if(_listLogUndoCmd.size() > _nMaxLogCommand){
		DeleteUndo(1);
	}

	// Clear redo list for operation's exclusion
	if(bClearRedo)
		DeleteRedo(_listLogRedoCmd.size());
}

void CCommandManager::DeleteRedo(int nDel)
{
	int i = 0;
	for(; i < nDel; i ++){
		delete _listLogRedoCmd.back();
		_listLogRedoCmd.pop_back();
	}

	return;
}

void CCommandManager::DeleteUndo(int nDel)
{
	int i = 0;
	for(; i < nDel; i ++){
		delete _listLogUndoCmd.back();
		_listLogUndoCmd.pop_back();
	}

	return;
}

void CCommandManager::DestroyManager()
{
	StopAllLiveCmd();
	DeleteRedo(_listLogRedoCmd.size());
	DeleteUndo(_listLogUndoCmd.size());
}

