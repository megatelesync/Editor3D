#include "EditableModel.h"

namespace Dv
{
	namespace EditableModel
	{
		// Helper function for implementing FlipNormal:
		// Fixes edge and vertex links (including edge link direction flag)
		void FixInversedEdge
		(
			const ModelEdgeHandle& originalEdge, 
			const ModelEdgeHandle& originalPreviousEdge,
			const ModelEdgeHandle& reversedEdge
		);		
		void FlipNormal(const ModelPolygonHandle& poly)
		{
			ModelEdgeHandle originalFirstEdge = poly.GetFirstEdge();
			ModelEdgeHandle originalLastEdge = poly.GetLastEdge();
			ModelEdgeHandle originalPreviousEdge = poly.GetFirstEdge();
			ModelEdgeHandle originalEdge = poly.GetFirstEdge().GetNextEdge();
			ModelEdgeHandle reversedEdge = poly.GetFirstEdge();
			while(true)
			{				
				ModelEdgeHandle nextEdge = originalEdge.GetNextEdge();

				// Move original edge so it stands right before the start edge
				// and name it "reversedEdge"
				reversedEdge = poly.MoveEdgeBefore(originalEdge, reversedEdge);
				
				// Fix all links (vertex and edge) of the originalEdge
				FixInversedEdge(originalEdge, originalPreviousEdge, reversedEdge);

				// Get next edge
				originalPreviousEdge = originalEdge;
				originalEdge = nextEdge;

				if (originalEdge == originalLastEdge)
				{
					FixInversedEdge(originalFirstEdge, originalLastEdge, originalFirstEdge);
					break;
				}
			}
		}

		void FixInversedEdge
		(
			const ModelEdgeHandle& originalEdge, 
			const ModelEdgeHandle& originalPreviousEdge,
			const ModelEdgeHandle& reversedEdge
		)
		{
			if (originalEdge != reversedEdge)
			{
				// Replace the edge in the vertex-edge links
				// WARNING: The iterator in the originalEdge may be already invalidated
				ReplaceEdge_InEdgeToVertexLinks(originalEdge, reversedEdge);
			}
			// Copy the links to polys of the original edge
			ReplaceEdge_InEdgeToEdgeLinks(originalPreviousEdge, reversedEdge);
			// Inverse links to the edges
			InverseEdgeLinks(reversedEdge);
		}

		void FixLinks_OfTwoVertexPoly(const ModelPolygonHandle& poly)
		{
			assert(poly.GetVertexCount() == 2);			
			ModelEdgeHandle edge = poly.GetFirstEdge();
			ModelEdgeHandle otherEdge = edge.GetNextEdge();
			Model *pModel = edge.GetModel();
			auto edgeAdjacent = pModel->GetAdjEdgeHashMap(edge);
			auto otherEdgeAdjacent = pModel->GetAdjEdgeHashMap(otherEdge);
			LinkOverlappedEdgePairs(edgeAdjacent.begin(), edgeAdjacent.end(), otherEdgeAdjacent.begin(), otherEdgeAdjacent.end());
		}		

		void LinkVerticesToPolyEdges (const ModelPolygonHandle& poly)
		{
			ModelEdgeHandle edge = poly.GetFirstEdge();
			while(true)
			{
				LinkEdgeStartToVertex(edge);
				edge = edge.GetNextEdge();				
				if(edge == poly.GetFirstEdge())
				{
					break;
				}
			}
		}

		void UnlinkPolyFromVertices (const ModelEdgeHandle& startEdge, const ModelEdgeHandle& lastEdge)
		{
			ModelEdgeHandle edge = startEdge;
			while(true)
			{
				UnlinkEdgeStartFromVertex(edge);
				edge = edge.GetNextEdge();
				if(edge == lastEdge) { return; }
			}
		}

		void UnlinkPolyFromEdges(const ModelPolygonHandle& poly)
		{
			UnlinkPolyFromEdges(poly.GetFirstEdge(), poly.GetLastEdge());
		}
		
		void UnlinkPolyFromEdges(const ModelEdgeHandle& firstEdge, const ModelEdgeHandle& lastEdge)
		{
			Model *pModel = firstEdge.GetModel();
			ModelEdgeHandle edge = firstEdge;
			while(true)
			{				
				pModel->UnlinkEdge(edge);
				edge = edge.GetNextEdge();
				if(edge == lastEdge)
				{
					break;
				}
			}
		}

		EdgeMapSequence MoveEdgeLinkSetSequence_CorrespondingPolyEdges(const ModelEdgeHandle& startEdge, const ModelEdgeHandle& lastEdge)
		{
			EdgeMapSequence res = CopyEdgeLinkSetSequence_CorrespodingPolyEdges(startEdge, lastEdge);
			UnlinkPolyFromEdges(startEdge, lastEdge);
			return res;
		}

		bool IsLinkedByEdge(const ModelPolygonConstHandle& poly)
		{
			ModelEdgeConstHandle edge = poly.GetFirstEdge();
			while(true)
			{
				if(IsLinked(edge)) { return true; }
				edge = edge.GetNextEdge();
				if(edge == poly.GetFirstEdge())
				{
					return false;
				}
			}
		}

		bool IsVertexLinked(const ModelPolygonConstHandle& poly)
		{
			ModelEdgeConstHandle edge = poly.GetFirstEdge();
			while(true)
			{
				if(NumOutEdges(edge.GetStartModelVertexHandle()) > 1) { return true; }
				edge = edge.GetNextEdge();
				if(edge == poly.GetFirstEdge())
				{
					return false;
				}
			}
		}

		bool CheckPolyVertexLinks(const ModelPolygonConstHandle& poly)
		{
			ModelEdgeConstHandle edge = poly.GetFirstEdge();
			while(true)
			{
				if(!CheckEdgeVertexLink(edge)) { return false; }
				edge = edge.GetNextEdge();
				if(edge == poly.GetFirstEdge())
				{
					return true;
				}
			}
		}

		/*
		void LinkPolySideEdges(const ModelPolygonHandle& poly)
		{
			ModelEdgeHandle baseEdge = poly.GetFirstEdge();			
			while(true)
			{
				ModelEdgeHandle adjBaseEdge = FirstAdjacentEdge(baseEdge).first;
				ModelEdgeHandle nextBaseEdge = baseEdge.GetNextEdge();		
				ModelEdgeHandle nextAdjBaseEdge = FirstAdjacentEdge(nextBaseEdge).first;
				ModelEdgeHandle sideEdge = adjBaseEdge.GetPreviousEdge();
				ModelEdgeHandle nextSideEdge = nextAdjBaseEdge.GetNextEdge();
				LinkCounter(sideEdge, nextSideEdge);
				baseEdge = nextBaseEdge;
				if(baseEdge == poly.GetLastEdge())
				{
					return;
				}
			}			
		}
		*/
	} // EditableModel
} // Dv