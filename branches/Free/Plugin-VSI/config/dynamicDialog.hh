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

#ifndef VSI_DYNAMICDIALOG_HH
#define VSI_DYNAMICDIALOG_HH

//== INCLUDES =================================================================
#include <QDialog>
#include <QVector>
#include <QMap>
#include <QString>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================

class Input;
class DynamicValue;

//== CLASS DEFINITION =========================================================

/** Dialog for input configuration during script execution
  */

class DynamicDialog : public QDialog {
  Q_OBJECT

  public:

    /// Constructor
    DynamicDialog (QVector<Input *> _inputs, QWidget *_parent = 0);

    /// Destructor
    ~DynamicDialog ();

    /// Results
    QMap<QString, QString> getResults ();

  private:
    QVector<Input *> inputs_;
    QVector<DynamicValue *> values_;
};
  
//=============================================================================
}
//=============================================================================

#endif