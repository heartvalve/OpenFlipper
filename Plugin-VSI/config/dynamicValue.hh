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

#ifndef VSI_DYNAMICVALUE_HH
#define VSI_DYNAMICVALUE_HH

//== INCLUDES =================================================================
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>

#include "../parser/input.hh"
#include "../parser/typeWidget.hh"

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
