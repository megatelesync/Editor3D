#pragma once

namespace Dv
{
	namespace Util
	{
		/********************************************************************************************
		* Hash constants: WARNING!!! Do NOT Touch!!!
		********************************************************************************************/
		const size_t FNV_HASH_BASE         = 2166136261;
		const size_t FNV_HASH_COEFF        = 16777619;

		// One-at-a-time hash: Bob jenkin's lookup hash: quickly reaches avalanche, performs very well
		size_t Calc_OneAtATime_LookupHash(const void *pBytes, size_t InNumBytes);

		// Calculate FNV Hash
		size_t Calc_FNVHash(const void *pBytes, size_t InNumBytes);
		size_t Calc_BernsteinHash(const void *pBytes, size_t InNumBytes);
		size_t Calc_ModifiedBernstein(const void *pBytes, size_t InNumBytes);

		// Better algorithms: 
		// One of the better algorithms, however it does not perform sufficiently better 
		// to justify slightly more complication implementation
		size_t Calc_ElfHash(const void *pBytes, size_t InNumBytes);

		// Jenkins hash: passed all kinds of hash for avalanche and permutations
		size_t Calc_Jenkins(const void *pBytes, size_t InNumBytes, size_t InInitValue);

		// Shift-Add-Xor-Hash: fails test for avalanche, but it does not seem to affect its performance in practise;
		// Was designed for strings, however works with similar efficiency for other types of data.
		size_t Calc_ShiftAddXor(const void *pBytes, size_t InNumBytes);

		// Rotating hash: sufficient, can be considered as a minimal acceptable algorithm
		size_t Calc_RotatingHash(const void *pBytes, size_t InNumBytes);

		// Bad algorithm: xor hash
		size_t Calc_XorHash(const void *pBytes, size_t InNumBytes);

		// Very bad algorithm: additive: hash based on commutative operation is bad
		size_t Calc_AddHash(const void *pBytes, size_t InNumBytes);
	}
}

namespace std
{
	/**************************************************************************************************************
	* Fowler/Noll/Vo hash
	**************************************************************************************************************/
	template<class T>
	class FNVHash
	{
	public:
		size_t operator() (const T& InValue);
		size_t operator() (const T *pInValue);

	private:
	};

	template<class T>
	class FNVHashBytes
	{
	public:
		FNVHashBytes                                            (size_t InNumBytes);

		size_t                           operator               () (const T *pInValue);
		size_t                           GetNumBytes            () const           { return _numBytes; }

	private:
		size_t                            _numBytes;
	};

	template<class T>
	class LookupHash_OneAtATime
	{
	public:
		size_t operator() (const T& InValue);
		size_t operator() (const T *pInValue);
	};

	template<class T>
	class RotatingHash
	{
	public:
		size_t operator() (const T& InValue);
		size_t operator() (const T *pInValue);
	};

	template<class T>
	class BernsteinHash
	{
	public:
		size_t operator() (const T& InValue);
		size_t operator() (const T *pInValue);
	};

	/**************************************************************************************************************
	* One-at-a-time hash implementation
	**************************************************************************************************************/
	template<class T>
	size_t LookupHash_OneAtATime<T>::operator() (const T& InValue)
	{
		return Calc_OneAtATime_LookupHash(&InValue, sizeof(T));
	}

	template<class T>
	size_t LookupHash_OneAtATime<T>::operator() (const T *pInValue)
	{
		return Calc_OneAtATime_LookupHash(pInValue, sizeof(T));
	}

	/**************************************************************************************************************
	* Bernstein hash implementation
	**************************************************************************************************************/
	template<class T>
	size_t BernsteinHash<T>::operator() (const T& InValue)
	{
		return Calc_BernsteinHash(&InValue, sizeof(T));
	}

	template<class T>
	size_t BernsteinHash<T>::operator() (const T *pInValue)
	{
		return Calc_BernsteinHash(pInValue, sizeof(T));
	}

	/**************************************************************************************************************
	* RotatingHash implementation
	**************************************************************************************************************/
	template<class T>
	size_t RotatingHash<T>::operator() (const T& InValue)
	{
		return Calc_RotatingHash(&InValue, sizeof(T));
	}

	template<class T>
	size_t RotatingHash<T>::operator() (const T *pInValue)
	{
		return Calc_RotatingHash(pInValue, sizeof(T));
	}

	/**************************************************************************************************************
	* Fowler/Noll/Vo hash bytes implementation
	**************************************************************************************************************/
	template<class T>
	FNVHashBytes<T>::FNVHashBytes(size_t InNumBytes) :
		_numBytes(InNumBytes)
	{
	}

	template<class T>
	size_t FNVHashBytes<T>::operator()(const T *pInValue)
	{
		return Calc_FNVHash(pInValue, _numBytes);
	}

	/**************************************************************************************************************
	* Fowler/Noll/Vo hash implementation
	**************************************************************************************************************/
	template<class T>
	size_t FNVHash<T>::operator() (const T& InValue)
	{
		return Calc_FNVHash(&InValue, sizeof(T));
	}

	template<class T>
	size_t FNVHash<T>::operator() (const T *pInValue)
	{
		return Calc_FNVHash(pInValue, sizeof(T));
	}
}

namespace Dv
{
	namespace Util
	{
	}
}