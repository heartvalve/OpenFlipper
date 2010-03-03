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

#ifndef VSI_DYNAMICVALUE_HH
#define VSI_DYNAMICVALUE_HH

//== INCLUDES =================================================================
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>

#include "parser/input.hh"
#include "parser/typeWidget.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================

class ElementInput;

//== CLASS DEFINITION =========================================================

/** Class that holds everything to configure an input in the DynamicDialog
  */

class DynamicValue : public QObject {
  Q_OBJECT

  public:

    /// Constructor
    DynamicValue (Input *_input);

    /// Destructor
    ~DynamicValue ();

    friend class DynamicDialog;

  private slots:

    // reset to default;
    void toDefault ();

  private:
    Input *input_;

    QGroupBox *group_;
    TypeWidget *main_;
    QPushButton *default_;
};

//=============================================================================
}
//=============================================================================

#endif