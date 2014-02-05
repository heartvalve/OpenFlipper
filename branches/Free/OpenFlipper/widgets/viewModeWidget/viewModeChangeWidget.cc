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




#include "viewModeChangeWidget.hh"
#include <OpenFlipper/widgets/coreWidget/CoreWidget.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <QInputDialog>
#include <QMessageBox>

/// Constructor
viewModeChangeWidget::viewModeChangeWidget(const QVector< ViewMode* >& _modes, QWidget *_parent)
  : QDialog(_parent),
    modes_(_modes)
{
  setupUi(this);


  connect(viewModeList, SIGNAL(clicked (QModelIndex)), this, SLOT(slotModeClicked(QModelIndex)) );
  

}

// =======================================================================================================
// External communication
// =======================================================================================================

/// Slot for changing View and closing widget
void viewModeChangeWidget::slotModeClicked(QModelIndex /*_idx*/ ){
  
  // Search for current mode 
  int id = -1;
  if ( viewModeList->selectedItems().count() > 0) 
    for (int i=0; i < modes_.size(); i++)
      if (modes_[i]->name == viewModeList->currentItem()->text()){
        id = i;
        break;
      }
      
  if ( id == -1 ) {
    std::cerr << "Currently selected Mode not found?!" << std::endl; 
    return;
  }

  
  emit changeView(modes_[id]->name,modes_[id]->visibleToolboxes,modes_[id]->visibleToolbars,modes_[id]->visibleContextMenus);
  close();
}


/// overloaded show function
void viewModeChangeWidget::show(QString _lastMode){
  QDialog::show();
  
  //fill viewModeList
  viewModeList->clear();
  for (int i=0; i < modes_.size(); i++){
    QListWidgetItem *item = new QListWidgetItem(viewModeList);
    item->setTextAlignment(Qt::AlignHCenter);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    
    QFile iconFile( OpenFlipper::Options::iconDirStr() + QDir::separator () + modes_[i]->icon  );
    
    if ( iconFile.exists() )
      item->setIcon( QIcon(iconFile.fileName()) );
    else {
      iconFile.setFileName( OpenFlipper::Options::configDirStr() + QDir::separator() + "Icons" + QDir::separator() + modes_[i]->icon );
      if ( iconFile.exists() )
        item->setIcon( QIcon(iconFile.fileName()) );
      else {
        item->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + QDir::separator () + "Unknown.png")  );
        std::cerr << "Unable to find icon file! " << iconFile.fileName().toStdString() <<  std::endl;
      }
    }
    
    item->setText(modes_[i]->name);
    
    if (modes_[i]->custom)
      viewModeList->item(i)->setForeground( QBrush(QColor(0,0,150) ) );
    else
      viewModeList->item(i)->setForeground( QBrush(QColor(0,0,0) ) );
  }
  
  //select given mode
  viewModeList->setCurrentRow(0);
  
  for (int i=0; i < viewModeList->count(); i++)
    if (viewModeList->item(i)->text() == _lastMode)
      viewModeList->setCurrentRow(i);
    
}

