//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




#ifndef VIEWMODEWIDGET_HH
#define VIEWMODEWIDGET_HH

#include "ui_viewMode.hh"
#include <QStringList>
#include <QVector>

struct ViewMode;

class viewModeWidget : public QDialog, public Ui::viewMode
{
  Q_OBJECT
  private:
    const QVector< ViewMode* >& modes_;
  public:
    viewModeWidget(const QVector< ViewMode* >& _modes, QWidget *parent = 0 );
    void show(QString _lastMode);
  signals:
    void saveMode(QString _name, bool _custom, QStringList _toolWidgets);
    void removeMode(QString _name);
    void changeView(QString _mode, QStringList _toolWidgets);
  private slots:
    void slotMoveUp();
    void slotMoveDown();
    void slotRemoveWidget();
    void slotRemoveMode();
    void slotChangeView();
    void slotModeChanged(QString _mode);
    void slotModeClicked(QModelIndex _id);
    void slotSaveMode();
    void slotSetToolWidgets();
    /// Context Menus
    void slotModeContextMenu ( const QPoint & _pos );
    void slotToolContextMenu ( const QPoint & _pos );
};

#endif //VIEWMODEWIDGET_HH
