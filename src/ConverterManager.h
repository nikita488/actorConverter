#pragma once

#include <fbxsdk.h>
#include "ActorConverter.h"
#include "AnimationConverter.h"

namespace Converter
{
	class ConverterManager
	{
	private:
		FbxManager* manager;
		FbxExporter* exporter;
		FbxScene* scene;

	public:
		ConverterManager();
		~ConverterManager();

		bool Initialize(const char* fileName);
		bool Export();

		FbxScene* GetScene();
	};
}