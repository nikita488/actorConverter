#pragma once

#include <fbxsdk.h>
#include <igCore/igMetaObject.h>
#include <igAttrs/igAttr.h>
#include <igSg/igAttrSet.h>
#include <igSg/igIterateGraph.h>

using namespace Gap;
using namespace Core;
using namespace Attrs;
using namespace Sg;

namespace Converter
{
	namespace Utils
	{
		template<class T>
		T* findAttribute(igAttrSet* attributes, igInt unitID = -1)
		{
			igAttr* attribute = NULL;

			for (igInt i = 0; i < attributes->getAttrCount(); i++)
				if ((attribute = attributes->getAttr(i)) && attribute->isOfType(T::getClassMeta()) && (unitID < 0 || attribute->getUnitID() == unitID))
					return static_cast<T*>(attribute);
			return NULL;
		};
		template<class T>
		T* findParentAttribute(igIterateGraph* iterator, igInt unitID = -1)
		{
			const igNonRefCountedNodeList* path = iterator->getCurrentPath();
			igAttrSet* attributes = NULL;
			T* attribute = NULL;

			for (igInt i = path->getCount() - 2; i >= 0; i--)
				if ((attributes = igAttrSet::dynamicCast(path->get(i))) && (attribute = findAttribute<T>(attributes, unitID)))
					return attribute;
			return NULL;
		};
		template<class T>
		T* findParentNode(igIterateGraph* iterator)
		{
			const igNonRefCountedNodeList* path = iterator->getCurrentPath();
			igNode* node = NULL;

			for (igInt i = path->getCount() - 2; i >= 0; i--)
				if ((node = path->get(i)) && node->isOfType(T::getClassMeta()))
					return static_cast<T*>(node);
			return NULL;
		};
		FbxDouble3 ig2FbxDouble3(igVec3f vec);
		FbxDouble3 ig2FbxDouble3(igVec4f vec);
		FbxDouble3 quaternion2FbxDouble3(igQuaternionf quat);
		FbxVector2 ig2FbxVector2(igVec2f vec);
		FbxVector4 ig2FbxVector4(igVec3f vec);
		FbxVector4 ig2FbxVector4(igVec4f vec);
		FbxColor ig2FbxColor(igVec4f rgba);
		FbxMatrix ig2FbxMatrix(igMatrix44f mat);
		void matrix2TRS(igMatrix44f* mat, igVec3f& t, igQuaternionf& r, igVec3f& s);
	}
}