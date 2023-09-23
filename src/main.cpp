#include "main.h"

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "Usage : %s <inputFile.igb> <outputFile.fbx|dae|obj>\n", argv[0]);
		return 1;
	}

	igAlchemy alchemy;
	Sg::igArkRegisterForIGBFiles();
	ArkCore->getPluginHelper()->loadPlugin("libRaven", "Alchemy");

	igString inputFile = argv[1];
	igString outputFile = argv[2];

	if (!igIGBResource->load(inputFile))
	{
		fprintf(stderr, "Failed to load IGB file %s\n", inputFile);
		return -1;
	}

	igAnimationDatabaseRef animDB = igAnimationDatabase::dynamicCast(igIGBResource->getInfoByType(inputFile, "igAnimationDatabase"));

	igIGBResource->unload(inputFile);

	if (!animDB)
	{
		fprintf(stderr, "Failed to get igAnimationDatabase from IGB file %s\n", inputFile);
		return -2;
	}

	igInt skinCount = animDB->getSkinList()->getCount();
	igInt skelCount = animDB->getSkeletonList()->getCount();

	if (skinCount != 1 || skelCount != 1)
	{
		fprintf(stderr, "igAnimationDatabase must contain exactly 1 skin and skeleton, found %d skins and %d skeletons\n", skinCount, skelCount);
		return -3;
	}
	
	Converter::ConverterManager manager;

	if (!manager.Initialize(outputFile))
	{
		fprintf(stderr, "Failed to initialize exporter for %s\n", outputFile);
		return -4;
	}

	Converter::ActorConverter converter(manager.GetScene(), animDB->getSkin(0), animDB->getSkeleton(0));
	//Converter::AnimationConverter animConverter(manager.GetScene(), animDB->getSkeleton(0), animDB->getAnimationList());

	animDB = NULL;
	converter.Convert();
	//animConverter.Convert();

	if (!manager.Export())
	{
		fprintf(stderr, "Failed to export %s\n", outputFile);
		return -5;
	}

	return 0;
}

//int main2()
//{
//	// Create the FBX SDK manager
//	FbxManager* lSdkManager = FbxManager::Create();
//
//	// Create an IOSettings object.
//	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
//	lSdkManager->SetIOSettings(ios);
//
//	// ... Configure the FbxIOSettings object ...
//
//	// Create an importer.
//	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
//
//	// Declare the path and filename of the file containing the scene.
//	// In this case, we are assuming the file is in the same directory as the executable.
//	const char* lFilename = "C:\\1801.fbx";
//
//	// Initialize the importer.
//	bool lImportStatus = lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings());
//
//	if (!lImportStatus) {
//		printf("Call to FbxImporter::Initialize() failed.\n");
//		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
//		return -1;
//	}
//
//	FbxScene* scene = fbxsdk::FbxScene::Create(lSdkManager, "Scene Root");
//
//	lImporter->Import(scene);
//
//	FbxNode* node = scene->FindNodeByName("meshh");
//	FbxSkin* skin = (FbxSkin*)node->GetMesh()->GetDeformer(0);
//
//	for (int i = 0; i < skin->GetClusterCount(); i++)
//	{
//		FbxCluster* cluster = skin->GetCluster(i);
//		FbxNode* link = cluster->GetLink();
//		fbxsdk::FbxAMatrix tm;
//
//		cluster->GetTransformLinkMatrix(tm);
//
//		FbxVector4 t = tm.GetT();
//		FbxVector4 q = tm.GetR();
//
//		printf("%s translation [%f,%f,%f]\n", link->GetName(), t[0], t[1], t[2]);
//		printf("%s rotation [%f,%f,%f]\n", link->GetName(), q[0], q[1], q[2]);
//	}
//
//	lImporter->Destroy();
//	return 0;
//}