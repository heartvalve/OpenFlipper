#define FILEOFFPLUGIN_C

#include "FileOFF.hh"

#include <OpenMesh/Core/Utils/color_cast.hh>
#include <OpenMesh/Core/Geometry/VectorT.hh>


template< class MeshT >
bool FileOFFPlugin::writeMesh(std::ostream& _out, MeshT& _mesh ){
    
    unsigned int i, j, nV, nF, idx;
    Vec3f v, n;
    Vec2f t;
    typename MeshT::VertexHandle vh;
    std::vector<typename MeshT::VertexHandle> vhandles;
    OpenMesh::Vec3f c;
    float alpha;
        
    /*****************
    * HEADER
    ******************/
    
    // Write option ST
    if(_mesh.has_vertex_texcoords2D() && userWriteOptions_ & OFFImporter::VERTEXTEXCOORDS) {
        _out << "ST ";
    }
    
    // Write option C
    if(_mesh.has_vertex_colors() && userWriteOptions_ & OFFImporter::VERTEXCOLOR) {
        _out << "C ";
    }
    
    // Write option N
    if(_mesh.has_vertex_normals() && userWriteOptions_ & OFFImporter::VERTEXNORMAL) {
        _out << "N ";
    }
    
    // Write
    _out << "OFF";
    
    // Write BINARY
    if(userWriteOptions_ & OFFImporter::BINARY) {
        _out << " BINARY";
    }
    
    _out << "\n";
    
    /*****************
    * DATA
    ******************/
    
    // Call corresponding write routine
    if(userWriteOptions_ & OFFImporter::BINARY) {
        return writeBinaryData(_out, _mesh);
    } else {
        return writeASCIIData(_out, _mesh);
    }
}

//------------------------------------------------------------------------------------------------------

template< class MeshT >
bool FileOFFPlugin::writeASCIIData(std::ostream& _out, MeshT& _mesh ) {
    
    typename MeshT::Point p;
    typename MeshT::Normal n;
    typename OpenMesh::Vec4f c;
    typename MeshT::TexCoord2D t;
    
    typename MeshT::VertexIter vit = _mesh.vertices_begin();
    typename MeshT::VertexIter end_vit = _mesh.vertices_end();
    
    // #V #F #E
    _out << _mesh.n_vertices() << " " << _mesh.n_faces() << " " << _mesh.n_edges();
    
    // Write vertex data
    for(; vit != end_vit; ++vit) {
        
        _out << "\n";
        
        // Write vertex p[0] p[1] p[2]
        p = _mesh.point(vit.handle());
        _out << p[0] << " " << p[1] << " " << p[2];
        
        // Write vertex normals
        if(_mesh.has_vertex_normals() && (userWriteOptions_ & OFFImporter::VERTEXNORMAL)) {
            n = _mesh.normal(vit.handle());
            _out << " " << n[0] << " " << n[1] << " " << n[2];
        }
        
        // Write vertex colors
        if(_mesh.has_vertex_colors() && (userWriteOptions_ & OFFImporter::VERTEXCOLOR)) {
            c = OpenMesh::color_cast<OpenMesh::Vec4f> (_mesh.color(vit.handle()));
            _out.precision(6);
            _out << " " << std::showpoint << c[0] << " " << std::showpoint << c[1] << " " << std::showpoint << c[2];
        }
        
        // Write vertex texcoords
        if(_mesh.has_vertex_texcoords2D() && (userWriteOptions_ & OFFImporter::VERTEXTEXCOORDS)) {
            t = _mesh.texcoord2D(vit.handle());
            _out << " " << t[0] << " " << t[1];
        }
    }
    
    typename MeshT::FaceIter fit = _mesh.faces_begin();
    typename MeshT::FaceIter end_fit = _mesh.faces_end();
    typename MeshT::FaceVertexIter fvit;
    
    // Write face data
    for(; fit != end_fit; ++fit) {
        
        _out << "\n";
        
        // Write face valence
        _out << _mesh.valence(fit.handle());
        
        // Get face-vertex iterator
        fvit = _mesh.fv_iter(fit.handle());
        
        // Write vertex indices
        for(;fvit; ++fvit) {
            _out << " " << fvit.handle().idx();
        }
        
        // Write face colors
        if(_mesh.has_face_colors() && userWriteOptions_ & OFFImporter::FACECOLOR) {
            c = OpenMesh::color_cast<OpenMesh::Vec4f> (_mesh.color(fit.handle()));
            _out.precision(6);
            _out << " " << std::showpoint << c[0] << " " << std::showpoint << c[1] << " " << std::showpoint << c[2];
            
            if(userWriteOptions_ & OFFImporter::COLORALPHA) _out <<  " " << std::showpoint << c[3];
        }
    }
    
    return true;
}

//------------------------------------------------------------------------------------------------------

template< class MeshT >
bool FileOFFPlugin::writeBinaryData(std::ostream& _out, MeshT& _mesh ){
    
    Vec3f v, n;
    Vec2f t;
    OpenMesh::Vec4f c;
    OpenMesh::Vec3f p;
    
    typename MeshT::VertexIter vit = _mesh.vertices_begin();
    typename MeshT::VertexIter end_vit = _mesh.vertices_end();
    
    // #vertices, #faces, #edges
    writeValue(_out, (uint)_mesh.n_vertices() );
    writeValue(_out, (uint)_mesh.n_faces() );
    writeValue(_out, (uint)_mesh.n_edges() );
    
    // Write vertex data
    for(; vit != end_vit; ++vit) {
   
        // Write vertex p[0] p[1] p[2]
        p = _mesh.point(vit.handle());
        writeValue(_out, p[0]);
        writeValue(_out, p[1]);
        writeValue(_out, p[2]);
        
        // Write vertex normals
        if(_mesh.has_vertex_normals() && (userWriteOptions_ & OFFImporter::VERTEXNORMAL)) {
            n = _mesh.normal(vit.handle());
            writeValue(_out, n[0]);
            writeValue(_out, n[1]);
            writeValue(_out, n[2]);
        }
        
        // Write vertex colors
        if(_mesh.has_vertex_colors() && (userWriteOptions_ & OFFImporter::VERTEXCOLOR)) {
            c = OpenMesh::color_cast<OpenMesh::Vec4f> (_mesh.color(vit.handle()));
            writeValue(_out, c[0]);
            writeValue(_out, c[1]);
            writeValue(_out, c[2]);
        }
        
        // Write vertex texcoords
        if(_mesh.has_vertex_texcoords2D() && (userWriteOptions_ & OFFImporter::VERTEXTEXCOORDS)) {
            t = _mesh.texcoord2D(vit.handle());
            writeValue(_out, t[0]);
            writeValue(_out, t[1]);
        }
    }
    
    typename MeshT::FaceIter fit = _mesh.faces_begin();
    typename MeshT::FaceIter end_fit = _mesh.faces_end();
    typename MeshT::FaceVertexIter fvit;
    
    // Write face data
    for(; fit != end_fit; ++fit) {
        
        // Write face valence
        writeValue(_out, _mesh.valence(fit.handle()));
        
        // Get face-vertex iterator
        fvit = _mesh.fv_iter(fit.handle());
        
        // Write vertex indices
        for(;fvit; ++fvit) {
            writeValue(_out, fvit.handle().idx());
        }
        
        // Write face colors
        if(_mesh.has_face_colors() && (userWriteOptions_ & OFFImporter::FACECOLOR)) {
            
            // Number of color components
            if(userWriteOptions_ & OFFImporter::COLORALPHA) writeValue(_out, (uint)4);
            else writeValue(_out, (uint)3);
            
            // Color itself
            c = OpenMesh::color_cast<OpenMesh::Vec4f> (_mesh.color(fit.handle()));
            writeValue(_out, c[0]);
            writeValue(_out, c[1]);
            writeValue(_out, c[2]);
            
            if(userWriteOptions_ & OFFImporter::COLORALPHA) writeValue(_out, c[3]);
        } else {
            // There are no face colors
            writeValue(_out, (uint)0);
        }
    }
    
    return true;
}