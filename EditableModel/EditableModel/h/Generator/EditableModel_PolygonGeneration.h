#ifndef _EditableModel_PolygonGeneration_h_
#define _EditableModel_PolygonGeneration_h_

#include <array>
#include "../../EditableModel_GeneratorBase.h"

namespace Dv
{
	namespace EditableModel
	{
		namespace Generator
		{ 
			inline ModelPolygonHandle CreateQuad
			(
				Model *pModel, int materialIndex, 
				const VertexDesc& firstVert,
				const VertexDesc& secondVert,
				const VertexDesc& thirdVert,
				const VertexDesc& fourthVert
			);

			inline ModelPolygonHandle CreateTri
			(
				Model *pModel, int materialIndex,				
				const VertexDesc& firstVert,
				const VertexDesc& secondVert,
				const VertexDesc& thirdVert
			);

			inline ModelPolygonHandle CreatePlaneQuadPolygon
			(
				Model *pModel, int materialIndex,
				float width, float height, const Vector& tangent, const Vector& binormal
			);

			inline ModelPolygonHandle CreateZPlaneRectPolygon
			(
				Model *pModel, int materialIndex, float width, float height
			);

			// impl
			inline ModelPolygonHandle CreateQuad
			(
				Model *pModel, int materialIndex, 
				const VertexDesc& firstVert,
				const VertexDesc& secondVert,
				const VertexDesc& thirdVert,
				const VertexDesc& fourthVert
			)
			{
				std::array<VertexDesc, 4> vertices = { firstVert, secondVert, thirdVert, fourthVert };
				return CreatePolygon(pModel,materialIndex,vertices.cbegin(),vertices.cend());
			}

			inline ModelPolygonHandle CreateTri
			(
				Model *pModel, int materialIndex,				
				const VertexDesc& firstVert,
				const VertexDesc& secondVert,
				const VertexDesc& thirdVert
			)
			{
				std::array<VertexDesc, 4> vertices = { firstVert, secondVert, thirdVert };
				return CreatePolygon(pModel,materialIndex,vertices.cbegin(),vertices.cend());
			}

			inline ModelPolygonHandle CreatePlaneQuadPolygon
			(
				Model *pModel, int materialIndex,
				float width, float height, const Vector& tangent, const Vector& binormal
			)
			{
				VertexDescList vertices = CreatePlaneQuadVertexDescList(width, height, tangent, binormal);
				return CreatePolygon(pModel,materialIndex,vertices.cbegin(),vertices.cend());
			}

			inline ModelPolygonHandle CreateZPlaneRectPolygon
			(
				Model *pModel, int materialIndex, float width, float height
			)
			{
				VertexDescList vertices = CreateZPlaneRectVertexDescList(width,height);
				return CreatePolygon(pModel,materialIndex,vertices.cbegin(),vertices.cend());
			}
		} // Generator 
	} // EditableModel
} // Dv

#endif // _EditableModel_PolygonGeneration_h_