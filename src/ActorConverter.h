#pragma once

#include <fbxsdk.h>
#include <igCore/igStringObj.h>
#include <igAttrs/igMaterialAttr.h>
#include <igAttrs/igTextureBindAttr.h>
#include <igAttrs/igTextureAttr.h>
#include <igGfx/igImage.h>
#include <igSg/igCommonTraversal.h>
#include <igSg/igIterateGraph.h>
#include <igSg/igNode.h>
#include <igSg/igGroup.h>
#include <igSg/igGeometry.h>
#include <igSg/igBlendMatrixSelect.h>
#include <igSg/igActor.h>
#include <igSg/igSkin.h>
#include <igSg/igSkeleton.h>
#include <igSg/igAnimationCombiner.h>
#include "GeometryWrapper.h"
#include "Utils.h"

using namespace Gap;
using namespace Sg;

namespace Converter
{
	class ActorConverter
	{
	private:
		FbxScene* scene;
		FbxAnimLayer* animLayer;
		FbxArray<FbxNode*> linkNodes;
		igAnimationCombinerRef bindCombiner;
		igAnimationCombinerRef defaultCombiner;
		igNodeRef skinnedGraph;

	public:
		ActorConverter(FbxScene* scene, igSkin* skin, igSkeleton* skeleton);
		~ActorConverter();

		void Convert();
	private:
		igAnimationCombinerRef createCombiner(igCommonTraversal* traversal, igActor* actor, igSkeleton* skeleton, igAnimation* animation = NULL);
		FbxNode* ProcessBone(igInt boneIndex = 0);
	};
}