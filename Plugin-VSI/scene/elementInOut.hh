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
#ifndef VSI_ELEMENTINOUT_HH
#define VSI_ELEMENTINOUT_HH

//== INCLUDES =================================================================

#include <QList>

//== FORWARDDECLARATIONS ======================================================
class QGraphicsItem;

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class Text;
class ConnectionPoint;
class Connection;
class InOut;
class GraphicsScene;
class SceneElement;

//== CLASS DEFINITION =========================================================

/** Base class for inputs and outputs of scene elements.
  * It provides the visible widgets and handles connections
  */
class ElementInOut {

  public:

    /// Type
    enum Type {
      TypeInput,
      TypeOutput
    };

    /// Constructor
    ElementInOut (InOut *_io, SceneElement *_parent);

    /// Destructor
    virtual ~ElementInOut ();

    /// Type
    virtual Type type () const = 0;

    /// Connection point widget
    ConnectionPoint *connectionPointItem () const { return conn_; }

    /// Type text widget
    Text *typeTextItem () const { return typeText_; }

    /// Short description widget
    Text *descriptionTextItem () const { return descText_; }

    /// Connections
    QList<Connection *> connections () const { return connections_; };

    /// Add the connection
    virtual void addConnection (Connection *_conn);

    /// Remove the Connection
    virtual void removeConnection (Connection *_conn);

    /// Can this input/output be connected to _e
    bool validConnection (ElementInOut *_e);

    /// InOut context object
    InOut *inOut () const { return io_; };

    /// Scene element
    SceneElement *element () { return element_; };

  private:
    InOut *io_;
    SceneElement *element_;

    ConnectionPoint *conn_;
    Text            *typeText_;
    Text            *descText_;

    QList<Connection *> connections_;
};

//=============================================================================
}
//=============================================================================

#endif