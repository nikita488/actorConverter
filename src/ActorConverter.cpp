#include "ActorConverter.h"

namespace Converter
{
	ActorConverter::ActorConverter(FbxScene* scene, igSkin* skin, igSkeleton* skeleton) :
		scene(scene),
		animLayer(FbxAnimLayer::Create(scene, "defaultAnimation")),
		linkNodes(skeleton->getJointCount()),
		skinnedGraph(skin->getSkinnedGraph())
	{
		igActorRef actor = igActor::instantiateRefFromPool(kIGMemoryPoolTemporary);
		igCommonTraversalRef traversal = igCommonTraversal::instantiateRefFromPool(kIGMemoryPoolTemporary);

		bindCombiner = createCombiner(traversal, actor, skeleton);

		igAnimationRef defaultAnimation = igAnimation::instantiateRefFromPool(kIGMemoryPoolActor);
		igAnimationTrackList* tracks = defaultAnimation->getTrackList();

		defaultAnimation->setName("defaultAnimation");
		tracks->setCount(skeleton->getBoneCount() - 1);

		for (igInt i = 1; i < skeleton->getBoneCount(); i++)
		{
			igAnimationTrackRef track = igAnimationTrack::instantiateRefFromPool(kIGMemoryPoolActor);

			track->setName(skeleton->getBoneName(i));
			track->getConstantQuaternion()->set(*bindCombiner->getResultQuaternion(i));
			tracks->set(i - 1, track);
		}

		defaultCombiner = createCombiner(traversal, actor, skeleton, defaultAnimation);
		actor = NULL;
		traversal = NULL;
	}

	ActorConverter::~ActorConverter()
	{
		skinnedGraph = NULL;
		defaultCombiner = NULL;
		bindCombiner = NULL;
		linkNodes.Clear();
		animLayer->Destroy();
	}

	igAnimationCombinerRef ActorConverter::createCombiner(igCommonTraversal* traversal, igActor* actor, igSkeleton* skeleton, igAnimation* animation)
	{
		igAnimationCombinerRef combiner = igAnimationCombiner::instantiateRefFromPool(kIGMemoryPoolActor);

		combiner->configure(skeleton);
		actor->configure(combiner);

		igMatrix44f* matrix = combiner->getBoneMatrixArray();

		for (igInt i = 0; i < combiner->getBoneCount(); i++, matrix++)
			matrix->makeIdentity();
		
		matrix = combiner->getBlendMatrixArray();

		for (igInt i = 0; i < combiner->getJointCount(); i++, matrix++)
			matrix->makeIdentity();

		matrix = combiner->getAnimationCacheMatrixArray();

		for (igInt i = 0; i < combiner->getAnimationCacheMatrixArrayLength(); i++, matrix++)
			matrix->makeIdentity();

		if (animation)
		{
			igAnimationBlendParameters parameters;

			parameters._animation = animation;
			combiner->add(parameters);
		}
		else
		{
			combiner->setBindPose(-1);
		}

		actor->setActorCacheUsed(false);
		traversal->setUserTimeState(true);
		traversal->setTimeAsLong(0L);
		actor->updateAnimation(traversal);
		return combiner;
	}

	fbxsdk::FbxNode* ActorConverter::ProcessBone(igInt boneIndex)
	{
		igSkeleton* skeleton = bindCombiner->getSkeleton();
		igSkeletonBoneInfo* bone = skeleton->getBoneInfo(boneIndex);
		igInt childCount = skeleton->getChildCount(boneIndex);
		fbxsdk::FbxNode* node = fbxsdk::FbxNode::Create(scene, bone->getName());
		fbxsdk::FbxSkeleton* skeletonAttr = fbxsdk::FbxSkeleton::Create(scene, bone->getName());

		node->SetNodeAttribute(skeletonAttr);

		if (skeleton->getParent(boneIndex) <= 0)
			skeletonAttr->SetSkeletonType(fbxsdk::FbxSkeleton::eRoot);
		else
			skeletonAttr->SetSkeletonType(childCount > 0 ? fbxsdk::FbxSkeleton::eLimb : fbxsdk::FbxSkeleton::eEffector);

		igMatrix44f* matrix = bindCombiner->getAnimationCacheMatrix(boneIndex);
		igVec3f translation;
		igFloat rotX, rotY, rotZ;

		matrix->getTranslation(translation);
		matrix->getRotation(rotX, rotY, rotZ);
		node->LclTranslation = Utils::ig2FbxDouble3(translation);
		node->LclRotation = FbxDouble3(rotX, rotY, rotZ);

		matrix = defaultCombiner->getAnimationCacheMatrix(boneIndex);
		matrix->getTranslation(translation);
		matrix->getRotation(rotX, rotY, rotZ);

		fbxsdk::FbxAnimCurve* curve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
		fbxsdk::FbxTime time;
		igInt keyIndex;

		if (curve)
		{
			curve->KeyModifyBegin();
			time.SetSecondDouble(0.0F);
			curve->KeySetValue(keyIndex = curve->KeyAdd(time), translation[0]);
			curve->KeySetInterpolation(keyIndex, fbxsdk::FbxAnimCurveDef::eInterpolationConstant);
			curve->KeyModifyEnd();
		}

		if (curve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, true))
		{
			curve->KeyModifyBegin();
			time.SetSecondDouble(0.0F);
			curve->KeySetValue(keyIndex = curve->KeyAdd(time), translation[1]);
			curve->KeySetInterpolation(keyIndex, fbxsdk::FbxAnimCurveDef::eInterpolationConstant);
			curve->KeyModifyEnd();
		}

		if (curve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, true))
		{
			curve->KeyModifyBegin();
			time.SetSecondDouble(0.0F);
			curve->KeySetValue(keyIndex = curve->KeyAdd(time), translation[2]);
			curve->KeySetInterpolation(keyIndex, fbxsdk::FbxAnimCurveDef::eInterpolationConstant);
			curve->KeyModifyEnd();
		}

		/*if (curve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, true))
		{
			curve->KeyModifyBegin();
			time.SetSecondDouble(0.0F);
			curve->KeySetValue(keyIndex = curve->KeyAdd(time), rotX);
			curve->KeySetInterpolation(keyIndex, fbxsdk::FbxAnimCurveDef::eInterpolationConstant);
			curve->KeyModifyEnd();
		}

		if (curve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, true))
		{
			curve->KeyModifyBegin();
			time.SetSecondDouble(0.0F);
			curve->KeySetValue(keyIndex = curve->KeyAdd(time), rotY);
			curve->KeySetInterpolation(keyIndex, fbxsdk::FbxAnimCurveDef::eInterpolationConstant);
			curve->KeyModifyEnd();
		}

		if (curve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, true))
		{
			curve->KeyModifyBegin();
			time.SetSecondDouble(0.0F);
			curve->KeySetValue(keyIndex = curve->KeyAdd(time), rotZ);
			curve->KeySetInterpolation(keyIndex, fbxsdk::FbxAnimCurveDef::eInterpolationConstant);
			curve->KeyModifyEnd();
		}*/

		if (bone->getBmIdx() >= 0)
			linkNodes.SetAt(bone->getBmIdx(), node);

		for (igInt i = 0; i < childCount; i++)
			node->AddChild(ProcessBone(skeleton->getSpecifiedChild(boneIndex, i)));
		return node;
	}

	void ActorConverter::Convert()
	{
		fbxsdk::FbxNode* actorRoot = ProcessBone();

		actorRoot->SetName("ActorRoot");
		scene->GetRootNode()->AddChild(actorRoot);

		igIterateGraphRef iterator = igIterateGraph::instantiateRefFromPool(kIGMemoryPoolTemporary);
		igMaterialAttr* lastMaterialAttr = NULL;
		igTextureBindAttr* lastTextureBindAttr = NULL;
		fbxsdk::FbxSurfaceMaterial* lastMaterial = NULL;
		fbxsdk::FbxTexture* lastTexture = NULL;

		for (igNode* node = iterator->beginOfType(skinnedGraph, igGeometry::getClassMeta());
			iterator->isNotLast();
			node = iterator->getNextOfType(igGeometry::getClassMeta()))
		{
			igBlendMatrixSelect* blendMatrices = Utils::findParentNode<igBlendMatrixSelect>(iterator);

			if (!blendMatrices)
				continue;

			igGeometry* geometry = igGeometry::dynamicCast(node);
			GeometryWrapper wrapper(geometry);

			if (!wrapper.configured())
				continue;

			igVertexFormat format = wrapper.getVertexFormat();
			fbxsdk::FbxNode* meshNode = fbxsdk::FbxNode::Create(scene, geometry->getName());
			fbxsdk::FbxMesh* mesh = fbxsdk::FbxMesh::Create(scene, geometry->getName());
			fbxsdk::FbxAMatrix transform = meshNode->EvaluateGlobalTransform();
			fbxsdk::FbxSkin* skin = fbxsdk::FbxSkin::Create(scene, "");
			igMaterialAttr* materialAttr = Utils::findParentAttribute<igMaterialAttr>(iterator);
			igTextureBindAttr* textureBindAttr = Utils::findParentAttribute<igTextureBindAttr>(iterator, 0);

			if (materialAttr)
			{
				if (materialAttr != lastMaterialAttr || textureBindAttr != lastTextureBindAttr)
				{
					fbxsdk::FbxSurfacePhong* material = fbxsdk::FbxSurfacePhong::Create(scene, geometry->getName());
					igVec4f diffuse = *materialAttr->getDiffuse();

					material->Diffuse = Utils::ig2FbxDouble3(diffuse);
					material->Ambient = Utils::ig2FbxDouble3(*materialAttr->getAmbient());
					material->Specular = Utils::ig2FbxDouble3(*materialAttr->getSpecular());
					material->Shininess = materialAttr->getShininess() / 128.0F;
					material->TransparencyFactor = 1.0F - diffuse[3];
					material->ShadingModel = "Phong";

					if (textureBindAttr && textureBindAttr->getTexture())
					{
						if (textureBindAttr != lastTextureBindAttr)
						{
							igTextureAttr* textureAttr = textureBindAttr->getTexture();

							if (textureAttr->getImageCount() > 0)
							{
								igImage* image = textureAttr->getImage();
								igStringObjRef imageName = igStringObj::instantiateRefFromPool(kIGMemoryPoolTemporary);

								imageName->set(image->getName());
								imageName->removePathFromFileName();
								imageName->removeFileExtension();

								fbxsdk::FbxFileTexture* texture = fbxsdk::FbxFileTexture::Create(scene, imageName->getBuffer());

								imageName->set(image->getName());
								imageName->removePathFromFileName();
								texture->SetFileName(imageName->getBuffer());
								texture->SetTextureUse(fbxsdk::FbxTexture::eStandard);
								texture->SetMappingType(fbxsdk::FbxTexture::eUV);
								texture->SetMaterialUse(fbxsdk::FbxFileTexture::eModelMaterial);
								//texture->SetSwapUV(false);
								//texture->SetTranslation(0.0, 0.0);
								//texture->SetScale(1.0, 1.0);
								//texture->SetRotation(0.0, 0.0);
								material->Diffuse.ConnectSrcObject(texture);
								imageName = NULL;
								lastTextureBindAttr = textureBindAttr;
								lastTexture = texture;
							}
						}
						else
						{
							material->Diffuse.ConnectSrcObject(lastTexture);
						}
					}

					meshNode->AddMaterial(material);
					lastMaterial = material;
					lastMaterialAttr = materialAttr;
				}
				else
				{
					meshNode->AddMaterial(lastMaterial);
				}
			}

			meshNode->SetNodeAttribute(mesh);
			scene->GetRootNode()->AddChild(meshNode);
			mesh->InitControlPoints(wrapper.getVertexCount());
			mesh->AddDeformer(skin);

			if (meshNode->GetMaterialCount() > 0)
			{
				fbxsdk::FbxGeometryElementMaterial* material = mesh->CreateElementMaterial();

				material->SetMappingMode(FbxGeometryElement::eAllSame);
				material->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
				material->GetIndexArray().Add(0);
			}

			if (format.hasNormals())
			{
				fbxsdk::FbxGeometryElementNormal* normal = mesh->CreateElementNormal();

				normal->SetMappingMode(FbxGeometryElement::eByControlPoint);
				normal->SetReferenceMode(FbxGeometryElement::eDirect);
				normal->GetDirectArray().SetCount(mesh->GetControlPointsCount());
			}

			if (format.hasVertexColors())
			{
				fbxsdk::FbxGeometryElementVertexColor* color = mesh->CreateElementVertexColor();

				color->SetMappingMode(FbxGeometryElement::eByControlPoint);
				color->SetReferenceMode(FbxGeometryElement::eDirect);
				color->GetDirectArray().SetCount(mesh->GetControlPointsCount());
			}

			if (format.getTextureCoordCount() > 0)
			{
				fbxsdk::FbxGeometryElementUV* uv = mesh->CreateElementUV("Diffuse");

				uv->SetMappingMode(FbxGeometryElement::eByControlPoint);
				uv->SetReferenceMode(FbxGeometryElement::eDirect);
				uv->GetDirectArray().SetCount(mesh->GetControlPointsCount());
			}

			for (igInt i = 0; i < blendMatrices->getBlendMatrixIndexCount(); i++)
			{
				fbxsdk::FbxCluster* cluster = fbxsdk::FbxCluster::Create(scene, "");
				igInt bmIndex = blendMatrices->getBlendMatrixIndex(i);

				skin->AddCluster(cluster);
				cluster->SetLink(linkNodes[bmIndex]);
				cluster->SetLinkMode(fbxsdk::FbxCluster::eTotalOne);
				cluster->SetTransformMatrix(transform);
				cluster->SetTransformLinkMatrix(cluster->GetLink()->EvaluateGlobalTransform());
			}

			igUnsignedInt uvIndex = 0;
			fbxsdk::FbxLayerElement::EType uvType = fbxsdk::FbxLayerElement::eTextureDiffuse;

			for (igInt i = 0; i < mesh->GetControlPointsCount(); i++)
			{
				mesh->GetControlPoints()[i] = Utils::ig2FbxVector4(wrapper.getPosition(i));

				if (mesh->GetElementNormalCount() > 0)
					mesh->GetElementNormal()->GetDirectArray().SetAt(i, Utils::ig2FbxVector4(wrapper.getNormal(i)));

				if (mesh->GetElementVertexColorCount() > 0)
					mesh->GetElementVertexColor()->GetDirectArray().SetAt(i, Utils::ig2FbxColor(wrapper.getColor(i)));

				if (mesh->GetElementUVCount(uvType) > 0)
					mesh->GetElementUV(uvIndex, uvType)->GetDirectArray().SetAt(i, Utils::ig2FbxVector2(wrapper.getTextureCoord(uvIndex, i)));

				for (igUnsignedInt j = 0; j < wrapper.getWeightCount(); j++)
				{
					igFloat weight = wrapper.getWeight(j, i);

					if (weight > 0.0F)
						skin->GetCluster(wrapper.getBlendMatrixIndex(j, i))->AddControlPointIndex(i, weight);
				}
			}

			igIndexArray* indices = wrapper.getIndexArray();

			for (igUnsignedInt i = 0; i < wrapper.getPrimitiveCount(); i++)
			{
				mesh->BeginPolygon();
				mesh->AddPolygon(indices->getIndex(i * 3 + 0));
				mesh->AddPolygon(indices->getIndex(i * 3 + 1));
				mesh->AddPolygon(indices->getIndex(i * 3 + 2));
				mesh->EndPolygon();
			}
		}

		iterator = NULL;

		fbxsdk::FbxPose* bindPose = fbxsdk::FbxPose::Create(scene, "Bind Pose");
		fbxsdk::FbxPose* restPose = fbxsdk::FbxPose::Create(scene, "Rest Pose");
		fbxsdk::FbxAnimStack* animStack = fbxsdk::FbxAnimStack::Create(scene, "defaultAnimDB");
		fbxsdk::FbxTime time;

		time.SetSecondDouble(0.0);
		bindPose->SetIsBindPose(true);
		animStack->AddMember(animLayer);
		scene->AddPose(bindPose);
		scene->AddPose(restPose);

		for (igInt i = 1; i < scene->GetNodeCount(); i++)
		{
			fbxsdk::FbxNode* node = scene->GetNode(i);
			bindPose->Add(node, node->EvaluateGlobalTransform());
			restPose->Add(node, node->EvaluateGlobalTransform(time));
		}
	}
}