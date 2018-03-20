#ifndef _ModelVertexConstHandle_fwd_h_
#define _ModelVertexConstHandle_fwd_h_

#include "EditableModel_structs.h"
#include "EditableModel_Model_types_fwd.h"

namespace Dv
{
	namespace EditableModel
	{
		// class fwd
		template<class ModelTypeArg, class VertexIteratorTypeArg> class ModelVertexHandleBase;

		// Handle typedefs
		typedef ModelVertexHandleBase<const Model, VertexContConstIt> ModelVertexConstHandle;
		//typedef ModelVertexHandleBase<const Model, PerVertexDataConstIt> ModelVertexConstHandle;

		// operation fwd declarations
		inline XMVECTOR CalcNormal
		(
			const ModelVertexConstHandle& vertOne,
			const ModelVertexConstHandle& vertTwo,
			const ModelVertexConstHandle& vertThree
		);
	} // EditableModel
} // Dv

#endif // _ModelVertexConstHandle_fwd_h_