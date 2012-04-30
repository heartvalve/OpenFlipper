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
//  CLASS SplatCloudObjectSelectionPlugin
//
//================================================================


#ifndef SPLATCLOUDOBJECTSELECTIONPLUGIN_HH
#define SPLATCLOUDOBJECTSELECTIONPLUGIN_HH


//== INCLUDES ====================================================


#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/SelectionInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/INIFile/INIFile.hh>

#include <ObjectTypes/SplatCloud/SplatCloud.hh>


//== CLASS DEFINITION ============================================


class SplatCloudObjectSelectionPlugin : public QObject, BaseInterface, BackupInterface, LoggingInterface, SelectionInterface, LoadSaveInterface, INIInterface, KeyInterface, ScriptInterface
{
	Q_OBJECT
	Q_INTERFACES( BaseInterface      )
	Q_INTERFACES( BackupInterface    )
	Q_INTERFACES( LoggingInterface   )
	Q_INTERFACES( SelectionInterface )
	Q_INTERFACES( LoadSaveInterface  )
	Q_INTERFACES( INIInterface       )
	Q_INTERFACES( KeyInterface       )
	Q_INTERFACES( ScriptInterface    )

public:
	/// Default constructor
	SplatCloudObjectSelectionPlugin() : vertexType_( 0 ), allSupportedTypes_( 0 ) { }

	friend class SelectVolumeAction;

signals:

	//-- Base Interface --
	void updateView();
	void updatedObject( int _identifier, const UpdateType& _type );
	void nodeVisibilityChanged( int _identifier );
	void setSlotDescription( QString _slotName, QString _slotDescription, QStringList _parameters, QStringList _descriptions );

	//-- Backup Interface --
	void createBackup( int _objectId, QString _name, UpdateType _type = UPDATE_ALL );

	//-- Logging Interface --
	void log( Logtype _type, QString _message );
	void log( QString _message );

	//-- Selection Interface --
	void addSelectionEnvironment( QString _modeName, QString _description, QIcon _icon, QString &_handleName );
	void registerType( QString _handleName, DataType _type );
	void addPrimitiveType( QString _handleName, QString _name, QIcon _icon, SelectionInterface::PrimitiveType &_typeHandle );
	void addSelectionOperations( QString _handleName, QStringList _operationsList, QString _category, SelectionInterface::PrimitiveType _type = 0u );

	void showToggleSelectionMode     ( QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes );
	void showSphereSelectionMode     ( QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes );
	void showLassoSelectionMode      ( QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes );
	void showVolumeLassoSelectionMode( QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes );

	void getActiveDataTypes( SelectionInterface::TypeList &_types );
	void getActivePrimitiveType( SelectionInterface::PrimitiveType &_type );
	void targetObjectsOnly( bool &_targetsOnly );

	void registerKeyShortcut( int _key, Qt::KeyboardModifiers _modifiers = Qt::NoModifier );

	//-- LoadSave Interface --
	void deleteObject( int _objectId );

	//-- Script Interface --
	void scriptInfo( QString _functionName );

public slots:

	//-- Selection Interface --
	void loadSelection( int _objId, const QString &_filename );

private slots:

	//-- INI Interface --
	void loadIniFile( INIFile &_ini, int _id );
	void saveIniFile( INIFile &_ini, int _id );

	//-- Base Interface --
	void initializePlugin();
	void pluginsInitialized();
	void noguiSupported() { };

	//-- Selection Interface --
	void slotSelectionOperation( QString _operation );

	void slotToggleSelection     ( QMouseEvent *_event,                 SelectionInterface::PrimitiveType _currentType, bool _deselect );
	void slotSphereSelection     ( QMouseEvent *_event, double _radius, SelectionInterface::PrimitiveType _currentType, bool _deselect );
	void slotLassoSelection      ( QMouseEvent *_event,                 SelectionInterface::PrimitiveType _currentType, bool _deselect );
	void slotVolumeLassoSelection( QMouseEvent *_event,                 SelectionInterface::PrimitiveType _currentType, bool _deselect );

	void slotLoadSelection( const INIFile &_file );
	void slotSaveSelection( INIFile &_file );

	void slotKeyShortcutEvent( int _key, Qt::KeyboardModifiers _modifiers );

public:

	//-- Base Interface --
	QString name() { return QString( tr("SplatCloud Object Selection") ); }

	QString description() { return QString( tr("Allows to select parts of SplatCloud Objects") ); }

	//===========================================================================
	/** @name Private methods
	* @{ */
	//===========================================================================
private:

	/// Set descriptions for local public slots
	void updateSlotDescriptions();

	/// Set color for selection
	void setColorForSelection( const int _objectId, const PrimitiveType _primitiveType );

	/** @} */

	//===========================================================================
	/** @name Private slots
	* @{ */
	//===========================================================================
private slots:

	/** @} */

public slots:

	QString version() { return QString( "1.0" ); }

	// Is vertex type active? (for use in plugins that need SplatCloud selection)
	bool vertexTypeActive()
	{
		SelectionInterface::PrimitiveType t;
		emit getActivePrimitiveType( t );
		return (t & vertexType_) > 0;
	}

	//===========================================================================
	/** @name Scriptable slots
	* @{ */
	//===========================================================================
public slots:

	//==========================================
	// VERTEX OPERATIONS
	//==========================================

	void   selectVertices         ( int _objectId, IdList _vertexList );             /// Select given vertices
	void   unselectVertices       ( int _objectId, IdList _vertexList );             /// Unselect given vertices
	void   selectAllVertices      ( int _objectId );                                 /// Select all vertices
	void   clearVertexSelection   ( int _objectId );                                 /// Unselect all vertices
	void   invertVertexSelection  ( int _objectId );                                 /// Invert the current vertex selection
	IdList getVertexSelection     ( int _objectId );                                 /// Return a list of all selected vertices
	void   deleteVertexSelection  ( int _objectId );                                 /// Delete vertices that are currently selected
	void   colorizeVertexSelection( int _objectId, int _r, int _g, int _b, int _a ); /// Colorize the vertex selection

	//===========================================================================

	void lassoSelect( QRegion &_region, PrimitiveType _primitiveType, bool _deselection ); /// Lasso selection tool

	/** @} */

	//===========================================================================
	/** @name Template Functions
	* @{ */
	//===========================================================================

private:

	bool splatCloudDeleteSelection    ( SplatCloud *_splatCloud,                                                      PrimitiveType _primitiveType                                  ); /// Delete all selected elements of a SplatCloud
	void splatCloudToggleSelection    ( SplatCloud *_splatCloud, uint _index, ACG::Vec3d &_hit_point,                 PrimitiveType _primitiveType                                  ); /// Toggle SplatCloud selection
	void splatCloudSphereSelection    ( SplatCloud *_splatCloud, uint _index, ACG::Vec3d &_hit_point, double _radius, PrimitiveType _primitiveTypes, bool _deselection              ); /// Use the event to paint selection with a sphere
	bool splatCloudVolumeSelection    ( SplatCloud *_splatCloud, ACG::GLState &_state, QRegion *_region,              PrimitiveType _primitiveTypes, bool _deselection              ); /// Surface volume selection tool
	void splatCloudColorizeSelection  ( SplatCloud *_splatCloud,                                                      PrimitiveType _primitiveTypes, int _r, int _g, int _b, int _a ); /// Colorize the selection

	/** @} */

	//===========================================================================
	/** @name Member variables
	* @{ */
	//===========================================================================

private:

	QString                           environmentHandle_; /// Handle to selection environment
	SelectionInterface::PrimitiveType vertexType_;        /// Primitive type handle
	SelectionInterface::PrimitiveType allSupportedTypes_; /// Primitive type handle
	QPolygon                          lasso_2Dpoints_;    /// Used for lasso selection tool
	QVector<QPoint>                   volumeLassoPoints_; /// Used for volume lasso tool

	/** @} */
};


/// Traverse the scenegraph and call the selection function for all SplatCloud nodes
class SelectVolumeAction
{
public:

	SelectVolumeAction( QRegion &_region, SplatCloudObjectSelectionPlugin *_plugin, unsigned int _type, bool _deselection, ACG::GLState &_state ) : 
		state_(_state), region_(_region), plugin_(_plugin), type_(_type), deselection_(_deselection) { }

	void enter( BaseNode * /*_node*/ ) { }
	void leave( BaseNode * /*_node*/ ) { }

	bool operator()( BaseNode *_node );

private:
	ACG::GLState                    &state_;
	QRegion                         &region_;
	SplatCloudObjectSelectionPlugin *plugin_;
	unsigned int                    type_;
	bool                            deselection_;
};


//================================================================


#endif // SPLATCLOUDOBJECTSELECTIONPLUGIN_HH
