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

#include "HoleFillerPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <ACG/Scenegraph/BaseNode.hh>

#include <MeshTools/MeshFunctions.hh>
#include <MeshTools/MeshSelectionT.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <map>
#include <limits>
#include <algorithm>

#if QT_VERSION >= 0x050000 
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "HoleInfoT.hh"

#define HOLEINFO "HoleInfoData"

/// Constructor
HoleFillerPlugin::HoleFillerPlugin() :
       tool_(0)
{

}

/// Initialize the toolbox widget
void HoleFillerPlugin::initializePlugin()
{
   tool_ = new HoleFillerToolbarWidget();
   QSize size(300, 300);
   tool_->resize(size);

   connect(tool_->tableWidget,SIGNAL(itemSelectionChanged()),this,SLOT(slotItemSelectionChanged()));
   
   connect(tool_->tableWidget,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(slotCellDoubleClicked(int,int)));

   connect(tool_->detect,SIGNAL(clicked()),this,SLOT(detectButton()));
   connect(tool_->fillButton, SIGNAL(clicked()), this, SLOT(slotFillSelection()) );

   QIcon* toolIcon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"holefilling.png");

   emit addToolbox( tr("Hole Filler") , tool_ , toolIcon);
} 

/// add PickModes after initialization
void HoleFillerPlugin::pluginsInitialized()
{
   emit addPickMode("Separator");
   emit addPickMode("Hole Filler");

   emit setSlotDescription("fillAllHoles(int)", tr("Fill all holes from a given Mesh"),
                           QString("objectId").split(","), QString("Id of the mesh").split(","));

   emit setSlotDescription("fillHole(int,int)", tr("Fill a holes from a given Mesh where edgeHandle is on the boundary"),
                           QString("objectId,edgeHandle").split(","), QString("Id of the mesh,Handle of one boundary edge of the hole").split(","));

   //build the tableWidget with headers
   update_menu();
}

/// Fill all selected holes
void HoleFillerPlugin::slotFillSelection(){

  //get selected rows
  std::vector< int > rows;

  QList<QTableWidgetItem *> items = tool_->tableWidget->selectedItems();

  int itemCount = items.count() / 3;

  for (int i=0; i < itemCount ; i++)
      rows.push_back( items[i]->row() );

  //get a map from objectID to (selected) holeIDs
  std::vector< int > holes;
  std::vector< int > objects;

  for (uint i=0; i < rows.size(); i++){
    int objID  = holeMapping_[ rows[i] ].first;
    int holeID = holeMapping_[ rows[i] ].second;

    holes.push_back  ( holeID );
    objects.push_back( objID );
  }

  //init progressDialog
  QProgressDialog progress(tr("Filling holes..."), tr("Abort"), 0, holes.size(), 0);
  progress.setWindowModality(Qt::ApplicationModal);
  progress.setValue(0);

  int counter = 0;

  //iterate over all objects with holes that should be filled
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {

    // TYPE is TRIMESH
    if ( o_it->dataType( DATA_TRIANGLE_MESH ) ) {

      TriMesh* mesh = PluginFunctions::triMesh(o_it);

      //get perObjectData
      HoleInfo< TriMesh >* holeInfo = dynamic_cast< HoleInfo< TriMesh >* > ( o_it->objectData(HOLEINFO) );
    
      if (holeInfo == 0){
        holeInfo = new HoleInfo< TriMesh >( mesh );
        o_it->setObjectData(HOLEINFO, holeInfo);
      }

      //fill the holes
      for (uint i = 0; i < objects.size(); i++)
        if ( objects[i] == o_it->id() ){
          holeInfo->fillHole( holes[i]);

          if (progress.wasCanceled())
            break;
          else
            progress.setValue(++counter);
        }

      //update the object
      o_it->update();
    
      holeInfo->getHoles();
    
      o_it->update();
      update_menu();
      emit createBackup( o_it->id(), "Hole Filling", UPDATE_GEOMETRY | UPDATE_TOPOLOGY | UPDATE_SELECTION);
    }
    // DATATYPE is  POLYMESH
    else if ( o_it->dataType( DATA_POLY_MESH ) ) {
      emit log(LOGWARN, tr("HoleFilling unsupported for poly meshes") );
      continue;
    }
    
    //abort if user wants to
    if (progress.wasCanceled())
      break;
  }

  progress.close();

  emit updateView();
}

/// slot for displaying selected holes
void HoleFillerPlugin::slotItemSelectionChanged() {

  //get selected rows
  std::vector< int > rows;

  QList<QTableWidgetItem *> items = tool_->tableWidget->selectedItems();

  int itemCount = items.count() / 3;

  for (int i=0; i < itemCount ; i++)
      rows.push_back( items[i]->row() );

  //get a map from objectID to (selected) holeIDs
  std::vector< int > holes;
  std::vector< int > objects;

  for (uint i=0; i < rows.size(); i++){
    int objID  = holeMapping_[ rows[i] ].first;
    int holeID = holeMapping_[ rows[i] ].second;

    holes.push_back  ( holeID );
    objects.push_back( objID );
  }

  //iterate over all objects with holes that should be displayed
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {

    // TYPE is TRIMESH
    if ( o_it->dataType( DATA_TRIANGLE_MESH ) ) {

      TriMesh* mesh = PluginFunctions::triMesh(o_it);

      //get perObjectData
      HoleInfo< TriMesh >* holeInfo = dynamic_cast< HoleInfo< TriMesh >* > ( o_it->objectData(HOLEINFO) );
    
      if (holeInfo == 0){
        holeInfo = new HoleInfo< TriMesh >( mesh );
        o_it->setObjectData(HOLEINFO, holeInfo);
      }

      //clear the edge selection
      MeshSelection::clearEdgeSelection(mesh);

      //select the holes
      for (uint i = 0; i < objects.size(); i++)
        if ( objects[i] == o_it->id() )
          holeInfo->selectHole( holes[i] );

      //update the object
      o_it->update();
    }
    // DATATYPE is  POLYMESH
    else if ( o_it->dataType( DATA_POLY_MESH ) ) {

      PolyMesh* mesh = PluginFunctions::polyMesh(o_it);

      //get perObjectData
      HoleInfo< PolyMesh >* holeInfo = dynamic_cast< HoleInfo< PolyMesh >* > ( o_it->objectData(HOLEINFO) );
    
      if (holeInfo == 0){
        holeInfo = new HoleInfo< PolyMesh >( mesh );
        o_it->setObjectData(HOLEINFO, holeInfo);
      }

      //clear the edge selection
      MeshSelection::clearEdgeSelection(mesh);

      //select the holes
      for (uint i = 0; i < objects.size(); i++)
        if ( objects[i] == o_it->id() )
          holeInfo->selectHole( holes[i] );

      //update the object
      o_it->update();

    }
  }

  emit updateView();
}

/// Slot for filling holes from double-clicked rows
void HoleFillerPlugin::slotCellDoubleClicked(int _row , int /*_col*/) {

  if ( _row > (int)holeMapping_.size() ) {
    emit log(LOGWARN, tr("Error for holeMapping_ vector size") );
    return;
  }

  BaseObjectData* object;
  int objID  = holeMapping_[_row].first;
  int holeID = holeMapping_[_row].second;

  if ( !PluginFunctions::getObject( objID, object ) ) {
    emit log(LOGWARN, tr("Unable to find object for hole (should not happen!!)") );
    return;
  }

  // TYPE is TRIMESH
  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh* mesh = PluginFunctions::triMesh(object);

    //get perObjectData
    HoleInfo< TriMesh >* holeInfo = dynamic_cast< HoleInfo< TriMesh >* > ( object->objectData(HOLEINFO) );
  
    if (holeInfo == 0){
      holeInfo = new HoleInfo< TriMesh >( mesh );
      object->setObjectData(HOLEINFO, holeInfo);
    }

    //fill the hole
    holeInfo->fillHole( holeID );

    emit updatedObject(object->id(),UPDATE_ALL);

    holeInfo->getHoles();

    update_menu();

    emit updateView();
  }
  // DATATYPE is  POLYMESH
  else if ( object->dataType( DATA_POLY_MESH ) ) {
    
//     PolyMesh* mesh = PluginFunctions::polyMesh(object);
//     
//     HoleInfo< PolyMesh >* holeInfo = dynamic_cast< HoleInfo< PolyMesh >* > ( object->objectData(HOLEINFO) );
//     
//     if (holeInfo == 0){
//       holeInfo = new HoleInfo< PolyMesh >( mesh );
//       object->setObjectData(HOLEINFO, holeInfo);
//     }
//     
//     holeInfo->getHoles();    
    
    emit log(LOGWARN,tr("HoleFilling unsupported for poly meshes"));
    return;
  }
}

/// detect holes on all objects
void HoleFillerPlugin::detectButton( )
{
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {

    //case TRIMESH
    if ( o_it->dataType( DATA_TRIANGLE_MESH ) ) {

      HoleInfo< TriMesh >* holeInfo = dynamic_cast< HoleInfo< TriMesh >* > ( o_it->objectData(HOLEINFO) );
    
      if ( holeInfo == 0 ){
        TriMesh* mesh = PluginFunctions::triMesh(*o_it);
        holeInfo = new HoleInfo< TriMesh >( mesh );
        o_it->setObjectData(HOLEINFO, holeInfo);
      }

      holeInfo->getHoles();
    }

//     //case POLYMESH
//     if ( o_it->dataType( DATA_POLY_MESH ) ) {
// 
//       HoleInfo< PolyMesh >* holeInfo = dynamic_cast< HoleInfo< PolyMesh >* > ( o_it->objectData(HOLEINFO) );
//     
//       if (holeInfo == 0){
//         PolyMesh* mesh = PluginFunctions::polyMesh(*o_it);
//         holeInfo = new HoleInfo< PolyMesh >( mesh );
//         o_it->setObjectData(HOLEINFO, holeInfo);
//       }
// 
//       holeInfo->getHoles();
//     }
  }

  update_menu();
}

/// check for holes if an object has changed
void HoleFillerPlugin::slotObjectUpdated(int _identifier) {

  BaseObjectData* object;

  // Check if this is a usable Object
  if ( !PluginFunctions::getObject(_identifier,object) ) 
    return;

  bool updated = false;
  
  // get holes for TRIMESH
  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    HoleInfo< TriMesh >* holeInfo = dynamic_cast< HoleInfo< TriMesh >* > ( object->objectData(HOLEINFO) );
  
    if ( holeInfo ) {
      holeInfo->getHoles();
      updated = true;
    }
  }

  // get holes for POLYMESH
  else if ( object->dataType( DATA_POLY_MESH ) ) {

    HoleInfo< PolyMesh >* holeInfo = dynamic_cast< HoleInfo< PolyMesh >* > ( object->objectData(HOLEINFO) );
  
    if ( holeInfo ) {
      holeInfo->getHoles();
      updated = true;
    }
  }

  // Only update if something has changed!
  if ( updated )
    update_menu();
}

///update the entries in the tableWidget
void HoleFillerPlugin::update_menu() {

  holeMapping_.clear();

  tool_->tableWidget->clear();

  tool_->tableWidget->setRowCount ( 0 );
  tool_->tableWidget->setColumnCount ( 4 );

  QStringList headerdata;
  headerdata << "Object" << "Edges" << "Boundary Length" << "BB Diagonal";

  tool_->tableWidget->setHorizontalHeaderLabels(headerdata);
  tool_->updateGeometry();
  
  int elements = 0;
  int count = 0;

  //iterate over all objects
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {

    // DATATYPE is  TRIMESH
    if ( o_it->dataType( DATA_TRIANGLE_MESH ) ) {

      //get perObjectData  
      HoleInfo< TriMesh >* holeInfo = dynamic_cast< HoleInfo< TriMesh >* > ( o_it->objectData(HOLEINFO) );

      if (holeInfo != 0){
        elements += holeInfo->holes()->size();
        tool_->tableWidget->setRowCount ( elements );

        //add holes to the table
        for (uint i = 0 ; i < holeInfo->holes()->size() ; ++i ) {
          // Set Name of the object
          QTableWidgetItem* name = new QTableWidgetItem( o_it->name() );
          name->setFlags( 0 );
          name->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled);
          tool_->tableWidget->setItem(count,0,name);
    
          // Set Number of the edges
          QTableWidgetItem* size = new QTableWidgetItem( QString::number( (*holeInfo->holes())[i].size() ) );
          size->setFlags( 0 );
          size->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled);
          tool_->tableWidget->setItem(count,1,size);
    
          // Set boundary Length
          std::vector< TriMesh::EdgeHandle >::iterator endIter = (*holeInfo->holes())[i].end();
          double boundaryLength = 0.0;
          TriMesh* mesh = 0;
          PluginFunctions::getMesh(o_it->id(),mesh);
          for (std::vector< TriMesh::EdgeHandle >::iterator edgeIter = (*holeInfo->holes())[i].begin(); edgeIter != endIter; ++edgeIter)
            boundaryLength += mesh->calc_edge_length(*edgeIter);
          QTableWidgetItem* boundaryLengthWidget = new QTableWidgetItem( QString::number(boundaryLength) );
          boundaryLengthWidget->setFlags( 0 );
          boundaryLengthWidget->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled);
          tool_->tableWidget->setItem(count,2,boundaryLengthWidget);

          //compute bounding box
          TriMesh::Point minCoord = TriMesh::Point(std::numeric_limits<TriMesh::Scalar>::max(),std::numeric_limits<TriMesh::Scalar>::max(),std::numeric_limits<TriMesh::Scalar>::max());
          TriMesh::Point maxCoord = TriMesh::Point(-std::numeric_limits<TriMesh::Scalar>::max(),-std::numeric_limits<TriMesh::Scalar>::max(),-std::numeric_limits<TriMesh::Scalar>::max());
          for (std::vector< TriMesh::EdgeHandle >::iterator edgeIter = (*holeInfo->holes())[i].begin(); edgeIter != endIter; ++edgeIter)
          {
            TriMesh::Point pos = mesh->point(mesh->from_vertex_handle(mesh->halfedge_handle(*edgeIter,0)));
            minCoord[0] = std::min(minCoord[0],pos[0]);
            minCoord[1] = std::min(minCoord[1],pos[1]);
            minCoord[2] = std::min(minCoord[2],pos[2]);

            maxCoord[0] = std::max(maxCoord[0],pos[0]);
            maxCoord[1] = std::max(maxCoord[1],pos[1]);
            maxCoord[2] = std::max(maxCoord[2],pos[2]);
          }

          TriMesh::Scalar bbDiagonal = (minCoord-maxCoord).length();

          QTableWidgetItem* bbDiagonalWidget = new QTableWidgetItem( QString::number(bbDiagonal) );
          bbDiagonalWidget->setFlags( 0 );
          bbDiagonalWidget->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled);
          tool_->tableWidget->setItem(count,3,bbDiagonalWidget);
    
          // remember the id for the hole
          holeMapping_.push_back( std::pair<int , int>( o_it->id() , i ) );
    
          ++count;
        }
      }
    }
    // DATATYPE is  POLYMESH
    else if ( o_it->dataType( DATA_POLY_MESH ) ) {

      //get perObjectData  
      HoleInfo< PolyMesh >* holeInfo = dynamic_cast< HoleInfo< PolyMesh >* > ( o_it->objectData(HOLEINFO) );

      if (holeInfo != 0){
        elements += holeInfo->holes()->size();
        tool_->tableWidget->setRowCount ( elements );

        //add holes to the table
        for (uint i = 0 ; i < holeInfo->holes()->size() ; ++i ) {
          // Set Name of the object
          QTableWidgetItem* name = new QTableWidgetItem( o_it->name() );
          name->setFlags( 0 );
          name->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled);
          tool_->tableWidget->setItem(count,0,name);
    
          // Set Number of the edges
          QTableWidgetItem* size = new QTableWidgetItem( QString::number( (*holeInfo->holes())[i].size() ) );
          size->setFlags( 0 );
          size->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled);
          tool_->tableWidget->setItem(count,1,size);
    
          // Set radius
          QTableWidgetItem* radius = new QTableWidgetItem( "TODO" );
          radius->setFlags( 0 );
          radius->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled);
          tool_->tableWidget->setItem(count,2,radius);
    
          // remember the id for the hole
          holeMapping_.push_back( std::pair<int , int>( o_it->id() , i ) );
    
          ++count;
        }
      }
    }
  }

  tool_->tableWidget->resizeColumnToContents ( 1 );
}

/// fill all holes from a given object
void HoleFillerPlugin::fillAllHoles(int _objectID){
  BaseObjectData* object = 0;
  PluginFunctions::getObject( _objectID , object );

  if (object == 0){
    emit log(LOGERR, tr("Could not get object from ID.") );
    return;
  }
  
  emit scriptInfo( "fillAllHoles( ObjectId )"  );


  // TYPE is TRIMESH
  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh* mesh = PluginFunctions::triMesh(object);

    //get perObjectData
    HoleInfo< TriMesh >* holeInfo = dynamic_cast< HoleInfo< TriMesh >* > ( object->objectData(HOLEINFO) );
  
    if (holeInfo == 0){
      holeInfo = new HoleInfo< TriMesh >( mesh );
      object->setObjectData(HOLEINFO, holeInfo);
    }

    //fill the hole
    holeInfo->fillAllHoles();

    //and update everything
    emit updatedObject(object->id(),UPDATE_ALL);

    holeInfo->getHoles();

    update_menu();

    emit updateView();
  }
  // DATATYPE is  POLYMESH
  else if ( object->dataType( DATA_POLY_MESH ) ) {
    emit log(LOGERR, tr("HoleFilling unsopported for poly meshes.") );
    return;
  }
}


/// fill a hole in given object where _edgeHandle is on the boundary
void HoleFillerPlugin::fillHole(int _objectID, int _edgeHandle){
  BaseObjectData* object = 0;
  PluginFunctions::getObject( _objectID , object );

  if (object == 0){
    emit log(LOGERR, tr("Could not get object from ID.") );
    return;
  }
  
  emit scriptInfo( "fillHole( ObjectId , EdgeHandle )"  );


  // TYPE is TRIMESH
  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh* mesh = PluginFunctions::triMesh(object);

    //get perObjectData
    HoleInfo< TriMesh >* holeInfo = dynamic_cast< HoleInfo< TriMesh >* > ( object->objectData(HOLEINFO) );
  
    if (holeInfo == 0){
      holeInfo = new HoleInfo< TriMesh >( mesh );
      object->setObjectData(HOLEINFO, holeInfo);
    }

    //check edgeHandle
    TriMesh::EdgeHandle eh(_edgeHandle);

    if ( !eh.is_valid() || !mesh->is_boundary(eh) ){
      emit log(LOGERR, tr("Invalid edge handle.") );
      return;
    }

    //fill the hole
    holeInfo->fillHole( eh );

    //and update everything
    emit updatedObject(object->id(),UPDATE_ALL);

    holeInfo->getHoles();

    update_menu();

    emit updateView();
  }
  // DATATYPE is  POLYMESH
  else if ( object->dataType( DATA_POLY_MESH ) ) {
    emit log(LOGERR, tr("HoleFilling unsopported for poly meshes.") );
    return;
  }
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( holefillerplugin , HoleFillerPlugin );
#endif

