#ifndef _EditableModel_structs_h_
#define _EditableModel_structs_h_

#include <DirectXMath.h>
#include <list>
#include <vector>
#include "EditableModel_Model_types_fwd.h"

using namespace DirectX;

namespace Dv
{
	namespace EditableModel
	{
		// forward declarations
		class Model;
		struct PolygonVertexInfo;
		struct ModelVertexInfo;
		struct ModelPolygonInfo;

		// typedef declarations
		typedef std::list<ModelVertexInfo>									VertexCont;
		typedef VertexCont::const_iterator									VertexContConstIt;
		typedef VertexCont::iterator										VertexContIt;		
		typedef std::pair<PerVertexDataIt, PerVertexDataIt>					VertexContRange; // range of model vertices		
		typedef std::list<PolygonVertexInfo>								PolygonVertexCont; // container of polygon vertices (each is represented as iterator);		
		typedef PolygonVertexCont::iterator									PolygonVertexContIt;
		typedef PolygonVertexCont::const_iterator							PolygonVertexContConstIt;
		typedef std::list<ModelPolygonInfo>									PolygonCont;		
		typedef PolygonCont::iterator										PolygonIt;
		typedef PolygonCont::const_iterator									PolygonConstIt;				

		// This structures does not reference the model
		struct ModelVertexInfo
		{
		public:
			XMFLOAT3 position;

			XMVECTOR GetPositionVector() const
			{
				return XMLoadFloat3(&position);
			}			

			void SetPosition(const XMVECTOR& positionVecIn)
			{
				XMStoreFloat3(&position, positionVecIn);
			}

			inline ModelVertexInfo(const XMVECTOR& positionVecIn);
			inline ModelVertexInfo(const XMFLOAT3& positionIn);	
			inline ModelVertexInfo() : position(0.0F,0.0F,0.0F) {}
		};				

		struct ModelPolygonInfo
		{
		public:
			PolygonVertexCont vertices;
			int materialIndex;

			// Constructs a polygon with no vertices and material number zero
			inline ModelPolygonInfo();
			// Constructs a polygon with a given material and no vertices
			inline ModelPolygonInfo(int materialIndexIn);
			// Constructs a polygon with a given material and a given vertex set
			inline ModelPolygonInfo(int materialIndexIn, PolygonVertexContConstIt startVertIt, PolygonVertexContConstIt endVertIt);

			// Constructs a degenerated quad of tow vertices
			inline ModelPolygonInfo::ModelPolygonInfo
			(
				int materialIndexIn, 
				const PolygonVertexInfo& first, 
				const PolygonVertexInfo& second
			);			
			// Constructs a quad
			inline ModelPolygonInfo::ModelPolygonInfo
			(
				int materialIndexIn, 
				const PolygonVertexInfo& first, 
				const PolygonVertexInfo& second, 
				const PolygonVertexInfo& third, 
				const PolygonVertexInfo& fourth
			);
		};

		struct PolygonVertexInfo
		{
		public:
			// Iterator to the model-level per-vertex data
			//VertexContIt vertexIterator;
			PerVertexDataIt vertexIterator;
			XMFLOAT2 texCoord;

			//const ModelVertexInfo&				GetVertexInfo()							const			{ return vertexIterator->info; }
			//ModelVertexInfo&					GetVertexInfo()											{ return vertexIterator->info; }			

			XMVECTOR GetTexCoordVector() const
			{
				return XMLoadFloat2(&texCoord);
			}

			void SetTexCoord(const XMVECTOR& texCoordVecIn)
			{
				XMStoreFloat2(&texCoord, texCoordVecIn);
			}

			inline PolygonVertexInfo()
			{
			}

			inline PolygonVertexInfo(const PerVertexDataIt& vertexIteratorIn, const XMFLOAT2& texCoordIn) :
				vertexIterator(vertexIteratorIn), texCoord(texCoordIn)
			{			
			}

			inline PolygonVertexInfo(const PerVertexDataIt& vertexIteratorIn, const XMVECTOR& texCoordVecIn) :
				vertexIterator(vertexIteratorIn)
			{			
				SetTexCoord(texCoordVecIn);
			}
		};

		// Description of the vertex of polygon and corresponding model vertex
		// Contains all the necessary information.
		struct VertexDesc
		{
			ModelVertexInfo			vertex;
			XMFLOAT2				texCoord;

			VertexDesc() : vertex(), texCoord() {}
			VertexDesc(const ModelVertexInfo& vertexIn, const XMFLOAT2& texCoordIn) : vertex(vertexIn), texCoord(texCoordIn) {}
		};
		typedef std::vector<VertexDesc> VertexDescList;
		inline VertexDescList CreateVertexDescList
		(
			const VertexDesc& v0,
			const VertexDesc& v1,
			const VertexDesc& v2			
		);
		inline VertexDescList CreateVertexDescList
		(
			const VertexDesc& v0,
			const VertexDesc& v1,
			const VertexDesc& v2,
			const VertexDesc& v3
		);		
		inline VertexDescList CreatePlaneVertexDescList
		(
			const Vector2& v0,
			const Vector2& v1,
			const Vector2& v2,
			const Vector& tangent,
			const Vector& bitangent
		);		
		inline VertexDescList CreatePlaneVertexDescList
		(
			const Vector2& v0,
			const Vector2& v1,
			const Vector2& v2,
			const Vector2& v3,
			const Vector& tangent,
			const Vector& bitangent
		);			
		inline VertexDescList CreateZPlaneRectVertexDescList(float width, float height);
		inline VertexDesc GetVertexOnPlaneDesc
		(
			const Vector2& position, 
			const Vector& tangent, 
			const Vector& binormal
		);
		inline VertexDescList CreatePlaneQuadVertexDescList
		(
			float width, float height, const Vector& tangent, const Vector& binormal
		);
		
		inline void TransformPosition(ModelVertexInfo& vert, const XMMATRIX& mat);

		inline XMVECTOR CalcNormal
		(
			const ModelVertexInfo& vertOne, 
			const ModelVertexInfo& vertTwo, 
			const ModelVertexInfo& vertThree
		);

		// ModelVertexInfo impl		
		inline ModelVertexInfo::ModelVertexInfo(const XMFLOAT3& positionIn) :
			position(positionIn)
		{
		}

		inline ModelVertexInfo::ModelVertexInfo(const XMVECTOR& positionVecIn)
		{
			SetPosition(positionVecIn);
		}

		inline void TransformPosition(ModelVertexInfo& vert, const XMMATRIX& mat)
		{
			vert.SetPosition(XMVector3Transform(vert.GetPositionVector(), mat));
		}
		inline XMVECTOR CalcNormal
		(
			const ModelVertexInfo& vertOne, 
			const ModelVertexInfo& vertTwo, 
			const ModelVertexInfo& vertThree
		)
		{
			XMVECTOR vectOne = vertTwo.GetPositionVector()-vertOne.GetPositionVector();
			XMVECTOR vectTwo = vertThree.GetPositionVector()-vertTwo.GetPositionVector();
			return XMVector3Cross(vectOne, vectTwo);
		}

		// ModelPolygonInfo impl
		inline ModelPolygonInfo::ModelPolygonInfo
		(
			int materialIndexIn, 
			const PolygonVertexInfo& first, 
			const PolygonVertexInfo& second
		) :
			materialIndex(materialIndexIn)
		{
			vertices.push_back(first);
			vertices.push_back(second);
		}

		inline ModelPolygonInfo::ModelPolygonInfo
		(
			int materialIndexIn, 
			const PolygonVertexInfo& first, 
			const PolygonVertexInfo& second, 
			const PolygonVertexInfo& third, 
			const PolygonVertexInfo& fourth
		) :
			materialIndex(materialIndexIn)
		{
			vertices.push_back(first);
			vertices.push_back(second);
			vertices.push_back(third);
			vertices.push_back(fourth);
		}

		inline ModelPolygonInfo::ModelPolygonInfo() : materialIndex(0) {}
		
		inline ModelPolygonInfo::ModelPolygonInfo(int materialIndexIn) : materialIndex(materialIndexIn) {}
		
		inline ModelPolygonInfo::ModelPolygonInfo
		(
			int materialIndexIn, 
			PolygonVertexContConstIt startVertIt, PolygonVertexContConstIt endVertIt
		) : 
			materialIndex(materialIndexIn), vertices(startVertIt, endVertIt)
		{
		}

		// VertexDesc impl
		inline VertexDescList CreateVertexDescList
		(
			const VertexDesc& v0,
			const VertexDesc& v1,
			const VertexDesc& v2			
		)
		{
			VertexDescList res;
			res.push_back(v0);
			res.push_back(v1);
			res.push_back(v2);
			return res;
		}
		inline VertexDescList CreateVertexDescList
		(
			const VertexDesc& v0,
			const VertexDesc& v1,
			const VertexDesc& v2,
			const VertexDesc& v3
		)
		{
			VertexDescList res = CreateVertexDescList(v0, v1, v2);
			res.push_back(v3);
			return res;
		}
		inline VertexDescList CreatePlaneVertexDescList
		(
			const Vector2& v0,
			const Vector2& v1,
			const Vector2& v2,
			const Vector& tangent,
			const Vector& bitangent
		)
		{
			VertexDesc v0desc = GetVertexOnPlaneDesc(v0, tangent, bitangent);
			VertexDesc v1desc = GetVertexOnPlaneDesc(v1, tangent, bitangent);
			VertexDesc v2desc = GetVertexOnPlaneDesc(v2, tangent, bitangent);
			VertexDescList res = CreateVertexDescList(v0desc, v1desc, v2desc);
			return res;
		}
		inline VertexDescList CreatePlaneVertexDescList
		(
			const Vector2& v0,
			const Vector2& v1,
			const Vector2& v2,
			const Vector2& v3,
			const Vector& tangent,
			const Vector& bitangent
		)
		{
			VertexDescList res = CreatePlaneVertexDescList(v0, v1, v2, tangent, bitangent);
			VertexDesc v3desc = GetVertexOnPlaneDesc(v3, tangent, bitangent);
			res.push_back(v3desc);
			return res;
		}
		inline VertexDesc GetVertexOnPlaneDesc
		(
			const Vector2& position, 
			const Vector& tangent, const Vector& binormal
		)
		{
			Vector tangentVec = position.x * tangent;
			Vector binormalVec = position.y * binormal;
			Vector res_position = tangentVec + binormalVec;
			ModelVertexInfo res_vertexInfo (res_position);
			VertexDesc res_vertexDesc (res_vertexInfo, position);
			return res_vertexDesc;
		}

		inline VertexDescList CreateZPlaneRectVertexDescList(float width, float height)
		{
			Vector tangent = GetVectorX();
			Vector binormal = GetVectorY();
			VertexDescList res = CreatePlaneQuadVertexDescList(width, height, tangent, binormal);
			return res;
		}

		inline VertexDescList CreatePlaneQuadVertexDescList
		(
			float width, float height, const Vector& tangent, const Vector& binormal
		)
		{
			float half_width = width / 2.0F;
			float half_height = height / 2.0F;
			Vector2 v0 (-half_width, half_height);
			Vector2 v1 (half_width, half_height);
			Vector2 v2 (half_width, -half_height);
			Vector2 v3 (-half_width, -half_height);
			VertexDescList res_list = CreatePlaneVertexDescList(v0, v1, v2, v3, tangent, binormal);
			return res_list;
		}
	} // EditableModel
} // Dv

#endif // _EditableModel_structs_h_