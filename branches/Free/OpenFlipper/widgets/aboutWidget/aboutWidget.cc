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



#include "aboutWidget.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

#include <iostream>

AboutWidget::AboutWidget(QWidget *parent )
    : QMainWindow(parent)
{
  setupUi(this);

  QIcon icon;
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"window-close.png");
  actionClose->setIcon(icon);

  setWindowTitle(tr("About %1").arg(TOSTRING(PRODUCT_NAME)));
  const int idx = About->indexOf(tab_2);
  About->setTabText(idx, tr(TOSTRING(PRODUCT_NAME), "about box tab title"));

  if (QFile::exists(":/branding/license_text.html")) {
      OpenFlipperLicense->setText("");
      QFile licenseTextFile(":/branding/license_text.html");
      licenseTextFile.open(QFile::ReadOnly);
      QByteArray licenseTextBA = licenseTextFile.readAll();
      QString licenseText = QString::fromUtf8(licenseTextBA.data(), licenseTextBA.size());
      OpenFlipperLicense->setHtml(licenseText.arg(TOSTRING(PRODUCT_NAME)));
  } else {
      OpenFlipperLicense->setHtml(OpenFlipperLicense->toHtml().arg(TOSTRING(PRODUCT_NAME)));
  }

  connect( actionClose , SIGNAL(triggered() ) , this, SLOT(hide()) );
  connect( closeButton , SIGNAL(clicked()   ) , this, SLOT(hide()) );
  closeButton->setFocus();
}

void AboutWidget::keyPressEvent(QKeyEvent * event) {
    if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) {
        hide();
    }
}
