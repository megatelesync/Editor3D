#ifndef _TestGeometryGenerator_h_
#define _TestGeometryGenerator_h_

#include "EditableModel_GeneratorBase.h"
#include "EditableModel.h"
#include <array>

namespace Dv
{
	namespace EditableModel
	{
		struct TestGeomeratryParams
		{
		public:
			float length;
			float width;
			float height;		
			float pitDepth;
			float sideStep;
			float wallHeight;

			float GetSideSplitCoeff(float edgeLengthIn) const
			{
				return sideStep/edgeLengthIn;
			}			

			// Конструирует параметры по умолчанию
			TestGeomeratryParams()
			{
				length=10.0F;
				width=1.0F;
				height=3.0F;
				pitDepth = 5.0F;
				sideStep=0.2F;
				wallHeight = 100.0F;
			}
		};

		struct Floor
		{
		public:
			float floorY;
			float leftX;
			float rightX;
			float nearZ;
			float farZ;

			// returns back edges
			// Warning: the res[2] is commented until the VS2012 splice bug it repaired
			std::array<ModelEdgeHandle, 2> GetBackEdges() const 
			{
				std::array<ModelEdgeHandle, 2> res;
				res[0] = farLeftEdge;
				res[1] = farCenterEdge;
				//res[2] = farRightEdge;
				return res;  
			}
			
			// vertices
			PolygonVertexInfo leftVertex;
			PolygonVertexInfo rightVertex;
			PolygonVertexInfo leftFarVertex;
			PolygonVertexInfo rightFarVertex;

			ModelPolygonHandle leftestPoly;
			ModelPolygonHandle farCenterPoly; // center front polygon
			ModelPolygonHandle rightPoly;

			ModelPolygonHandle centerMiddlePoly; // center middle polygon to make the pit from
			ModelPolygonHandle nearCenterPoly;

			ModelEdgeHandle centerPolyRightEdge;			
			ModelEdgeHandle centerPolyRightEdgeCenter;
			ModelEdgeHandle centerPolyRightEdgeNear;			

			ModelEdgeHandle centerPolyLeftEdge;
			ModelEdgeHandle centerPolyLeftEdgeFar;
			ModelEdgeHandle centerPolyLeftEdgeCenter;

			// neighbour edges of the middleCenterPoly
			// (they belong to the polys that are neighbours of the middleCenterPoly)
			ModelEdgeHandle middleCenterPolyFrontNeighbourEdge;
			ModelEdgeHandle middleCenterPolyFarNeighbourEdge;

			// near right edge
			ModelEdgeHandle nearRightEdge;
			// far edge
			ModelEdgeHandle farLeftEdge;
			// left edge
			ModelEdgeHandle leftEdge;
			// right edge
			ModelEdgeHandle rightEdge;

			ModelEdgeHandle nearCenterEdge;
			ModelEdgeHandle farCenterEdge;
			ModelEdgeHandle nearLeftEdge;		
			ModelEdgeHandle farRightEdge;

			inline XMFLOAT3 GetFloorCenter() const
			{
				float floorCenterX = (rightX - leftX)/2.0F;
				float floorCenterZ = (farZ - nearZ)/2.0F;
				return XMFLOAT3(floorCenterX, floorY, floorCenterZ);
			}
			inline void Init(Model *pModel, const TestGeomeratryParams& paramsIn)
			{
				floorY = 0.0F;
				leftX=-paramsIn.width/2.0F;
				rightX=paramsIn.width/2.0F;
				nearZ=0.0F;
				farZ=paramsIn.length;
				// Left vertex
				auto leftVertexInfo = Dv::EditableModel::ModelVertexInfo(XMFLOAT3(leftX,floorY,nearZ));
				leftVertex = PolygonVertexInfo(pModel->AddVertex(leftVertexInfo).GetPerVertexDataIterator(), XMFLOAT2(0.0F, 0.0F));
				// Right vertex
				auto rightVertexInfo = Dv::EditableModel::ModelVertexInfo(XMFLOAT3(rightX,floorY,nearZ));
				rightVertex = PolygonVertexInfo(pModel->AddVertex(rightVertexInfo).GetPerVertexDataIterator(), XMFLOAT2(0.0F,1.0F));
				// Left far vertex
				auto leftFarVertexInfo = Dv::EditableModel::ModelVertexInfo(XMFLOAT3(leftX,floorY,farZ));
				leftFarVertex = PolygonVertexInfo(pModel->AddVertex(leftFarVertexInfo).GetPerVertexDataIterator(), XMFLOAT2(1.0F, 0.0F));
				// Right far vertex
				auto rightFarVertexInfo = Dv::EditableModel::ModelVertexInfo(XMFLOAT3(rightX,floorY,farZ));
				rightFarVertex = PolygonVertexInfo(pModel->AddVertex(rightFarVertexInfo).GetPerVertexDataIterator(), XMFLOAT2(1.0F, 1.0F));
				// Add the floor polygon
				ModelPolygonInfo polyInfo
				(
					0, 
					leftVertex, 
					leftFarVertex, 
					rightFarVertex, 
					rightVertex
				);
				leftestPoly = pModel->AddPoly(polyInfo);
				// ----------------------------------------------------------------------------
				// Get the edges of the new poly
				leftEdge = leftestPoly.GetFirstEdge();
				rightEdge = leftestPoly.GetLastEdge().GetPreviousEdge();
				nearRightEdge = leftEdge.GetPreviousEdge();
				farLeftEdge = leftEdge.GetNextEdge();
				// ----------------------------------------------------------------------------
				// Create near center edge
				nearCenterEdge = SplitEdge(nearRightEdge, paramsIn.GetSideSplitCoeff(Length(nearRightEdge).m128_f32[0]));
				nearLeftEdge = SplitEdge(nearCenterEdge, 1.0F-paramsIn.GetSideSplitCoeff(Length(nearCenterEdge).m128_f32[0]));
				// Create far center edge
				farCenterEdge = SplitEdge(farLeftEdge, paramsIn.GetSideSplitCoeff(Length(farLeftEdge).m128_f32[0]));
				farRightEdge = SplitEdge(farCenterEdge, 1.0F-paramsIn.GetSideSplitCoeff(Length(farCenterEdge).m128_f32[0]));
				// -------------------------------------------------------------------------------------------
				// Connect edge vertices
				rightPoly = leftestPoly.SplitOrdered(farRightEdge, nearCenterEdge);
				farCenterPoly = leftestPoly.SplitOrdered(farCenterEdge, nearLeftEdge); // at this place the farRightEdge is adulterated
				// -------------------------------------------------------------------------------------------
				centerPolyRightEdge = EdgeTwo(farCenterPoly);
				centerPolyLeftEdge = farCenterPoly.GetLastEdge();
				// -------------------------------------------------------------------------------------------
				// Split the right splitter
				centerPolyRightEdgeCenter = SplitEdge(centerPolyRightEdge,paramsIn.GetSideSplitCoeff(Length(centerPolyRightEdge).m128_f32[0]));
				centerPolyRightEdgeNear = SplitEdge(centerPolyRightEdgeCenter,1.0F-paramsIn.GetSideSplitCoeff(Length(centerPolyRightEdgeCenter).m128_f32[0]));
				// Split the left splitter
				centerPolyLeftEdgeCenter = SplitEdge(centerPolyLeftEdge,paramsIn.GetSideSplitCoeff(Length(centerPolyLeftEdge).m128_f32[0]));
				centerPolyLeftEdgeFar = SplitEdge(centerPolyLeftEdgeCenter,1.0F-paramsIn.GetSideSplitCoeff(Length(centerPolyLeftEdgeCenter).m128_f32[0]));
				// -------------------------------------------------------------------------------------------
				// Connect the nearest vertices of the left and right splitters
				nearCenterPoly = farCenterPoly.SplitOrdered(centerPolyRightEdgeNear, centerPolyLeftEdgeCenter);				
				// Connect the farthest vertices of the left and right splitters
				centerMiddlePoly = farCenterPoly.SplitOrdered(centerPolyRightEdgeCenter, centerPolyLeftEdgeFar);
				// -----------------------------------------------------------------------------------------
				
			}
			inline Floor() {}
		};

		class TestGeometryGenerator : public Generator::GeneratorBase
		{
		public:
			inline TestGeometryGenerator						(const TestGeomeratryParams& paramsIn);
			inline void Generate() override;			

			inline const Floor& GetFloor() const					{ return _floor; }
			inline const TestGeomeratryParams& GetParams() const    { return _params; }			

		private:
			TestGeomeratryParams _params;
			Floor _floor;
		};		

		inline void TestGeometryGenerator::Generate()
		{
			XMFLOAT3 pitExtrudeOffset(0.0F, -_params.pitDepth, 0.0F);
			XMFLOAT3 wallExtrudeOffset(0.0F, _params.wallHeight, 0.0F);
			// 1. Init floor geometry
			_floor.Init(&GetModel(), _params);
			// 2. Init pit
			ModelPolyHandleRange pitSidePolys = Extrude(_floor.centerMiddlePoly, pitExtrudeOffset);
			// 3. Init back poly			
			//for(ModelEdgeHandle& edge : _floor.GetBackEdges())
			//{			
			//	ExtrudeEdge(edge,wallExtrudeOffset);
			//}
		}

		inline TestGeometryGenerator::TestGeometryGenerator(const TestGeomeratryParams& paramsIn) :
			_params(paramsIn)
		{
		}
	} // EditableModel
} // Dv

#endif