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

#ifndef VSI_NUMWIDGET_HH
#define VSI_NUMWIDGET_HH

//== INCLUDES =================================================================
#include "parser/typeWidget.hh"

//== FORWARDDECLARATIONS ======================================================
class QSlider;
class QDoubleSpinBox;
class QPushButton;

//== NAMESPACES ===============================================================
namespace VSI {

//== CLASS DEFINITION =========================================================

/** Widget to configure number inputs
 */

class NumWidget : public TypeWidget {
  Q_OBJECT

  public:
    /// Constructor
    NumWidget (QMap <QString, QString> &_hints, QString _typeName, QWidget *_parent = NULL);

    /// Destructor
    ~NumWidget ();

    /// Convert current value to string
    QString toValue ();

    /// Read value from string
    void fromValue (QString _from);

    /// Reset to default
    void toDefault ();

  private slots:

    // handle slider changes
    void sliderValueChanged (int _value);
    // handle spinbox changes
    void spinValueChanged (double _value);

  private:
    double precision_;
    bool isFloat_;

    int numDecimals_;

    double sliderMul_;
    double default_;

    QSlider *slider_;
    QDoubleSpinBox *spin_;
};
//=============================================================================
}
//=============================================================================

#endif
