#include "SaveFileCommand.h"

#include "../../Scene/ObjectManagerFactory.h"
#include "../../Utility/UtilityTool.h"
#include "../GLCameraView.h"

#include <QDir>
#include <QFileDialog>

void CSaveFileCommand::BeginCmdImplement()
{
	// Save the things into a calibration configuration file
	
	// 1. Save path
	QString defaultPath = CObjectManagerFactory::GetSceneObjectManager().GetDataDir();

	if (defaultPath.size() == 0)
		defaultPath = QDir::currentPath();

	QFileDialog::Options options;
	QString selectedFilter;
	QString fileName;

	fileName = QFileDialog::getSaveFileName(0,
		QFileDialog::tr("Save Calibration File"),
		defaultPath,
		QFileDialog::tr("Calibration File (*.xml)"),
		&selectedFilter,
		options);
	
	// 2. Save it
	if (!fileName.isEmpty()) {
		// Write the scene information into the selected file
		QFile file(fileName);
		if (!file.open(QFile::WriteOnly | QFile::Text)) {
			CCmdBase::Print(QFile::tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString()));

			return;
		}

		// Using xml writer, the scene information is saved into the file
		_xmlWriter.setDevice(&file);

		_xmlWriter.writeStartDocument();
		_xmlWriter.writeDTD("<!DOCTYPE Camera_Calibration>");

		_xmlWriter.writeStartElement("Camera_Calibration");
		_xmlWriter.writeAttribute("version", "1.0");

		// (1) Calibration information of the cameras
		WriteCameraCalibration();

		// (2) Model information captured by the cameras
		WriteModelCapture();
		
		_xmlWriter.writeEndElement();
		_xmlWriter.writeEndDocument();

		file.close();
	}

	return;
}

void CSaveFileCommand::WriteCameraCalibration()
{
	const std::list<CCameraAgent *> &listCamera = CObjectManagerFactory::GetCameraManager().GetCameras();
	auto itCamera = listCamera.begin();

	for (; itCamera != listCamera.end(); itCamera++) {
		CCameraAgent *pCamera = *itCamera;
		const CCameraAgent::S_CameraIntrinsicParameter &intrinsicParam = pCamera->GetIntrinsicParameter();
		const CCameraAgent::S_CameraExtrinsicParameter &extrinsicParam = pCamera->GetExtrinsicParameter();

		_xmlWriter.writeStartElement("Camera");

		_xmlWriter.writeAttribute("name", QString::fromStdString(pCamera->GetName())); 

		// intrinsic
		QString txtIntrinsicParam = QString::number(intrinsicParam.cx, 'f', 12) + ";" + QString::number(intrinsicParam.cy, 'f', 12) + ";" +
			QString::number(intrinsicParam.fx, 'f', 12) + ";" + QString::number(intrinsicParam.fy, 'f', 12) + ";" + QString::number(intrinsicParam.skew, 'f', 12);

		_xmlWriter.writeTextElement("IntrinsicParam", txtIntrinsicParam);

		// extrinsic
		QString txtExtrinsicParam = QString::number(extrinsicParam.tx, 'f', 12) + ";" + QString::number(extrinsicParam.ty, 'f', 12) + ";" +
			QString::number(extrinsicParam.tz, 'f', 12) + ";" + QString::number(extrinsicParam.R[0], 'f', 12) + ";" + QString::number(extrinsicParam.R[1], 'f', 12) + ";" + 
			QString::number(extrinsicParam.R[2], 'f', 12) + ";" + QString::number(extrinsicParam.R[3], 'f', 12) + ";" + QString::number(extrinsicParam.R[4], 'f', 12) + ";" + 
			QString::number(extrinsicParam.R[5], 'f', 12) + ";" + QString::number(extrinsicParam.R[6], 'f', 12) + ";" + QString::number(extrinsicParam.R[7], 'f', 12) + ";" + 
			QString::number(extrinsicParam.R[8], 'f', 12);

		_xmlWriter.writeTextElement("ExtrinsicParam", txtExtrinsicParam);

		_xmlWriter.writeEndElement();
	}

	return;
}

void CSaveFileCommand::WriteModelCapture()
{
	// Model path, photo path, model transformation (translation, rotation)
	QString photoDir = CObjectManagerFactory::GetSceneObjectManager().GetPhotoDir(),
		modelPath = CObjectManagerFactory::GetSceneObjectManager().GetModelPath();
	CGLCameraView *pMainView = CObjectManagerFactory::GetCommandManager().GetMainWindowHandle()->GetCameraView(CMainWindow::MAIN);

	S_ModelTransform &modelTransform = pMainView->GetModelTransform();

	const Vec3 & translate = modelTransform.m_Translate;
	Matrix4f &rotation = modelTransform.m_RotTransform;
	double rotMatrix[16];

	rotation.get(rotMatrix);

	_xmlWriter.writeStartElement("ModelCapture");

	_xmlWriter.writeTextElement("Photo_Dir", photoDir);
	_xmlWriter.writeTextElement("Model_Path", modelPath); 

	QString transform = QString::number(translate.x(), 'f', 12) + ";" + QString::number(translate.y(), 'f', 12) + ";" +
		QString::number(translate.z(), 'f', 12) + ";" + QString::number(rotMatrix[0], 'f', 12) + ";" + QString::number(rotMatrix[1], 'f', 12) + ";" +
		QString::number(rotMatrix[2], 'f', 12) + ";" + QString::number(rotMatrix[3], 'f', 12) + ";" + QString::number(rotMatrix[4], 'f', 12) + ";" +
		QString::number(rotMatrix[5], 'f', 12) + ";" + QString::number(rotMatrix[6], 'f', 12) + ";" + QString::number(rotMatrix[7], 'f', 12) + ";" +
		QString::number(rotMatrix[8], 'f', 12) + ";" + QString::number(rotMatrix[9], 'f', 12) + ";" + QString::number(rotMatrix[10], 'f', 12) + ";" + 
		QString::number(rotMatrix[11], 'f', 12) + ";" + QString::number(rotMatrix[12], 'f', 12) + ";" + QString::number(rotMatrix[13], 'f', 12) + ";" + 
		QString::number(rotMatrix[14], 'f', 12) + ";" + QString::number(rotMatrix[15], 'f', 12);
	_xmlWriter.writeTextElement("Transform", transform);

	_xmlWriter.writeEndElement();

	return;
}