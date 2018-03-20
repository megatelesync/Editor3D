#ifndef _EditableModel_h_
#define _EditableModel_h_

#include "EditableModel_Math.h"
#include "ModelEdgeHandleBase.h"
#include "ModelPolygonConstHandle_fwd.h"
#include "ModelPolygonHandleBase.h"
#include "ModelVertexHandle.h"
#include "ModelVertexConstHandle_fwd.h"
#include "ModelEdgeConstHandle_fwd.h"
#include "EditableModel_Model.h"
#include "EditableModel_handle_fwd.h"
#include "EditableModel_ModelPolygonHandle.h"
#include <DirectXMath.h>
#include <list>

using namespace DirectX;
using namespace Dv::Util::Container;

namespace Dv
{
	namespace EditableModel
	{	
		EdgeMap GetEdgeMap(const EdgeHashMap& edgeHashMap);
		// Creates a model consisting of the only polygon with a specified set of vertices
		template<class VertexDescIt> ModelPolygonHandle			CreatePolygon						(Model *pModel, int materialIndex, VertexDescIt first, VertexDescIt last);
		// True if container vertex count (disregarding of whether some vertices conincide or not) is less than ever 3
		// (the minimal triangle count)
		inline bool IsTooFewVertices																(const ModelPolygonConstHandle& handle);
		inline bool IsTriangle																		(const ModelPolygonConstHandle& handle);
		inline bool IsQuad																			(const ModelPolygonConstHandle& handle);
		// number of vertices more than 4
		inline bool IsComplexPoly																	(const ModelPolygonConstHandle& handle);
		// find edge, equal to the edge that is located after the edge in the poly's container of edges
		// (returns edge (the edge iterator value we searched for) if no other edge equal edge found)
		inline ModelEdgeConstHandle		FindSuccessingEqualVertex_StopIfEnd							(const ModelEdgeConstHandle& edge);
		inline ModelEdgeHandle			FindSuccessingEqualVertex_StopIfEnd							(const ModelEdgeHandle& edge);
		// Find the first pair of equal edges
		// (if no such pair found, return a pair of equal edges {startEdge,startEdge} )
		inline std::pair<ModelEdgeConstHandle, ModelEdgeConstHandle> FindEqualVertex 				(const ModelEdgeConstHandle& startEdge); 

		// Splits polygon and fixes all vertex and edge links
		// Returns: polygon handle
		ModelPolygonHandle				SplitPolyOrdered				(const ModelEdgeHandle& firstEdgeVert, const ModelEdgeHandle& lastEdgeVert);
		
		// Checks whether a specified polygon contains an edge that is linked with some other edge
		bool IsLinkedByEdge(const ModelPolygonConstHandle& poly);
		// Checks whether at least one vertex of the polygon linked with more than one poly
		bool IsVertexLinked(const ModelPolygonConstHandle& poly);
		// Checks that each edge of the poly is linked with the corresponding start vertex
		bool CheckPolyVertexLinks(const ModelPolygonConstHandle& poly);		
		// Permutate pairs of edges and link edges in the pair if the edges're overlapped.
		// The overlap direction is equal to the direction of the edge edgesStartIt
		// The first edge of the pair is to be taken from range [edgesStartIt, edgesEndIt),
		// the seocnd edge of the pair is to be taken from tange [otherEdgesStartI, otherEdgesEndIt)
		// Iterator: iterator to std::pair<ModelEdgeHandle, EdgeAdjacency>		
		template<class Iterator> void LinkOverlappedEdgePairs(Iterator edgesStartIt, Iterator edgesEndIt, Iterator otherEdgesStartIt, Iterator otherEdgesEndIt);

		// Link pair of edges if the edges are overlapped in the specified direction
		// The function should work correctly independent of whether the edges are
		// counter directed or not.
		// Result: true, if the edges was overlapped (and so, linked)
		bool LinkEdges_IfOverlapped(const Vector& overlapDirection, const ModelEdgeHandle& edge, const ModelEdgeHandle& otherEdge);

		// Unlink a poly from any edge linked to one of its edges		
		void UnlinkPolyFromEdges									(const ModelPolygonHandle& poly);
		// Unlink a poly from any edge linked to one of edges in a given range
		void UnlinkPolyFromEdges									(const ModelEdgeHandle& firstEdge, const ModelEdgeHandle& lastEdge);
		// Unlink each vertex of poly from its edges
		inline void UnlinkPolyFromVertices							(const ModelPolygonHandle& poly);	
		// Unlink vertices of poly edges in a given range
		void UnlinkPolyFromVertices									(const ModelEdgeHandle& startEdge, const ModelEdgeHandle& lastEdge);
		// Links all vertices of poly with correspoding edges of that poly
		void LinkVerticesToPolyEdges								(const ModelPolygonHandle& poly);
		// Remove all vertices of a given poly that wil be orphan if the polygon will be removed
		// Post-condition:
		// 1. All vertices that linked only to this polygon will be removed.
		// 2. The poly itself never removed as the result of the call of this function
		// 3. The edges that started with linked only to this poly vertices
		//	  are to be removed.
		// Returns: number of removed vertices
		inline int RemovePolyOrphanVertices(const ModelPolygonHandle& polyHandle);
 
		// Fixes links of the given poly
		// (both links between vertices and edges, and links between edges and adjacent edges)
		// The poly must contain exactly two vertices (assert constraint).
		void FixLinks_OfTwoVertexPoly(const ModelPolygonHandle& poly);

		// Copy vertex info and change tex coord information
		inline PolygonVertexInfo CopyPolygonVertexInfo(const PolygonVertexInfo& polyVertexInfo, const XMFLOAT2& newTexCoord);
		// Welds start vertices of the edges srcVertex and endVertex
		inline void WeldPolyVertices(const ModelEdgeHandle& srcVertex, const ModelEdgeHandle& destVertex);
		// Welds vertices of poly
		// If bRemoveStartVertex == true, than remove the start vertex,
		// otherwise remove end vertex
		// WARNING: Does not fix the links of the removable vertex
		void									WeldPolyEdgeVertices		(const ModelEdgeHandle& edge, bool bRemoveStartVertex);

		inline ModelEdgeHandle					EdgeThree		(const ModelPolygonHandle& poly);
		inline ModelEdgeConstHandle				EdgeThree		(const ModelPolygonConstHandle& poly);
		inline ModelEdgeHandle					EdgeTwo			(const ModelPolygonHandle& poly);
		inline ModelEdgeConstHandle				EdgeTwo			(const ModelPolygonConstHandle& poly);

		inline ModelEdgeHandle					ExtrudeEdge		(const ModelEdgeHandle& edge, const XMFLOAT3& offset);
		inline ModelPolyHandleRange				Extrude			(const ModelPolygonHandle& poly, XMVECTOR offset);
		// Makes the edge quad.
		// Automatically creates the edge link between the baseEdge and an adjacent edge of the new quad.
		// The adjacent edge is always the first edge of the new quad.
		inline ModelPolygonHandle				MakeEdgeQuad	(const ModelEdgeHandle& edgeHandle);
		inline ModelPolyHandleRange				MakeEdgeQuads	(const ModelPolygonHandle& poly);
		// Automatically creates the edge link between the baseEdge and an adjacent edge of the new triangle.
		// The adjacent edge is always the first edge of the new quad.
		inline ModelPolygonHandle				MakeEdgeTriangle (const ModelEdgeHandle& edgeHandle, const PolygonVertexInfo& newVertex);
		// Link edges of the side polys
		// Every side poly is allowed to be quad or triangle.
		void									LinkPolySideEdges	(const ModelPolygonHandle& poly);
		inline ModelPolyHandleRange				BridgeEdgeRange
		(
			int materialIndex, 
			const ModelEdgeHandle& startForwardEdge,
			const ModelEdgeHandle& lastFowardEdge,
			const ModelEdgeHandle& startBackwardEdge
		);
		inline ModelPolyHandleRange BridgePolyEdges
		(
			int materialIndex,
			const ModelPolygonHandle& poly,
			const ModelEdgeHandle& startBackwardEdge
		);

		inline ModelPolygonHandle BridgeEdgeVertices
		(
			int materialIndexIn,			
			const ModelEdgeHandle& startForwardEdge, const ModelEdgeHandle& endBackwardEdge
		);
		inline ModelPolygonHandle DetachByVertexDuplication
		(
			const ModelPolygonHandle& poly,
			Model *pModelToCopyInto
		);
		inline void SubstituteModelVertices
		(
			const PolygonVertexContIt& startPolyVertIt, 
			const VertexContRange& modelVertexRange
		);		
		inline VertexContRange DuplicateVertices
		(
			const PolygonVertexContConstIt& startVertIt, const PolygonVertexContConstIt& endVertIt,
			Model *pModelToCopyInto
		);
		inline VertexContRange DuplicateVertices
		(
			const ModelPolygonHandle& poly,
			Model *pModelToCopyInto
		);
		inline VertexContRange DuplicateVertices
		(
			const ModelPolygonInfo& poly,
			Model *pModelToCopyInto
		);
		inline void TransformPosition
		(
			const ModelEdgeHandle& firstEdge, 
			const ModelEdgeHandle& lastEdge,
			const XMMATRIX& mat
		);
		inline void TransformPosition
		(
			const PolygonVertexContIt& startVertIt, const PolygonVertexContIt& endVertIt, 
			const XMMATRIX& mat
		);
		inline void TransformPosition(const ModelPolygonInfo& poly, const XMMATRIX& mat);
		inline void TransformPosition(const ModelPolygonHandle& poly, const XMMATRIX& mat);
		inline XMVECTOR CalcNormal(const ModelPolygonInfo& poly);
		inline XMVECTOR CalcNormal(const ModelPolygonConstHandle& poly);
		// Flips normal and sets all the vertex and edges links correctly
		void FlipNormal(const ModelPolygonHandle& poly);

		/*
		template<class PolyHandleTypeArg, class EdgeIteratorTypeArg, class VertexHandleTypeArg>
		class ModelEdgeHandleBase
		{
		public:
			typedef ModelEdgeHandleBase<PolyHandleTypeArg, EdgeIteratorTypeArg, VertexHandleTypeArg> ThisType;
			typedef PolyHandleTypeArg PolyHandleType;
			typedef EdgeIteratorTypeArg EdgeIteratorType; 
			typedef VertexHandleTypeArg VertexHandleType; 

			inline ModelEdgeHandleBase() {}
			inline ModelEdgeHandleBase(const typename PolyHandleType::PolygonHandleBaseType & polyHandleIn, EdgeIteratorTypeArg startVertexIterIn);			
			inline operator ModelEdgeConstHandle() const { return ModelEdgeConstHandle(_polyHandle, _startVertexIterator); }

			inline typename PolyHandleType::ModelType		 *GetModel()		  const { return _polyHandle.GetModel(); }
			inline const PolyHandleType&					  GetPolygon()		  const { return _polyHandle; }		
			inline EdgeIteratorType							  GetVertexIterator() const { return _startVertexIterator; }

			inline VertexHandleType							  GetStartModelVertexHandle() const;
			inline VertexHandleType						      GetEndModelVertexHandle() const;
			inline ThisType									  GetNextEdge() const;
			inline ThisType									  GetPreviousEdge() const;

			inline const PolygonVertexInfo&					  GetStartVertexInfo() const;
			inline const PolygonVertexInfo&					  GetEndVertexInfo() const;

		protected:
			PolyHandleTypeArg _polyHandle;
			EdgeIteratorTypeArg _startVertexIterator;
		};
		*/

		void WeldEdgeVertices (const ModelEdgeHandle& edge);

		inline bool operator== (const ModelEdgeConstHandle& first, const ModelEdgeConstHandle& second);
		inline bool operator!= (const ModelEdgeConstHandle& first, const ModelEdgeConstHandle& second);
		inline bool operator<  (const ModelEdgeConstHandle& first,  const ModelEdgeConstHandle& second)
		{
			return &(*first.GetVertexIterator()) < &(*second.GetVertexIterator());
		}
		inline bool AreDifferentPolys					(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle othreEdge);
		inline bool IsSameOwnerPoly						(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle othreEdge);
		inline int AreCoincide							(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& otherEdge);
		inline bool AreCoincideForward					(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& otherEdge);
		inline bool AreCoincideCounter					(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& otherEdge);
		// true, if the start and end vertices of the edges coalesced
		inline bool IsVertex							(const ModelEdgeConstHandle& edge);
		inline int GetIncidenceDirection				(const ModelEdgeHandle& first, const ModelEdgeHandle& second);
		// if bStart == true, returns start vertex info, otherwise - returns end vertex info
		inline PolygonVertexInfo 		GetVertexInfo	(const ModelEdgeHandle& first, bool bStart);
		// if bNext == true, returns next edge handle, otherwise returns previous edge handle
		inline ModelEdgeHandle			NextOrPrev		(const ModelEdgeHandle& edge, bool bNext);
		inline ModelEdgeConstHandle		NextOrPrev		(const ModelEdgeConstHandle& edge, bool bNext);	
		// Get pair, where first == position of start and second == position of end
		inline VectorPair				GetStartEnd		(const ModelEdgeConstHandle& edge);
		// Get pair, where vertices are inversed if bReverse == true;
		inline VectorPair				GetStartEnd		(const ModelEdgeConstHandle& edge, bool bReverse);

		// Get order of two vertices in direction of the edge
		// Params:
		//		bCounterDirection: true if the edge should be counter-directed before the examination
		// Returns:
		//		0, if both points are treated equally
		//		1, if the secondVert goes after the firstVert
		//		-1, if the firstVert goes after the secondVert
		inline Scalar GetVertexOrder		(const ModelEdgeConstHandle& edge, bool bCounterDirected, const Vector& firstVert, const Vector& secondVert);
		// returns true if two edges have different direction
		inline bool	  AreCounterDirected	(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& otherEdge);
		// returns true if the vertex is contained in the edge
		bool	  EdgeContainsVertex	(const ModelEdgeConstHandle& edge, const ModelVertexConstHandle& vertex);

		// Find the first of adjacent edge of two edges.
		// Returns: pair <adjacent edge handle, adjacency info)
		// If the edge has no adjacent edges, then the returned adjacent edge equal to edge provided as argument.
		std::pair<ModelEdgeConstHandle,EdgeAdjacency>		FirstAdjacentEdge								(const ModelEdgeConstHandle& edge);
		//std::pair<ModelEdgeHandle,EdgeAdjacency>			FirstAdjacentEdge								(const ModelEdgeHandle& edge);
		ModelEdgeConstHandle								GetCoincideVertexEdge_ForStartVertexOfEdge		(const ModelEdgeConstHandle& edge);
		ModelEdgeHandle										GetCoincideVertexEdge_ForStartVertexOfEdge		(const ModelEdgeHandle& edge);

		// Find edge handle, coinciding and codirected with a given edge.
		// It will search the first edge among edges linked with start of of the edge provided as argument
		// If there's no codirected edge: return null ModelEdgeHandle
		ModelEdgeHandle FindCodirectedCoincideEdge_ByLinkedStartVert					(const ModelEdgeConstHandle& edge);
		// Find handle of edge, coinciding and counter directed with a given edge
		ModelEdgeHandle FindCounterDirectedCoincideEdge_ByLinkedEndVert					(const ModelEdgeConstHandle& edge);
		// Find handle of edge that coincides with a given edge
		// The edge should be linked with one of its vertices
		ModelEdgeHandle FindCoincideEdge_ByLinkedVerts									(const ModelEdgeConstHandle& edge);		
		// Find edge that contains a given vertex
		// within a set of edges adjacent to the given edge
		// returns: null edge handle if no such edge
		ModelEdgeHandle FindAdjacentEdgeContainingVertex								(const ModelEdgeConstHandle& edge, const ModelVertexConstHandle& vertexToFind);		
		
		// Checks whether a specified edge (edge) is linked with (and only with!) other edge (adjEdge)
		// and vice versa.
		inline bool						AreSingleLinked											(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& adjEdge);
		// Checks whether a specified edge (edge) is linked with (and only with!) other edge (adjEdge)
		inline bool						IsSingleLinkedTo										(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& adjEdge);
		inline bool						IsSingleLinked											(const ModelEdgeConstHandle& edge);
		inline int						NumLinkedEdges   										(const ModelEdgeConstHandle& edge);
		inline bool						IsLinked												(const ModelEdgeConstHandle& edge);
		inline bool						AreLinked												(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& otherEdge);
		// Link specified edges and mark the link as counter-link
		inline void						LinkCounter												(const ModelEdgeHandle& edge, const ModelEdgeHandle& otherEdge);
		// Link specified edges with marking the link as forward-link
		inline void						Link													(const ModelEdgeHandle& edge, const ModelEdgeHandle& otherEdge);
		// Edge link operations		
		inline EdgeAdjacency			GetLinkInfo												(const ModelEdgeConstHandle &edgeOne, const ModelEdgeConstHandle &edgeTwo);
		inline EdgeMap					GetEdgeLinkMap											(const ModelEdgeConstHandle& edge);
		// Replace link of edge to oldLinkedEdge with link of edge to newLinkedEdge
		inline void						MoveEdgeLink											(const ModelEdgeHandle& edge, const ModelEdgeHandle& oldLinkedEdge, const ModelEdgeHandle& newLinkedEdge);
		// copy link to edge from oldLinkedEdge to newLinkedEdge
		inline void						CopyEdgeLink											(const ModelEdgeHandle& edge, const ModelEdgeHandle& oldLinkedEdge, const ModelEdgeHandle& newLinkedEdge);
		// moves or copies edge link from edge to nextEdge
		// depending whether the adjEdge overlaps edge and
		// whether the edge overlaps nextEdge
		// Returns:
		//		-1 - no moving or copying occured (adjEdge belongs to edge only)
		//		0 - only copying occured (adjEdge belongs to both)
		//		1 - both moving and copying occured (adjEdge belongs to next edge only)
		int								MoveOrCopyEdgeLinkToNextEdge_IfNecessary				(const Vector& direction, const ModelEdgeHandle& edge, const ModelEdgeHandle& adjEdge);
		// For every edge link of firstEdge examine whether it should be linked to next edge instead
		// (the edge common to both first edge and next edge will not be moved)
		void							MoveEachEdgeLink_FromEdgeToNextEdge_IfNecessary			(const ModelEdgeHandle& firstEdge);
		// Replaces all links between start vertex of oldEdge so that instead of oldEdge the vertex is linked with newEdge
		void							ReplaceEdge_InEdgeToVertexLinks							(const ModelEdgeHandle& oldEdge, const ModelEdgeHandle& newEdge);
		// Replaces all links between vertex with handle vertexHandle
		// so that instead of oldEdge the vertex is linked with newEdge
		void							ReplaceEdge_InEdgeToVertexLinks							(const ModelVertexHandle& vertexHandle, const ModelEdgeHandle& oldEdge, const ModelEdgeHandle& newEdge);
		void							ReplaceEdge_InEdgeToEdgeLinks							(const ModelEdgeHandle& oldEdge, const ModelEdgeConstHandle& newEdge);
		void							MakeEdgeToEdgeLink_ByLinkedVerts						(const ModelEdgeHandle& edge);		

		// Copies edge sets for each of the edges in the range of poly edges
		EdgeMapSequence					CopyEdgeLinkSetSequence_CorrespodingPolyEdges			(const ModelEdgeConstHandle& firstEdge, const ModelEdgeConstHandle& lastEdge);
		// Unlinks and copies edge sets for each of the edges in the range of poly edges
		EdgeMapSequence					MoveEdgeLinkSetSequence_CorrespondingPolyEdges			(const ModelEdgeHandle& startEdge, const ModelEdgeHandle& lastEdge);
		// Links edges of polys within a given range with corresponding edges
		// in linkMapsCorrespondingEdges
		void							LinkPolyEdges											(const ModelEdgeHandle& startEdge, const ModelEdgeConstHandle& lastEdge, const EdgeMapSequence& linkMapsCorrespondingEdges);
		void							LinkPolyEdges											(const ModelEdgeHandle& startEdge, const ModelEdgeHandle& lastEdge, EdgeMapSequenceIt firstEdgeMapIt, EdgeMapSequenceIt lastEdgeMapIt);
		// Link each edge within the key-value pairs in the edgesToLinkWith.
		// The adjacency link information is provided as the value in the key-value pair.
		void							LinkEdgeTo												(const ModelEdgeHandle& edge, const EdgeMap& edgesToLinkWith);

		// Inverses direction of link between edge and adjEdge
		inline void						InverseEdgeLink											(const ModelEdgeHandle& edge, const ModelEdgeConstHandle& adjEdge);
		// Inverses all edges links between this edge and other edges
		inline void						InverseEdgeLinks										(const ModelEdgeHandle& edge);

		// Remove specified model edge (which is denoted as one of its polygon edges)
		// from both (or one poly, if the edge has no adjacent edges) polys that are adjacent to this edge.		
		// The vertex itself is not to be removed.
		// WARNING: The edge must have at most only one coinciding edge
		// WARNING: Edge adjacency information must be correctly set
		void							RemoveModelEdge_FromBothAdjacentPolys					(const ModelEdgeHandle& edge);
		// Removes the poly edge and all of its adjacency links (both vertex and edge adjacency links)
		void							RemoveEdgeAndLinks										(const ModelEdgeHandle& edgeToRemove);

		// Copy vertices of the edge into the model pModelToCopyInto
		inline ModelVertexHandlePair DuplicateVertices(const ModelEdgeHandle& edge, Model *pModelToCopyInto);		
		
		inline void TransformPosition(const ModelEdgeHandle& edge, const XMMATRIX& mat);
		// Split an edge with a given coeff
		inline ModelEdgeHandle SplitEdge(const ModelEdgeHandle &edge, float coeff);		
		// Split an edge with a given splitter vertex
		inline ModelEdgeHandle SplitEdge(const ModelEdgeHandle &edge, const PolygonVertexInfo& splitterVertex);	
		// Split a set of edges (the edges may belong to different polys)
		template<class EdgeIterator> void SplitPolyEdges (EdgeIterator firstEdgeIt, EdgeIterator lastEdgeIt);
		// Lerp edge and return vertex info
		inline ModelVertexInfo LerpEdgeVertex(const ModelEdgeConstHandle& edge, float coeff);
		// Get direction vector of the edge
		inline XMVECTOR Direction(const ModelEdgeConstHandle& edge);
		// lerp the edge position
		inline XMVECTOR Lerp(const ModelEdgeConstHandle& edge, float coeff);
		// Get length of the  edge
		inline XMVECTOR Length(const ModelEdgeConstHandle& edge);
		// Get direction vector of the edge texcoord
		inline XMVECTOR DirectionTexCoord(const ModelEdgeConstHandle& edge);
		// lerp the tex coord of the edge
		inline XMVECTOR LerpTexCoord(const ModelEdgeConstHandle& edge, float coeff);
		inline Vector	StartPosition(const ModelEdgeConstHandle& edge);
		inline Vector	EndPosition(const ModelEdgeConstHandle& edge);
		inline Vector&	StartPosition(const ModelEdgeHandle& edge);		
		inline Vector&	EndPosition(const ModelEdgeHandle& edge);
		inline Vector	StartTexCoord(const ModelEdgeConstHandle& edge);
		inline Vector	EndTexCoord(const ModelEdgeConstHandle& edge);
		inline Vector&	StartTexCoord(const ModelEdgeHandle& edge);
		inline Vector&	EndTexCoord(const ModelEdgeHandle& edge);
		// calculate the normal by three edge start vertices
		inline XMVECTOR CalcNormal
		(
			const ModelEdgeConstHandle& vertexEdgeOne,
			const ModelEdgeConstHandle& vertexEdgeTwo,
			const ModelEdgeConstHandle& vertexEdgeThree
		);
		// calculate the normal by three start vertex iterators
		inline XMVECTOR CalcNormal
		(
			const PolygonVertexContConstIt& vertexEdgeOneIt,
			const PolygonVertexContConstIt& vertexEdgeTwoIt,
			const PolygonVertexContConstIt& vertexEdgeThreeIt
		);			
		// calculate the iterator distance between the two edges
		inline int IterDistance(const ModelEdgeConstHandle& first, const ModelEdgeConstHandle& last);		

		

		// Model vertex basic operations impl		
		inline VertexContIt Model::GetVertexBeginIterator()
		{
			return _vertices.begin();
		}
		inline VertexContIt Model::GetVertexEndIterator()
		{
			return _vertices.end();
		}
		inline VertexContConstIt Model::GetVertexBeginConstIterator() const
		{
			return _vertices.cbegin();
		}
		inline VertexContConstIt Model::GetVertexEndConstIterator() const
		{
			return _vertices.cend();
		}
		inline int Model::GetVertexCount() const
		{
			return _vertices.size();
		}
		inline ModelVertexHandle Model::AddVertex(const ModelVertexInfo& vertexInfoIn)
		{
			auto newVertexData = ModelPerVertexData(vertexInfoIn);
			auto newVertexDataIt = _perVertexData.insert(_perVertexData.end(), newVertexData);
			return ModelVertexHandle(this, newVertexDataIt);
			// deprecated
			// {
			//auto newVertexIt = _vertices.insert(_vertices.end(), vertexInfoIn);
			//return ModelVertexHandle(this, newVertexIt);
			// }
		}
		inline void Model::RemoveVertex(const ModelVertexHandle& vertexHandleIn)
		{
			RemoveLinkedPolys(vertexHandleIn);
			_perVertexData.erase(vertexHandleIn.GetPerVertexDataIterator());
		}

		//  Model polygon basic operations impl
		inline PolygonIt Model::GetPolyBeginIterator()
		{
			return _polygons.begin();
		}

		inline PolygonIt Model::GetPolyEndIterator()
		{
			return _polygons.end();
		}
		inline PolygonConstIt Model::GetPolyBeginConstIterator() const
		{
			return _polygons.cbegin();
		}
		inline PolygonConstIt Model::GetPolyEndConstIterator() const
		{
			return _polygons.cend();
		}

		inline int Model::GetPolyCount() const
		{
			return _polygons.size();
		}

		inline void Model::RemovePoly(const ModelPolygonHandle& polyHandle)
		{
			RemovePolyOrphanVertices(polyHandle);
			UnlinkPolyFromVertices(polyHandle);			
			_polygons.erase(polyHandle.GetPolygonIterator());
		}

		inline ModelPolygonHandle Model::AddPoly(const ModelPolygonInfo& polyInfo)
		{
			auto newIt = _polygons.insert(_polygons.end(), polyInfo);
			ModelPolygonHandle handle = ModelPolygonHandle(this,newIt);
			LinkVerticesToPolyEdges(handle);
			return handle;
		}

		// ModelPolygonHandleBase impl		
		// ModelPolygonHandle impl
		inline ModelPolygonHandle::ModelPolygonHandle(Model *pModelIn, const PolygonIt& polygonIteratorIn) : 
			ModelPolygonHandleBase(pModelIn, polygonIteratorIn)
		{
		}		

		inline void Splice
		(			
			PolygonVertexContIt destSpliceStartIt,
			PolygonIt polyToSplitIt,
			PolygonVertexContIt srcSpliceStartIt,
			PolygonVertexContIt srcSpliceEndIt,			
			ModelPolygonInfo& newPolyInfo
		)
		{
			// assert that range to splice is not empty
			assert(srcSpliceStartIt != srcSpliceEndIt);
			// forming up the new polygon's vertex list by splicing
			newPolyInfo.vertices.splice
			(
				destSpliceStartIt, 
				polyToSplitIt->vertices, 
				srcSpliceStartIt, srcSpliceEndIt
			);
		}

		// Splices edges in range (startVertexEdge, endVertexEdge)
		// and inserts them into the newPolyInfo AFTER the first element
		// (WARNING! The interval does not include startVertexEdge and endVertexEdge)
		// Links of edges in range [startVertexEdge, endVertexEdge) only will be returned
		inline EdgeMapSequence SpliceAndGetOldEdgeLinks
		(
			const ModelPolygonHandle& polyToSplit,
			const ModelEdgeHandle& startVertexEdge,
			const ModelEdgeHandle& endVertexEdge,			
			ModelPolygonInfo& newPolyInfo
		)
		{
			EdgeMapSequence edgeMapSequence = MoveEdgeLinkSetSequence_CorrespondingPolyEdges(startVertexEdge, endVertexEdge);
			
			// start vertex of the splitter edge iterator
			auto splitterStartVertIt = startVertexEdge.GetVertexIterator();
			// end vertex of the splitter edge iterator
			auto splitterEndVertIt = endVertexEdge.GetVertexIterator();		
			// where we want to splice into
			auto destSpliceStartIt = std::next(newPolyInfo.vertices.begin());
			//++destSpliceStartIt;
			// start iterator to the position from which we want to splice out
			auto srcSpliceStartIt = std::next(splitterStartVertIt);
			//++srcSpliceStartIt;
			// end iterator to the position from which we want to splice out
			auto srcSpliceEndIt = splitterEndVertIt;			
			// forming up the new polygon's vertex list by splicing
			Splice(destSpliceStartIt, polyToSplit.GetPolygonIterator(), srcSpliceStartIt, srcSpliceEndIt, newPolyInfo);

			/*
			newPolyInfo.vertices.splice
			(
				destSpliceStartIt, 
				polyToSplit._polygonIterator->vertices, 
				srcSpliceStartIt, srcSpliceEndIt
			);
			*/

			return edgeMapSequence;
		}

		inline ModelPolygonHandle ModelPolygonHandle::SplitOrdered
		(
			const ModelEdgeHandle& startVertexEdge, 
			const ModelEdgeHandle& endVertexEdge
		) const
		{	
			ModelVertexHandle splitterStartVertex = startVertexEdge.GetStartModelVertexHandle();
			ModelVertexHandle splitterEndVertex = endVertexEdge.GetStartModelVertexHandle();
			// model we work on
			Dv::EditableModel::Model *pModel = startVertexEdge.GetModel();
			// poly we want to split
			Dv::EditableModel::ModelPolygonHandle polyToSplit = startVertexEdge.GetPolygon();
			// new poly that is to be formed as the split result
			ModelPolygonInfo newPolyInfo = ModelPolygonInfo
			(
				GetPolyInfo().materialIndex, 
				startVertexEdge.GetStartVertexInfo(), endVertexEdge.GetStartVertexInfo()
			);	

			UnlinkPolyFromVertices(startVertexEdge, endVertexEdge);
			EdgeMapSequence oldLinkedEdgeMapSequence = SpliceAndGetOldEdgeLinks(polyToSplit, startVertexEdge, endVertexEdge, newPolyInfo);			

			// now the endVertexEdge points to the splitter edge of the splitted polygon
			// register the new polygon
			ModelPolygonHandle newPolyHandle = pModel->AddPoly(newPolyInfo);			
			// Link common edges with vertices that they out from:
			// a)  
			// b)
			// Fix link between adjacent common edges			

			ModelEdgeHandle firstSplicedEdge = newPolyHandle.GetFirstEdge().GetNextEdge();
			ModelEdgeHandle lastSplicedEdge = newPolyHandle.GetLastEdge();
			/*
			// OLD:
			// Fix links to the edges that were spliced into a new poly:
			// Vertices of these edges at this point linked with 
			// the edges that was spliced from the old poly.
			// So, we need to link them with edges of the added poly.
			// Besides, all this links except the common vertices should be unlinked.
			//LinkVerticesToPolyEdges(newPolyHandle);			
			//UnlinkPolyFromVertices(newPolyHandle.GetFirstEdge().GetNextEdge(), newPolyHandle.GetLastEdge().GetPreviousEdge());
			*/
			// All links of edges spliced into the new poly and their adjacent edges
			// should be transformed into the links between the old adjacent edges and
			// the new edges corresponding the spliced poly vertices
			LinkPolyEdges(newPolyHandle.GetFirstEdge(), lastSplicedEdge, oldLinkedEdgeMapSequence);			

			ModelEdgeHandle splitterEdge = startVertexEdge;
			ModelEdgeHandle adjSplitterEdge = newPolyHandle.GetLastEdge();
			LinkCounter(splitterEdge, adjSplitterEdge);
			LinkEdgeStartToVertex(splitterEdge);
			return newPolyHandle;
		}

		inline ModelEdgeHandle ModelPolygonHandle::SplitEdge(const ModelEdgeHandle &edge, const PolygonVertexInfo& splitterVertex) const
		{
			Model *pModel = edge.GetModel();
			ModelVertexHandle splitterVertexHandle(pModel, splitterVertex.vertexIterator);
			// polygon on which adjacent edge the splitter vertex is lying on
			ModelPolygonHandle 		adjEdgePoly;
			// edge of the poly produced during the split of the adjacent edge
			ModelEdgeHandle 		newAdjEdge;
			// Split edge passed as the split operation argument
			ModelEdgeHandle newEdge = SplitPolyEdge(edge, splitterVertex);
			// Split adjacent edge if there's one
			ModelEdgeHandle adjEdgeToSplit = FindAdjacentEdgeContainingVertex(edge, splitterVertexHandle);			
			// Adjacency between edge passed as argument and its adjacent edge
			EdgeAdjacency  edgeAdj;
			if(!adjEdgeToSplit.IsNull())
			{
				edgeAdj = GetLinkInfo(edge, adjEdgeToSplit);
				adjEdgePoly = adjEdgeToSplit.GetPolygon();
				newAdjEdge = adjEdgePoly.SplitPolyEdge(adjEdgeToSplit, splitterVertex);				
			}
			// Move each link of the edge passed as the split operation argument to the newEdge
			MoveEachEdgeLink_FromEdgeToNextEdge_IfNecessary(edge);

			// Link an edge produced during splitting an adjacent edge (if there's one)
			// with an edge passed as the split operation argument
			if(!newAdjEdge.IsNull())
			{				
				pModel->Link(edge, newAdjEdge, edgeAdj);
			}
			return newEdge;
		}

		inline ModelEdgeHandle ModelPolygonHandle::SplitPolyEdge(const ModelEdgeHandle& edge, const PolygonVertexInfo& splitterVertex) const
		{
			auto newPolyVertIt = _polygonIterator->vertices.insert(std::next(edge.GetVertexIterator()), splitterVertex);
			ModelEdgeHandle newEdge(*this, newPolyVertIt);
			LinkVertexToEdge_WithoutLinkingEdges(newEdge.GetStartModelVertexHandle(), newEdge);
			return newEdge;
		}

		inline void ModelPolygonHandle::RemoveEdge(const ModelEdgeHandle& edge) const
		{
			_polygonIterator->vertices.erase(edge.GetVertexIterator());
		}

		inline void ModelPolygonHandle::RemoveEdge(const ModelEdgeConstHandle& edge) const
		{
			_polygonIterator->vertices.erase(edge.GetVertexIterator());
		}

		inline ModelEdgeHandle ModelPolygonHandle::MoveEdgeBefore(const ModelEdgeHandle& edgeToMove, const ModelEdgeHandle& whereToMove) const
		{
			ModelPolygonHandle poly = edgeToMove.GetPolygon();
			auto iteratorAfterInsertPosition = whereToMove.GetVertexIterator();
			auto iteratorToEdgeToMove = edgeToMove.GetVertexIterator();
			auto iteratorAfterEdgeToMove = std::next(iteratorToEdgeToMove);
			_polygonIterator->vertices.splice(iteratorAfterInsertPosition, _polygonIterator->vertices, iteratorToEdgeToMove, iteratorAfterEdgeToMove);
			return ModelEdgeHandle(poly, std::prev(iteratorAfterInsertPosition));
		}

		template<class VertexDescIt> ModelPolygonHandle CreatePolygon (Model *pModel, int materialIndex, VertexDescIt first, VertexDescIt last)
		{
			ModelPolygonInfo polyInfo;
			polyInfo.materialIndex = materialIndex;
			VertexDescIt it = first;
			while(true)
			{
				ModelVertexInfo modelVertexInfo = it->vertex;
				ModelVertexHandle vertexHandle = pModel->AddVertex(modelVertexInfo);
				auto polyVertexInfo = PolygonVertexInfo(vertexHandle.GetPerVertexDataIterator(), it->texCoord);
				polyInfo.vertices.push_back(polyVertexInfo);
				++it;
				if(it == last) break;
			}
			ModelPolygonHandle polyHandle = pModel->AddPoly(polyInfo);
			return polyHandle;
		}

		// ModelEdgeHandleBase impl
		template<class PolyHandleTypeArg, class EdgeIteratorTypeArg, class VertexHandleTypeArg>
		ModelEdgeHandleBase<PolyHandleTypeArg,EdgeIteratorTypeArg,VertexHandleTypeArg>::ModelEdgeHandleBase
		(
			const typename PolyHandleType::PolygonHandleBaseType& polyHandleIn, 
			EdgeIteratorTypeArg startVertexIterIn
		) :
			_polyHandle(polyHandleIn),
			_startVertexIterator(startVertexIterIn)
		{
			_hash = reinterpret_cast<size_t>(&(*_startVertexIterator));
		}
		
		template<class PolyHandleTypeArg, class EdgeIteratorTypeArg, class VertexHandleTypeArg>
		typename ModelEdgeHandleBase<PolyHandleTypeArg, EdgeIteratorTypeArg, VertexHandleTypeArg>::VertexHandleType
		ModelEdgeHandleBase<PolyHandleTypeArg, EdgeIteratorTypeArg, VertexHandleTypeArg>::GetStartModelVertexHandle() const
		{
			return VertexHandleType(GetModel(), _startVertexIterator->vertexIterator);
		}

		template<class PolyHandleTypeArg, class EdgeIteratorTypeArg, class VertexHandleTypeArg>
		typename ModelEdgeHandleBase<PolyHandleTypeArg, EdgeIteratorTypeArg, VertexHandleTypeArg>::VertexHandleType 
		ModelEdgeHandleBase<PolyHandleTypeArg, EdgeIteratorTypeArg, VertexHandleTypeArg>::GetEndModelVertexHandle() const
		{
			return GetNextEdge().GetStartModelVertexHandle();
		}

		template<class PolyHandleTypeArg, class EdgeIteratorTypeArg, class VertexHandleTypeArg>
		typename ModelEdgeHandleBase<PolyHandleTypeArg, EdgeIteratorTypeArg, VertexHandleTypeArg>::ThisType 
		ModelEdgeHandleBase<PolyHandleTypeArg, EdgeIteratorTypeArg, VertexHandleTypeArg>::GetNextEdge() const
		{
			if(_startVertexIterator == _polyHandle.GetLastEdge()._startVertexIterator)
			{
				return _polyHandle.GetFirstEdge();
			}
			else
			{
				auto nextEdgeIt = _startVertexIterator;
				++nextEdgeIt;
				ThisType nextEdge(GetPolygon(), nextEdgeIt);
				return nextEdge;
			}
		}

		template<class PolyHandleTypeArg, class EdgeIteratorTypeArg, class VertexHandleTypeArg>
		typename ModelEdgeHandleBase<PolyHandleTypeArg, EdgeIteratorTypeArg, VertexHandleTypeArg>::ThisType 
		ModelEdgeHandleBase<PolyHandleTypeArg, EdgeIteratorTypeArg, VertexHandleTypeArg>::GetPreviousEdge() const
		{			
			if(_startVertexIterator == _polyHandle.GetFirstEdge()._startVertexIterator)
			{
				return _polyHandle.GetLastEdge();
			}
			else
			{
				auto previousEdgeIt = std::prev(_startVertexIterator);
				ThisType previousEdge(GetPolygon(), previousEdgeIt);
				return previousEdge;
			}
		}

		template<class PolyHandleTypeArg, class EdgeIteratorTypeArg, class VertexHandleTypeArg>
		const PolygonVertexInfo& ModelEdgeHandleBase<PolyHandleTypeArg, EdgeIteratorTypeArg, VertexHandleTypeArg>::GetStartVertexInfo() const
		{
			return *_startVertexIterator;
		}

		template<class PolyHandleTypeArg, class EdgeIteratorTypeArg, class VertexHandleTypeArg>
		const PolygonVertexInfo& ModelEdgeHandleBase<PolyHandleTypeArg, EdgeIteratorTypeArg, VertexHandleTypeArg>::GetEndVertexInfo() const
		{
			return GetNextEdge().GetStartVertexInfo();
		}

		// ModelEdgeHandle impl
		inline bool operator== (const ModelEdgeConstHandle& first, const ModelEdgeConstHandle& second)
		{
			return first.GetPolygon() == second.GetPolygon() && 
				   first.GetVertexIterator() == second.GetVertexIterator();
		}

		inline bool operator!= (const ModelEdgeConstHandle& first, const ModelEdgeConstHandle& second)
		{
			return first.GetPolygon() != second.GetPolygon() ||
				   first.GetVertexIterator() != second.GetVertexIterator();
		}

		inline const PolygonVertexInfo& GetEndVertexInfo(const ModelEdgeHandle& edgeHandle);

		// Vertex operations
		inline const PolygonVertexInfo& GetEndVertexInfo(const ModelEdgeHandle& edgeHandle)
		{
			return edgeHandle.GetEndVertexInfo();
		}		

		inline XMVECTOR CalcNormal
		(
			const ModelVertexConstHandle& vertOne,
			const ModelVertexConstHandle& vertTwo,
			const ModelVertexConstHandle& vertThree
		)
		{
			return CalcNormal(vertOne.GetVertexInfo(), vertTwo.GetVertexInfo(), vertThree.GetVertexInfo());
		}

		// Edge operations
		inline bool AreDifferentPolys(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle otherEdge)
		{
			return edge.GetPolygon().GetPolygonIterator() != otherEdge.GetPolygon().GetPolygonIterator();
		}

		inline bool IsSameOwnerPoly(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& otherEdge)
		{
			return edge.GetPolygon().GetPolygonIterator() == otherEdge.GetPolygon().GetPolygonIterator();
		}

		inline bool AreCoincideForward(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& otherEdge)
		{
			bool bEdgeStartEqOtherStart = edge.GetStartModelVertexHandle() == otherEdge.GetStartModelVertexHandle();
			bool bEdgeEndEqOtherEnd = edge.GetEndModelVertexHandle() == otherEdge.GetEndModelVertexHandle();
			return bEdgeStartEqOtherStart == bEdgeEndEqOtherEnd;
		}

		inline bool AreCoincideCounter(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& otherEdge)
		{
			bool bEdgeStartEqOtherEnd = edge.GetStartModelVertexHandle() == otherEdge.GetEndModelVertexHandle();
			bool bEdgeEndEqOtherStart = edge.GetEndModelVertexHandle() == otherEdge.GetStartModelVertexHandle();
			return bEdgeStartEqOtherEnd && bEdgeEndEqOtherStart;
		}


		inline int AreCoincide(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& otherEdge)
		{
			if(AreCoincideCounter(edge, otherEdge)) { return -1; }
			if(AreCoincideForward(edge, otherEdge)) { return 1; }			
			return 0;
		}

		inline bool IsVertex(const ModelEdgeConstHandle& edge)
		{
			return edge.GetStartModelVertexHandle() == edge.GetEndModelVertexHandle();
		}

		inline int GetIncidenceDirection(const ModelEdgeHandle& first, const ModelEdgeHandle& second)
		{
			if(first.GetNextEdge() == second) { return 1; }
			if(first.GetPreviousEdge() == second) { return -1; }
			return 0;
		}
		inline bool	AreCounterDirected(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& otherEdge)
		{
			Vector edgeDirection = Direction(edge);
			Vector otherEdgeDirection = Direction(otherEdge);
			if(Dot3(edgeDirection, otherEdgeDirection) < 0) { return true; }
			return false;
		}

		inline ModelVertexHandlePair DuplicateVertices(const ModelEdgeHandle& edge, Model *pModelToCopyInto)
		{
			ModelVertexHandle startVertexCopy = DuplicateVertex(edge.GetStartModelVertexHandle(), pModelToCopyInto);
			ModelVertexHandle endVertexCopy = DuplicateVertex(edge.GetEndModelVertexHandle(), pModelToCopyInto);
			return std::make_pair(startVertexCopy, endVertexCopy);
		}

		inline void TransformPosition(const ModelEdgeHandle& edge, const XMMATRIX& mat)
		{
			const ModelVertexHandle startVert = edge.GetStartModelVertexHandle();
			const ModelVertexHandle endVert = edge.GetEndModelVertexHandle();
			TransformPosition(startVert, mat);
			TransformPosition(endVert, mat);
		}

		inline XMVECTOR CalcNormal
		(
			const ModelEdgeConstHandle& vertexEdgeOne,
			const ModelEdgeConstHandle& vertexEdgeTwo,
			const ModelEdgeConstHandle& vertexEdgeThree
		)
		{
			return CalcNormal
			(
				vertexEdgeOne.GetVertexIterator(),
				vertexEdgeTwo.GetVertexIterator(),
				vertexEdgeThree.GetVertexIterator()
			);
		}

		inline XMVECTOR CalcNormal
		(
			const PolygonVertexContConstIt& vertexEdgeOneIt,
			const PolygonVertexContConstIt& vertexEdgeTwoIt,
			const PolygonVertexContConstIt& vertexEdgeThreeIt
		)
		{
			return CalcNormal
			(
				vertexEdgeOneIt->vertexIterator->info, 
				vertexEdgeTwoIt->vertexIterator->info, 
				vertexEdgeThreeIt->vertexIterator->info
			);
		}

		inline ModelEdgeHandle SplitEdge(const ModelEdgeHandle &edge, const PolygonVertexInfo& splitterVertex)
		{
			return edge.GetPolygon().SplitEdge(edge, splitterVertex);
		}

		inline ModelEdgeHandle SplitEdge(const ModelEdgeHandle& edge, float coeff)
		{
			ModelVertexInfo splitterVertexInfo = LerpEdgeVertex(edge, coeff);
			ModelVertexHandle splitterVertex = edge.GetModel()->AddVertex(splitterVertexInfo);
			XMVECTOR splitterVertexTexCoord = LerpTexCoord(edge, coeff);
			PolygonVertexInfo polygonVertexInfo(splitterVertex.GetPerVertexDataIterator(), splitterVertexTexCoord);
			ModelEdgeHandle newEdge = SplitEdge(edge, polygonVertexInfo);
			return newEdge;
		}

		template<class EdgeIterator> void SplitPolyEdges (EdgeIterator firstEdgeIt, EdgeIterator lastEdgeIt)
		{
			EdgeIterator it = firstEdgeIt;
			while(it != lastEdgeIt)
			{
				ModelEdgeHandle edge = *it;
				ModelPolygonHandle poly = edge.GetPolygon();
				SplitEdge(edge, 0.5F);
				++it;
			}
		}

		inline ModelVertexInfo LerpEdgeVertex(const ModelEdgeConstHandle& edge, float coeff)
		{
			XMVECTOR newVertexPositionVec = Lerp(edge, coeff);
			ModelVertexInfo newVertexInfo(newVertexPositionVec);
			return newVertexInfo;
		}

		inline Vector	StartTexCoord(const ModelEdgeConstHandle& edge)
		{
			return edge.GetStartVertexInfo().GetTexCoordVector();
		}

		inline Vector	EndTexCoord(const ModelEdgeConstHandle& edge)
		{
			return edge.GetEndVertexInfo().GetTexCoordVector();
		}

		inline Vector&	StartTexCoord(const ModelEdgeHandle& edge)
		{
			return edge.GetStartVertexInfo().GetTexCoordVector();
		}

		inline Vector&	EndTexCoord(const ModelEdgeHandle& edge)
		{
			return edge.GetEndVertexInfo().GetTexCoordVector();
		}

		inline XMVECTOR Direction(const ModelEdgeConstHandle& edge)
		{
			XMVECTOR direction 
				= edge.GetEndModelVertexHandle().GetVertexInfo().GetPositionVector() - edge.GetStartModelVertexHandle().GetVertexInfo().GetPositionVector();
			return direction;
		}
		
		inline XMVECTOR Lerp(const ModelEdgeConstHandle& edge, float coeff)
		{
			XMVECTOR direction = Direction(edge);
			XMVECTOR res = edge.GetStartModelVertexHandle().GetVertexInfo().GetPositionVector() + direction * coeff;
			return res;
		}

		inline XMVECTOR Length(const ModelEdgeConstHandle& edge)
		{
			XMVECTOR direction = Direction(edge);
			XMVECTOR res = XMVector3Length(direction);
			return res;
		}

		inline XMVECTOR DirectionTexCoord(const ModelEdgeConstHandle& edge)
		{
			XMVECTOR direction 
				= edge.GetEndVertexInfo().GetTexCoordVector() -
				  edge.GetStartVertexInfo().GetTexCoordVector();
			return direction;
		}

		inline XMVECTOR LerpTexCoord(const ModelEdgeConstHandle& edge, float coeff)
		{
			XMVECTOR direction = DirectionTexCoord(edge);
			XMVECTOR res = edge.GetStartVertexInfo().GetTexCoordVector() + direction * coeff;
			return res;
		}

		inline int IterDistance(const ModelEdgeConstHandle& first, const ModelEdgeConstHandle& last)
		{
			int dist = 0;
			ModelEdgeConstHandle edge = first;
			while(true)
			{
				if(edge == last) { return dist; }
				++dist;
				edge = edge.GetNextEdge();				
			}			
			return dist;
		}

		inline void UnlinkEdgeStartFromVertex(const ModelEdgeHandle& edge)
		{
			std::list<ModelEdgeHandle>& outEdges = edge.GetStartModelVertexHandle().GetOutEdges();			
			outEdges.remove(edge);
		}

		inline EdgeAdjacency GetLinkInfo(const ModelEdgeConstHandle &edgeOne, const ModelEdgeConstHandle &edgeTwo)
		{
			const auto &adjEdges = edgeOne.GetModel()->GetAdjEdgeHashMap(edgeOne);
			return adjEdges.at(edgeTwo.hash()).adj;
		}

		inline EdgeMap	GetEdgeLinkMap(const ModelEdgeConstHandle& edge)
		{
			const Model *pModel = edge.GetModel();
			return GetEdgeMap(pModel->GetAdjEdgeHashMap(edge));
		}

		inline void MoveEdgeLink(const ModelEdgeHandle& edge, const ModelEdgeHandle& oldLinkedEdge, const ModelEdgeHandle& newLinkedEdge)
		{
			Model *pModel = edge.GetModel();
			auto adjLinkInfo = GetLinkInfo(edge, oldLinkedEdge);
			pModel->Unlink(edge, oldLinkedEdge);
			pModel->Link(edge, newLinkedEdge, adjLinkInfo);
		}

		inline void CopyEdgeLink(const ModelEdgeHandle& edge, const ModelEdgeHandle& oldLinkedEdge, const ModelEdgeHandle& newLinkedEdge)
		{
			Model *pModel = edge.GetModel();
			auto adjLinkInfo = GetLinkInfo(edge, oldLinkedEdge);			
			pModel->Link(edge, newLinkedEdge, adjLinkInfo);
		}

		inline Scalar GetVertexOrder(const ModelEdgeConstHandle& edge, bool bCounterDirected, const Vector& firstVert, const Vector& secondVert)
		{
			Vector adjEdge_Start = edge.GetStartModelVertexHandle().GetVertexInfo().GetPositionVector();
			Vector adjEdge_End = edge.GetEndModelVertexHandle().GetVertexInfo().GetPositionVector();				
			Vector nextEdge_Start = edge.GetEndModelVertexHandle().GetVertexInfo().GetPositionVector();
			Vector direction = Direction(edge); 
			if (bCounterDirected) { direction = -direction; }
			return GetVertexOrder(direction, nextEdge_Start, adjEdge_Start);
		}

		// vertex operations
		inline void RemoveLinkedPolys(const ModelVertexHandle& vert)
		{
			Model *pModel = vert.GetModel();
			auto linkedOutEdges = vert.GetOutEdges();
			for(auto linkedEdge : linkedOutEdges)
			{
				auto &poly = linkedEdge.GetPolygon();
				pModel->RemovePoly(poly);
			}
		}

		// polygon operations
		inline bool IsTooFewVertices(const ModelPolygonConstHandle& handle)
		{
			return handle.GetVertexCount() < 3;
		}

		inline bool IsTriangle(const ModelPolygonConstHandle& handle)
		{
			return handle.GetVertexCount() == 3;
		}

		inline bool IsQuad(const ModelPolygonConstHandle& handle)
		{
			return handle.GetVertexCount() == 4;
		}

		inline bool IsComplexPoly(const ModelPolygonConstHandle& handle)
		{
			return handle.GetVertexCount() > 4;
		}

		inline ModelEdgeConstHandle FindSuccessingEqualVertex_StopIfEnd (const ModelEdgeConstHandle& edge)
		{
			const ModelPolygonConstHandle& poly = edge.GetPolygon();
			for(auto currEdge = edge; (currEdge = currEdge.GetNextEdge()) != poly.GetFirstEdge(); )
			{				
				if(currEdge.GetStartModelVertexHandle() == edge.GetStartModelVertexHandle()) { return currEdge; }
			}
			return edge;
		}

		inline ModelEdgeHandle	FindSuccessingEqualVertex_StopIfEnd	(const ModelEdgeHandle& edge)
		{
			const ModelPolygonHandle& poly = edge.GetPolygon();
			for(auto currEdge = edge; (currEdge = currEdge.GetNextEdge()) != poly.GetFirstEdge(); )
			{				
				if(currEdge.GetStartModelVertexHandle() == edge.GetStartModelVertexHandle()) { return currEdge; }
			}
			return edge;
		}

		inline std::pair<ModelEdgeConstHandle, ModelEdgeConstHandle> FindEqualVertex(const ModelEdgeConstHandle& startEdge)
		{
			ModelPolygonConstHandle poly = startEdge.GetPolygon();
			ModelEdgeConstHandle currEdge = startEdge;
			while(true)
			{
				currEdge = currEdge.GetNextEdge();
				if(currEdge == poly.GetFirstEdge()) { break; }
				ModelEdgeConstHandle& equalVertexEdge = FindSuccessingEqualVertex_StopIfEnd(currEdge);
				if(equalVertexEdge != currEdge) { return std::make_pair(currEdge,equalVertexEdge); }
			}
			return std::make_pair(startEdge,startEdge);
		}

		inline void UnlinkPolyFromVertices(const ModelPolygonHandle& poly)
		{			
			UnlinkPolyFromVertices(poly.GetFirstEdge(), poly.GetFirstEdge());
		}

		template<class Iterator> 
		void LinkOverlappedEdgePairs(Iterator edgesStartIt, Iterator edgesEndIt, Iterator otherEdgesStartIt, Iterator otherEdgesEndIt)		
		{
			/*
			ModelEdgeHandle startEdge = edgesStartIt->first;
			Model *pModel = startEdge.GetModel();
			Vector overlapDirection = Direction(startEdge);
			for(Iterator edgeIt = edgesStartIt; edgeIt != edgesEndIt; ++edgeIt)
			{
				for(Iterator otherEdgeIt = otherEdgesStartIt; otherEdgeIt != otherEdgesEndIt; ++otherEdgeIt)
				{
					ModelEdgeHandle edge = edgeIt->first;
					ModelEdgeHandle otherEdge = otherEdgeIt->first;
					LinkEdges_IfOverlapped(overlapDirection, edge, otherEdge);
				}
			}
			*/
		}

		inline void InverseEdgeLink (const ModelEdgeHandle& edge, const ModelEdgeConstHandle& adjEdge)
		{
			Model *pModel = edge.GetModel();
			auto adjLinkInfo = GetLinkInfo(edge, adjEdge);
			adjLinkInfo.bCounterDirected = !adjLinkInfo.bCounterDirected;
			pModel->Unlink(edge, adjEdge);
			pModel->Link(edge, adjEdge, adjLinkInfo);			
		}

		inline void	InverseEdgeLinks (const ModelEdgeHandle& edge)
		{
			Model *pModel = edge.GetModel();
			auto &adjEdges = pModel->GetAdjEdgeHashMap(edge);
			for(auto adjLink : adjEdges)
			{
				InverseEdgeLink(edge, adjLink.second.adjEdge);
			}
		}

		inline int RemovePolyOrphanVertices(const ModelPolygonHandle& polyHandle)
		{
			int numRemovedVertices = 0;
			Model				*pModel = polyHandle.GetModel();			
			ModelEdgeHandle		 edge = polyHandle.GetFirstEdge();
			std::vector<ModelVertexHandle> linkedVertices;
			while(true)
			{
				ModelVertexHandle					vertex = edge.GetStartModelVertexHandle();
				std::list<ModelEdgeHandle>&			outEdges = vertex.GetOutEdges();
				ModelEdgeHandle						nextEdge = edge.GetNextEdge();
				if(outEdges.size() == 1 && outEdges.front().GetPolygon() == polyHandle)
				{
					// WARNING!
					// Model::RemoveVertex calls RemovePolyOrphanVertices for each linked poly
					// (at this case, only one poly linked: the poly with handle provided as argument to
					// the RemovePolyOrphanVertices function).
					// So, to avoid looping we need to unlink the vertex from the poly.
					outEdges.clear();
					pModel->RemoveVertex(vertex);
					++numRemovedVertices;
					polyHandle.RemoveEdge(edge);
				}				
				edge = nextEdge;
				if(edge == polyHandle.GetFirstEdge())
				{
					return numRemovedVertices;
				}
			}
		}

		inline void WeldPolyVertices(const ModelEdgeHandle& srcVertex, const ModelEdgeHandle& destVertex)
		{
			destVertex.GetModel()->RemoveVertex(destVertex.GetStartModelVertexHandle());
			destVertex.GetVertexIterator()->vertexIterator = srcVertex.GetVertexIterator()->vertexIterator;
		}

		inline PolygonVertexInfo CopyPolygonVertexInfo(const PolygonVertexInfo& polyVertexInfo, const XMFLOAT2& newTexCoord)
		{
			PolygonVertexInfo res = polyVertexInfo;
			res.texCoord = newTexCoord;
			return res;
		}

		inline ModelPolygonHandle MakeEdgeQuad(const ModelEdgeHandle& edgeHandle)
		{
			Model *pModel = edgeHandle.GetModel();
			ModelPolygonHandle edgePoly = edgeHandle.GetPolygon();
			ModelVertexHandlePair edgeCopy = DuplicateVertices(edgeHandle, pModel);
			// creating quad
			ModelPolygonInfo polyInfo (edgePoly.GetPolyInfo().materialIndex);
			PolygonVertexInfo baseEdgeEnd = CopyPolygonVertexInfo(edgeHandle.GetEndVertexInfo(), XMFLOAT2(1.0F,0.0F));
			PolygonVertexInfo baseEdgeStart = CopyPolygonVertexInfo(edgeHandle.GetStartVertexInfo(), XMFLOAT2(0.0F,0.0F));			
			PolygonVertexInfo baseEdgeStartParallel (edgeCopy.first.GetPerVertexDataIterator(), XMFLOAT2(0.0F, 1.0F));
			PolygonVertexInfo baseEdgeEndParallel (edgeCopy.second.GetPerVertexDataIterator(), XMFLOAT2(1.0F, 1.0F));
			polyInfo.vertices.push_back(baseEdgeEnd);
			polyInfo.vertices.push_back(baseEdgeStart);
			polyInfo.vertices.push_back(baseEdgeStartParallel);
			polyInfo.vertices.push_back(baseEdgeEndParallel);
			ModelPolygonHandle res = pModel->AddPoly(polyInfo);	
			// edge adjacent, to the edge on which the edge quad was built
			ModelEdgeHandle baseEdgeAdj = res.GetFirstEdge();
			// links the specified two edges
			LinkCounter(baseEdgeAdj, edgeHandle);
			return res;
		}

		inline ModelPolygonHandle MakeEdgeTriangle(const ModelEdgeHandle& edgeHandle, const PolygonVertexInfo& newVertex)
		{
			Model *pModel = edgeHandle.GetModel();
			ModelPolygonHandle edgePoly = edgeHandle.GetPolygon();
			ModelVertexHandlePair edgeCopy = DuplicateVertices(edgeHandle, pModel);
			ModelPolygonInfo polyInfo (edgePoly.GetPolyInfo().materialIndex);
			PolygonVertexInfo baseEdgeEnd = CopyPolygonVertexInfo(edgeHandle.GetEndVertexInfo(), XMFLOAT2(1.0F,0.0F));
			PolygonVertexInfo baseEdgeStart = CopyPolygonVertexInfo(edgeHandle.GetStartVertexInfo(), XMFLOAT2(0.0F,0.0F));
			polyInfo.vertices.push_back(baseEdgeEnd);
			polyInfo.vertices.push_back(baseEdgeStart);
			polyInfo.vertices.push_back(newVertex);
			ModelPolygonHandle res = pModel->AddPoly(polyInfo);	
			// edge adjacent, to the edge on which the edge quad was built
			ModelEdgeHandle baseEdgeAdj = res.GetFirstEdge();
			// links the specified two edges
			LinkCounter(baseEdgeAdj, edgeHandle);
			return res;
		}

		inline ModelEdgeHandle NextOrPrev(const ModelEdgeHandle& edge, bool bNext)
		{
			if (bNext)
			{
				return edge.GetNextEdge();
			}
			else
			{
				return edge.GetPreviousEdge();
			}
		}
		inline ModelEdgeConstHandle NextOrPrev(const ModelEdgeConstHandle& edge, bool bNext)
		{
			if (bNext)
			{
				return edge.GetNextEdge();
			}
			else
			{
				return edge.GetPreviousEdge();
			}
		}

		inline VectorPair GetStartEnd(const ModelEdgeConstHandle& edge)
		{
			return VectorPair(StartPosition(edge), EndPosition(edge));
		}
		
		inline VectorPair GetStartEnd(const ModelEdgeConstHandle& edge, bool bReverse)
		{
			auto res_pair = GetStartEnd(edge);
			if (bReverse)
			{
				std::swap(res_pair.first, res_pair.second);
			}
			return res_pair;
		}

		inline ModelEdgeHandle EdgeThree(const ModelPolygonHandle& poly)
		{
			return EdgeTwo(poly).GetNextEdge();
		}
		inline ModelEdgeConstHandle EdgeThree(const ModelPolygonConstHandle& poly)
		{
			return EdgeTwo(poly).GetNextEdge();
		}
		inline ModelEdgeHandle EdgeTwo(const ModelPolygonHandle& poly)
		{
			return poly.GetFirstEdge().GetNextEdge();
		}
		inline ModelEdgeConstHandle EdgeTwo(const ModelPolygonConstHandle& poly)
		{
			return poly.GetFirstEdge().GetNextEdge();
		}
		inline ModelEdgeHandle ExtrudeEdge(const ModelEdgeHandle& edge, const XMFLOAT3& offset)
		{
			ModelPolygonHandle newPoly = MakeEdgeQuad(edge);
			XMMATRIX mat = XMMatrixTranslation(offset.x, offset.y, offset.z);
			TransformPosition(EdgeThree(newPoly), mat);
			return edge;
		}
		inline ModelPolyHandleRange Extrude(const ModelPolygonHandle& poly, XMFLOAT3 offset)
		{
			ModelPolyHandleRange sideQuadRange = MakeEdgeQuads(poly);
			XMMATRIX mat = XMMatrixTranslation(offset.x, offset.y, offset.z);
			TransformPosition(poly,mat);
			return sideQuadRange;
		}

		struct MakeEdgeQuadsLoop
		{
		public:
			Model *pModel;
			ModelPolygonHandle poly;
			const XMVECTOR& texCoordUpLeft;
			const XMVECTOR& texCoordUpRight;
			ModelEdgeHandle edgeOut;
			ModelVertexHandle parallelModelVertexOut;
			ModelEdgeHandle nextEdgeOut;
			ModelVertexHandle nextParallelModelVertexOut;
			ModelPolygonHandle addedPolygonOut;		
			ModelVertexHandle firstParallelModelVertex;
			ModelPolygonHandle firstAddedQuad, lastAddedQuad;

			inline bool Iterate_And_Return_Is_EndOfLoop(ModelPolygonHandle& addedPolygonOut);

			ModelPolyHandleRange Compute()
			{
				pModel = poly.GetModel();
				// edge of poly passed as argument
				edgeOut = poly.GetFirstEdge();			
				// model vertex, parralel to the start vertex of edge
				firstParallelModelVertex = pModel->AddVertex(edgeOut.GetStartModelVertexHandle().GetVertexInfo());
				parallelModelVertexOut = firstParallelModelVertex;
				bool bEndOfLoop = Iterate_And_Return_Is_EndOfLoop(firstAddedQuad);				
				if(!bEndOfLoop)
				{
					while(true)			
					{		
						bEndOfLoop = Iterate_And_Return_Is_EndOfLoop(lastAddedQuad);

						// break if all edges iterated
						if (bEndOfLoop) { break; }						
					}
				}
				return std::make_pair(firstAddedQuad,lastAddedQuad);
			}

			MakeEdgeQuadsLoop
			(
				const ModelPolygonHandle& polyIn,
				const XMVECTOR& texCoordUpLeftIn,
				const XMVECTOR& texCoordUpRightIn
			) :
				texCoordUpLeft(texCoordUpLeftIn), 
				texCoordUpRight(texCoordUpRightIn),
				poly(polyIn)
			{

			}
		};

		inline bool MakeEdgeQuadsLoop::Iterate_And_Return_Is_EndOfLoop(ModelPolygonHandle& addedPolygonOut)
		{
			// edge next to the edge
			nextEdgeOut = edgeOut.GetNextEdge();
			// model vertex, parallel to the start vertex of the next edge
			if(edgeOut != poly.GetLastEdge())
			{
				nextParallelModelVertexOut = pModel->AddVertex(nextEdgeOut.GetStartModelVertexHandle().GetVertexInfo());
			}
			else
			{
				nextParallelModelVertexOut = firstParallelModelVertex;
			}			
			// create quad
			auto parallelVertexInfo = PolygonVertexInfo(parallelModelVertexOut.GetPerVertexDataIterator(), texCoordUpLeft);
			auto nextParallelVertexInfo = PolygonVertexInfo(nextParallelModelVertexOut.GetPerVertexDataIterator(), texCoordUpRight);
			ModelPolygonInfo edgeQuadInfo = ModelPolygonInfo
			(
				poly.GetPolyInfo().materialIndex, 
				nextEdgeOut.GetStartVertexInfo(), 
				edgeOut.GetStartVertexInfo(),
				parallelVertexInfo,
				nextParallelVertexInfo
			);
			// add quad
			addedPolygonOut = pModel->AddPoly(edgeQuadInfo);			
			// break if all edges iterated
			if (edgeOut == poly.GetLastEdge()) { return true; }
			// go to the next edge
			edgeOut = nextEdgeOut;
			parallelModelVertexOut = nextParallelModelVertexOut;				
			return false;
		}

		inline ModelPolyHandleRange MakeEdgeQuads(const ModelPolygonHandle& poly)
		{
			XMFLOAT2 texCoordUpLeft = XMFLOAT2(1.0F, 0.0F);
			XMVECTOR texCoordUpLeftVec = XMLoadFloat2(&texCoordUpLeft);
			XMFLOAT2 texCoordUpRight = XMFLOAT2(1.0F, 1.0F);
			XMVECTOR texCoordUpRightVec = XMLoadFloat2(&texCoordUpRight);
			MakeEdgeQuadsLoop methObj(poly,texCoordUpLeftVec,texCoordUpRightVec);
			return methObj.Compute();
		}
	
		inline ModelPolyHandleRange BridgeEdgeRange
		(
			int materialIndex, 
			const ModelEdgeHandle& startForwardEdge,
			const ModelEdgeHandle& lastFowardEdge,
			const ModelEdgeHandle& startBackwardEdge
		)
		{
			ModelPolygonHandle startBridgePoly, lastBridgePoly;
			auto edge = startForwardEdge;
			auto backwardEdge = startBackwardEdge;
			
			lastBridgePoly = startBridgePoly = BridgeEdgeVertices(materialIndex, edge, backwardEdge);
			while(true)
			{
				edge = edge.GetNextEdge();
				backwardEdge = backwardEdge.GetNextEdge();
				if(edge == lastFowardEdge) { break; }		
				lastBridgePoly = BridgeEdgeVertices(materialIndex, edge, backwardEdge);								
			}
			return std::make_pair(startBridgePoly, lastBridgePoly);
		}

		inline ModelPolyHandleRange BridgePolyEdges
		(
			int materialIndex,
			const ModelPolygonHandle& poly,
			const ModelEdgeHandle& startBackwardEdge
		)
		{
			return BridgeEdgeRange(materialIndex, poly.GetFirstEdge(), poly.GetLastEdge(), startBackwardEdge);
		}

		inline ModelPolygonHandle BridgeEdgeVertices
		(
			int materialIndexIn,
			const ModelEdgeHandle& startForwardEdge, const ModelEdgeHandle& endBackwardEdge
		)
		{
			Model *pModel = startForwardEdge.GetModel();
			auto polyInfo = ModelPolygonInfo
			(
				materialIndexIn, 
				startForwardEdge.GetStartVertexInfo(), 
				startForwardEdge.GetEndVertexInfo(),
				endBackwardEdge.GetEndVertexInfo(),
				endBackwardEdge.GetStartVertexInfo()
			);
			ModelPolygonHandle newPoly = pModel->AddPoly(polyInfo);
			return newPoly;
		}
		inline ModelPolygonHandle DetachByVertexDuplication
		(
			const ModelPolygonHandle& poly,
			Model *pModelToCopyInto
		)
		{
			VertexContRange duplicatedVertexRange = DuplicateVertices(poly, pModelToCopyInto);
			SubstituteModelVertices(poly.GetFirstEdge().GetVertexIterator(), duplicatedVertexRange);
		}

		inline void SubstituteModelVertices
		(
			const PolygonVertexContIt& startPolyVertIt, 
			const VertexContRange& modelVertexRange
		)
		{
			PolygonVertexContIt it = startPolyVertIt;
			PerVertexDataIt modelVertIt = modelVertexRange.first;
			while(modelVertIt != modelVertexRange.second)
			{
				it->vertexIterator = modelVertIt;
				++it;
				++modelVertIt;
			}
		}

		inline VertexContRange DuplicateVertices
		(
			ModelPolygonHandle& poly,
			Model *pModelToCopyInto
		)
		{
			return DuplicateVertices(poly.GetPolyInfo(), pModelToCopyInto);
		}

		inline VertexContRange DuplicateVertices
		(
			const ModelPolygonInfo& poly,
			Model *pModelToCopyInto
		)
		{
			return DuplicateVertices(poly.vertices.cbegin(), poly.vertices.cend(), pModelToCopyInto);
		}

		inline VertexContRange DuplicateVertices
		(
			const PolygonVertexContConstIt& startVertIt, const PolygonVertexContConstIt& endVertIt,
			Model *pModelToCopyInto
		)
		{
			PerVertexDataIt firstAddedIt, lastAddedIt;

			PolygonVertexContConstIt it = startVertIt;
			if(it != endVertIt)
			{
				firstAddedIt = pModelToCopyInto->AddVertex(it->vertexIterator->info).GetPerVertexDataIterator();
				while(true)
				{
					++it;
					if(it == endVertIt) { break; }
					lastAddedIt = pModelToCopyInto->AddVertex(it->vertexIterator->info).GetPerVertexDataIterator();
				}
				return std::make_pair(firstAddedIt, lastAddedIt);
			}			
		}		

		inline void TransformPosition
		(
			const ModelEdgeHandle& firstEdge, 
			const ModelEdgeHandle& lastEdge,
			const XMMATRIX& mat
		)
		{
			TransformPosition(firstEdge.GetVertexIterator(), std::next(lastEdge.GetVertexIterator()), mat);
		}

		inline void TransformPosition
		(
			const PolygonVertexContIt& startVertIt, const PolygonVertexContIt& endVertIt, 
			const XMMATRIX& mat
		)
		{
			PolygonVertexContIt it = startVertIt;
			while(it != endVertIt)
			{
				TransformPosition((it->vertexIterator)->info, mat);
				++it;
			}
		}

		inline void TransformPosition(ModelPolygonInfo &poly, const XMMATRIX& mat)
		{
			TransformPosition(poly.vertices.begin(), poly.vertices.end(), mat);
		}

		inline void TransformPosition(const ModelPolygonHandle &poly, const XMMATRIX& mat)
		{
			TransformPosition(poly.GetFirstEdge(), poly.GetLastEdge(), mat);
		}

		inline XMVECTOR CalcNormal(const ModelPolygonInfo& poly)
		{
			auto firstEdgeIt = poly.vertices.cbegin();
			auto secondEdgeIt = std::next(firstEdgeIt);
			auto thirdEdgeIt = std::next(secondEdgeIt);
			return CalcNormal(firstEdgeIt, secondEdgeIt, thirdEdgeIt);
		}

		inline XMVECTOR CalcNormal(const ModelPolygonConstHandle& poly)
		{
			return CalcNormal(poly.GetPolyInfo());
		}

		inline Vector	StartPosition(const ModelEdgeConstHandle& edge)
		{
			return edge.GetStartModelVertexHandle().GetVertexInfo().GetPositionVector();
		}

		inline Vector	EndPosition(const ModelEdgeConstHandle& edge)
		{
			return edge.GetEndModelVertexHandle().GetVertexInfo().GetPositionVector();
		}

		inline Vector&	StartPosition(const ModelEdgeHandle& edge)
		{
			return edge.GetStartModelVertexHandle().GetVertexInfo().GetPositionVector();
		}

		inline Vector&	EndPosition(const ModelEdgeHandle& edge)
		{
			return edge.GetEndModelVertexHandle().GetVertexInfo().GetPositionVector();
		}

		inline PolygonVertexInfo GetVertexInfo(const ModelEdgeHandle& first, bool bStart)
		{
			if(bStart) 
			{
				return first.GetStartVertexInfo(); 
			}
			else 
			{
				return first.GetEndVertexInfo(); 
			}
		}

		inline int CountPolyEdges(const Model& model)
		{
			int res = 0;
			for(auto polyIt = model.GetPolyBeginConstIterator(); polyIt != model.GetPolyEndConstIterator(); ++polyIt)
			{
				res += polyIt->vertices.size();
			}
			return res;
		}		

		inline bool IsLinked(const ModelEdgeConstHandle& edge)
		{
			const Model *pModel = edge.GetModel();
			return pModel->IsLinked(edge);
		}

		inline bool IsSingleLinked(const ModelEdgeConstHandle& edge)
		{
			return NumLinkedEdges(edge) == 1;
		}

		inline bool IsSingleLinkedTo(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& adjEdge)
		{
			const Model *pModel = edge.GetModel();
			const auto &adjEdges = pModel->GetAdjEdgeHashMap(edge);
			if(adjEdges.size() != 1) { return false; }
			return adjEdges.cbegin()->first == adjEdge.hash(); 
		}

		inline int NumLinkedEdges(const ModelEdgeConstHandle& edge)
		{
			const Model *pModel = edge.GetModel();
			auto adjEdges = pModel->GetAdjEdgeHashMap(edge);
			return adjEdges.size();
		}

		inline bool AreLinked(const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& otherEdge)
		{
			const Model *pModel = edge.GetModel();
			const auto &adjEdges = pModel->GetAdjEdgeHashMap(edge);
			return adjEdges.find(otherEdge.hash()) != adjEdges.cend(); 
		}

		inline bool AreSingleLinked (const ModelEdgeConstHandle& edge, const ModelEdgeConstHandle& adjEdge)
		{
			if(!IsSingleLinkedTo(edge,adjEdge)) { return false; }
			return IsSingleLinked(adjEdge);
		}

		inline void LinkCounter(const ModelEdgeHandle& edge, const ModelEdgeHandle& otherEdge)
		{
			Model *pModel = edge.GetModel();
			EdgeAdjacency adjInfo (true);
			pModel->Link(edge, otherEdge, adjInfo);
		}
		
		inline void Link(const ModelEdgeHandle& edge, const ModelEdgeHandle& otherEdge)
		{
			Model *pModel = edge.GetModel();
			EdgeAdjacency adjInfo (false);
			pModel->Link(edge, otherEdge, adjInfo);
		}

		// model adjacency getters operations impl
		inline bool Model::IsLinked(const ModelEdgeConstHandle& edge) const
		{
			//return ContainsKey(_adjMap, edge);
			//return ContainsKey(_adjMapHash, edge.hash());
			return _adjLoadedKeyMap.find(edge.hash())  != _adjLoadedKeyMap.cend();
		}		

		/*
		inline EdgeHashMap Model::GetAdjEdges(const ModelEdgeConstHandle& edge) const
		{
			return _adjMapHash.find(edge.hash())->second;
			//return _adjMap.find(edge)->second;
		
			EdgeMap res;
			const auto &adjEdges = _adjMapHash.find(edge.hash())->second;
			for(auto it = adjEdges.cbegin(); it != adjEdges.cend(); ++it)
			{
				res.insert(std::make_pair(it->second.adjEdge, it->second.adj));
			}
			return res;
		
		}			

		inline EdgeMap Model::GetAdjEdgeMap (const ModelEdgeConstHandle& edge) const
		{
			auto it_adjacentyRow = _adjLoadedKeyMap.find(edge.hash());
			return Util::Container::GetKeyValueToValueMap(_adjLoadedKeyMap, it_adjacentyRow);
		}
		*/		

		inline EdgeHashMap Model::GetAdjEdgeHashMap(const ModelEdgeConstHandle& edge) const
		{
			EdgeHashMap res;
			auto adjEdgesIt = _adjLoadedKeyMap.find(edge.hash());
			if(adjEdgesIt != _adjLoadedKeyMap.cend())
			{
				const auto &adjEdges = adjEdgesIt->second;
				for(auto it = adjEdges.cbegin(); it != adjEdges.cend(); ++it)
				{
					auto key = it->first;
					ModelEdgeHandle keyValue = _adjLoadedKeyMap.get_key_value(key);
					auto value = it->second;
					EdgeToEdgeAdj edgeToEdgeAdj (value, keyValue);
					res.insert(std::make_pair(key, edgeToEdgeAdj));
				}
				return res;
			}			
			else
			{
				return EdgeHashMap();
			}
		}

		inline void	Model::Link(const ModelEdgeHandle& edgeOne, const ModelEdgeHandle& edgeTwo, const EdgeAdjacency& newAdj)
		{
			assert(AreDifferentPolys(edgeOne,edgeTwo));
			_adjMap.set(edgeOne, edgeTwo, newAdj);

			EdgeToEdgeAdj edgeToEdgeAdj (newAdj, edgeTwo);
			_adjMapHash.set(edgeOne.hash(), edgeTwo.hash(), edgeToEdgeAdj);

			_adjLoadedKeyMap.set(edgeOne.hash(), edgeTwo.hash(), newAdj);
			_adjLoadedKeyMap.set_key_value(edgeOne.hash(), edgeOne);
			_adjLoadedKeyMap.set_key_value(edgeTwo.hash(), edgeTwo);
		}		

		inline void	Model::Link(const ModelEdgeConstHandle& edgeOne, const ModelEdgeConstHandle& edgeTwo, const EdgeAdjacency& newAdj)
		{
			assert(AreDifferentPolys(edgeOne,edgeTwo));
			_adjMap.set(edgeOne, edgeTwo, newAdj);
		}		

		inline bool	Model::Unlink(const ModelEdgeConstHandle& edgeOne, const ModelEdgeConstHandle& edgeTwo)
		{
			assert(AreDifferentPolys(edgeOne,edgeTwo));
			_adjMap.erase(edgeOne, edgeTwo) > 0;
			//return _adjMapHash.erase(edgeOne.hash(), edgeTwo.hash()) > 0;			
			return _adjLoadedKeyMap.erase(edgeOne.hash(), edgeTwo.hash()) > 0;
		}

		inline void	Model::UnlinkEdge(const ModelEdgeConstHandle& edge)
		{			
			_adjMap.erase(edge);
			_adjMapHash.erase(edge.hash());
			_adjLoadedKeyMap.erase(edge.hash());
		}
	} // EditableModel
} // Dv

#endif // _EditableModel_h_