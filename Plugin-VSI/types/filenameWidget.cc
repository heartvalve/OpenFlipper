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

//== INCLUDES =================================================================
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

#include "filenameWidget.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::FilenameWidget - IMPLEMENTATION
//
//=============================================================================

/// Constructor
FilenameWidget::FilenameWidget(QMap< QString, QString > &_hints, QString _typeName, QWidget *_parent) :
  TypeWidget (_hints, _typeName, _parent),
  default_ (""),
  line_ (0),
  selectButton_ (0),
  mode_ (QFileDialog::AcceptOpen),
  filter_ ("Any File (*)"),
  defaultSuffix_ (""),
  dirOnly_ (false)
{
  QHBoxLayout *hL = new QHBoxLayout;

  line_ = new QLineEdit;

  if (_hints.contains ("default"))
    default_ = _hints["default"];

  line_->setText (default_);
  
  hL->addWidget (line_);

  selectButton_ = new QPushButton ("Select");

  connect (selectButton_, SIGNAL (pressed ()), this, SLOT (showDialog ()));
  hL->addWidget (selectButton_);

  setLayout (hL);

  if (_typeName == "Directory")
    dirOnly_ = true;

  if (_hints.contains ("mode") && _hints["mode"] == "Save")
    mode_ = QFileDialog::AcceptSave;

  if (_hints.contains ("filter"))
    filter_ = _hints["filter"];

  if (_hints.contains ("default_suffix"))
    defaultSuffix_ = _hints["default_suffix"];

}

//------------------------------------------------------------------------------

/// Destructor
FilenameWidget::~ FilenameWidget()
{
}

//------------------------------------------------------------------------------

/// Convert current value to string
QString FilenameWidget::toValue()
{
  return "\"" + line_->text () + "\"";
}

//------------------------------------------------------------------------------

/// Read value from string
void FilenameWidget::fromValue(QString _from)
{
  if (_from.isEmpty ())
  {
    line_->clear ();
    return;
  }

  _from.remove (0, 1);
  _from.remove (_from.length () - 1, 1);

  line_->setText (_from);
}

//------------------------------------------------------------------------------

/// Reset to default
void FilenameWidget::toDefault()
{
  line_->setText (default_);
}

//------------------------------------------------------------------------------

// Show file dialog
void FilenameWidget::showDialog()
{

  QFileDialog d (this, tr("Select File"), QString() , filter_);
  d.setAcceptMode (mode_);
  d.setDefaultSuffix (defaultSuffix_);
  if (dirOnly_)
  {
    d.setFileMode (QFileDialog::Directory);
    d.setOption (QFileDialog::ShowDirsOnly, true);
  }

  if (QDialog::Accepted == d.exec ())
    line_->setText (d.selectedFiles ()[0]);
}

//------------------------------------------------------------------------------
}
