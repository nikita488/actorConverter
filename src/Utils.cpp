#include "Utils.h"

namespace Converter
{
	namespace Utils
	{
		FbxDouble3 ig2FbxDouble3(igVec3f vec)
		{
			return FbxDouble3(vec[0], vec[1], vec[2]);
		}

		FbxDouble3 ig2FbxDouble3(igVec4f vec)
		{
			return FbxDouble3(vec[0], vec[1], vec[2]);
		}

		FbxDouble3 quaternion2FbxDouble3(igQuaternionf quat)
		{
			igFloat degreesX, degreesY, degreesZ;

			quat.getRotation(degreesX, degreesY, degreesZ);
			return FbxDouble3(degreesX, degreesY, degreesZ);
		}

		FbxVector2 ig2FbxVector2(igVec2f vec)
		{
			return FbxVector2(vec[0], vec[1]);
		}

		FbxVector4 ig2FbxVector4(igVec3f vec)
		{
			return FbxVector4(vec[0], vec[1], vec[2], 0.0F);
		}

		FbxVector4 ig2FbxVector4(igVec4f vec)
		{
			return FbxVector4(vec[0], vec[1], vec[2], vec[3]);
		}

		FbxColor ig2FbxColor(igVec4f rgba)
		{
			return FbxColor(rgba[0], rgba[1], rgba[2], rgba[3]);
		}

		FbxMatrix ig2FbxMatrix(igMatrix44f mat)
		{
			return FbxMatrix(
				mat.mat[0][0], mat.mat[0][1], mat.mat[0][2], mat.mat[0][3],
				mat.mat[1][0], mat.mat[1][1], mat.mat[1][2], mat.mat[1][3],
				mat.mat[2][0], mat.mat[2][1], mat.mat[2][2], mat.mat[2][3],
				mat.mat[3][0], mat.mat[3][1], mat.mat[3][2], mat.mat[3][3]
			);
		}

		void matrix2TRS(igMatrix44f* mat, igVec3f& t, igQuaternionf& r, igVec3f& s)
		{
			mat->getTranslation(t);
			mat->getQuaternion(r);
			mat->getScale(s);
		}
	}
}