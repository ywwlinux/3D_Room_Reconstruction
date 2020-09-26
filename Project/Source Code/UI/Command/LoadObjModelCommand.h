/*******************************************************************
* Author	: wwyang
* Date		: 2019.12.27
* Copyright : Zhejiang Gongshang University
* Head File :
* Version   : 1.0
*********************************************************************/
// .NAME CLoadObjModelCommand -- command for loading a 3D model from an .obj file

// .SECTION Description
// This command is used for loading a 3d mesh from an .obj file

// .SECTION See also
// CCmdBase

#ifndef _LOADOBJMODELCOMMAND_H_
#define _LOADOBJMODELCOMMAND_H_

#include "../BaseCommand.h"

class CLoadObjModelCommand : public CCmdBase
{
public:
	CLoadObjModelCommand() { }
	~CLoadObjModelCommand() {}

	void Render(CBaseWindow *pWindow) {} // do nothing
	void HandleMsg(CMsg& msg) {} // do nothing

								 // Description:
								 // Redo/undo this command
	void Redo(CCmdLogBase *pCmdLog) {}
	void Undo(CCmdLogBase *pCmdLog) {}

protected:
	void BeginCmdImplement();
	void EndCmdImplement() {} // do nothing
};

#endif // !_LOADOBJMODELCOMMAND_H_

