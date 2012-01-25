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
*                                                                            *
*   $Revision: 11279 $                                                       *
*   $LastChangedBy: kremer $                                                 *
*   $Date: 2011-04-11 17:35:27 +0200 (Mon, 11 Apr 2011) $                    *
*                                                                            *
\*===========================================================================*/

#ifndef TYPEPOLYHEDRALMESH_HH
#define TYPEPOLYHEDRALMESH_HH

#include <QObject>

#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>
#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>

class TypePolyhedralMeshPlugin : public QObject, BaseInterface, LoadSaveInterface, TypeInterface, KeyInterface, LoggingInterface, ContextMenuInterface {
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(LoadSaveInterface)
Q_INTERFACES(TypeInterface)
Q_INTERFACES(KeyInterface)
Q_INTERFACES(LoggingInterface)
Q_INTERFACES(ContextMenuInterface)

signals:
    // Logging interface
    void log(Logtype _type, QString _message);
    void log(QString _message);

    // LoadSave Interface
    void emptyObjectAdded(int _id);

    // ContextMenuInterface
    void addContextMenuItem(QAction* /*_action*/, DataType /*_objectType*/, ContextMenuType /*_type*/);

private slots:

    void noguiSupported() {}

    void pluginsInitialized();

    void slot_change_shrinkage();

    void slotKeyEvent(QKeyEvent* _event);

    void slotObjectUpdated(int _identifier);

    void objectDeleted(int _identifier);

    void slot_update_planes_in_scenegraph_node(int _deletedObject = -1);

public:

    ~TypePolyhedralMeshPlugin() {
    }

    TypePolyhedralMeshPlugin();

    QString name() {
        return (QString("TypePolyhedralMesh"));
    }

    QString description() {
        return (QString(tr("Register Polyhedral Mesh Type")));
    }

    bool registerType();

public slots:

    QString version() {
        return QString("1.0");
    }

    // Type Interface
    int addEmpty();

    DataType supportedType() {
        return DATA_POLYHEDRAL_MESH;
    }
    

private:

    QString get_unique_name(PolyhedralMeshObject* _object);
};

#endif //TYPEPOLYHEDRALMESH_HH
