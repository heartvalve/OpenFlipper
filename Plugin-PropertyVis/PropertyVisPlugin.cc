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
//  CLASS PropertyVisPlugin - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <Qt>
#include <QtGui>
#include <QSpacerItem>
#include <QFileDialog>

#include "PropertyVisPlugin.hh"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "OpenFlipper/INIFile/INIFile.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ACG/Utils/ColorCoder.hh>


//== IMPLEMENTATION ==========================================================

#define PROP_VIS "PropertyVisualization"

PropertyVisPlugin::PropertyVisPlugin() :
pickModeActive_(false),
lastPickMode_(""),
tool_(0),
file_stream_(0)
{
}

void PropertyVisPlugin::initializePlugin()
{
  tool_ = new PropertyVisToolbar();
  
  QSize size(300,300);
  tool_->resize(size);
  
  // connect toolbox elements
  connect(tool_->meshNames,       SIGNAL( currentIndexChanged(int) ), this, SLOT( slotMeshChanged(int) ) );

  connect(tool_->visualizeButton, SIGNAL( clicked() ), this, SLOT( slotVisualize() ) );
  connect(tool_->clearButton,     SIGNAL( clicked() ), this, SLOT( slotAllCleared() ) );
  connect(tool_->createNewButton, SIGNAL( clicked() ), this, SLOT( slotCreateProperty() ) );

  connect(tool_->load_property, SIGNAL( clicked() ), this, SLOT( loadSelectedProperty() ) );
  connect(tool_->save_property, SIGNAL( clicked() ), this, SLOT( saveSelectedProperty() ) );
  
  connect(tool_->refresh_property_names_tb, SIGNAL( clicked() ), this, SLOT( slotMeshChanged() ) );
  connect(tool_->duplicate_property_tb, SIGNAL( clicked() ), this, SLOT( slotDuplicateProperty() ) );
  connect(tool_->remove_property_tb, SIGNAL( clicked() ), this, SLOT( slotRemoveProperty() ) );

  connect(tool_, SIGNAL( widgetShown() ), this, SLOT( updateGUI() ) );
  
  tool_->propertyName_lv->setModel(&propertyNameListModel_);

  connect(tool_->propertyName_lv->selectionModel(),
          SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
          this,
          SLOT( propertySelectionChanged() ));
  connect(&propertyNameListModel_, SIGNAL( modelReset() ), this, SLOT( propertySelectionChanged() ));

  tool_->meshNames->setModel(&objectListItemModel_);

  emit addHiddenPickMode( PROP_VIS );
  pickModeActive_ = false;
  connect(tool_->pickButton, SIGNAL( clicked() ), this, SLOT( pickValue() ) );
  
  QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
  tool_->pickButton->setIcon( QIcon(iconPath + "color-picker.png") );
  
  QIcon* toolIcon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"PropertyVisIcon.png");

  emit addToolbox( tr("Property Visualization") , tool_, toolIcon );
}

//-----------------------------------------------------------------------------


void PropertyVisPlugin::pluginsInitialized() 
{
    propertySelectionChanged();
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::slotPickModeChanged( const std::string& _mode)
{
  pickModeActive_ = (_mode == PROP_VIS);
  
  tool_->pickButton->setChecked(pickModeActive_);
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::pickValue()
{
  if ( tool_->pickButton->isChecked() ){
  
    lastPickMode_   = PluginFunctions::pickMode();
    lastActionMode_ = PluginFunctions::actionMode();

    PluginFunctions::pickMode(PROP_VIS);
    PluginFunctions::actionMode(Viewer::PickingMode);
    
  } else {
    
    PluginFunctions::pickMode(lastPickMode_);
    PluginFunctions::actionMode(lastActionMode_);
  }
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::slotAllCleared()
{
  for(unsigned int i=0; i<lineNodes_.size(); ++i)
    delete lineNodes_[i];
  lineNodes_.clear();

  emit updateView();
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::slotObjectUpdated( int _identifier, const UpdateType& _type )
{
    if( tool_->isVisible() )
        updateGUI();
}

void PropertyVisPlugin::updateGUI() {
    objectListItemModel_.refresh(DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH));
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::propertySelectionChanged() {
    enum TYPE {
        T_VECTOR        = 1 << 0,
        T_DOUBLE        = 1 << 1,
        T_INT           = 1 << 2,
        T_UINT          = 1 << 3,
        T_BOOL          = 1 << 4,
        T_SKIN_WEIGHTS  = 1 << 5,
        T_VECTOR_EDGES  = 1 << 6,
    };

    int visibleMask = 0;

    int selectedVectors = 0;

    QModelIndexList selectedIndices = tool_->propertyName_lv->selectionModel()->selectedIndexes();
    for (QModelIndexList::const_iterator it = selectedIndices.begin(), it_end = selectedIndices.end();
            it != it_end; ++it) {

        const PropertyNameListModel::PROP_INFO &currentProp = propertyNameListModel_[it->row()];

        if (currentProp.typeinfo() == PropertyNameListModel::proptype_Vec3d ||
                currentProp.typeinfo() == PropertyNameListModel::proptype_Vec3f) {
            visibleMask |= T_VECTOR;
            ++selectedVectors;
            if (currentProp.entityType() == PropertyNameListModel::EF_FACE)
                visibleMask |= T_VECTOR_EDGES;
        } else if (currentProp.typeinfo() == PropertyNameListModel::proptype_double) {
            visibleMask |= T_DOUBLE;
        } else if (currentProp.typeinfo() == PropertyNameListModel::proptype_int) {
            visibleMask |= T_INT;
        } else if (currentProp.typeinfo() == PropertyNameListModel::proptype_uint) {
            visibleMask |= T_UINT;
        } else if (currentProp.typeinfo() == PropertyNameListModel::proptype_bool) {
            visibleMask |= T_BOOL;
#ifdef ENABLE_SKELETON_SUPPORT
        } else if (currentProp.typeinfo() == PropertyNameListModel::proptype_SkinWeights) {
            visibleMask |= T_SKIN_WEIGHTS;
#endif
        }

    }

    tool_->hint_gb->setVisible(visibleMask & T_VECTOR);
    tool_->vectorFieldDifference_gb->setVisible(selectedVectors == 2);
    if (selectedVectors != 2) {
        tool_->visualizeVectorDifference_cb->setChecked(false);
    }

    tool_->paramVector->setVisible(visibleMask & T_VECTOR);
    tool_->paramDouble->setVisible(visibleMask & T_DOUBLE);
    tool_->paramUint->setVisible(visibleMask & T_UINT);
    tool_->paramInt->setVisible(visibleMask & T_INT);
    tool_->paramBool->setVisible(visibleMask & T_BOOL);
#ifdef ENABLE_SKELETON_SUPPORT
    tool_->paramSkinWeights->setVisible(visibleMask & T_SKIN_WEIGHTS);
#endif
    tool_->vectors_edges_rb->setVisible(visibleMask & T_VECTOR_EDGES);
    if (tool_->vectors_edges_rb->isChecked() && (visibleMask & T_VECTOR_EDGES) == 0)
        tool_->vectors_strokes_rb->setChecked(true);
}


//-----------------------------------------------------------------------------

void PropertyVisPlugin::slotMeshChanged(int /*_index*/)
{

  int id = tool_->meshNames->itemData( tool_->meshNames->currentIndex() ).toInt();
  BaseObjectData* object = 0;

  PluginFunctions::getObject( id, object );

  if (object == 0) {
    updatePropertyList((TriMesh*)0);
    return;
  }


  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    updatePropertyList(mesh);

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    updatePropertyList(mesh);
  }
}

//-----------------------------------------------------------------------------

template<typename MeshT>
void PropertyVisPlugin::visualizeProperty(MeshT *mesh, const PropertyNameListModel::PROP_INFO &currentProp) {
    //VECTOR
    if (!tool_->visualizeVectorDifference_cb->isChecked() &&
            (currentProp.typeinfo() == PropertyNameListModel::proptype_Vec3d ||
             currentProp.typeinfo() == PropertyNameListModel::proptype_Vec3f))
        visualizeVector(mesh, currentProp);
    //DOUBLE
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_double)
        visualizeDouble(mesh, currentProp);
    //UNSIGNED INT
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_uint)
        visualizeUnsignedInteger(mesh, currentProp);
    //INT
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_int)
        visualizeInteger(mesh, currentProp);
    //BOOL
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_bool)
        visualizeBool(mesh, currentProp);
#ifdef ENABLE_SKELETON_SUPPORT
    //SKIN WEIGHTS
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_SkinWeights)
        visualizeSkinWeights(mesh, tool_->boneId->value(), currentProp);
#endif
}


namespace {

template<typename MeshT>
float scalarFn_norm_of_diff(const typename MeshT::Point &a, const typename MeshT::Point &b) {
    return (a - b).norm();
}

template<typename MeshT>
float scalarFn_diff_of_norms(const typename MeshT::Point &a, const typename MeshT::Point &b) {
    return std::fabs(a.norm() - b.norm());
}

template<typename MeshT>
float scalarFn_4_symm_diff(const typename MeshT::Point &a, const typename MeshT::Point &b) {
    double alpha = std::acos((a|b) / a.norm() / b.norm());
    alpha -= std::floor((alpha + M_PI_4) / M_PI_2) * M_PI_2;
    return std::fabs(alpha);
}

template<typename Mesh, typename Prop1, typename Prop2, float (*ScalarFn)(const typename Prop1::value_type &, const typename Prop2::value_type &)>
class ScalarAssigner {
    public:
        ScalarAssigner(const Mesh &mesh, const Prop1 &prop1, const Prop2 &prop2) :
            mesh(mesh), prop1(prop1), prop2(prop2) { }

        template<typename Handle>
        float operator() (const Handle &handle) const {
            return ScalarFn(mesh.property(prop1, handle), mesh.property(prop2, handle));
        }

    protected:
        const Mesh &mesh;
        const Prop1 &prop1;
        const Prop2 &prop2;
};


template<typename MeshT, typename MeshIteratorT>
class ColorSetIterator {
    public:
        typedef std::output_iterator_tag iterator_category;
        typedef void value_type;
        typedef void difference_type;
        typedef void pointer;
        typedef void reference;

        ColorSetIterator(MeshT &mesh, MeshIteratorT it) : mesh(mesh), it(it) {}

        ColorSetIterator &operator=(const ACG::Vec4f &color) {
            mesh.set_color(*it, color);
            return *this;
        }

        ColorSetIterator &operator*() {
            return *this;
        }

        ColorSetIterator &operator++() {
            ++it;
            return *this;
        }

        ColorSetIterator operator++(int) {
            ColorSetIterator<MeshT, MeshIteratorT> cpy(*this);
            ++it;
            return cpy;
        }

    private:
        MeshT &mesh;
        MeshIteratorT it;
};

template<typename MeshT, typename IteratorT, typename PropHandleType, float (*ScalarFn)(const typename MeshT::Point &, const typename MeshT::Point &)>
void colorElements(MeshT *mesh, const PropertyNameListModel::PROP_INFO &p1,
                   const PropertyNameListModel::PROP_INFO &p2,
                   IteratorT primitivesBegin, IteratorT primitivesEnd) {
    PropHandleType prop1, prop2;
    if (!mesh->get_property_handle(prop1, p1.propName())) return;
    if (!mesh->get_property_handle(prop2, p2.propName())) return;

    std::vector<float> scalars;
    std::transform(primitivesBegin, primitivesEnd, std::back_inserter(scalars),
                   ScalarAssigner<MeshT, PropHandleType, PropHandleType, ScalarFn>(*mesh, prop1, prop2));

    const float min = *std::min_element(scalars.begin(), scalars.end());
    const float max = *std::max_element(scalars.begin(), scalars.end());

    std::cerr << "Range: " << min << ", " << max << std::endl;

    std::transform(scalars.begin(), scalars.end(), ColorSetIterator<MeshT, IteratorT>(*mesh, primitivesBegin),
                   ACG::ColorCoder(min, max, false));
}

}

template<typename MeshT, float (*ScalarFn)(const typename MeshT::Point &, const typename MeshT::Point &)>
void PropertyVisPlugin::visualizeVectorFieldDifference(MeshT *mesh, const PropertyNameListModel::PROP_INFO &p1,
                                                       const PropertyNameListModel::PROP_INFO &p2) {

    if (p1.isVertexProp()) {
        if (!mesh->has_vertex_colors())
            mesh->request_vertex_colors();
        colorElements<MeshT, typename MeshT::VertexIter, OpenMesh::VPropHandleT<typename MeshT::Point>, ScalarFn>(mesh, p1, p2, mesh->vertices_begin(), mesh->vertices_end());
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_POINTS_COLORED);
    } else if (p1.isFaceProp()) {
        if (!mesh->has_face_colors())
            mesh->request_face_colors();
        colorElements<MeshT, typename MeshT::FaceIter, OpenMesh::FPropHandleT<typename MeshT::Point>, ScalarFn>(mesh, p1, p2, mesh->faces_begin(), mesh->faces_end());
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED);
    } else if (p1.isEdgeProp()) {
        if (!mesh->has_edge_colors())
            mesh->request_edge_colors();
        colorElements<MeshT, typename MeshT::EdgeIter, OpenMesh::EPropHandleT<typename MeshT::Point>, ScalarFn>(mesh, p1, p2, mesh->edges_begin(), mesh->edges_end());
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::EDGES_COLORED);
    } else if (p1.isHalfedgeProp()) {
        if (!mesh->has_halfedge_colors())
            mesh->request_halfedge_colors();
        colorElements<MeshT, typename MeshT::HalfedgeIter, OpenMesh::HPropHandleT<typename MeshT::Point>, ScalarFn>(mesh, p1, p2, mesh->halfedges_begin(), mesh->halfedges_end());
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::HALFEDGES_COLORED);
    } else {
        emit log("Got property which refers to no type of primitive.");
    }
}

namespace {
    class PROP_INFO_TYPE_SORTER {
        public:
            bool operator() (const PropertyNameListModel::PROP_INFO *a, const PropertyNameListModel::PROP_INFO *b) {
                return (a->entityType() == b->entityType() && a->typeinfo() < b->typeinfo()) ||
                        a->entityType() < b->entityType();
            }
    };
    class PROP_INFO_TYPE_EQUAL {
        public:
            bool operator() (const PropertyNameListModel::PROP_INFO *a, const PropertyNameListModel::PROP_INFO *b) {
                return a->entityType() == b->entityType() && a->typeinfo() == b->typeinfo();
            }
    };
}

void PropertyVisPlugin::slotVisualize()  {

    int id = tool_->meshNames->itemData( tool_->meshNames->currentIndex() ).toInt();
    BaseObjectData* object = 0;

    PluginFunctions::getObject( id, object );

    if (object == 0){
        emit log(LOGERR, "Unable to get object");
        return;
    }

    TriMesh * const triMesh = (object->dataType(DATA_TRIANGLE_MESH) ? PluginFunctions::triMesh(object) : 0);
    PolyMesh * const polyMesh = (object->dataType(DATA_POLY_MESH) ? PluginFunctions::polyMesh(object) : 0);;

    if (triMesh == 0 && polyMesh == 0) {
        emit log(LOGERR,"Unable to get mesh");
        return;
    }

    // Actual Viz takes place in try-catch block to catch VizExceptions.
    try {

        /*
         * Visualize single properties.
         */
        QModelIndexList selectedIndices = tool_->propertyName_lv->selectionModel()->selectedIndexes();
        for (QModelIndexList::const_iterator it = selectedIndices.begin(), it_end = selectedIndices.end();
                it != it_end; ++it) {

            const PropertyNameListModel::PROP_INFO &currentProp = propertyNameListModel_[it->row()];

            if (triMesh)
                visualizeProperty(triMesh, currentProp);
            else if (polyMesh)
                visualizeProperty(polyMesh, currentProp);
        }

        /*
         * Visualize composed properties.
         */
        if (tool_->visualizeVectorDifference_cb->isChecked()) {

            // Look for two vector properties of the same type and entity.

            std::vector<const PropertyNameListModel::PROP_INFO*> propList;

            QModelIndexList selectedIndices =
                    tool_->propertyName_lv->selectionModel()->selectedIndexes();
            for (QModelIndexList::const_iterator it = selectedIndices.begin(), it_end =
                    selectedIndices.end(); it != it_end; ++it) {
                const PropertyNameListModel::PROP_INFO &currentProp =
                        propertyNameListModel_[it->row()];
                if (currentProp.typeinfo() == PropertyNameListModel::proptype_Vec3d
                        || currentProp.typeinfo() == PropertyNameListModel::proptype_Vec3f)
                    propList.push_back(&currentProp);
            }

            std::sort(propList.begin(), propList.end(), PROP_INFO_TYPE_SORTER());
            std::vector<const PropertyNameListModel::PROP_INFO*>::const_iterator it =
                    std::adjacent_find(propList.begin(), propList.end(), PROP_INFO_TYPE_EQUAL());

            /*
             * Are there two selected properties of equal vector type?
             */
            if (it != propList.end()) {
                if (tool_->vecFieldDiff_norm_diff_rb->isChecked()) {
                    if (triMesh)
                        visualizeVectorFieldDifference<TriMesh, scalarFn_norm_of_diff<TriMesh> >(
                                triMesh, **it, **(it + 1));
                    else if (polyMesh)
                        visualizeVectorFieldDifference<PolyMesh, scalarFn_norm_of_diff<PolyMesh> >(
                                polyMesh, **it, **(it + 1));
                } else if (tool_->vecFieldDiff_diff_norm_rb->isChecked()) {
                    if (triMesh)
                        visualizeVectorFieldDifference<TriMesh, scalarFn_diff_of_norms<TriMesh> >(
                                triMesh, **it, **(it + 1));
                    else if (polyMesh)
                        visualizeVectorFieldDifference<PolyMesh, scalarFn_diff_of_norms<PolyMesh> >(
                                polyMesh, **it, **(it + 1));
                } else if (tool_->vecFieldDiff_4symm_rb->isChecked()) {
                    if (triMesh)
                        visualizeVectorFieldDifference<TriMesh, scalarFn_4_symm_diff<TriMesh> >(
                                triMesh, **it, **(it + 1));
                    else if (polyMesh)
                        visualizeVectorFieldDifference<PolyMesh, scalarFn_4_symm_diff<PolyMesh> >(
                                polyMesh, **it, **(it + 1));
                } else {
                    emit log("This vector field difference mode is not implemented.");
                }
            }
        }
    } catch (const VizException &e) {
        QMessageBox::warning(this->tool_, trUtf8("Error in Visualization"), trUtf8(e.what()), QMessageBox::Cancel, QMessageBox::Cancel);
    }

    emit updatedObject( object->id(), UPDATE_COLOR );
}


//-----------------------------------------------------------------------------

void PropertyVisPlugin::slotCreateProperty() 
{

  int id = tool_->meshNames->itemData( tool_->meshNames->currentIndex() ).toInt();
  BaseObjectData* object = 0;

  PluginFunctions::getObject( id, object );

  if (object == 0){
    emit log(LOGERR, "Unable to get object");
    return;
  }


  if ( object->dataType(DATA_TRIANGLE_MESH) ) 
  {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return;
    }

    addNewProperty(mesh);
    emit updatedObject( object->id(), UPDATE_ALL );
  }
  else 
  {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return;
    }

    addNewProperty(mesh);
    emit updatedObject( object->id(), UPDATE_ALL );
  }
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::slotMouseEvent( QMouseEvent* _event ) {
  
  if (!pickModeActive_) return;
  
  if (_event->type() == QEvent::MouseButtonPress)
  {
    unsigned int   node_idx, target_idx;
    ACG::Vec3d     hit_point;

    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;


      if ( PluginFunctions::getPickedObject(node_idx, object) ) {

         // Set hit point
         hit_point_ = ACG::Vec3d(hit_point[0], hit_point[1], hit_point[2]);

         if ( object->picked(node_idx) && object->dataType(DATA_TRIANGLE_MESH) )
            getPropertyValue( PluginFunctions::triMesh(object) , object->id(), target_idx, hit_point_ );

         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH) )
            getPropertyValue( PluginFunctions::polyMesh(object) , object->id(), target_idx, hit_point_ );

      } else return;
    }
  }
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::saveOnExit( INIFile& _ini ) {
  saveIniFileOptions(_ini);
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::saveIniFileOptions( INIFile& _ini ) {

  QString sectionName = "PropertyVis";
  
  _ini.add_entry(sectionName , "DataType" , tool_->propertyDataType->currentIndex()  );
  _ini.add_entry(sectionName , "Type"   , tool_->propertyType->currentIndex() );

}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::loadIniFileOptions( INIFile& _ini ) {

  QString sectionName = "PropertyVis";
  
   if ( !_ini.section_exists( sectionName ) )
     return;

   int index;
   
   if ( _ini.get_entry( index , sectionName , "DataType" ) )
     tool_->propertyDataType->setCurrentIndex( index );
   
   if ( _ini.get_entry( index , sectionName , "Type" ) )
     tool_->propertyType->setCurrentIndex( index );
}

//-----------------------------------------------------------------------------


QString 
PropertyVisPlugin::
getLoadFilename()
{
  QString filter(tr("default"));
  QString ptype = tool_->propertyType->currentText();

  if( ptype == tr("Vertex"))
    filter = tr("Vertex Property (*.vprop);; All Files (*.*)");
  else if( ptype == tr("Edge"))
    filter = tr("Edge Property (*.eprop);; All Files (*.*)");
  else if( ptype == tr("Face"))
    filter = tr("Face Property (*.fprop);; All Files (*.*)");
  else if( ptype == tr("HalfEdge"))
    filter = tr("HalfEdge Property (*.hprop);; All Files (*.*)");

  QString fileName = QFileDialog::getOpenFileName(0, tr("Load Property"), QString(), filter);

  return fileName;
}


//-----------------------------------------------------------------------------


QString
PropertyVisPlugin::
getSaveFilename()
{
  QString filter(tr("default"));
  QString ptype = tool_->propertyType->currentText();

  if( ptype == tr("Vertex"))
    filter = tr("Vertex Property (*.vprop);; All Files (*.*)");
  else if( ptype == tr("Edge"))
    filter = tr("Edge Property (*.eprop);; All Files (*.*)");
  else if( ptype == tr("Face"))
    filter = tr("Face Property (*.fprop);; All Files (*.*)");
  else if( ptype == tr("HalfEdge"))
    filter = tr("HalfEdge Property (*.hprop);; All Files (*.*)");

  QString fileName = QFileDialog::getSaveFileName(0, tr("Save Property"), QString(), filter);

  return fileName;
}


//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
openFile(QString _filename)
{
  closeFile();
  file_.setFileName(_filename);
  if(file_.open(QIODevice::ReadWrite | QIODevice::Text))
    file_stream_ = new QTextStream(&file_);
}


//-----------------------------------------------------------------------------

void
PropertyVisPlugin::
closeFile()
{
  if( file_stream_)
  {
    delete file_stream_;
    file_stream_ = 0;
  }

  if(file_.exists()) file_.close();
}


//-----------------------------------------------------------------------------


bool
PropertyVisPlugin::
eofFile()
{
  if(!file_stream_) return true;
  else return file_stream_->atEnd();
}


//-----------------------------------------------------------------------------


double
PropertyVisPlugin::
readDouble()
{
  if(file_stream_)
  {
    double d;
    (*file_stream_) >> d;
    return d;
  }
  else return 0.0;
}


//-----------------------------------------------------------------------------


int
PropertyVisPlugin::
readInteger()
{
  if(file_stream_)
  {
    int i;
    (*file_stream_) >> i;
    return i;
  }
  else return 0;
}

//-----------------------------------------------------------------------------


unsigned int
PropertyVisPlugin::
readUnsignedInteger()
{
  if(file_stream_)
  {
    unsigned int i;
    (*file_stream_) >> i;
    return i;
  }
  else return 0;
}


//-----------------------------------------------------------------------------


QString
PropertyVisPlugin::
readString()
{
  if(file_stream_)
  {
    QString s;
    (*file_stream_) >> s;
    return s;
  }
  else return QString("");
}


//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
writeDouble(double _d)
{
  if(file_stream_)
  {
    (*file_stream_) << _d  << '\n';
  }
  else std::cerr << "Warning: filestream not available...\n";
}

//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
writeInteger(int _i)
{
  if(file_stream_)
  {
    (*file_stream_) << _i << '\n';
  }
  else std::cerr << "Warning: filestream not available...\n";
}

//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
writeUnsignedInteger(unsigned int _i)
{
  if(file_stream_)
  {
    (*file_stream_) << _i << '\n';
  }
  else std::cerr << "Warning: filestream not available...\n";
}

//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
writeString(QString _s)
{
  if(file_stream_)
  {
    (*file_stream_) << _s << '\n';
  }
  else std::cerr << "Warning: filestream not available...\n";
}


//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
setVPropV3D(QString _property, int _vidx, int _vector_idx, double _val)
{
  BaseObjectData* object = getSelectedObject();

  if (object != 0)
  {
    if ( object->dataType(DATA_TRIANGLE_MESH) ) 
    {
      TriMesh* mesh = PluginFunctions::triMesh(object);
      OpenMesh::VPropHandleT<TriMesh::Point> prop;

      if ( mesh != 0 ) 
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_vidx >= 0 && _vidx < (int)mesh->n_vertices())
            if( _vector_idx >= 0 && _vector_idx < 3)
              mesh->property(prop, mesh->vertex_handle(_vidx))[_vector_idx] = _val;
      }
    }
    else if ( object->dataType(DATA_TRIANGLE_MESH) ) 
    {
      PolyMesh* mesh = PluginFunctions::polyMesh(object);
      OpenMesh::VPropHandleT<PolyMesh::Point> prop;

      if ( mesh != 0 ) 
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_vidx >= 0 && _vidx < (int) mesh->n_vertices())
            if( _vector_idx >= 0 && _vector_idx < 3)
              mesh->property(prop, mesh->vertex_handle(_vidx))[_vector_idx] = _val;
      }
    }
  }
}


//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
setEPropV3D(QString _property, int _eidx, int _vector_idx, double _val)
{
  BaseObjectData* object = getSelectedObject();

  if (object != 0)
  {
    if ( object->dataType(DATA_TRIANGLE_MESH) ) 
    {
      TriMesh* mesh = PluginFunctions::triMesh(object);
      OpenMesh::EPropHandleT<TriMesh::Point> prop;

      if ( mesh != 0 ) 
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_eidx >= 0 && _eidx < (int) mesh->n_edges())
            if( _vector_idx >= 0 && _vector_idx < 3)
              mesh->property(prop, mesh->edge_handle(_eidx))[_vector_idx] = _val;
      }
    }
    else if ( object->dataType(DATA_TRIANGLE_MESH) ) 
    {
      PolyMesh* mesh = PluginFunctions::polyMesh(object);
      OpenMesh::EPropHandleT<PolyMesh::Point> prop;

      if ( mesh != 0 ) 
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_eidx >= 0 && _eidx < (int) mesh->n_edges())
            if( _vector_idx >= 0 && _vector_idx < 3)
              mesh->property(prop, mesh->edge_handle(_eidx))[_vector_idx] = _val;
      }
    }
  }
}


//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
setFPropV3D(QString _property, int _fidx, int _vector_idx, double _val)
{
  BaseObjectData* object = getSelectedObject();

  if (object != 0)
  {
    if ( object->dataType(DATA_TRIANGLE_MESH) ) 
    {
      TriMesh* mesh = PluginFunctions::triMesh(object);
      OpenMesh::FPropHandleT<TriMesh::Point> prop;

      if ( mesh != 0 ) 
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_fidx >= 0 && _fidx < (int) mesh->n_faces())
            if( _vector_idx >= 0 && _vector_idx < 3)
              mesh->property(prop, mesh->face_handle(_fidx))[_vector_idx] = _val;
      }
    }
    else if ( object->dataType(DATA_TRIANGLE_MESH) ) 
    {
      PolyMesh* mesh = PluginFunctions::polyMesh(object);
      OpenMesh::FPropHandleT<PolyMesh::Point> prop;

      if ( mesh != 0 ) 
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_fidx >= 0 && _fidx < (int) mesh->n_faces())
            if( _vector_idx >= 0 && _vector_idx < 3)
              mesh->property(prop, mesh->face_handle(_fidx))[_vector_idx] = _val;
      }
    }
  }
}


//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
setHPropV3D(QString _property, int _hidx, int _vector_idx, double _val)
{
  BaseObjectData* object = getSelectedObject();

  if (object != 0)
  {
    if ( object->dataType(DATA_TRIANGLE_MESH) ) 
    {
      TriMesh* mesh = PluginFunctions::triMesh(object);
      OpenMesh::HPropHandleT<TriMesh::Point> prop;

      if ( mesh != 0 ) 
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_hidx >= 0 && _hidx < (int) mesh->n_halfedges())
            if( _vector_idx >= 0 && _vector_idx < 3)
              mesh->property(prop, mesh->halfedge_handle(_hidx))[_vector_idx] = _val;
      }
    }
    else if ( object->dataType(DATA_TRIANGLE_MESH) ) 
    {
      PolyMesh* mesh = PluginFunctions::polyMesh(object);
      OpenMesh::HPropHandleT<PolyMesh::Point> prop;

      if ( mesh != 0 ) 
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_hidx >= 0 && _hidx < (int) mesh->n_halfedges())
            if( _vector_idx >= 0 && _vector_idx < 3)
              mesh->property(prop, mesh->halfedge_handle(_hidx))[_vector_idx] = _val;
      }
    }
  }
}


//-----------------------------------------------------------------------------


BaseObjectData*
PropertyVisPlugin::
getSelectedObject()
{
  int id = tool_->meshNames->itemData( tool_->meshNames->currentIndex() ).toInt();
  BaseObjectData* object = 0;
  PluginFunctions::getObject( id, object );

  return object;
}


//-----------------------------------------------------------------------------


int
PropertyVisPlugin::
n_vertices()
{
  BaseObjectData* object = getSelectedObject();

  if(object)
  {
    if(object->dataType(DATA_TRIANGLE_MESH))
      return PluginFunctions::triMesh(object)->n_vertices();
    else if(object->dataType(DATA_POLY_MESH))
      return PluginFunctions::polyMesh(object)->n_vertices();
  }

  // return default
  return 0;
}


//-----------------------------------------------------------------------------


int
PropertyVisPlugin::
n_edges()
{
  BaseObjectData* object = getSelectedObject();

  if(object)
  {
    if(object->dataType(DATA_TRIANGLE_MESH))
      return PluginFunctions::triMesh(object)->n_edges();
    else if(object->dataType(DATA_POLY_MESH))
      return PluginFunctions::polyMesh(object)->n_edges();
  }

  // return default
  return 0;
}


//-----------------------------------------------------------------------------


int
PropertyVisPlugin::
n_faces()
{
  BaseObjectData* object = getSelectedObject();

  if(object)
  {
    if(object->dataType(DATA_TRIANGLE_MESH))
      return PluginFunctions::triMesh(object)->n_faces();
    else if(object->dataType(DATA_POLY_MESH))
      return PluginFunctions::polyMesh(object)->n_faces();
  }

  // return default
  return 0;
}


//-----------------------------------------------------------------------------


int
PropertyVisPlugin::
n_halfedges()
{
  BaseObjectData* object = getSelectedObject();

  if(object)
  {
    if(object->dataType(DATA_TRIANGLE_MESH))
      return PluginFunctions::triMesh(object)->n_halfedges();
    else if(object->dataType(DATA_POLY_MESH))
      return PluginFunctions::polyMesh(object)->n_halfedges();
  }

  // return default
  return 0;
}


//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
selectObjectByID( int _id)
{
  BaseObjectData* object = 0;
  PluginFunctions::getObject( _id, object );

  if (object == 0)
    return;
  else
    selectObjectByName( object->name() );
}

//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
selectObjectByName( QString _name)
{
  int cb_id = tool_->meshNames->findText( _name );
  
  if( cb_id != -1)
    tool_->meshNames->setCurrentIndex(cb_id);
}

//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
selectElementType( QString _etype)
{
  int cb_id = tool_->propertyType->findText( _etype);

  if( cb_id != -1)
    tool_->propertyType->setCurrentIndex(cb_id);
  else
    std::cerr << "Warning: selectElementType does not support Type " << _etype.toAscii().data() << std::endl;
}


//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
selectDataType   ( QString _dtype)
{
  int cb_id = tool_->propertyDataType->findText( _dtype);

  if( cb_id != -1)
    tool_->propertyDataType->setCurrentIndex(cb_id);
  else
    std::cerr << "Warning: selectElementType does not support Type " << _dtype.toAscii().data() << std::endl;
}


//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
setCurrentProp( int _element_idx, double _val, int _vector_idx)
{
  // get current property name
  if (!tool_->propertyName_lv->currentIndex().isValid()) return;
  const PropertyNameListModel::PROP_INFO &focusedProp = propertyNameListModel_[tool_->propertyName_lv->currentIndex().row()];

  QString pname = QString::fromStdString(focusedProp.propName());
  QString etype = tool_->propertyType->currentText();
  QString dtype = tool_->propertyDataType->currentText();

  if( etype == "Vertex")
  {
    if(dtype == "Double")
      setVProp<double>( pname, _element_idx, _val);
    else if(dtype == "Unsigned Integer")
      setVProp<unsigned int>( pname, _element_idx, int(_val));
    else if(dtype == "Integer")
      setVProp<int>( pname, _element_idx, int(_val));
    else if(dtype == "Bool")
      setVProp<bool>( pname, _element_idx, bool(_val));
    else if(dtype == "3D Vector")
      setVPropV3D( pname, _element_idx, _vector_idx, _val);
  }
  else if( etype == "Edge")
  {
    if(dtype == "Double")
      setEProp<double>( pname, _element_idx, _val);
    else if(dtype == "Unsigned Integer")
      setEProp<unsigned int>( pname, _element_idx, int(_val));
    else if(dtype == "Integer")
      setEProp<int>( pname, _element_idx, int(_val));
    else if(dtype == "Bool")
      setEProp<bool>( pname, _element_idx, bool(_val));
    else if(dtype == "3D Vector")
      setEPropV3D( pname, _element_idx, _vector_idx, _val);
  }
  else if( etype == "Face")
  {
    if(dtype == "Double")
      setFProp<double>( pname, _element_idx, _val);
    else if(dtype == "Unsigned Integer")
      setFProp<unsigned int>( pname, _element_idx, int(_val));
    else if(dtype == "Integer")
      setFProp<int>( pname, _element_idx, int(_val));
    else if(dtype == "Bool")
      setFProp<bool>( pname, _element_idx, bool(_val));
    else if(dtype == "3D Vector")
      setFPropV3D( pname, _element_idx, _vector_idx, _val);
  }
  else if( etype == "HalfEdge")
  {
    if(dtype == "Double")
      setHProp<double>( pname, _element_idx, _val);
    else if(dtype == "Unsigned Integer")
      setHProp<unsigned int>( pname, _element_idx, int(_val));
    else if(dtype == "Integer")
      setHProp<int>( pname, _element_idx, int(_val));
    else if(dtype == "Bool")
      setHProp<bool>( pname, _element_idx, bool(_val));
    else if(dtype == "3D Vector")
      setHPropV3D( pname, _element_idx, _vector_idx, _val);
  }
}

//-----------------------------------------------------------------------------


double
PropertyVisPlugin::
getCurrentProp( int _element_idx, int _vector_idx)
{
  // get current property name
  if (!tool_->propertyName_lv->currentIndex().isValid()) return 0;
  const PropertyNameListModel::PROP_INFO &focusedProp = propertyNameListModel_[tool_->propertyName_lv->currentIndex().row()];

  QString pname = QString::fromStdString(focusedProp.propName());
  QString etype = tool_->propertyType->currentText();
  QString dtype = tool_->propertyDataType->currentText();

  // get selected mesh
  BaseObjectData* object = getSelectedObject();

  if(object)
  {
    if(object->dataType(DATA_TRIANGLE_MESH))
    {
      TriMesh* mesh = PluginFunctions::triMesh(object);

      if( etype == "Vertex"   )
      {
        if( dtype == "Double")
        {
          OpenMesh::VPropHandleT<double> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_vertices())
            return mesh->property(prop, mesh->vertex_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Unsigned Integer")
        {
          OpenMesh::VPropHandleT<unsigned int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_vertices())
            return mesh->property(prop, mesh->vertex_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Integer")
        {
          OpenMesh::VPropHandleT<int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_vertices())
            return mesh->property(prop, mesh->vertex_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Bool")
        {
          OpenMesh::VPropHandleT<bool> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_vertices())
            return mesh->property(prop, mesh->vertex_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "3D Vector")
        {
          OpenMesh::VPropHandleT<TriMesh::Point> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_vertices())
            return mesh->property(prop, mesh->vertex_handle(_element_idx))[_vector_idx];
          else
            return 0;
        }
      }
      else if( etype == "Edge"     )
      {
        if( dtype == "Double")
        {
          OpenMesh::EPropHandleT<double> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_edges())
            return mesh->property(prop, mesh->edge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Unsigned Integer")
        {
          OpenMesh::EPropHandleT<unsigned int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_edges())
            return mesh->property(prop, mesh->edge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Integer")
        {
          OpenMesh::EPropHandleT<int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_edges())
            return mesh->property(prop, mesh->edge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Bool")
        {
          OpenMesh::EPropHandleT<bool> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_edges())
            return mesh->property(prop, mesh->edge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "3D Vector")
        {
          OpenMesh::EPropHandleT<TriMesh::Point> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_edges())
            return mesh->property(prop, mesh->edge_handle(_element_idx))[_vector_idx];
          else
            return 0;
        }
      }
      else if( etype == "Face"     )
      {
        if( dtype == "Double")
        {
          OpenMesh::FPropHandleT<double> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_faces())
            return mesh->property(prop, mesh->face_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Unsigned Integer")
        {
          OpenMesh::FPropHandleT<unsigned int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_faces())
            return mesh->property(prop, mesh->face_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Integer")
        {
          OpenMesh::FPropHandleT<int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_faces())
            return mesh->property(prop, mesh->face_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Bool")
        {
          OpenMesh::FPropHandleT<bool> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_faces())
            return mesh->property(prop, mesh->face_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "3D Vector")
        {
          OpenMesh::FPropHandleT<TriMesh::Point> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_faces())
            return mesh->property(prop, mesh->face_handle(_element_idx))[_vector_idx];
          else
            return 0;
        }
      }
      else if( etype == "HalfEdge" )
      {
        if( dtype == "Double")
        {
          OpenMesh::HPropHandleT<double> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_halfedges())
            return mesh->property(prop, mesh->halfedge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Unsigned Integer")
        {
          OpenMesh::HPropHandleT<unsigned int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_halfedges())
            return mesh->property(prop, mesh->halfedge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Integer")
        {
          OpenMesh::HPropHandleT<int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_halfedges())
            return mesh->property(prop, mesh->halfedge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Bool")
        {
          OpenMesh::HPropHandleT<bool> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_halfedges())
            return mesh->property(prop, mesh->halfedge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "3D Vector")
        {
          OpenMesh::HPropHandleT<TriMesh::Point> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_halfedges())
            return mesh->property(prop, mesh->halfedge_handle(_element_idx))[_vector_idx];
          else
            return 0;
        }
      }
    }
    else if(object->dataType(DATA_POLY_MESH))
    {
      PolyMesh* mesh = PluginFunctions::polyMesh(object);


      if( etype == "Vertex"   )
      {
        if( dtype == "Double")
        {
          OpenMesh::VPropHandleT<double> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_vertices())
            return mesh->property(prop, mesh->vertex_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Unsigned Integer")
        {
          OpenMesh::VPropHandleT<unsigned int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_vertices())
            return mesh->property(prop, mesh->vertex_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Integer")
        {
          OpenMesh::VPropHandleT<int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_vertices())
            return mesh->property(prop, mesh->vertex_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Bool")
        {
          OpenMesh::VPropHandleT<bool> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_vertices())
            return mesh->property(prop, mesh->vertex_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "3D Vector")
        {
          OpenMesh::VPropHandleT<TriMesh::Point> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_vertices())
            return mesh->property(prop, mesh->vertex_handle(_element_idx))[_vector_idx];
          else
            return 0;
        }
      }
      else if( etype == "Edge"     )
      {
        if( dtype == "Double")
        {
          OpenMesh::EPropHandleT<double> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_edges())
            return mesh->property(prop, mesh->edge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Unsigned Integer")
        {
          OpenMesh::EPropHandleT<unsigned int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_edges())
            return mesh->property(prop, mesh->edge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Integer")
        {
          OpenMesh::EPropHandleT<int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_edges())
            return mesh->property(prop, mesh->edge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Bool")
        {
          OpenMesh::EPropHandleT<bool> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_edges())
            return mesh->property(prop, mesh->edge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "3D Vector")
        {
          OpenMesh::EPropHandleT<TriMesh::Point> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_edges())
            return mesh->property(prop, mesh->edge_handle(_element_idx))[_vector_idx];
          else
            return 0;
        }
      }
      else if( etype == "Face"     )
      {
        if( dtype == "Double")
        {
          OpenMesh::FPropHandleT<double> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_faces())
            return mesh->property(prop, mesh->face_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Unsigned Integer")
        {
          OpenMesh::FPropHandleT<unsigned int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_faces())
            return mesh->property(prop, mesh->face_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Integer")
           {
             OpenMesh::FPropHandleT<int> prop;
             mesh->get_property_handle(prop, pname.toAscii().data());
             if( _element_idx >= 0 && _element_idx < (int)mesh->n_faces())
               return mesh->property(prop, mesh->face_handle(_element_idx));
             else
               return 0;
           }
        else if( dtype == "Bool")
        {
          OpenMesh::FPropHandleT<bool> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_faces())
            return mesh->property(prop, mesh->face_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "3D Vector")
        {
          OpenMesh::FPropHandleT<TriMesh::Point> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_faces())
            return mesh->property(prop, mesh->face_handle(_element_idx))[_vector_idx];
          else
            return 0;
        }
      }
      else if( etype == "HalfEdge" )
      {
        if( dtype == "Double")
        {
          OpenMesh::HPropHandleT<double> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_halfedges())
            return mesh->property(prop, mesh->halfedge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Unsigned Integer")
        {
          OpenMesh::HPropHandleT<unsigned int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_halfedges())
            return mesh->property(prop, mesh->halfedge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Integer")
        {
          OpenMesh::HPropHandleT<int> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_halfedges())
            return mesh->property(prop, mesh->halfedge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "Bool")
        {
          OpenMesh::HPropHandleT<bool> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_halfedges())
            return mesh->property(prop, mesh->halfedge_handle(_element_idx));
          else
            return 0;
        }
        else if( dtype == "3D Vector")
        {
          OpenMesh::HPropHandleT<TriMesh::Point> prop;
          mesh->get_property_handle(prop, pname.toAscii().data());
          if( _element_idx >= 0 && _element_idx < (int)mesh->n_halfedges())
            return mesh->property(prop, mesh->halfedge_handle(_element_idx))[_vector_idx];
          else
            return 0;
        }
      }
    }
  }
  else
  {
    std::cerr << "Warning: no mesh selected!!!\n";
    return 0.0;
  }

  return 0.0;
}


//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
loadSelectedProperty()
{
  QString etype = tool_->propertyType->currentText();
  QString dtype = tool_->propertyDataType->currentText();

  // get selected mesh
  BaseObjectData* object = getSelectedObject();

  int n = 0;
  
  if(object)
  {
    if(object->dataType(DATA_TRIANGLE_MESH))
    {
      TriMesh* mesh = PluginFunctions::triMesh(object);

      if     ( etype == "Vertex"   ) n = mesh->n_vertices();
      else if( etype == "Edge"     ) n = mesh->n_edges();
      else if( etype == "Face"     ) n = mesh->n_faces();
      else if( etype == "HalfEdge" ) n = mesh->n_halfedges();
    }
    else if(object->dataType(DATA_POLY_MESH))
    {
      PolyMesh* mesh = PluginFunctions::polyMesh(object);

      if     ( etype == "Vertex"   ) n = mesh->n_vertices();
      else if( etype == "Edge"     ) n = mesh->n_edges();
      else if( etype == "Face"     ) n = mesh->n_faces();
      else if( etype == "HalfEdge" ) n = mesh->n_halfedges();
    }
  }
  else
  {
    std::cerr << "Warning: no mesh selected!!!\n";
    return;
  }
  

  QString filename = getLoadFilename();
  openFile(filename);
  for(int i=0; i<n; ++i)
  {
    if(dtype == "Double")
    {
      double d = readDouble();
      setCurrentProp( i, d);
    }
    else if(dtype == "Unsigned Integer")
    {
      unsigned int d = readUnsignedInteger();
      setCurrentProp( i, d);
    }
    else if(dtype == "Integer")
    {
      int d = readInteger();
      setCurrentProp( i, d);
    }
    else if(dtype == "Bool")
    {
      bool d = readInteger();
      setCurrentProp( i, d);
    }
    else if(dtype == "3D Vector")
    {
      for(unsigned int j=0; j<3; ++j)
      {
	double d = readDouble();
	setCurrentProp( i, d, j);
      }
    }
  }    

  closeFile();
}


//-----------------------------------------------------------------------------


void
PropertyVisPlugin::
saveSelectedProperty()
{
  QString etype = tool_->propertyType->currentText();
  QString dtype = tool_->propertyDataType->currentText();

  // get selected mesh
  BaseObjectData* object = getSelectedObject();

  int n = 0;
  
  if(object)
  {
    if(object->dataType(DATA_TRIANGLE_MESH))
    {
      TriMesh* mesh = PluginFunctions::triMesh(object);

      if     ( etype == "Vertex"   ) n = mesh->n_vertices();
      else if( etype == "Edge"     ) n = mesh->n_edges();
      else if( etype == "Face"     ) n = mesh->n_faces();
      else if( etype == "HalfEdge" ) n = mesh->n_halfedges();
    }
    else if(object->dataType(DATA_POLY_MESH))
    {
      PolyMesh* mesh = PluginFunctions::polyMesh(object);

      if     ( etype == "Vertex"   ) n = mesh->n_vertices();
      else if( etype == "Edge"     ) n = mesh->n_edges();
      else if( etype == "Face"     ) n = mesh->n_faces();
      else if( etype == "HalfEdge" ) n = mesh->n_halfedges();
    }
  }
  else
  {
    std::cerr << "Warning: no mesh selected!!!\n";
    return;
  }
  

  QString filename = getSaveFilename();
  openFile(filename);
  for(int i=0; i<n; ++i)
  {
    if(dtype == "Double")
    {
      double d = getCurrentProp( i);
      writeDouble(d);
    }
    else if(dtype == "Unsigned Integer")
    {
      unsigned int d = getCurrentProp( i);
      writeUnsignedInteger(d);
    }
    else if(dtype == "Integer")
    {
      int d = getCurrentProp( i);
      writeInteger(d);
    }
    else if(dtype == "Bool")
    {
      bool d = getCurrentProp( i);
      writeInteger(d);
    }
    else if(dtype == "3D Vector")
    {
      for(unsigned int j=0; j<3; ++j)
      {
  	double d = getCurrentProp(i,j);
  	writeDouble(d);
      }
    }
  }    

  closeFile();
}

OpenMesh::Vec4f PropertyVisPlugin::convertColor(const QColor _color){

  OpenMesh::Vec4f color;

  color[0] = _color.redF();
  color[1] = _color.greenF();
  color[2] = _color.blueF();
  color[3] = _color.alphaF();

  return color;
}

void PropertyVisPlugin::slotDuplicateProperty() {
    int id = tool_->meshNames->itemData( tool_->meshNames->currentIndex() ).toInt();
    BaseObjectData* object = 0;
    PluginFunctions::getObject(id, object);

    if (object == 0){
      emit log(LOGERR, "Unable to get object");
      return;
    }

    TriMesh * const triMesh = (object->dataType(DATA_TRIANGLE_MESH) ? PluginFunctions::triMesh(object) : 0);
    PolyMesh * const polyMesh = (object->dataType(DATA_POLY_MESH) ? PluginFunctions::polyMesh(object) : 0);;

    if (triMesh == 0 && polyMesh == 0) {
        emit log(LOGERR,"Unable to get mesh");
        return;
    }

    QModelIndexList selectedIndices = tool_->propertyName_lv->selectionModel()->selectedIndexes();
    for (QModelIndexList::const_iterator it = selectedIndices.begin(), it_end = selectedIndices.end();
            it != it_end; ++it) {

        const PropertyNameListModel::PROP_INFO &currentProp = propertyNameListModel_[it->row()];

        if (triMesh) duplicateProperty(triMesh, currentProp);
        else if (polyMesh) duplicateProperty(polyMesh, currentProp);
    }

    slotMeshChanged();
}

void PropertyVisPlugin::slotRemoveProperty() {
    int id = tool_->meshNames->itemData( tool_->meshNames->currentIndex() ).toInt();
    BaseObjectData* object = 0;
    PluginFunctions::getObject(id, object);

    if (object == 0){
      emit log(LOGERR, "Unable to get object");
      return;
    }

    TriMesh * const triMesh = (object->dataType(DATA_TRIANGLE_MESH) ? PluginFunctions::triMesh(object) : 0);
    PolyMesh * const polyMesh = (object->dataType(DATA_POLY_MESH) ? PluginFunctions::polyMesh(object) : 0);;

    if (triMesh == 0 && polyMesh == 0) {
        emit log(LOGERR,"Unable to get mesh");
        return;
    }

    QModelIndexList selectedIndices = tool_->propertyName_lv->selectionModel()->selectedIndexes();
    for (QModelIndexList::const_iterator it = selectedIndices.begin(), it_end = selectedIndices.end();
            it != it_end; ++it) {

        const PropertyNameListModel::PROP_INFO &currentProp = propertyNameListModel_[it->row()];

        if (triMesh) removeProperty(triMesh, currentProp);
        else if (polyMesh) removeProperty(polyMesh, currentProp);
    }

    slotMeshChanged();
}

namespace {

template<typename MeshT, typename PropHandleT>
class CopyProperty {
    public:
        CopyProperty(const PropHandleT &p1, const PropHandleT &p2, MeshT &mesh) :
            p1(p1), p2(p2), mesh(mesh) {}

        template<typename PrimitiveHandleT>
        inline void operator() (const PrimitiveHandleT &pr) {
            mesh.property(p1, pr) = mesh.property(p2, pr);
        }

    private:
        const PropHandleT &p1, &p2;
        MeshT &mesh;
};

template<typename MeshT, typename PropHandleT, typename IteratorT>
inline void duplicateProperty_stage2(MeshT &mesh, const PropertyNameListModel::PROP_INFO &prop,
                                     IteratorT first, IteratorT last) {
    PropHandleT propHandle;
    if (!mesh.get_property_handle(propHandle, prop.propName())) return;

    std::string newPropertyName;
    for (int i = 1;; ++i) {
        std::ostringstream oss;
        oss << prop.propName() << " Copy " << i;
        newPropertyName = oss.str();

        PropHandleT tmp;
        if (!mesh.get_property_handle(tmp, newPropertyName)) break;
    }

    PropHandleT newProperty;
    mesh.add_property(newProperty, newPropertyName);
    std::for_each(first, last, CopyProperty<MeshT, PropHandleT>(newProperty, propHandle, mesh));
}

template<typename MeshT, typename PropType>
inline void duplicateProperty_stage1(MeshT &mesh, const PropertyNameListModel::PROP_INFO &prop) {
    if (prop.isEdgeProp())
        duplicateProperty_stage2<MeshT, OpenMesh::EPropHandleT<PropType>, typename MeshT::EdgeIter>(mesh, prop, mesh.edges_begin(), mesh.edges_end());
    else if (prop.isVertexProp())
        duplicateProperty_stage2<MeshT, OpenMesh::VPropHandleT<PropType>, typename MeshT::VertexIter>(mesh, prop, mesh.vertices_begin(), mesh.vertices_end());
    else if (prop.isFaceProp())
        duplicateProperty_stage2<MeshT, OpenMesh::FPropHandleT<PropType>, typename MeshT::FaceIter>(mesh, prop, mesh.faces_begin(), mesh.faces_end());
    else if (prop.isHalfedgeProp())
        duplicateProperty_stage2<MeshT, OpenMesh::HPropHandleT<PropType>, typename MeshT::HalfedgeIter>(mesh, prop, mesh.halfedges_begin(), mesh.halfedges_end());
}

template<typename MeshT, typename PropHandleT>
inline void removeProperty_stage2(MeshT &mesh, const PropertyNameListModel::PROP_INFO &prop) {
    PropHandleT propHandle;
    if (!mesh.get_property_handle(propHandle, prop.propName())) return;
    mesh.remove_property(propHandle);
}

template<typename MeshT, typename PropType>
inline void removeProperty_stage1(MeshT &mesh, const PropertyNameListModel::PROP_INFO &prop) {
    if (prop.isEdgeProp())
        removeProperty_stage2<MeshT, OpenMesh::EPropHandleT<PropType> >(mesh, prop);
    else if (prop.isVertexProp())
        removeProperty_stage2<MeshT, OpenMesh::VPropHandleT<PropType> >(mesh, prop);
    else if (prop.isFaceProp())
        removeProperty_stage2<MeshT, OpenMesh::FPropHandleT<PropType> >(mesh, prop);
    else if (prop.isHalfedgeProp())
        removeProperty_stage2<MeshT, OpenMesh::HPropHandleT<PropType> >(mesh, prop);
}

}

template<typename MeshT>
void PropertyVisPlugin::duplicateProperty(MeshT *mesh, const PropertyNameListModel::PROP_INFO &currentProp) {
    if ((currentProp.typeinfo() == PropertyNameListModel::proptype_Vec3d ||
            currentProp.typeinfo() == PropertyNameListModel::proptype_Vec3f))
        duplicateProperty_stage1<MeshT, typename MeshT::Point>(*mesh, currentProp);
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_double)
        duplicateProperty_stage1<MeshT, double>(*mesh, currentProp);
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_uint)
        duplicateProperty_stage1<MeshT, uint>(*mesh, currentProp);
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_int)
        duplicateProperty_stage1<MeshT, int>(*mesh, currentProp);
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_bool)
        duplicateProperty_stage1<MeshT, bool>(*mesh, currentProp);
#ifdef ENABLE_SKELETON_SUPPORT
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_SkinWeights)
        duplicateProperty_stage1<MeshT, BaseSkin::SkinWeights>(*mesh, currentProp);
#endif
}

template<typename MeshT>
void PropertyVisPlugin::removeProperty(MeshT *mesh, const PropertyNameListModel::PROP_INFO &currentProp) {
    if ((currentProp.typeinfo() == PropertyNameListModel::proptype_Vec3d ||
            currentProp.typeinfo() == PropertyNameListModel::proptype_Vec3f))
        removeProperty_stage1<MeshT, typename MeshT::Point>(*mesh, currentProp);
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_double)
        removeProperty_stage1<MeshT, double>(*mesh, currentProp);
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_uint)
        removeProperty_stage1<MeshT, uint>(*mesh, currentProp);
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_int)
        removeProperty_stage1<MeshT, int>(*mesh, currentProp);
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_bool)
        removeProperty_stage1<MeshT, bool>(*mesh, currentProp);
#ifdef ENABLE_SKELETON_SUPPORT
    else if (currentProp.typeinfo() == PropertyNameListModel::proptype_SkinWeights)
        removeProperty_stage1<MeshT, BaseSkin::SkinWeights>(*mesh, currentProp);
#endif
}

Q_EXPORT_PLUGIN2( propertyvisplugin , PropertyVisPlugin );

