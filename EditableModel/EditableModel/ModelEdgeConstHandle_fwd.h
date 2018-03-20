#ifndef _ModelEdgeConstHandle_fwd_h_
#define _ModelEdgeConstHandle_fwd_h_

#include "EditableModel_structs.h"
#include "ModelVertexConstHandle_fwd.h"

namespace Dv
{
	namespace EditableModel
	{	
		template<class PolyHandleTypeArg, class EdgeIteratorTypeArg, class VertexHandleTypeArg> class ModelEdgeHandleBase;

		class ModelPolygonConstHandle;

		// Handle typedefs
		typedef ModelEdgeHandleBase<ModelPolygonConstHandle, PolygonVertexContConstIt, ModelVertexConstHandle> ModelEdgeConstHandle;		
	} // EditableModel
} // Dv

#endif // _ModelEdgeConstHandle_fwd_h_