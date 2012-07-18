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
	*	@param NumVerts number of vertices
	*	@param IndexSize size in bytes of one index: 1, 2, 4 supported
	*	@param pIndices [in] index buffer
	*/
	GPUCacheOptimizer(unsigned int NumTris, unsigned int NumVerts, unsigned int IndexSize, const void* pIndices);
	virtual ~GPUCacheOptimizer(void);

	
	/** \brief Retrieves the map from dst triangle to src triangle.
	* how to remap:
	*  for each triangle t in DstTriBuffer
	*    DstTriBuffer[t] = SrcTriBuffer[TriangleMap[t]]
	* 
	* you can also use WriteIndexBuffer() to get the result
	*/
	const unsigned int* GetTriangleMap() const {return m_pTriMap;}

	/** \brief Applies the remapping on the initial pIndices constructor's param
	* and stores the result in the given destination buffer.
	*
	* @param DstIndexSize size in bytes of one index in the dst buffer
	*	@param pDst [out] index buffer to store the result may also be the same memory as pIndices of constructor's call
	* NOTE:
	* make sure pIndices is not modified/deleted between constructor's and this call
	*/
	void WriteIndexBuffer(unsigned int DstIndexSize, void* pDst);


	/** \brief Reorders vertex buffer to minimize memory address jumps.
	 *
	 * for best performace, use AFTER triangle remapping
	 * see description on RemapVertices() on how to apply this map
	 *
	 *
	 * @param pIndices [in] index buffer
	 * @param pVertMap [out] vertex remap, allocate NumVerts unsigned ints before calling
	 *	      dst vertex index = pVertMap[src vertex index]
	 *				NOTE: if a vertex is not referenced by any triangle, the value 0xFFFFFFFF
	 *				will be stored as the destination index!
	 * @param NumTris   Number of triangles
	 * @param NumVerts  Number of vertices
	 * @param IndexSize Size of the index
	*/
	static void OptimizeVertices(unsigned int NumTris, unsigned int NumVerts, unsigned int IndexSize,
								 const void* pIndices, unsigned int* pVertMap);
	// this function is declared static to be able to operate on the whole model
	// instead of just a subset of it
	// example use:
	// - optimize triangle list per material group
  // - optimize vertex buffer on whole mesh independently of material subsets


	/** \brief Applies the remap table of OptimizeVertices to a vertex and index buffer
	 *
	 * Pseudo code for manual remapping
	 * \code
	 *   for each index i in IndexBuffer:
	 *     IndexBuffer[i] = VertMap[IndexBuffer[i]]
	 *   TmpBuf = VertexBuffer
	 *   for each vertex v in TmpBuf
	 *     if (VertMap[v] != 0xFFFFFFFF)
	 *       VertexBuffer[VertMap[v]] = TmpBuf[v]
	 * \endcode
	 *
   * @param NumVerts  Number of vertices
	 * @param	pVertMap  vertex remap, result from OptimizeVertices() (input)
	 * @param	IndexSize size in bytes of one index: 1, 2, 4 supported
	 * @param pInOutIndices (triangle list) index buffer, remapped after call (input/output)
	 * @param VertexStride  size in bytes of one vertex
	 * @param pInOutVertices vertex buffer, remapped after call (input/output)
	 * @param NumTris   Number of triangles
	 *
	 */
	static void RemapVertices(unsigned int NumTris, unsigned int NumVerts, const unsigned int* pVertMap,
		unsigned int IndexSize, void* pInOutIndices, unsigned int VertexStride, void* pInOutVertices);


	/** \brief 
	* Returns the total number of vertex transforms performed with a certain VertexCache.
	*/
	unsigned int ComputeNumberOfVertexTransformations(unsigned int VertexCacheSize = 16);

	/** \brief Measures the efficiency use of the vertex cache.
	*  ACMR: Average Cache Miss Ratio
	* @return ratio: # vertex transformations / # tris
	*/
	float ComputeACMR(unsigned int VertexCacheSize = 16);

	/** \brief Measures the efficiency use of the vertex cache.
	* ATVR: Average Transform to Vertex Ratio
	* similar to ACMR, but easier to interpret
	* the optimal value is 1.0 given a mesh w/o duplicate vertices
	* @return ratio: # vertex transformations / # verts
	*/
	float ComputeATVR(unsigned int VertexCacheSize = 16);

protected:
	// look up  m_pIndices w.r.t. index size at location 'i'
	unsigned int GetIndex(unsigned int i) const;

	static unsigned int GetIndex(unsigned int i, unsigned int IndexSize, const void* pIB);
	static void SetIndex(unsigned int i, unsigned int val, unsigned int IndexSize, void* pIB);

	virtual void MakeAbstract() = 0;

protected:


	unsigned int m_NumVerts;
	unsigned int m_NumTris;
	
	/**
	TriMap[new tri index] = old tri index
	allocated in base class, computed in child class
	*/
	unsigned int* m_pTriMap;

private:
	unsigned int m_IndexSize;
	const void* m_pIndices;


	unsigned int m_NumTransformations;


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
		unsigned int* pTris;

		/// forsyth's score function
		void FindScore(unsigned int MaxSizeVertexCache);

		void RemoveTriFromList(unsigned int tri);
	};

	struct Opt_Tris
	{
		Opt_Tris() : bAdded(0), fScore(0.0f) 
		{ v[0] = v[1] = v[2] = 0xDEADBEEF;}

		int bAdded;
		/// sum of scores of vertices
		float fScore;
		/// vertices of this triangle
		unsigned int v[3];

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

	/** \brief The actual computation happens here in this constructor
	 *
   * @param NumVerts  Number of vertices
   * @param IndexSize size in bytes of one index: 1, 2, 4 supported
   * @param pIndices  index buffer
   * @param CacheSize number of entries in the vertex cache
   * @param NumTris   Number of triangles
	*/
	GPUCacheOptimizerTipsify(unsigned int CacheSize,
	                         unsigned int NumTris,
	                         unsigned int NumVerts,
	                         unsigned int IndexSize,
	                         const void* pIndices);

private:

	void MakeAbstract(){}

	// simple and fast fixed size stack used in tipsify implementation
	struct RingStack
	{
	private:
		unsigned int* pStack;
		unsigned int uiStart, uiLen;
		unsigned int uiSize;

		inline unsigned int pos(unsigned int i) const
		{
			unsigned int t = uiStart + i;
			if (t >= uiLen) t -= uiLen;
			return t;
		}

	public:
		RingStack(unsigned int _uiSize) : uiStart(0), uiLen(0), uiSize(_uiSize)
		{ pStack = new unsigned int[uiSize];}
		~RingStack() {delete [] pStack;}

		unsigned int length() const {return uiLen;} // current stack length
		unsigned int size() const {return uiSize;} // reserved stack size i.e. maximum length

		inline void push(unsigned int v)
		{
			if (uiLen == uiSize)
			{
				pStack[uiStart++] = v;
				if (uiStart == uiSize) uiStart = 0;
			}
			else
				pStack[pos(uiLen++)] = v; // pos(uiLen) gives the index of the last element + 1
		}

		inline unsigned int pop()
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
	GPUCacheEfficiencyTester(unsigned int NumTris, unsigned int NumVerts, unsigned int IndexSize, const void* pIndices);

private:
	void MakeAbstract(){}
};



//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_GPU_CACHE_OPT_HH defined
//=============================================================================

