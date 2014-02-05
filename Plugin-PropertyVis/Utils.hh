/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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

#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <iostream>
#include <set>
#include <typeinfo>

#include <QObject>

#include <QMessageBox>


/*! \class TypeInfoWrapper
 *  \brief Wraps the information of a type.
 *
 * This class is used to handle different property types. It provides a human readable
 * string representation of the type and can also be used to find out which type a
 * property has.
 */
class TypeInfoWrapper {
    public:
        TypeInfoWrapper(const std::type_info & ti, const char *friendlyName) : ti(&ti), friendlyName(friendlyName) {}
        TypeInfoWrapper(const std::type_info & ti) : ti(&ti),friendlyName("") {}

        operator const std::type_info *() const { return ti; }
        operator const std::type_info &() const { return *ti; }
        operator const char *() const { return friendlyName; }

        const std::type_info *operator->() const { return ti; }
        const std::type_info &get() const { return *ti; }
        const char *getName() const { return friendlyName; }

        bool operator==(const TypeInfoWrapper & other) const {
            /*
             * We compare name strings, not the type ids themselves because
             * the same type from different SOs will give different type ids.
             */
            return strcmp(ti->name(), other.ti->name()) == 0;
        }

        bool operator<(const TypeInfoWrapper & other) const {
            return strcmp(ti->name(), other.ti->name()) < 0;
        }

    private:
        const std::type_info *ti;
        const char *friendlyName;
};

/*! \class PropertyInfo
 *  \brief Cellection of information about a property.
 *
 * This class combines all the interesting information about a property. It stores
 * the entity type (Vertex, Edge, ...), the properties name and its type.
 */
class PropertyInfo {
    public:
        enum ENTITY_FILTER {
            EF_ANY      = 0xFF,
            EF_FACE     = 0x01,
            EF_EDGE     = 0x02,
            EF_HALFEDGE = 0x04,
            EF_VERTEX   = 0x08,
            EF_HALFFACE = 0x10,
            EF_CELL     = 0x20
        };

        /// Returns a symbol representation for an entity
        static const char* entity2str(ENTITY_FILTER entity);

        PropertyInfo(const std::string &propName, const TypeInfoWrapper &typeinfo, ENTITY_FILTER entity) :
            propName_(propName), typeinfo_(typeinfo), entity(entity) {}

        QString toString() const {
            return QObject::tr("%3 %1 : %2").arg(propName_.c_str()).arg(friendlyTypeName()).arg(QString::fromUtf8(entity2str(entity)));
        }

        bool operator==(const PropertyInfo &rhs) const {
            return propName_ == rhs.propName_ && typeinfo_ == rhs.typeinfo_ && entity == rhs.entity;
        }

        bool operator<(const PropertyInfo &rhs) const {
            if (entity != rhs.entity) return entity < rhs.entity;

            int result = propName_.compare(rhs.propName_);
            if (result) return result < 0;

            return typeinfo_ < rhs.typeinfo_;
        }

        inline bool isCellProp() const     { return entity == EF_CELL;     }
        inline bool isFaceProp() const     { return entity == EF_FACE;     }
        inline bool isHalffaceProp() const { return entity == EF_HALFFACE; }
        inline bool isEdgeProp() const     { return entity == EF_EDGE;     }
        inline bool isHalfedgeProp() const { return entity == EF_HALFEDGE; }
        inline bool isVertexProp() const   { return entity == EF_VERTEX;   }

        inline const std::string &propName() const     { return propName_;           }
        inline const char *friendlyTypeName() const    { return typeinfo_.getName(); }
        inline const TypeInfoWrapper &typeinfo() const { return typeinfo_;           }
        inline ENTITY_FILTER entityType() const        { return entity;              }

    private:
        std::string propName_;
        TypeInfoWrapper typeinfo_;
        ENTITY_FILTER entity;
};


/*! \class NewNameMessageBox
 *  \brief Asks the user how to proceed after a name clash.
 *
 * When loading a property the provided name can already be in use. This message box is
 * used to ask the user how to proceed. Cancel loading, set a new name or replace the
 * old property by the new one?
 */
class NewNameMessageBox: public QMessageBox
{
    Q_OBJECT

public:
    NewNameMessageBox(QString propName);

private slots:
    void slotReplace() { replace = true; }
    void slotRename()  { rename = true; }
    void slotCancel()  { cancel = true; }

private:
    QLabel* problemDescription;

    QPushButton* buttonRename;
    QPushButton* buttonReplace;
    QPushButton* buttonCancel;

    QString propName;

public:
    bool replace;
    bool rename;
    bool cancel;

};

#endif /* UTILS_H */
