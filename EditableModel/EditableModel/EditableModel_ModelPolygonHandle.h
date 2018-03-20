#ifndef _EditableModel_ModelPolygonHandle_h_
#define _EditableModel_ModelPolygonHandle_h_

#include "ModelEdgeHandleBase.h"
#include "ModelPolygonHandleBase.h"

namespace Dv
{
	namespace EditableModel
	{
		class ModelPolygonHandle : public ModelPolygonHandleBase<Model, PolygonIt, ModelEdgeHandle>
		{
		public:			
			// create default model handle
			inline ModelPolygonHandle() {}
			inline ModelPolygonHandle(const PolygonHandleBaseType& other) : ModelPolygonHandleBase(other.GetModel(), other.GetPolygonIterator()) {}
			inline ModelPolygonHandle(Model *pModelIn, const PolygonIt& polygonIteratorIn);			

			inline void								RemoveEdge			(const ModelEdgeHandle& edge) const;
			inline void								RemoveEdge			(const ModelEdgeConstHandle& edge) const;
			inline ModelEdgeHandle					SplitEdge			(const ModelEdgeHandle& edge, const PolygonVertexInfo& splitterVertex) const;			
			// Split edge of the poly (not affecting the adjacent edges)
			// Automatically links the new vertex with the new edge
			inline ModelEdgeHandle					SplitPolyEdge		(const ModelEdgeHandle& edge, const PolygonVertexInfo& splitterVertex) const;				
			inline ModelPolygonHandle				SplitOrdered		(const ModelEdgeHandle& startVertexEdge, const ModelEdgeHandle& endVertexEdge) const;
			inline ModelEdgeHandle					MoveEdgeBefore		(const ModelEdgeHandle& edgeToMove, const ModelEdgeHandle& whereToMove) const;

		private:
		};
	} // EditableModel
} // Dv

#endif // _EditableModel_ModelPolygonHandle_h_