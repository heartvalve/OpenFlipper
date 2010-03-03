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

#ifndef VSI_INOUT_HH
#define VSI_INOUT_HH

//== INCLUDES =================================================================
#include <QString>
#include <QVariant>
#include <QMap>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class Element;

//== CLASS DEFINITION =========================================================

/** Base class for element inputs and outputs. It holds all common data.
  */
class InOut {

  public:

    /// Constructor
    InOut (Element *_e);

    /// Name
    const QString &name () { return name_; };

    /// Short description
    const QString &shortDescription () { return shortDesc_; };

    /// Long description
    const QString &longDescription () { return longDesc_; };

    /// Element of this input/output
    const Element * element () { return element_; };

    /// Parsed hints for this input/output type
    QMap<QString, QString> hints () const { return hints_; };

    /// Type
    QString typeString ();

    friend class Context;

  private:
    QString name_;
    QString shortDesc_;
    QString longDesc_;

    QString type_;

    QMap<QString, QString> hints_;

    Element *element_;
};

//=============================================================================
}
//=============================================================================

#endif
