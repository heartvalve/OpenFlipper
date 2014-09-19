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
*   $Revision: 11127 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-03-15 16:18:28 +0100 (Di, 15 Mär 2011) $                     *
*                                                                            *
\*===========================================================================*/

#ifndef HOLEINFO_HH
#define HOLEINFO_HH


#include <OpenFlipper/common/perObjectData.hh>
#include <OpenFlipper/common/Types.hh>

#include "HoleFillerT.hh"

template< class MeshT >
class HoleInfo : public PerObjectData
{
  
  public :
    typedef std::vector< typename MeshT::EdgeHandle > Hole;

  private :
    // the mesh
    MeshT* mesh_;
    
    // list of holes
    std::vector< Hole > holes_;

    // holeFiller
    HoleFiller< MeshT >* filler_;
 
  public :

    //Konstruktor
    HoleInfo(MeshT* _mesh);
  
    //Destruktor
    ~HoleInfo();

    // find all holes in the mesh
    void getHoles();

    //fill hole with given index
    void fillHole(int _index, int _stages = 3 );

    //fill hole with given boundary edgeHandle
    void fillHole(typename MeshT::EdgeHandle _eh, int _stages = 3 );

    //fill all holes
    void fillAllHoles( int _stages = 3 );

    //select a hole with given index
    void selectHole(int _index);

    std::vector< std::vector< typename MeshT::EdgeHandle > >* holes();
};

#if defined(INCLUDE_TEMPLATES) && !defined(HOLEINFO_C)
#define HOLEINFO_TEMPLATES
#include "HoleInfoT.cc"
#endif

#endif //HOLEINFO_HH
