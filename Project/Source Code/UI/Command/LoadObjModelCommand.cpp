#include "LoadObjModelCommand.h"

#include "../../Scene/ObjectManagerFactory.h"
#include "../../Utility/UtilityTool.h"

#include <QDir>
#include <QFileDialog>

void CLoadObjModelCommand::BeginCmdImplement()
{
	// Load model from .obj file

	// 1. Specify the path of the configuration file
	QString defaultPath = CObjectManagerFactory::GetSceneObjectManager().GetDataDir();

	if (defaultPath.size() == 0)
		defaultPath = QDir::currentPath();

	QFileDialog::Options options;
	QString selectedFilter;
	QString fileName;


	fileName = QFileDialog::getOpenFileName(0,
		"Select an .obj File",
		defaultPath,
		"Image files (*.obj)",
		&selectedFilter,
		options);
	
	// 2. Open the file and load things from the file
	if (!fileName.isEmpty()) {
		CMesh *pMesh = CUtilityTool::LoadMeshFromObjFile(fileName.toStdString().c_str());

		pMesh->ComputeCenter();
		CObjectManagerFactory::GetSceneObjectManager().Set3DModel(pMesh);

		CObjectManagerFactory::GetSceneObjectManager().SetModelPath(fileName);
	}

	return;
}