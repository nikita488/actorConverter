#pragma once

#include <fbxsdk.h>
#include <igAttrs/igMaterialAttr.h>
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
	class AnimationConverter
	{
	private:
		FbxScene* scene;
		FbxPose* pose;
		FbxAnimLayer* animLayer;
		FbxArray<FbxNode*> boneNodes;
		igCommonTraversalRef traversal;
		igActorRef actor;
		igAnimationListRef animations;

	public:
		AnimationConverter(FbxScene* scene, igSkeleton* skeleton, igAnimationListRef animations);
		~AnimationConverter();

		void Convert();
	private:
		FbxNode* ProcessBone(igInt boneIndex = 0);
	};
}