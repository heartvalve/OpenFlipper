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


#ifndef RENDERERINFO_HH
#define RENDERERINFO_HH

#include <QString>
#include <QObject>
#include <ACG/Scenegraph/DrawModes.hh>

#include <vector>

/** Type defining a currently loaded Renderer */
class RendererInfo{

  public :

    RendererInfo() {
     plugin = 0;
     name = "";
  }

  /// Pointer to the loaded plugin (Already casted when loading it)
  QObject*    plugin;

  /// Name of the plugin ( requested from the plugin on load)
  QString     name;

  /// Supported DrawModes
  ACG::SceneGraph::DrawModes::DrawMode modes;
};

class RenderManager {
  public:
    RenderManager();

    /** \brief Check if a renderer with the given name exists
     *
     * @param _name Name of the renderer
     * @return exists or not
     */
    bool rendererExists(QString _name);

    /**\brief  Get a new renderer Instance
     *
     * @param _name Name of the new renderer
     *
     * @return Pointer to renderer. If it exists, the existing one is returned!
     */
    RendererInfo* newRenderer(QString _name);

    /** \brief get renderer with the given name
     *
     * @param _name Name of the renderer
     * @return pointer or 0 if it does not exist
     */
    RendererInfo* getRenderer(QString _name);

    /** \brief count renderers for a DrawMode (excluding the default renderer)
     *
     * Checks in the list of renderers how many support the given draw mode combination.
     * The combination is checked as one complete block.
     *
     * @param _mode The mode that should be checked.
     *
     */
    int countRenderers(ACG::SceneGraph::DrawModes::DrawMode _mode);

    std::vector<int> getRendererIds(ACG::SceneGraph::DrawModes::DrawMode _mode);

    RendererInfo* operator[](unsigned int _id);

  private:
    std::vector<RendererInfo> availableRenderers_;
};

/// Get an instance of the render manager
RenderManager& renderManager();

#endif //RENDERERINFO_HH
