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

#include <OpenFlipper/INIFile/INIFile.hh>

//-----------------------------------------------------------------------------

/** \brief Select all elements of each (target) object
 * 
 */
void SelectionPlugin::slotSelectAll()
{
  PluginFunctions::IteratorRestriction restriction;
  if ( !tool_->restrictOnTargets->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_ALL)) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      selectAll( o_it->id() );

  emit updateView();
}


//-----------------------------------------------------------------------------

/** \brief Clear the selection of each (target) object
 * 
 */
void SelectionPlugin::slotClearSelection()
{
  PluginFunctions::IteratorRestriction restriction;
  if ( !tool_->restrictOnTargets->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  // process all meshes
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_ALL )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      clearSelection( o_it->id() );

  emit updateView();
}


//-----------------------------------------------------------------------------

/** \brief Invert the selection of each (target) object
 * 
 */ 
void SelectionPlugin::slotInvertSelection() {
  PluginFunctions::IteratorRestriction restriction;
  if ( !tool_->restrictOnTargets->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  // process all meshes
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_ALL) ) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it) 
    if ( o_it->visible() )
      invertSelection( o_it->id() );

  emit updateView();
}


//-----------------------------------------------------------------------------

/** \brief Select the boundary of each (target) object
 * 
 */
void SelectionPlugin::slotSelectBoundary()
{
  PluginFunctions::IteratorRestriction restriction;
  if ( !tool_->restrictOnTargets->isChecked() )
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

/** \brief Shrink the selection of each (target) object
 * 
 */
void SelectionPlugin::slotShrinkSelection() {
  
  PluginFunctions::IteratorRestriction restriction;
  if ( !tool_->restrictOnTargets->isChecked() )
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

/** \brief Grow the selection of each (target) object
 * 
 */
void SelectionPlugin::slotGrowSelection() {
  PluginFunctions::IteratorRestriction restriction;
  if ( !tool_->restrictOnTargets->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else 
    restriction = PluginFunctions::TARGET_OBJECTS;
  
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
        o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( o_it->visible() )
      growSelection( o_it->id() );
  
  emit updateView();
}


//-----------------------------------------------------------------------------

/** \brief load a selection from ini file
  * 
  */
void SelectionPlugin::slotLoadSelection(){


  QString fileName = QFileDialog::getOpenFileName(0, tr("Open File"),"", tr("INI files (*.ini)"));

  if ( !fileName.isEmpty() ){

    INIFile ini;

    if ( ! ini.connect(fileName,false) ) {
      emit log(LOGERR,"Failed to load ini file " + fileName);
      return;
    }

    PluginFunctions::IteratorRestriction restriction;
    if ( !tool_->restrictOnTargets->isChecked() )
      restriction = PluginFunctions::ALL_OBJECTS;
    else 
      restriction = PluginFunctions::TARGET_OBJECTS;
    
    for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
          o_it != PluginFunctions::objectsEnd(); ++o_it)
      if ( o_it->visible() )
        loadIniFile(ini, o_it->id() );

    ini.disconnect();
  }

}


//-----------------------------------------------------------------------------

/** \brief Save a selection to ini file
  * 
  */
void SelectionPlugin::slotSaveSelection(){

  QString fileName = QFileDialog::getSaveFileName(0, tr("Save File"),"", tr("INI files (*.ini)"));

  if ( !fileName.isEmpty() ){

    INIFile ini;

    if ( ! ini.connect(fileName,true) ) {
      emit log(LOGERR,"Failed to save ini file " + fileName);
      return;
    }

    PluginFunctions::IteratorRestriction restriction;
    if ( !tool_->restrictOnTargets->isChecked() )
      restriction = PluginFunctions::ALL_OBJECTS;
    else 
      restriction = PluginFunctions::TARGET_OBJECTS;
    
    for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
          o_it != PluginFunctions::objectsEnd(); ++o_it)
      if ( o_it->visible() ){

        ini.add_section( o_it->name() );
        saveIniFile(ini, o_it->id() );
      }

    ini.disconnect();
  }
}
