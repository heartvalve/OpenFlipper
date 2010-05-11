#ifndef MOUSEANDKEYPLUGIN_HH
#define MOUSEANDKEYPLUGIN_HH

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/common/Types.hh>

class MouseAndKeyPlugin: public QObject,
		BaseInterface,
		MouseInterface,
		PickingInterface,
		ContextMenuInterface,
		ToolboxInterface,
		KeyInterface,
		LoggingInterface {
	Q_OBJECT
	Q_INTERFACES(BaseInterface)
	Q_INTERFACES(MouseInterface)
	Q_INTERFACES(PickingInterface)
	Q_INTERFACES(KeyInterface)
	Q_INTERFACES(ContextMenuInterface)
	Q_INTERFACES(ToolboxInterface)
	Q_INTERFACES(LoggingInterface)

	signals:

	//BaseInterface
	void updateView();
	void updatedObject(int _id);
	//LoggingInterface
	void log(Logtype _type, QString _message);
	void log(QString _message);
	//ContextMenuInterface
	void addContextMenuItem(QAction* _action , ContextMenuType _type);
	void addContextMenuItem(QAction* _action , DataType _objectType , ContextMenuType _type );
	//PickingInterface
	void addPickMode(const std::string _mode);
	void addHiddenPickMode(const std::string _mode);
	//KeyInterface
	void registerKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description, bool _multiUse = false);
        // ToolboxInterface
        void addToolbox( QString _name  , QWidget* _widget );  

	private slots:

	// BaseInterface
	void initializePlugin();
	void pluginsInitialized();
	//MouseInterface
	void slotMouseEvent( QMouseEvent* _event );
	//KeyInterface
	void slotKeyEvent( QKeyEvent* _event );
	//PickingInterface
	void slotPickModeChanged( const std::string& _mode);

	public:

	// BaseInterface
	QString name() {return (QString("Mouse and Keyboard Plugin"));};
	QString description() {return (QString("Shows some basic mouse and key embedding"));};

	private:

	// Transform geometry with given Matrix
	template <typename MeshT>
	void transformMesh(ACG::Matrix4x4d _mat , MeshT& _mesh );

	// The context menu action
	QMenu* contextMenuEntry_;

	// The toolbox widget and the button in it
	QWidget* tool_;
	QPushButton* pickButton_;

	// Last picked object
	int activeObject_;

	// Rotation axes
	ACG::Vec3d axis_x_;
	ACG::Vec3d axis_y_;

	private slots:

	// Is called each time the button in the toolbox is clicked
	void slotButtonClicked();

	// Is called if context menu item has been selected
	void contextMenuItemSelected(QAction* _action);

	public slots:
	QString version() { return QString("1.0"); };
};

#endif //MOUSEANDKEYPLUGIN_HH
