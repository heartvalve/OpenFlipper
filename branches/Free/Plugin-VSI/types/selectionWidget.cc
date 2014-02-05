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
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>

#include "../parser/context.hh"
#include "selectionWidget.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::SelectionWidget - IMPLEMENTATION
//
//=============================================================================

/// Constructor
SelectionWidget::SelectionWidget(QMap< QString, QString > &_hints, QString _typeName, QWidget *_parent) :
  TypeWidget (_hints, _typeName, _parent),
  multiple_ (false),
  default_ (),
  combo_ (0)
{
  QHBoxLayout *hL = new QHBoxLayout;

  if (_hints.contains ("names"))
  {
    QStringList n = _hints["names"].split (',');
    QStringList d;

    if (_hints.contains ("descriptions"))
    {
      d = _hints["descriptions"].split (',');
      if (n.length () != d.length ())
        d = n;
    }

    for (int i = 0; i < n.length (); i++)
      names_.append (QPair<QString,QString> (n[i], d[i]));
  }
  else
    names_.append (QPair<QString,QString> ("none", "None"));

  if (_hints.contains ("multiple"))
    multiple_ = Context::strToBool (_hints["multiple"]);

  if (_hints.contains ("default"))
    default_ = _hints["default"].split (',');

  if (!multiple_)
  {
    combo_ = new QComboBox ();
    for (int i  = 0; i < names_.size (); i++)
      combo_->addItem (names_[i].second, names_[i].first);

    if (default_.length ())
      combo_->setCurrentIndex (combo_->findData (default_[0]));
    else
      combo_->setCurrentIndex (0);

    hL->addWidget (combo_);
  }
  else
  {
    QGridLayout *gL = new QGridLayout;
    for (int i = 0; i < names_.size (); i++)
    {
      QCheckBox *c = new QCheckBox (names_[i].second);
      checkBox_.append (c);
      gL->addWidget (c, i / 2, i & 1);

      if (default_.contains (names_[i].first))
        c->setChecked (true);
      else
        c->setChecked (false);
    }
    hL->addLayout (gL);
  }

  setLayout (hL);
}

//------------------------------------------------------------------------------

/// Destructor
SelectionWidget::~ SelectionWidget()
{
}

//------------------------------------------------------------------------------

/// Convert current value to string
QString SelectionWidget::toValue()
{
  QString rv;

  if (!multiple_)
  {
    rv = combo_->itemData (combo_->currentIndex ()).toString ();
  }
  else
  {
    QStringList sl;

    for (int i = 0; i < names_.size (); i++)
    {
      if (checkBox_[i]->isChecked ())
        sl << names_[i].first;
    }

    rv = sl.join (",");
  }

  return "\"" + rv + "\"";
}

//------------------------------------------------------------------------------

/// Read value from string
void SelectionWidget::fromValue(QString _from)
{
  if (_from.isEmpty ())
  {
    if (!multiple_)
      combo_->setCurrentIndex (0);
    else
      foreach (QCheckBox *c, checkBox_)
        c->setChecked (false);
    return;
  }

  _from.remove (0, 1);
  _from.remove (_from.length () - 1, 1);

  QStringList values = _from.split (',');

  if (!multiple_)
  {
    if (values.length ())
      combo_->setCurrentIndex (combo_->findData (values[0]));
    else
      combo_->setCurrentIndex (0);
  }
  else
    for (int i = 0; i < names_.size (); i++)
    {
      if (values.contains (names_[i].first))
        checkBox_[i]->setChecked (true);
      else
        checkBox_[i]->setChecked (false);
    }
}

//------------------------------------------------------------------------------

/// Reset to default
void SelectionWidget::toDefault()
{
  if (!multiple_)
  {
    if (default_.length ())
      combo_->setCurrentIndex (combo_->findData (default_[0]));
    else
      combo_->setCurrentIndex (0);
  }
  else
    for (int i = 0; i < names_.size (); i++)
    {
      if (default_.contains (names_[i].first))
        checkBox_[i]->setChecked (true);
      else
        checkBox_[i]->setChecked (false);
    }

}

//------------------------------------------------------------------------------
}
