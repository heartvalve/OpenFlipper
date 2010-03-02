/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openmesh.org                                *
 *                                                                           *
 *---------------------------------------------------------------------------* 
 *  This file is part of OpenMesh.                                           *
 *                                                                           *
 *  OpenMesh is free software: you can redistribute it and/or modify         * 
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
 *  OpenMesh is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenMesh.  If not,                                    *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/ 

/*===========================================================================*\
 *                                                                           *             
 *   $Revision: 137 $                                                         *
 *   $Date: 2009-06-04 10:46:29 +0200 (Thu, 04 Jun 2009) $                   *
 *                                                                           *
\*===========================================================================*/


#ifndef OFFIMPORTER_HH
#define OFFIMPORTER_HH


//=== INCLUDES ================================================================


// STL
#include <vector>

// OpenMesh
#include <OpenFlipper/common/GlobalDefines.hh>
#include <OpenMesh/Core/Geometry/VectorT.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <OpenFlipper/common/BaseObject.hh>

//=== IMPLEMENTATION ==========================================================

typedef int VertexHandle;
typedef int FaceHandle;
typedef std::vector<VertexHandle> VHandles;
typedef OpenMesh::Vec3f Vec3f;
typedef OpenMesh::Vec2f Vec2f;
typedef OpenMesh::Vec4uc Vec4uc;
typedef OpenMesh::Vec3uc Vec3uc;


class OFFImporter
{
  public:

    enum ObjectOptionsE
    {
      NONE             = 0,
      BINARY           = 1,
      TRIMESH          = 1 << 1,
      POLYMESH         = 1 << 2,
      VERTEXNORMAL     = 1 << 3,
      VERTEXTEXCOORDS  = 1 << 4,
      VERTEXCOLOR      = 1 << 5,
      FACECOLOR        = 1 << 6,
      COLORALPHA       = 1 << 7,
      FORCE_NOCOLOR    = 1 << 8,
      FORCE_NONORMALS  = 1 << 9,
      FORCE_NOTEXTURES = 1 << 10,
      SKIPCOLORDET     = 1 << 11
    };
    
    typedef uint ObjectOptions;

    /// constructor
    OFFImporter();
    
    /// base class needs virtual destructor
    ~OFFImporter();

    /// add initial object
    void addObject( BaseObject* _object );
    
    /// add a vertex with coordinate \c _point
    VertexHandle addVertex(const Vec3f& _point);

    /// get vertex with given index
    Vec3f vertex(uint _index);
    
    /// add texture coordinates
    int addTexCoord(const Vec2f& _coord);
    
    /// add a color
    int addColor(const Vec4uc& _color);
    
    /// add a color
    int addColor(const Vec3uc& _color);
    
    /// add a normal
    int addNormal(const Vec3f& _normal);
        
    /// get a pointer to the active polyMesh
    PolyMesh* polyMesh();
    
    /// get a pointer to the active triMesh
    TriMesh* triMesh();

    /// set vertex texture coordinate
    void setVertexTexCoord(VertexHandle _vh, int _texCoordID);
    
    /// set vertex normal
    void setNormal(VertexHandle _vh, int _normalID);
    
    /// set vertex color
    void setVertexColor(VertexHandle _vh, int _colorIndex);
    
    /// set face color
    void setFaceColor(FaceHandle _fh, int _colorIndex);
    
    /// add a face with indices _indices refering to vertices
    int addFace(const VHandles& _indices);
    
    /// Query Object Options
    bool hasVertexNormals();
    bool hasTextureCoords();
    bool hasVertexColors();
    bool hasFaceColors();
    bool isTriangleMesh();
    bool isPolyMesh();
    bool isBinary();
    
    /// Global Properties
    uint n_vertices();
    uint n_normals();
    uint n_texCoords();
        
    /// Path of the OFF file
    QString path();
    void setPath(QString _path);
    
    /// store an initial options object for an object
    /// containing info about the meshType
    void setObjectOptions(ObjectOptions _options);
    
    /// add an option
    void addOption(ObjectOptionsE _option);
    
    /// remove an option
    void removeOption(ObjectOptionsE _option);
    
    /// test if object has a certain option
    bool hasOption(ObjectOptionsE _option);
    
    /// get Object Options
    ObjectOptions& objectOptions();
        
    /// change the name of an object
    void setObjectName(QString _name);

    /// get BaseObject data of object
    BaseObject* getObject();
    
  private:
    
    // general data
    std::vector< Vec3f > vertices_;
    std::vector< Vec3f > normals_;
    std::vector< Vec2f > texCoords_;
    std::vector< Vec4uc > colors_;
    
    // file path
    QString path_;
    
    // polyMesh data
    std::map< int, PolyMesh::VertexHandle > vertexMapPoly_;
    
    std::vector< PolyMesh::FaceHandle > faceMapPoly_;
    
    PolyMesh* polyMesh_;
    
    // triMesh data
    std::map< int, TriMesh::VertexHandle > vertexMapTri_;
    
    std::vector< TriMesh::FaceHandle > faceMapTri_;
    
    TriMesh* triMesh_;

    //object data
    BaseObject* object_;
    ObjectOptions objectOptions_;
};

//=============================================================================
#endif // OFFIMPORTER_HH
//=============================================================================
