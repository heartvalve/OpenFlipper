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
 *   $Revision$                                                       *
 *   $Author$                                                       *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#ifndef POLYLINESELECTIONPLUGIN_HH
#define POLYLINESELECTIONPLUGIN_HH

#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/SelectionInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/INIFile/INIFile.hh>

#include <ObjectTypes/PolyLine/PolyLine.hh>

class PolyLineSelectionPlugin : public QObject, BaseInterface, KeyInterface,
            INIInterface, BackupInterface, LoggingInterface, ScriptInterface, SelectionInterface
{
    Q_OBJECT
    Q_INTERFACES(BaseInterface)
    Q_INTERFACES(KeyInterface)
    Q_INTERFACES(INIInterface)
    Q_INTERFACES(BackupInterface)
    Q_INTERFACES(LoggingInterface)
    Q_INTERFACES(ScriptInterface)    
    Q_INTERFACES(SelectionInterface)

public:
    
    /// IdList type
    typedef std::vector<int> IdList;
    
    /// Default constructor
    PolyLineSelectionPlugin();

    /// Default destructor
    ~PolyLineSelectionPlugin();
    
signals:

    // BaseInterface
    void updateView();
    void updatedObject(int, const UpdateType&);
    void nodeVisibilityChanged(int _identifier);
    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                            QStringList _parameters, QStringList _descriptions);

    // LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);

    // ScriptInterface
    void scriptInfo(QString _functionName);

    // SelectionInterface
    void addSelectionEnvironment(QString _modeName, QString _description, QIcon _icon, QString& _handleName);
    void registerType(QString _handleName, DataType _type);
    void addPrimitiveType(QString _handleName, QString _name, QIcon _icon, SelectionInterface::PrimitiveType& _typeHandle);
    void addSelectionOperations(QString _handleName, QStringList _operationsList, QString _category, SelectionInterface::PrimitiveType _type = 0u);
    void showToggleSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes);
    void showVolumeLassoSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes);

    void getActiveDataTypes(SelectionInterface::TypeList& _types);
    void getActivePrimitiveType(SelectionInterface::PrimitiveType& _type);
    void targetObjectsOnly(bool& _targetsOnly);
    
    void registerKeyShortcut(int _key, Qt::KeyboardModifiers _modifiers = Qt::NoModifier);

private slots:

    // INIInterface
    void loadIniFile(INIFile& _ini, int _id);
    void saveIniFile(INIFile& _ini, int _id);

    // BaseInterface
    void initializePlugin();
    void pluginsInitialized();
    void noguiSupported() {};

    // SelectionInterface
    void slotSelectionOperation(QString _operation);
    void slotToggleSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect);
    void slotVolumeLassoSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect);

    void slotLoadSelection(const INIFile& _file);
    void slotSaveSelection(INIFile& _file);
    
    void slotKeyShortcutEvent(int _key, Qt::KeyboardModifiers _modifiers);

public slots:

    // SelectionInterface
    void loadSelection(int _objId, const QString& _filename);

public:

    // BaseInterface
    QString name() {
        return (QString(tr("PolyLine Selection")));
    };
    
    QString description() {
        return (QString(tr("Allows to select PolyLines")));
    };

    //===========================================================================
    /** @name Private methods
    * @{ */
    //===========================================================================
private:
    
    /// Set slot descriptions for scripting functions
    void updateSlotDescriptions();
    
    /** @} */
    
public slots:

    QString version() {
        return QString("1.0");
    };

    //===========================================================================
    /** @name Scriptable slots
    * @{ */
    //===========================================================================
    
    /// Select all vertices of a skeleton
    void selectAllVertices(int _objectId);
    
    /// Deselect all vertices of a skeleton
    void deselectAllVertices(int _objectId);
    
    /// Invert vertex selection
    void invertVertexSelection(int _objectId);
    
    /// Delete selected vertices
    void deleteSelectedVertices(int _objectId);
    
    /// Select specific vertices of a skeleton
    void selectVertices(int _objectId, const IdList& _ids, bool _deselect = false);
    
    /// Get current vertex selection
    IdList getVertexSelection(int _objectId);
    
    //===========================================================================
    
    /// Select all edges of a skeleton
    void selectAllEdges(int _objectId);

    /// Deselect all edges of a skeleton
    void deselectAllEdges(int _objectId);

    /// Invert edge selection
    void invertEdgeSelection(int _objectId);

    /// Delete selected edges
    void deleteSelectedEdges(int _objectId);

    /// Select specific edges of a skeleton
    void selectEdges(int _objectId, const IdList& _ids, bool _deselect = false);

    /// Get current edge selection
    IdList getEdgeSelection(int _objectId);
    
    /** @} */

    //===========================================================================
    /** @name Member variables
    * @{ */
    //===========================================================================

private:

    /// Handle to selection environment
    QString environmentHandle_;

    /// Primitive type handles:
    PrimitiveType vertexType_;
    PrimitiveType edgeType_;
    
    PrimitiveType allSupportedTypes_;
    
    /// Keep volume lasso points
    QVector<QPoint> volumeLassoPoints_;

    /** @} */

};

//=============================================================================
#endif // POLYLINESELECTIONPLUGIN_HH defined
//=============================================================================
