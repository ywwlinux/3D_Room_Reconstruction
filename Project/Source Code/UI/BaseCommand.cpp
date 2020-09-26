
#include "BaseCommand.h"
#include "CommandManager.h"
#include "../scene/ObjectManagerFactory.h" // For manager factory

// -----------------------------------------------------------------------
// Implementation of CCmdBase

void CCmdBase::BeginCmd()
{ 
	ModifyOtherCmdState(); 
	SetTransformState(false);

	_bIsAlive = true; 
	CObjectManagerFactory::GetCommandManager().AddToLiveList(this);

	BeginCmdImplement();
}

void CCmdBase::EndCmd()
{
	_bIsAlive = false; 
	EndCmdImplement(); 

	CObjectManagerFactory::GetCommandManager().DelLiveCmd(this);

	if(CObjectManagerFactory::GetCommandManager().GetNumberofLiveCommand() == 0){
		SetTransformState(true);
	}
}

void CCmdBase::ModifyOtherCmdState()
{
	// Default: stop alive cmds before a new command
	// It can be overrided by specific command
	CObjectManagerFactory::GetCommandManager().StopAllLiveCmd();

	return;
}

void CCmdBase::Print(const QString& info)
{
	// Insert the information and re-output the entire information
	QStringList newInfoList;

	newInfoList = info.split("\n");
	if(newInfoList.back().isEmpty()){
		newInfoList.pop_back();
	} 

	_printInfo.append(newInfoList);

	// Leave only 20 informations to be displayed
	const int nLeftInfo = 20;
	int nListSize = _printInfo.size();
	
	if(nListSize > nLeftInfo){
		int iLeftPos = nListSize - nLeftInfo;
		_printInfo = _printInfo.mid(iLeftPos, nLeftInfo);
	}

	_isDebugInfoDirt = true;
}

const QStringList& CCmdBase::GetDebugInfo() 
{
	_isDebugInfoDirt = false;

	return _printInfo;
}

bool CCmdBase::IsDebugInfoDirt()
{
	return _isDebugInfoDirt;
}

QStringList CCmdBase::_printInfo; 
bool CCmdBase::_isDebugInfoDirt = false;