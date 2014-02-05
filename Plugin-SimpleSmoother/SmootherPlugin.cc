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



#include "SmootherPlugin.hh"


#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

SmootherPlugin::SmootherPlugin() :
        iterationsSpinbox_(0)
{

}

SmootherPlugin::~SmootherPlugin()
{

}

void SmootherPlugin::initializePlugin()
{
   // Create the Toolbox Widget
   QWidget* toolBox = new QWidget();
   QGridLayout* layout = new QGridLayout(toolBox);

   QPushButton* smoothButton = new QPushButton("&Smooth",toolBox);
   smoothButton->setToolTip(tr("Smooths an Object using Laplacian Smoothing."));
   smoothButton->setWhatsThis(tr("Smooths an Object using Laplacian Smoothing. Use the Smooth Plugin for more options."));



   iterationsSpinbox_ =  new QSpinBox(toolBox) ;
   iterationsSpinbox_->setMinimum(1);
   iterationsSpinbox_->setMaximum(1000);
   iterationsSpinbox_->setSingleStep(1);
   iterationsSpinbox_->setToolTip(tr("The number of the smooting operations."));
   iterationsSpinbox_->setWhatsThis(tr("Give the number, how often the Laplacian Smoothing should modify the object."));

   QLabel* label = new QLabel("Iterations:");

   layout->addWidget( label             , 0, 0);
   layout->addWidget( smoothButton      , 1, 1);
   layout->addWidget( iterationsSpinbox_, 0, 1);

   layout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding,QSizePolicy::Expanding),2,0,1,2);

   connect( smoothButton, SIGNAL(clicked()), this, SLOT(simpleLaplace()) );

   QIcon* toolIcon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"smoother1.png");
   emit addToolbox( tr("Simple Smoother") , toolBox, toolIcon );
}

void SmootherPlugin::pluginsInitialized() {
    
    // Emit slot description
    emit setSlotDescription(tr("simpleLaplace(int)"),   tr("Smooth mesh using the Laplace operator with uniform weights."),
                            QStringList(tr("iterations")), QStringList(tr("Number of iterations")));
}

/** \brief simpleLaplace
 *
 *  Smooth mesh using the Laplace operator
 *  with uniform weights.
 */
void SmootherPlugin::simpleLaplace() {

    int iterations = 1;
    
    if(!OpenFlipper::Options::nogui()) {
        iterations = iterationsSpinbox_->value();
    }
    
    simpleLaplace(iterations);
}

/** \brief simpleLaplace
 *
 * Smooth mesh using the Laplace operator
 * with uniform weights.
 *
 * @param _iterations Number of iterations
 */
void SmootherPlugin::simpleLaplace(int _iterations) {
    
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {

    bool selectionExists = false;

    if ( o_it->dataType( DATA_TRIANGLE_MESH ) ) {

        // Get the mesh to work on
      TriMesh* mesh = PluginFunctions::triMesh(*o_it);

      // Property for the active mesh to store original point positions
      OpenMesh::VPropHandleT< TriMesh::Point > origPositions;

      // Add a property to the mesh to store original vertex positions
      mesh->add_property( origPositions, "SmootherPlugin_Original_Positions" );

      for ( int i = 0 ; i < _iterations ; ++i ) {

          // Copy original positions to backup ( in Vertex property )
          TriMesh::VertexIter v_it, v_end=mesh->vertices_end();
          for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it) {
            mesh->property( origPositions, *v_it ) = mesh->point(*v_it);
            // See if at least one vertex has been selected
            selectionExists |= mesh->status(*v_it).selected();
          }

          // Do one smoothing step (For each point of the mesh ... )
          for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it) {

            if(selectionExists && mesh->status(*v_it).selected() == false) {
              continue;
            }

            TriMesh::Point point = TriMesh::Point(0.0,0.0,0.0);

            // Flag, to skip boundary vertices
            bool skip = false;

            // ( .. for each Outoing halfedge .. )
            TriMesh::VertexOHalfedgeIter voh_it(*mesh,*v_it);
            for ( ; voh_it.is_valid(); ++voh_it ) {
                // .. add the (original) position of the Neighbour ( end of the outgoing halfedge )
                point += mesh->property( origPositions, mesh->to_vertex_handle(*voh_it) );

                // Check if the current Halfedge is a boundary halfedge
                // If it is, abort and keep the current vertex position
                if ( mesh->is_boundary( *voh_it ) ) {
                  skip = true;
                  break;
                }

            }

            // Devide by the valence of the current vertex
            point /= mesh->valence( *v_it );

            if ( ! skip ) {
                // Set new position for the mesh if its not on the boundary
                mesh->point(*v_it) = point;
            }
          }

      }// Iterations end

      // Remove the property
      mesh->remove_property( origPositions );

      mesh->update_normals();

      emit updatedObject( o_it->id(), UPDATE_GEOMETRY );
      
      // Create backup
      emit createBackup(o_it->id(), "Simple Smoothing", UPDATE_GEOMETRY );

   } else if ( o_it->dataType( DATA_POLY_MESH ) ) {

       // Get the mesh to work on
      PolyMesh* mesh = PluginFunctions::polyMesh(*o_it);

      // Property for the active mesh to store original point positions
      OpenMesh::VPropHandleT< TriMesh::Point > origPositions;

      // Add a property to the mesh to store original vertex positions
      mesh->add_property( origPositions, "SmootherPlugin_Original_Positions" );

      for ( int i = 0 ; i < _iterations ; ++i ) {

         // Copy original positions to backup ( in Vertex property )
         PolyMesh::VertexIter v_it, v_end=mesh->vertices_end();
         for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it) {
            mesh->property( origPositions, *v_it ) = mesh->point(*v_it);
            // See if at least one vertex has been selected
            selectionExists |= mesh->status(*v_it).selected();
         }

         // Do one smoothing step (For each point of the mesh ... )
         for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it) {

            if(selectionExists && mesh->status(*v_it).selected() == false) {
              continue;
            }

            PolyMesh::Point point = PolyMesh::Point(0.0,0.0,0.0);

            // Flag, to skip boundary vertices
            bool skip = false;

            // ( .. for each Outoing halfedge .. )
            PolyMesh::VertexOHalfedgeIter voh_it(*mesh,*v_it);
            for ( ; voh_it.is_valid(); ++voh_it ) {
               // .. add the (original) position of the Neighbour ( end of the outgoing halfedge )
               point += mesh->property( origPositions, mesh->to_vertex_handle(*voh_it) );

               // Check if the current Halfedge is a boundary halfedge
               // If it is, abort and keep the current vertex position
               if ( mesh->is_boundary( *voh_it ) ) {
                  skip = true;
                  break;
               }

            }

            // Devide by the valence of the current vertex
            point /= mesh->valence( *v_it );

            if ( ! skip ) {
               // Set new position for the mesh if its not on the boundary
               mesh->point(*v_it) = point;
            }
         }

      }// Iterations end

      // Remove the property
      mesh->remove_property( origPositions );

      mesh->update_normals();

      emit updatedObject( o_it->id() , UPDATE_GEOMETRY);
      
      // Create backup
      emit createBackup(o_it->id(), "Simple Smoothing", UPDATE_GEOMETRY);

    } else {

      emit log(LOGERR, "DataType not supported.");
    }
  }
  
  // Show script logging
  emit scriptInfo("simpleLaplace(" + QString::number(_iterations) + ")");
  
  emit updateView();
}


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( smootherplugin , SmootherPlugin );
#endif



