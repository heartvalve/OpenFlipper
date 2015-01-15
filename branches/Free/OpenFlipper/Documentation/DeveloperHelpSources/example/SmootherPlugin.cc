//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================



#include "SmootherPlugin.hh"


#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

SmootherPlugin::SmootherPlugin() :
        iterationsSpinbox_(0)
{

}

void SmootherPlugin::initializePlugin()
{
  // Create the Toolbox Widget
  QWidget* toolBox = new QWidget();

  QPushButton* smoothButton = new QPushButton("&Smooth", toolBox);

  iterationsSpinbox_ = new QSpinBox(toolBox);
  iterationsSpinbox_->setMinimum(1);
  iterationsSpinbox_->setMaximum(1000);
  iterationsSpinbox_->setSingleStep(1);

  QLabel* label = new QLabel("Iterations:");

  QGridLayout* layout = new QGridLayout(toolBox);

  layout->addWidget(label, 0, 0);
  layout->addWidget(smoothButton, 1, 1);
  layout->addWidget(iterationsSpinbox_, 0, 1);

  layout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding), 2, 0, 1, 2);

  connect(smoothButton, SIGNAL(clicked()), this, SLOT(simpleLaplace()));

  emit addToolbox(tr("Smoother"), toolBox);
}

/** \brief
 *
 */
void SmootherPlugin::simpleLaplace()
{

  for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS); o_it != PluginFunctions::objectsEnd();
      ++o_it) {

    if (o_it->dataType(DATA_TRIANGLE_MESH)) {

      // Get the mesh to work on
      TriMesh* mesh = PluginFunctions::triMesh(*o_it);

      // Property for the active mesh to store original point positions
      OpenMesh::VPropHandleT<TriMesh::Point> origPositions;

      // Add a property to the mesh to store original vertex positions
      mesh->add_property(origPositions, "SmootherPlugin_Original_Positions");

      for (int i = 0; i < iterationsSpinbox_->value(); ++i) {

        // Copy original positions to backup ( in vertex property )
        TriMesh::VertexIter v_it, v_end = mesh->vertices_end();
        for (v_it = mesh->vertices_begin(); v_it != v_end; ++v_it) {
          mesh->property(origPositions, v_it) = mesh->point(v_it);
        }

        // Do one smoothing step (For each point of the mesh ... )
        for (v_it = mesh->vertices_begin(); v_it != v_end; ++v_it) {

          TriMesh::Point point = TriMesh::Point(0.0, 0.0, 0.0);

          // Flag, to skip boundary vertices
          bool skip = false;

          // ( .. for each outgoing halfedge .. )
          TriMesh::VertexOHalfedgeIter voh_it(*mesh, v_it);

          for (; voh_it; ++voh_it) {

            // .. add the (original) position of the neighbour ( end of the outgoing halfedge )
            point += mesh->property(origPositions, mesh->to_vertex_handle(voh_it));

            // Check if the current Halfedge is a boundary halfedge
            // If it is, abort and keep the current vertex position
            if (mesh->is_boundary(*voh_it)) {
              skip = true;
              break;
            }
          }

          // Devide by the valence of the current vertex
          point /= mesh->valence(v_it);

          if (!skip) {
            // Set new position for the mesh if its not on the boundary
            mesh->point(v_it) = point;
          }
        }

      }	    // Iterations end

      // Remove the property
      mesh->remove_property(origPositions);

      mesh->update_normals();

      emit updatedObject(o_it->id(),UPDATE_GEOMETRY);

    } else if (o_it->dataType(DATA_POLY_MESH)) {

      // Get the mesh to work on
      PolyMesh* mesh = PluginFunctions::polyMesh(*o_it);

      // Property for the active mesh to store original point positions
      OpenMesh::VPropHandleT<PolyMesh::Point> origPositions;

      // Add a property to the mesh to store original vertex positions
      mesh->add_property(origPositions, "SmootherPlugin_Original_Positions");

      for (int i = 0; i < iterationsSpinbox_->value(); ++i) {

        // Copy original positions to backup ( in Vertex property )
        PolyMesh::VertexIter v_it, v_end = mesh->vertices_end();
        for (v_it = mesh->vertices_begin(); v_it != v_end; ++v_it) {
          mesh->property(origPositions, v_it) = mesh->point(v_it);
        }

        // Do one smoothing step (For each point of the mesh ... )
        for (v_it = mesh->vertices_begin(); v_it != v_end; ++v_it) {

          PolyMesh::Point point = PolyMesh::Point(0.0, 0.0, 0.0);

          // Flag, to skip boundary vertices
          bool skip = false;

          // ( .. for each Outoing halfedge .. )
          PolyMesh::VertexOHalfedgeIter voh_it(*mesh, v_it);
          for (; voh_it; ++voh_it) {
            // .. add the (original) position of the Neighbour ( end of the outgoing halfedge )
            point += mesh->property(origPositions, mesh->to_vertex_handle(voh_it));

            // Check if the current Halfedge is a boundary halfedge
            // If it is, abort and keep the current vertex position
            if (mesh->is_boundary(*voh_it)) {
              skip = true;
              break;
            }

          }

          // Devide by the valence of the current vertex
          point /= mesh->valence(v_it);

          if (!skip) {
            // Set new position for the mesh if its not on the boundary
            mesh->point(v_it) = point;
          }
        }

      }	    // Iterations end

      // Remove the property
      mesh->remove_property(origPositions);

      mesh->update_normals();

      emit updatedObject(o_it->id(),UPDATE_GEOMETRY);

    } else {

      emit log(LOGERR, "Data type not supported.");

    } // Switch data type
  }
}

Q_EXPORT_PLUGIN2( smootherplugin, SmootherPlugin);

