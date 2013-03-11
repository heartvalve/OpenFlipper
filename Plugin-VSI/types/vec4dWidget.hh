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
*   $Revision: 13386 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2012-01-13 12:21:45 +0100 (Fr, 13 Jan 2012) $                     *
*                                                                            *
\*===========================================================================*/

#ifndef VSI_VEC4DWIDGET_HH
#define VSI_VEC4DWIDGET_HH

//== INCLUDES =================================================================
#include "../parser/typeWidget.hh"

//== FORWARDDECLARATIONS ======================================================
class QLineEdit;

//== NAMESPACES ===============================================================
namespace VSI {

//== CLASS DEFINITION =========================================================

/** Widget to configure number inputs
 */

class Vec4DWidget : public TypeWidget {
  Q_OBJECT

  public:
    /// Constructor
    Vec4DWidget (QMap <QString, QString> &_hints, QString _typeName, QWidget *_parent = NULL);

    /// Destructor
    ~Vec4DWidget ();

    /// Convert current value to string
    QString toValue ();

    /// Read value from string
    void fromValue (QString _from);

    /// Reset to default
    void toDefault ();

  private slots:
    void editingFinished ();

  private:

    float current_[4];
    float default_[4];

    QLineEdit *fields_[4];
};
//=============================================================================
}
//=============================================================================

#endif
