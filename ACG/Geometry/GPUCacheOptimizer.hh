/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$															 *
 *   $Author$														 *
 *   $Date$													 *
 *                                                                           *
\*===========================================================================*/

#ifndef ACG_GPU_CACHE_OPT_HH
#define ACG_GPU_CACHE_OPT_HH


//== INCLUDES =================================================================

#include <ACG/Config/ACGDefines.hh>

//== FORWARDDECLARATIONS ======================================================

typedef unsigned int UINT;

//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================


/** \class GPUCacheOptimizer GPUCacheOptimizer.hh <ACG/Scenegraph/MeshNodeT.hh>

    This class optimizes a triangle list for efficient vertex cache usage.
*/

class ACGDLLEXPORT GPUCacheOptimizer
{
public:

	/** \brief constructor
	*     
	* The constructor needs a mesh on which this node will work.
	*
	* @param NumTris number of triangles
	*		 NumVerts number of vertices
	*		 IndexSize size in bytes of one index: 1, 2, 4 supported
	*		 pIndices [in] index buffer
	*/
	GPUCacheOptimizer(UINT NumTris, UINT NumVerts, UINT IndexSize, const void* pIndices);
	virtual ~GPUCacheOptimizer(void);

	
	/** \brief Retrieves the map from dst triangle to src triangle.
	* how to remap:
	*  for each triangle t in DstTriBuffer
	*    DstTriBuffer[t] = SrcTriBuffer[TriangleMap[t]]
	* 
	* you can also use WriteIndexBuffer() to get the result
	*/
	const UINT* GetTriangleMap() const {return m_pTriMap;}

	/** \brief Applies the remapping on the initial pIndices constructor's param
	* and stores the result in the given destination buffer.
	*
	* @param DstIndexSize size in bytes of one index in the dst buffer
	*		 pDst [out] index buffer to store the result
	*			         may also be the same memory as pIndices of constructor's call
	* NOTE:
	* make sure pIndices is not modified/deleted between constructor's and this call
	*/
	void WriteIndexBuffer(UINT DstIndexSize, void* pDst);


	/** \brief Reorders vertex buffer to minimize memory address jumps.
	* for best performace, use AFTER triangle remapping
	*   
	* see description on RemapVertices() on how to apply this map
	*
	*
	* @param
	*	pIndices [in] index buffer
	*	pVertMap [out] vertex remap, allocate NumVerts uints before calling
	*			   dst vertex index = pVertMap[src vertex index]
	*				NOTE: if a vertex is not referenced by any triangle, the value 0xFFFFFFFF
	*				will be stored as the destination index!
	*
	*/
	static void OptimizeVertices(UINT NumTris, UINT NumVerts, UINT IndexSize,
								 const void* pIndices, UINT* pVertMap);
	/* this function is declared static to be able to operate on the whole model
	instead of just a subset of it
	example use : 1. optimize triangle list per material group
	              2. optimize vertex buffer on whole mesh
	     				independently of material subsets
	*/


	/** \brief
	* applies the remap table of OptimizeVertices to a vertex and index buffer
	*
	* pseudo code (manual remapping):
	*
	* for each index i in IndexBuffer:
	*   IndexBuffer[i] = VertMap[IndexBuffer[i]]
	* TmpBuf = VertexBuffer
	* for each vertex v in TmpBuf
	*   if (VertMap[v] != 0xFFFFFFFF)
	*     VertexBuffer[VertMap[v]] = TmpBuf[v]
	*
	* @param
	*	pVertMap [in] vertex remap, result from OptimizeVertices()
	*	IndexSize size in bytes of one index: 1, 2, 4 supported
	*	pInOutIndices [in/out] (triangle list) index buffer, remapped after call
	*	VertexStride size in bytes of one vertex
	*	pInOutVertices [in/out] vertex buffer, remapped after call
	*
	*/
	static void RemapVertices(UINT NumTris, UINT NumVerts, const UINT* pVertMap,
		UINT IndexSize, void* pInOutIndices, UINT VertexStride, void* pInOutVertices);


	/** \brief 
	* Returns the total number of vertex transforms performed with a certain VertexCache.
	*/
	UINT ComputeNumberOfVertexTransformations(UINT VertexCacheSize = 16);

	/** \brief Measures the efficiency use of the vertex cache.
	*  ACMR: Average Cache Miss Ratio
	* @return ratio: # vertex transformations / # tris
	*/
	float ComputeACMR(UINT VertexCacheSize = 16);

	/** \brief Measures the efficiency use of the vertex cache.
	* ATVR: Average Transform to Vertex Ratio
	* similar to ACMR, but easier to interpret
	* the optimal value is 1.0 given a mesh w/o duplicate vertices
	* @return ratio: # vertex transformations / # verts
	*/
	float ComputeATVR(UINT VertexCacheSize = 16);

protected:
	// look up  m_pIndices w.r.t. index size at location 'i'
	UINT GetIndex(UINT i) const;

	static UINT GetIndex(UINT i, UINT IndexSize, const void* pIB);
	static void SetIndex(UINT i, UINT val, UINT IndexSize, void* pIB);

	virtual void MakeAbstract() = 0;

protected:


	UINT m_NumVerts;
	UINT m_NumTris;
	
	/**
	TriMap[new tri index] = old tri index
	allocated in base class, computed in child class
	*/
	UINT* m_pTriMap;

private:
	UINT m_IndexSize;
	const void* m_pIndices;


	UINT m_NumTransformations;


protected:
	/** internal data structures used
	 forsyth and tipsify implementation
	*/

	struct Opt_Vertex
	{
		Opt_Vertex(): iCachePos(-1), fScore(0.0f), iNumTrisTotal(0), iNumTrisLeft(0), pTris(0) {}
		~Opt_Vertex() {delete [] pTris;}

		int iCachePos;
		float fScore;
		/// # tris using this vertex
		int iNumTrisTotal;
		/// # tris left to add to final result
		int iNumTrisLeft;
		UINT* pTris;

		/// forsyth's score function
		void FindScore(UINT MaxSizeVertexCache);

		void RemoveTriFromList(UINT tri);
	};

	struct Opt_Tris
	{
		Opt_Tris() : bAdded(0), fScore(0.0f) 
		{ v[0] = v[1] = v[2] = 0xDEADBEEF;}

		int bAdded;
		/// sum of scores of vertices
		float fScore;
		/// vertices of this triangle
		UINT v[3];

		inline void FindScore(const Opt_Vertex* pVertices)
		{
			fScore = 0.0f;
			for (int i = 0; i < 3; ++i)
				fScore += pVertices[v[i]].fScore;
		}
	};
};

//////////////////////////////////////////////////////////////////////////

/** \class GPUCacheOptimizerTipsify GPUCacheOptimizer.hh

    Implementation of "Fast Triangle Reordering for Vertex Locality and Reduced Overdraw" by Sander et. al.
	http://www.cs.princeton.edu/gfx/pubs/Sander_2007_%3ETR/index.php
*/

class ACGDLLEXPORT GPUCacheOptimizerTipsify : public GPUCacheOptimizer
{
public:
	/** \brief 
		The actual computation happens here in this constructor.

	 @param CacheSize number of entries in the vertex cache
	*/
	GPUCacheOptimizerTipsify(UINT CacheSize, UINT NumTris, UINT NumVerts,
		UINT IndexSize, const void* pIndices);

private:

	void MakeAbstract(){}

	// simple and fast fixed size stack used in tipsify implementation
	struct RingStack
	{
	private:
		UINT* pStack;
		UINT uiStart, uiLen;
		UINT uiSize;

		inline UINT pos(UINT i) const
		{
			UINT t = uiStart + i;
			if (t >= uiLen) t -= uiLen;
			return t;
		}

	public:
		RingStack(UINT _uiSize) : uiStart(0), uiLen(0), uiSize(_uiSize)
		{ pStack = new UINT[uiSize];}
		~RingStack() {delete [] pStack;}

		UINT length() const {return uiLen;} // current stack length
		UINT size() const {return uiSize;} // reserved stack size i.e. maximum length

		inline void push(UINT v)
		{
			if (uiLen == uiSize)
			{
				pStack[uiStart++] = v;
				if (uiStart == uiSize) uiStart = 0;
			}
			else
				pStack[pos(uiLen++)] = v; // pos(uiLen) gives the index of the last element + 1
		}

		inline UINT pop()
		{
			if (uiSize && uiLen) return pStack[pos(--uiLen)];
			return 0xFFFFFFFF;
		}
	};
};

/** \class GPUCacheEfficiencyTester GPUCacheOptimizer.hh

    simple class providing ATVR and ACMR computations w/o any optimizing
*/
class ACGDLLEXPORT GPUCacheEfficiencyTester : public GPUCacheOptimizer
{
public:
	GPUCacheEfficiencyTester(UINT NumTris, UINT NumVerts, UINT IndexSize, const void* pIndices);

private:
	void MakeAbstract(){}
};



//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_GPU_CACHE_OPT_HH defined
//=============================================================================

