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

#include "PluginDialog.hh"

#include <QMessageBox>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

const QColor PluginDialog::blockColor_  = QColor(228, 155, 18);
const QColor PluginDialog::unloadColor_ = QColor(172, 172, 172);
const QColor PluginDialog::loadedBuiltInColor_ = QColor(208, 240, 192);
const QColor PluginDialog::failColor_ = Qt::red;
const QColor PluginDialog::loadedExternalColor_ = QColor(152, 255, 152);

PluginDialog::PluginDialog(std::vector<PluginInfo>& _plugins, QWidget *parent)
    : QDialog(parent),
      plugins_(_plugins)
{
  setupUi(this);

  list->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(loadButton, SIGNAL(clicked()), this, SIGNAL(loadPlugin()));
  connect(loadButton, SIGNAL(clicked()), this, SLOT(reject()));
  connect(list,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(slotContextMenu(const QPoint&)));

  //set icons
  QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

  closeButton->setIcon( QIcon(iconPath + "window-close.png"));
  loadButton->setIcon( QIcon(iconPath + "network-connect.png"));

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
    descriptions_.push_back(description);

    vlayout->addLayout(hlayout,20);
    vlayout->addWidget(description);
    frame->setLayout(vlayout);

    QListWidgetItem *item = new QListWidgetItem("");
    frames_.push_back(frame);
    item->setSizeHint( QSize (100,50) );

    //set color depending on the current status
    switch(plugins_[i].status)
    {
      case PluginInfo::LOADED:
        if (plugins_[i].buildIn)
          item->setBackground(loadedBuiltInColor_);
        else
        {
          item->setBackground(loadedExternalColor_);
          description->setText(description->text()+tr(" *EXTERNAL*"));
        }
        break;
      case PluginInfo::FAILED:
        item->setBackground(failColor_);
        description->setText(description->text()+tr(" *FAILED*"));
        break;
      case PluginInfo::BLOCKED:
        item->setBackground(blockColor_);
        description->setText(description->text()+tr(" *BLOCKED*"));
        break;
      case PluginInfo::UNLOADED:
        item->setBackground(unloadColor_);
        description->setText(description->text()+tr(" *UNLOADED*"));
        break;
    }
    list->addItem(item);
    list->setItemWidget(item, frame);
  }

  int ret = QDialog::exec();

  for (int i=0; i < frames_.count(); i++)
    delete frames_[i];

  return ret;
}

void PluginDialog::slotBlockPlugin()
{
  for (int i=0; i < list->selectedItems().size(); ++i)
  {
    QListWidgetItem* widget = list->selectedItems()[i];
    widget->setBackground(blockColor_);

    PluginInfo* plugin = &plugins_[ list->row( widget ) ];
    descriptions_[list->row( widget )]->setText(plugin->description + tr(" *BLOCKED*"));

    emit blockPlugin(plugin->name);
  }
}
void PluginDialog::slotUnBlockPlugin()
{
  for (int i=0; i < list->selectedItems().size(); ++i)
  {
    QListWidgetItem* widget = list->selectedItems()[i];
    widget->setBackground(unloadColor_);

    PluginInfo* plugin = &plugins_[ list->row( widget ) ];
    descriptions_[list->row( widget )]->setText(plugin->description);

    emit unBlockPlugin(plugin->name);
  }
}

void PluginDialog::slotLoadPlugin()
{
  for (int i=0; i < list->selectedItems().size(); ++i)
  {
    QListWidgetItem* widget = list->selectedItems()[i];

    PluginInfo* plugin = &plugins_[ list->row( widget ) ];

    if (plugin->status == PluginInfo::BLOCKED)
    {
      QMessageBox msgBox;
      msgBox.setText("Plugin is blocked. Unblock it?");
      msgBox.setWindowTitle("Plugin blocked");
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      msgBox.setDefaultButton(QMessageBox::Yes);
      int rep = msgBox.exec();
      if ( rep == QMessageBox::Yes)
        emit unBlockPlugin(plugin->name);
    }

    if (plugin->buildIn)
      widget->setBackground(loadedBuiltInColor_);
    else
      widget->setBackground(loadedExternalColor_);

    descriptions_[list->row( widget )]->setText(plugin->description);

    QString licenseErros;
    emit loadPlugin(plugin->path,false,licenseErros,plugin->plugin);

    if (plugin->status == PluginInfo::FAILED)
    {
      descriptions_[list->row( widget )]->setText(plugin->description + tr(" *FAILED*"));
      widget->setBackground(failColor_);
    }else
    {
      plugin->status = PluginInfo::LOADED;
    }
  }
}

void PluginDialog::slotContextMenu(const QPoint& _point)
{
  if (!list->count())
    return;

  QMenu *menu = new QMenu(list);
  QAction* action = 0;

  PluginInfo* plugin = &plugins_[list->currentRow()];

  if ( plugin->status != PluginInfo::BLOCKED)//not blocked
  {
    action = menu->addAction(tr("Block Plugin"));
    connect(action,SIGNAL(triggered(bool)),this,SLOT(slotBlockPlugin()));
  }else//blocked
  {
    action = menu->addAction(tr("Unblock Plugin"));
    connect(action,SIGNAL(triggered(bool)),this,SLOT(slotUnBlockPlugin()));
  }

  if ( plugin->status != PluginInfo::LOADED)
  {
    action = menu->addAction(tr("Load Plugin"));
    connect(action,SIGNAL(triggered(bool)),this,SLOT(slotLoadPlugin()));
  }

  menu->exec(list->mapToGlobal(_point),0);

}
