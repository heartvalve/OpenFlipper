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



#include <OpenFlipper/common/RendererInfo.hh>

static RenderManager        renderManager_;
static PostProcessorManager postProcessorManager_;

RenderManager& renderManager() {
  return renderManager_;
}

PostProcessorManager& postProcessorManager() {
  return postProcessorManager_;
}

RendererInfo::RendererInfo():
  plugin(0),
  name(""),
  optionsAction(0)
{
}

RendererInfo::RendererInfo(RenderInterface* _plugin,QString _name) :
    plugin(_plugin),
    name(_name),
    optionsAction(0)
{
}


RenderManager::RenderManager()
{
  availableRenderers_.clear();
  availableRenderers_.push_back(RendererInfo(0,"Default internal renderer"));

  activeRenderers_.clear();
}

bool RenderManager::rendererExists(QString _name) {

  for ( unsigned int i = 0 ; i < availableRenderers_.size() ; ++i)
    if ( availableRenderers_[i].name == _name)
      return true;

  return false;

}

RendererInfo* RenderManager::newRenderer(QString _name) {

  for ( unsigned int i = 0 ; i < availableRenderers_.size() ; ++i)
    if ( availableRenderers_[i].name == _name)
      return &availableRenderers_[i];

  availableRenderers_.push_back(RendererInfo());
  availableRenderers_[availableRenderers_.size()-1].name = _name;

  return &availableRenderers_[availableRenderers_.size()-1];
}

RendererInfo* RenderManager::getRenderer(QString _name) {

  for ( unsigned int i = 0 ; i < availableRenderers_.size() ; ++i)
    if ( availableRenderers_[i].name == _name)
      return &availableRenderers_[i];

  return 0;
}

int RenderManager::getRendererId(QString _name) {

  for ( unsigned int i = 0 ; i < availableRenderers_.size() ; ++i)
    if ( availableRenderers_[i].name == _name)
      return i;

  return -1;
}

int RenderManager::countRenderers(ACG::SceneGraph::DrawModes::DrawMode _mode) {

  int renderers = 0;

  // Skip first one as it is the default renderer
  for ( unsigned int i = 1 ; i < availableRenderers_.size() ; ++i)
     if ( (availableRenderers_[i].modes & _mode) )
       renderers++;

  return renderers;
}

RendererInfo* RenderManager::operator[](unsigned int _id) {

  if ( _id >= availableRenderers_.size())
    return 0;
  else
    return &availableRenderers_[_id];

}

unsigned int RenderManager::available() {
  return availableRenderers_.size();
}


void RenderManager::setActive(unsigned int _active, int _id) {

  if ( _id < 0 ) {
    std::cerr << "RenderManager::setActive illegal viewer id: " << _id << std::endl;
    return;
  }

  // Increase vector size
  if ( _id >= (int)activeRenderers_.size() )
    activeRenderers_.resize(_id +1 );

  if ( _active <  availableRenderers_.size() )
    activeRenderers_[_id] = _active;
  else
    std::cerr << "Out of range error when setting active renderer" << std::endl;
}

void RenderManager::setActive(QString _active, int _id) {

  if ( _id < 0 ) {
    std::cerr << "RenderManager::setActive illegal viewer id: " << _id << std::endl;
    return;
  }

  // Increase vector size
  if ( _id >= (int)activeRenderers_.size() ) {
    activeRenderers_.resize(_id +1 );

  }

  for ( unsigned int i = 0 ; i < availableRenderers_.size() ; ++i)
    if ( availableRenderers_[i].name == _active) {
      activeRenderers_[_id] = i;
    }

}

RendererInfo* RenderManager::active( int _id) {

  // Temporary viewer with no fixed id, return first renderer
  if ( _id < 0 )
    return 0;

  // Increase vector size
  if ( _id >= (int)activeRenderers_.size() )
    activeRenderers_.resize(_id +1 );

  return &availableRenderers_[activeRenderers_[_id]];
}

unsigned int RenderManager::activeId( int _id) {

  // Temporary viewer with no fixed id, return first renderer
  if ( _id < 0 )
    return 0;

  // Increase vector size
  if ( _id >= (int)activeRenderers_.size() )
    activeRenderers_.resize(_id +1 );

  return activeRenderers_[_id];
}

//===================================================================

PostProcessorInfo::PostProcessorInfo(PostProcessorInterface* _plugin, QString _name) :
          plugin(_plugin),
          name(_name),
          optionsAction(0)
{
}

PostProcessorInfo::PostProcessorInfo():
    plugin(0),
    name(""),
    optionsAction(0)
{
}


PostProcessorManager::PostProcessorManager()
{
  availablePostProcessors_.clear();
  availablePostProcessors_.push_back(PostProcessorInfo(0,"Default internal post processor"));

  activePostProcessors_.clear();
}


bool PostProcessorManager::postProcessorExists(QString _name) {

  for ( unsigned int i = 0 ; i < availablePostProcessors_.size() ; ++i)
    if ( availablePostProcessors_[i].name == _name)
      return true;

  return false;

}

PostProcessorInfo* PostProcessorManager::newPostProcessor(QString _name) {

  for ( unsigned int i = 0 ; i < availablePostProcessors_.size() ; ++i)
    if ( availablePostProcessors_[i].name == _name)
      return &availablePostProcessors_[i];

  availablePostProcessors_.push_back(PostProcessorInfo());
  availablePostProcessors_[availablePostProcessors_.size()-1].name = _name;

  return &availablePostProcessors_[availablePostProcessors_.size()-1];
}

PostProcessorInfo* PostProcessorManager::getPostProcessor(QString _name) {

  for ( unsigned int i = 0 ; i < availablePostProcessors_.size() ; ++i)
    if ( availablePostProcessors_[i].name == _name)
      return &availablePostProcessors_[i];

  return 0;
}

PostProcessorInfo* PostProcessorManager::operator[](unsigned int _id ) {

  if ( _id >= availablePostProcessors_.size())
    return 0;
  else
    return &availablePostProcessors_[_id];

}

unsigned int PostProcessorManager::available() {
  return availablePostProcessors_.size();
}

void PostProcessorManager::setActive(unsigned int _active, int _viewerId ) {

  // Temporary viewer with no fixed id
  if ( _viewerId < 0 ) {
    std::cerr << "PostProcessorManager::setActive illegal viewer id: " << _viewerId << std::endl;
    return;
  }

  // Increase vector size
  if ( _viewerId >= (int)activePostProcessors_.size() )
    activePostProcessors_.resize(_viewerId +1 );

  // clear chain and add postprocessor
  activePostProcessors_[_viewerId].clear();
  activePostProcessors_[_viewerId].push_back(_active);
}

void PostProcessorManager::setActive(QString _active, int _id ) {

  // Temporary viewer with no fixed id
  if ( _id < 0 ) {
    std::cerr << "PostProcessorManager::setActive illegal viewer id: " << _id << std::endl;
    return;
  }

  // Increase vector size
  if ( _id >= (int)activePostProcessors_.size() )
    activePostProcessors_.resize(_id +1 );

  for ( unsigned int i = 0 ; i < availablePostProcessors_.size() ; ++i)
    if ( availablePostProcessors_[i].name == _active) {

      // clear chain and add postprocessor
      activePostProcessors_[_id].clear();
      activePostProcessors_[_id].push_back(i);
    }

}

PostProcessorInfo* PostProcessorManager::active( int _id, int _chainIdx ) {

  // Temporary viewer with no fixed id, return first post processor
  if ( _id < 0 )
    return 0;

  // Increase vector size
   if ( _id >= (int)activePostProcessors_.size() )
     activePostProcessors_.resize(_id +1 );

   if ( (int)activePostProcessors_[_id].size() <= _chainIdx)
     return 0;

  return &availablePostProcessors_[activePostProcessors_[_id][_chainIdx]];
}

unsigned int PostProcessorManager::activeId( int _id, int _chainIdx ) {

  // Temporary viewer with no fixed id, return first post processor
  if ( _id < 0 )
    return 0;

  // Increase vector size
  if ( _id >= (int)activePostProcessors_.size() )
    activePostProcessors_.resize(_id +1 );

  if ( (int)activePostProcessors_[_id].size() <= _chainIdx)
    return 0;

  return activePostProcessors_[_id][_chainIdx];
}

int PostProcessorManager::numActive( int _id ) {

  // Temporary viewer with no fixed id, return first post processor
  if ( _id < 0 )
    return 0;

  // Increase vector size
  if ( _id >= (int)activePostProcessors_.size() )
    activePostProcessors_.resize(_id +1 );

  return (int)activePostProcessors_[_id].size();
}

void PostProcessorManager::append( unsigned int _active, int _viewerId ) {

  // Temporary viewer with no fixed id
  if ( _viewerId < 0 ) {
    std::cerr << "PostProcessorManager::append illegal viewer id: " << _viewerId << std::endl;
    return;
  }

  // Increase vector size
  if ( _viewerId >= (int)activePostProcessors_.size() )
    activePostProcessors_.resize(_viewerId +1 );

  if ( _active <  availablePostProcessors_.size() )
    activePostProcessors_[_viewerId].push_back(_active);
  else
    std::cerr << "Out of range error when setting active post processor" << std::endl;

}

void PostProcessorManager::append( QString _active, int _id ) {

  // Temporary viewer with no fixed id
  if ( _id < 0 ) {
    std::cerr << "PostProcessorManager::append illegal viewer id: " << _id << std::endl;
    return;
  }

  // Increase vector size
  if ( _id >= (int)activePostProcessors_.size() )
    activePostProcessors_.resize(_id +1 );

  for ( unsigned int i = 0 ; i < availablePostProcessors_.size() ; ++i)
    if ( availablePostProcessors_[i].name == _active) {
      activePostProcessors_[_id].push_back(i);
    }

}


void PostProcessorManager::insert( unsigned int _active, int _chainIdx, int _viewerId ) {

  // Temporary viewer with no fixed id
  if ( _viewerId < 0 ) {
    std::cerr << "PostProcessorManager::insert illegal viewer id: " << _viewerId << std::endl;
    return;
  }

  // Increase vector size
  if ( _viewerId >= (int)activePostProcessors_.size() )
    activePostProcessors_.resize(_viewerId +1 );

  if ( _active <  availablePostProcessors_.size() )
    activePostProcessors_[_viewerId].insert(activePostProcessors_[_viewerId].begin() + _chainIdx, _active);
  else
    std::cerr << "Out of range error when setting active post processor" << std::endl;

}

void PostProcessorManager::insert( QString _active, int _chainIdx, int _id ) {

  // Temporary viewer with no fixed id
  if ( _id < 0 ) {
    std::cerr << "PostProcessorManager::insert illegal viewer id: " << _id << std::endl;
    return;
  }

  // Increase vector size
  if ( _id >= (int)activePostProcessors_.size() )
    activePostProcessors_.resize(_id +1 );

  for ( unsigned int i = 0 ; i < availablePostProcessors_.size() ; ++i)
    if ( availablePostProcessors_[i].name == _active) {
      activePostProcessors_[_id].insert(activePostProcessors_[_id].begin() + _chainIdx, i);
    }

}

void PostProcessorManager::remove( int _id, int _chainIdx ) {

  // Temporary viewer with no fixed id, return first post processor
  if ( _id < 0 )
    return;

  // Increase vector size
  if ( _id >= (int)activePostProcessors_.size() )
    activePostProcessors_.resize(_id +1 );

  if ( (int)activePostProcessors_[_id].size() <= _chainIdx)
    return;

  activePostProcessors_[_id].erase(activePostProcessors_[_id].begin() + _chainIdx);

}



