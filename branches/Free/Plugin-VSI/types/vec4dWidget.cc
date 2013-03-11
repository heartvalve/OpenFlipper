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
*   $Revision: 10745 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-01-26 10:23:50 +0100 (Mi, 26. Jan 2011) $                     *
*                                                                            *
\*===========================================================================*/

//== INCLUDES =================================================================
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>

#include "vec4dWidget.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::Vec3DWidget - IMPLEMENTATION
//
//=============================================================================

/// Constructor
Vec4DWidget::Vec4DWidget(QMap< QString, QString > &_hints, QString _typeName, QWidget *_parent) :
  TypeWidget (_hints, _typeName, _parent)
{
  bool ok;

  QHBoxLayout *hL = new QHBoxLayout;

  for (int i = 0; i < 4; i++)
    default_[i] = 0.0;

  ok = true;
  if (_hints.contains ("default"))
  {
    QStringList sl = _hints["default"].split (',');

    if (sl.length () == 4)
    {
      for (int i = 0; i < 4 && ok; i++)
        default_[i] = sl[i].toFloat (&ok);

      if (!ok)
        for (int i = 0; i < 4; i++)
          default_[i] = 0.0;
    }
  }

  for (int i = 0; i < 4; i++)
    fields_[i] = new QLineEdit;

  hL->addWidget (new QLabel ("("));
  hL->addWidget (fields_[0]);
  hL->addWidget (new QLabel (","));
  hL->addWidget (fields_[1]);
  hL->addWidget (new QLabel (","));
  hL->addWidget (fields_[2]);
  hL->addWidget (new QLabel (","));
  hL->addWidget (fields_[3]);
  hL->addWidget (new QLabel (")"));

  for (int i = 0; i < 4; i++)
  {
    fields_[i]->setText (QString::number (default_[i]));
    connect (fields_[i], SIGNAL (editingFinished ()), this, SLOT (editingFinished ()));
  }

  setLayout (hL);

  for (int i = 0; i < 4; i++)
    current_[i] = default_[i];
}

/// Destructor
Vec4DWidget::~ Vec4DWidget()
{
}

//------------------------------------------------------------------------------

/// Convert current value to string
QString Vec4DWidget::toValue()
{
  QString rv = "Vector4 (";
  rv += QString::number (current_[0]) + ",";
  rv += QString::number (current_[1]) + ",";
  rv += QString::number (current_[2]) + ",";
  rv += QString::number (current_[3]) + ")";
  return rv;
}

//------------------------------------------------------------------------------

/// Read value from string
void Vec4DWidget::fromValue(QString _from)
{
  if (_from.startsWith ("Vector4 ("))
    _from.remove (0, 8);
  if (_from.endsWith (")"))
    _from.remove (_from.length () - 1, 1);
  
  QStringList sl = _from.split (',');

  float v[4];
  bool ok = true;

  if (sl.length () == 4)
  {
    for (int i = 0; i < 4 && ok; i++)
      v[i] = sl[i].toFloat (&ok);

    if (ok)
      for (int i = 0; i < 4; i++)
      {
        current_[i] = v[i];
        fields_[i]->setText (QString::number (current_[i]));
      }
  }
}

//------------------------------------------------------------------------------

// handle slider changes
void Vec4DWidget::editingFinished ()
{
  bool ok;
  float v;

  for (int i = 0; i < 4; i++)
  {
    v = fields_[i]->text ().toFloat (&ok);

    if (ok)
      current_[i] = v;
    else
      fields_[i]->setText (QString::number (current_[i]));
  }
}



//------------------------------------------------------------------------------

/// Reset to default
void Vec4DWidget::toDefault()
{
  for (int i = 0; i < 4; i++)
  {
    current_[i] = default_[i];
    fields_[i]->setText (QString::number (current_[i]));
  }
}

//------------------------------------------------------------------------------
}

