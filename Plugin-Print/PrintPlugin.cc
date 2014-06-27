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


#include <QPrintDialog>
#include <QPrinter>

#include "PrintPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#if QT_VERSION >= 0x050000 
#include <QtWidgets>
#else
#include <QtGui>
#endif

void PrintPlugin::pluginsInitialized()
{
  // Print menu
  QMenu *printMenu = new QMenu(tr("&Printing"));

  printMenu->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-print.png"));

  emit addMenubarAction(printMenu->menuAction(), FILEMENU );

  QAction* AC_Print = new QAction(tr("&Print"), this);;
  AC_Print->setStatusTip(tr("Print the current view"));
  AC_Print->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-print.png"));

  connect(AC_Print, SIGNAL(triggered()), this, SLOT(printView()));
  printMenu->addAction(AC_Print);
}

void PrintPlugin::printView () {


  QPrinter printer(QPrinter::HighResolution);

  QPrintDialog *dialog = new QPrintDialog(&printer);
  dialog->setWindowTitle(tr("Print Current View"));
  if (dialog->exec() != QDialog::Accepted)
      return;

  QImage image;

  // create the snapshot
  PluginFunctions::getCurrentViewImage(image);

  QPainter painter;
  painter.begin(&printer);

  painter.save();

  QRect rect = painter.viewport();
  QSize size = image.size();
  size.scale(rect.size(), Qt::KeepAspectRatio);
  painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
  painter.setWindow(image.rect());

  painter.drawImage(0, 0, image);

  painter.restore();
  painter.end();

}

void PrintPlugin::slotKeyEvent( QKeyEvent* _event ) {
  if (_event->modifiers() == Qt::ControlModifier ) {
    switch (_event->key())
    {
      case Qt::Key_P :
              printView();
              return;
    }
  }
}


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( printplugin , PrintPlugin );
#endif

