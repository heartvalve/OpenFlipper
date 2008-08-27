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





#include <QtGui>

#include "DataControlPlugin.hh"

#include <QLayout>
#include <QGridLayout> 
#include <QItemSelectionModel>
 
#include <iostream>
#include <ACG/GL/GLState.hh>
#include <QStringList>
#include <ACG/Scenegraph/BaseNode.hh>
#include <QModelIndexList>

#include <queue>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


void DataControlPlugin::pluginsInitialized() {
  QMenu* contextMenu = new QMenu("Object selection");
  
  //Target Objects
  QAction* hideAction = new QAction(tr("&Hide"), this);
  hideAction->setStatusTip(tr("Hide object"));
  connect(hideAction, SIGNAL(triggered()), this, SLOT(slotContextMenuHide()) );
  contextMenu->addAction(hideAction);
  
  //Target Objects
  targetAction_ = new QAction(tr("&target"), this);
  targetAction_->setCheckable(true);
  targetAction_->setStatusTip(tr("Set object as target"));
  connect(targetAction_, SIGNAL(triggered()), this, SLOT(slotContextMenuTarget()) );
  contextMenu->addAction(targetAction_);
  
  //Source Objects
  sourceAction_ = new QAction(tr("&source"), this);
  sourceAction_->setCheckable(true);
  sourceAction_->setStatusTip(tr("Set object as source"));
  connect(sourceAction_, SIGNAL(triggered()), this, SLOT(slotContextMenuSource()) );
  contextMenu->addAction(sourceAction_);  
  
  emit addContextMenu(contextMenu , DATA_ALL , CONTEXTTOPLEVELMENU);   
}

bool DataControlPlugin::initializeToolbox(QWidget*& _widget)
{
   locked = false;
   tool_ = new QWidget();  
   _widget = tool_;
   QSize size(300, 300);
   tool_->resize(size); 
   MeshDialogLayout_ = new QGridLayout( tool_); 
   
   model_ = new TreeModel( );
   
   view_ = new QTreeView(0);
   view_->setModel(model_);
   
   view_->QTreeView::resizeColumnToContents(1);
   view_->QTreeView::resizeColumnToContents(2);
   view_->QTreeView::resizeColumnToContents(3);
   
   view_->setContextMenuPolicy(Qt::CustomContextMenu);
   
   view_->setSelectionBehavior(QAbstractItemView::SelectRows);
   view_->setSelectionMode(QAbstractItemView::ExtendedSelection);
   
   connect( model_,SIGNAL(dataChanged( const QModelIndex&, const QModelIndex& ) ),
             this,SLOT(slotDataChanged( const QModelIndex&, const QModelIndex& )));
   
   connect( model_ , SIGNAL(rowsRemoved( const QModelIndex &, int , int ) ),
            this , SLOT(slotRowsRemoved ( const QModelIndex &, int , int ) ) );
   
   connect( view_,SIGNAL(customContextMenuRequested ( const QPoint &  )  ),
            this,SLOT(slotCustomContextMenuRequested ( const QPoint & ) ));
   
   MeshDialogLayout_->addWidget( view_ , 0,0  ); 
   
   viewHeader_ = view_->header(); 
   viewHeader_->setContextMenuPolicy(Qt::CustomContextMenu);
   
   // connect the slot for the context menu
   connect( viewHeader_, SIGNAL(customContextMenuRequested ( const QPoint &  )  ),
            this,        SLOT(slotHeaderCustomContextMenuRequested ( const QPoint & ) ));
   
   return true;
}

void DataControlPlugin::slotObjectUpdated( int _identifier ) {
  model_->updatedObject( _identifier );
}

void DataControlPlugin::slotCellClicked(int _row , int _col) {
   
//    if ( _col == 0 ) {
//          QTableWidgetItem* target = objectList_->item(_row,_col);
//          if ( target->checkState() == Qt::Checked ) {
//             target->setCheckState(Qt::Unchecked);
//             (*data_)[_row]->target(false); 
//          } else { 
//             target->setCheckState(Qt::Checked);
//             (*data_)[_row]->target(true); 
//          }
//         emit activeObjectChanged();
//    }
}

void DataControlPlugin::verticalHeaderClicked( int _row ) {
//     (*data_)[_row]->target ( !(*data_)[_row]->target() );
//    QTableWidgetItem* target = objectList_->item(_row,3);
//    if ( target->checkState() == Qt::Checked ) {
//       target->setCheckState(Qt::Unchecked);
//       (*data_)[_row]->target(false); 
//    } else { 
//       target->setCheckState(Qt::Checked);
//       (*data_)[_row]->target(true); 
//    }
//    emit activeObjectChanged();
// //    QTableWidgetSelectionRange range(5,0,5,3);
// //    objectList_->setRangeSelected(range,true);
}

void DataControlPlugin::verticalCountClicked( int  id , int old , int newc ) {
//    emit log(LOGWARN,"VHeader : Count " + QString::number(id) + " " + QString::number(old) + " " + QString::number(newc));
}


void DataControlPlugin::slotActiveObjectChanged() 
{
  ///@todo set all target checkboxes right
  // hint : dont call slotObjectUpdated -1 as this will do a redraw
//   slotObjectUpdated(-1);
  update_active();
}

void DataControlPlugin::update_active( ) {
  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; 
                                        o_it != PluginFunctions::objects_end(); ++o_it)  {
      if ( o_it->target() ) {
               o_it->materialNode()->disable_blending();
               OpenMesh::Vec4f base_color = o_it->materialNode()->base_color();
               base_color[3] = 1.0;
               o_it->materialNode()->set_base_color(base_color);
               OpenMesh::Vec4f ambient_color = o_it->materialNode()->ambient_color();
               ambient_color[3] = 1.0;
               o_it->materialNode()->set_ambient_color(ambient_color);
               OpenMesh::Vec4f diffuse_color = o_it->materialNode()->diffuse_color();
               diffuse_color[3] = 1.0;
               o_it->materialNode()->set_diffuse_color(diffuse_color);
      }  else { 
            o_it->materialNode()->enable_blending();
            OpenMesh::Vec4f base_color = o_it->materialNode()->base_color();
            base_color[3] = 0.4;
            o_it->materialNode()->set_base_color(base_color);
            OpenMesh::Vec4f ambient_color = o_it->materialNode()->ambient_color();
            ambient_color[3] = 0.4;
            o_it->materialNode()->set_ambient_color(ambient_color);
            OpenMesh::Vec4f diffuse_color = o_it->materialNode()->diffuse_color();
            diffuse_color[3] = 0.4;
            o_it->materialNode()->set_diffuse_color(diffuse_color);
      }                                     
  }
  
  emit update_view();
}

void DataControlPlugin::slotKeyEvent( QKeyEvent* _event )
{

//    QList<QTableWidgetItem *> selected ;
//    QList<QTableWidgetSelectionRange> ranges;
//    switch (_event->key()) 
//    {
//      case Qt::Key_M : 
//           for ( uint i = 0 ; i < data_->size() ; ++i ) {
//                QTableWidgetSelectionRange range(i,0,i,3);
//                objectList_->setRangeSelected(range,true);
//          }
//          return;          
//           
//       case Qt::Key_N : 
//          std::cerr << "Checking : " << std::endl;
//          for ( uint i = 0 ; i < data_->size() ; ++i ) {
//             QTableWidgetItem * item = objectList_->verticalHeaderItem ( i );
//             if ( objectList_->isItemSelected(item)  ) 
//                std::cerr << i << item->text().toStdString() << " : yes" << std::endl;
//             else {
//                std::cerr << i << item->text().toStdString() << " : no" << std::endl;
//             }
//          }
//          
//          selected = objectList_->selectedItems();
//          std::cerr << "Selected " << selected.size() << std::endl;
//          
//          ranges = objectList_->selectedRanges();
//          std::cerr << "Ranges " << ranges.size() << std::endl;
//          
//          for ( int i = 0 ; i < ranges.size(); ++i ) {
//            std::cerr << "Range " << i << " : " << ranges[i].topRow() << " .. " << ranges[i].bottomRow() << std::endl;        
//            std::cerr << "Range " << i << " : " << ranges[i].leftColumn() << " .. " << ranges[i].rightColumn() << std::endl;        
//          }
//          
//          return;
//       case Qt::Key_C : 
//           if ( _event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) ) {
//                for ( uint i = 0 ; i < data_->size() ; ++i ) {  
//                   (*data_)[i]->source(false);
//                }
//                emit updated_objects(-1);
//               return;
//           } 
//           
//           if ( _event->modifiers() == (Qt::AltModifier | Qt::ShiftModifier) ) {
//              for ( uint i = 0 ; i < data_->size() ; ++i ) {  
//                 (*data_)[i]->target(false);
//               }
//               emit updated_objects(-1);
//              return;
//           } 
//           
//           return;
//           
//        case Qt::Key_A : 
//           if ( _event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) ) {
//                for ( uint i = 0 ; i < data_->size() ; ++i ) {  
//                   (*data_)[i]->source(true);
//                }
//                emit updated_objects(-1);
//               return;
//           } 
//           
//           if ( _event->modifiers() == (Qt::AltModifier | Qt::ShiftModifier) ) {
//              for ( uint i = 0 ; i < data_->size() ; ++i ) {  
//                 (*data_)[i]->target(true);
//               }
//               emit updated_objects(-1);
//              return;
//           } 
//           
//           return;
//       default:
//       break;
//   }
  
  if ( _event->modifiers() == Qt::ControlModifier ) {
    switch (_event->key()) {
      case Qt::Key_A : 
          setAllTarget();
        return;
      default:
        return;
    }
  }
}

void DataControlPlugin::setAllTarget() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; 
                                    o_it != PluginFunctions::objects_end(); ++o_it)  
  o_it->target(true);                       
  emit activeObjectChanged();
  emit updated_objects(-1);  
}

void DataControlPlugin::setAllSource() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; 
                                    o_it != PluginFunctions::objects_end(); ++o_it)  
  o_it->source(true);                       
  emit updated_objects(-1);  
}

void DataControlPlugin::clearAllTarget() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; 
                                    o_it != PluginFunctions::objects_end(); ++o_it)  
  o_it->target(false);                       
  emit activeObjectChanged();
  emit updated_objects(-1);  
}

void DataControlPlugin::clearAllSource() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; 
                                    o_it != PluginFunctions::objects_end(); ++o_it)  
  o_it->source(false);                       
  emit updated_objects(-1);  
}

void DataControlPlugin::hideAll() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; 
                                    o_it != PluginFunctions::objects_end(); ++o_it)  
    o_it->hide();                       
  emit update_view();
}

void DataControlPlugin::showAll() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; 
                                    o_it != PluginFunctions::objects_end(); ++o_it)  
    o_it->show();                       
  emit update_view();
}

void DataControlPlugin::slotDataChanged ( const QModelIndex & topLeft, 
                                          const QModelIndex & bottomRight ) 
{
  
  switch (topLeft.column()) {
    case 0:
//         std::cerr << "Todo : NameChange" << std::endl;
      break;
      
    // show/hide
    case 1:
      break;
      
    // source
    case 2:
      break; 
      
    // target
    case 3:
      emit activeObjectChanged();
      break;
      
    default:
      break;
  }

  view_->expandToDepth(0);
  emit update_view();
}

void DataControlPlugin::slotRowsRemoved ( const QModelIndex & _parent, int _start, int _end ) {
  std::cerr << "Row removed! " << std::endl;
  emit update_view();
}

void DataControlPlugin::loadIniFileOptions( INIFile& _ini ) {
  if ( !_ini.section_exists( "Groups" ) ) 
    return;
  
  // Names of all groups
  QStringList groupNames;
  
  // names of the primary groups
  QStringList rootGroup;
  
  // Get the list of group names to the file
  _ini.get_entry(groupNames,"Groups","groups");
  
  // Get the primary group names to the file
  _ini.get_entry(rootGroup,"Groups","rootGroup");
  
  // Go over one level of the groups
  while ( rootGroup.size() > 0 ) {
    QString current = rootGroup[0];
    rootGroup.removeFirst();
    
    QStringList groupChildren;
    QStringList elementChildren;
    
    _ini.get_entry(elementChildren ,current,"children");
    _ini.get_entry(groupChildren ,current,"subgroups");
    
    // if we get a parent item, scan the tree for it or use the root node otherwise
    BaseObject* parentItem;
    QString parentName;
    if ( _ini.get_entry(parentName,current,"parent") ) {
      parentItem = PluginFunctions::objectRoot()->childExists(parentName);
      if ( parentItem == 0 )
        parentItem = PluginFunctions::objectRoot(); 
    } else 
      parentItem = PluginFunctions::objectRoot(); 
    
    rootGroup << groupChildren;      
      
    // check if this group already exists
    BaseObject* group =  PluginFunctions::objectRoot()->childExists( current );
    
    // group does not exist
    if ( !group ) {
      
      group = dynamic_cast< BaseObject* >( new GroupObject( current, dynamic_cast< GroupObject* >(parentItem ) ) );
      
      parentItem->appendChild(group);
      group->setParent(parentItem);
    }
    
    // process children
    for ( int i = 0 ; i < elementChildren.size() ; ++i ) {
      BaseObject* childItem =  PluginFunctions::objectRoot()->childExists( elementChildren[i] );
      if ( childItem ) {
        childItem->parent()->removeChild(childItem);
        childItem->setParent(group);
        group->appendChild(childItem);
      }
    }
  }
    
  emit updated_objects(-1);   
}

void DataControlPlugin::saveIniFileOptions( INIFile& _ini ) {
  if ( !_ini.section_exists( "Groups" ) ) 
    _ini.add_section("Groups"); 
  
  std::queue< BaseObject* > children;
  children.push( PluginFunctions::objectRoot() );
  
  std::vector< BaseObject* > groups;
  
  // Get all groups from the tree
  while ( children.size() > 0 ) {
    BaseObject* item = children.front();
    children.pop();
    
    for ( int i = 0 ; i < item->childCount(); ++i ) 
      if ( item->child(i)->dataType(DATA_GROUP)) 
        children.push( item->child(i) ); 
    
    if ( item->dataType(DATA_GROUP) && (item != PluginFunctions::objectRoot() ) )
      groups.push_back(item);
  }
  
  // Names of all groups
  QStringList groupNames;
  
  // names of the primary groups
  QStringList rootGroup;
  
  for ( uint i = 0 ; i < groups.size() ; ++i ) {
    groupNames.push_back( groups[i]->name() );  
    
    if ( !_ini.section_exists( groups[i]->name() ) ) 
      _ini.add_section(groups[i]->name()); 
    
    _ini.add_entry(groups[i]->name(),"groupname",groups[i]->name());
    
    // write the name of the parent 
    if ( ( groups[i]->parent() != 0 ) && ( groups[i]->parent() != PluginFunctions::objectRoot() ) )
      _ini.add_entry(groups[i]->name(),"parent",groups[i]->parent()->name());
    
    if ( groups[i]->parent() == PluginFunctions::objectRoot() )
      rootGroup.push_back( groups[i]->name() );
    
    // Write a list of this groups children
    QStringList groupchildren;
    QStringList elementchildren;
    for ( int j = 0 ; j < groups[i]->childCount(); ++j ) {
      if  ( groups[i]->child(j)->dataType(DATA_GROUP) )
        groupchildren.push_back( groups[i]->child(j)->name() );
      else
        elementchildren.push_back( groups[i]->child(j)->name() );
    }
    
    _ini.add_entry(groups[i]->name(),"subgroups",groupchildren);
    _ini.add_entry(groups[i]->name(),"children",elementchildren);
  }
  
  // Write the list of group names to the file
  _ini.add_entry("Groups","groups",groupNames);
  
  // Write the primary group names to the file
  _ini.add_entry("Groups","rootGroup",rootGroup);
}


Q_EXPORT_PLUGIN2( datacontrolplugin , DataControlPlugin );

