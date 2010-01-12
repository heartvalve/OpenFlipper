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


#ifndef OBJIMPORTER_HH
#define OBJIMPORTER_HH


//=== INCLUDES ================================================================


// STL
#include <vector>

// OpenMesh
#include <OpenFlipper/common/GlobalDefines.hh>
#include <OpenMesh/Core/Geometry/VectorT.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <OpenFlipper/common/BaseObject.hh>


#ifdef ENABLE_BSPLINECURVE_SUPPORT
#include <ObjectTypes/BSplineCurve/BSplineCurve.hh>
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
#include <ObjectTypes/BSplineSurface/BSplineSurface.hh>
#endif

#include "Material.hh"

//=== IMPLEMENTATION ==========================================================

typedef int VertexHandle;
typedef int FaceHandle;
typedef std::vector<VertexHandle> VHandles;
typedef OpenMesh::Vec3f Vec3f;
typedef OpenMesh::Vec2f Vec2f;
typedef OpenMesh::Vec3uc Vec3uc;
typedef OpenMesh::Vec4uc Vec4uc;



class OBJImporter
{
  public:

    enum ObjectOptionsE
    {
      NONE             = 0,
      TRIMESH          = 1,
      POLYMESH         = 1 << 1,
      CURVE            = 1 << 2,
      SURFACE          = 1 << 3,
      NORMALS          = 1 << 4,
      TEXCOORDS        = 1 << 5,
      FACECOLOR        = 1 << 6,
      TEXTURE          = 1 << 7,
      FORCE_NOCOLOR    = 1 << 8,
      FORCE_NONORMALS  = 1 << 9,
      FORCE_NOTEXTURES = 1 << 10
    };
    
    typedef uint ObjectOptions;

    /// base class needs virtual destructor
    ~OBJImporter();

    /// add a vertex with coordinate \c _point
    VertexHandle addVertex(const Vec3f& _point);

    /// get vertex with given index
    Vec3f vertex(uint _index);
    
    /// add texture coordinates
    int addTexCoord(const Vec2f& _coord);
    
    /// add a normal
    int addNormal(const Vec3f& _normal);
    
    /// set degree
    void setDegreeU(int _degree);
    void setDegreeV(int _degree);
    
    /// get current degree
    int degreeU();
    int degreeV();
    
    /// add an object
    void addObject( BaseObject* _object );
    
    /// get id of the active object
    int currentObject();
    
    /// get a pointer to the active polyMesh
    PolyMesh* currentPolyMesh();
    
    /// get a pointer to the active triMesh
    TriMesh* currentTriMesh();

#ifdef ENABLE_BSPLINECURVE_SUPPORT
    BSplineCurve* currentCurve();
#endif

#ifdef ENABLE_BSPLINECURVE_SUPPORT
    BSplineSurface* currentSurface();
#endif

    /// check if the vertex of a face is already added to the mesh. if not add it.
    void checkExistance(VertexHandle _vh);
    
    /// set vertex texture coordinate
    void setVertexTexCoord(VertexHandle _vh, int _texCoordID);
    
    /// set vertex normal
    void setNormal(VertexHandle _vh, int _normalID);

    /// add a face with indices _indices refering to vertices
    void addFace(const VHandles& _indices);
    
    /// add face and texture coordinates
    void addFace(const VHandles& _indices, const std::vector<int>& _face_texcoords);
    
    /// force all meshes to be opened with specific type
    void forceMeshType( ObjectOptions _meshType );
    
    /// Query Object Options
    bool hasNormals(int _objectID);
    bool hasTexture(int _objectID);
    bool hasTextureCoords(int _objectID);
    bool isTriangleMesh(int _objectID);
    bool isPolyMesh(int _objectID);
    bool isCurve(int _objectID);
    bool isSurface(int _objectID);

    /// Global Properties
    uint n_vertices();
    uint n_normals();
    uint n_texCoords();

    uint objectCount();
    
    
    /// return object with given index
    BaseObject* object(int _objectID );
    
    /// return all loaded materials
    MaterialList& materials();
    
    /// Add a material
    void addMaterial(std::string _materialName);
    
    /// used materials
    const std::vector<std::string> usedMaterials(uint _objectID);
    void useMaterial( std::string _materialName );
    
    /// Path of the OBJ file
    QString path();
    void setPath(QString _path);
    
    /// store an initial options object for an object
    /// containing info about the meshType
    void addObjectOptions(ObjectOptions _options);
    
    /// Object Options for all objects
    std::vector< ObjectOptions >& objectOptions();
    
    /// check if object with given id has given option
    bool hasOption( uint _id, ObjectOptions _option );
    
    /// change the name of an object
    void setObjectName(int _objectID, QString _name);
    
    /// sets the name of the group where the objects are put into
    void setGroup(QString _groupName);
    QString group();
    
  private:
    
    // general data
    std::vector< Vec3f > vertices_;
    std::vector< Vec3f > normals_;
    std::vector< Vec2f > texCoords_;
    
    int degreeU_;
    int degreeV_;
    
    MaterialList materials_;
    
    QString path_;
    QString groupName_;
    
    // polyMesh data
    std::map< int, PolyMesh::VertexHandle > vertexMapPoly_;
    
    PolyMesh::FaceHandle addedFacePoly_;
    
    std::vector< PolyMesh* > polyMeshes_;
    
    // triMesh data
    std::map< int, TriMesh::VertexHandle > vertexMapTri_;
    
    std::vector< TriMesh::FaceHandle > addedFacesTri_;
    
    std::vector< TriMesh* > triMeshes_;
    
#ifdef ENABLE_BSPLINECURVE_SUPPORT
    std::vector< BSplineCurve* > curves_;
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
    std::vector< BSplineSurface* > surfaces_;
#endif

    //object data
    std::vector< BaseObject* > objects_;
    std::vector< ObjectOptions > objectOptions_;
    
    // for each object a vector of materialNames
    std::vector< std::vector< std::string > > usedMaterials_;

};

//=============================================================================
#endif // OBJIMPORTER_HH
//=============================================================================
