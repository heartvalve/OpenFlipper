/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 9089 $                                                       *
 *   $Author: kremer $                                                       *
 *   $Date: 2010-04-21 18:42:47 +0200 (Mi, 21. Apr 2010) $                   *
 *                                                                           *
\*===========================================================================*/

//=============================================================================
//
//  CLASS Core - IMPLEMENTATION of Comunication with plugins
//
//=============================================================================


//== INCLUDES =================================================================

#include "Core.hh"

//== IMPLEMENTATION ==========================================================

//========================================================================================
// ===             Selection Communication                    ============================
//========================================================================================

void Core::slotAddSelectionEnvironment(QString _modeName, QString _description, QString _icon, QString& _handleName) {
    emit addSelectionEnvironment(_modeName, _description, _icon, _handleName);
}

void Core::slotRegisterType(QString _handleName, DataType _type) {
    emit registerType(_handleName, _type);
}

void Core::slotAddPrimitiveType(QString _handleName, QString _name, QString _icon, SelectionInterface::PrimitiveType& _typeHandle) {
    emit addPrimitiveType(_handleName, _name, _icon, _typeHandle);
}

void Core::slotAddCustomSelectionMode(QString _handleName, QString _modeName, QString _description, QString _icon,
                                      SelectionInterface::PrimitiveType _associatedTypes, QString& _customIdentifier) {
    emit addCustomSelectionMode(_handleName, _modeName, _description, _icon, _associatedTypes, _customIdentifier);
}

void Core::slotAddCustomSelectionMode(QString _handleName, QString _modeName, QString _description, QString _icon,
                                      SelectionInterface::PrimitiveType _associatedTypes, QString& _customIdentifier,
                                      DataType _objectTypeRestriction) {
    emit addCustomSelectionMode(_handleName, _modeName, _description, _icon, _associatedTypes, _customIdentifier, _objectTypeRestriction);
}

void Core::slotAddSelectionOperations(QString _handleName,QStringList _operationsList, QString _category, SelectionInterface::PrimitiveType _type) {
    emit addSelectionOperations(_handleName, _operationsList, _category, _type);
}
   
void Core::slotSelectionOperation(QString _operation) {
    emit selectionOperation(_operation);
}

void Core::slotShowToggleSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    emit showToggleSelectionMode(_handleName, _show, _associatedTypes);
}

void Core::slotShowLassoSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    emit showLassoSelectionMode(_handleName, _show, _associatedTypes);
}

void Core::slotShowVolumeLassoSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    emit showVolumeLassoSelectionMode(_handleName, _show, _associatedTypes);
}

void Core::slotShowSurfaceLassoSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    emit showSurfaceLassoSelectionMode(_handleName, _show, _associatedTypes);
}

void Core::slotShowSphereSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    emit showSphereSelectionMode(_handleName, _show, _associatedTypes);
}

void Core::slotShowClosestBoundarySelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    emit showClosestBoundarySelectionMode(_handleName, _show, _associatedTypes);
}

void Core::slotShowFloodFillSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    emit showFloodFillSelectionMode(_handleName, _show, _associatedTypes);
}

void Core::slotShowComponentsSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    emit showComponentsSelectionMode(_handleName, _show, _associatedTypes);
}

void Core::slotToggleSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {
    emit toggleSelection(_event, _currentType, _deselect);
}

void Core::slotLassoSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {
    emit lassoSelection(_event, _currentType, _deselect);
}

void Core::slotVolumeLassoSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {
    emit volumeLassoSelection(_event, _currentType, _deselect);
}

void Core::slotSurfaceLassoSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {
    emit surfaceLassoSelection(_event, _currentType, _deselect);
}

void Core::slotSphereSelection(QMouseEvent* _event, double _radius, SelectionInterface::PrimitiveType _currentType, bool _deselect) {
    emit sphereSelection(_event, _radius, _currentType, _deselect);
}

void Core::slotClosestBoundarySelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {
    emit closestBoundarySelection(_event, _currentType, _deselect);
}

void Core::slotFloodFillSelection(QMouseEvent* _event, double _maxAngle, SelectionInterface::PrimitiveType _currentType, bool _deselect) {
    emit floodFillSelection(_event, _maxAngle, _currentType, _deselect);
}

void Core::slotComponentsSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {
    emit componentsSelection(_event, _currentType, _deselect);
}

void Core::slotCustomSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, QString _customIdentifier, bool _deselect) {
    emit customSelection(_event, _currentType, _customIdentifier, _deselect);
}

void Core::slotGetActiveDataTypes(SelectionInterface::TypeList& _types) {
    emit getActiveDataTypes(_types);
}

void Core::slotGetActivePrimitiveType(SelectionInterface::PrimitiveType& _type) {
    emit getActivePrimitiveType(_type);
}

void Core::slotTargetObjectsOnly(bool& _targetsOnly) {
    emit targetObjectsOnly(_targetsOnly);
}

void Core::slotLoadSelection(const INIFile& _file) {
    emit loadSelection(_file);
}

void Core::slotSaveSelection(INIFile& _file) {
    emit saveSelection(_file);
}

void Core::slotRegisterKeyShortcut(int _key, Qt::KeyboardModifiers _modifiers) {
    emit registerKeyShortcut(_key, _modifiers);
}

void Core::slotKeyShortcutEvent(int _key, Qt::KeyboardModifiers _modifiers) {
    emit keyShortcutEvent(_key, _modifiers);
}
