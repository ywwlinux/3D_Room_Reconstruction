#include "OpenFileCommand.h"

#include <QDir>
#include <QFileDialog>

#include "../../Scene/ObjectManagerFactory.h"
#include "LoadCameraCalibrationCommand.h"
#include "../../Utility/UtilityTool.h"

void COpenFileCommand::BeginCmdImplement()
{
	// Load the things in a selected configuration file

	// 1. Specify the path of the configuration file
	// Save the things into a calibration configuration file

	// 1. Save path
	QString defaultPath = CObjectManagerFactory::GetSceneObjectManager().GetDataDir();

	if (defaultPath.size() == 0)
		defaultPath = QDir::currentPath();

	QFileDialog::Options options;
	QString selectedFilter;
	QString fileName;

	fileName = QFileDialog::getOpenFileName(0,
		QFileDialog::tr("Open Calibration File"),
		defaultPath,
		QFileDialog::tr("Calibration File (*.xml)"),
		&selectedFilter,
		options);

	// 2. Open the file and load things from the file
	if (!fileName.isEmpty()) {
		// Destroy current content
		CObjectManagerFactory::DestroyAllManagers();

		QFile file(fileName);
		if (!file.open(QFile::ReadOnly | QFile::Text)) {
			CCmdBase::Print(QFile::tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));

			return;
		}

		// Using xml reader, the scene information is read from the file
		_xmlReader.setDevice(&file);

		if (_xmlReader.readNextStartElement()) {
			const QXmlStreamAttributes& attribute = _xmlReader.attributes();
			if (_xmlReader.name() == "Camera_Calibration" && attribute.value("version") == "1.0") {
				// Read scene information from the file
				Q_ASSERT(_xmlReader.isStartElement() && _xmlReader.name() == "Camera_Calibration");

				LoadCameraCalibration();
			}
			else
				CCmdBase::Print(QObject::tr("The file is not an animation_scene version 1.0 file."));
		}

		file.close();

	}

	return;
}

void COpenFileCommand::LoadCameraCalibration()
{
	while (_xmlReader.readNextStartElement()) {
		if (_xmlReader.name() == "Camera") {
			CCameraAgent *pCamera = LoadACamera();

			if (pCamera != 0) {
				CObjectManagerFactory::GetCameraManager().InsertACamera(pCamera);
			}
		}
		else if (_xmlReader.name() == "ModelCapture") {
			LoadModelCapture();
		}
		else {
			_xmlReader.skipCurrentElement();
		}
	}

	return;
}

CCameraAgent* COpenFileCommand::LoadACamera()
{
	Q_ASSERT(_xmlReader.isStartElement() && _xmlReader.name() == "Camera");
	CCameraAgent *pCamera = new CCameraAgent;

	// Read the camera's calibration
	const QXmlStreamAttributes& attribute = _xmlReader.attributes();

	pCamera->SetName(attribute.value("name").toString().toStdString());

	while (_xmlReader.readNextStartElement()) {
		if (_xmlReader.name() == "IntrinsicParam") {
			const QString &txtIntrinsicParam = _xmlReader.readElementText();
			QStringList paramList = txtIntrinsicParam.split(';');
			QStringList::const_iterator itParam = paramList.begin();
			CCameraAgent::S_CameraIntrinsicParameter intrinsicParam;
			
			intrinsicParam.cx = (*itParam).toDouble(); itParam++;
			intrinsicParam.cy = (*itParam).toDouble(); itParam++;
			intrinsicParam.fx = (*itParam).toDouble(); itParam++;
			intrinsicParam.fy = (*itParam).toDouble(); itParam++;
			intrinsicParam.skew = (*itParam).toDouble(); 

			pCamera->SetIntrinsicParameter(intrinsicParam);
		}
		else if (_xmlReader.name() == "ExtrinsicParam") {
			const QString &txtExtrinsicParam = _xmlReader.readElementText();
			QStringList paramList = txtExtrinsicParam.split(';');
			QStringList::const_iterator itParam = paramList.begin();
			CCameraAgent::S_CameraExtrinsicParameter extrinsicParam;

			extrinsicParam.tx = (*itParam).toDouble(); itParam++;
			extrinsicParam.ty = (*itParam).toDouble(); itParam++;
			extrinsicParam.tz = (*itParam).toDouble(); itParam++;

			for (int i = 0; i < 9; i++)
			{
				extrinsicParam.R[i] = (*itParam).toDouble(); itParam++;
			}

			pCamera->SetExtrinsicParameter(extrinsicParam);
		}
		else {
			_xmlReader.skipCurrentElement();
		}
	}

	return pCamera;
}

void COpenFileCommand::LoadModelCapture()
{
	Q_ASSERT(_xmlReader.isStartElement() && _xmlReader.name() == "ModelCapture");

	while (_xmlReader.readNextStartElement()) {
		if (_xmlReader.name() == "Photo_Dir") {
			const QString &photoDir = _xmlReader.readElementText();
			
			// read the photos
			if (!photoDir.isEmpty()) {
				CObjectManagerFactory::GetCommandManager().GetLoadCameraCalibrationCommand().LoadCameraPhotos(photoDir);
				CObjectManagerFactory::GetSceneObjectManager().SetPhotoDir(photoDir);
			}
		}
		else if (_xmlReader.name() == "Model_Path") {
			const QString &modelPath = _xmlReader.readElementText();

			if (!modelPath.isEmpty()) {
				CMesh *pMesh = CUtilityTool::LoadMeshFromObjFile(modelPath.toStdString().c_str());

				pMesh->ComputeCenter();
				CObjectManagerFactory::GetSceneObjectManager().Set3DModel(pMesh);

				CObjectManagerFactory::GetSceneObjectManager().SetModelPath(modelPath);
			}

		}
		else if (_xmlReader.name() == "Transform") {
			const QString &txtTransform = _xmlReader.readElementText();
			QStringList transformList = txtTransform.split(';');
			QStringList::const_iterator itParam = transformList.begin();
			double translate[3], rot[16];

			translate[0] = (*itParam).toDouble(); itParam++;
			translate[1] = (*itParam).toDouble(); itParam++;
			translate[2] = (*itParam).toDouble(); itParam++;

			for (int i = 0; i < 16; i++)
			{
				rot[i] = (*itParam).toDouble(); itParam++;
			}

			CGLCameraView *pMainView = CObjectManagerFactory::GetCommandManager().GetMainWindowHandle()->GetCameraView(CMainWindow::MAIN);
			S_ModelTransform &transform = pMainView->GetModelTransform();

			transform.m_Translate.Set(translate[0], translate[1], translate[2]);
			transform.m_RotTransform.set(rot);
		}
		else {
			_xmlReader.skipCurrentElement();
		}
	}

	return;
}