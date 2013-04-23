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

#ifndef VOLUME_MESH_BUFFER_MANAGER_HH
#define VOLUME_MESH_BUFFER_MANAGER_HH

#include <ACG/GL/GLState.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/Scenegraph/BaseNode.hh>
#include <ACG/GL/VertexDeclaration.hh>

#include "VolumeMeshDrawModesContainer.hh"

#include <OpenVolumeMesh/Attribs/StatusAttrib.hh>
#include <OpenVolumeMesh/Attribs/NormalAttrib.hh>
#include <OpenVolumeMesh/Attribs/ColorAttrib.hh>

/*! \class VolumeMeshBufferManager
 *  \brief This class creates buffers that can be used to render open volume meshs.
 */
template<class VolumeMesh>
class VolumeMeshBufferManager
{
    // typedefs for easy access
    typedef OpenVolumeMesh::VertexHandle VertexHandle;
    typedef OpenVolumeMesh::EdgeHandle EdgeHandle;
    typedef OpenVolumeMesh::HalfEdgeHandle HalfEdgeHandle;
    typedef OpenVolumeMesh::FaceHandle FaceHandle;
    typedef OpenVolumeMesh::HalfFaceHandle HalfFaceHandle;
    typedef OpenVolumeMesh::CellHandle CellHandle;

    typedef typename VolumeMesh::PointT Vertex;
    typedef OpenVolumeMesh::OpenVolumeMeshEdge Edge;
    typedef OpenVolumeMesh::OpenVolumeMeshFace Face;
    typedef OpenVolumeMesh::OpenVolumeMeshCell Cell;

public:

    enum PrimitiveMode
    {
        PM_NONE,
        PM_CELLS,
        PM_FACES,
        PM_FACES_ON_CELLS,
        PM_HALFFACES,
        PM_HALFFACES_ON_CELLS,
        PM_EDGES,
        PM_IRREGULAR_EDGES,
        PM_EDGES_ON_CELLS,
        PM_HALFEDGES,
        PM_VERTICES,
        PM_VERTICES_ON_CELLS
    };

    enum NormalMode
    {
        NM_NONE,
        NM_FACE,
        NM_HALFFACE,
        NM_VERTEX
    };

    enum ColorMode
    {
        CM_NONE,
        CM_CELL,
        CM_FACE,
        CM_HALFFACE,
        CM_EDGE,
        CM_HALFEDGE,
        CM_VERTEX,
        CM_PICK
    };

    enum TexCoordMode
    {
        TCM_NONE,
        TCM_VERTEX_1D,
        TCM_VERTEX_2D
    };

    class Plane {
    public:
        Plane(const ACG::Vec3d& _p, const ACG::Vec3d& _n, const ACG::Vec3d& _x, const ACG::Vec3d& _y) :
            position(_p), normal(_n), xdirection(_x), ydirection(_y) {
        }

        // position
        ACG::Vec3d position;
        // normal
        ACG::Vec3d normal;
        // xdirection scaled by 1.0/width -> [0,1] is inrange
        ACG::Vec3d xdirection;
        // ydirection scaled by 1.0/height -> [0,1] is inrange
        ACG::Vec3d ydirection;
    };

    VolumeMeshBufferManager(const VolumeMesh &mesh_, OpenVolumeMesh::StatusAttrib& statusAttrib_,
                                               OpenVolumeMesh::ColorAttrib<ACG::Vec4f>& colorAttrib_,
                                               OpenVolumeMesh::NormalAttrib<VolumeMesh>& normalAttrib_);
    virtual ~VolumeMeshBufferManager(){ free(); }


    /// Sets the scale used to shrink cells.
    void setScale(double _scale) { mScale = _scale; invalidate();}

    /// Return the scale used to shrink cells.
    double getScale() const     { return mScale;  }


    /// Returns the vertex stride
    unsigned char getStride()       { return mVertexSize; }

    /// Returns the color's offset within the buffer
    unsigned char getColorOffset()  { return mColorOffset;  }

    /// Returns the normal's offset within the buffer
    unsigned char getNormalOffset() { return mNormalOffset; }


    /// Returns a VertexDeclaration for the vertices stored in the buffer
    const ACG::VertexDeclaration* getVertexDeclaration() { return &mVertexDeclaration; }


    /// Returns the name of the buffer
    GLuint getBuffer();

    /// Returns the name of the pick buffer
    GLuint getPickBuffer(ACG::GLState &_state, unsigned int _offset);


    /// Invalidates the buffer
    void invalidate();

    /// Invalidates geometry
    void invalidateGeometry();

    /// Invalidates colors
    void invalidateColors();

    /// Invalidates normals
    void invalidateNormals();


    /// Deletes the buffers on the GPU
    void free();


    /// Returns the number of vertices stored in the buffer
    unsigned int getNumOfVertices();


    /// Sets the default color
    void setDefaultColor(ACG::Vec4f _defaultColor);


    /// Configures the buffer manager's options from a DrawMode
    void setOptionsFromDrawMode(ACG::SceneGraph::DrawModes::DrawMode _drawMode);


    /// Disables colors
    void disableColors()            { mColorMode  = CM_NONE;     }

    /// Enables per cell colors
    void enablePerCellColors()      { mColorMode  = CM_CELL;     }

    /// Enables per face colors
    void enablePerFaceColors()      { mColorMode  = CM_FACE;     }

    /// Enables per halfface colors
    void enablePerHalffaceColors()  { mColorMode  = CM_HALFFACE; }

    /// Enables per edge colors
    void enablePerEdgeColors()      { mColorMode  = CM_EDGE;     }

    /// Enables per halfedge colors
    void enablePerHalfedgeColors()  { mColorMode  = CM_HALFEDGE; }

    /// Enables per vertex colors
    void enablePerVertexColors()    { mColorMode  = CM_VERTEX;   }

    /// Enables picking colors
    void enablePickColors()         { mColorMode  = CM_PICK;     }


    /// Disables normals
    void disableNormals()           { mNormalMode = NM_NONE;     }

    /// Enables per face normals
    void enablePerFaceNormals()     { mNormalMode = NM_FACE;     }

    /// Enables per halfface normals
    void enablePerHalffaceNormals() { mNormalMode = NM_HALFFACE; }

    /// Enables per vertex normals
    void enablePerVertexNormals()   { mNormalMode = NM_VERTEX;   }


    /// Disables primitives (nothing will be drawn)
    void disablePrimitives()              { mPrimitiveMode = PM_NONE;               }

    /// Enables cell primitives
    void enableCellPrimitives()           { mPrimitiveMode = PM_CELLS;              }

    /// Enables face primitives
    void enableFacePrimitives()           { mPrimitiveMode = PM_FACES;              }

    /// Enables face primitives which are drawn on the cells
    void enableFaceOnCellPrimitives()     { mPrimitiveMode = PM_FACES_ON_CELLS;     }

    /// Enables halfface primitves
    void enableHalffacePrimitives()       { mPrimitiveMode = PM_HALFFACES;          }

    /// Enables halfface primitives which are drawn on the cells
    void enableHalffaceOnCellPrimitives() { mPrimitiveMode = PM_HALFFACES_ON_CELLS; }

    /// Enables edge primitves
    void enableEdgePrimitives()           { mPrimitiveMode = PM_EDGES;              }

    /// Enables edge primitives for irregular edges
    void enableIrregularEdgePrimitives()  { mPrimitiveMode = PM_IRREGULAR_EDGES;    }

    /// Enables edge primitives which are drawn on the cells
    void enableEdgeOnCellPrimitives()     { mPrimitiveMode = PM_EDGES_ON_CELLS;     }

    /// Enables per halfedge primitives
    void enableHalfedgePrimitives()       { mPrimitiveMode = PM_HALFEDGES;          }

    /// Enables vertex primitives
    void enableVertexPrimitives()         { mPrimitiveMode = PM_VERTICES;           }

    /// Enables vertex primitives that are drawn on the cells
    void enableVertexOnCellPrimitives()   { mPrimitiveMode = PM_VERTICES_ON_CELLS;  }

    /// Checks whether only selected primitives should be added to the buffer
    bool selectionOnly()                       { return mSkipUnselected;            }

    /// Set whether only selected primitves should be added to the buffer
    void setSelectionOnly(bool _selectionOnly) { mSkipUnselected = _selectionOnly;  }


    /// Removes all cut planes
    void clearCutPlanes();

    /// Adds a cut plane
    void addCutPlane(const Plane& _p);

    /// Adds a cut plane
    void addCutPlane(const ACG::Vec3d& _p, const ACG::Vec3d& _n, const ACG::Vec3d& _xsize, const ACG::Vec3d& _ysize);


    /// Sets whether only the boundary primitves should be rendered
    void setBoundaryOnly(bool _boundaryOnly) { mBoundaryOnly = _boundaryOnly; }

private:
    /// Tests whether the given point is inside w.r.t. all cut planes
    bool is_inside(const ACG::Vec3d& _p);

    /// Tests whether the given vertex is inside w.r.t. all cut planes
    bool is_inside(const VertexHandle& _vh);

    /// Tests whether the given halfedge is inside w.r.t. all cut planes
    bool is_inside(const HalfEdgeHandle& _heh);

    /// Tests whether the given edge is inside w.r.t. all cut planes
    bool is_inside(const EdgeHandle& _eh);

    /// Tests whether the given halfface is inside w.r.t. all cut planes
    bool is_inside(const HalfFaceHandle& _hfh);

    /// Tests whether the given face is inside w.r.t. all cut planes
    bool is_inside(const FaceHandle& _fh);

    /// Tests whether the given cell is inside w.r.t. all cut planes
    bool is_inside(const CellHandle& _ch);

    /// Calculates for all cells whether they are inside w.r.t. all cut planes
    void calculateCellInsideness();


    /// Tests whether the options were changed since the last time building the buffer
    bool optionsChanged();

    /// State that the current buffer was built with the current options
    void saveOptions();


    /// Adds a float to the buffer
    void addFloatToBuffer( float _value, unsigned char *&_buffer );

    /// Adds an unsigned char to the buffer
    void addUCharToBuffer( unsigned char _value, unsigned char *&_buffer );

    /// Adds a position to the buffer
    void addPositionToBuffer(ACG::Vec3d _position, unsigned char* _buffer , unsigned int _offset);

    /// Adds a color to the buffer
    void addColorToBuffer(ACG::Vec4uc _color, unsigned char* _buffer, unsigned int _offset);

    /// Adds a color to the buffer
    void addColorToBuffer(ACG::Vec4f _color, unsigned char* _buffer, unsigned int _offset);

    /// Adds a normal to the buffer
    void addNormalToBuffer(ACG::Vec3d _normal, unsigned char *_buffer, unsigned int _offset);


    /// Constructs a VertexDeclaration, the size and the offsets for the vertices stored in the buffer
    void calculateVertexDeclaration();


    /// Adds all vertices to the buffer
    void buildVertexBuffer(unsigned char* _buffer);

    /// Adds all normals to the buffer
    void buildNormalBuffer(unsigned char* _buffer);

    /// Adds all colors to the buffer
    void buildColorBuffer(unsigned char* _buffer);

    /// Adds all picking colors to the buffer
    void buildPickColorBuffer(ACG::GLState &_state, unsigned int _offset, unsigned char* _buffer);


    /// Returns a color code for irregular edges
    ACG::Vec4f getValenceColorCode(unsigned int _valence, bool _inner) const;


    /// Counts the number of vertices that need to be stored in the buffer
    void countNumOfVertices();


    /// Returns the number of cells that are incident to the given face and also inside w.r.t. all cut planes
    int getNumOfIncidentCells(OpenVolumeMesh::FaceHandle _fh);

    /// Returns the number of cells that are incident to the given edge and also inside w.r.t. all cut planes
    int getNumOfIncidentCells(OpenVolumeMesh::EdgeHandle _eh);

    /// Returns the number of cells that are incident to the given vertex and also inside w.r.t. all cut planes
    int getNumOfIncidentCells(OpenVolumeMesh::VertexHandle _vh);

    /// Returns the center of gravity of the given cell
    ACG::Vec3d getCOG(OpenVolumeMesh::CellHandle _ch);

    /// Calculates the center of gravity for all cells
    void calculateCOGs();

    /// Checks whether positions need to be rebuild
    bool positionsNeedRebuild();

    /// Checks whether colors need to be rebuild
    bool colorsNeedRebuild();

    /// Checks whether normals need to be rebuild
    bool normalsNeedRebuild();

private:
    ACG::Vec4f mDefaultColor;

    const VolumeMesh& mMesh;
    OpenVolumeMesh::StatusAttrib& mStatusAttrib;
    OpenVolumeMesh::ColorAttrib<ACG::Vec4f>& mColorAttrib;
    OpenVolumeMesh::NormalAttrib<VolumeMesh>& mNormalAttrib;

    int mNumOfVertices;
    int mCurrentNumOfVertices;

    unsigned int mVertexSize;
    ACG::VertexDeclaration mVertexDeclaration;

    unsigned int mColorOffset;
    unsigned int mNormalOffset;

    double mScale;

    GLuint mBuffer;

    unsigned int mCurrentPickOffset;

    bool mInvalidated;
    bool mGeometryChanged;
    bool mNormalsChanged;
    bool mColorsChanged;

    //draw modes
    VolumeMeshDrawModesContainer mDrawModes;

    //options
    PrimitiveMode mPrimitiveMode;
    NormalMode mNormalMode;
    ColorMode mColorMode;

    bool mSkipUnselected;
    bool mShowIrregularInnerEdges;
    bool mShowIrregularOuterValence2Edges;
    bool mSkipRegularEdges;
    bool mBoundaryOnly;

    PrimitiveMode mCurrentPrimitiveMode;
    NormalMode mCurrentNormalMode;
    ColorMode mCurrentColorMode;

    bool mCurrentSkipUnselected;
    bool mCurrentShowIrregularInnerEdges;
    bool mCurrentShowIrregularOuterValence2Edges;
    bool mCurrentSkipRegularEdges;
    bool mCurrentBoundaryOnly;

    unsigned char mCurrentVertexSize;
    unsigned char mCurrentNormalOffset;
    unsigned char mCurrentColorOffset;

    std::vector<Plane> cut_planes_;

    std::vector<ACG::Vec3d> mCogs;
    bool mCogsValid;
    std::vector<bool> mCellInsideness;
    bool mCellInsidenessValid;

    bool mMultiTexturing;
    TexCoordMode mTexCoordMode;
    TexCoordMode mCurrentTexCoordMode;
    unsigned char mTexCoordOffset;
    unsigned char mCurrentTexCoordOffset;



};

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(VOLUMEMESHBUFFERMANAGERT_CC)
#include "VolumeMeshBufferManagerT.cc"
#endif
//=============================================================================

#endif // VOLUME_MESH_BUFFER_MANAGER_HH
