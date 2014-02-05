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
 *   $Revision: 17990 $                                                       *
 *   $LastChangedBy: moeller $                                                *
 *   $Date: 2014-01-07 17:36:34 +0100 (Di, 07. Jan 2014) $                     *
 *                                                                            *
 \*===========================================================================*/

#define COMPONENTSPLUGIN_CC

#include "ComponentsPlugin.hh"
#include <iostream>
#include <vector>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//------------------------------------------------------------------------------

/** \brief Split mesh into components
 *
 * This function will split an arbitrary component out of a mesh.
 * It takes a copy of the original mesh.
 *
 * @param _mesh the original mesh
 * @param _copy original mesh copy with identical topology and geometry
 *              the copy will contain one component of the original mesh.
 */
template< class MeshT >
void
ComponentsPlugin::splitComponent( MeshT*   _mesh, MeshT* _copy){

  //init properties
  OpenMesh::FPropHandleT< bool > visited;

  if ( !_mesh->get_property_handle(visited,"visited") )
    _mesh->add_property(visited, "visited");

  typename MeshT::FaceIter f_it;
  typename MeshT::FaceIter f_end = _mesh->faces_end();

  //init property for all faces
  for (f_it = _mesh->faces_begin(); f_it != f_end; ++f_it)
    _mesh->property(visited, *f_it) = false;

  typename MeshT::FaceHandle fh;

  //Take the first face in the mesh as we only split one component apart
  fh = *(_mesh->faces_begin());
  _mesh->property(visited, fh) = true;


  //if none was found -> finished
  if (!fh.is_valid() ) return;

  std::vector< typename MeshT::FaceHandle > handles;
  handles.push_back( fh );

  //grow from face and collect faces in this component
  while( handles.size() > 0 ){

    typename MeshT::FaceHandle current = handles.back();
    handles.pop_back();

    typename MeshT::FaceFaceIter ff_it;

    for (ff_it=_mesh->ff_iter( current ); ff_it.is_valid(); ++ff_it)
      if ( ! _mesh->property(visited, *ff_it) ){
        _mesh->property(visited, *ff_it) = true;
        handles.push_back( *ff_it );
      }
  }

  //delete the found component from the original mesh and keep it in the copy
  for (typename MeshT::FaceIter f_it = _mesh->faces_begin(); f_it != f_end; ++f_it)
    // -1 means not the component
    if ( _mesh->property(visited, *f_it) )
      _mesh->delete_face( *f_it );
    else
      _copy->delete_face( *f_it );


  //remove garbage from the components
  _copy->garbage_collection();
  _mesh->garbage_collection();

  _mesh->remove_property(visited);

}

template< class MeshT >
void
ComponentsPlugin::selectBiggestComponent( MeshT*   _mesh){


  OpenMesh::FPropHandleT< bool > visited;
  if ( !_mesh->get_property_handle(visited,"visited") )
    _mesh->add_property(visited, "visited");

  std::vector<typename MeshT::FaceHandle> facesInBiggest;

  for (typename MeshT::FaceIter fIter = _mesh->faces_begin(); fIter != _mesh->faces_end(); ++fIter)
  {
    if (_mesh->property(visited,*fIter))
      continue;

    //It is a vertex, which is not visited => new component
    std::vector<typename MeshT::FaceHandle> componentFaces;
    componentFaces.push_back(*fIter);

    for(std::size_t i = 0; i < componentFaces.size(); ++i )
    {
      //add all not visited neightbours
      for (typename MeshT::FaceFaceIter ffIter = _mesh->ff_begin(componentFaces[i]); ffIter.is_valid() ;++ffIter)
      {
        if (!ffIter->is_valid())
          std::cout << "handleId: " << *ffIter << std::endl;
        if (ffIter->is_valid() && !_mesh->property(visited,*ffIter) )
        {
          _mesh->property(visited,*ffIter) = true;
          componentFaces.push_back(*ffIter);
        }
      }
    }

    //all faces are found, so compare the components
    if (facesInBiggest.size() < componentFaces.size())
    {
      std::swap(facesInBiggest,componentFaces);
    }
  }
  _mesh->remove_property(visited);

  //select all faces in the biggest component;
  for (typename std::vector<typename MeshT::FaceHandle>::iterator iter = facesInBiggest.begin(); iter != facesInBiggest.end(); ++iter)
  {
    _mesh->status(*iter).set_selected(true);
  }
}

template< class MeshT >
void
ComponentsPlugin::isolateBiggestComponent( MeshT*   _mesh)
{
  for (typename MeshT::FaceIter fIter = _mesh->faces_begin(); fIter != _mesh->faces_end(); ++fIter)
  {
    if (!_mesh->status(*fIter).selected())
    {
      //delete face and isolated vertices
      _mesh->delete_face(*fIter,true);
    }
    else
    {
      _mesh->status(*fIter).set_selected(false);
    }
  }

  _mesh->garbage_collection();
}
