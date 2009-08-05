/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include "PluginDialog.hh"
#include <QtGui>
#include <QMessageBox>
#include <QFileDialog>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/common/GlobalOptions.hh>


PluginDialog::PluginDialog(std::vector<PluginInfo>& _plugins, QWidget *parent)
    : QDialog(parent),
      plugins_(_plugins)
{
  setupUi(this);

  connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(loadButton, SIGNAL(clicked()), this, SIGNAL(loadPlugin()));
  connect(loadButton, SIGNAL(clicked()), this, SLOT(reject()));
  connect(unloadButton, SIGNAL(clicked()), this, SLOT(slotUnload()));

  //set icons
  QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

  closeButton->setIcon( QIcon(iconPath + "window-close.png"));
  loadButton->setIcon( QIcon(iconPath + "network-connect.png"));
  unloadButton->setIcon( QIcon(iconPath + "network-disconnect.png"));

}

void PluginDialog::closeEvent(QCloseEvent *event)
{
  event->accept();
  accept();
}

int PluginDialog::exec()
{

  for (uint i = 0; i < plugins_.size(); i++){
    QFrame* frame = new QFrame();
    QHBoxLayout* hlayout = new QHBoxLayout;
    QLabel* name = new QLabel( plugins_[i].name );
    QFont font;
    font.setBold(true);
    font.setPointSize(10);
    name->setFont(font);
    QLabel* version = new QLabel( plugins_[i].version );
//     QLabel* author = new QLabel( "RWTH Computer Graphics Group" );

    hlayout->addWidget(name);
    hlayout->addStretch();
    hlayout->addWidget(version);
//     hlayout->addSpacing(10);
//     hlayout->addWidget(author);

    QVBoxLayout* vlayout = new QVBoxLayout;

    QLabel* description = new QLabel( plugins_[i].description );

    vlayout->addLayout(hlayout,20);
    vlayout->addWidget(description);
    frame->setLayout(vlayout);

    QListWidgetItem *item = new QListWidgetItem("");
    frames_.push_back(frame);
    item->setSizeHint( QSize (100,50) );
    list->addItem(item);
    list->setItemWidget(item, frame);
  }

  int ret = QDialog::exec();

  for (int i=0; i < frames_.count(); i++)
    delete frames_[i];

  return ret;
}

void PluginDialog::slotUnload()
{
  int buttonState = QMessageBox::No;
  QStringList dontLoad;

  for (int i=0; i < list->selectedItems().size(); i++){

    QString name = plugins_[ list->row( list->selectedItems()[i] ) ].rpcName;
    
    if (list->selectedItems().size() == 1){
      //show messageBox without YESTOALL / NOTOALL
      buttonState = QMessageBox::question(this, tr("Prevent Plugin Loading"),
                   tr("Do you want to prevent OpenFlipper from loading this plugin on the next start?"),
                   QMessageBox::Yes | QMessageBox::No,
                   QMessageBox::No);
    }else{
      //show messageBox with YESTOALL / NOTOALL
      if (buttonState == QMessageBox::Yes || buttonState == QMessageBox::No)
      buttonState = QMessageBox::question(this, tr("Prevent Plugin Loading"),
                   tr("Do you want to prevent OpenFlipper from loading this plugin on the next start?"),
                   QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll ,
                   QMessageBox::No);
    }

  if (buttonState == QMessageBox::Yes || buttonState == QMessageBox::YesToAll)
    dontLoad << name;

    //unload plugin
    emit unloadPlugin(name);
  }

  //prevent OpenFlipper from loading the plugin on the next start
  if (!dontLoad.isEmpty())
    emit dontLoadPlugins(dontLoad);

  reject();
}
