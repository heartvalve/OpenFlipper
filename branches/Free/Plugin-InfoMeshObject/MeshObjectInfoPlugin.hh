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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

//=============================================================================
//
// CLASS InfoMeshObjectPlugin
//
// Author:  David Bommes <bommes@cs.rwth-aachen.de>
//=============================================================================

#ifndef INFOPLUGIN_HH
#define INFOPLUGIN_HH

//== INCLUDES =================================================================

#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/InformationInterface.hh>

#include <OpenFlipper/BasePlugin/StatusbarInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "infoDialog.hh"
#include "infoBar.hh"

//== CLASS DEFINITION =========================================================


/** \brief Plugin to visualize information about objects in the scene
 
  Plugin to visualize information about objects in the scene
*/
class InfoMeshObjectPlugin : public QObject, BaseInterface, InformationInterface, LoggingInterface, StatusbarInterface, LoadSaveInterface
{
  Q_OBJECT
      Q_INTERFACES(BaseInterface)
      Q_INTERFACES(InformationInterface)
      Q_INTERFACES(LoggingInterface)
      Q_INTERFACES(StatusbarInterface)
      Q_INTERFACES(LoadSaveInterface)


  signals:
    // BaseInterface
    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                            QStringList _parameters, QStringList _descriptions);

    // LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);

    // StatusbarInterface
    void addWidgetToStatusbar(QWidget* _widget);

  private slots :

    // BaseInterface
    void initializePlugin();
    void pluginsInitialized();
    void slotObjectUpdated( int _identifier, const UpdateType& _type );
    void slotObjectSelectionChanged( int _identifier );
    void slotAllCleared();
    
    //LoadSaveInterface
    void addedEmptyObject(int _id);
    void objectDeleted(int _id);


    void noguiSupported( ) {} ;

    // InformationInterface
    void slotInformationRequested(const QPoint _clickedPoint, DataType _type);
    DataType supportedDataTypes();

  public :

    // default constructor
    InfoMeshObjectPlugin();

    // default destructor
    ~InfoMeshObjectPlugin() {};

    /// Name of the Plugin
    QString name(){ return (QString("InfoMeshObject")); };

    /// Description of the Plugin
    QString description() { return (QString(tr("Provides Information on Mesh Objects"))); };

  private :
    InfoDialog* info_;

    InfoBar* infoBar_;
    
    QSet<int> targetMeshes_;

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
    /// get the number of boundaries for a given object
    int boundaryCount(int _id);
    /// get the number of components for a given object
    int componentCount(int _id);

    /// get the genus of the given object
    int genus(int _id);
    /// get the center of gravity
    Vector cog(int _id);
    /// get minumum bounding box point
    Vector boundingBoxMin(int _id);
    /// get maximum bounding box point
    Vector boundingBoxMax(int _id);
    /// get the size of the bounding box
    Vector boundingBoxSize(int _id);

    /// get the length of an edge
    double edgeLength(int _id, int _edgeHandle);
    /// get the area of a face
    double faceArea  (int _id, int _faceHandle);
    /// get the aspect ratio of a face
    double aspectRatio(int _id, int _faceHandle);
    /// get vertex valence
    int vertexValence  (int _id, int _vertexHandle);

    /// get the minimal edge length
    double minEdgeLength(int _id);
    /// get the maximal edge length
    double maxEdgeLength(int _id);
    /// get the mean edge length
    double meanEdgeLength(int _id);

  public slots:
    QString version() { return QString("1.0"); };

  private:
    /// set scripting slot descriptions
    void setDescriptions();
    
    /// Get closest vertex index from a face
    template< class MeshT >
    int getClosestVertexInFace(MeshT* _mesh, int _face_idx, ACG::Vec3d& _hitPoint);
    
    /// Get closest edge index from a face
    template< class MeshT >
    int getClosestEdgeInFace(MeshT* _mesh, int _face_idx, const ACG::Vec3d& _hitPoint);

    /// Get closest vertex index from an edge
    template< class MeshT >
    int getClosestVertexFromEdge(MeshT* _mesh, int _edge_idx, ACG::Vec3d& _hitPoint);

    /// Get edge lengths
    template< class MeshT >
    void getEdgeLengths(MeshT* _mesh, double &min, double &max, double &mean);

    /// Get edge lengths
    bool getEdgeLengths(int _id, double &min, double &max, double &mean);
    
/** @} */

};

#endif //MOVEPLUGIN_HH

