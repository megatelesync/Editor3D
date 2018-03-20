#ifndef _ModelVertexHandle_h_
#define _ModelVertexHandle_h_

#include "EditableModel_Model.h"
#include "ModelVertexHandleBase.h"
#include "EditableModel_Model_types_fwd.h"

namespace Dv
{
	namespace EditableModel
	{
		// Handle typedefs
		typedef ModelVertexHandleBase<Model, VertexContIt> ModelVertexHandle;
		//typedef ModelVertexHandleBase<Model, PerVertexDataIt> ModelVertexHandle;

		// forward declarations
		inline void							TransformPosition						(const ModelVertexHandle& vert, const XMMATRIX& mat);
		inline ModelVertexHandle			DuplicateVertex							(const ModelVertexHandle& vert, Model *pModelToCopyInto);
		
		// Welds a given source vertex with another vertex (destination vertex)
		// and fixes all vertex links by moving them from src vertex to dest vertex
		// WARNING: The adjacency links will not be fixed 
		// (if they are not fixed automatically by linking vertices)
		void								WeldVertexAndMoveVertexLinks			(const ModelVertexHandle& srcVertex, const ModelVertexHandle& destinationVertex);
		void								MoveVertexLinks							(const ModelVertexHandle& srcVertex, const ModelVertexHandle& destinationVertex);

		// impl
		inline void TransformPosition(const ModelVertexHandle& vert, const XMMATRIX& mat)
		{
			TransformPosition(vert.GetVertexInfo(), mat);
		}

		inline ModelVertexHandle DuplicateVertex(const ModelVertexHandle& vert, Model *pModelToCopyInto)
		{
			return pModelToCopyInto->AddVertex(vert.GetVertexInfo());
		}
	} // EditableModel
} // Dv

#endif // _ModelVertexHandle_h_