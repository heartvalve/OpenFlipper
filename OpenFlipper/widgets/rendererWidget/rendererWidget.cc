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

#include "rendererWidget.hh"

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QMessageBox>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


RendererDialog::RendererDialog(QWidget *parent)
    : QDialog(parent)
{
  setupUi(this);

  list->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(list,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(slotContextMenu(const QPoint&)));

  //set icons
  QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

  closeButton->setIcon( QIcon(iconPath + "window-close.png"));

}

void RendererDialog::closeEvent(QCloseEvent *event)
{
  event->accept();
  accept();
}


void RendererDialog::showEvent ( QShowEvent * ) {
  update();
}

void RendererDialog::update()
{

  list->clear();

  for ( unsigned int i = 0 ; i < renderManager().available() ; ++i) {

    // Get and check post processor
    RendererInfo* renderer = renderManager()[i];
    if ( ! renderer )
      continue;

    QFrame* frame = new QFrame();
    QHBoxLayout* hlayout = new QHBoxLayout;

    QLabel* name = new QLabel( renderer->name );
    QFont font;
    font.setBold(true);
    font.setPointSize(10);
    name->setFont(font);
    QLabel* version = new QLabel( renderer->version );
    hlayout->addWidget(name);
    hlayout->addStretch();
    hlayout->addWidget(version);

    QVBoxLayout* vlayout = new QVBoxLayout;

    QLabel* description = new QLabel( renderer->description );
    descriptions_.push_back(description);

    vlayout->addLayout(hlayout,20);
    vlayout->addWidget(description);
    frame->setLayout(vlayout);

    QListWidgetItem *item = new QListWidgetItem("");
    frames_.push_back(frame);
    item->setSizeHint( QSize (100,50) );

    list->addItem(item);
    list->setItemWidget(item, frame);

    if ( renderManager().activeId(PluginFunctions::activeExaminer()) == i )
      item->setBackground( Qt::green );
  }

}

void RendererDialog::slotActivateRenderer() {
  for (int i=0; i < list->selectedItems().size(); ++i)
  {
    QListWidgetItem* widget = list->selectedItems()[i];

    renderManager().setActive( list->row( widget ), PluginFunctions::activeExaminer());
  }

  update();
}


void RendererDialog::slotContextMenu(const QPoint& _point)
{
  if (!list->count())
    return;

  QMenu *menu = new QMenu(list);
  QAction* action = 0;

  action = menu->addAction(tr("Activate"));
  connect(action,SIGNAL(triggered(bool)),this,SLOT(slotActivateRenderer()));


  menu->exec(list->mapToGlobal(_point),0);

}
