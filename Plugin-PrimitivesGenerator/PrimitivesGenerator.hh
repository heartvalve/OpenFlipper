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

#ifndef PRIMITIVESGENERATORPLUGIN_HH
#define PRIMITIVESGENERATORPLUGIN_HH

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>
#endif


class PrimitivesGeneratorPlugin: public QObject, BaseInterface, LoggingInterface, MenuInterface, LoadSaveInterface, RPCInterface, BackupInterface {
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(MenuInterface)
  Q_INTERFACES(LoadSaveInterface)
  Q_INTERFACES(RPCInterface)
  Q_INTERFACES(BackupInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-PrimitiveGenerator")
#endif

  signals:

  //BaseInterface
  void updateView();
  void updatedObject(int _id, const UpdateType& _type);
  void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                          QStringList _parameters, QStringList _descriptions);
  //LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);

  // Menu Interface
  void getMenubarMenu (QString _name, QMenu *& _menu, bool _create);

  // LoadSaveInterface
  void addEmptyObject( DataType _type, int& _id);

  // BackupInterface
  void createBackup( int _objectid, QString _name, UpdateType _type = UPDATE_ALL);

public:

  PrimitivesGeneratorPlugin();
  ~PrimitivesGeneratorPlugin();

  // BaseInterface
  QString name() { return (QString("Primitives Generator"));  } ;
  QString description() {  return (QString("Plugin used for generating basic primitives")); } ;

private slots:

  /// BaseInterface
  void initializePlugin();
  void pluginsInitialized();

  /// Tell system that this plugin runs without ui
  void noguiSupported( ) {} ;

public slots:

  QString version() { return QString("1.0"); };

  int addTriangulatedCylinder(const Vector& _position = Vector(0.0,0.0,0.0),
                              const Vector& _axis     = Vector(0.0,0.0,1.0),
                              const double _radius   = 1.0,
                              const double _height   = 5.0,
                              const bool   _top      = true,
                              const bool   _bottom   = true);

  int addSphere(const Vector& _position = Vector(0.0,0.0,0.0),
                const double _radius = 1.0);

  int addSubdivisionSphere(const Vector& _position = Vector(0.0,0.0,0.0),
                const double _radius = 1.0);

  int addTetrahedron(const Vector& _position = Vector(0.0,0.0,0.0),
                const double _length = 2.0);

  int addPyramid(const Vector& _position = Vector(0.0,0.0,0.0),
                 const double _length = 2.0);

  int addTriangulatedCube(const Vector& _position = Vector(0.0,0.0,0.0),
                          const double _length = 2.0);

  int addIcosahedron(const Vector& _position = Vector(0.0,0.0,0.0),
                     const double _length = 2.0);

  int addOctahedron(const Vector& _position = Vector(0.0,0.0,0.0),
                    const double _length = 2.0);

  int addDodecahedron(const Vector& _position = Vector(0.0,0.0,0.0),
                      const double _length = 2.0);

#ifdef ENABLE_BSPLINECURVE_SUPPORT
  int addRandomBSplineCurve(const Vector& _position = Vector(0, 0, 0), int nDiv = 5);
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
  int addRandomBSplineSurface(const Vector& _position = Vector(0, 0, 0), int nDiv = 5);
#endif

#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
  int addTetrahedralCube(const Vector& _position = Vector(0.0,0.0,0.0),
                         const double _length = 2.0);

  int addTetrahedralCuboid(const Vector& _position = Vector(0.0,0.0,0.0),
                           const Vector& _length = Vector(4.0,6.0,12.0),
                           const unsigned int n_x = 5,
                           const unsigned int n_y = 5,
                           const unsigned int n_z = 10);
#endif

private:
  int addTriMesh();
  int addPolyMesh();
  // construct Octahedron in triMesh_
  void constructOctahedron(const Vector& _position, const double _length);

#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
  int addPolyhedralMesh();
#endif

  inline void add_face( int _vh1 , int _vh2, int _vh3 );

  inline void add_face( int _vh1 , int _vh2, int _vh3, int _vh4 , int _vh5 );

  inline ACG::Vec3d positionOnCylinder(const int _sliceNumber,
                                       const int _stackNumber,
                                       const Vector _position ,
                                       const Vector _axis,
                                       const double _radius,
                                       const double _height);

  inline ACG::Vec3d positionOnSphere(int _sliceNumber, int _stackNumber, const double _radius, const Vector& _position);
  inline ACG::Vec2f texCoordOnSphere(int _sliceNumber, int _stackNumber);

  std::vector<TriMesh::VertexHandle> vhandles_;
  std::vector<PolyMesh::VertexHandle> vphandles_;

  TriMesh*  triMesh_;
  PolyMesh* polyMesh_;

  int slices_;
  int stacks_;

  QMenu* primitivesMenu_;

};

#endif //PRIMITIVESGENERATORPLUGIN_HH
