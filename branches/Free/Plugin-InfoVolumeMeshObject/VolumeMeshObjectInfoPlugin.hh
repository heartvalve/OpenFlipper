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
*   $Revision: 16191 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2013-01-24 15:35:53 +0100 (Thu, 24 Jan 2013) $                     *
*                                                                            *
\*===========================================================================*/

#ifndef INFOPLUGIN_HH
#define INFOPLUGIN_HH

//== INCLUDES =================================================================

#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/common/Types.hh>

//== CLASS DEFINITION =========================================================


/** \brief Plugin to visualize information about open volume meshs in the scene
 
  Plugin to visualize information about open volume meshs in the scene
*/
class InfoVolumeMeshObjectPlugin : public QObject, BaseInterface, LoggingInterface
{
  Q_OBJECT
      Q_INTERFACES(BaseInterface)
      Q_INTERFACES(LoggingInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-VolumeMeshObjectInfo")
#endif

  signals:
    // BaseInterface
    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                            QStringList _parameters, QStringList _descriptions);

    // LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);

  private slots :

    // BaseInterface
    void initializePlugin();
    void pluginsInitialized();
    void slotObjectUpdated( int _identifier, const UpdateType& _type );
    void slotObjectSelectionChanged( int _identifier );
    void slotAllCleared();
    
    void noguiSupported( ) {}

  public :

    // default constructor
    InfoVolumeMeshObjectPlugin();

    // default destructor
    ~InfoVolumeMeshObjectPlugin();

    /// Name of the Plugin
    QString name(){ return (QString("InfoVolumeMeshObject")); }

    /// Description of the Plugin
    QString description() { return (QString(tr("Provides Information on Volume Mesh Objects"))); }

  private :
    
    template< class MeshT >
    void printMeshInfo( MeshT* _mesh, int _id, unsigned int _face, ACG::Vec3d& _hitPoint );

//===========================================================================
/** @name Scripting Functions
  * @{ */
//===========================================================================

  public slots:

    /// get total number of vertices for a given object
    int vertexCount(int _id);
    /// get total number of edges for a given object
    int edgeCount(int _id);
    /// get total number of faces for a given object
    int faceCount(int _id);
    /// get total number of cells for a given object
    int cellCount(int _id);

  public slots:
    QString version() { return QString("0.1"); }

  private:
    /// set scripting slot descriptions
    void setDescriptions();
    
/** @} */

};

#endif //MOVEPLUGIN_HH

