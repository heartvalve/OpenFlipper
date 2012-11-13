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

#define FILEOFFPLUGIN_C

#include "FileOFF.hh"

#include <OpenMesh/Core/Utils/color_cast.hh>
#include <OpenMesh/Core/Geometry/VectorT.hh>


template< class MeshT >
bool FileOFFPlugin::writeMesh(std::ostream& _out, MeshT& _mesh ){

    /*****************
    * HEADER
    ******************/

    // Write option ST
    if(_mesh.has_vertex_texcoords2D() && (userWriteOptions_ & OFFImporter::VERTEXTEXCOORDS) ) {
        _out << "ST ";
    }

    // Write option C
    if(_mesh.has_vertex_colors() && (userWriteOptions_ & OFFImporter::VERTEXCOLOR) ) {
        _out << "C ";
    }

    // Write option N
    if(_mesh.has_vertex_normals() && (userWriteOptions_ & OFFImporter::VERTEXNORMAL) ) {
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
      if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0)
        _out.precision(savePrecision_->value());

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
        // Note: Vertex colors always have only three components.
        // This has to be determined since it can not be read dynamically in binary files.
        if(_mesh.has_vertex_colors() && (userWriteOptions_ & OFFImporter::VERTEXCOLOR)) {
            c = OpenMesh::color_cast<OpenMesh::Vec4f> (_mesh.color(vit.handle()));
            _out << " " << std::showpoint << c[0] << " " << std::showpoint << c[1] << " " << std::showpoint << c[2] << " " << std::showpoint << c[3];
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
        if(_mesh.has_face_colors() && (userWriteOptions_ & OFFImporter::FACECOLOR ) ) {
            c = OpenMesh::color_cast<OpenMesh::Vec4f> (_mesh.color(fit.handle()));
            _out << " " << std::showpoint << c[0] << " " << std::showpoint << c[1] << " " << std::showpoint << c[2] << " " << std::showpoint << c[3];

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
    OpenMesh::Vec4f c(1.0,1.0,1.0,1.0);
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
        // Note: Vertex colors always have only three components.
        // This has to be determined since it can not be read dynamically in binary files.
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
        }
    }

    return true;
}
