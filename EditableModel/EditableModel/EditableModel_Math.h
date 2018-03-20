#ifndef _EditableModel_Math_h_
#define _EditableModel_Math_h_

#include <DirectXMath.h>
#include <utility>
using namespace DirectX; // to make the operators of XMVECTOR work

namespace Dv
{
	namespace EditableModel
	{
		typedef DirectX::XMFLOAT3					Vector3;
		typedef DirectX::XMFLOAT2					Vector2;
		typedef XMVECTOR							 Vector;
		typedef float								 Scalar;
		typedef std::pair<Vector3,Vector3>	    Vector3Pair;
		typedef std::pair<Vector,Vector>	     VectorPair;

		inline Vector						   GetVectorX();
		inline Vector						   GetVectorY();
		inline Vector						   GetVector(float x, float y, float z, float w);
		inline Vector						   GetVector(float x, float y, float z)	{ return GetVector(x, y, z, 1.0F); };
		inline Vector						   GetVector(float x, float y)	{ return GetVector(x, y, 0.0F, 1.0F); };
		inline Vector						   GetVector(const XMFLOAT3& position) { return GetVector(position.x, position.y, position.z); }
		inline Vector						   GetVector(const XMFLOAT2& position) { return GetVector(position.x, position.y); }

		inline Scalar						   Dot3(const XMVECTOR& first, const XMVECTOR& second);

		// Determine order of two vertices in the specified direction
		// Returnes:
		// 0 - unordered (vertices treated as equal)
		// 1 - second vertex goes after the first
		// 2 - first vertex goes after the second
		inline Scalar							GetVertexOrder				(const Vector& direction, const Vector& first, const Vector& second);		

		// Determines whether the line segments [firstStart, firstEnd] and [secondStart, secondEnd] overlap.
		// Precondition:
		//		The vertices (firstStart,firstEnd) and (secondStart,secondEnd) must be codirected
		// Returnes:
		// 0 - overlap;
		// > 0 - no overlap: secondStart (the start of the the second segment) successes the end of the first
		// < 0 - no overlap: firstEnd predecesses the start of the second segment
		inline Scalar AreOverlappedCodirected						(const Vector& direction, 
																	 const Vector& firstStart, const Vector& firstEnd, 
																	 const Vector& secondStart, const Vector& secondEnd);		


		// Impl
		inline Scalar GetVertexOrder(const Vector& direction, const Vector& first, const Vector& second)
		{
			Vector orderDir = second - first;
			Scalar dotRes = Dot3(orderDir,orderDir);
			return dotRes;
		}

		inline Scalar AreOverlappedCodirected	(const Vector& direction, 
										 const Vector& firstStart, const Vector& firstEnd, 
										 const Vector& secondStart, const Vector& secondEnd)
		{
			Scalar order_firstEnd_secondStart = GetVertexOrder(direction, firstEnd, secondStart);
			Scalar order_secondEnd_firstStart = GetVertexOrder(direction, secondEnd, firstStart);
			if (order_firstEnd_secondStart >= 0) { return 1; }
			if (order_secondEnd_firstStart >= 0) { return 1; }
			return 0;
		}

		// Math operations impl
		inline Scalar Dot3(const XMVECTOR& first, const XMVECTOR& second)
		{
			return DirectX::XMVector3Dot(first, second).m128_f32[0];
		}

		inline Vector						   GetVectorX()			
		{
			return GetVector(1.0F, 0.0F, 0.0F); 
		}
		inline Vector						   GetVectorY()			
		{ 
			return GetVector(0.0F, 1.0F, 0.0F); 
		}
		inline Vector GetVector(float x, float y, float z, float w)			
		{ 
			XMFLOAT4 src (x, y, z, w);
			Vector res = XMLoadFloat4(&src);			
			return res;
		}
	} // EditableModel
} // Dv

#endif // _EditableModel_Math_h_
