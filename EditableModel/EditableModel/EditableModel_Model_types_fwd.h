#ifndef _EditableModel_Model_types_fwd_h_
#define _EditableModel_Model_types_fwd_h_

#include <list>

namespace Dv
{
	namespace EditableModel
	{		
		// The per-vertex data
		struct ModelPerVertexData;	
		
		// Model container typedefs		
		typedef std::list<ModelPerVertexData>								PerVertexDataCont; // container of per-vertex data
		typedef PerVertexDataCont::const_iterator							PerVertexDataConstIt;
		typedef PerVertexDataCont::iterator									PerVertexDataIt;
	} // EditableModel
} // Dv

#endif // _EditableModel_Model_types_fwd_h_