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

//=============================================================================
//
//  CLASS PropertyVisPlugin
//
//=============================================================================


#ifndef PROPERTYVISPLUGIN_HH
#define PROPERTYVISPLUGIN_HH


//== INCLUDES =================================================================

#include <QObject>
#include <QMenuBar>
#include <QSpinBox>
#include <QFile>
#include <QTextStream>
#include <vector>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>

#include <ACG/QtWidgets/QtExaminerViewer.hh>

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

#ifdef ENABLE_SKELETON_SUPPORT
  #include <ObjectTypes/Skeleton/BaseSkin.hh>
#endif

#include "PropertyVisToolbar.hh"
#include "PropertyNameListModel.hh"
#include "ObjectListItemModel.hh"

#include <ACG/Scenegraph/LineNode.hh>
#include <ACG/Utils/ColorGenerator.hh>

#include <stdexcept>

//== CLASS DEFINITION =========================================================

class VizException : public std::logic_error {
    public:
        VizException(const std::string &msg) : std::logic_error(msg) {}
};

class PropertyVisPlugin : public QObject, BaseInterface, ToolboxInterface, KeyInterface, ScriptInterface, MouseInterface, PickingInterface, LoggingInterface, INIInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(KeyInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(INIInterface)

signals:
  void updateView();
  void updatedObject(int, const UpdateType&);

  void log(Logtype, QString);
  void log(QString);
  
  // ToolboxInterface
  void addToolbox( QString  _name  , QWidget* _widget ,QIcon* _icon);

  //PickingInterface
  void addHiddenPickMode( const std::string& _mode );
  

public slots:

  // load and save properties in text format
  void loadSelectedProperty( );
  void saveSelectedProperty( );

  // script functions to change selection
  void selectObjectByID  ( int _id);
  void selectObjectByName( QString _name );
  void selectElementType ( QString _etype);
  void selectDataType    ( QString _dtype);

  // properties of selected mesh for scripting
  int n_vertices();
  int n_edges();
  int n_faces();
  int n_halfedges();
  
  // File Handling
  QString getLoadFilename();
  QString getSaveFilename();
  void    openFile(QString _filename);
  void    closeFile();
  bool    eofFile();

  // Readers
  double       readDouble();
  int          readInteger();
  unsigned int readUnsignedInteger();
  QString      readString();

  // Writers
  void writeDouble(double _d);
  void writeUnsignedInteger(unsigned int _i);
  void writeInteger(int _i);
  void writeString(QString _s);

  // access of selected property (_vector_idx is only used if a vector property is selected)
  void   setCurrentProp( int _element_idx, double _val, int _vector_idx = 0);
  double getCurrentProp( int _element_idx,              int _vector_idx = 0);

private:
		  
  // Property Handling
  template <typename PropertyType>
  void setVProp(QString _property, int _vidx, PropertyType _val);

  void setVPropV3D(QString _property, int _vidx, int _vector_idx, double _val);

  template <typename PropertyType>
  void setEProp  (QString _property, int _eidx, PropertyType _val);
  void setEPropV3D(QString _property, int _eidx, int _vector_idx, double _val);

  template <typename PropertyType>
  void setFProp  (QString _property, int _fidx, PropertyType _val);
  void setFPropV3D(QString _property, int _fidx, int _vector_idx, double _val);

  template <typename PropertyType>
  void setHProp  (QString _property, int _hidx, PropertyType _val);
  void setHPropV3D(QString _property, int _hidx, int _vector_idx, double _val);

private slots:

  // BaseInterface
  void slotObjectUpdated( int _identifier, const UpdateType& _type );
  void slotAllCleared();
  
  void slotCreateProperty();
  
  // initialization functions
  void initializePlugin();
  void pluginsInitialized();


  // toolbox slots
  void slotMeshChanged(int _index = -1);
  void slotVisualize();

  //INIInterface
  void loadIniFileOptions( INIFile& _ini);
  void saveIniFileOptions( INIFile& _ini);
  void saveOnExit( INIFile& _ini );
  
  // MouseInterface
  void slotMouseEvent( QMouseEvent* _event );
  
  //PickingInterface
  void slotPickModeChanged( const std::string& _mode);
  
  void updateGUI();

  void propertySelectionChanged();

  /// Does exactly what the name suggests.
  void slotDuplicateProperty();

  /// Does exactly what the name suggests.
  void slotRemoveProperty();

public :

  PropertyVisPlugin();
  ~PropertyVisPlugin() {};


  QString name() { return (QString("Property Visualization")); };
  QString description( ) { return (QString("Computes the PropertyVis of the the active Mesh")); }; 


private :
  bool pickModeActive_;
  
  ACG::Vec3d hit_point_;
  
  std::string lastPickMode_;
  Viewer::ActionMode lastActionMode_;
  
  // Widget for Toolbox
  PropertyVisToolbar* tool_;

  // Line Node
  std::vector<ACG::SceneGraph::LineNode*> lineNodes_;

  // Converts a qcolor to a mesh color
  OpenMesh::Vec4f convertColor(const QColor _icon);

private slots:
  
  void pickValue();
  
//templates
private:

  template< class MeshT >
  void visualizeVector( MeshT*   _mesh, const PropertyNameListModel::PROP_INFO &currentProp);

  //compute the point for visualization
  template< class MeshT >
  typename MeshT::Point halfedge_point(const typename MeshT::HalfedgeHandle _heh, const MeshT *_mesh);

  template< class MeshT >
  void visualizeVector_asStroke( MeshT*   _mesh, const PropertyNameListModel::PROP_INFO &currentProp);

  template< class MeshT >
  void visualizeVector_asColor( MeshT*   _mesh, const PropertyNameListModel::PROP_INFO &currentProp);

  template< class MeshT >
  void visualizeVector_onEdges( MeshT*   _mesh, const PropertyNameListModel::PROP_INFO &currentProp);

  template< class MeshT >
  void visualizeDouble( MeshT*   _mesh, const PropertyNameListModel::PROP_INFO &currentProp);
  
  template< class MeshT >
  void visualizeUnsignedInteger( MeshT*   _mesh, const PropertyNameListModel::PROP_INFO &currentProp);

  template< class MeshT >
  void visualizeInteger( MeshT*   _mesh, const PropertyNameListModel::PROP_INFO &currentProp);
  
  template< class MeshT >
  void visualizeBool( MeshT*   _mesh, const PropertyNameListModel::PROP_INFO &currentProp);
  
  template< class MeshT >
  void visualizeSkinWeights( MeshT*   _mesh, int _boneId, const PropertyNameListModel::PROP_INFO &currentProp);

  template< class MeshT >
  void updatePropertyList(MeshT* _mesh);
  
  template< class MeshT >
  void getPropertyValue( MeshT* _mesh, int _id, unsigned int _face, ACG::Vec3d& _hitPoint );
  
  /// Get closest vertex index
  template< class MeshT >
  int getClosestVertex(MeshT* _mesh, int _face_idx);
  
  /// Get closest edge index
  template< class MeshT >
  int getClosestEdge(MeshT* _mesh, int _face_idx);

  /// Get closest halfedge index
  template< class MeshT >
  int getClosestHalfEdge(MeshT* _mesh, int _face_idx);


  template< class MeshT >
  void addNewProperty(MeshT* _mesh);

  /// Does exactly what the name suggests.
  template<typename MeshT>
  void visualizeProperty(MeshT *mesh, const PropertyNameListModel::PROP_INFO &currentProp);

  /// Does exactly what the name suggests.
  template<typename MeshT>
  void duplicateProperty(MeshT *mesh, const PropertyNameListModel::PROP_INFO &currentProp);

  /// Does exactly what the name suggests.
  template<typename MeshT>
  void removeProperty(MeshT *mesh, const PropertyNameListModel::PROP_INFO &currentProp);

  /// Does exactly what the name suggests.
  template<typename MeshT, float (*ScalarFn)(const typename MeshT::Point &, const typename MeshT::Point &)>
  void visualizeVectorFieldDifference(MeshT *mesh, const PropertyNameListModel::PROP_INFO &p1,
                                      const PropertyNameListModel::PROP_INFO &p2);

  BaseObjectData* getSelectedObject();

private:

  // file for reading
  QFile        file_;
  QTextStream* file_stream_;

  // Color Generator
  ACG::ColorGenerator colorGenerator_;
  PropertyNameListModel propertyNameListModel_;
  ObjectListItemModel objectListItemModel_;
};

#if defined(INCLUDE_TEMPLATES) && !defined(PROPERTYVISPLUGIN_CC)
#define PROPERTYVISPLUGIN_TEMPLATES
#include "PropertyVisPluginT.cc"
#endif
//=============================================================================
#endif // PROPERTYVISPLUGIN_HH defined
//=============================================================================

