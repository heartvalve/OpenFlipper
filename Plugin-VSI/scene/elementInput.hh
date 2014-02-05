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

#ifndef VSI_ELEMENTINPUT_HH
#define VSI_ELEMENTINPUT_HH

//== INCLUDES =================================================================
#include <QString>

#include <QDomDocument>
#include <QDomElement>
#include <QXmlQuery>

#include "elementInOut.hh"
#include "../parser/input.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//== CLASS DEFINITION =========================================================

/** Class for inputs of a scene element.
  * It handles all the different ways of input (connection to an output,
  * user configured during script creation, asking user during script execution,
  * optional)
  */
class ElementInput : public ElementInOut {

  public:

    /// Constructor
    ElementInput (Input *_in, SceneElement *_parent);

    /// Destructor
    ~ElementInput ();

    /// Type (=Input)
    Type type () const { return TypeInput; };

    /// Sets the valid flag (needed during code generation)
    void setValid (bool _value) { valid_ = _value; };

    /// Returns state of valid flag (needed during code generation)
    bool valid ();

    /// Set to a user value
    void setValue (QString _value);

    /// Returns value set by user
    QString value () const { return value_; };

    /// Add connection
    void addConnection (Connection *_conn);

    /// Remove connection
    void removeConnection (Connection *_conn);

    /// "Set" flag to mark input as set by user
    void set (bool _set);

    /// Return "set" flag
    bool isSet ();

    /// "ForceAsk" flag to mark an optional input for asking during script execution
    void setForceAsk (bool _set) { forceAsk_ = _set; };

    /// Return "ForceAsk" flag
    bool isForceAskSet () const { return forceAsk_; };

    /// VSI::Input state passthrough
    unsigned int state ();

    /// Save to xml
    void saveToXml (QDomDocument &_doc, QDomElement &_root);

    /// Load from xml
    void loadFromXml (QXmlQuery &_xml);

  private:
    Input *in_;


    bool valid_;

    bool isSet_;
    QString value_;

    bool forceAsk_;
};

//=============================================================================
}
//=============================================================================

#endif
