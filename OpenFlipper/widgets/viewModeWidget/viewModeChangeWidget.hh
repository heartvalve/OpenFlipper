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




#ifndef VIEWMODECHANGEWIDGET_HH
#define VIEWMODECHANGEWIDGET_HH

#include "ui_viewModeChange.hh"
#include <QStringList>
#include <QVector>

struct ViewMode;

class viewModeChangeWidget : public QDialog, public Ui::ViewModeChangeDialog
{
  Q_OBJECT
  private:
    // Vector holding list of all available modes
    const QVector< ViewMode* >& modes_;
  public:
    viewModeChangeWidget(const QVector< ViewMode* >& _modes, QWidget *parent = 0 );
    
    void show(QString _lastMode);
    
  private slots:

    void slotModeClicked(QModelIndex _id);
    
    
  signals:
    /// Changes the view mode to the currently configured one
    void changeView(QString _mode, QStringList _toolboxWidgets, QStringList _toolbars, QStringList _contextmenus);    
   
};

#endif //VIEWMODECHANGEWIDGET_HH
