/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#ifndef ViewControlPlugin_HH
#define ViewControlPlugin_HH

#include <QObject>
#include <QMenuBar>
#include <QGroupBox>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/PolyVolMesh/PolyVolMesh.hh>
#include <ObjectTypes/TetMeshCGAL/TetMeshCGAL.hh>
#include "shaderWidget.hh"

struct ShaderInfo {
      QString name;
      QString description;
      QString details;
      QString version;
      QString example;

      QString directory;

      QString vertexShader;
      QString fragmentShader;

      QString pickVertexShader;
      QString pickFragmentShader;

      bool        hasUniforms;
      QStringList uniforms;
      QStringList uniformTypes;
      QStringList uniformsDefault;
      QStringList uniformsMax;
      QStringList uniformsMin;
};

class ViewControlPlugin : public QObject, BaseInterface , PickingInterface, LoggingInterface, ContextMenuInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(ContextMenuInterface)

  signals:
    void updateView();
    void updatedObject(int);

    void addHiddenPickMode( const std::string _mode );

    void log(Logtype _type, QString _message);
    void log(QString _message);

    // ContextMenuInterface
    void addContextMenuItem(QAction* _action , ContextMenuType _type);
    void addContextMenuItem(QAction* _action , DataType _objectType , ContextMenuType _type );

    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                            QStringList _parameters, QStringList _descriptions);

  private slots:

    void pluginsInitialized();

    void contextMenuTriggered(QAction* _action);

    void slotUpdateContextMenu( int _objectId );

  public :

    ~ViewControlPlugin() {};


    QString name() { return (QString("ViewControl")); };
    QString description( ) { return (QString("Control the current or global visualization settings")); };


  private:
    QMenu* viewControlMenu_;

    /** Last object id for context menu. Is -1 if no object was selected
     */
    int lastObjectId_;

    /** Active draw Modes for the last clicked object and its children
     */
    unsigned int activeDrawModes_;

    /** Available draw Modes for the last clicked object and its children
     */
    unsigned int availDrawModes_;

    ShaderWidget* shaderWidget_;

    /** List of available shaders
    */
    std::vector <ShaderInfo> shaderList_;

  private slots:
    QString version() { return QString("1.0"); };

    // This slot is called when a custom draw mode is selected from the context menu
    void slotDrawModeSelected( QAction * _action );

    // This slot is called when the user clicks on an item in the shaderWidget
    void slotShaderClicked( QListWidgetItem * _item );

    // This slot is called when the user doubleclicks on an item in the shaderWidget
    void slotShaderDoubleClicked( QListWidgetItem * _item );

    // slot for setting a shader from the widget
    void slotSetShader();

    // slot is called when the data of the uniforms-table changes
    void itemChanged(QTableWidgetItem* item);

  private:
    // Update the list of available shaders
    void updateShaderList();

    // initialize the shader Widget
    void initShaderWidget();

    // Announce local slots
    void setDescriptions();


  public slots:
    /// For meshes returns if the selection for this object is visible
    bool selectionVisible( int _id );

    /// Allows to enable/disable visualization of the objects selection for meshes
    void showSelection( int _id , bool _state );

    /// For meshes returns if the modeling areas for this object is visible
    bool modelingAreasVisible( int _id );

    /// Allows to enable/disable visualization of the objects modeling area for meshes
    void showModelingAreas( int _id , bool _state  );

    /** Sets a shader for the object and the given drawMode( can be a combination of draw modes )
     *  @param _id Object Id
     *  @param _drawMode ; seperated list of drawmodes used by the shader
     *  @param _vertexShader   File with the vertex shader code
     *  @param _fragmentShader File with the fragment shader code
     *  @param _pickVertexShader   File with the picking vertex shader code
     *  @param _pickFragmentShader File with the picking fragment shader code
     */
    void setShader(int _id, QString _drawMode, QString _vertexShader, QString _fragmentShader,
                   QString _pickVertexShader = "", QString _pickFragmentShader = "");

    /** Sets a Shader from the Shader directory of OpenFlipper ( Name of the one given inside the ini file )
     *
     */
    void setShader(int _id, QString _drawMode, QString _name );

    /// get information about available uniforms for a given shader
    QStringList getUniforms(QString _shader);

    QString getUniformType(QString _shader, QString _uniform );
    QString getUniformDefault(QString _shader, QString _uniform );
    QString getUniformMin(QString _shader, QString _uniform );
    QString getUniformMax(QString _shader, QString _uniform );

    /// set the value of a uniform in a shader for a specific drawMode
    void setUniform(int _objID, unsigned int _drawMode, QString _shader, QString _uniform, QString _value );


    /// Set the viewing direction
    void setViewingDirection( Vector _direction, Vector _upvector , int _viewer = PluginFunctions::ALL_VIEWERS );

    /// Rotate Scene
    void rotate( Vector _axis, double _angle, Vector _center , int _viewer = PluginFunctions::ALL_VIEWERS );

    /// translate Scene
    void translate( Vector _vec , int _viewer = PluginFunctions::ALL_VIEWERS );

    /// Set the draw mode for a viewer
    void setDrawMode(QString _mode, int _viewer = PluginFunctions::ALL_VIEWERS );

    /// Get a viewers viewing direction
    Vector viewingDirection( int _viewer = PluginFunctions::ACTIVE_VIEWER );

    /// get a viewers up vector
    Vector upVector( int _viewer = PluginFunctions::ACTIVE_VIEWER );

    /// get a viewers eye Position
    Vector eyePosition( int _viewer = PluginFunctions::ACTIVE_VIEWER );

    /// Get the scene center
    Vector sceneCenter( int _viewer = PluginFunctions::ACTIVE_VIEWER );

    /// Set the scene center
    void setSceneCenter( Vector _center, int _viewer = PluginFunctions::ALL_VIEWERS );

    /// Enable or disable Backface culling
    void enableBackfaceCulling( bool _state , int _viewer = PluginFunctions::ALL_VIEWERS );


    void setEyePosition(Vector _eye);

    /// Change View on all viewers to view whole scene
    void viewAll();

    /// Change View on given Viewer to view whole scene
    void viewAll(int _viewer);

    /// Change View on given Viewer to view home position
    void viewHome();

    /// Change View on all Viewers to view home position
    void viewHome(int _viewer);



    /// Change Viewer to orthographicProjection
    void orthographicProjection();

    /// Change all Viewers to orthographicProjection
    void orthographicProjection(int _viewer);



    /// Change Viewer to perspectiveProjection
    void perspectiveProjection();

    /// Change all Viewers to perspectiveProjection
    void perspectiveProjection(int _viewer);

};

#endif //ViewControlPlugin_HH
