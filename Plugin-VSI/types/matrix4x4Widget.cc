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
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>

#include "matrix4x4Widget.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::Matrix4x4Widget - IMPLEMENTATION
//
//=============================================================================

/// Constructor
Matrix4x4Widget::Matrix4x4Widget(QMap< QString, QString > &_hints, QString _typeName, QWidget *_parent) :
  TypeWidget (_hints, _typeName, _parent)
{
  bool ok;

  QGridLayout *gL = new QGridLayout;

  for (int i = 0; i < 16; i++)
  {
    default_[i] = 0.0;
    if (i%5 == 0) //set diagonal to 1, convenient for e.g. transformation matrices
      default_[i]  = 1.0;
  }

  ok = true;
  if (_hints.contains ("default"))
  {
    QStringList sl = _hints["default"].split (',');

    if (sl.length () == 16)
    {
      for (int i = 0; i < 16 && ok; i++)
        default_[i] = sl[i].toFloat (&ok);

      if (!ok)
        for (int i = 0; i < 16; i++)
          default_[i] = 0.0;
    }
  }

  for (int i = 0; i < 16; i++)
    fields_[i] = new QLineEdit;

  for (int i = 0; i < 4; i++)
  {
    gL->addWidget (new QLabel ("("),i, 0);
    gL->addWidget (fields_[i], i, 1);
    gL->addWidget (new QLabel (","), i, 2);
    gL->addWidget (fields_[i + 4], i, 3);
    gL->addWidget (new QLabel (","), i, 4);
    gL->addWidget (fields_[i + 8], i, 5);
    gL->addWidget (new QLabel (","), i, 6);
    gL->addWidget (fields_[i + 12], i, 7);
    gL->addWidget (new QLabel (")"), i, 8);
  }

  for (int i = 0; i < 16; i++)
  {
    fields_[i]->setText (QString::number (default_[i]));
    connect (fields_[i], SIGNAL (editingFinished ()), this, SLOT (editingFinished ()));
  }

  setLayout (gL);

  for (int i = 0; i < 16; i++)
    current_[i] = default_[i];
}

/// Destructor
Matrix4x4Widget::~ Matrix4x4Widget()
{
}

//------------------------------------------------------------------------------

/// Convert current value to string
QString Matrix4x4Widget::toValue()
{
  QString rv = "Matrix4x4 (";
  for (int i = 0; i < 16; i++)
    rv += QString::number (current_[i]) + ",";

  rv.remove (rv.length () - 1, 1);
  rv += ")";
  return rv;
}

//------------------------------------------------------------------------------

/// Read value from string
void Matrix4x4Widget::fromValue(QString _from)
{
  if (_from.startsWith ("Matrix4x4 ("))
    _from.remove (0, 11);
  if (_from.endsWith (")"))
    _from.remove (_from.length () - 1, 1);
  
  QStringList sl = _from.split (',');

  float v[16];
  bool ok = true;

  if (sl.length () == 16)
  {
    for (int i = 0; i < 16 && ok; i++)
      v[i] = sl[i].toFloat (&ok);

    if (ok)
      for (int i = 0; i < 16; i++)
      {
        current_[i] = v[i];
        fields_[i]->setText (QString::number (current_[i]));
      }
  }
}

//------------------------------------------------------------------------------

// handle slider changes
void Matrix4x4Widget::editingFinished ()
{
  bool ok;

  for (int i = 0; i < 16; i++)
  {
    const float v = fields_[i]->text ().toFloat (&ok);

    if (ok)
      current_[i] = v;
    else
      fields_[i]->setText (QString::number (current_[i]));
  }
}



//------------------------------------------------------------------------------

/// Reset to default
void Matrix4x4Widget::toDefault()
{
  for (int i = 0; i < 16; i++)
  {
    current_[i] = default_[i];
    fields_[i]->setText (QString::number (current_[i]));
  }
}

//------------------------------------------------------------------------------
}

