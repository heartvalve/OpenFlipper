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

#ifndef VSI_CONFIGDIALOG_HH
#define VSI_CONFIGDIALOG_HH

//== INCLUDES =================================================================

#include <QDialog>
#include <QVector>

//== FORWARDDECLARATIONS ======================================================
class QLineEdit;

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================

class ElementInput;
class ConfigValue;

//== CLASS DEFINITION =========================================================

/** Dialog for input configuration
  */
class ConfigDialog : public QDialog {
  Q_OBJECT

  public:
    /// Constructor
    ConfigDialog (QVector<ElementInput *> _inputs, QString _name = QString (), QWidget *_parent = 0);

    /// Destructor
    ~ConfigDialog ();

    /// return entered name
    QString name ();

  public slots:
    // OK pressed
    void ok ();

  private:
    QVector<ElementInput *> inputs_;
    QVector<ConfigValue *> values_;

    QLineEdit *name_;
};
  
//=============================================================================
}
//=============================================================================

#endif

