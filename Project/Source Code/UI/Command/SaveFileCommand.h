/*******************************************************************
* Author	: wwyang
* Date		: 2019.12.27
* Copyright : Zhejiang Gongshang University
* Head File :
* Version   : 1.0
*********************************************************************/
// .NAME CSaveFileCommand -- command for saving the calibration configuration file

// .SECTION Description
// This command is used for saving the process of camera calibration vertification into a configuration file. 
// In the configuration, things are included such as camera calibration, phots, models, and user transformation for the model 


// .SECTION See also
// CCmdBase

#ifndef _SAVEFILECOMMAND_H_
#define _SAVEFILECOMMAND_H_

#include "../BaseCommand.h"

#include <QXmlStreamWriter>

class CSaveFileCommand : public CCmdBase
{
public:
	CSaveFileCommand() { _saveMode = SAVE; }
	~CSaveFileCommand() {}

	void Render(CBaseWindow *pWindow) {} // do nothing
	void HandleMsg(CMsg& msg) {} // do nothing

								 // Description:
								 // Redo/undo this command
	void Redo(CCmdLogBase *pCmdLog) {}
	void Undo(CCmdLogBase *pCmdLog) {}

	// Description:
	// Set the mode for save operation
	enum E_SaveMode {SAVE, SAVEAS};
	void SetSaveMode(E_SaveMode saveMode)
	{
		_saveMode = saveMode;
	}

protected:
	void BeginCmdImplement();
	void EndCmdImplement() {} // do nothing

	void WriteCameraCalibration();
	void WriteModelCapture();

private:
	E_SaveMode _saveMode;

	QXmlStreamWriter _xmlWriter;
};

#endif // !_OPENFILECOMMAND_H_


