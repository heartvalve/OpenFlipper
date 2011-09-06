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


#ifndef RENDERINTERFACE_HH
#define RENDERINTERFACE_HH

#include <ACG/Scenegraph/DrawModes.hh>

/** \file RenderInterface.hh
*
* Interface for adding global rendering functions. \ref RenderInterfacePage
*/

/** \brief Interface to add additional rendering functions from within plugins.
 *
 * \ref RenderInterfacePage "Detailed description"
 * \n
 * \n
 *
 * Interface for adding and controlling rendering functions from a plugin.\n
*/
class RenderInterface {

  public:

    /// Destructor
    virtual ~RenderInterface() {};

  private slots:

    /** \brief rendering function
     *
     * Implement this function to render the scene. You have to traverse the
     * scenegraph in this function. The glContext is made current before this
     * call.
     * If you need to initialize something,you can do it within this function.
     * gl and glew are already setUp here.
     */
    virtual void render(/* Unknow options yet.. to be evaluated*/) {};


    /** \brief announce required draw modes
     *
     * This function is called by the core to get a list of draw modes that are supported
     * by this renderer. If such a draw mode is set and the currently active
     * renderer does not support the given mode, the core will switch to
     * this renderer automatically. If there are multiple renderers capable of rendering
     * the drawmode, the core will ask the user for its preference.
     *
     * @param _mode Combined list of drawmodes
     */
    virtual void supportedDrawModes(ACG::SceneGraph::DrawModes::DrawMode& _mode) {};

    /** \brief announce name for the rendering function
     *
     *
     * @param _name displayed name of the rendering function
     */
    virtual QString rendererName() = 0;

  signals:

    /** \brief register a new rendering function to the core
     *
     * Emit this signal to register your plugin with a given drawmode.
     * Whenever this drawmode is active OpenFlipper will not call its internal
     * rendering but your plugins render function.
     *
     * @param _mode The drawmode for the new rendering function
     */
   virtual void registerRenderingFunction(ACG::SceneGraph::DrawModes::DrawMode _mode) {};

};

/** \page RenderInterfacePage Render Interface
\image html RenderInterface.png
\n

The RenderInterface can be used by plugins to add special rendering functions to OpenFlipper.
You can create a dedicated rendering function for a drawmode. E.g. when you need to render 4
passes with different shaders you can do that in your rendering function.


Example Code for functions:
\code
 TODO
\endcode


To use the RenderInterface:
<ul>
<li> include RenderInterface.hh in your plugins header file
<li> derive your plugin from the class RenderInterface
<li> add Q_INTERFACES(RenderInterface) to your plugin class
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them)
</ul>

*/


Q_DECLARE_INTERFACE(RenderInterface,"OpenFlipper.RenderInterface/1.0")

#endif // RPCINTERFACE_HH
