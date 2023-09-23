#include "ConverterManager.h"

namespace Converter
{
	ConverterManager::ConverterManager() :
		manager(FbxManager::Create()),
		exporter(FbxExporter::Create(manager, "")),
		scene(FbxScene::Create(manager, "Scene Graph"))
	{
		fbxsdk::FbxIOSettings* settings = FbxIOSettings::Create(manager, IOSROOT);

		manager->SetIOSettings(settings);
		scene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::Max);
	}

	ConverterManager::~ConverterManager()
	{
		scene->Destroy();
		exporter->Destroy();
		manager->Destroy();
	}

	bool ConverterManager::Initialize(const char* fileName)
	{
		return exporter->SetFileExportVersion(FBX_2012_00_COMPATIBLE) && exporter->Initialize(fileName, -1, manager->GetIOSettings());
	}

	bool ConverterManager::Export()
	{
		return exporter->Export(scene);
	}

	FbxScene* ConverterManager::GetScene()
	{
		return scene;
	}
}