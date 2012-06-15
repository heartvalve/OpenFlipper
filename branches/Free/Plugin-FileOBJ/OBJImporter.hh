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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/


#ifndef OBJIMPORTER_HH
#define OBJIMPORTER_HH

#define TEXTUREINDEX "OriginalTexIndexMapping"

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
typedef std::vector<OpenMesh::VertexHandle> OMVHandles;
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
    
    typedef unsigned int ObjectOptions;

    /// Constructor
    OBJImporter() : degreeU_(0),degreeV_(0),currentGroup_(0) {
        // Add default group
        addGroup("DefaultGroup");
    }

    /// base class needs virtual destructor
    ~OBJImporter();

    /// add a vertex with coordinate \c _point
    VertexHandle addVertex(const Vec3f& _point);

    /// get vertex with given index
    Vec3f vertex(unsigned int _index);
    
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
    void setObject( BaseObject* _object, int _groupId );
    
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

    /// add all vertices that are used to the mesh (in correct order)
    void addUsedVertices(int _groupId);
    
    /// set vertex texture coordinate
    void setVertexTexCoord(VertexHandle _vh, int _texCoordID);
    
    /// set vertex normal
    void setNormal(int _index, int _normalID);

    /// add a face with indices _indices refering to vertices
    void addFace(const VHandles& _indices);
    
    /// add face and texture coordinates
    void addFace(const VHandles& _indices, const std::vector<int>& _face_texcoords);
    
  private:
    bool addFace(const VHandles& _indices, OpenMesh::FaceHandle &_outFH, std::vector< TriMesh::VertexHandle > &_outTriVertices, std::vector< PolyMesh::VertexHandle > &_outPolyVertices);
  public:

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
    unsigned int n_vertices();
    unsigned int n_normals();
    unsigned int n_texCoords();

    unsigned int objectCount();
    
    /// return object with given index
    BaseObject* object(int _objectID );
    
    /// return all loaded materials
    MaterialList& materials();
    
    /// Add a material
    void addMaterial(std::string _materialName);
    
    /// used materials
    const std::vector<std::string> usedMaterials(unsigned int _objectID);
    void useMaterial( std::string _materialName );
    
    ///used vertices
    void useVertex(int _vertex_index);
    
    /// Path of the OBJ file
    QString path();
    void setPath(QString _path);
    
    /// store an initial options object for an object
    /// containing info about the meshType
    void setObjectOptions(ObjectOptions _options);
    
    /// Object Options for all objects
    std::vector< ObjectOptions >& objectOptions();
    
    /// check if object with given id has given option
    bool hasOption( unsigned int _id, ObjectOptions _option );
    
    /// change the name of an object
    void setObjectName(int _objectID, QString _name);
    
    // Add a new group
    int addGroup(const QString& _groupName);
    int groupId(const QString& _groupName) const;
    unsigned int numGroups() const { return groupNames_.size(); }
    const QString groupName(const int _grpId) const;
    void setGroupName(const int _grp, const QString& _name);

    void setCurrentGroup(const int _current);
    int currentGroup() const;

    /// Finish up importing:
    /// Duplicate vertices of non-manifold faces and
    /// add new face as isolated one
    void finish();

  private:
    
    bool vertexListIsManifold(const std::vector<PolyMesh::VertexHandle>& _vertices) const;

    // general data
    std::vector< Vec3f > vertices_;
    std::vector< Vec3f > normals_;
    std::vector< Vec2f > texCoords_;
    
    //stores half edge normals of the current face
    std::map<TriMesh::VertexHandle,TriMesh::Normal> storedTriHENormals_;
    std::map<TriMesh::VertexHandle,PolyMesh::Normal> storedPolyHENormals_;


    std::size_t halfedges_write_;

    int degreeU_;
    int degreeV_;
    
    MaterialList materials_;
    
    QString path_;
    
    std::vector<QString> groupNames_;
    int currentGroup_;

    // polyMesh data
    std::vector<std::map< int, PolyMesh::VertexHandle > > vertexMapPoly_;
    
    PolyMesh::FaceHandle addedFacePoly_;
    
    // triMesh data
    std::vector<std::map< int, TriMesh::VertexHandle > > vertexMapTri_;
    
    std::vector<std::vector< TriMesh::FaceHandle > > addedFacesTri_;
    
    //std::vector< BaseObject* > objects_;

    //object data
    std::vector<TriMeshObject*> triMeshes_;
    std::vector<PolyMeshObject*> polyMeshes_;
#ifdef ENABLE_BSPLINECURVE_SUPPORT
    std::vector<BSplineCurveObject*> bSplineCurves_;
#endif
#ifdef ENABLE_BSPLINESURFACE_SUPPORT
    std::vector<BSplineSurfaceObject*> bSplineSurfaces_;
#endif

    std::vector< ObjectOptions > objectOptions_;
    
    // for each object a vector of materialNames
    std::vector< std::vector< std::string > > usedMaterials_;
    
    // for each object a vector of vertex indices
    // this ensures that a vertex defined first gets a lower index
    std::vector< std::map< int, VertexHandle > > usedVertices_;

    // Store vertices of invalid faces due to non-manifold
    // configurations.
    std::vector< OMVHandles > invalidFaces_;

};

//=============================================================================
#endif // OBJIMPORTER_HH
//=============================================================================
