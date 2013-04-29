/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision: 13867 $                                                       *
*   $LastChangedBy: kremer $                                                 *
*   $Date: 2012-02-23 09:15:26 +0100 (Thu, 23 Feb 2012) $                    *
*                                                                            *
\*===========================================================================*/

#define VOLUMEMESHBUFFERMANAGERT_CC

#include "VolumeMeshBufferManager.hh"

template <class VolumeMesh>
VolumeMeshBufferManager<VolumeMesh>::VolumeMeshBufferManager(const VolumeMesh &mesh_, OpenVolumeMesh::StatusAttrib& statusAttrib_,
                                                                                OpenVolumeMesh::ColorAttrib<ACG::Vec4f>& colorAttrib_,
                                                                                OpenVolumeMesh::NormalAttrib<VolumeMesh>& normalAttrib_)
    :
      mDefaultColor(ACG::Vec4f(0.0,0.0,0.0,1.0)),
      mMesh(mesh_),
      mStatusAttrib(statusAttrib_),
      mColorAttrib(colorAttrib_),
      mNormalAttrib(normalAttrib_),
      mNumOfVertices(-1),
      mCurrentNumOfVertices(-1),
      mScale(0.8),
      mBuffer(0),
      mCurrentPickOffset(-1),
      mInvalidated(true),
      mGeometryChanged(true),
      mNormalsChanged(true),
      mColorsChanged(true),
      mDrawModes(),
      mPrimitiveMode(PM_NONE),
      mNormalMode(NM_NONE),
      //mColorMode(CM_NONE),
      mSkipUnselected(false),
      mShowIrregularInnerEdges(false),
      mShowIrregularOuterValence2Edges(false),
      mSkipRegularEdges(false),
      mBoundaryOnly(false),
      mCurrentPrimitiveMode(PM_NONE),
      mCurrentNormalMode(NM_NONE),
      //mCurrentColorMode(CM_NONE),
      mCurrentSkipUnselected(false),
      mCurrentShowIrregularInnerEdges(false),
      mCurrentShowIrregularOuterValence2Edges(false),
      mCurrentSkipRegularEdges(false),
      mCurrentBoundaryOnly(false),
      mCurrentNormalOffset(0),
      mCurrentColorOffset(0),
      mCogsValid(false),
      mCellInsidenessValid(),
      mMultiTexturing(false),
      mTexCoordMode(TCM_NONE),
      mCurrentTexCoordMode(TCM_NONE),
      mTexCoordOffset(0),
      mCurrentTexCoordOffset(0)
{

}

/**
 * @brief Adds a float to the buffer
 *
 * The given value will be inserted at the location the pointer points to.
 * After execution the pointer will point to the element after the inserted one.
 *
 * @param _value The value that should be inserted
 * @param _buffer A pointer to the place the value should be inserted to
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::addFloatToBuffer( float _value, unsigned char *&_buffer )
{
    // get pointer
    unsigned char *v = (unsigned char *) &_value;

    // copy over 4 bytes
    *_buffer++ = *v++;
    *_buffer++ = *v++;
    *_buffer++ = *v++;
    *_buffer++ = *v;
}

/**
 * @brief Adds an unsigned char to the buffer
 *
 * The given value will be inserted at the location the pointer points to.
 * After execution the pointer will point to the element after the inserted one.
 *
 * @param _value The value that should be inserted
 * @param _buffer A pointer to the place the value should be inserted to
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::addUCharToBuffer( unsigned char _value, unsigned char *&_buffer )
{
    *_buffer = _value;
    ++_buffer;
}

/**
 * @brief Adds a position to the buffer
 *
 * This method should be called after the size of a vertex was calculated using
 * calculateVertexDeclaration()
 *
 * @param _position The position that should be inserted
 * @param _buffer A pointer to the start of the buffer
 * @param _offset The offset (in number of vertices) to the place the position should be inserted
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::addPositionToBuffer(ACG::Vec3d _position, unsigned char* _buffer , unsigned int _offset)
{
    unsigned char* buffer = _buffer + _offset*mVertexSize;
    addFloatToBuffer(_position[0], buffer);
    addFloatToBuffer(_position[1], buffer);
    addFloatToBuffer(_position[2], buffer);
}

/**
 * @brief Adds a color to the buffer
 *
 * This method should be called after the size of a vertex and the color offset within a vertex
 * was calculated using calculateVertexDeclaration()
 *
 * @param _color The color that should be inserted
 * @param _buffer A pointer to the start of the buffer
 * @param _offset The offset (in number of vertices) to the place the color should be inserted
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::addColorToBuffer(ACG::Vec4uc _color, unsigned char* _buffer, unsigned int _offset)
{
    unsigned char* buffer = _buffer + _offset*mVertexSize + mColorOffset;
    addUCharToBuffer(_color[0], buffer);
    addUCharToBuffer(_color[1], buffer);
    addUCharToBuffer(_color[2], buffer);
    addUCharToBuffer(_color[3], buffer);
}

/**
 * @brief Adds a color to the buffer
 *
 * The color is added as unsigned character between 0 and 255 for _color between 0.0f and 1.0f
 *
 * This method should be called after the size of a vertex and the color offset within a vertex
 * was calculated using calculateVertexDeclaration()
 *
 * @param _color The color that should be inserted
 * @param _buffer A pointer to the start of the buffer
 * @param _offset The offset (in number of vertices) to the place the color should inserted
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::addColorToBuffer(ACG::Vec4f _color, unsigned char* _buffer, unsigned int _offset)
{
    unsigned char* buffer = _buffer + _offset*mVertexSize + mColorOffset;
    addUCharToBuffer(_color[0]*255, buffer);
    addUCharToBuffer(_color[1]*255, buffer);
    addUCharToBuffer(_color[2]*255, buffer);
    addUCharToBuffer(_color[3]*255, buffer);
}

/**
 * @brief Adds a normal to the buffer
 *
 * This method should be called after the size of a vertex and the normal offset within a vertex
 * was calculated using calculateVertexDeclaration()
 *
 * @param _normal The normal that should be inserted
 * @param _buffer A pointer to the start of the buffer
 * @param _offset The offset (in number of vertices) to the place the normal should inserted
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::addNormalToBuffer(ACG::Vec3d _normal, unsigned char* _buffer, unsigned int _offset)
{
    unsigned char* buffer = _buffer + _offset*mVertexSize + mNormalOffset;
    addFloatToBuffer(_normal[0], buffer);
    addFloatToBuffer(_normal[1], buffer);
    addFloatToBuffer(_normal[2], buffer);
}


/**
 * @brief Constructs a VertexDeclaration, the size and the offsets for the vertices stored in the buffer
 *
 * Depending on the current options this method calculates an appropriate VertexDeclaration which can be used for
 * rendering. Also it calculates the vertex stride and the offsets for colors and normals.
 *
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::calculateVertexDeclaration()
{
    unsigned int currentOffset = 0;
    mVertexDeclaration.clear();

    mNormalOffset = -1;
    mColorOffset  = -1;

    if (mPrimitiveMode != PM_NONE) //should always be the case
    {
        mVertexDeclaration.addElement(GL_FLOAT, 3, ACG::VERTEX_USAGE_POSITION, reinterpret_cast<GLuint*>(currentOffset));
        currentOffset += 3*sizeof(float);
    }

    if (mNormalMode != NM_NONE)
    {
        mNormalOffset = currentOffset;
        mVertexDeclaration.addElement(GL_FLOAT, 3, ACG::VERTEX_USAGE_NORMAL, reinterpret_cast<GLuint*>(currentOffset));
        currentOffset += 3*sizeof(float);
    }

    /*
    if ((mColorMode != CM_NONE) || mShowIrregularInnerEdges || mShowIrregularOuterValence2Edges)
    {
        mColorOffset = currentOffset;
        mVertexDeclaration.addElement(GL_UNSIGNED_BYTE, 4, ACG::VERTEX_USAGE_COLOR, reinterpret_cast<GLuint*>(currentOffset));
        currentOffset += 4*sizeof(char);
    }
    */

    if ((mTexCoordMode != TCM_NONE))
    {
        mTexCoordOffset = currentOffset;
        unsigned char numOfCoords = 0;
        if (mTexCoordMode == TCM_VERTEX_1D)
            numOfCoords = 1;
        else if (mTexCoordMode == TCM_VERTEX_2D)
            numOfCoords = 2;
       // mVertexDeclaration.addElement(GL_FLOAT, numOfCoords, VERTEX_USAGE_TEXCOORD, reinterpret_cast<GLuint*>(currentOffset));
        currentOffset += numOfCoords * sizeof(float);
    }

    mVertexSize = currentOffset;
}


/**
* @brief Returns the number of vertices stored in the buffer
*
*/
template <class VolumeMesh>
unsigned int VolumeMeshBufferManager<VolumeMesh>::getNumOfVertices()
{
    if (mNumOfVertices == -1)
        countNumOfVertices();
    return mNumOfVertices;
}

/**
 * @brief Sets the default color
 *
 * The default color is only used for regular edges when drawn together with irregular edges.
 *
 * @param _defaultColor The default color
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::setDefaultColor(ACG::Vec4f _defaultColor)
{
    if (mDefaultColor != _defaultColor)
    {
       invalidateColors();
       mDefaultColor = _defaultColor;
    }
}

/**
 * @brief Configures the buffer manager's options from a DrawMode
 *
 * This method extracts from the given draw mode which primitve mode, color mode and normal mode should be used
 * and which irregular edges should be shown.
 *
 * @param _drawMode The DrawMode for which the options should be set
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::setOptionsFromDrawMode(ACG::SceneGraph::DrawModes::DrawMode _drawMode)
{
    // colors
    if (_drawMode & (mDrawModes.cellsColoredPerCell))
        enablePerCellColors();
    else if (_drawMode & (mDrawModes.cellsColoredPerFace | mDrawModes.facesColoredPerFace))
        enablePerFaceColors();
    else if (_drawMode & (mDrawModes.cellsColoredPerHalfface | mDrawModes.halffacesColoredPerHalfface))
        enablePerHalffaceColors();
    else if (_drawMode & ( mDrawModes.edgesColoredPerEdge ))
        enablePerEdgeColors();
    else if (_drawMode & (mDrawModes.halfedgesColoredPerHalfedge))
        enablePerHalfedgeColors();
    else if (_drawMode & (mDrawModes.cellsColoredPerVertex | mDrawModes.facesColoredPerVertex |
                          mDrawModes.halffacesColoredPerVertex | mDrawModes.edgesColoredPerEdge |
                          mDrawModes.verticesColored))
        enablePerVertexColors();
    /*else
        disableColors();*/

    //normals
    if (_drawMode & (mDrawModes.cellsFlatShaded | mDrawModes.halffacesFlatShaded))
        enablePerHalffaceNormals();
    else if (_drawMode & (mDrawModes.facesFlatShaded))
        enablePerFaceNormals();
    else if (_drawMode & (mDrawModes.cellsSmoothShaded | mDrawModes.facesSmoothShaded | mDrawModes.halffacesSmoothShaded))
        enablePerVertexNormals();
    else
        disableNormals();

    if (_drawMode & (mDrawModes.irregularInnerEdges))
        mShowIrregularInnerEdges = true;
    else
        mShowIrregularInnerEdges = false;

    if (_drawMode & (mDrawModes.irregularOuterEdges))
        mShowIrregularOuterValence2Edges = true;
    else
        mShowIrregularOuterValence2Edges = false;

    if (!mShowIrregularInnerEdges && !mShowIrregularOuterValence2Edges)
        mSkipRegularEdges = false;
    else
        mSkipRegularEdges = true;

    if (_drawMode & (mDrawModes.cellBasedDrawModes))
        mPrimitiveMode = PM_CELLS;
    else if (_drawMode & (mDrawModes.facesOnCells))
        mPrimitiveMode = PM_FACES_ON_CELLS;
    else if (_drawMode & (mDrawModes.faceBasedDrawModes | mDrawModes.hiddenLineBackgroundFaces))
        mPrimitiveMode = PM_FACES;
    else if (_drawMode & (mDrawModes.halffaceBasedDrawModes))
        mPrimitiveMode = PM_HALFFACES;
    else if (_drawMode & (mDrawModes.edgesOnCells))
        mPrimitiveMode = PM_EDGES_ON_CELLS;
    else if (_drawMode & ((mDrawModes.edgeBasedDrawModes) & ~(mDrawModes.irregularInnerEdges | mDrawModes.irregularOuterEdges)))
        mPrimitiveMode = PM_EDGES;
    else if (_drawMode & (mDrawModes.irregularInnerEdges | mDrawModes.irregularOuterEdges)) // note: this has to be checked after _drawMode & (mDrawModes.edgeBasedDrawModes)
        mPrimitiveMode = PM_IRREGULAR_EDGES;                                                // if this is true, irregular edges are already included
    else if (_drawMode & (mDrawModes.halfedgeBasedDrawModes))
        mPrimitiveMode = PM_HALFEDGES;
    else if (_drawMode & (mDrawModes.verticesOnCells))
        mPrimitiveMode = PM_VERTICES_ON_CELLS;
    else if (_drawMode & (mDrawModes.vertexBasedDrawModes))
        mPrimitiveMode = PM_VERTICES;
    else
        mPrimitiveMode = PM_NONE;
}


/**
* @brief Removes all cut planes
*
* After removing the cut planes the buffer is invalidated so it is rebuild the next time someone wants to use it.
*
*/
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::clearCutPlanes()
{
    cut_planes_.clear();
    invalidateGeometry();
    mCellInsidenessValid = false;
}

/**
* @brief Adds a cut plane
*
* After adding the cut plane the buffer is invalidated so it is rebuild the next time someone wants to use it.
*
* @param _p The plane that is added.
*/
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::addCutPlane(const VolumeMeshBufferManager<VolumeMesh>::Plane &_p)
{
    cut_planes_.push_back(_p);
    invalidateGeometry();
    mCellInsidenessValid = false;
}

/**
* @brief Adds a cut plane
*
* After adding the cut plane the buffer is invalidated so it is rebuild the next time someone wants to use it.
*
* @param _p The position of the plane.
* @param _n The normal of the plane.
* @param _xsize The x-direction of the plane scaled by 1.0/width
* @param _ysize The y-direction of the plane scaled by 1.0/height
*/
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::addCutPlane(const ACG::Vec3d &_p, const ACG::Vec3d &_n, const ACG::Vec3d &_xsize, const ACG::Vec3d &_ysize)
{
    addCutPlane(Plane(_p, _n, _xsize, _ysize));
}

/**
* @brief Tests whether the given point is inside w.r.t. all cut planes
*
* @param _p The position to be tested.
*
* @return True iff the point is inside.
*/
template <class VolumeMesh>
bool VolumeMeshBufferManager<VolumeMesh>::is_inside(const ACG::Vec3d& _p)
{
    for(typename std::vector<Plane>::iterator it = cut_planes_.begin(); it != cut_planes_.end(); ++it) {
            // get local position
            ACG::Vec3d pl = _p - it->position;
            // evaluate dot products
            double pn = (pl | it->normal);
            double px = (pl | it->xdirection);
            double py = (pl | it->ydirection);

            if (pn < 0.0 && px > -0.5 && px < 0.5 && py > -0.5 && py < 0.5)
                return false;
        }

        return true;
}

/**
* @brief Tests whether the given vertex is inside w.r.t. all cut planes
*
* A vertex is considered inside iff its positions is inside.
*
* @param _vh A Handle to the vertex that should be tested.
*
* @return True iff the vertex is inside.
*/
template <class VolumeMesh>
bool VolumeMeshBufferManager<VolumeMesh>::is_inside(const VertexHandle& _vh)
{
    if (cut_planes_.size() == 0) return true;
    return is_inside(mMesh.vertex(_vh));
}

/**
* @brief Tests whether the given halfedge is inside w.r.t. all cut planes
*
* A halfedge is considered inside iff both from- and to-vertex are inside.
*
* @param _heh A Handle to the halfedge that should be tested.
*
* @return True iff the halfadge is inside.
*/
template <class VolumeMesh>
bool VolumeMeshBufferManager<VolumeMesh>::is_inside(const HalfEdgeHandle& _heh)
{
    if (cut_planes_.size() == 0) return true;
    Edge e(mMesh.halfedge(_heh));
    return is_inside(mMesh.vertex(e.from_vertex())) && is_inside(mMesh.vertex(e.to_vertex()));
}

/**
* @brief Tests whether the given edge is inside w.r.t. all cut planes
*
* An edge is considered inside iff both from- and to-vertex are inside.
*
* @param _eh A Handle to the edge that should be tested.
*
* @return True iff the edge is inside.
*/
template <class VolumeMesh>
bool VolumeMeshBufferManager<VolumeMesh>::is_inside(const EdgeHandle& _eh)
{
    if (cut_planes_.size() == 0) return true;
    Edge e(mMesh.edge(_eh));
    return is_inside(mMesh.vertex(e.from_vertex())) && is_inside(mMesh.vertex(e.to_vertex()));
}

/**
* @brief Tests whether the given halfface is inside w.r.t. all cut planes
*
* A halfface is considered inside iff all its incident vertices are inside.
*
* @param _hfh A Handle to the halfface that should be tested.
*
* @return True iff the halfface is inside.
*/
template <class VolumeMesh>
bool VolumeMeshBufferManager<VolumeMesh>::is_inside(const HalfFaceHandle& _hfh)
{
    if (cut_planes_.size() == 0) return true;
    for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(_hfh); hfv_it; ++hfv_it)
        if (!is_inside(*hfv_it)) return false;

    return true;
}

/**
* @brief Tests whether the given face is inside w.r.t. all cut planes
*
* A face is considered inside iff all its incident vertices are inside.
*
* @param _fh A Handle to the face that should be tested.
*
* @return True iff the face is inside.
*/
template <class VolumeMesh>
bool VolumeMeshBufferManager<VolumeMesh>::is_inside(const FaceHandle& _fh)
{
    if (cut_planes_.size() == 0) return true;
    return is_inside(mMesh.halfface_handle(_fh,0));
}

/**
* @brief Tests whether the given cell is inside w.r.t. all cut planes
*
* A cell is considered inside iff the positions of all its incident vertices moved
* towards the cell's center of gravity depending on the current scaling parameter
* are inside.
*
* @param _ch A Handle to the cell that should be tested.
*
* @return True iff the cell is inside.
*/
template <class VolumeMesh>
bool VolumeMeshBufferManager<VolumeMesh>::is_inside(const CellHandle& _ch)
{
    if (!mCellInsidenessValid)
        calculateCellInsideness();

    return mCellInsideness[_ch.idx()];
}


/**
 * @brief Calculates for all cells whether they are inside w.r.t. all cut planes
 *
 * The insideness is stored so it can be reused until the geometry or cut planes change to
 * improve performance.
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::calculateCellInsideness()
{
    if (mCellInsideness.size() != mMesh.n_cells())
        mCellInsideness.resize(mMesh.n_cells());

    for (unsigned int i = 0; i < mMesh.n_cells(); i++)
    {
        CellHandle ch = CellHandle(i);
        bool inside;
        ACG::Vec3d cog = getCOG(ch);
        if (cut_planes_.size() == 0)
            inside = true;
        else
        {
            inside = true;
            for (OpenVolumeMesh::CellVertexIter cv_it = mMesh.cv_iter(ch); cv_it; ++cv_it)
            {
                ACG::Vec3d vertexPos = mScale * mMesh.vertex(*cv_it) + (1-mScale) * cog;
                if (!is_inside(vertexPos)) inside = false;
            }
        }
        mCellInsideness[i] = inside;
    }

    mCellInsidenessValid = true;
}


/**
* @brief Tests whether the options were changed since the last time building the buffer
*
* For this method to work saveOptions() should be called immidiately after building the buffer
*
* @return True if options have changed, False if not
*/
template <class VolumeMesh>
bool VolumeMeshBufferManager<VolumeMesh>::optionsChanged()
{
    return mCurrentPrimitiveMode                   != mPrimitiveMode                   ||
           mCurrentNormalMode                      != mNormalMode                      ||
           mCurrentColorMode                       != mColorMode                       ||
           mCurrentSkipUnselected                  != mSkipUnselected                  ||
           mCurrentShowIrregularInnerEdges         != mShowIrregularInnerEdges         ||
           mCurrentShowIrregularOuterValence2Edges != mShowIrregularOuterValence2Edges ||
           mCurrentSkipRegularEdges                != mSkipRegularEdges                ||
           mCurrentBoundaryOnly                    != mBoundaryOnly;
}

/**
* @brief State that the current buffer was built with the current options
*
* This method should be called immidiately after building the buffer
*/
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::saveOptions()
{
    mCurrentPrimitiveMode                   = mPrimitiveMode;
    mCurrentNormalMode                      = mNormalMode;
    mCurrentColorMode                       = mColorMode;
    mCurrentSkipUnselected                  = mSkipUnselected;
    mCurrentShowIrregularInnerEdges         = mShowIrregularInnerEdges;
    mCurrentShowIrregularOuterValence2Edges = mShowIrregularOuterValence2Edges;
    mCurrentSkipRegularEdges                = mSkipRegularEdges;
    mCurrentBoundaryOnly                    = mBoundaryOnly;
    mCurrentVertexSize                      = mVertexSize;
    mCurrentNormalOffset                    = mNormalOffset;
    mCurrentColorOffset                     = mColorOffset;
    mCurrentNumOfVertices                   = mNumOfVertices;
}


/**
* @brief Returns a color code for irregular edges
*
* @param _valence Number of incident cells
* @param _inner False if the edge is a boundary edge, True if not
*
* @return The appropraite color
*/
template <class VolumeMesh>
ACG::Vec4f VolumeMeshBufferManager<VolumeMesh>::getValenceColorCode(unsigned int _valence, bool _inner) const {

    if(_inner && _valence == 3) {
        return ACG::Vec4f(0.0f, 1.0f, 1.0f, 1.0f);
    } else if(_inner && _valence == 5) {
        return ACG::Vec4f(1.0f, 0.0f, 1.0f, 1.0f);
    } else if(!_inner && _valence > 3) {
        return ACG::Vec4f(0.0f, 1.0f, 0.0f, 1.0f);
    } else if(!_inner && _valence == 2) {
        return ACG::Vec4f(1.0f, 1.0f, 0.0f, 1.0f);
    } else if(_inner && _valence > 5) {
        return ACG::Vec4f(0.5f, 1.0f, 0.5f, 1.0f);
    }
    return ACG::Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
}

/**
 * @brief Counts the number of vertices that need to be stored in the buffer
 *
 * The number of vertices is stored in mNumOfVertices
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::countNumOfVertices()
{
    unsigned int numOfVertices = 0;

    if (mSkipUnselected) //only count selected
    {
        if (mPrimitiveMode == PM_CELLS)
        {
            OpenVolumeMesh::CellIter c_begin(mMesh.cells_begin()), c_end(mMesh.cells_end());
            for (OpenVolumeMesh::CellIter c_it = c_begin; c_it != c_end; ++c_it)
                if (mStatusAttrib[*c_it].selected() && is_inside(*c_it))
                {
                    std::vector<HalfFaceHandle> hfs = mMesh.cell(*c_it).halffaces();
                    for (unsigned int i = 0; i < hfs.size(); ++i)
                        numOfVertices += ((mMesh.valence(mMesh.face_handle(hfs[i])))-2)*3;
                }
        }
        else if (mPrimitiveMode == PM_FACES)
        {
            OpenVolumeMesh::FaceIter f_begin(mMesh.faces_begin()), f_end(mMesh.faces_end());
            for (OpenVolumeMesh::FaceIter f_it = f_begin; f_it != f_end; ++f_it)
                if (mStatusAttrib[*f_it].selected() && is_inside(*f_it) && (!mBoundaryOnly || mMesh.is_boundary(*f_it)))
                    numOfVertices += ((mMesh.valence(*f_it))-2)*3;  //additional vertices are added for faces with more than 3 adjacent vertices
        }
        else if (mPrimitiveMode == PM_FACES_ON_CELLS)
        {
            OpenVolumeMesh::FaceIter f_begin(mMesh.faces_begin()), f_end(mMesh.faces_end());
            for (OpenVolumeMesh::FaceIter f_it = f_begin; f_it != f_end; ++f_it)
                if (mStatusAttrib[*f_it].selected())
                    numOfVertices += ((mMesh.valence(*f_it))-2)*3*getNumOfIncidentCells(*f_it);  //additional vertices are added for faces with more than 3 adjacent vertices
        }
        else if (mPrimitiveMode == PM_HALFFACES)
        {
            OpenVolumeMesh::HalfFaceIter hf_begin(mMesh.halffaces_begin()), hf_end(mMesh.halffaces_end());
            for (OpenVolumeMesh::HalfFaceIter hf_it = hf_begin; hf_it != hf_end; ++hf_it)
                if (mStatusAttrib[*hf_it].selected() && is_inside(*hf_it) && (!mBoundaryOnly || mMesh.is_boundary(*hf_it)))
                    numOfVertices += ((mMesh.valence(mMesh.face_handle(*hf_it)))-2)*3;  //additional vertices are added for faces with more than 3 adjacent vertices
        }
        else if (mPrimitiveMode == PM_EDGES)
        {
            OpenVolumeMesh::EdgeIter e_begin(mMesh.edges_begin()), e_end(mMesh.edges_end());
            for (OpenVolumeMesh::EdgeIter e_it = e_begin; e_it != e_end; ++e_it)
                if (mStatusAttrib[*e_it].selected() && is_inside(*e_it) && (!mBoundaryOnly || mMesh.is_boundary(*e_it)))
                    numOfVertices += 2;
        }
        else if (mPrimitiveMode == PM_EDGES_ON_CELLS)
        {
            OpenVolumeMesh::EdgeIter e_begin(mMesh.edges_begin()), e_end(mMesh.edges_end());
            for (OpenVolumeMesh::EdgeIter e_it = e_begin; e_it != e_end; ++e_it)
                if (mStatusAttrib[*e_it].selected())
                    numOfVertices += 2*getNumOfIncidentCells(*e_it);
        }
        else if (mPrimitiveMode == PM_HALFEDGES)
        {
            OpenVolumeMesh::HalfEdgeIter he_begin(mMesh.halfedges_begin()), he_end(mMesh.halfedges_end());
            for (OpenVolumeMesh::HalfEdgeIter he_it = he_begin; he_it != he_end; ++he_it)
                if (mStatusAttrib[*he_it].selected() && is_inside(*he_it) && (!mBoundaryOnly || mMesh.is_boundary(*he_it)))
                    numOfVertices += 2;
        }
        else if (mPrimitiveMode == PM_VERTICES)
        {
            OpenVolumeMesh::VertexIter v_begin(mMesh.vertices_begin()), v_end(mMesh.vertices_end());
            for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it != v_end; ++v_it)
                if (mStatusAttrib[*v_it].selected() && is_inside(*v_it) && (!mBoundaryOnly || mMesh.is_boundary(*v_it)))
                        numOfVertices += 1;
        }
        else if (mPrimitiveMode == PM_VERTICES_ON_CELLS)
        {
            OpenVolumeMesh::VertexIter v_begin(mMesh.vertices_begin()), v_end(mMesh.vertices_end());
            for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it != v_end; ++v_it)
                if (mStatusAttrib[*v_it].selected())
                        numOfVertices += getNumOfIncidentCells(*v_it);
        }
    }
    else
    {
        if (mPrimitiveMode == PM_VERTICES_ON_CELLS)
        {
            for (unsigned int i = 0; i < mMesh.n_vertices(); i++)
                numOfVertices += getNumOfIncidentCells(VertexHandle(i));
        }
        else if (mPrimitiveMode == PM_VERTICES)
        {
            for (unsigned int i = 0; i < mMesh.n_vertices(); i++)
                if (is_inside(VertexHandle(i)) && (!mBoundaryOnly || mMesh.is_boundary(VertexHandle(i))))
                    numOfVertices += 1;
        }
        else if (mPrimitiveMode == PM_FACES)
        {
            OpenVolumeMesh::FaceIter f_begin(mMesh.faces_begin()), f_end(mMesh.faces_end());
            for (OpenVolumeMesh::FaceIter f_it = f_begin; f_it != f_end; ++f_it)
                if (is_inside(*f_it) && (!mBoundaryOnly || mMesh.is_boundary(*f_it)))
                    numOfVertices += ((mMesh.valence(*f_it))-2)*3;  //additional vertices are added for faces with more than 3 adjacent vertices
        }
        else if (mPrimitiveMode == PM_FACES_ON_CELLS)
        {
            OpenVolumeMesh::FaceIter f_begin(mMesh.faces_begin()), f_end(mMesh.faces_end());
            for (OpenVolumeMesh::FaceIter f_it = f_begin; f_it != f_end; ++f_it)
                numOfVertices += ((mMesh.valence(*f_it))-2)*3*getNumOfIncidentCells(*f_it);  //additional vertices are added for faces with more than 3 adjacent vertices
        }
        else if (mPrimitiveMode == PM_HALFFACES)
        {
            OpenVolumeMesh::HalfFaceIter hf_begin(mMesh.halffaces_begin()), hf_end(mMesh.halffaces_end());
            for (OpenVolumeMesh::HalfFaceIter hf_it = hf_begin; hf_it != hf_end; ++hf_it)
                if (is_inside(*hf_it) && (!mBoundaryOnly || mMesh.is_boundary(*hf_it)))
                    numOfVertices += ((mMesh.valence(mMesh.face_handle(*hf_it)))-2)*3;  //additional vertices are added for faces with more than 3 adjacent vertices
        }
        else if (mPrimitiveMode == PM_CELLS)
        {
            OpenVolumeMesh::CellIter c_begin(mMesh.cells_begin()), c_end(mMesh.cells_end());
            for (OpenVolumeMesh::CellIter c_it = c_begin; c_it != c_end; ++c_it)
            {
                if (is_inside(*c_it))
                {
                    std::vector<HalfFaceHandle> hfs = mMesh.cell(*c_it).halffaces();
                    for (unsigned int i = 0; i < hfs.size(); ++i)
                        numOfVertices += ((mMesh.valence(mMesh.face_handle(hfs[i])))-2)*3;
                }
            }
        }
        else if (mPrimitiveMode == PM_EDGES_ON_CELLS)
        {
            for (OpenVolumeMesh::EdgeIter e_it = mMesh.edges_begin(); e_it != mMesh.edges_end(); ++e_it)
                numOfVertices += 2*getNumOfIncidentCells(*e_it);
        }
        else if ( mPrimitiveMode == PM_EDGES ) //all edges are drawn, so irregular ones are already included
        {
            for (OpenVolumeMesh::EdgeIter e_it = mMesh.edges_begin(); e_it != mMesh.edges_end(); ++e_it)
                if (is_inside(*e_it) && (!mBoundaryOnly || mMesh.is_boundary(*e_it)))
                    numOfVertices += 2;
        }
        else if ( mPrimitiveMode == PM_IRREGULAR_EDGES )
        {
            for (OpenVolumeMesh::EdgeIter e_it = mMesh.edges_begin(); e_it != mMesh.edges_end(); ++e_it)
                if (is_inside(*e_it) && (!mBoundaryOnly || mMesh.is_boundary(*e_it)))
                {
                    bool boundary = mMesh.is_boundary(*e_it);
                    unsigned int valence = mMesh.valence(*e_it);

                    // Skip regular edges
                    if(( boundary && valence == 3) || (!boundary && valence == 4)) continue;
                    //skip boundary valence 2 edges if not drawn
                    if ((!(mShowIrregularOuterValence2Edges)) && ( boundary && valence == 2)) continue;
                    //skip irregular inner edges if not drawn
                    if ((!(mShowIrregularInnerEdges)) && (( !boundary && valence != 4) ||
                                                          ( boundary && valence < 2) ||
                                                          ( boundary && valence > 3))) continue;

                    numOfVertices += 2;
                }
        }
        else if ( mPrimitiveMode == PM_HALFEDGES )
        {
            for (OpenVolumeMesh::HalfEdgeIter he_it = mMesh.halfedges_begin(); he_it != mMesh.halfedges_end(); ++he_it)
                if (is_inside(*he_it) && (!mBoundaryOnly || mMesh.is_boundary(*he_it)))
                    numOfVertices += 2;
        }
        else /*if ( mPrimitiveMode == PM_NONE)*/
        {
            numOfVertices = 0;
        }
    }

    mNumOfVertices = numOfVertices;
}

/**
 * @brief Returns the number of cells that are incident to the given face and also inside w.r.t. all cut planes
 *
 * @param _fh A Handle of the face of interest
 *
 * @return Number of incident cells that are inside.
 */
template <class VolumeMesh>
int VolumeMeshBufferManager<VolumeMesh>::getNumOfIncidentCells(OpenVolumeMesh::FaceHandle _fh)
{
    int incidentCells = 0;
    OpenVolumeMesh::HalfFaceHandle hf0 = mMesh.halfface_handle(_fh, 0);
    if (mMesh.incident_cell(hf0) != CellHandle(-1))
        if (is_inside(mMesh.incident_cell(hf0)))
            incidentCells += 1;
    OpenVolumeMesh::HalfFaceHandle hf1 = mMesh.halfface_handle(_fh, 1);
    if (mMesh.incident_cell(hf1) != CellHandle(-1))
        if (is_inside(mMesh.incident_cell(hf1)))
            incidentCells += 1;
    return incidentCells;
}

/**
 * @brief Returns the number of cells that are incident to the given edge and also inside w.r.t. all cut planes
 *
 * @param _eh A handle of the edge of interest
 *
 * @return Number of incident cells that are inside.
 */
template <class VolumeMesh>
int VolumeMeshBufferManager<VolumeMesh>::getNumOfIncidentCells(OpenVolumeMesh::EdgeHandle _eh)
{
    int incidentCells = 0;
    OpenVolumeMesh::HalfEdgeHandle heh = mMesh.halfedge_handle(_eh, 0);
    for (OpenVolumeMesh::HalfEdgeCellIter hec_it = OpenVolumeMesh::HalfEdgeCellIter(heh,&mMesh); hec_it.valid(); ++hec_it)
        if (hec_it->idx() != -1)
            if (is_inside(*hec_it))
                incidentCells++;
    return incidentCells;
}

/**
 * @brief Returns the number of cells that are incident to the given vertex and also inside w.r.t. all cut planes
 *
 * @param _vh A handle of the vertex of interest
 *
 * @return Number of incident cells that are inside.
 */
template <class VolumeMesh>
int VolumeMeshBufferManager<VolumeMesh>::getNumOfIncidentCells(OpenVolumeMesh::VertexHandle _vh)
{
    int incidentCells = 0;
    for (OpenVolumeMesh::VertexCellIter vc_it = OpenVolumeMesh::VertexCellIter(_vh,&mMesh); vc_it; ++vc_it)
        if (vc_it->idx() != -1)
            if (is_inside(*vc_it))
                incidentCells++;
    return incidentCells;
}

/**
 * @brief Returns the center of gravity of the given cell
 *
 * @param _ch A handle of the cell of interest
 *
 * @return The cell's center of gravity
 */
template <class VolumeMesh>
ACG::Vec3d VolumeMeshBufferManager<VolumeMesh>::getCOG(OpenVolumeMesh::CellHandle _ch)
{
    if (!mCogsValid)
        calculateCOGs();

    return mCogs[_ch.idx()];
}

/**
 * @brief Calculates the center of gravity for all cells
 *
 * The centers of gravity are stored in the property mCogProperty points to.
 * Make sure this points to a correct property before calling this method.
 *
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::calculateCOGs()
{
    if (mCogs.size() != mMesh.n_cells())
        mCogs.resize(mMesh.n_cells());

    OpenVolumeMesh::CellIter c_begin(mMesh.cells_begin()), c_end(mMesh.cells_end());
    for (OpenVolumeMesh::CellIter c_it = c_begin; c_it != c_end; ++c_it)
    {

        std::vector<HalfFaceHandle> hfs = mMesh.cell(*c_it).halffaces();
        ACG::Vec3d cog = ACG::Vec3d(0.0f);
        int count = 0;
        for (unsigned int i = 0; i < hfs.size(); ++i)
            for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(hfs[i]); hfv_it; ++hfv_it)
            {
                cog += mMesh.vertex(*hfv_it);
                count += 1;
            }
        cog = 1.0/count * cog;

        mCogs[c_it->idx()] = cog;
    }

    mCogsValid = true;
}

/**
 * @brief Checks whether positions need to be rebuild
 *
 * @return True iff positions need to be rebuild
 */
template <class VolumeMesh>
bool VolumeMeshBufferManager<VolumeMesh>::positionsNeedRebuild()
{
    return  (mGeometryChanged)                        ||
            (mVertexSize != mCurrentVertexSize)       ||
            (mPrimitiveMode != mCurrentPrimitiveMode) ||
            (mNumOfVertices != mCurrentNumOfVertices);
}

/**
 * @brief Checks whether colors need to be rebuild
 *
 * @return True iff colors need to be rebuild
 */
template <class VolumeMesh>
bool VolumeMeshBufferManager<VolumeMesh>::colorsNeedRebuild()
{
    return  (mColorsChanged)                          ||
            (mVertexSize    != mCurrentVertexSize)    ||
            (mNumOfVertices != mCurrentNumOfVertices) ||
            (mPrimitiveMode != mCurrentPrimitiveMode) ||
            (mColorOffset   != mCurrentColorOffset)   ||
            (mColorMode     != mCurrentColorMode)     ||
            (mShowIrregularInnerEdges != mCurrentShowIrregularInnerEdges) ||
            (mShowIrregularOuterValence2Edges != mCurrentShowIrregularOuterValence2Edges);
}

/**
 * @brief Checks whether normals need to be rebuild
 *
 * @return True iff normals need to be rebuild
 */
template <class VolumeMesh>
bool VolumeMeshBufferManager<VolumeMesh>::normalsNeedRebuild()
{
    return (mNormalsChanged)                         ||
           (mVertexSize != mCurrentVertexSize)       ||
           (mPrimitiveMode != mCurrentPrimitiveMode) ||
           (mNormalOffset != mCurrentNormalOffset)   ||
           (mNormalMode != mCurrentNormalMode)       ||
           (mNumOfVertices != mCurrentNumOfVertices);
}

/**
 * @brief Adds all vertices to the buffer
 *
 * @param _buffer Pointer to the start of the buffer
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::buildVertexBuffer(unsigned char* _buffer)
{
    unsigned int pos = 0;

    if (mPrimitiveMode == PM_VERTICES)
    {
        for (unsigned int i = 0; i < mMesh.n_vertices(); ++i) {
            if (mSkipUnselected && !mStatusAttrib[VertexHandle(i)].selected()) continue;
            if (!is_inside(VertexHandle(i))) continue;
            if (mBoundaryOnly && !mMesh.is_boundary(VertexHandle(i))) continue;
            ACG::Vec3d p = mMesh.vertex(VertexHandle(i));
            addPositionToBuffer(p, _buffer, pos++);
        }
    }
    else if (mPrimitiveMode == PM_VERTICES_ON_CELLS)
    {
        for (unsigned int i = 0; i < mMesh.n_vertices(); ++i) {
            if (mSkipUnselected && !mStatusAttrib[VertexHandle(i)].selected()) continue;
            ACG::Vec3d p = mMesh.vertex(VertexHandle(i));
            for (OpenVolumeMesh::VertexCellIter vc_it = OpenVolumeMesh::VertexCellIter(VertexHandle(i),&mMesh); vc_it; ++vc_it)
            {
                ACG::Vec3d cog = getCOG(*vc_it);
                ACG::Vec3d newPos = p*mScale + cog*(1-mScale);
                if (is_inside(*vc_it))
                    addPositionToBuffer(p*mScale + cog*(1-mScale), _buffer, pos++);
            }
        }
    }
    else if (mPrimitiveMode == PM_FACES)
    {
        std::vector<ACG::Vec3d> vertices;
        OpenVolumeMesh::FaceIter f_begin(mMesh.faces_begin()), f_end(mMesh.faces_end());
        for (OpenVolumeMesh::FaceIter f_it = f_begin; f_it != f_end; ++f_it)
        {
            if (mSkipUnselected && !mStatusAttrib[*f_it].selected()) continue;
            if (mBoundaryOnly && !mMesh.is_boundary(*f_it)) continue;
            if (!is_inside(*f_it)) continue;
            vertices.clear();
            for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(mMesh.halfface_handle(*f_it,0)); hfv_it; ++hfv_it)
                vertices.push_back(mMesh.vertex(*hfv_it));

            for (unsigned int i = 0; i < vertices.size()-2; i++)
            {
                addPositionToBuffer(vertices[0],   _buffer, pos++);
                addPositionToBuffer(vertices[i+1], _buffer, pos++);
                addPositionToBuffer(vertices[i+2], _buffer, pos++);
            }
        }
    }
    else if (mPrimitiveMode == PM_FACES_ON_CELLS)
    {
        std::vector<ACG::Vec3d> vertices;
        OpenVolumeMesh::FaceIter f_begin(mMesh.faces_begin()), f_end(mMesh.faces_end());
        for (OpenVolumeMesh::FaceIter f_it = f_begin; f_it != f_end; ++f_it)
        {
            if (mSkipUnselected && !mStatusAttrib[*f_it].selected()) continue;
            vertices.clear();
            for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(mMesh.halfface_handle(*f_it,0)); hfv_it; ++hfv_it)
                vertices.push_back(mMesh.vertex(*hfv_it));

            for (int i = 0; i < 2; i++)
            {
                OpenVolumeMesh::CellHandle ch = mMesh.incident_cell(mMesh.halfface_handle(*f_it,i));
                if (ch != CellHandle(-1))
                {
                    if (!is_inside(ch)) continue;
                    ACG::Vec3d cog = getCOG(ch);
                    for (unsigned int i = 0; i < vertices.size()-2; i++)
                    {
                        //we dont care about the correct facing because we dont cull the back side of faces
                        addPositionToBuffer(vertices[0]*mScale + cog*(1-mScale),   _buffer, pos++);
                        addPositionToBuffer(vertices[i+1]*mScale + cog*(1-mScale), _buffer, pos++);
                        addPositionToBuffer(vertices[i+2]*mScale + cog*(1-mScale), _buffer, pos++);
                    }
                }
            }

        }
    }
    else if (mPrimitiveMode == PM_HALFFACES)
    {
        std::vector<ACG::Vec3d> vertices;
        OpenVolumeMesh::HalfFaceIter hf_begin(mMesh.halffaces_begin()), hf_end(mMesh.halffaces_end());
        for (OpenVolumeMesh::HalfFaceIter hf_it = hf_begin; hf_it != hf_end; ++hf_it)
        {
            if (mSkipUnselected && !mStatusAttrib[*hf_it].selected()) continue;
            if (mBoundaryOnly && !mMesh.is_boundary(*hf_it)) continue;
            if (!is_inside(*hf_it)) continue;
            vertices.clear();
            for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(*hf_it); hfv_it; ++hfv_it)
                vertices.push_back(mMesh.vertex(*hfv_it));

            for (unsigned int i = 0; i < vertices.size()-2; i++)
            {
                addPositionToBuffer(vertices[0],   _buffer, pos++);
                addPositionToBuffer(vertices[i+1], _buffer, pos++);
                addPositionToBuffer(vertices[i+2], _buffer, pos++);
            }
        }
    }
    else if (mPrimitiveMode == PM_CELLS)
    {
        std::vector<ACG::Vec3d> vertices;
        OpenVolumeMesh::CellIter c_begin(mMesh.cells_begin()), c_end(mMesh.cells_end());
        for (OpenVolumeMesh::CellIter c_it = c_begin; c_it != c_end; ++c_it)
        {
            if (mSkipUnselected && !mStatusAttrib[*c_it].selected()) continue;
            if (!is_inside(*c_it)) continue;
            std::vector<HalfFaceHandle> hfs = mMesh.cell(*c_it).halffaces();
            ACG::Vec3d cog = getCOG(*c_it);
            for (unsigned int i = 0; i < hfs.size(); ++i)
            {
                vertices.clear();
                for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(hfs[i]); hfv_it; ++hfv_it)
                    vertices.push_back(mScale*mMesh.vertex(*hfv_it)+(1-mScale)*cog);

                for (unsigned int i = 0; i < vertices.size()-2; i++)
                {
                    addPositionToBuffer(vertices[0],   _buffer, pos++);
                    addPositionToBuffer(vertices[i+1], _buffer, pos++);
                    addPositionToBuffer(vertices[i+2], _buffer, pos++);
                }
            }
        }

    }
    else if (mPrimitiveMode == PM_EDGES)
    {
        OpenVolumeMesh::EdgeIter e_begin(mMesh.edges_begin()), e_end(mMesh.edges_end());
        for (OpenVolumeMesh::EdgeIter e_it = e_begin; e_it != e_end; ++e_it)
        {
            if (mSkipUnselected && !mStatusAttrib[*e_it].selected()) continue;
            if (mBoundaryOnly && !mMesh.is_boundary(*e_it)) continue;
            if (!is_inside(*e_it)) continue;

            Edge e(mMesh.edge(*e_it));
            addPositionToBuffer(mMesh.vertex(e.from_vertex()), _buffer, pos++);
            addPositionToBuffer(mMesh.vertex(e.to_vertex()),   _buffer, pos++);
        }
    }
    else if (mPrimitiveMode == PM_EDGES_ON_CELLS)
    {
        OpenVolumeMesh::EdgeIter e_begin(mMesh.edges_begin()), e_end(mMesh.edges_end());
        for (OpenVolumeMesh::EdgeIter e_it = e_begin; e_it != e_end; ++e_it)
        {
            if (mSkipUnselected && !mStatusAttrib[*e_it].selected()) continue;

            Edge e(mMesh.edge(*e_it));
            OpenVolumeMesh::HalfEdgeHandle heh = mMesh.halfedge_handle(*e_it, 0);
            for (OpenVolumeMesh::HalfEdgeCellIter hec_it = OpenVolumeMesh::HalfEdgeCellIter(heh,&mMesh); hec_it.valid(); ++hec_it)
            {
                if (hec_it->idx() != -1)
                {
                    if (!is_inside(*hec_it)) continue;
                    ACG::Vec3d cog = getCOG(*hec_it);
                    addPositionToBuffer(mMesh.vertex(e.from_vertex())*mScale + cog*(1-mScale), _buffer, pos++);
                    addPositionToBuffer(mMesh.vertex(e.to_vertex())  *mScale + cog*(1-mScale), _buffer, pos++);
                }
            }
        }
    }
    else if (mPrimitiveMode == PM_IRREGULAR_EDGES)
    {
        OpenVolumeMesh::EdgeIter e_begin(mMesh.edges_begin()), e_end(mMesh.edges_end());
        for (OpenVolumeMesh::EdgeIter e_it = e_begin; e_it != e_end; ++e_it)
        {
            if (mSkipUnselected && !mStatusAttrib[*e_it].selected()) continue;
            if (mBoundaryOnly && !mMesh.is_boundary(*e_it)) continue;
            if (!is_inside(*e_it)) continue;

            bool boundary = mMesh.is_boundary(*e_it);
            unsigned int valence = mMesh.valence(*e_it);
            // Skip regular
            if(( boundary && valence == 3) ||
             (!boundary && valence == 4)) continue;
            //skip boundary valence 2 edges if not drawn
            if ((!mShowIrregularOuterValence2Edges) && ( boundary && valence == 2)) continue;
            //skip irregular inner edges if not drawn
            if ((!mShowIrregularInnerEdges) && (( !boundary && valence != 4) ||
                                                (  boundary && valence < 2)  ||
                                                (  boundary && valence > 3))) continue;

            Edge e(mMesh.edge(*e_it));
            addPositionToBuffer(mMesh.vertex(e.from_vertex()), _buffer, pos++);
            addPositionToBuffer(mMesh.vertex(e.to_vertex()),   _buffer, pos++);
        }
    }
    else if (mPrimitiveMode == PM_HALFEDGES)
    {
        OpenVolumeMesh::HalfEdgeIter he_begin(mMesh.halfedges_begin()), he_end(mMesh.halfedges_end());
        for (OpenVolumeMesh::HalfEdgeIter he_it = he_begin; he_it != he_end; ++he_it)
        {
            if (mSkipUnselected && !mStatusAttrib[*he_it].selected()) continue;
            if (mBoundaryOnly && !mMesh.is_boundary(*he_it)) continue;
            if (!is_inside(*he_it)) continue;

            double lambda = 0.4;
            Edge e(mMesh.halfedge(*he_it));
            addPositionToBuffer(mMesh.vertex(e.from_vertex()), _buffer, pos++);
            addPositionToBuffer((1-lambda)*mMesh.vertex(e.from_vertex()) + lambda*mMesh.vertex(e.to_vertex()),   _buffer, pos++);
        }
    }
    else /*if (mPrimitiveMode == PM_NONE)*/
    {
        //This case should not happen. Do nothing.
    }
}

/**
 * @brief Adds all normals to the buffer
 *
 * @param _buffer Pointer to the start of the buffer
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::buildNormalBuffer(unsigned char* _buffer)
{
    if (mNormalMode == NM_NONE) return;

    unsigned int pos = 0;

    if ((mPrimitiveMode == PM_FACES) && (mNormalMode == NM_FACE))
    {
        OpenVolumeMesh::FaceIter f_begin(mMesh.faces_begin()), f_end(mMesh.faces_end());
        for (OpenVolumeMesh::FaceIter f_it = f_begin; f_it != f_end; ++f_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*f_it)) continue;
            if (!is_inside(*f_it)) continue;
            ACG::Vec3d normal = mNormalAttrib[*f_it];
            unsigned int numOfVerticesInFace = mMesh.valence(*f_it);

            for (unsigned int i = 0; i < numOfVerticesInFace - 2; i++)
            {
                addNormalToBuffer(normal, _buffer, pos++);
                addNormalToBuffer(normal, _buffer, pos++);
                addNormalToBuffer(normal, _buffer, pos++);
            }
        }
    }
    else if ((mPrimitiveMode == PM_FACES) && (mNormalMode == NM_VERTEX))
    {
        std::vector<ACG::Vec3d> normals;
        OpenVolumeMesh::FaceIter f_begin(mMesh.faces_begin()), f_end(mMesh.faces_end());
        for (OpenVolumeMesh::FaceIter f_it = f_begin; f_it != f_end; ++f_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*f_it)) continue;
            if (!is_inside(*f_it)) continue;
            normals.clear();
            for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(mMesh.halfface_handle(*f_it,0)); hfv_it; ++hfv_it)
                normals.push_back(mNormalAttrib[*hfv_it]);

            for (unsigned int i = 0; i < normals.size()-2; i++)
            {
                addNormalToBuffer(normals[0],   _buffer, pos++);
                addNormalToBuffer(normals[i+1], _buffer, pos++);
                addNormalToBuffer(normals[i+2], _buffer, pos++);
            }
        }
    }
    else if ((mPrimitiveMode == PM_HALFFACES) && (mNormalMode == NM_HALFFACE))
    {
        OpenVolumeMesh::HalfFaceIter hf_begin(mMesh.halffaces_begin()), hf_end(mMesh.halffaces_end());
        for (OpenVolumeMesh::HalfFaceIter hf_it = hf_begin; hf_it != hf_end; ++hf_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*hf_it)) continue;
            if (!is_inside(*hf_it)) continue;
            ACG::Vec3d normal = mNormalAttrib[*hf_it];
            unsigned int numOfVerticesInCell = 0;
            for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(*hf_it); hfv_it; ++hfv_it)
                ++numOfVerticesInCell;

            for (unsigned int i = 0; i < numOfVerticesInCell- 2; i++)
            {
                addNormalToBuffer(normal, _buffer, pos++);
                addNormalToBuffer(normal, _buffer, pos++);
                addNormalToBuffer(normal, _buffer, pos++);
            }
        }
    }
    else if ((mPrimitiveMode == PM_HALFFACES) && (mNormalMode == NM_VERTEX))
    {
        std::vector<ACG::Vec3d> normals;
        OpenVolumeMesh::HalfFaceIter hf_begin(mMesh.halffaces_begin()), hf_end(mMesh.halffaces_end());
        for (OpenVolumeMesh::HalfFaceIter hf_it = hf_begin; hf_it != hf_end; ++hf_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*hf_it)) continue;
            if (!is_inside(*hf_it)) continue;
            normals.clear();
            for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(*hf_it); hfv_it; ++hfv_it)
                normals.push_back(mNormalAttrib[*hfv_it]);

            for (unsigned int i = 0; i < normals.size()-2; i++)
            {
                addNormalToBuffer(normals[0],   _buffer, pos++);
                addNormalToBuffer(normals[i+1], _buffer, pos++);
                addNormalToBuffer(normals[i+2], _buffer, pos++);
            }
        }
    }
    else if ((mPrimitiveMode == PM_CELLS) && (mNormalMode == NM_HALFFACE))
    {
        OpenVolumeMesh::CellIter c_begin(mMesh.cells_begin()), c_end(mMesh.cells_end());
        for (OpenVolumeMesh::CellIter c_it = c_begin; c_it != c_end; ++c_it)
        {
            if (!is_inside(*c_it)) continue;
            std::vector<HalfFaceHandle> hfs = mMesh.cell(*c_it).halffaces();
            for (unsigned int i = 0; i < hfs.size(); ++i)
            {
                ACG::Vec3d normal = -1.0*mNormalAttrib[hfs[i]];
                unsigned int numOfVerticesInFace = mMesh.valence(mMesh.face_handle(hfs[i]));

                for (unsigned int i = 0; i < numOfVerticesInFace-2; i++)
                {
                    addNormalToBuffer(normal, _buffer, pos++);
                    addNormalToBuffer(normal, _buffer, pos++);
                    addNormalToBuffer(normal, _buffer, pos++);
                }
            }
        }
    }
    else if ((mPrimitiveMode == PM_CELLS) && (mNormalMode == NM_VERTEX))
    {
        std::vector<ACG::Vec3d> normals;
        OpenVolumeMesh::CellIter c_begin(mMesh.cells_begin()), c_end(mMesh.cells_end());
        for (OpenVolumeMesh::CellIter c_it = c_begin; c_it != c_end; ++c_it)
        {
            if (!is_inside(*c_it)) continue;
            std::vector<HalfFaceHandle> hfs = mMesh.cell(*c_it).halffaces();

            for (unsigned int i = 0; i < hfs.size(); ++i)
            {
                normals.clear();
                for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(hfs[i]); hfv_it; ++hfv_it)
                    normals.push_back(mNormalAttrib[*hfv_it]);

                for (unsigned int i = 0; i < normals.size()-2; i++)
                {
                    addNormalToBuffer(normals[0],   _buffer, pos++);
                    addNormalToBuffer(normals[i+1], _buffer, pos++);
                    addNormalToBuffer(normals[i+2], _buffer, pos++);
                }
            }
        }
    }

}

/**
 * @brief Adds all colors to the buffer
 *
 * @param _buffer Pointer to the start of the buffer
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::buildColorBuffer(unsigned char* _buffer)
{
    unsigned int pos = 0;

    if ((mPrimitiveMode == PM_VERTICES) && (mColorMode == CM_VERTEX))
    {
        for (unsigned int i = 0; i < mMesh.n_vertices(); ++i)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(VertexHandle(i))) continue;
            if (!is_inside(VertexHandle(i))) continue;
            ACG::Vec4f color = mColorAttrib[VertexHandle(i)];
            addColorToBuffer(color, _buffer, pos++);
        }
    }
    else if ((mPrimitiveMode == PM_FACES) && (mColorMode == CM_VERTEX))
    {
        std::vector<ACG::Vec4f> colors;
        OpenVolumeMesh::FaceIter f_begin(mMesh.faces_begin()), f_end(mMesh.faces_end());
        for (OpenVolumeMesh::FaceIter f_it = f_begin; f_it != f_end; ++f_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*f_it)) continue;
            if (!is_inside(*f_it)) continue;
            colors.clear();

            for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(mMesh.halfface_handle(*f_it,0)); hfv_it; ++hfv_it)
                colors.push_back(mColorAttrib[*hfv_it]);

            for (unsigned int i = 0; i < (colors.size()-2); i++)
            {
                addColorToBuffer(colors[0], _buffer, pos++);
                addColorToBuffer(colors[i+1], _buffer, pos++);
                addColorToBuffer(colors[i+2], _buffer, pos++);
            }
        }
    }
    else if ((mPrimitiveMode == PM_FACES) && (mColorMode == CM_FACE))
    {
        OpenVolumeMesh::FaceIter f_begin(mMesh.faces_begin()), f_end(mMesh.faces_end());
        for (OpenVolumeMesh::FaceIter f_it = f_begin; f_it != f_end; ++f_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*f_it)) continue;
            if (!is_inside(*f_it)) continue;
            ACG::Vec4f color = mColorAttrib[*f_it];

            unsigned int numOfVerticesInFace = mMesh.valence(*f_it);
            unsigned int numOfDrawnTriangles = numOfVerticesInFace-2;

            for (unsigned int i = 0; i < numOfDrawnTriangles*3; i++)
                addColorToBuffer(color, _buffer, pos++);
        }
    }
    else if ((mPrimitiveMode == PM_HALFFACES) && (mColorMode == CM_HALFFACE))
    {
        OpenVolumeMesh::HalfFaceIter hf_begin(mMesh.halffaces_begin()), hf_end(mMesh.halffaces_end());
        for (OpenVolumeMesh::HalfFaceIter hf_it = hf_begin; hf_it != hf_end; ++hf_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*hf_it)) continue;
            if (!is_inside(*hf_it)) continue;
            ACG::Vec4f color = mColorAttrib[*hf_it];

            unsigned int numOfVerticesInFace = mMesh.valence(mMesh.face_handle(*hf_it));
            unsigned int numOfDrawnTriangles = numOfVerticesInFace-2;

            for (unsigned int i = 0; i < numOfDrawnTriangles*3; i++)
                addColorToBuffer(color, _buffer, pos++);
        }
    }
    else if ((mPrimitiveMode == PM_HALFFACES) && (mColorMode == CM_VERTEX))
    {
        std::vector<ACG::Vec4f> colors;
        OpenVolumeMesh::HalfFaceIter hf_begin(mMesh.halffaces_begin()), hf_end(mMesh.halffaces_end());
        for (OpenVolumeMesh::HalfFaceIter hf_it = hf_begin; hf_it != hf_end; ++hf_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*hf_it)) continue;
            if (!is_inside(*hf_it)) continue;
            colors.clear();

            for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(*hf_it); hfv_it; ++hfv_it)
                colors.push_back(mColorAttrib[*hfv_it]);

            for (unsigned int i = 0; i < (colors.size()-2); i++)
            {
                addColorToBuffer(colors[0], _buffer, pos++);
                addColorToBuffer(colors[i+1], _buffer, pos++);
                addColorToBuffer(colors[i+2], _buffer, pos++);
            }
        }
    }
    else if ((mPrimitiveMode == PM_CELLS) && (mColorMode == CM_CELL))
    {
        OpenVolumeMesh::CellIter c_begin(mMesh.cells_begin()), c_end(mMesh.cells_end());
        for (OpenVolumeMesh::CellIter c_it = c_begin; c_it != c_end; ++c_it)
        {
            if (!is_inside(*c_it)) continue;
            ACG::Vec4f color = mColorAttrib[*c_it];
            std::vector<HalfFaceHandle> hfs = mMesh.cell(*c_it).halffaces();
            for (unsigned int i = 0; i < hfs.size(); ++i)
            {
                unsigned int numOfVerticesInHalfface = 0;
                for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(hfs[i]); hfv_it; ++hfv_it)
                    ++numOfVerticesInHalfface;

                unsigned int numOfDrawnFaces = numOfVerticesInHalfface-2;
                for (unsigned int i = 0; i < numOfDrawnFaces*3; i++)
                    addColorToBuffer(color, _buffer, pos++);
            }
        }
    }
    else if ((mPrimitiveMode == PM_CELLS) && (mColorMode == CM_HALFFACE))
    {
        OpenVolumeMesh::CellIter c_begin(mMesh.cells_begin()), c_end(mMesh.cells_end());
        for (OpenVolumeMesh::CellIter c_it = c_begin; c_it != c_end; ++c_it)
        {
            if (!is_inside(*c_it)) continue;
            std::vector<HalfFaceHandle> hfs = mMesh.cell(*c_it).halffaces();
            for (unsigned int i = 0; i < hfs.size(); ++i)
            {
                ACG::Vec4f color = mColorAttrib[hfs[i]];
                unsigned int numOfVerticesInHalfface = 0;
                for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(hfs[i]); hfv_it; ++hfv_it)
                    ++numOfVerticesInHalfface;

                unsigned int numOfDrawnFaces = numOfVerticesInHalfface-2;
                for (unsigned int i = 0; i < numOfDrawnFaces*3; i++)
                    addColorToBuffer(color, _buffer, pos++);
            }
        }
    }
    else if ((mPrimitiveMode == PM_CELLS) && (mColorMode == CM_FACE))
    {
        OpenVolumeMesh::CellIter c_begin(mMesh.cells_begin()), c_end(mMesh.cells_end());
        for (OpenVolumeMesh::CellIter c_it = c_begin; c_it != c_end; ++c_it)
        {
            if (!is_inside(*c_it)) continue;
            std::vector<HalfFaceHandle> hfs = mMesh.cell(*c_it).halffaces();
            for (unsigned int i = 0; i < hfs.size(); ++i)
            {
                ACG::Vec4f color = mColorAttrib[mMesh.face_handle(hfs[i])];
                unsigned int numOfVerticesInHalfface = 0;
                for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(hfs[i]); hfv_it; ++hfv_it)
                    ++numOfVerticesInHalfface;

                unsigned int numOfDrawnFaces = numOfVerticesInHalfface-2;
                for (unsigned int i = 0; i < numOfDrawnFaces*3; i++)
                    addColorToBuffer(color, _buffer, pos++);
            }
        }
    }
    else if ((mPrimitiveMode == PM_CELLS) && (mColorMode == CM_VERTEX))
    {
        std::vector<ACG::Vec4f> colors;
        OpenVolumeMesh::CellIter c_begin(mMesh.cells_begin()), c_end(mMesh.cells_end());
        for (OpenVolumeMesh::CellIter c_it = c_begin; c_it != c_end; ++c_it)
        {
            if (!is_inside(*c_it)) continue;
            std::vector<HalfFaceHandle> hfs = mMesh.cell(*c_it).halffaces();
            for (unsigned int i = 0; i < hfs.size(); ++i)
            {
                colors.clear();
                for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(hfs[i]); hfv_it; ++hfv_it)
                    colors.push_back(mColorAttrib[*hfv_it]);

                for (unsigned int i = 0; i < colors.size()-2; i++)
                {
                    addColorToBuffer(colors[0], _buffer, pos++);
                    addColorToBuffer(colors[i+1], _buffer, pos++);
                    addColorToBuffer(colors[i+2], _buffer, pos++);
                }
            }
        }
    }
    else if ((mPrimitiveMode == PM_EDGES) && (mColorMode == CM_EDGE))
    {
        OpenVolumeMesh::EdgeIter e_begin(mMesh.edges_begin()), e_end(mMesh.edges_end());
        for (OpenVolumeMesh::EdgeIter e_it = e_begin; e_it != e_end; ++e_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*e_it)) continue;
            if (!is_inside(*e_it)) continue;
            ACG::Vec4f color = mColorAttrib[*e_it];
            addColorToBuffer(color, _buffer, pos++);
            addColorToBuffer(color, _buffer, pos++);
        }
    }
    else if ((mPrimitiveMode == PM_EDGES) && (mShowIrregularInnerEdges || mShowIrregularOuterValence2Edges))
    {
        OpenVolumeMesh::EdgeIter e_begin(mMesh.edges_begin()), e_end(mMesh.edges_end());
        for (OpenVolumeMesh::EdgeIter e_it = e_begin; e_it != e_end; ++e_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*e_it)) continue;
            if (!is_inside(*e_it)) continue;

            bool boundary = mMesh.is_boundary(*e_it);
            unsigned int valence = mMesh.valence(*e_it);

            bool isIrregularInner         = ( boundary && valence <  2) || ( boundary && valence >  3) || (!boundary && valence != 4);
            bool isIrregularOuterValence2 = ( boundary && valence == 2);

            ACG::Vec4f color;

            if (isIrregularInner && mShowIrregularInnerEdges)
                color = getValenceColorCode(valence, !boundary);
            else if (isIrregularOuterValence2 && mShowIrregularOuterValence2Edges)
                color = getValenceColorCode(valence, !boundary);
            else
                color = mDefaultColor;

            addColorToBuffer(color, _buffer, pos++);
            addColorToBuffer(color, _buffer, pos++);
        }
    }
    else if ( mPrimitiveMode == PM_IRREGULAR_EDGES )
    {
        OpenVolumeMesh::EdgeIter e_begin(mMesh.edges_begin()), e_end(mMesh.edges_end());
        for (OpenVolumeMesh::EdgeIter e_it = e_begin; e_it != e_end; ++e_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*e_it)) continue;
            if (!is_inside(*e_it)) continue;

            bool boundary = mMesh.is_boundary(*e_it);
            unsigned int valence = mMesh.valence(*e_it);

            bool isRegular                = ( boundary && valence == 3) || (!boundary && valence == 4);
            bool isIrregularInner         = ( boundary && valence <  2) || ( boundary && valence >  3) || (!boundary && valence != 4);
            bool isIrregularOuterValence2 = ( boundary && valence == 2);

            if (isRegular)                                                     continue;
            if (isIrregularInner         && !mShowIrregularInnerEdges)         continue;
            if (isIrregularOuterValence2 && !mShowIrregularOuterValence2Edges) continue;

            ACG::Vec4f color;

            if (isIrregularInner && mShowIrregularInnerEdges)
                color = getValenceColorCode(valence, !boundary);
            else if (isIrregularOuterValence2 && mShowIrregularOuterValence2Edges)
                color = getValenceColorCode(valence, !boundary);
            else
                color = mDefaultColor;

            addColorToBuffer(color, _buffer, pos++);
            addColorToBuffer(color, _buffer, pos++);
        }
    }
    else if ((mPrimitiveMode == PM_HALFEDGES) && (mColorMode == CM_HALFEDGE))
    {
        OpenVolumeMesh::HalfEdgeIter he_begin(mMesh.halfedges_begin()), he_end(mMesh.halfedges_end());
        for (OpenVolumeMesh::HalfEdgeIter he_it = he_begin; he_it != he_end; ++he_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*he_it)) continue;
            if (!is_inside(*he_it)) continue;
            ACG::Vec4f color = mColorAttrib[*he_it];

            addColorToBuffer(color, _buffer, pos++);
            addColorToBuffer(color, _buffer, pos++);
        }
    }

}

/**
 * @brief Adds all picking colors to the buffer
 *
 * @param _state The current state which provides the picking colors.
 * @param _offset An offset that is added to the primitive's idx when asking for a picking color. This is necessary when different types of primitives
 *                are picked simultaneously. The number of other primives
 * @param _buffer Pointer to the start of the buffer
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::buildPickColorBuffer(ACG::GLState& _state, unsigned int _offset, unsigned char* _buffer)
{
    unsigned int pos = 0;

    if (mPrimitiveMode == PM_VERTICES)
    {
        for (unsigned int i = 0; i < mMesh.n_vertices(); ++i)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(VertexHandle(i))) continue;
            if (!is_inside(VertexHandle(i))) continue;
            ACG::Vec4uc color = _state.pick_get_name_color(VertexHandle(i).idx() + _offset);
            addColorToBuffer(color, _buffer, pos++);
        }
    }
    if (mPrimitiveMode == PM_VERTICES_ON_CELLS)
    {
        for (unsigned int i = 0; i < mMesh.n_vertices(); ++i) {
            ACG::Vec4uc color = _state.pick_get_name_color(VertexHandle(i).idx() + _offset);
            unsigned int numOfIncidentCells = getNumOfIncidentCells(VertexHandle(i));
            for (unsigned int j = 0; j < numOfIncidentCells; j++)
                addColorToBuffer(color, _buffer, pos++);
        }
    }
    else if (mPrimitiveMode == PM_EDGES)
    {
        OpenVolumeMesh::EdgeIter e_begin(mMesh.edges_begin()), e_end(mMesh.edges_end());
        for (OpenVolumeMesh::EdgeIter e_it = e_begin; e_it != e_end; ++e_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*e_it)) continue;
            if (!is_inside(*e_it)) continue;
            ACG::Vec4uc color = _state.pick_get_name_color(e_it->idx()+_offset);
            addColorToBuffer(color, _buffer, pos++);
            addColorToBuffer(color, _buffer, pos++);
        }
    }
    else if (mPrimitiveMode == PM_EDGES_ON_CELLS)
    {
        OpenVolumeMesh::EdgeIter e_begin(mMesh.edges_begin()), e_end(mMesh.edges_end());
        for (OpenVolumeMesh::EdgeIter e_it = e_begin; e_it != e_end; ++e_it)
        {
            ACG::Vec4uc color = _state.pick_get_name_color(e_it->idx()+_offset);
            unsigned int numOfIncidentCells = getNumOfIncidentCells(*e_it);
            for (unsigned int i = 0; i < numOfIncidentCells*2;i++)
                addColorToBuffer(color, _buffer, pos++);
        }
    }
    else if (mPrimitiveMode == PM_FACES)
    {
        OpenVolumeMesh::FaceIter f_begin(mMesh.faces_begin()), f_end(mMesh.faces_end());
        for (OpenVolumeMesh::FaceIter f_it = f_begin; f_it != f_end; ++f_it)
        {
            if (mBoundaryOnly && !mMesh.is_boundary(*f_it)) continue;
            if (!is_inside(*f_it)) continue;
            ACG::Vec4uc color = _state.pick_get_name_color(f_it->idx());

            unsigned int numOfVertices = 0;
            for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(mMesh.halfface_handle(*f_it,0)); hfv_it; ++hfv_it)
                ++numOfVertices;

            for (unsigned int i = 0; i < (numOfVertices-2)*3; i++)
                addColorToBuffer(color, _buffer, pos++);
        }
    }
    else if (mPrimitiveMode == PM_FACES_ON_CELLS)
    {
        OpenVolumeMesh::FaceIter f_begin(mMesh.faces_begin()), f_end(mMesh.faces_end());
        for (OpenVolumeMesh::FaceIter f_it = f_begin; f_it != f_end; ++f_it)
        {
            ACG::Vec4uc color = _state.pick_get_name_color(f_it->idx()+_offset);

            unsigned int numOfVerticesInHalfface = 0;
            for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(mMesh.halfface_handle(*f_it,0)); hfv_it; ++hfv_it)
                ++numOfVerticesInHalfface;

            for (int i = 0; i < 2; i++)
            {
                OpenVolumeMesh::CellHandle ch = mMesh.incident_cell(mMesh.halfface_handle(*f_it,i));
                if (ch != CellHandle(-1))
                {
                    if (!is_inside(ch)) continue;
                    for (unsigned int i = 0; i < (numOfVerticesInHalfface-2)*3; i++)
                        addColorToBuffer(color, _buffer, pos++);
                }
            }
        }
    }
    else if (mPrimitiveMode == PM_CELLS)
    {
        OpenVolumeMesh::CellIter c_begin(mMesh.cells_begin()), c_end(mMesh.cells_end());
        for (OpenVolumeMesh::CellIter c_it = c_begin; c_it != c_end; ++c_it)
        {
            if (!is_inside(*c_it)) continue;
            ACG::Vec4uc color = _state.pick_get_name_color(c_it->idx()+_offset);
            std::vector<HalfFaceHandle> hfs = mMesh.cell(*c_it).halffaces();
            for (unsigned int i = 0; i < hfs.size(); ++i)
            {
                unsigned int numOfVerticesInHalfface = 0;
                for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mMesh.hfv_iter(hfs[i]); hfv_it; ++hfv_it)
                    ++numOfVerticesInHalfface;

                unsigned int numOfDrawnFaces = numOfVerticesInHalfface-2;
                for (unsigned int i = 0; i < numOfDrawnFaces*3; i++)
                    addColorToBuffer(color, _buffer, pos++);
            }
        }
    }
}

/**
 * @brief Returns the name of the buffer
 *
 * This method also builds the buffer when necessary.
 *
 * @return The name of the buffer
 */
template <class VolumeMesh>
GLuint VolumeMeshBufferManager<VolumeMesh>::getBuffer()
{
    if ((mBuffer == 0) || optionsChanged() || mInvalidated)
    {

        if (mBuffer == 0)
            glGenBuffers(1, &mBuffer);

        calculateVertexDeclaration();

        if (optionsChanged())
            mNumOfVertices = -1;

        unsigned int numOfVertices = getNumOfVertices();

        if (getNumOfVertices() > 0)
        {

            unsigned int bufferSize = mVertexSize * numOfVertices;

            ACG::GLState::bindBuffer(GL_ARRAY_BUFFER, mBuffer);
            glBufferData(GL_ARRAY_BUFFER, bufferSize, 0, GL_STATIC_DRAW);

            unsigned char* buffer = (unsigned char *) glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

            if (buffer)
            {

                if (positionsNeedRebuild())
                    buildVertexBuffer(buffer);
                if (normalsNeedRebuild())
                    buildNormalBuffer(buffer);
                if (colorsNeedRebuild())
                    buildColorBuffer(buffer);


                glUnmapBuffer(GL_ARRAY_BUFFER);

                ACG::GLState::bindBuffer(GL_ARRAY_BUFFER, 0);

            }
            else
            {
                std::cerr << "error while mapping buffer" << std::endl;
            }

        }

        saveOptions();
        mInvalidated     = false;
        mGeometryChanged = false;
        mColorsChanged   = false;
        mNormalsChanged  = false;

    }

    return mBuffer;
}

/**
 * @brief Returns the name of the pick buffer
 *
 * @param _state The state that provides the picking colors
 * @param _offset The offset for the picking colors
 *
 * @return The name of the pick buffer
 */
template <class VolumeMesh>
GLuint VolumeMeshBufferManager<VolumeMesh>::getPickBuffer(ACG::GLState &_state, unsigned int _offset)
{
    if (_offset != mCurrentPickOffset)
        invalidateColors();

    if ((mBuffer == 0) || optionsChanged() || mInvalidated)
    {
        if (mBuffer == 0)
            glGenBuffers(1, &mBuffer);

        calculateVertexDeclaration();

        if (optionsChanged())
            mNumOfVertices = -1;

        unsigned int numOfVertices = getNumOfVertices();

        if (getNumOfVertices() > 0)
        {

            unsigned int bufferSize = mVertexSize * numOfVertices;

            ACG::GLState::bindBuffer(GL_ARRAY_BUFFER, mBuffer);
            glBufferData(GL_ARRAY_BUFFER, bufferSize, 0, GL_STATIC_DRAW);

            unsigned char* buffer = (unsigned char *) glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

            if (buffer)
            {

                if (positionsNeedRebuild())
                    buildVertexBuffer(buffer);
                if (colorsNeedRebuild())
                    buildPickColorBuffer(_state, _offset, buffer);

                glUnmapBuffer(GL_ARRAY_BUFFER);

                ACG::GLState::bindBuffer(GL_ARRAY_BUFFER, 0);

            }
            else
            {
                std::cerr << "error while mapping buffer" << std::endl;
            }

        }

        mCurrentPickOffset = _offset;
        saveOptions();
        mInvalidated = false;
        mGeometryChanged = false;
        mColorsChanged   = false;

    }

    return mBuffer;
}

/**
 * @brief Invalidates the buffer
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::invalidate()
{
    invalidateGeometry();
    invalidateNormals();
    invalidateColors();
}

/**
 * @brief Invalidates geometry
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::invalidateGeometry()
{
    mInvalidated         = true;
    mNumOfVertices       = -1;
    mGeometryChanged     = true;
    mColorsChanged       = true;
    mNormalsChanged      = true;
    mCogsValid           = false;
    mCellInsidenessValid = false;
}

/**
 * @brief Invalidates colors
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::invalidateColors()
{
    mInvalidated   = true;
    mColorsChanged = true;
}

/**
 * @brief Invalidates normals
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::invalidateNormals()
{
    mInvalidated    = true;
    mNormalsChanged = true;
}

/**
 * @brief Deletes the buffers on the GPU
 *
 * This also invalidates the buffer
 */
template <class VolumeMesh>
void VolumeMeshBufferManager<VolumeMesh>::free()
{
    if (mBuffer != 0)
        glDeleteBuffers(1, &mBuffer);

    mBuffer = 0;

    invalidate();
}
