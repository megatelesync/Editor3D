#ifndef _EditableModel_handle_fwd_h_
#define _EditableModel_handle_fwd_h_

namespace Dv
{
	namespace EditableModel
	{
		// Handle forward declarations		
		class Model;		
		class ModelPolygonHandle;		
		
		template<class ModelTypeArg, class VertexIteratorTypeArg> class ModelVertexHandleBase;
		template<class PolyHandleTypeArg, class EdgeIteratorTypeArg, class VertexHandleTypeArg> class ModelEdgeHandleBase;
		template<class ModelTypeArg, class PolyIteratorTypeArg, class EdgeTypeArg> class ModelPolygonHandleBase;
		
		// Handle typedefs
		typedef ModelVertexHandleBase<Model, VertexContIt>												ModelVertexHandle;
		typedef ModelEdgeHandleBase<ModelPolygonHandle, PolygonVertexContIt, ModelVertexHandle>			ModelEdgeHandle;		
		typedef std::pair<ModelVertexHandle, ModelVertexHandle>											ModelVertexHandleRange; // range of model vertex handles		
		typedef std::pair<PolygonIt, PolygonIt>															ModelPolyRange; // range of polygons
		typedef std::pair<ModelPolygonHandle, ModelPolygonHandle>										ModelPolyHandleRange; // range of polygon handles		
		typedef std::pair<ModelVertexHandle, ModelVertexHandle>											ModelVertexHandlePair;
	} // EditableModel
} // Dv

#endif // _EditableModel_handle_fwd_h_