/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                      *
 *   $Author$                                                       *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

//================================================================
//
//  CLASS SplatCloudObjectSelectionPlugin - IMPLEMENTATION
//
//================================================================


//== INCLUDES ====================================================


#include "SplatCloudObjectSelectionPlugin.hh"

#include <set>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>


//== DEFINES =====================================================


// Primitive type icons
#define VERTEX_TYPE     "selection_splat.png"

// =======================================
// Define operations
// =======================================

// Vertices:
#define V_SELECT_ALL "Select All Splats"
#define V_CLEAR      "Clear Splat Selection"
#define V_INVERT     "Invert Splat Selection"
#define V_DELETE     "Delete Splat Selection"
#define V_COLORIZE   "Colorize Splat Selection"


//== IMPLEMENTATION ==============================================


void SplatCloudObjectSelectionPlugin::initializePlugin()
{
  // Tell core about all scriptable slots
  updateSlotDescriptions();
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::pluginsInitialized()
{
  // Create new selection environment for SplatCloud objects 
  // and register SplatCloud type for the environment.

  QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();

  emit addSelectionEnvironment( "SplatCloud Object Selections", "Select SplatCloud object primitives (vertices/points).", QIcon( iconPath + "selections.png" ), environmentHandle_ );

  // Register SplatCloud object type
  emit registerType( environmentHandle_, DATA_SPLATCLOUD );

  // Register SplatCloud primitive types
  emit addPrimitiveType( environmentHandle_, "Select Splats", QIcon( iconPath + VERTEX_TYPE ), vertexType_ );

  // Combine all supported types
  allSupportedTypes_ = vertexType_;

  // Determine, which selection modes are requested
  emit showToggleSelectionMode     ( environmentHandle_, true, allSupportedTypes_ );
  emit showSphereSelectionMode     ( environmentHandle_, true, allSupportedTypes_ );
//  emit showLassoSelectionMode      ( environmentHandle_, true, allSupportedTypes_ );
  emit showVolumeLassoSelectionMode( environmentHandle_, true, allSupportedTypes_ );

  // Define vertex operations
  QStringList vertexOperations;
  vertexOperations.append( V_SELECT_ALL );
  vertexOperations.append( V_CLEAR      );
  vertexOperations.append( V_INVERT     );
  vertexOperations.append( V_DELETE     );
  vertexOperations.append( V_COLORIZE   );

  emit addSelectionOperations( environmentHandle_, vertexOperations,  "Splat Operations", vertexType_ );

  // Register key shortcuts
  emit registerKeyShortcut( Qt::Key_A,      Qt::ControlModifier ); // Select (a)ll
  emit registerKeyShortcut( Qt::Key_C,      Qt::NoModifier      ); // (C)lear selection
  emit registerKeyShortcut( Qt::Key_I,      Qt::NoModifier      ); // (I)nvert selection
  emit registerKeyShortcut( Qt::Key_Delete, Qt::NoModifier      ); // Delete selected entities
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::updateSlotDescriptions()
{
  emit setSlotDescription( "selectVertices(int,IdList)",                tr("Select the specified vertices"),                         QString    ( "objectId,vertexList" ).split( "," ), QString    ( "Id of object,List of vertices"          ).split( "," ) );
  emit setSlotDescription( "unselectVertices(int,IdList)",              tr("Unselect the specified vertices"),                       QString    ( "objectId,vertexList" ).split( "," ), QString    ( "Id of object,List of vertices"          ).split( "," ) );
  emit setSlotDescription( "selectAllVertices(int)",                    tr("Select all vertices of an object"),                      QStringList( "objectId" ),                         QStringList( "Id of object"    )                                     );
  emit setSlotDescription( "clearVertexSelection(int)",                 tr("Clear vertex selection of an object"),                   QStringList( "objectId" ),                         QStringList( "Id of an object" )                                     );
  emit setSlotDescription( "invertVertexSelection(int)",                tr("Invert vertex selection of an object"),                  QStringList( "objectId" ),                         QStringList( "Id of an object" )                                     );
  emit setSlotDescription( "deleteVertexSelection(int)",                tr("Delete selected vertices"),                              QStringList( "objectId" ),                         QStringList( "Id of an object" )                                     );
  emit setSlotDescription( "colorizeVertexSelection(int,int,int,int)",  tr("Colorize the selected vertices"),                        QString    ( "objectId,r,g,b"      ).split( "," ), QString    ( "Id of an object,Red,Green,Blue"         ).split( "," ) );
  emit setSlotDescription( "loadSelection(int,QString)",                tr("Load selection from selection file"),                    QString    ( "objectId,filename"   ).split( "," ), QString    ( "Id of an object,Selection file"         ).split( "," ) );
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::slotSelectionOperation( QString _operation )
{
  SelectionInterface::PrimitiveType type = 0u;
  emit getActivePrimitiveType( type );

  if( (type & allSupportedTypes_) == 0 )
    return;

  // Test if operation should be applied to target objects only
  bool targetsOnly = false;
  emit targetObjectsOnly( targetsOnly );
  PluginFunctions::IteratorRestriction restriction = (targetsOnly ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS);

  if( _operation == V_SELECT_ALL )
  {
    // Select all vertices
    for( PluginFunctions::ObjectIterator o_it( restriction, DataType( DATA_ALL ) ); o_it != PluginFunctions::objectsEnd(); ++o_it )
    {
      if( o_it->visible() )
      {
        if( type & vertexType_ )
          selectAllVertices( o_it->id() );
        emit updatedObject( o_it->id(), UPDATE_SELECTION );
        emit createBackup ( o_it->id(), "Select All", UPDATE_SELECTION );
      }
    }
  }
  else if( _operation == V_CLEAR )
  {
    // Clear vertex selection
    for( PluginFunctions::ObjectIterator o_it( restriction, DataType( DATA_ALL ) ); o_it != PluginFunctions::objectsEnd(); ++o_it )
    {
      if( o_it->visible() )
      {
        if( type & vertexType_ )
          clearVertexSelection( o_it->id() );
        emit updatedObject( o_it->id(), UPDATE_SELECTION );
        emit createBackup ( o_it->id(), "Clear Selection", UPDATE_SELECTION );
      }
    }
  }
  else if( _operation == V_INVERT )
  {
    // Invert vertex selection
    for( PluginFunctions::ObjectIterator o_it( restriction, DataType( DATA_ALL ) ); o_it != PluginFunctions::objectsEnd(); ++o_it )
    {
      if( o_it->visible() )
      {
        if( type & vertexType_ )
          invertVertexSelection( o_it->id() );
        emit updatedObject( o_it->id(), UPDATE_SELECTION );
        emit createBackup ( o_it->id(), "Invert Selection", UPDATE_SELECTION );
      }
    }
  }
  else if( _operation == V_DELETE )
  {
    // Delete vertex selection
    for( PluginFunctions::ObjectIterator o_it( restriction, DataType( DATA_ALL ) ); o_it != PluginFunctions::objectsEnd(); ++o_it )
    {
      if( o_it->visible() )
      {
        emit updatedObject( o_it->id(), addUpdateType("RestoreOriginalColors") );
        emit createBackup ( o_it->id(), "Pre Deletion", UPDATE_ALL );

        if( type & vertexType_ )
          deleteVertexSelection( o_it->id() );

        emit createBackup ( o_it->id(), "Delete Selection", UPDATE_ALL );
        emit updatedObject( o_it->id(), addUpdateType("RestoreColormodeColors") );

        emit updatedObject( o_it->id(), UPDATE_ALL );
      }
    }
  }
  else if( _operation == V_COLORIZE )
  {
    // Colorize vertex selection
    for( PluginFunctions::ObjectIterator o_it( restriction, DataType( DATA_ALL ) ); o_it != PluginFunctions::objectsEnd(); ++o_it )
    {
      if( o_it->visible() )
      {
        emit updatedObject( o_it->id(), addUpdateType("RestoreOriginalColors") );
        emit createBackup ( o_it->id(), "Pre Colorization", UPDATE_COLOR );

        if( type & vertexType_ )
          setColorForSelection( o_it->id(), vertexType_ );

        emit createBackup ( o_it->id(), "Colorize Selection", UPDATE_COLOR );
        emit updatedObject( o_it->id(), addUpdateType("RestoreColormodeColors") );

        emit updatedObject( o_it->id(), UPDATE_COLOR );
      }
    }
  }
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::setColorForSelection( const int _objectId, const PrimitiveType _primitiveTypes )
{
  QColor c = QColorDialog::getColor( Qt::red, 0, tr("Choose color"), QColorDialog::ShowAlphaChannel );

  if( c.isValid() )
  {
    if( _primitiveTypes & vertexType_ )
    {
      // Vertex colorization
      colorizeVertexSelection( _objectId, c.red(), c.green(), c.blue(), c.alpha() );
    }
  }
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::slotToggleSelection( QMouseEvent *_event, SelectionInterface::PrimitiveType _currentType, bool _deselect )
{
  // Return if none of the currently active types is handled by this plugin
  if( (_currentType & allSupportedTypes_) == 0 )
    return;

  unsigned int node_idx, target_idx;
  ACG::Vec3d hit_point;

  // First of all, pick anything to find all possible objects
  if( PluginFunctions::scenegraphPick( ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx, target_idx, &hit_point ) )
  {
    BaseObjectData *object = 0;
    PluginFunctions::getPickedObject( node_idx, object );
    if( !object )
      return;

    if( object->dataType() == DATA_SPLATCLOUD )
    {
      // Pick SplatCloud
      if( PluginFunctions::scenegraphPick( ACG::SceneGraph::PICK_VERTEX, _event->pos(), node_idx, target_idx, &hit_point ) )
      {
        if( object->dataType( DATA_SPLATCLOUD ) )
        {
          splatCloudToggleSelection( PluginFunctions::splatCloud( object ), target_idx, hit_point, _currentType );
        }
      }
    }

    emit updatedObject( object->id(), UPDATE_SELECTION );
    emit createBackup ( object->id(), "Toggle Selection", UPDATE_SELECTION );
  }
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::slotSphereSelection( QMouseEvent *_event, double _radius, SelectionInterface::PrimitiveType _currentType, bool _deselect )
{
  // Return if none of the currently active types is handled by this plugin
  if( (_currentType & allSupportedTypes_) == 0 )
    return;

  unsigned int node_idx, target_idx;
  ACG::Vec3d hit_point;

  if( PluginFunctions::scenegraphPick( ACG::SceneGraph::PICK_VERTEX, _event->pos(), node_idx, target_idx, &hit_point) )
  {
    BaseObjectData *object = 0;

    if( PluginFunctions::getPickedObject( node_idx, object ) )
    {
      if( object->picked( node_idx ) && object->dataType( DATA_SPLATCLOUD ) )
      {
        splatCloudSphereSelection( PluginFunctions::splatCloud( object ), target_idx, hit_point, _radius, _currentType, _deselect );
      }

      emit updatedObject( object->id(), UPDATE_SELECTION );
      if ( _event->type() == QEvent::MouseButtonRelease )
        emit createBackup( object->id(), "Sphere Selection", UPDATE_SELECTION );
    }
  }
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::slotLassoSelection( QMouseEvent *_event, SelectionInterface::PrimitiveType _currentType, bool _deselect )
{
  // Return if none of the currently active types is handled by this plugin
  if( (_currentType & allSupportedTypes_) == 0 )
    return;

  if( _event->type() == QEvent::MouseButtonPress )
  {
    // Add picked point
    lasso_2Dpoints_.push_back( _event->pos() );
    return;
  }

  if( _event->type() == QEvent::MouseButtonDblClick )
  {    
    // Finish surface lasso selection
    if( lasso_2Dpoints_.size() > 2 )
    {
      QRegion region( lasso_2Dpoints_ );
      lassoSelect( region, _currentType, _deselect );
        }

        // Clear points
        lasso_2Dpoints_.clear();
    return;
  }
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::slotVolumeLassoSelection( QMouseEvent *_event, SelectionInterface::PrimitiveType _currentType, bool _deselect )
{
  // Return if none of the currently active types is handled by this plugin
  if( (_currentType & allSupportedTypes_) == 0 )
    return;

  if( _event->type() == QEvent::MouseButtonPress )
  {
    // Add point on viewing plane to selection polygon
    volumeLassoPoints_.append( _event->pos() );
        return;
    }

    if( _event->type() == QEvent::MouseButtonDblClick )
    {
    ACG::GLState &state = PluginFunctions::viewerProperties().glState();
    bool updateGL = state.updateGL();
    state.set_updateGL( false );

    QPolygon p( volumeLassoPoints_ );
    QRegion region = QRegion( p );

    SelectVolumeAction action( region, this, _currentType, _deselect, state );
    ACG::SceneGraph::traverse( PluginFunctions::getRootNode(), action );

    state.set_updateGL( updateGL );

    // Clear lasso points
    volumeLassoPoints_.clear();
  }
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::loadSelection( int _objectId, const QString &_filename )
{
  // Load ini file
  INIFile file;

  if( !file.connect( _filename, false ) )
  {
    emit log( LOGERR, QString( "Could not read file '%1'!" ).arg( _filename ) );
    return;
  }

  // Load selection from file
  loadIniFile( file, _objectId );
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::loadIniFile( INIFile &_ini, int _objectId )
{
  BaseObjectData *object = 0;
  if( !PluginFunctions::getObject( _objectId, object ) )
  {
    emit log( LOGERR, tr("Cannot find object for id ") + QString::number( _objectId ) + tr(" in saveFile") );
    return;
  }

  std::vector<int> ids;
  bool invert = false;

  bool updated_selection = false;
  bool updated_modeling_regions = false;

  QString sectionName = object->name();

  if( _ini.get_entry( ids, sectionName, "VertexSelection" ) )
  {
    invert = false;
    _ini.get_entry( invert, sectionName, "InvertVertexSelection" );

    if( invert )
    {
      selectAllVertices( object->id() );
      unselectVertices( object->id(), ids );
    }
    else
    {
      clearVertexSelection( object->id() );
      selectVertices( object->id(), ids );
    }

    updated_selection = true;
  }

  if( updated_modeling_regions )
  {
    emit updatedObject( object->id(), UPDATE_ALL );
    emit updateView();
  }
  else if( updated_selection )
  {
    emit updatedObject( object->id(), UPDATE_SELECTION );
    emit updateView();
  }

  if( updated_modeling_regions || updated_selection )
    emit createBackup( object->id(), "Load Selection", UPDATE_SELECTION );
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::saveIniFile( INIFile &_ini, int _objectId )
{
  BaseObjectData *object = 0;
  if( !PluginFunctions::getObject( _objectId, object ) )
  {
    emit log( LOGERR, tr("Cannot find object for id ") + QString::number( _objectId ) + tr(" in saveFile") );
    return;
  }

  std::vector<int> ids;

  // The objects section should already exist
  QString sectionName = object->name();
  if( !_ini.section_exists( sectionName ) )
  {
    emit log( LOGERR, tr("Cannot find object section id ") + QString::number( _objectId ) + tr(" in saveFile") );
    return;
  }

  _ini.add_entry( sectionName, "VertexSelection", getVertexSelection( object->id() ) );
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::slotLoadSelection( const INIFile &_file )
{
  // Iterate over all SplatCloud objects in the scene and save
  // the selections for all supported entity types
  for( PluginFunctions::ObjectIterator o_it( PluginFunctions::ALL_OBJECTS, DataType( DATA_SPLATCLOUD ) ); o_it != PluginFunctions::objectsEnd(); ++o_it)
  {
    // Read section for each object
    // Append object name to section identifier
    QString section = QString( "SplatCloudObjectSelection" ) + "//" + o_it->name();

    if( !_file.section_exists( section ) )
      continue;

    std::vector<int> ids;
    // Load vertex selection:
    _file.get_entry( ids, section, "VertexSelection" );
    selectVertices( o_it->id(), ids );
    ids.clear();

    emit updatedObject( o_it->id(), UPDATE_SELECTION );
    emit createBackup ( o_it->id(), "Load Selection", UPDATE_SELECTION );
  }
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::slotSaveSelection( INIFile &_file )
{
  // Iterate over all SplatCloud objects in the scene and save
  // the selections for all supported entity types
  for( PluginFunctions::ObjectIterator o_it( PluginFunctions::ALL_OBJECTS, DataType( DATA_SPLATCLOUD ) ); o_it != PluginFunctions::objectsEnd(); ++o_it)
  {
    // Create section for each object
    // Append object name to section identifier
    QString section = QString( "SplatCloudObjectSelection" ) + "//" + o_it->name();

    // Store vertex selection:
    _file.add_entry( section, "VertexSelection", getVertexSelection( o_it->id() ) );
  }
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::slotKeyShortcutEvent( int _key, Qt::KeyboardModifiers _modifiers )
{
  SelectionInterface::PrimitiveType type = 0u;
  emit getActivePrimitiveType( type );

  if( (type & allSupportedTypes_) == 0 )
  {
    // No supported type is active
    return;
  }

  bool targetsOnly = false;
  emit targetObjectsOnly( targetsOnly );
  PluginFunctions::IteratorRestriction restriction = (targetsOnly ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS);

  if( _key == Qt::Key_A && _modifiers == Qt::ControlModifier )
  {
    for( PluginFunctions::ObjectIterator o_it( restriction, DataType( DATA_SPLATCLOUD ) ); o_it != PluginFunctions::objectsEnd(); ++o_it )
    {
      if( o_it->visible() )
      {
        if( type & vertexType_ )
          selectAllVertices( o_it->id() );
        emit updatedObject( o_it->id(), UPDATE_SELECTION );
        emit createBackup ( o_it->id(), "Select All", UPDATE_SELECTION );
      }
    }
  }
  else if( _key == Qt::Key_C && _modifiers == Qt::NoModifier )
  {
    for( PluginFunctions::ObjectIterator o_it( restriction, DataType( DATA_SPLATCLOUD ) ); o_it != PluginFunctions::objectsEnd(); ++o_it )
    {
      if( o_it->visible() )
      {
        if( type & vertexType_ )
          clearVertexSelection( o_it->id() );
        emit updatedObject( o_it->id(), UPDATE_SELECTION );
        emit createBackup ( o_it->id(), "Clear Selection", UPDATE_SELECTION );
      }
    }
  }
  else if( _key == Qt::Key_I && _modifiers == Qt::NoModifier )
  {
    for( PluginFunctions::ObjectIterator o_it( restriction, DataType( DATA_SPLATCLOUD ) );  o_it != PluginFunctions::objectsEnd(); ++o_it )
    {
      if( o_it->visible() )
      {
        if( type & vertexType_ )
          invertVertexSelection( o_it->id() );
        emit updatedObject( o_it->id(), UPDATE_SELECTION );
        emit createBackup ( o_it->id(), "Invert Selection", UPDATE_SELECTION );
            }
    }
  }
  else if( _key == Qt::Key_Delete && _modifiers == Qt::NoModifier )
  {
    for( PluginFunctions::ObjectIterator o_it( restriction, DataType( DATA_SPLATCLOUD ) ); o_it != PluginFunctions::objectsEnd(); ++o_it )
    {
      if( o_it->visible() )
      {
        emit updatedObject( o_it->id(), addUpdateType("RestoreOriginalColors") );
        emit createBackup ( o_it->id(), "Pre Deletion", UPDATE_ALL );

        if( type & vertexType_ )
          deleteVertexSelection( o_it->id() );

        emit createBackup ( o_it->id(), "Delete Selection", UPDATE_ALL );
        emit updatedObject( o_it->id(), addUpdateType("RestoreColormodeColors") );

        emit updatedObject( o_it->id(), UPDATE_ALL );
      }
    }
  }
}


//----------------------------------------------------------------


void SplatCloudObjectSelectionPlugin::lassoSelect( QRegion &_region, PrimitiveType _primitiveType, bool _deselection )
{
  // <object id, primitive id>
  QList< QPair<unsigned int,unsigned int> > list;

  if( _primitiveType & vertexType_)
  {
    PluginFunctions::scenegraphRegionPick( ACG::SceneGraph::PICK_FRONT_VERTEX, _region, list );

    std::set<int> alreadySelectedObjects;

    for( int i=0; i<list.size(); ++i )
    {
      if( alreadySelectedObjects.count( list[i].first ) != 0 )
        continue;

      BaseObjectData *bod = 0;
      PluginFunctions::getPickedObject( list[i].first, bod );

      if( bod && (bod->dataType() == DATA_SPLATCLOUD ) )
      {
        IdList elements;
        for( int j=0; j<list.size(); ++j )
        {
          if( list[j].first == list[i].first )
          {
            elements.push_back( list[j].second );
          }
        }
        selectVertices( bod->id(), elements );
        alreadySelectedObjects.insert( list[i].first );
        emit updatedObject( bod->id(), UPDATE_SELECTION );
        emit createBackup ( bod->id(), "Lasso Selection", UPDATE_SELECTION );
      }
    }
  }
}


//----------------------------------------------------------------


/// Traverse the scenegraph and call the selection function for all SplatCloud nodes
bool SelectVolumeAction::operator()( BaseNode *_node )
{
  BaseObjectData *object = 0;
  if( PluginFunctions::getPickedObject( _node->id(), object ) )
  {    
    bool selected = false;
    if( object->dataType( DATA_SPLATCLOUD ) )
    {
      SplatCloud *sc = PluginFunctions::splatCloud( object );
      selected = plugin_->splatCloudVolumeSelection( sc, state_, &region_, type_, deselection_ );
        }

    if( selected )
    {
      emit plugin_->updatedObject( object->id(), UPDATE_SELECTION );
      emit plugin_->createBackup ( object->id(), "Lasso Selection", UPDATE_SELECTION );
    }
  }

  return true;
}


//----------------------------------------------------------------


Q_EXPORT_PLUGIN2( splatcloudobjectselectionplugin, SplatCloudObjectSelectionPlugin );
