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
*   $Revision: 11270 $                                                       *
*   $LastChangedBy: wilden $                                                *
*   $Date: 2011-04-11 15:13:21 +0200 (Mo, 11 Apr 2011) $                     *
*                                                                            *
\*===========================================================================*/


//== INCLUDES =================================================================

#include "DecimaterPlugin.hh"
#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include <string>
#include <sstream>

//== IMPLEMENTATION ===========================================================

/** \brief initialization of the plugin
 *
 */
void DecimaterPlugin::initializePlugin() {
  tool_ = new DecimaterToolbarWidget();

  // connect signals to slots
  connect(tool_->cbDistance, SIGNAL(stateChanged(int)), this, SLOT(cbUpdateDistance(int)));
  connect(tool_->cbNormalDev, SIGNAL(stateChanged(int)), this, SLOT(cbUpdateNormalDev(int)));
  connect(tool_->cbAspectRatio, SIGNAL(stateChanged(int)), this, SLOT(cbUpdateAspectRatio(int)));
  connect(tool_->cbEdgeLength, SIGNAL(stateChanged(int)), this, SLOT(cbUpdateEdgeLength(int)));
  connect(tool_->cbNumTriangles, SIGNAL(stateChanged(int)), this, SLOT(cbUpdateNumTriangles(int)));

  connect(tool_->normalDev, SIGNAL(valueChanged(int)), this, SLOT(updateNormalDev(int)));
  connect(tool_->normalDevSlider, SIGNAL(valueChanged(int)), this, SLOT(updateNormalDev(int)));
  connect(tool_->aspectRatio, SIGNAL(valueChanged(double)), this, SLOT(updateAspectRatio(double)));
  connect(tool_->aspectRatioSlider, SIGNAL(valueChanged(int)), this, SLOT(updateAspectRatio(int)));
  connect(tool_->numTriangles, SIGNAL(valueChanged(int)), this, SLOT(updateNumTriangles(int)));
  connect(tool_->numTrianglesSlider, SIGNAL(valueChanged(int)), this, SLOT(updateNumTriangles(int)));

  connect(tool_->pbDecimate, SIGNAL(clicked()), this, SLOT(slot_decimate()));

  // Force update if the Toolbox gets visible
  connect(tool_, SIGNAL(showing()), this, SLOT(slotUpdateNumTriangles()));

  toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"decimater.png");
  emit addToolbox(QString("Decimater"), tool_, toolIcon_);

}

//-----------------------------------------------------------------------------

/** \brief gets and sets the current maximum number of triangles
 *
 */
void DecimaterPlugin::slotUpdateNumTriangles() {
  // only update if the tool is visible
  if (!tool_->isVisible())
    return;

  uint max = 0;

  for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DataType(DATA_TRIANGLE_MESH));
                                       o_it != PluginFunctions::objectsEnd(); ++o_it) {
    TriMesh* mesh = PluginFunctions::triMesh(o_it->id());
    max = std::max(mesh->n_faces(), max);
  }

  tool_->numTriangles->setMinimum(1);
  tool_->numTriangles->setMaximum(max);
  tool_->numTrianglesSlider->setMinimum(1);
  tool_->numTrianglesSlider->setMaximum(max);
}


//-----------------------------------------------------------------------------

/** \brief enables and disables slider and/or spinbox
 *
 */
void DecimaterPlugin::cbUpdateDistance(int _state) {
  if (_state == Qt::Checked)
    tool_->distance->setEnabled(true);
  else if (_state == Qt::Unchecked)
    tool_->distance->setEnabled(false);
}


//-----------------------------------------------------------------------------

/** \brief enables and disables slider and/or spinbox
 *
 */
void DecimaterPlugin::cbUpdateNormalDev(int _state) {
  if (_state == Qt::Checked) {
    tool_->normalDev->setEnabled(true);
    tool_->normalDevSlider->setEnabled(true);
  } else if (_state == Qt::Unchecked) {
    tool_->normalDev->setEnabled(false);
    tool_->normalDevSlider->setEnabled(false);
  }
}


//-----------------------------------------------------------------------------

/** \brief enables and disables slider and/or spinbox
 *
 */
void DecimaterPlugin::cbUpdateAspectRatio(int _state) {
  if (_state == Qt::Checked) {
    tool_->aspectRatio->setEnabled(true);
    tool_->aspectRatioSlider->setEnabled(true);
  } else if (_state == Qt::Unchecked) {
    tool_->aspectRatio->setEnabled(false);
    tool_->aspectRatioSlider->setEnabled(false);
  }
}


//-----------------------------------------------------------------------------

/** \brief enables and disables slider and/or spinbox
 *
 */
void DecimaterPlugin::cbUpdateEdgeLength(int _state) {
  if (_state == Qt::Checked)
    tool_->edgeLength->setEnabled(true);
  else if (_state == Qt::Unchecked)
    tool_->edgeLength->setEnabled(false);
}


//-----------------------------------------------------------------------------

/** \brief enables and disables slider and/or spinbox
 *
 */
void DecimaterPlugin::cbUpdateNumTriangles(int _state) {
  if (_state == Qt::Checked) {
    tool_->numTriangles->setEnabled(true);
    tool_->numTrianglesSlider->setEnabled(true);
  } else if (_state == Qt::Unchecked) {
    tool_->numTriangles->setEnabled(false);
    tool_->numTrianglesSlider->setEnabled(false);
  }
}

//-----------------------------------------------------------------------------

void DecimaterPlugin::slotObjectSelectionChanged(int /*_identifier*/)
{
  slotUpdateNumTriangles();
}

//-----------------------------------------------------------------------------

void DecimaterPlugin::slotObjectUpdated(int /*_identifier*/ , const UpdateType _type )
{
  if (_type.contains(UPDATE_TOPOLOGY))
    slotUpdateNumTriangles();
}

//-----------------------------------------------------------------------------

/** \brief syncs slider and spinbox
 *
 */
void DecimaterPlugin::updateNormalDev(int _value) {
  tool_->normalDev->setValue(_value);
  tool_->normalDevSlider->setValue(_value);
}

//-----------------------------------------------------------------------------

/** \brief syncs slider and spinbox
 *
 */
void DecimaterPlugin::updateAspectRatio(double _value) {
  tool_->aspectRatioSlider->setValue((int)(_value * 100));
}

//-----------------------------------------------------------------------------

/** \brief syncs slider and spinbox
 *
 */
void DecimaterPlugin::updateAspectRatio(int _value) {
  tool_->aspectRatio->setValue((double)_value / 100);
}

//-----------------------------------------------------------------------------

/** \brief syncs slider and spinbox
 *
 */
void DecimaterPlugin::updateNumTriangles(int _value) {
  tool_->numTriangles->setValue(_value);
  tool_->numTrianglesSlider->setValue(_value);
}

//-----------------------------------------------------------------------------

/** \brief decimation called from the toolbox
 *
 */
void DecimaterPlugin::slot_decimate() {
  for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DATA_TRIANGLE_MESH);
                                       o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    // initialize
    TriMeshObject* pObject = PluginFunctions::triMeshObject(*o_it);

    if (pObject == 0)
      emit log(LOGWARN, tr("Unable to get object"));

    TriMesh* mesh = PluginFunctions::triMesh(*o_it);

    if ((uint)tool_->numTriangles->value() >= mesh->n_faces()) return;

    DecimaterType decimater_object(*mesh);

    // constraint handles for decimation
    ModQuadricH hModQuatric;
    ModNormalFlippingH hModNormalFlipping;
    ModHausdorffH hModHausdorff;
    ModNormalDeviationH hModNormalDeviation;
    ModAspectRatioH hModAspectRatio;
    ModEdgeLengthH hModEdgeLength;

    // remove old constraints first
    decimater_object.remove(hModQuatric);
    decimater_object.remove(hModHausdorff);
    decimater_object.remove(hModNormalDeviation);
    decimater_object.remove(hModNormalFlipping);
    decimater_object.remove(hModAspectRatio);
    decimater_object.remove(hModEdgeLength);

    // priority module: quadrics, normal dev or edge length
    if (tool_->rbByDistance->isChecked()) {
      decimater_object.add(hModQuatric);
      decimater_object.module(hModQuatric).set_binary(false);
    } else if (tool_->rbByNormalDeviation->isChecked()) {
      decimater_object.add(hModNormalDeviation);
      decimater_object.module(hModNormalDeviation).set_binary(false);
    } else if (tool_->rbByEdgeLength->isChecked()) {
      decimater_object.add(hModEdgeLength);
      decimater_object.module(hModEdgeLength).set_binary(false);
    }


    // binary modules (order wrt. computation cost)
    if (tool_->cbEdgeLength->isChecked())  decimater_object.add(hModEdgeLength);
    if (tool_->cbAspectRatio->isChecked()) decimater_object.add(hModAspectRatio);
    if (tool_->cbNormalDev->isChecked())   decimater_object.add(hModNormalDeviation);
    else                                   decimater_object.add(hModNormalFlipping);
    if (tool_->cbDistance->isChecked())    decimater_object.add(hModHausdorff);


    // init decimater
    if (!decimater_object.initialize()) {
      emit log(LOGWARN, tr("Decimater could not be initialized"));
      continue;
    }

    // go over checkboxes
    if (!tool_->cbDistance->isChecked()) tool_->distance->setValue(-1.0);
    if (!tool_->cbNormalDev->isChecked()) tool_->normalDev->setValue(-1);
    if (!tool_->cbAspectRatio->isChecked()) tool_->aspectRatio->setValue(-1.0);
    if (!tool_->cbEdgeLength->isChecked()) tool_->edgeLength->setValue(-1.0);
    if (!tool_->cbNumTriangles->isChecked()) tool_->numTriangles->setValue(1);

    // Hausdorff error
    if (tool_->distance->value() >= 0.0 && hModHausdorff.is_valid())
      decimater_object.module(hModHausdorff).set_tolerance(tool_->distance->value());

    // normal deviation
    if (tool_->normalDev->value() >= 0.0 && hModNormalDeviation.is_valid())
      decimater_object.module(hModNormalDeviation).set_normal_deviation(tool_->normalDev->value());

    // roundness
    if (tool_->aspectRatio->value() >= 0.0 && hModAspectRatio.is_valid())
      decimater_object.module(hModAspectRatio).set_roundness(tool_->aspectRatio->value());

    // edge length
    if (tool_->edgeLength->value() >= 0.0 && hModEdgeLength.is_valid())
      decimater_object.module(hModEdgeLength).set_edge_length(tool_->edgeLength->value());

    // if something is selected: lock un-selected vertices
    TriMesh::VertexIter  v_it, v_end(mesh->vertices_end());
    bool something_selected = false;
    for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it) {
      if (mesh->status(v_it).selected()) {
        something_selected = true;
        break;
      }
    }
    if (something_selected)
      for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it)
        mesh->status(v_it).set_locked(!mesh->status(v_it).selected());

    std::ostringstream infostream;
    decimater_object.info(infostream);
    std::cout << infostream.str();

    // decimate
    decimater_object.decimate_to_faces(0, tool_->numTriangles->value());

    // unlock again
    if (something_selected)
      for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it)
            mesh->status(v_it).set_locked(false);

    pObject->mesh()->garbage_collection();
    pObject->mesh()->update_normals();
    pObject->update();

    emit createBackup(o_it->id(), "Decimation");
    emit updatedObject(o_it->id(), UPDATE_TOPOLOGY);
  }

  emit updateView();
}

//-----------------------------------------------------------------------------

Q_EXPORT_PLUGIN2(DecimaterPlugin, DecimaterPlugin);
