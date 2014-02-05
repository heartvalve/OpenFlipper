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
#include <QPushButton>
#include <QComboBox>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include "objectIdWidget.hh"
#include "objectPickDialog.hh"
#include "TreeModel.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::ObjectIdWidget - IMPLEMENTATION
//
//=============================================================================

/// Constructor
ObjectIdWidget::ObjectIdWidget(QMap< QString, QString > &_hints, QString _typeName, QWidget *_parent) :
  TypeWidget (_hints, _typeName, _parent),
  pickButton_ (0),
  combo_ (0)
{
  int n_ok = 0;

  QHBoxLayout *hL = new QHBoxLayout;

  combo_ = new QComboBox ();
  pickButton_ = new QPushButton (tr("Pick Object"));

  hL->addWidget (combo_);
  hL->setStretchFactor (combo_, 1);
  hL->addWidget (pickButton_);

  if (_hints.contains ("flags"))
    flags_ = _hints["flags"].split(',');

  if (_hints.contains ("types"))
    types_ = _hints["types"].split(',');

  if (flags_.contains("all", Qt::CaseInsensitive))
    flags_.clear ();

  withGroups_ = types_.contains("Group");

  if (types_.contains("All"))
    types_.clear ();

  setLayout (hL);

  connect (pickButton_, SIGNAL (pressed()), this, SLOT (showPickDialog ()));

  for (PluginFunctions::BaseObjectIterator o_it; o_it != PluginFunctions::baseObjectsEnd(); ++o_it)  {

    bool ok = true;

    if (!flags_.empty ())
    {
      bool found = false;
      foreach (QString flag, flags_)
        if (o_it->flag (flag))
        {
          found = true;
          break;
        }

      if (!found)
        ok = false;
    }

    if (!types_.empty ())
    {
      if (!types_.contains (typeName (o_it->dataType())))
        ok = false;
    }

    if (o_it->isGroup() && !withGroups_)
      continue;

    if (ok)
    {
      combo_->addItem (o_it->name() + " (" + QString::number (o_it->id ()) + ")", QVariant (o_it->id()));
      n_ok++;
    }
  }

  if (n_ok < 2)
  {
    pickButton_->setEnabled (false);
    combo_->setEnabled (false);
  }
}

/// Destructor
ObjectIdWidget::~ ObjectIdWidget()
{
  for (PluginFunctions::BaseObjectIterator o_it; o_it != PluginFunctions::baseObjectsEnd(); ++o_it)  {
    o_it->setFlag("vsi_objectId_selected", false);
  }
}

//------------------------------------------------------------------------------

/// Convert current value to string
QString ObjectIdWidget::toValue()
{
  if (combo_->count ())
  {
    return QString::number (combo_->itemData (combo_->currentIndex()).toInt ());
  }
  else
    return 0;
}

//------------------------------------------------------------------------------

/// Read value from string
void ObjectIdWidget::fromValue(QString _from)
{
  combo_->setCurrentIndex (combo_->findData (QVariant (_from.toInt ())));
}

//------------------------------------------------------------------------------

/// Reset to default
void ObjectIdWidget::toDefault()
{
  combo_->setCurrentIndex (0);
}

//------------------------------------------------------------------------------

void ObjectIdWidget::showPickDialog()
{
  ObjectPickDialog d(flags_, types_, withGroups_);
  d.selectedId (combo_->itemData (combo_->currentIndex()).toInt ());

  if (d.exec () == QDialog::Accepted)
    combo_->setCurrentIndex (combo_->findData (QVariant (d.selectedId())));
}

//------------------------------------------------------------------------------
}


