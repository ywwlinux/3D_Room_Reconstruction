#include "LoadCameraCalibrationCommand.h"
#include "../../Scene/ObjectManagerFactory.h"

#include <QDir>
#include <QFileDialog>
#include <stdio.h>
#include <unordered_map>
#include <queue>
#include <stack>

#include "opencv2/opencv.hpp"
#include "../../Utility/UtilityTool.h"

void CLoadCameraCalibrationCommand::BeginCmdImplement()
{
	// Load calibration information for a set of calibrated cameras from a directory

	// 1. Specify the directory where the txt files locate
	QString defaultPath = CObjectManagerFactory::GetSceneObjectManager().GetDataDir();

	if (defaultPath.size() == 0)
		defaultPath = QDir::currentPath();

	QString dirPath = QFileDialog::getExistingDirectory(0, QFileDialog::tr("Select the calibration directory"),
		defaultPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (dirPath.size() == 0) return;

	CObjectManagerFactory::GetSceneObjectManager().SetDataDir(dirPath);

	// 2. Read the calibrations of each camera
	LoadCameraCalibration(dirPath);

	// transform all of the cameras into a global coordinate system
	TransformToGlobalCoordinateSystem(); 

	// 3. Read the photo taken by each camera
	QString photoPath = dirPath + "/Photos";
	QDir photoDir;
	bool bExist;

	photoDir.setPath(photoPath);
	bExist = photoDir.exists();
	if (!bExist) {
		photoPath = dirPath + "/photos";
		photoDir.setPath(photoPath);
		bExist = photoDir.exists();
	}

	if (bExist) {
		LoadCameraPhotos(photoPath);
		CObjectManagerFactory::GetSceneObjectManager().SetPhotoDir(photoPath);
	}

	return;
}

void CLoadCameraCalibrationCommand::LoadCameraCalibration(const QString &infoFilePath)
{
	// We assign each camera a name so as to identify it and store its calibration information in a txt file
	// with the same name as the camera.
	QDir txtFileDir(infoFilePath);  
	QStringList fileList = txtFileDir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
	int nFile = fileList.size();
	auto itFile = fileList.begin();

	// Clear the camera agents
	CObjectManagerFactory::GetCameraManager().DestroyManager();

	for (int i = 0; i < nFile; i++, itFile++) {
		QString fileName = *itFile;
		bool isTxtFile = IsTxtFile(fileName);

		if (isTxtFile) { // Load a camera calibration from 
			CCameraAgent *pNewCamera = LoadCalibrationForACamera(infoFilePath + "/" + fileName);

			if (pNewCamera != 0) {
				pNewCamera->SetName(GetFileName(fileName));
				CObjectManagerFactory::GetCameraManager().InsertACamera(pNewCamera);
			}
		}
	}

	return;
}

void CLoadCameraCalibrationCommand::LoadCameraPhotos(const QString &photoFilePath)
{
	QDir txtFileDir(photoFilePath);
	QStringList fileList = txtFileDir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
	int nFile = fileList.size();
	auto itFile = fileList.begin();

	for (int i = 0; i < nFile; i++, itFile++) {
		QString fileName = *itFile;
		bool isImageFile = IsImageFile(fileName);
		
		if (!isImageFile) continue;

		// Try to find the camera which takes this photo
		std::string imageName = GetFileName(fileName);
		CCameraAgent *pAttachCamera = 0;
		const std::list<CCameraAgent *> &listCamera = CObjectManagerFactory::GetCameraManager().GetCameras();
		auto itCamera = listCamera.begin();

		for (; itCamera != listCamera.end(); itCamera++) {
			CCameraAgent *pCamera = *itCamera;
			if (pCamera->GetName().compare(imageName) == 0) {
				pAttachCamera = pCamera;
				break;
			}
		}

		if (pAttachCamera != 0) {
			CImageObject *pImage = new CImageObject;
			if (pImage->LoadImageFromFile(photoFilePath + "/" + fileName)) {
				pAttachCamera->SetPhotoImage(pImage);
			}
			else {
				delete pImage;
			}	
		}
	}

	return;
}

CCameraAgent* CLoadCameraCalibrationCommand::LoadCalibrationForACamera(const QString &fileName)
{
	FILE *fp = fopen(fileName.toStdString().c_str(), "r");

	if (fp == 0) {
		return 0;
	}

	// read the calibration data of the camera
	CCameraAgent::S_CameraIntrinsicParameter intrinsicParam;
	std::vector<CCameraAgent::S_CameraCheckboardExtrinsicParameter> arrayExtrinsicParam;
	CCameraAgent::S_CameraCheckboardExtrinsicParameter extrinsicParam;
	bool hasIntrinsic = false, hasExtrinsic = false;

	while (!feof(fp)) {
		char line[128]; 
		QString buddyCamera;
		fgets(line, 128, fp);

		if (IsIntrinsicParameter(line)) { 
			// read the intrinsic parameters
			fscanf(fp, "%lf %lf %lf %lf %lf", &(intrinsicParam.fx), &(intrinsicParam.fy),
				&(intrinsicParam.cx), &(intrinsicParam.cy), &(intrinsicParam.skew));
			fgets(line, 128, fp);

			hasIntrinsic = true;
		}
		else if (IsCheckerboardExtrinsicParameter(line, buddyCamera)) {
			// read the extrinsic parameters
			double om[3];

			extrinsicParam.buddyCamera = buddyCamera.toStdString();

			fscanf(fp, "%lf %lf %lf", &(om[0]), &(om[1]), &(om[2]));
			fgets(line, 128, fp);

			ConvertLieAlgebraToRotation(om, extrinsicParam.R);

			fscanf(fp, "%lf %lf %lf", &(extrinsicParam.tx), &(extrinsicParam.ty), &(extrinsicParam.tz));
			fgets(line, 128, fp);

			arrayExtrinsicParam.push_back(extrinsicParam);
			hasExtrinsic = true;
		}
	}
	fclose(fp);

	CCameraAgent *pCamera = 0;
	if (hasIntrinsic && hasExtrinsic) {
		// Create a new camera
		pCamera = new CCameraAgent;

		pCamera->SetIntrinsicParameter(intrinsicParam);
		pCamera->SetCheckboardExtrinsicParameter(arrayExtrinsicParam);
	}

	return pCamera;
}

bool CLoadCameraCalibrationCommand::IsTxtFile(const QString &fileName) const
{
	int dotIndex = fileName.lastIndexOf('.');
	QString sufix = fileName.right(fileName.length() - 1 - dotIndex);

	if (sufix.compare("txt") == 0) {
		return true;
	}

	return false;
}

bool CLoadCameraCalibrationCommand::IsImageFile(const QString &fileName) const
{
	int dotIndex = fileName.lastIndexOf('.');
	QString sufix = fileName.right(fileName.length() - 1 - dotIndex);

	if (sufix.compare("jpg") == 0 || sufix.compare("jepg") == 0 || 
		sufix.compare("bmp") == 0 || sufix.compare("png") == 0) {
		return true;
	}

	return false;
}

std::string CLoadCameraCalibrationCommand::GetFileName(const QString &fileName)
{
	int dotIndex = fileName.lastIndexOf('.');

	return fileName.left(dotIndex).toStdString();
}

bool CLoadCameraCalibrationCommand::IsIntrinsicParameter(char line[128])
{
	QStringList strList;
	QString lineStr(line);

	strList = lineStr.split(' ');

	QString item = strList[0];

	if (item.compare("%intrinsic")==0 || item.compare("%Intrinsic")==0) {
		return true;
	}
	
	return false;
}

bool CLoadCameraCalibrationCommand::IsCheckerboardExtrinsicParameter(char line[128], QString &buddyCamera)
{
	QStringList strList;
	QString lineStr(line);

	strList = lineStr.split(' ');

	QString item = strList[0];

	if (item.compare("%buddy-extrinsic")==0 || item.compare("%Buddy-extrinsic")==0
		|| item.compare("%buddy-Extrinsic")==0 || item.compare("%Buddy-Extrinsic")==0) {
		
		buddyCamera = strList[1];
		
		return true;
	}

	return false;
}

void CLoadCameraCalibrationCommand::ConvertLieAlgebraToRotation(const double om[3], double R[9])
{
	using namespace cv;

	cv::Mat rot_vec = Mat::zeros(1, 3, CV_64F), rot_mat = Mat::zeros(3, 3, CV_64F);

	rot_vec.at<double>(0, 0) = om[0];
	rot_vec.at<double>(0, 1) = om[1];
	rot_vec.at<double>(0, 2) = om[2];

	cv::Rodrigues(rot_vec, rot_mat);

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			R[i * 3 + j] = rot_mat.at<double>(i, j);

	// Is it (row, col) correct? 

	return;
}

void CLoadCameraCalibrationCommand::TransformToGlobalCoordinateSystem()
{
	std::unordered_map<std::string, CCameraAgent*> hashCamera;
	const std::list<CCameraAgent *> &listCamera = CObjectManagerFactory::GetCameraManager().GetCameras();

	auto itCamera = listCamera.begin();
	for (; itCamera != listCamera.end(); itCamera++) {
		CCameraAgent *pCamera = *itCamera;
		hashCamera[pCamera->GetName()] = pCamera;
	}

	// 1. Use the main camera to define the global coordinate system
	CCameraAgent::S_CameraExtrinsicParameter extrinsicParameter;
	CCameraAgent *pMainCamera = hashCamera["camera_main"];
	assert(pMainCamera != 0);

	if (pMainCamera == 0) return;

	extrinsicParameter.tx = extrinsicParameter.ty = extrinsicParameter.tz = .0;
	extrinsicParameter.R[0] = 1.; extrinsicParameter.R[1] = .0; extrinsicParameter.R[2] = .0;
	extrinsicParameter.R[3] = .0; extrinsicParameter.R[4] = 1.0; extrinsicParameter.R[5] = .0;
	extrinsicParameter.R[6] = .0; extrinsicParameter.R[7] = .0; extrinsicParameter.R[8] = 1.0;

	pMainCamera->SetExtrinsicParameter(extrinsicParameter);
	
	// 2. Compute the extrinsic parameter of other cameras with respect to the GLC system
	itCamera = listCamera.begin();
	
	for (; itCamera != listCamera.end(); itCamera++) {
		CCameraAgent *pCamera = *itCamera;

		if (pCamera == pMainCamera) continue;
		ComputeGlobalCameraExtrinsicParameter(pCamera, pMainCamera, hashCamera);
	}

	return;
}

void CLoadCameraCalibrationCommand::ComputeGlobalCameraExtrinsicParameter(CCameraAgent *pCamera, CCameraAgent *pMainCamera, std::unordered_map<std::string, CCameraAgent *> &hashCamera)
{
	// 1. Find a shortest path from 'pCamera' to 'pMainCamera' via the camera's buddy.
	const std::list<CCameraAgent *> &listCamera = CObjectManagerFactory::GetCameraManager().GetCameras();

	auto itCamera = listCamera.begin();
	for (; itCamera != listCamera.end(); itCamera++) {
		(*itCamera)->_iReserved = -1; // not visited
	}

	std::queue<CCameraAgent*> curPath;

	// starting from the 'pCamera'
	bool bFound = false;
	pCamera->_iReserved = 0;
	curPath.push(pCamera);

	while (!curPath.empty()) {
		// propagate the currently shortest path
		CCameraAgent *pCurCamera = curPath.front();
		curPath.pop();

		if (pCurCamera == pMainCamera) {// the path has been found
			bFound = true;
			break;
		}

		// propagate the path
		const std::vector<CCameraAgent::S_CameraCheckboardExtrinsicParameter> &buddies = pCurCamera->GetCheckboardExtrinsicParameter();
		auto itBuddy = buddies.begin();

		for (; itBuddy != buddies.end(); itBuddy++) {
			CCameraAgent *pBuddyCamera = hashCamera[(*itBuddy).buddyCamera.c_str()];
			assert(pBuddyCamera != 0);

			if (pBuddyCamera->_iReserved == -1) { // not visited
				pBuddyCamera->_iReserved = (long)pCurCamera;
				curPath.push(pBuddyCamera);
			}
		}
	}

	assert(bFound);

	// Extract the shortest path
	std::stack<CCameraAgent *> cameraPath;
	CCameraAgent *pCurCamera = pMainCamera;

	cameraPath.push(pCurCamera);
	while (pCurCamera->_iReserved != 0) {
		pCurCamera = (CCameraAgent *)(pCurCamera->_iReserved);
		cameraPath.push(pCurCamera);
	}
	assert(pCurCamera == pCamera);

	// 2. Visit the path from 'pCamera' to 'pMainCamera' to compute the global extrinsic parameter of the 'pCamera'
	double extrinsic_R[9], extrinsic_T[3];
	
	pCurCamera = cameraPath.top();
	cameraPath.pop();

	extrinsic_T[0] = extrinsic_T[1] = extrinsic_T[2] = .0;
	extrinsic_R[0] = 1.; extrinsic_R[1] = .0; extrinsic_R[2] = .0;
	extrinsic_R[3] = .0; extrinsic_R[4] = 1.0; extrinsic_R[5] = .0;
	extrinsic_R[6] = .0; extrinsic_R[7] = .0; extrinsic_R[8] = 1.0;

	while (!cameraPath.empty()) {
		CCameraAgent *pNextCamera = cameraPath.top();
		cameraPath.pop();

		// Propagate the extrinsic parameter from current camera to the next camera
		const CCameraAgent::S_CameraCheckboardExtrinsicParameter *pCheckerboard1 = 0, *pCheckerboard2 = 0;
		const std::vector<CCameraAgent::S_CameraCheckboardExtrinsicParameter> &arrayCheckerboard1 = pCurCamera->GetCheckboardExtrinsicParameter(),
			&arrayCheckerboard2 = pNextCamera->GetCheckboardExtrinsicParameter();
		auto itCheckerboard1 = arrayCheckerboard1.begin(), itCheckerboard2 = arrayCheckerboard2.begin();

		for (; itCheckerboard1 != arrayCheckerboard1.end(); itCheckerboard1++) {
			if ((*itCheckerboard1).buddyCamera == pNextCamera->GetName()) {
				pCheckerboard1 = &(*itCheckerboard1);
				break;
			}
		}

		for (; itCheckerboard2 != arrayCheckerboard2.end(); itCheckerboard2++) {
			if ((*itCheckerboard2).buddyCamera == pCurCamera->GetName()) {
				pCheckerboard2 = &(*itCheckerboard2);
				break;
			}
		}

		assert(pCheckerboard1 != 0 && pCheckerboard2 != 0);
		double *R1 = const_cast<double *>(pCheckerboard1->R), T1[3] = { pCheckerboard1->tx, pCheckerboard1->ty, pCheckerboard1->tz },
			*R2 = const_cast<double *>(pCheckerboard2->R), T2[3] = { pCheckerboard2->tx, pCheckerboard2->ty, pCheckerboard2->tz };
		double R1_T[9], extrinsicR_12[9], tempT[3], extrinsicT_12[3];

		CUtilityTool::TransposeMatrix(R1, R1_T);
		CUtilityTool::MultiplyMatrix(R2, R1_T, extrinsicR_12);
		CUtilityTool::MultiplyMatrixVector(extrinsicR_12, T1, tempT);

		extrinsicT_12[0] = T2[0] - tempT[0];
		extrinsicT_12[1] = T2[1] - tempT[1];
		extrinsicT_12[2] = T2[2] - tempT[2];

		double R[9], T[3];
		CUtilityTool::MultiplyMatrix(extrinsicR_12, extrinsic_R, R);
		CUtilityTool::MultiplyMatrixVector(extrinsicR_12, extrinsic_T, tempT);

		memcpy(extrinsic_R, R, sizeof(double) * 9);
		
		for (int i = 0; i < 3; i++)
			extrinsic_T[i] = tempT[i] + extrinsicT_12[i];

		pCurCamera = pNextCamera;
	}

	assert(pCurCamera == pMainCamera);

	CCameraAgent::S_CameraExtrinsicParameter extrinsicParameter;

	extrinsicParameter.tx = extrinsic_T[0];
	extrinsicParameter.ty = extrinsic_T[1];
	extrinsicParameter.tz = extrinsic_T[2];
	memcpy(extrinsicParameter.R, extrinsic_R, sizeof(double) * 9);

	pCamera->SetExtrinsicParameter(extrinsicParameter);

	return;
}