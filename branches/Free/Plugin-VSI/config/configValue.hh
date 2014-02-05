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

#ifndef VSI_CONFIGVALUE_HH
#define VSI_CONFIGVALUE_HH

//== INCLUDES =================================================================
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>

#include "../parser/typeWidget.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================

class ElementInput;

//== CLASS DEFINITION =========================================================

/** Class that holds everything to configure an input in the ConfigDialog
  */

class ConfigValue : public QObject {
  Q_OBJECT

  public:

    /// Constructor
    ConfigValue (ElementInput *_input);

    /// Destructor
    ~ConfigValue ();

    friend class ConfigDialog;

  private slots:
    // reset to default
    void toDefault ();

    // type selection changed enable/disable configuration subwidget
    void selectionChange ();

  private:
    ElementInput *input_;

    QGroupBox *group_;
    TypeWidget *main_;
    QPushButton *default_;

    QRadioButton *constant_;
    QRadioButton *optional_;
    QRadioButton *forceAskUser_;

    QButtonGroup *buttonGroup_;
};

//=============================================================================
}
//=============================================================================

#endif
