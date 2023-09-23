#include "GeometryWrapper.h"

namespace Converter
{
	GeometryWrapper::GeometryWrapper(igGeometry* geometry) :
		attr(Utils::findAttribute<igGeometryAttr2>(geometry)),
		oldAttr(!attr ? Utils::findAttribute<igGeometryAttr>(geometry) : NULL)
	{
		if (getPrimitiveType() == IG_GFX_DRAW_TRIANGLE_STRIP)
		{
			if (attr)
			{
				igVertexTools::deindex(geometry);
				igVertexTools::unStripGeometry(geometry);
				igVertexTools::compactGeometryAttr(geometry);
				igVertexTools::reindex(geometry);
				igVertexTools::removeDegenerate(geometry);
				attr = Utils::findAttribute<igGeometryAttr2>(geometry);
			}
			else if (oldAttr)
			{
				igVertexArrayHelperRef helper = Opt::igVertexArrayHelper::instantiateRefFromPool(kIGMemoryPoolTemporary);

				helper->deindex(geometry);
				helper->unStripGeometry(geometry);
				helper->compactGeometryAttr(geometry);
				helper->reindex(geometry);
				helper->removeDegenerate(geometry);
				helper = NULL;
				oldAttr = Utils::findAttribute<igGeometryAttr>(geometry);
			}
		}

		if (attr)
			igVertexArray2Helper::getVertexFormat(attr->getVertexArray(), &format);
		else if (oldAttr)
			format = oldAttr->getVertexFormat();
	}

	igBool GeometryWrapper::configured()
	{
		return (attr || oldAttr) && format.hasPositions() && getIndexArray() != NULL;
	}

	const igVertexFormat GeometryWrapper::getVertexFormat()
	{
		return format;
	}

	IG_GFX_DRAW GeometryWrapper::getPrimitiveType()
	{
		if (attr)
			return attr->getPrimitiveType();
		else if (oldAttr)
			return oldAttr->getPrimitiveType();
		else
			return IG_GFX_DRAW::IG_GFX_DRAW_TRIANGLES;
	}

	igUnsignedInt GeometryWrapper::getPrimitiveCount()
	{
		if (attr)
			return attr->getPrimitiveCount();
		else if (oldAttr)
			return oldAttr->getPrimitiveCount();
		else
			return 0;
	}

	igUnsignedInt GeometryWrapper::getVertexCount()
	{
		if (attr)
			return attr->getVertexCount();
		else if (oldAttr)
			return oldAttr->getVertexCount();
		else
			return 0;
	}

	igUnsignedInt GeometryWrapper::getWeightCount()
	{
		return format.getBlendWeightCount();
	}

	igIndexArray* GeometryWrapper::getIndexArray()
	{
		if (attr)
			return attr->getIndexArray();
		else if (oldAttr)
			return oldAttr->getIndexArray();
		else
			return NULL;
	}

	const igVec3f GeometryWrapper::getPosition(igUnsignedInt vertexIndex)
	{
		igVec3f pos;
		
		if (attr)
			igVertexArray2Helper::getPosition(attr->getVertexArray(), vertexIndex, pos);
		else
			oldAttr->getPosition(vertexIndex, pos);
		return pos;

		/*if (attr)
		{
			igComponentEditInfo2 info;

			info._componentType = igVertexData::IG_VERTEX_COMPONENT_POSITION;
			attr->getVertexArray()->getEditableComponent(info);

			const igVec3f pos = info.getPosition(0);
		}
		else if (oldAttr)
		{
			igComponentEditInfo info;

			info._componentType.setType(IG_GFX_VERTEX_COMPONENT::IG_GFX_POSITION);
			oldAttr->getVertexArray()->getEditableComponent(&info, 0);

			const igVec3f pos = igGetPosition(info, 0);
		}*/
	}

	const igVec3f GeometryWrapper::getNormal(igUnsignedInt vertexIndex)
	{
		igVec3f normal;

		if (attr)
			igVertexArray2Helper::getNormal(attr->getVertexArray(), vertexIndex, normal);
		else
			oldAttr->getNormal(vertexIndex, normal);
		return normal;
	}

	const igVec4f GeometryWrapper::getColor(igUnsignedInt vertexIndex)
	{
		igVec4f color;

		if (attr)
			igVertexArray2Helper::getColor(attr->getVertexArray(), vertexIndex, color);
		else
			oldAttr->getColor(vertexIndex, color);
		return color;
	}

	const igVec2f GeometryWrapper::getTextureCoord(igUnsignedInt uvIndex, igUnsignedInt vertexIndex)
	{
		igVec2f texCoord;

		if (attr)
			igVertexArray2Helper::getTextureCoord(attr->getVertexArray(), uvIndex, vertexIndex, texCoord);
		else
			oldAttr->getTextureCoord(uvIndex, vertexIndex, texCoord);
		return texCoord;
	}

	igInt GeometryWrapper::getBlendMatrixIndex(igUnsignedInt weightIndex, igUnsignedInt vertexIndex)
	{
		if (attr)
			return igVertexArray2Helper::getBlendMatrixIndex(attr->getVertexArray(), weightIndex, vertexIndex);
		else if (oldAttr)
			return oldAttr->getBlendMatrixIndex(weightIndex, vertexIndex);
		else
			return -1;
	}

	igFloat GeometryWrapper::getWeight(igUnsignedInt weightIndex, igUnsignedInt vertexIndex)
	{
		if (attr)
			return igVertexArray2Helper::getWeight(attr->getVertexArray(), weightIndex, vertexIndex);
		else if (oldAttr)
			return oldAttr->getWeight(weightIndex, vertexIndex);
		else
			return 0.0F;
	}
}