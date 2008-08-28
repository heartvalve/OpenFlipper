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




 
#include <QtGui>
 #include <QSpacerItem>

#include "SmootherPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh> 

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

 bool SmootherPlugin::initializeToolbox(QWidget*& _widget)
{
   // Create the Toolbox Widget
   tool_ = new QWidget();  
   _widget = tool_;
   QSize size(300, 300);
   tool_->resize(size); 
   MeshDialogLayout_ = new QGridLayout( tool_); 
   
   
   smoothButton_ = new QPushButton("&Smooth",tool_);
   
   iterationsSpinbox_ =  new QSpinBox(tool_) ;
   iterationsSpinbox_->setMinimum(1);
   iterationsSpinbox_->setMaximum(1000);
   iterationsSpinbox_->setSingleStep(1);
   
   smootherTypeBox_ = new QComboBox();
   smootherTypeBox_->addItem("Simple Laplace");
  
   MeshDialogLayout_->addWidget( smootherTypeBox_,0 , 0 , 1 , 2 , Qt::AlignTop );
   MeshDialogLayout_->addWidget( smoothButton_     , 1, 0 , Qt::AlignTop );
   MeshDialogLayout_->addWidget( iterationsSpinbox_, 1, 1 , Qt::AlignTop );
   MeshDialogLayout_->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding,QSizePolicy::Expanding),2,0,1,2);
   
   connect(smoothButton_,SIGNAL(clicked()),this,SLOT(slotSmooth()));
   
   return true;
}

void SmootherPlugin::simpleLaplace() {
   
   
   for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DATA_TRIANGLE_MESH) ; 
                                         o_it != PluginFunctions::objects_end(); ++o_it)  {
       // Get the mesh to work on
      TriMesh* mesh = PluginFunctions::triMesh(*o_it);
   
      // Add a property to the mesh to store original vertex positions
      mesh->add_property( orig_pos_, "SmootherPlugin_Original_Positions" );
      
      for ( int i = 0 ; i < iterationsSpinbox_->value() ; ++i ) {
      
         // Copy original positions to backup ( in Vertex property )
         TriMesh::VertexIter v_it, v_end=mesh->vertices_end();
         for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it) {
            mesh->property(  orig_pos_ , v_it ) = mesh->point(v_it);
         }
         
         // Do one smoothing step (For each point of the mesh ... )
         for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it) {
            
            TriMesh::Point point = TriMesh::Point(0.0,0.0,0.0);
            
            // Flag, to skip boundary vertices
            bool skip = false;
            
            // ( .. for each Outoing halfedge .. )
            TriMesh::VertexOHalfedgeIter voh_it(*mesh,v_it);
            for ( ; voh_it; ++voh_it ) {
               // .. add the (original) position of the Neighbour ( end of the outgoing halfedge )
               point += mesh->property( orig_pos_ , mesh->to_vertex_handle(voh_it) );
               
               // Check if the current Halfedge is a boundary halfedge
               // If it is, abort and keep the current vertex position
               if ( mesh->is_boundary( voh_it.handle() ) ) {
                  skip = true;
                  break;  
               }
                  
            }
            
            // Devide by the valence of the current vertex
            point /= mesh->valence( v_it );
            
            if ( ! skip ) {
               // Set new position for the mesh if its not on the boundary
               mesh->point(v_it) = point;
            }
         }
         
      }// Iterations end
      
      // Remove the property
      mesh->remove_property( orig_pos_);
      
      mesh->update_normals();
   }
   
     for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DATA_POLY_MESH) ; 
                                         o_it != PluginFunctions::objects_end(); ++o_it)  {
       // Get the mesh to work on
      PolyMesh* mesh = dynamic_cast< MeshObject< PolyMesh,DATA_POLY_MESH >* > (*o_it)->mesh();
   
      // Add a property to the mesh to store original vertex positions
      mesh->add_property( orig_pos_, "SmootherPlugin_Original_Positions" );
      
      for ( int i = 0 ; i < iterationsSpinbox_->value() ; ++i ) {
      
         // Copy original positions to backup ( in Vertex property )
         PolyMesh::VertexIter v_it, v_end=mesh->vertices_end();
         for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it) {
            mesh->property(  orig_pos_ , v_it ) = mesh->point(v_it);
         }
         
         // Do one smoothing step (For each point of the mesh ... )
         for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it) {
            
            PolyMesh::Point point = PolyMesh::Point(0.0,0.0,0.0);
            
            // Flag, to skip boundary vertices
            bool skip = false;
            
            // ( .. for each Outoing halfedge .. )
            PolyMesh::VertexOHalfedgeIter voh_it(*mesh,v_it);
            for ( ; voh_it; ++voh_it ) {
               // .. add the (original) position of the Neighbour ( end of the outgoing halfedge )
               point += mesh->property( orig_pos_ , mesh->to_vertex_handle(voh_it) );
               
               // Check if the current Halfedge is a boundary halfedge
               // If it is, abort and keep the current vertex position
               if ( mesh->is_boundary( voh_it.handle() ) ) {
                  skip = true;
                  break;  
               }
                  
            }
            
            // Devide by the valence of the current vertex
            point /= mesh->valence( v_it );
            
            if ( ! skip ) {
               // Set new position for the mesh if its not on the boundary
               mesh->point(v_it) = point;
            }
         }
         
      }// Iterations end
      
      // Remove the property
      mesh->remove_property( orig_pos_);
      
      mesh->update_normals();
   }
   
   emit update_view(); 
}

 void SmootherPlugin::slotSmooth()
{
  switch (smootherTypeBox_->currentIndex() ) {
    case 0 :  
       simpleLaplace();
       break;
     default : 
        std::cerr << "Smoother : Smoother type error" << std::endl;
  }
}


Q_EXPORT_PLUGIN2( smootherplugin , SmootherPlugin );

