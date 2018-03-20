#ifndef _EditableModel_Model_h_
#define _EditableModel_Model_h_

#include "EditableModel_structs.h"
#include "ModelVertexHandleBase.h"
#include "ModelEdgeHandleBase.h"
#include "ModelEdgeConstHandle_fwd.h"
#include "CommutativeMap2D.h"
#include "KeyLoadedCommutativeMap.h"
#include "EditableModel_Model_types_fwd.h"
#include "ModelPolygonHandleBase.h"
#include "ModelVertexHandleBase.h"
#include "EditableModel_handle_fwd.h"
#include "EditableModel_ModelPolygonHandle.h"
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>

namespace Dv
{
	namespace EditableModel
	{
		// Adjacency info
		struct EdgeAdjacency
		{
			// true, если 2 edge направлены в противоположные стороны
			bool bCounterDirected;

			EdgeAdjacency() : bCounterDirected(false) {}
			EdgeAdjacency(bool bCounterDirectedIn) : bCounterDirected(bCounterDirectedIn) {}
		};

		// Structure for storing information about edge adjacency
		struct EdgeToEdgeAdj
		{
			EdgeAdjacency			adj;
			ModelEdgeHandle			adjEdge;

			EdgeToEdgeAdj() {}
			EdgeToEdgeAdj(const EdgeAdjacency& adjIn, const ModelEdgeHandle& adjEdgeIn) : adj(adjIn), adjEdge(adjEdgeIn) {}
		};		

		struct ModelPerVertexData
		{
			ModelVertexInfo								info;
			std::list<ModelEdgeHandle>					outEdges;

			inline ModelPerVertexData(const ModelVertexInfo& infoIn);
		};
		
		typedef std::map<ModelEdgeConstHandle,EdgeAdjacency>						ConstEdgeMap;
		typedef std::map<ModelEdgeHandle, EdgeAdjacency>							EdgeMap;		
		typedef std::map<size_t, EdgeToEdgeAdj>										EdgeHashMap;
		typedef Util::Container::KeyLoadedCommutativeMap<size_t, EdgeAdjacency, ModelEdgeHandle, std::map<size_t, EdgeAdjacency>> AdjacencyLoadedKeyMap;
		//typedef std::unordered_set<ModelEdgeHandle>									EdgeSet;
		//typedef std::unordered_set<ModelEdgeConstHandle>					   ConstEdgeSet;
		typedef Util::Container::CommutativeMap2D<ModelEdgeConstHandle, EdgeAdjacency, std::map<ModelEdgeConstHandle, EdgeAdjacency>> AdjacencyMap;
		typedef Util::Container::CommutativeMap2D<size_t, EdgeToEdgeAdj, std::map<size_t, EdgeToEdgeAdj>> AdjacencyMapHash;
		// Expandable sequence container that maps sequenced integers to sets of edges
		//typedef std::vector<EdgeSet>							EdgeSetSequence; 
		//typedef std::vector<ConstEdgeSet>						ConstEdgeSetSequence;
		typedef std::vector<EdgeMap>							EdgeMapSequence;
		typedef std::vector<ConstEdgeMap>						ConstEdgeMapSequence;
		typedef EdgeMapSequence::iterator						EdgeMapSequenceIt;
		typedef EdgeMapSequence::const_iterator					EdgeMapSequenceConstIt;
		typedef std::vector<ConstEdgeMap>::iterator				ConstEdgeMapSequenceIt;
		typedef std::vector<ConstEdgeMap>::const_iterator		ConstEdgeMapSequenceConstIt;
		
		class Model
		{
		public:	
			// Vertex getters
			inline int                                          GetVertexCount()																const;
			inline VertexContIt                                 GetVertexBeginIterator();
			inline VertexContIt                                 GetVertexEndIterator();
			inline VertexContConstIt                            GetVertexBeginConstIterator()													const;
			inline VertexContConstIt                            GetVertexEndConstIterator()														const;			

			// Polygon getters
			inline int                                          GetPolyCount()																	const;
			inline PolygonIt                                    GetPolyBeginIterator();
			inline PolygonIt                                    GetPolyEndIterator();
			inline PolygonConstIt                               GetPolyBeginConstIterator()														const;
			inline PolygonConstIt                               GetPolyEndConstIterator()														const;

			// Vertex operations
			inline ModelVertexHandle                            AddVertex	 (const ModelVertexInfo& vertexInfoIn);
			// removes vertex and all polys linked with this vertex
			inline void                                         RemoveVertex (const ModelVertexHandle& vertexHandleIn);

			// Polygon operations									
			inline void                                         RemovePoly   (const ModelPolygonHandle& polyHanlde);
			inline ModelPolygonHandle                           AddPoly      (const ModelPolygonInfo& polyInfo);

			// Link getters
			inline bool                                         IsLinked	 (const ModelEdgeConstHandle& edge)									const;
			// deprecated (use GetAdjEdgeHashMap instead)
			//inline EdgeHashMap                                  GetAdjEdges	 (const ModelEdgeConstHandle& edge)									const;
			// deprecated (use GetAdjEdgeHashMap instead)
			//inline EdgeMap                                      GetAdjEdgeMap  (const ModelEdgeConstHandle& edge) const;
			inline EdgeHashMap                                  GetAdjEdgeHashMap (const ModelEdgeConstHandle& edge) const;
			
			// Link operations
			// Link: links two edges and assigns specified adjacency information to the pair.
			// Updates data if edge already exists.
			// Asserts: 
			// 1) failure if the edges belong to the same poly
			// 2) failure if one of the edges does not exist
			inline void                                          Link		 (const ModelEdgeHandle& edgeOne, const ModelEdgeHandle& edgeTwo, const EdgeAdjacency& newAdj);
			inline void                                          Link		 (const ModelEdgeConstHandle& edgeOne, const ModelEdgeConstHandle& edgeTwo, const EdgeAdjacency& newAdj);
			// Unlink: unlinks two edges, return true if really unlinked, (otherwise returns false);
			inline bool                                          Unlink		 (const ModelEdgeConstHandle& edgeOne, const ModelEdgeConstHandle& edgeTwo);
			inline void                                          UnlinkEdge          (const ModelEdgeConstHandle& edge);

		private:
			VertexCont										_vertices;
			PolygonCont										_polygons;
			AdjacencyMap									_adjMap;
			PerVertexDataCont								_perVertexData;
			AdjacencyMapHash								_adjMapHash;
			AdjacencyLoadedKeyMap							_adjLoadedKeyMap;
		};
		// Count number of polygon edges of model
		// (each edge of each poly treated as a distinct edge)
		inline int CountPolyEdges(const Model& model);

		// ModelPerVertexData impl
		inline ModelPerVertexData::ModelPerVertexData(const ModelVertexInfo& infoIn) : info(infoIn)
		{
		}
	} // EditableModel
} // Dv

#endif //  _EditableModel_Model_h_