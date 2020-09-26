/*******************************************************************
* Author	: wwyang
* Date		: 2019.12.27
* Copyright : Zhejiang Gongshang University
* Head File :
* Version   : 1.0
*********************************************************************/
// .NAME CLoadCameraCalibrationCommand -- command for loading camera calibration information from files

// .SECTION Description
// This command is used for loading camera calibration information where the information is stored in a txt file for each camera. 
// The information in a txt file include the intrinsic and extrinsic parameters of a camera. All the txt files for a set of calibrated cameras
// locate in the same directory and are named in an order so as to match the corresponding photos captured by the cameras 

// .SECTION See also
// CCmdBase

#ifndef _LOADCAMERACALIBRATIONCOMMAND_H_
#define _LOADCAMERACALIBRATIONCOMMAND_H_

#include "../BaseCommand.h"
#include "../../Object/CameraAgent.h"

#include <string>
#include <QString>
#include <unordered_map>

class CLoadCameraCalibrationCommand : public CCmdBase
{
public:
	CLoadCameraCalibrationCommand() { }
	~CLoadCameraCalibrationCommand() {}

	void Render(CBaseWindow *pWindow) {} // do nothing
	void HandleMsg(CMsg& msg) {} // do nothing

	// Description:
	// Redo/undo this command
	void Redo(CCmdLogBase *pCmdLog) {}
	void Undo(CCmdLogBase *pCmdLog) {}

protected:
	void BeginCmdImplement();
	void EndCmdImplement() {} // do nothing

	// Load calibrations of the cameras
	void LoadCameraCalibration(const QString &infoFilePath);
	CCameraAgent* LoadCalibrationForACamera(const QString &fileName);

	// Description:
	// Transform all the cameras to a global coordinate sysem with respect to the main camera
	void TransformToGlobalCoordinateSystem();
	void ComputeGlobalCameraExtrinsicParameter(CCameraAgent *pCamera, CCameraAgent *pMainCamera, std::unordered_map<std::string, CCameraAgent*> &hashCamera);

public:
	void LoadCameraPhotos(const QString &photoFilePath);

private:
	// Utility functions
	bool IsTxtFile(const QString &fileName) const;
	bool IsImageFile(const QString &fileName) const;
	std::string GetFileName(const QString &fileName);

	bool IsIntrinsicParameter(char line[128]);
	bool IsCheckerboardExtrinsicParameter(char line[128], QString &buddyCamera);

	void ConvertLieAlgebraToRotation(const double om[3], double R[9]);
};

#endif // !_LOADCAMERACALIBRATIONCOMMAND_H_
