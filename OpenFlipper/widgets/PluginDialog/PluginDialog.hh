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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#include "ui_pluginDialog.hh"
#include <QtGui>

#include <OpenFlipper/Core/PluginInfo.hh>

class PluginDialog : public QDialog, public Ui::PluginDialog
{
  Q_OBJECT

  public:
    PluginDialog(std::vector<PluginInfo>& _plugins, QWidget *parent = 0);

    int exec();

  private:
    std::vector<PluginInfo>& plugins_;
    std::vector<QLabel*> descriptions_;
    QVector< QFrame* > frames_;

    static const QColor blockColor_;
    static const QColor unloadColor_;
    static const QColor loadedBuiltInColor_;
    static const QColor loadedExternalColor_;
    static const QColor failColor_;

  private slots:
    void slotContextMenu(const QPoint& _point);
    void slotLoadPlugin();
    void slotBlockPlugin();
    void slotUnBlockPlugin();

  protected:
    void closeEvent(QCloseEvent *event);

  signals:
    void blockPlugin(const QString &_name);
    void unBlockPlugin(const QString &_name);
    void loadPlugin();
    void loadPlugin(const QString& _filename, bool _silent, QString& _licenseErrors, QObject* _plugin);
};

