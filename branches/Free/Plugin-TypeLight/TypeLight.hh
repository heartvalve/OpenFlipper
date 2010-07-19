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
 *   $Revision: 7683 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-11-30 13:51:21 +0100 (Mo, 30. Nov 2009) $                   *
 *                                                                           *
\*===========================================================================*/




#ifndef TYPELIGHTPLUGIN_HH
#define TYPELIGHTPLUGIN_HH

#include <QObject>

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/Light/Light.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>

class TypeLightPlugin : public QObject, BaseInterface, LoadSaveInterface, LoggingInterface, TypeInterface, MouseInterface, ToolbarInterface
{
   Q_OBJECT
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(LoadSaveInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(TypeInterface)
   Q_INTERFACES(MouseInterface)
   Q_INTERFACES(ToolbarInterface)

  signals:
    // Logging interface
    void log(Logtype _type, QString _message);
    void log(QString _message);
    
    // LoadSave Interface
    void emptyObjectAdded( int _id );
    void deleteObject(int _id);
    
    // BaseInterface
    void updatedObject(int _id, const UpdateType _type);
    
    // ToolbarInterface
    void getToolBar( QString _name, QToolBar*& _toolbar );
    
  private slots:

    void noguiSupported( ) {} ;
    
    // Is called whenever the scene is cleared
    void slotAllCleared();
    
    // Adds three default light sources to scene
    void addDefaultLights();
    // ... and removes them
    void removeDefaultLights();
    
    // Load/Save Interface
    void addedEmptyObject(int _id);
    void objectDeleted(int _id);
    
    // MouseInterface
    void slotMouseEventLight( QMouseEvent* _event);
    
    //-----------------------------------------------------------
    
    void setLightMode();
    
  private:
        
    /// Rotate light sources
    void rotateLights(ACG::Vec3d& _axis, double _angle);
    
    /// Update light sources' positions
    void updateLights();
    
    bool mapToSphere(const QPoint& _v2D, ACG::Vec3d& _v3D, int _width, int _height) const;
        
  public:

     ~TypeLightPlugin() {};
     TypeLightPlugin();

     QString name() { return (QString("TypeLight")); };
     QString description( ) { return (QString(tr("Register Light type"))); };
     
     bool registerType();

  public slots:

    // Base Interface
    QString version() { return QString("1.0"); };
    
    // Type Interface
    int addEmpty();
    DataType supportedType() { return DATA_LIGHT; }; 
    
    void pluginsInitialized();
    
    // Add default light
    int addDefaultLight(QString _name);
    
  private slots:
      
    void allLights(bool _b);
    void targetLights(bool _b);
    
  private:
    
    /// Return unique name for object
    QString get_unique_name(LightObject* _object);
    
    /// Find depth of nearest light source
    float findDepth();
    
    /// Count light sources in scene
    std::vector<int> lightSources_;
    
    bool defaultLights_;
    
    // Matrix for rotating light position
    ACG::GLMatrixd light_matrix_;
    
    // Button for light mode
    QToolButton* lightButton_;
    
    // Context menu which allows to switch between target and all light transformation
    QMenu* contextmenu_;
    
    // True if only target lights should be transformed
    bool onlyTargets_;

    // Mouse interactions
    QPoint      lastPoint2D_;
    ACG::Vec3d  lastPoint3D_;
    bool        lastPoint_hitSphere_;
    // depth of plane olong which objects are translated
    // if middle mouse button is pressed
    float       planeDepth_;
    ACG::Vec3d  transVec_;
    bool        rotation_;
    
    double ratioTrackballs_;
};

#endif //TYPELIGHTPLUGIN_HH
