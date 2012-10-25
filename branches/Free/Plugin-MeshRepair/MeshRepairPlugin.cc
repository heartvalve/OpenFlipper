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

#include <QtGui>

#include "MeshRepairPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include "OpenFlipper/INIFile/INIFile.hh"

#include <ACG/Geometry/Algorithms.hh>
#include <Math_Tools/Math_Tools.hh>

//-----------------------------------------------------------------------------

MeshRepairPlugin::MeshRepairPlugin() :
tool_(0),
toolIcon_(0)
{

}
//-----------------------------------------------------------------------------

void
MeshRepairPlugin::
initializePlugin()
{
  tool_ = new MeshRepairToolbarWidget();
  QSize size(300, 300);
  tool_->resize(size);

  //==================
  // Vertex operations
  //==================

  connect(tool_->valenceThreeButton, SIGNAL(clicked()), this, SLOT(slotDetectFlatValence3Vertices()) );
  connect(tool_->repairRemoveVButton, SIGNAL(clicked()), this, SLOT(slotRemoveSelectedVal3Vertices()) );

  //==================
  // Edge operations
  //==================

  connect(tool_->detectEShorterButton, SIGNAL(clicked()), this, SLOT(slotDetectEdgesShorter()) );
  connect(tool_->detectELargerButton, SIGNAL(clicked()), this, SLOT(slotDetectEdgesLonger()) );
  connect(tool_->repairCollapseEButton, SIGNAL(clicked()), this, SLOT(slotRemoveSelectedEdges()) );
  connect(tool_->detectCapAngle, SIGNAL(clicked()), this, SLOT(slotDetectSkinnyTriangleByAngle()) );
  connect(tool_->repairFlipEButton, SIGNAL(clicked()), this, SLOT(slotRemoveSkinnyTriangleByAngle()) );
  connect(tool_->detectFoldoverButton, SIGNAL(clicked()), this, SLOT(slotDetectFoldover()) );

  //==================
  // Face operations
  //==================
  connect(tool_->triangleAspectButton,SIGNAL(clicked()),this,SLOT(slotDetectTriangleAspect()));
  connect(tool_->flipOrientation,SIGNAL(clicked()),this,SLOT(slotFlipOrientation()));

  //==================
  // Normal operations
  //==================
  connect(tool_->computeNormals,SIGNAL(clicked()),this,SLOT(slotUpdateNormals()));
  connect(tool_->computeVertexNormals,SIGNAL(clicked()),this,SLOT(slotUpdateVertexNormals()));
  connect(tool_->computeFaceNormals,SIGNAL(clicked()),this,SLOT(slotUpdateFaceNormals()));
  connect(tool_->computeHalfedgeNormals,SIGNAL(clicked()),this,SLOT(slotUpdateHalfedgeNormals()));

  //==================
  // General
  //==================
  connect(tool_->snapBoundaryButton, SIGNAL(clicked()), this, SLOT(slotSnapBoundary()) );
  connect(tool_->fixNonManifoldVerticesButton,SIGNAL(clicked()),this,SLOT(slotFixNonManifoldVertices()));
  connect(tool_->fixMeshButton,SIGNAL(clicked()),this,SLOT(slotFixMesh()));


  toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"meshrepair-toolbox.png");
  tool_->repairCollapseEButton->setIcon(*toolIcon_);
  tool_->repairFlipEButton->setIcon(*toolIcon_);
  tool_->repairRemoveVButton->setIcon(*toolIcon_);

  emit addToolbox( tr("Mesh Repair") , tool_,  toolIcon_);
}

//===========================================================================
// Button Slots
//===========================================================================

void MeshRepairPlugin::slotRemoveSelectedVal3Vertices() {

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    removeSelectedVal3Vertices(o_it->id());

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotRemoveSelectedEdges(){

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it) 
    removeSelectedEdges(o_it->id());

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotDetectSkinnyTriangleByAngle()
{
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    detectSkinnyTriangleByAngle( o_it->id(), tool_->capAngleSpinbox->value(), false );

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotRemoveSkinnyTriangleByAngle()
{
  //rewrite!!!
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    detectSkinnyTriangleByAngle( o_it->id(), tool_->capAngleSpinbox->value(), true );

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotDetectFoldover() {

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    detectFoldover(o_it->id(), tool_->detectFoldoverSpinbox->value());

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotDetectTriangleAspect() {

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    detectTriangleAspect(o_it->id(), tool_->triangleAspectSpinbox->value());

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotFlipOrientation(){

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    flipOrientation(o_it->id());

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotFixMesh() {


  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    fixMesh(o_it->id() , tool_->fixMeshBox->value() );

  emit updateView();

}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotUpdateVertexNormals() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    updateVertexNormals(o_it->id());

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotUpdateFaceNormals() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    updateFaceNormals(o_it->id());

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotUpdateHalfedgeNormals() {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    updateHalfedgeNormals(o_it->id());

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotUpdateNormals(){
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    updateNormals(o_it->id());

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotDetectEdgesShorter(){
  double length = tool_->edgeSpin->value();

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    selectEdgesShorterThan(o_it->id(),length);

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotDetectEdgesLonger(){
  double length = tool_->edgeSpin->value();

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    selectEdgesLongerThan(o_it->id(),length);

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotDetectFlatValence3Vertices() {
  double angle = tool_->valenceThreeSpinbox->value();

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    detectFlatValence3Vertices(o_it->id(), angle);

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotSnapBoundary()
{
  double eps = tool_->snapBoundarySpinBox->value();
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
      snapBoundary(o_it->id(), eps);
  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotFixNonManifoldVertices()
{
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
    fixNonManifoldVertices(o_it->id());
  emit updateView();
}


//-----------------------------------------------------------------------------

/** \brief Initialization of the plugin when it is loaded by the core
 *
 */
void MeshRepairPlugin::pluginsInitialized() {

  // TODO: Check and update

  // ===============================
  // Vertex Operations
  // ===============================

  emit setSlotDescription("removeSelectedVal3Vertices(int)",tr("Remove all selected valence 3 vertices"),
                          QStringList(tr("objectId")),
                          QStringList(tr("ID of an object")));

  emit setSlotDescription("detectFlatValence3Vertices(int,double)",tr("Selects all vertices that have valence 3 and the normals of their neighboring faces have an angle less then the given angle"),
                          QString(tr("objectId,angle")).split(","),
                          QString(tr("ID of an object;the maximal angle between the adjacent faces")).split(";"));

  // ===============================
  // Edge Operations
  // ===============================

  emit setSlotDescription("selectEdgesShorterThan(int,double)",tr("Selects all edges of an object which are shorter than the given length"),
                          QString(tr("objectId,length")).split(","),
                          QString(tr("ID of an object;All edges shorter than this length will be selected")).split(";"));

  emit setSlotDescription("selectEdgesLongerThan(int,double)",tr("Selects all edges of an object which are longer than the given length"),
                          QString(tr("objectId,length")).split(","),
                          QString(tr("ID of an object;All edges longer than this length will be selected")).split(";"));

  emit setSlotDescription("removeSelectedEdges(int)",tr("Remove the selected edges"),
                          QStringList(tr("objectId")),
                          QStringList(tr("ID of an object")));

  emit setSlotDescription("detectSkinnyTriangleByAngle(int,double,bool)",tr("Select or remove skinny triangles (determined by a minimum angle threshold)."),
                          QString(tr("objectId,angle,remove")).split(","),
                          QString(tr("ID of an object;Minimum angle threshold;Remove")).split(";"));

  emit setSlotDescription("detectFoldover(int,float)",tr("Selects edges that are incident to folded over faces."),
                          QString(tr("objectId,angle")).split(","),
                          QString(tr("ID of an object;Minimum threshold angle for fold-overs")).split(";"));

  // ===============================
  // Face Operations
  // ===============================

  emit setSlotDescription("detectTriangleAspect(int,float)",tr("Selects all faces that have a larger aspect ratio than the given one."),
                           QString(tr("objectId,aspect")).split(","),
                           QString(tr("ID of an object;The minimal aspect ratio to select")).split(";"));

  emit setSlotDescription("flipOrientation(int)",tr("Flips the normals of all faces by changing the vertex order in each face"),
                           QStringList(tr("objectId")),
                           QStringList(tr("ID of an object")));


  // ===============================
  // Normal Fixing
  // ===============================

  emit setSlotDescription("updateFaceNormals(int)",tr("Recompute Face normals"),
                          QStringList(tr("objectId")),
                          QStringList(tr("ID of an object")));

  emit setSlotDescription("updateHalfedgeNormals(int)",tr("Recompute Halfedge normals"),
                          QStringList(tr("objectId")),
                          QStringList(tr("ID of an object")));

  emit setSlotDescription("updateVertexNormals(int)",tr("Recompute Vertex normals"),
                          QStringList(tr("objectId")),
                          QStringList(tr("ID of an object")));

  emit setSlotDescription("updateNormals(int)",tr("Recompute Face and Vertex normals"),
                          QStringList(tr("objectId")),
                          QStringList(tr("ID of an object")));


  // ===============================
  // General Mesh fixing
  // ===============================

  emit setSlotDescription("snapBoundary(int,double)",tr("Snaps selected boundary vertices if the distance is less than the given maximal distance."),
                           QString(tr("objectId,epsilon")).split(","),
                           QString(tr("ID of an object;Max Distance")).split(";"));

  emit setSlotDescription("(int)",tr("Fixes non manifold vertices."),
                            QString(tr("objectId")).split(","),
                            QString(tr("ID of an object;Non manifold vertices are splitted.")).split(";"));

  emit setSlotDescription("fixMesh(int,double)",tr("Fixes a mesh."),
                          QString(tr("objectId,distance")).split(","),
                          QString(tr("ID of an object;Vertices with distance lower than epsilon will be treated as one.")).split(";"));

}

Q_EXPORT_PLUGIN2( meshrepairplugin , MeshRepairPlugin );
