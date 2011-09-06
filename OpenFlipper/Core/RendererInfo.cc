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



#include <OpenFlipper/Core/RendererInfo.hh>

#include <iostream>

static RenderManager renderManager_;

RenderManager& renderManager() {
  return renderManager_;
}

RenderManager::RenderManager() {
  availableRenderers_.clear();

  std::cerr << "Render Manager Constructor" << std::endl;
}

bool RenderManager::rendererExists(QString _name) {
  std::cerr << "Render Manager rendererExists ?" << std::endl;

  for ( unsigned int i = 0 ; i < availableRenderers_.size() ; ++i)
    if ( availableRenderers_[i].name == _name)
      return true;

  return false;

}

RendererInfo* RenderManager::newRenderer(QString _name) {
  std::cerr << "Render Manager newRenderer" << std::endl;

  for ( unsigned int i = 0 ; i < availableRenderers_.size() ; ++i)
    if ( availableRenderers_[i].name == _name)
      return &availableRenderers_[i];

  availableRenderers_.push_back(RendererInfo());
  availableRenderers_[availableRenderers_.size()-1].name = _name;

  return &availableRenderers_[availableRenderers_.size()-1];
}

RendererInfo* RenderManager::getRenderer(QString _name) {

  std::cerr << "Render Manager getRenderer" << std::endl;

  for ( unsigned int i = 0 ; i < availableRenderers_.size() ; ++i)
    if ( availableRenderers_[i].name == _name)
      return &availableRenderers_[i];

  return 0;
}
