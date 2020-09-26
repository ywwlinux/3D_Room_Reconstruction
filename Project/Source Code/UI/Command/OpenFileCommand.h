/*******************************************************************
* Author	: wwyang
* Date		: 2019.12.27
* Copyright : Zhejiang Gongshang University
* Head File :
* Version   : 1.0
*********************************************************************/
// .NAME COpenFileCommand -- command for loading the calibration configuration file

// .SECTION Description
// This command is used for loading a configuration file for the camera calibration vertification. 
// In the configuration, things are included such as camera calibration, phots, models, and user transformation for the model 


// .SECTION See also
// CCmdBase

#ifndef _OPENFILECOMMAND_H_
#define _OPENFILECOMMAND_H_

#include "../BaseCommand.h"

#include <QXmlStreamReader>
#include "../../Object/CameraAgent.h"

class COpenFileCommand : public CCmdBase
{
public:
	COpenFileCommand() {}
	~COpenFileCommand() {}

	void Render(CBaseWindow *pWindow) {} // do nothing
	void HandleMsg(CMsg& msg) {} // do nothing

	// Description:
	// Redo/undo this command
	void Redo(CCmdLogBase *pCmdLog) {}
	void Undo(CCmdLogBase *pCmdLog) {}

protected:
	void BeginCmdImplement();
	void EndCmdImplement() {} // do nothing

	void LoadCameraCalibration();
	CCameraAgent* LoadACamera();
	void LoadModelCapture();

private:
	QXmlStreamReader _xmlReader;
};

#endif // !_OPENFILECOMMAND_H_

