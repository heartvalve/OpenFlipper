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
#include <OpenFlipper/BasePlugin/PostProcessorInterface.hh>
#include <OpenFlipper/BasePlugin/RenderInterface.hh>
#include <OpenFlipper/common/GlobalDefines.hh>

#include <vector>

/** Type defining a currently loaded Renderer */
class DLLEXPORT RendererInfo{

  public :

    RendererInfo();

    RendererInfo(RenderInterface* _plugin,QString _name);

    /// Pointer to the loaded plugin (Already casted when loading it)
    RenderInterface*    plugin;

    /// Name of the plugin ( requested from the plugin on load)
    QString     name;

    /// Supported DrawModes
    ACG::SceneGraph::DrawModes::DrawMode modes;

    /// Possible action to add an options action or menu to the system.
    QAction* optionsAction;
};


class DLLEXPORT RenderManager {
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

    /** \brief get renderer id with the given name
     *
     * @param _name Name of the renderer
     * @return Id of the renderer or -1 if it does not exist
     */
    int getRendererId(QString _name);

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

    /** \brief Get the renderer with the given id
     *
     * @param _id Id of the renderer
     * @return
     */
    RendererInfo* operator[](unsigned int _id);

    /** \brief number of available renderers
     *
     * @return number of available renderers
     */
    unsigned int available();

    /** \brief set the active renderer
     *
     * @param _id viewer id
     * @param _active id of the renderer
     */
    void setActive(unsigned int _active, int _id);

    /** \brief set the active renderer
    *
    * @param _active name of the renderer
    * @param _id viewer id
    */
    void setActive(QString _active, int _id);

    /** \brief Get the current active renderer
     *
     * @param _id viewer id
     * @return Renderer
     */
    RendererInfo* active(int _id);

    /** \brief Get the id of the active renderer
     *
     * @param _id viewer id
     * @return renderer id
     */
    unsigned int activeId(int _id);

  private:
    /// Vector holding all available renderers
    std::vector<RendererInfo> availableRenderers_;

    /// The currently active renderer ids
    std::vector<unsigned int> activeRenderers_;
};

/// Get an instance of the render manager
DLLEXPORT
RenderManager& renderManager();

//===================================================================================
// Post processor Manager
//===================================================================================

/** Type defining a currently loaded Post processor */
class DLLEXPORT PostProcessorInfo{

  public :

    PostProcessorInfo();

    PostProcessorInfo(PostProcessorInterface* _plugin, QString _name);

    /// Pointer to the loaded plugin (Already casted when loading it)
    PostProcessorInterface*    plugin;

    /// Name of the plugin ( requested from the plugin on load)
    QString     name;

    /// Possible action to add an options action or menu to the system.
    QAction* optionsAction;

};


class DLLEXPORT PostProcessorManager {
  public:

    PostProcessorManager();

    /** \brief Check if a post processor with the given name exists
     *
     * @param _name Name of the post processor
     * @return exists or not
     */
    bool postProcessorExists(QString _name);

    /**\brief  Get a new post processor Instance
     *
     * @param _name Name of the new post processor
     *
     * @return Pointer to post processor. If it exists, the existing one is returned!
     */
    PostProcessorInfo* newPostProcessor(QString _name);

    /** \brief get post processor with the given name
     *
     * @param _name Name of the post processor
     * @return pointer or 0 if it does not exist
     */
    PostProcessorInfo* getPostProcessor(QString _name);

    /** \brief Get the  post processor with the given id
    *
    * @param _id Id of the  post processor
    * @return
    */
    PostProcessorInfo* operator[](unsigned int _id);

    /** \brief number of available post processor
    *
    * @return number of available post processor
    */
    unsigned int available();

    /** \brief set the active post processor for viewer
    *
    * @param _id viewer id
    * @param _active id of the post processor
    */
    void setActive(unsigned int _active, int _id);

    /** \brief set the active post processor for viewer
    *
    * @param _id viewer id
    * @param _active name of the post processor
    */
    void setActive(QString _active, int _id);

    /** \brief Get the current active post processor for viewer
    *
    * @param _id ViewerId
    * @return post processor
    */
    PostProcessorInfo* active( int _id);


    /** \brief Get the id of the active post processor for viewer
    *
    * @param _id ViewerId
    * @return post processor id
    */
    unsigned int activeId( int _id);

  private:
    /// Vector holding all available  post processors
    std::vector<PostProcessorInfo> availablePostProcessors_;

    /// The currently active post processor id
    std::vector<unsigned int> activePostProcessors_;
};

/// Get an instance of the Post Processor manager
DLLEXPORT
PostProcessorManager& postProcessorManager();

#endif //RENDERERINFO_HH
