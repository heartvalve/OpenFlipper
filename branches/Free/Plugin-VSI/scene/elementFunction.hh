/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
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

#ifndef VSI_ELEMENTFUNCTION_HH
#define VSI_ELEMENTFUNCTION_HH

//== INCLUDES =================================================================
#include <QString>

#include <QDomDocument>
#include <QDomElement>
#include <QXmlQuery>


//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class Function;
class SceneElement;
class GraphicsScene;

/** Class that represents a function of an element.
  * It holds the VSI::GraphicsScene to edit the function
  */
class ElementFunction : public QObject{

  Q_OBJECT

  public:

    /// Constructor
    ElementFunction (Function *_f, SceneElement *_element);

    /// Destructor
    ~ElementFunction ();

    /// The scene element of this function element
    SceneElement *element () { return element_; };

    /// Function class
    Function *function () { return function_; };

    /// Scene
    GraphicsScene *scene () { return scene_; };

    /// Save to xml
    void saveToXml (QDomDocument &_doc, QDomElement &_root);

    /// Load from xml
    void loadFromXml (QXmlQuery &_xml);

  public slots:

    // Initates editing of this function
    void editFunction ();

  private:

    SceneElement *element_;
    Function *function_;

    GraphicsScene *scene_;
};

}

#endif
