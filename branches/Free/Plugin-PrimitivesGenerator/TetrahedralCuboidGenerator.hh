/*===========================================================================*\
*                                                                            *
 *                              OpenFlipper                                   *
 *      Copyright (C) 2001-2013 by Computer Graphics Group, RWTH Aachen       *
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

#pragma once

#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>

struct SortedFace
{
    explicit SortedFace(std::vector<OpenVolumeMesh::VertexHandle> const& face)
        : v(face)
    {
        std::sort(v.begin(), v.end());
    }

    SortedFace(OpenVolumeMesh::VertexHandle v1,
            OpenVolumeMesh::VertexHandle v2,
            OpenVolumeMesh::VertexHandle v3)
        : v(3)
    {
        v[0] = v1;
        v[1] = v2;
        v[2] = v3;
        std::sort(v.begin(), v.end());
    }

    std::vector<OpenVolumeMesh::VertexHandle> v;
};

inline bool operator<(SortedFace const& f1, SortedFace const& f2)
{
    return std::lexicographical_compare(f1.v.begin(), f1.v.end(),
            f2.v.begin(), f2.v.end());
}

class TetrahedralCuboidGenerator
{
public:
    TetrahedralCuboidGenerator(PolyhedralMesh& mesh, Vector const& position, Vector const& length,
                                    unsigned const n_x, unsigned const n_y, unsigned const n_z);

private:
    void add_vertices(Vector const& position, Vector const& length);
    void get_cube_vertices(std::size_t i, std::size_t j, std::size_t k,
            std::vector<OpenVolumeMesh::VertexHandle>& v) const;

    void add_faces();
    void add_cube_type_1_faces(std::size_t i, std::size_t j, std::size_t k,
            std::vector<OpenVolumeMesh::VertexHandle> const& v);
    void add_cube_type_2_faces(std::size_t i, std::size_t j, std::size_t k,
            std::vector<OpenVolumeMesh::VertexHandle> const& v);

    void add_cells();
    void add_cube_type_1_cells(std::size_t i, std::size_t j, std::size_t k,
            std::vector<OpenVolumeMesh::VertexHandle> const& v);
    void add_cube_type_2_cells(std::size_t i, std::size_t j, std::size_t k,
            std::vector<OpenVolumeMesh::VertexHandle> const& v);

    PolyhedralMesh* mesh_;

    std::size_t size_[3];
    std::vector<OpenVolumeMesh::VertexHandle> vertices_;
    std::map<SortedFace, OpenVolumeMesh::FaceHandle> faces_;
};

