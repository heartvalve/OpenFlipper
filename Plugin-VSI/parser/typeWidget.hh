//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2009 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: $
//   $Author: $
//   $Date: $
//
//=============================================================================

#ifndef VSI_TYPEWIDGET_HH
#define VSI_TYPEWIDGET_HH

//== INCLUDES =================================================================
#include <QWidget>
#include <QMap>
#include <QString>

//== NAMESPACES ===============================================================
namespace VSI {

//== CLASS DEFINITION =========================================================

/** Abstract base class for a type configuration widget
 */
class TypeWidget : public QWidget {
  public:

    /// Constructor
    TypeWidget (QMap <QString, QString> &_hints, QString _typeName, QWidget* _parent = NULL);

    /// Destructor
    virtual ~TypeWidget ();

    /// Return the type configuration result to a string
    virtual QString toValue () = 0;

    /// Set configuration to the value of the given string
    virtual void fromValue (QString _from) = 0;

    /// reset the widget to default
    virtual void toDefault () = 0;

};

//=============================================================================
}
//=============================================================================

#endif