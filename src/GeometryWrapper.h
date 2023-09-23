#pragma once

#include <igAttrs/igGeometryAttr.h>
#include <igAttrs/igGeometryAttr2.h>
#include <igGfx/igGfx.h>
#include <igGfx/igVertexData.h>
#include <igGfx/igVertexArray.h>
#include <igGfx/igVertexArray2.h>
#include <igGfx/igIndexArray.h>
#include <igMath/igVec2f.h>
#include <igMath/igVec4f.h>
#include <igMath/igVec4f.h>
#include <igSg/igGeometry.h>
#include <igGfx/igVertexArray2Helper.h>
#include <igOpt/igVertexArrayHelper.h>
#include <igOpt/igVertexTools.h>
#include "Utils.h"

using namespace Gap;
using namespace Attrs;
using namespace Sg;
using namespace Opt;

namespace Converter
{
	class GeometryWrapper
	{
	private:
		igGeometryAttr2* attr;
		igGeometryAttr* oldAttr;
		igVertexFormat format;

	public:
		GeometryWrapper(igGeometry* geometry);

		igBool configured();
		const igVertexFormat getVertexFormat();
		IG_GFX_DRAW getPrimitiveType();
		igUnsignedInt getPrimitiveCount();
		igUnsignedInt getVertexCount();
		igUnsignedInt getWeightCount();
		igIndexArray* getIndexArray();
		const igVec3f getPosition(igUnsignedInt vertexIndex);
		const igVec3f getNormal(igUnsignedInt vertexIndex);
		const igVec4f getColor(igUnsignedInt vertexIndex);
		const igVec2f getTextureCoord(igUnsignedInt uvIndex, igUnsignedInt vertexIndex);
		igInt getBlendMatrixIndex(igUnsignedInt weightIndex, igUnsignedInt vertexIndex);
		igFloat getWeight(igUnsignedInt weightIndex, igUnsignedInt vertexIndex);
	};
}