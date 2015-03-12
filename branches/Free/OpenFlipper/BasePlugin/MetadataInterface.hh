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
*   $Revision: $                                                       *
*   $LastChangedBy: $                                                *
*   $Date: $                     *
*                                                                            *
\*===========================================================================*/

#ifndef METADATAINTERFACE_HH
#define METADATAINTERFACE_HH

#include <OpenFlipper/common/Types.hh>

#include <QObject>
#include <QString>

#if QT_VERSION >= 0x050000
#include <QJsonDocument>
#endif



/** \file MetadataInterface.hh
 *
 * Interface implementing slots related to the deserialization of meta data
 * (usually encountered in screenshots).
 */

/** \brief Enables implementers to react on deserialization of meta data.
  *
  * \ref MetaDataInterfacePage "Detailed description"
  *
  * Using functions such as BaseObject::getCommentByKey() plugins can attach
  * meta data to objects. This meta data is currently only serialized when
  * taking viewer snapshots. "Plugin-DeserializeScreenshotMetadata" allows
  * deserializing this metadata from a viewer snapshot. (In the future
  * serialization and deserialization of meta data may occur in other contexts
  * as well.)
  *
  * Whenever meta gets deserialized the slots in this interface will get
  * triggered. Please refer to the documentation of the individual slots
  * to find out which specific signal suits your needs.
  *
  * The slots are always triggered in the order
  * slotGenericMetadataDeserialized(), slotObjectMetadataDeserialized(),
  * slotObjectMetadataDeserializedJson().
  */
class MetadataInterface {
    private slots:

        /**
         * This low-level signal is very inconvenient to use and chances are you
         * want to use one of the other signals.
         *
         * Triggered for every top level meta data entry. Currently the only top
         * level meta data entries created by OpenFlipper are "Mesh Comments",
         * "Mesh Materials" and "View". "Mesh Comments" contains the raw
         * concatenation of all object meta data and is very cumbersome to
         * parse. In most cases it's a better idea to use
         * objectMetadataDeserialized().
         */
        virtual void slotGenericMetadataDeserialized(
                QString key, QString value) {};

        /**
         * Triggered for every piece of object specific meta data encapsulated
         * in "Mesh Comments".
         */
        virtual void slotObjectMetadataDeserialized(
                QString object_name, QString value) {};

#if QT_VERSION >= 0x050000
        /**
         * Triggered for every piece of object specific meta data encapsulated
         * in "Mesh Comments" if it is valid JSON.
         */
        virtual void slotObjectMetadataDeserializedJson(
                QString object_name, QJsonDocument value) {};
#endif

    signals:
        /**
         * Can be called by anyone who deserializes meta data, e.g. from a
         * viewer snapshot PNG file. The appropriate slots within
         * this interface will be triggered.
         */
        virtual void metadataDeserialized(
                const QVector<QPair<QString, QString> > &data) = 0;

    public:

        /// Destructor
        virtual ~MetadataInterface() {};

};

/** \page metaDataInterfacePage MetaData Interface
\n
\image html metaDataInterface.png
\n

Using functions such as BaseObject::getCommentByKey() plugins can attach
meta data to objects. This meta data is currently only serialized when
taking viewer snapshots. "Plugin-DeserializeScreenshotMetadata" allows
deserializing this metadata from a viewer snapshot. (In the future
serialization and deserialization of meta data may occur in other contexts
as well.)

Whenever meta gets deserialized the slots in this interface will get
triggered. Please refer to the documentation of the individual slots
to find out which specific signal suits your needs.

*/


#if QT_VERSION >= 0x050000
Q_DECLARE_INTERFACE(MetadataInterface,"OpenFlipper.MetadataInterface_qt5/1.0")
#else
Q_DECLARE_INTERFACE(MetadataInterface,"OpenFlipper.MetadataInterface_qt4/1.0")
#endif

#endif // METADATAINTERFACE_HH
