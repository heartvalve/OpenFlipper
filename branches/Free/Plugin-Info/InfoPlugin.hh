//=============================================================================
//
// CLASS InfoPlugin
//
// Author:  David Bommes <bommes@cs.rwth-aachen.de>
//
// Version: $Revision: 1$
// Date:    $Author$
//   $Date: 03-10-2007$
//
//
//=============================================================================

#ifndef INFOPLUGIN_HH
#define INFOPLUGIN_HH

//== INCLUDES =================================================================

#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>

#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "infoDialog.hh"

//== CLASS DEFINITION =========================================================


/** Plugin for Info Support
 */
class InfoPlugin : public QObject, BaseInterface, MouseInterface, LoggingInterface
{
  Q_OBJECT
      Q_INTERFACES(BaseInterface)
      Q_INTERFACES(MouseInterface)
      Q_INTERFACES(LoggingInterface)


  signals:
    // BaseInterface
    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                            QStringList _parameters, QStringList _descriptions);

    // LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);


  private slots :
    // BaseInterface
    void pluginsInitialized();

    // MouseInterface
    void slotMouseEventIdentify( QMouseEvent* _event );

  public :

  // default constructor
      InfoPlugin(): info_(0) {};

  // default destructor
      ~InfoPlugin() {};

      /// Name of the Plugin
      QString name(){ return (QString("Info")); };

      /// Description of the Plugin
      QString description() { return (QString("Providing Information about Objects")); };

  private :
    InfoDialog* info_;

    template< class MeshT >
    void printMeshInfo( MeshT* _mesh, int _id, unsigned int _face );


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

  public slots:
    QString version() { return QString("1.0"); };

  private:
    /// set scripting slot descriptions
    void setDescriptions();
    
    /// Get closest vertex index
    template< class MeshT >
    int getClosestVertex(MeshT* _mesh, int _face_idx);
    
    /// Get closest edge index
    template< class MeshT >
    int getClosestEdge(MeshT* _mesh, int _face_idx);
    
    /// Mouse click hit point on face
    ACG::Vec3d hit_point_;
    
/** @} */

};

#endif //MOVEPLUGIN_HH

