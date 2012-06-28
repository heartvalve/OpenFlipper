//=============================================================================
//
//  CLASS CatmullClarkPlugin
//
//=============================================================================


#ifndef CATMULLCLARKPLUGIN_HH
#define CATMULLCLARKPLUGIN_HH


//== INCLUDES =================================================================

#include <QObject>
#include <QMenuBar>
#include <QSpinBox>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>

#include <ACG/QtWidgets/QtExaminerViewer.hh>

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

#include "CatmullClarkToolbar.hh"
//#include "CatmullClarkPerObjectDataT.hh"
#include "CatmullClarkT.hh"


//== CLASS DEFINITION =========================================================


class CatmullClarkPlugin : public QObject, BaseInterface, ToolboxInterface, KeyInterface, ScriptInterface, MouseInterface, PickingInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(KeyInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(PickingInterface)


  // typedef for easy access
  //typedef ACG::CatmullClarkT<TriMesh> CatmullClark;
  //typedef CatmullClarkPerObjectDataT<TriMesh>   POD;

signals:
  void updateView();
  void updatedObject(int);
  
  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget ); 


private slots:

  // initialization functions
  void initializePlugin();
  void pluginsInitialized();


  // compute
  void slotCompute();


public :

  ~CatmullClarkPlugin() {};

  QString name() { return (QString("CatmullClark")); };
  QString description( ) { return (QString("Computes the CatmullClark of the the active Mesh")); }; 

private :

  // return name of per object data
  //const char * pod_name() { return "CATMULLCLARK_PER_OBJECT_DATA";}

  // get CatmullClark object for a given object
  //CatmullClark* get_catmullclark_object( BaseObjectData* _object );
  


private :
  /// Widget for Toolbox
  CatmullClarkToolbar* tool_;
};


//=============================================================================
#endif // CATMULLCLARKPLUGIN_HH defined
//=============================================================================

