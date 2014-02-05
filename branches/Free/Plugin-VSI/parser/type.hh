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

#ifndef VSI_TYPE_HH
#define VSI_TYPE_HH

//== INCLUDES =================================================================
#include <QStringList>
#include <QMap>
#include <QWidget>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class TypeWidget;

//== CLASS DEFINITION =========================================================

/** Class representing a supported type in the visual scripting interface
  * A Type provides information about supported conversion to other types and
  * an optional configuration widget. This widget allows the user to directly
  * edit an input of this type.
  */
class Type {
  public:

    /// Constructor
    Type ();

    /// Destructor
    virtual ~Type ();

    /// Names of Types
    virtual QStringList supportedTypes ();

    /// Hints that may be stored for this type in the xml metadata
    virtual QStringList supportedHints ();

    /// Can this type be converted to the given type?
    virtual bool canConvertTo (QString _type);

    /// Does this type has a configuration widget
    virtual bool hasWidget () { return false; };

    /// Returns the configuration widget
    virtual TypeWidget *widget (QMap<QString, QString> /*_hints*/, QString /*_typeName*/, QWidget * /*_parent*/ = NULL) { return 0; };

  protected:
    QStringList types_;
    QStringList hints_;

    QStringList convertTypes_;  
};

//=============================================================================
}
//=============================================================================

#endif