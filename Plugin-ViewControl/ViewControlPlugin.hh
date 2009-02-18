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

    void addContextMenu(QMenu* _menu ,DataType _objectType , ContextMenuType _type );

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

    /** Actions in the draw Context Menu
     */
    std::vector< QAction * > drawMenuActions_;

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
     */
    void setShader(int _id, QString _drawMode, QString _vertexShader, QString _fragmentShader );

    /** Sets a Shader from the Shader directory of OpenFlipper ( Name of the one given inside the ini file )
     *
     */
    void setShader(int _id, QString _drawMode, QString _name );

};

#endif //ViewControlPlugin_HH
