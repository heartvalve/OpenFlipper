/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



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


  QString fileName = QFileDialog::getOpenFileName(0, tr("Open File"),"", tr("Selections (*.ini *.sel)"));

  if ( !fileName.isEmpty() ){

    PluginFunctions::IteratorRestriction restriction;
    if ( !tool_->restrictOnTargets->isChecked() )
      restriction = PluginFunctions::ALL_OBJECTS;
    else
      restriction = PluginFunctions::TARGET_OBJECTS;

    //first check if it's a Flipper Selection
    QFile file(fileName);

    if (file.open(QFile::ReadOnly)) {
      QTextStream input(&file);

      if ( input.readLine().contains("Selection") ) {

        //load a Flipper Modeling Selection

        for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ; 
          o_it != PluginFunctions::objectsEnd(); ++o_it)
          if ( o_it->visible() )
            loadFlipperModelingSelection(o_it->id(), fileName );

        return;
      }
    }

    INIFile ini;

    if ( ! ini.connect(fileName,false) ) {
      emit log(LOGERR,tr("Failed to load ini file ") + fileName);
      return;
    }
    
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

  QStringList filters;
  filters << "OpenFlipper Selection (*.ini)" << "Flipper Selection (*.sel)";

  QFileDialog dialog(0);

  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setNameFilters(filters);
  dialog.setDefaultSuffix("ini");

  if ( !dialog.exec() )
    return;

  QString fileName = dialog.selectedFiles()[0];

  if ( !fileName.isEmpty() ){

    //write OpenFlipper Selection
    if (dialog.selectedNameFilter() == filters[0] ){

      INIFile ini;

      if ( ! ini.connect(fileName,true) ) {
        emit log(LOGERR,tr("Failed to save ini file ") + fileName);
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

    } else {
      // write Flipper Selection
      bool saved = false;

      for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
            o_it != PluginFunctions::objectsEnd(); ++o_it)
        if ( o_it->visible() ){

          if ( !saved ){
            saveFlipperModelingSelection(o_it->id(), fileName );
            saved = true;
          } else {
            emit log(LOGWARN, tr("SaveSelection: Only selection of the first target was saved."));
            return;
          }
        }
    }
  }
}
