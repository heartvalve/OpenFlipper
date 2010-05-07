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

//== INCLUDES =================================================================
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include "objectSelectionWidget.hh"
#include "objectPickDialog.hh"
#include "TreeModelObjectSelection.hh"

//== NAMESPACES ===============================================================

//=============================================================================
//
//  CLASS ObjectSelectionWidget - IMPLEMENTATION
//
//=============================================================================

/// Constructor
ObjectSelectionWidget::ObjectSelectionWidget(QMap< QString, QString > &_hints, QString _typeName, QWidget *_parent) :
  QWidget(0),
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
ObjectSelectionWidget::~ ObjectSelectionWidget()
{
  for (PluginFunctions::BaseObjectIterator o_it; o_it != PluginFunctions::baseObjectsEnd(); ++o_it)  {
    o_it->setFlag("vsi_objectId_selected", false);
  }
}

//------------------------------------------------------------------------------

void ObjectSelectionWidget::showPickDialog()
{
  ObjectPickDialog d(flags_, types_, withGroups_);
  d.selectedId (combo_->itemData (combo_->currentIndex()).toInt ());

  if (d.exec () == QDialog::Accepted)
    combo_->setCurrentIndex (combo_->findData (QVariant (d.selectedId())));
}

//------------------------------------------------------------------------------


