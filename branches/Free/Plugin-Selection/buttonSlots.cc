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




#include "SelectionPlugin.hh"

#include <iostream>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//=========================================================
//==== Vertex selections
//=========================================================

void SelectionPlugin::slotSelectAll()
{
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_POLY_LINE)) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      selectAll( o_it->id() );

  emit updateView();
}

//-----------------------------------------------------------------------------

void SelectionPlugin::slotClearSelection()
{
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  // process all meshes
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_POLY_LINE )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      clearSelection( o_it->id() );

  emit updateView();
}

//-----------------------------------------------------------------------------

 
void SelectionPlugin::slotInvertSelection() {
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  // process all meshes
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_POLY_LINE )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it) 
    if ( o_it->visible() )
      invertSelection( o_it->id() );

  emit updateView();
}

//-----------------------------------------------------------------------------

void SelectionPlugin::slotSelectBoundary()
{
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      selectBoundary( o_it->id() );
  
  emit updateView();
}

//-----------------------------------------------------------------------------

void SelectionPlugin::slotShrinkSelection() {
  
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      shrinkSelection( o_it->id() );
  
  emit updateView();
}

//-----------------------------------------------------------------------------

void SelectionPlugin::slotGrowSelection() {
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      growSelection( o_it->id() );
  
  emit updateView();
}

//=========================================================
//==== Vertex selections
//=========================================================

void SelectionPlugin::slotSelectAllVertices()
{
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_POLY_LINE)) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      selectAllVertices( o_it->id() );

  emit updateView();
}

//-----------------------------------------------------------------------------

void SelectionPlugin::slotClearVertexSelection()
{
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  // process all meshes
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_POLY_LINE )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      clearVertexSelection( o_it->id() );

  emit updateView();
}

//-----------------------------------------------------------------------------

 
void SelectionPlugin::slotInvertVertexSelection() {
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  // process all meshes
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_POLY_LINE )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      invertVertexSelection( o_it->id() );

  emit updateView();
}

//-----------------------------------------------------------------------------

void SelectionPlugin::slotSelectBoundaryVertices()
{
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      selectBoundaryVertices( o_it->id() );
  
  emit updateView();
}

//-----------------------------------------------------------------------------

void SelectionPlugin::slotShrinkVertexSelection() {
  
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      shrinkVertexSelection( o_it->id() );
  
  emit updateView();
}

//-----------------------------------------------------------------------------

void SelectionPlugin::slotGrowVertexSelection() {
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      growVertexSelection( o_it->id() );
  
  emit updateView();
}


//=========================================================
//==== Face selections
//=========================================================

void SelectionPlugin::slotSelectAllFaces()
{
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      selectAllFaces( o_it->id() );
  
  emit updateView();
}

//------------------------------------------------------------------------------

void SelectionPlugin::slotClearFaceSelection()
{
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      clearFaceSelection( o_it->id() );
  
  emit updateView();
}

//-----------------------------------------------------------------------------


void SelectionPlugin::slotInvertFaceSelection() {
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      invertFaceSelection( o_it->id() );
  
  emit updateView();
}

//------------------------------------------------------------------------------

void SelectionPlugin::slotSelectBoundaryFaces()
{
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      selectBoundaryFaces( o_it->id() );
  
  emit updateView();
}

//-----------------------------------------------------------------------------

void SelectionPlugin::slotShrinkFaceSelection() {
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      shrinkFaceSelection( o_it->id() );
  
  emit updateView();
}

//-----------------------------------------------------------------------------


void SelectionPlugin::slotGrowFaceSelection() {
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      growFaceSelection( o_it->id() );
  
  emit updateView();
}


//-----------------------------------------------------------------------------



