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


#ifndef POSTPROCESSORINTERFACE_HH
#define POSTPROCESSORINTERFACE_HH

#include <ACG/GL/GLState.hh>
#include <QAction>

/** \file PostProcessorInterface.hh
*
* Interface for adding global image post processor functions. \ref postProcessorInterfacePage
*/

/** \brief Interface to add global image post processor functions from within plugins.
 *
 * \ref postProcessorInterfacePage "Detailed description"
 * \n
 * \n
 *
 * Interface for adding and controlling global image post processor functions from a plugin.\n
*/
class PostProcessorInterface {

  public:

    /// Destructor
    virtual ~PostProcessorInterface() {};
 
  public slots:

    /** \brief post processor function
     *
     */
    virtual void postProcess(ACG::GLState* _glState) {};

    /** \brief announce name for the postProcessor function
     *
     * @return The name of the post processor
     */
    virtual QString postProcessorName() = 0;

    /** \brief Return options menu
     *
     * If you want an options Menu or menu entry, you can return your action here.
     * It will be shown on top of the post processors list in the options menu.
     *
     *
     * @return Action for a menu or menu entry
     */
    virtual QAction* optionsAction() { return 0; };

};

/** \page postProcessorInterfacePage Post Processor Interface
\image html PostProcessorInterface.png
\n

The PostProcessor Interface can be used to add additional image post processing functions
to OpenFlipper. For example you get the rendered image in the usual buffers. Afterwards you can
add a post processor doing for example edge detection or taking the depth image from the
buffer and render it to the main image.


Example Code for functions:
\code
 TODO
\endcode


To use the PostProcessorInterface:
<ul>
<li> include PostProcessorInterface.hh in your plugins header file
<li> derive your plugin from the class PostProcessorInterface
<li> add Q_INTERFACES(PostProcessorInterface) to your plugin class
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them)
</ul>

*/


Q_DECLARE_INTERFACE(PostProcessorInterface,"OpenFlipper.PostProcessorInterface/1.0")

#endif // RPCINTERFACE_HH
