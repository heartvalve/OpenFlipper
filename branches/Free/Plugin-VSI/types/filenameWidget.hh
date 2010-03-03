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

#ifndef VSI_FILENAMEWIDGET_HH
#define VSI_FILENAMEWIDGET_HH

//== INCLUDES =================================================================
#include <QString>
#include <QFileDialog>

#include "parser/typeWidget.hh"

class QLineEdit;
class QPushButton;

//== NAMESPACES ===============================================================
namespace VSI {

//== CLASS DEFINITION =========================================================

/** Widget to configure filename inputs
 */

class FilenameWidget : public TypeWidget {
  Q_OBJECT

  public:

    /// Constructor
    FilenameWidget (QMap <QString, QString> &_hints, QString _typeName, QWidget *_parent = NULL);

    /// Destructor
    ~FilenameWidget ();

    /// Convert current value to string
    QString toValue ();

    /// Read value from string
    void fromValue (QString _from);

    /// Reset to default
    void toDefault ();

  private slots:

    // Show file dialog
    void showDialog ();

  private:
    QString default_;

    QLineEdit *line_;
    QPushButton *selectButton_;

    QFileDialog::AcceptMode mode_;
    QString filter_;
    QString defaultSuffix_;
    bool dirOnly_;
};
//=============================================================================
}
//=============================================================================

#endif
