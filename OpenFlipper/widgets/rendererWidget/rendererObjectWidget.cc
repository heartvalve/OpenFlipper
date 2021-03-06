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
*   $Revision: 15910 $                                                       *
*   $LastChangedBy: moeller $                                                *
*   $Date: 2012-12-05 12:53:39 +0100 (Mi, 05 Dez 2012) $                     *
*                                                                            *
\*===========================================================================*/

#include "rendererObjectWidget.hh"

#if QT_VERSION >= 0x050000
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QMessageBox>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


RendererObjectWidget::RendererObjectWidget(QWidget *parent)
    : QDialog(parent),
      highlighter_(0),
      textBrowser_(0)
{
  setupUi(this);

  connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));

  //set icons
  QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

  closeButton->setIcon( QIcon(iconPath + "window-close.png"));

  textBrowser_ = new TextBrowserWidget(this);
  textBrowserLayout->addWidget(textBrowser_);

  highlighter_ = new RenderObjectHighlighter( textBrowser_->document() );

  connect(showShadersBox,SIGNAL(clicked()),this,SLOT(update()));
}

void RendererObjectWidget::closeEvent(QCloseEvent *event)
{
  event->accept();
  accept();
}


void RendererObjectWidget::showEvent ( QShowEvent * ) {
  update();
}

void RendererObjectWidget::update()
{
  RendererInfo* renderer = renderManager().active(PluginFunctions::activeExaminer());

  textBrowser_->clear();

  if ( renderer ) {
    textBrowser_->insertPlainText(tr("Current renderer: ") + renderer->name +"\n");
    textBrowser_->insertPlainText(tr("Description:      ") + renderer->description +"\n");
    textBrowser_->insertPlainText(tr("Version:          ") + renderer->version + "\n" );
    textBrowser_->insertPlainText("\n" );

    //TODO: Flag for shader output activate/deactivate
    if (  renderManager().activeId(PluginFunctions::activeExaminer()) != 0 )
      textBrowser_->insertPlainText(renderer->plugin->renderObjectsInfo(showShadersBox->isChecked()));

  } else {
    textBrowser_->insertPlainText("Unable to get renderer!");
  }
}

