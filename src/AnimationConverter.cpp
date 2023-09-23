#include "AnimationConverter.h"

namespace Converter
{
	AnimationConverter::AnimationConverter(FbxScene* scene, igSkeleton* skeleton, igAnimationListRef animations) :
		scene(scene),
		pose(FbxPose::Create(scene, "Bind Pose")),
		animLayer(FbxAnimLayer::Create(scene, "Default Animation Layer")),
		boneNodes(skeleton->getBoneCount()),
		traversal(igCommonTraversal::instantiateRefFromPool(kIGMemoryPoolTemporary)),
		actor(igActor::instantiateRefFromPool(kIGMemoryPoolTemporary)),
		animations(animations)
	{
		fbxsdk::FbxAnimStack* animStack = fbxsdk::FbxAnimStack::Create(scene, "Default Animation Stack");

		animStack->AddMember(animLayer);
		pose->SetIsBindPose(true);
		scene->AddPose(pose);

		igAnimationCombinerRef combiner = igAnimationCombiner::instantiateRefFromPool(kIGMemoryPoolActor);

		combiner->configure(skeleton);
		actor->configure(combiner);

		igMatrix44f* matrix = combiner->getBoneMatrixArray();

		for (igUnsignedInt i = 0; i < combiner->getBoneCount(); i++, matrix++)
			matrix->makeIdentity();

		matrix = combiner->getBlendMatrixArray();

		for (igUnsignedInt i = 0; i < combiner->getJointCount(); i++, matrix++)
			matrix->makeIdentity();

		matrix = combiner->getAnimationCacheMatrixArray();

		for (igUnsignedInt i = 0; i < combiner->getAnimationCacheMatrixArrayLength(); i++, matrix++)
			matrix->makeIdentity();

		combiner->setBindPose(-1);
	}

	AnimationConverter::~AnimationConverter()
	{
		actor = NULL;
		traversal = NULL;
		pose->Destroy();
	}

	fbxsdk::FbxNode* AnimationConverter::ProcessBone(igInt boneIndex)
	{
		igSkeleton* skeleton = actor->getCombiner()->getSkeleton();
		igSkeletonBoneInfo* bone = skeleton->getBoneInfo(boneIndex);
		igInt childCount = skeleton->getChildCount(boneIndex);
		fbxsdk::FbxNode* node = fbxsdk::FbxNode::Create(scene, bone->getName());
		fbxsdk::FbxSkeleton* skeletonAttr = fbxsdk::FbxSkeleton::Create(scene, bone->getName());

		node->SetNodeAttribute(skeletonAttr);

		if (skeleton->getParent(boneIndex) <= 0)
			skeletonAttr->SetSkeletonType(fbxsdk::FbxSkeleton::eRoot);
		else
			skeletonAttr->SetSkeletonType(childCount > 0 ? fbxsdk::FbxSkeleton::eLimb : fbxsdk::FbxSkeleton::eEffector);

		igMatrix44f* matrix = actor->getCombiner()->getAnimationCacheMatrix(boneIndex);
		igVec3f translation;
		igFloat rotX, rotY, rotZ;

		matrix->getTranslation(translation);
		matrix->getRotation(rotX, rotY, rotZ);
		node->LclTranslation = Utils::ig2FbxDouble3(translation);
		node->LclRotation = FbxDouble3(rotX, rotY, rotZ);
		boneNodes.SetAt(boneIndex, node);

		for (igUnsignedInt i = 0; i < childCount; i++)
			node->AddChild(ProcessBone(skeleton->getSpecifiedChild(boneIndex, i)));
		return node;
	}

	void AnimationConverter::Convert()
	{
		fbxsdk::FbxNode* actorRoot = ProcessBone();

		actorRoot->SetName("ActorRoot");
		scene->GetRootNode()->AddChild(actorRoot);

		for (igUnsignedInt i = 0; i < boneNodes.GetCount(); i++)
		{
			fbxsdk::FbxNode* node = boneNodes[i];
			pose->Add(node, node->EvaluateGlobalTransform());
		}

		igFloat sampleRate = 0.2F;
		igLong increment = 1000000000L * sampleRate;
		igAnimationCombiner* combiner = actor->getCombiner();
		fbxsdk::FbxTime keyframeTime;

		for (igUnsignedInt i = 0; i < 1; i++)
		{
			igAnimationBlendParameters parameters;
			igAnimation* animation = animations->get(1);

			parameters._animation = animation;
			parameters._startRatio = 0.0F;
			parameters._endRatio = 1.0F;
			parameters._transitionMode = igAnimationState::TransitionMode::kNone;
			parameters._combineMode = igAnimationState::CombineMode::kOverride;
			combiner->add(parameters);

			for (igLong time = 0; time < animation->getDuration(); time += increment)
			{
				igDouble timeInSeconds = time / igDouble(1000000000L);
				printf("Seconds: %f\n", timeInSeconds);

				actor->setActorCacheUsed(false);
				traversal->setUserTimeState(true);
				traversal->setTimeAsLong(time);
				actor->updateAnimation(traversal);

				for (igUnsignedInt j = 1; j < combiner->getBoneCount(); j++)
				{
					FbxNode* boneNode = boneNodes[j];
					igMatrix44f* matrix = combiner->getAnimationCacheMatrix(j);
					igVec3f translation;
					igFloat rotX, rotY, rotZ;

					matrix->getTranslation(translation);
					matrix->getRotation(rotX, rotY, rotZ);

					fbxsdk::FbxAnimCurve* curve = boneNode->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
					igInt keyIndex;

					if (curve)
					{
						curve->KeyModifyBegin();
						keyframeTime.SetSecondDouble(timeInSeconds);
						curve->KeySetValue(keyIndex = curve->KeyAdd(keyframeTime), translation[0]);
						curve->KeySetInterpolation(keyIndex, fbxsdk::FbxAnimCurveDef::eInterpolationLinear);
						curve->KeyModifyEnd();
					}

					if (curve = boneNode->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, true))
					{
						curve->KeyModifyBegin();
						keyframeTime.SetSecondDouble(timeInSeconds);
						curve->KeySetValue(keyIndex = curve->KeyAdd(keyframeTime), translation[1]);
						curve->KeySetInterpolation(keyIndex, fbxsdk::FbxAnimCurveDef::eInterpolationLinear);
						curve->KeyModifyEnd();
					}

					if (curve = boneNode->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, true))
					{
						curve->KeyModifyBegin();
						keyframeTime.SetSecondDouble(timeInSeconds);
						curve->KeySetValue(keyIndex = curve->KeyAdd(keyframeTime), translation[2]);
						curve->KeySetInterpolation(keyIndex, fbxsdk::FbxAnimCurveDef::eInterpolationLinear);
						curve->KeyModifyEnd();
					}

					if (curve = boneNode->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, true))
					{
						curve->KeyModifyBegin();
						keyframeTime.SetSecondDouble(timeInSeconds);
						curve->KeySetValue(keyIndex = curve->KeyAdd(keyframeTime), rotX);
						curve->KeySetInterpolation(keyIndex, fbxsdk::FbxAnimCurveDef::eInterpolationLinear);
						curve->KeyModifyEnd();
					}

					if (curve = boneNode->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, true))
					{
						curve->KeyModifyBegin();
						keyframeTime.SetSecondDouble(timeInSeconds);
						curve->KeySetValue(keyIndex = curve->KeyAdd(keyframeTime), rotY);
						curve->KeySetInterpolation(keyIndex, fbxsdk::FbxAnimCurveDef::eInterpolationLinear);
						curve->KeyModifyEnd();
					}

					if (curve = boneNode->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, true))
					{
						curve->KeyModifyBegin();
						keyframeTime.SetSecondDouble(timeInSeconds);
						curve->KeySetValue(keyIndex = curve->KeyAdd(keyframeTime), rotZ);
						curve->KeySetInterpolation(keyIndex, fbxsdk::FbxAnimCurveDef::eInterpolationLinear);
						curve->KeyModifyEnd();
					}
				}
			}
		}
	}
}