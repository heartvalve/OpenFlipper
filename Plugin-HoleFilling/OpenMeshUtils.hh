/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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

//=============================================================================
//
// Often used abbreviations for OpenMesh entitites.
//
//=============================================================================

#ifndef OMAbbreviations_HH
#define OMAbbreviations_HH

#define import_om_abbreviations( Mesh )    \
\
  typedef Mesh::Point Point;   \
  typedef Mesh::Scalar Scalar; \
\
  typedef Mesh::VertexHandle   VH; \
  typedef Mesh::EdgeHandle     EH; \
  typedef Mesh::HalfedgeHandle HH; \
  typedef Mesh::FaceHandle     FH; \
\
  typedef Mesh::VertexIter     VI; \
  typedef Mesh::HalfedgeIter   HI; \
  typedef Mesh::EdgeIter       EI; \
  typedef Mesh::FaceIter       FI; \
\
  typedef Mesh::Vertex    Vertex;   \
  typedef Mesh::Halfedge  Halfedge; \
  typedef Mesh::Edge      Edge;     \
  typedef Mesh::Face      Face;     \
\
  typedef Mesh::ConstVertexIter    CVI; \
  typedef Mesh::ConstHalfedgeIter  CHI; \
  typedef Mesh::ConstEdgeIter      CEI; \
  typedef Mesh::ConstFaceIter      CFI; \
\
  typedef Mesh::VertexVertexIter    VVI;  \
  typedef Mesh::VertexOHalfedgeIter VOHI; \
  typedef Mesh::VertexIHalfedgeIter VIHI; \
  typedef Mesh::VertexEdgeIter      VEI;  \
  typedef Mesh::VertexFaceIter      VFI;  \
  typedef Mesh::FaceVertexIter      FVI;  \
  typedef Mesh::FaceHalfedgeIter    FHI;  \
  typedef Mesh::FaceEdgeIter        FEI;  \
  typedef Mesh::FaceFaceIter        FFI;  \
\
  typedef Mesh::ConstVertexVertexIter    CVVI;  \
  typedef Mesh::ConstVertexOHalfedgeIter CVOHI; \
  typedef Mesh::ConstVertexIHalfedgeIter CVIHI; \
  typedef Mesh::ConstVertexEdgeIter      CVEI;  \
  typedef Mesh::ConstVertexFaceIter      CVFI;  \
  typedef Mesh::ConstFaceVertexIter      CFVI;  \
  typedef Mesh::ConstFaceHalfedgeIter    CFHI;  \
  typedef Mesh::ConstFaceEdgeIter        CFEI;  \
  typedef Mesh::ConstFaceFaceIter        CFFI;  \

#endif
