/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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

#include "MeshObjectSelectionPlugin.hh"

#include <iostream>

#include <MeshTools/MeshSelectionT.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//=========================================================
//==== Vertex selections
//=========================================================

void MeshObjectSelectionPlugin::selectVertices(int _objectId, IdList _vertexList) {
    
    if(_vertexList.empty() ) return;
    
    BaseObjectData* object = 0;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("selectVertices: unable to get object")); 
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::selectVertices(PluginFunctions::triMesh(object), _vertexList);
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::selectVertices(PluginFunctions::polyMesh(object), _vertexList);
    else {
        emit log(LOGERR,tr("selectAllVertices: Unsupported object Type")); 
        return;
    }

    QString selection = "selectVertices(ObjectId(" + QString::number(_objectId) + "), [ " + QString::number(_vertexList[0]);

    for (uint i = 1 ; i < _vertexList.size(); ++i) {
        selection +=  ", " + QString::number(_vertexList[i]);
    }

    selection += " ])";

    emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
    emit scriptInfo(selection);
}

//=========================================================

void MeshObjectSelectionPlugin::unselectVertices(int _objectId, IdList _vertexList) {
    
    if(_vertexList.empty() ) return;
    
    BaseObjectData* object = 0;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("unselectVertices: unable to get object")); 
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::unselectVertices(PluginFunctions::triMesh(object), _vertexList);
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::unselectVertices(PluginFunctions::polyMesh(object), _vertexList);
    else {
        emit log(LOGERR,tr("unselectVertices: Unsupported object Type")); 
        return;
    }

    QString selection = "unselectVertices(ObjectId(" + QString::number(_objectId) + "), [ " + QString::number(_vertexList[0]);

    for (uint i = 1 ; i < _vertexList.size(); ++i) {
        selection +=  ", " + QString::number(_vertexList[i]);
    }

    selection += " ])";

    emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
    emit scriptInfo(selection);
}


//=========================================================


void MeshObjectSelectionPlugin::selectAllVertices(int _objectId) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("selectAllVertices: unable to get object")); 
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::selectAllVertices(PluginFunctions::triMesh(object));
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::selectAllVertices(PluginFunctions::polyMesh(object));
    else {
        emit log(LOGERR,tr("selectAllVertices: Unsupported object Type")); 
        return;
    }

    emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
    emit scriptInfo("selectAllVertices(ObjectId(" + QString::number(_objectId) + "))");
}

//=========================================================

void MeshObjectSelectionPlugin::clearVertexSelection(int _objectId) {
    
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("clearVertexSelection: unable to get object")); 
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::clearVertexSelection(PluginFunctions::triMesh(object));
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::clearVertexSelection(PluginFunctions::polyMesh(object));
    else {
        emit log(LOGERR,tr("clearVertexSelection: Unsupported object Type")); 
        return;
    }

    emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
    emit scriptInfo("clearVertexSelection(ObjectId(" + QString::number(_objectId) + "))");
}

//=========================================================

void MeshObjectSelectionPlugin::invertVertexSelection(int _objectId) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("invertVertexSelection: unable to get object")); 
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::invertVertexSelection(PluginFunctions::triMesh(object));
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::invertVertexSelection(PluginFunctions::polyMesh(object));
    else {
        emit log(LOGERR,tr("invertVertexSelection: Unsupported object Type")); 
        return;
    }

    emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
    emit scriptInfo("invertVertexSelection(ObjectId(" + QString::number(_objectId) + "))");
}

//=========================================================

void MeshObjectSelectionPlugin::selectBoundaryVertices(int _objectId) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("selectBoundaryVertices: unable to get object")); 
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::selectBoundaryVertices(PluginFunctions::triMesh(object));
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::selectBoundaryVertices(PluginFunctions::polyMesh(object));
    else {
        emit log(LOGERR,tr("selectBoundaryVertices: Unsupported object Type")); 
        return;
    }

    emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
    emit scriptInfo("selectBoundaryVertices(ObjectId(" + QString::number(_objectId) + "))");
}

//=========================================================

void MeshObjectSelectionPlugin::selectClosestBoundaryVertices(int _objectId, int _vertexId) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("selectClosestBoundaryVertices: unable to get object")); 
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH) {
        closestBoundarySelection(PluginFunctions::triMesh(object),_vertexId, vertexType_, false);
    } else if (object->dataType() == DATA_POLY_MESH) {
        closestBoundarySelection(PluginFunctions::polyMesh(object),_vertexId, vertexType_, false);
    } else {
        emit log(LOGERR,tr("selectClosestBoundaryVertices: Unsupported object Type")); 
        return;
    }

    emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
    emit scriptInfo("selectClosestBoundaryVertices(ObjectId(" + QString::number(_objectId) + "), VertexId)");
}

//=========================================================

void MeshObjectSelectionPlugin::shrinkVertexSelection(int _objectId) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("shrinkVertexSelection: unable to get object")); 
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::shrinkVertexSelection(PluginFunctions::triMesh(object));
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::shrinkVertexSelection(PluginFunctions::polyMesh(object));
    else {
        emit log(LOGERR,tr("shrinkVertexSelection: Unsupported object Type")); 
        return;
    }

    emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
    emit scriptInfo("shrinkVertexSelection(ObjectId(" + QString::number(_objectId) + "))");
}

//=========================================================

void MeshObjectSelectionPlugin::growVertexSelection(int _objectId) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("growVertexSelection: unable to get object")); 
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::growVertexSelection(PluginFunctions::triMesh(object));
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::growVertexSelection(PluginFunctions::polyMesh(object));
    else {
        emit log(LOGERR,tr("growVertexSelection: Unsupported object Type")); 
        return;
    }

    emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
    emit scriptInfo("growVertexSelection(ObjectId(" + QString::number(_objectId) + "))");
}

//=========================================================

IdList MeshObjectSelectionPlugin::getVertexSelection(int _objectId) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("getVertexSelection: unable to get object")); 
        return IdList(0);
    }

    emit scriptInfo("getVertexSelection(ObjectId(" + QString::number(_objectId) + "))");

    if (object->dataType() == DATA_TRIANGLE_MESH)
        return MeshSelection::getVertexSelection(PluginFunctions::triMesh(object));
    else if (object->dataType() == DATA_POLY_MESH)
        return MeshSelection::getVertexSelection(PluginFunctions::polyMesh(object));
    else {
        emit log(LOGERR,tr("getVertexSelection: Unsupported object Type")); 
        return IdList(0);
    }

    return IdList(0);
}

//=========================================================
  
void MeshObjectSelectionPlugin::deleteVertexSelection(int _objectId) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("deleteVertexSelection: unable to get object"));
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        deleteSelection(PluginFunctions::triMesh(object), vertexType_);
    else if (object->dataType() == DATA_POLY_MESH)
        deleteSelection(PluginFunctions::polyMesh(object), vertexType_);
    else {
        emit log(LOGERR,tr("deleteVertexSelection: Unsupported object Type"));
        return;
    }

    emit updatedObject(object->id(), UPDATE_ALL);
    emit scriptInfo("deleteVertexSelection(ObjectId(" + QString::number(_objectId) + "))");
}

//=========================================================

int MeshObjectSelectionPlugin::createMeshFromVertexSelection( int _objectId) {
  return createMeshFromSelection(_objectId, vertexType_ );
}

//=========================================================

/// colorize the vertex selection
void MeshObjectSelectionPlugin::colorizeVertexSelection(int _objectId, int r, int g, int b, int a) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId, object)) {
        emit log(LOGERR,"colorizeVertexSelection: unable to get object");
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH) {
        colorizeSelection(PluginFunctions::triMesh(object), vertexType_, r, g, b, a);
    } else if (object->dataType() == DATA_POLY_MESH) {
        colorizeSelection(PluginFunctions::polyMesh(object), vertexType_, r, g, b, a);
    } else {
        emit log(LOGERR,"colorizeVertexSelection: Unsupported object Type");
        return;
    }

    emit scriptInfo("colorizeVertexSelection(ObjectId(" + QString::number(_objectId) + "), "
              + QString::number(r) + ", " + QString::number(g) + ", " + QString::number(b) + ")");

    emit updatedObject(_objectId, UPDATE_COLOR);
}

//=========================================================
//==== Modeling Area selections
//=========================================================

void MeshObjectSelectionPlugin::selectHandleVertices(int _objectId, IdList _vertexList) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("selectHandleVertices: unable to get object")); 
        return;
    }

    if (_vertexList.empty() )
        return;

    if (object->dataType() == DATA_TRIANGLE_MESH){
        MeshSelection::setArea(PluginFunctions::triMesh(object), _vertexList, HANDLEAREA, true);
        update_regions(PluginFunctions::triMesh(object));
    } else if (object->dataType() == DATA_POLY_MESH){
        MeshSelection::setArea(PluginFunctions::polyMesh(object), _vertexList, HANDLEAREA, true);
        update_regions(PluginFunctions::polyMesh(object));
    } else {
        emit log(LOGERR,tr("selectHandleVertices: Unsupported object Type")); 
        return;
    }

    QString selection = "selectHandleVertices(ObjectId(" + QString::number(_objectId) + "), [ " + QString::number(_vertexList[0]);

    for (uint i = 1 ; i < _vertexList.size(); ++i) {
        selection +=  ", " + QString::number(_vertexList[i]);
    }

    selection += " ])";

    emit updatedObject(object->id(), UPDATE_ALL);
    emit scriptInfo(selection);
}

//=========================================================

void MeshObjectSelectionPlugin::unselectHandleVertices(int _objectId, IdList _vertexList) {
    
    if(_vertexList.empty()) return;
  
    BaseObjectData* object = 0;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("unselectHandleVertices: unable to get object")); 
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::setArea(PluginFunctions::triMesh(object), _vertexList, HANDLEAREA, false);
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::setArea(PluginFunctions::polyMesh(object), _vertexList, HANDLEAREA, false);
    else {
        emit log(LOGERR,tr("unselectHandleVertices: Unsupported object Type")); 
        return;
    }

    QString selection = "unselectHandleVertices(ObjectId(" + QString::number(_objectId) + "), [ " + QString::number(_vertexList[0]);

    for (uint i = 1 ; i < _vertexList.size(); ++i) {
        selection +=  ", " + QString::number(_vertexList[i]);
    }

    selection += " ])";

    emit updatedObject(object->id(), UPDATE_ALL);
    emit scriptInfo(selection);
}

//=========================================================

void MeshObjectSelectionPlugin::clearHandleVertices(int _objectId) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("clearHandleVertices: unable to get object")); 
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::setArea(PluginFunctions::triMesh(object), HANDLEAREA, false);
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::setArea(PluginFunctions::polyMesh(object), HANDLEAREA, false);
    else {
        emit log(LOGERR,tr("clearHandleVertices: Unsupported object Type")); 
    return;
    }

    emit updatedObject(object->id(), UPDATE_ALL);
    emit scriptInfo("clearHandleVertices(ObjectId(" + QString::number(_objectId) + "))");
}

//=========================================================

void MeshObjectSelectionPlugin::setAllHandleVertices(int _objectId) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("setAllHandleVertices: unable to get object")); 
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::setArea(PluginFunctions::triMesh(object), HANDLEAREA, true);
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::setArea(PluginFunctions::polyMesh(object), HANDLEAREA, true);
    else {
        emit log(LOGERR,tr("setAllHandleVertices: Unsupported object Type")); 
        return;
    }

    emit updatedObject(object->id(), UPDATE_ALL);
    emit scriptInfo("setAllHandleVertices(ObjectId(" + QString::number(_objectId) + "))");
}

//=========================================================

IdList MeshObjectSelectionPlugin::getHandleVertices(int _objectId) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("getHandleVertices: unable to get object")); 
        return IdList(0);
    }

    emit scriptInfo("getHandleVertices(ObjectId(" + QString::number(_objectId) + "))");

    if (object->dataType() == DATA_TRIANGLE_MESH)
        return MeshSelection::getArea(PluginFunctions::triMesh(object), HANDLEAREA);
    else if (object->dataType() == DATA_POLY_MESH)
        return MeshSelection::getArea(PluginFunctions::polyMesh(object), HANDLEAREA);
    else {
        emit log(LOGERR,tr("getHandleVertices: Unsupported object Type")); 
        return IdList(0);;
    }

    return IdList(0);
}

//=========================================================
//==== Modeling Area selections
//=========================================================

void MeshObjectSelectionPlugin::selectModelingVertices(int _objectId, IdList _vertexList) {
    
    if(_vertexList.empty() ) return;
  
    BaseObjectData* object = 0;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("selectModelingVertices: unable to get object"));
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH) {
        MeshSelection::setArea(PluginFunctions::triMesh(object), _vertexList, AREA, true);
        update_regions(PluginFunctions::triMesh(object));
    } else if (object->dataType() == DATA_POLY_MESH){
        MeshSelection::setArea(PluginFunctions::polyMesh(object), _vertexList, AREA, true);
        update_regions(PluginFunctions::polyMesh(object));
    } else {
        emit log(LOGERR,tr("selectModelingVertices: Unsupported object Type"));
        return;
    }

    QString selection = "selectModelingVertices(ObjectId(" + QString::number(_objectId) + "), [ " + QString::number(_vertexList[0]);

    for (uint i = 1 ; i < _vertexList.size(); ++i) {
        selection +=  ", " + QString::number(_vertexList[i]);
    }

    selection += " ])";

    emit updatedObject(object->id(), UPDATE_ALL);
    emit scriptInfo(selection);
}

//=========================================================

void MeshObjectSelectionPlugin::unselectModelingVertices(int _objectId, IdList _vertexList) {
    
    if(_vertexList.empty() ) return;
  
    BaseObjectData* object = 0;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("unselectModelingVertices: unable to get object"));
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::setArea(PluginFunctions::triMesh(object), _vertexList, AREA, false);
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::setArea(PluginFunctions::polyMesh(object), _vertexList, AREA, false);
    else{
        emit log(LOGERR,tr("unselectModelingVertices: Unsupported object Type"));
        return;
    }

    QString selection = "unselectModelingVertices(ObjectId(" + QString::number(_objectId) + "), [ " + QString::number(_vertexList[0]);

    for (uint i = 1 ; i < _vertexList.size(); ++i) {
        selection +=  ", " + QString::number(_vertexList[i]);
    }

    selection += " ])";

    emit updatedObject(object->id(), UPDATE_ALL);
    emit scriptInfo(selection);
}

// =========================================================

void MeshObjectSelectionPlugin::selectVerticesByValue(int _objectId, QString _component, bool _greater, double _value ) {
  BaseObjectData* object = 0;
  if (!PluginFunctions::getObject(_objectId,object)) {
    emit log(LOGERR,tr("selectVerticesByValue: unable to get object"));
    return;
  }

  if (object->dataType() == DATA_TRIANGLE_MESH)
    selectVerticesByValue(PluginFunctions::triMesh(object), _component,  _greater,  _value);
  else if (object->dataType() == DATA_POLY_MESH)
    selectVerticesByValue(PluginFunctions::polyMesh(object), _component,  _greater,  _value);
  else{
    emit log(LOGERR,tr("selectVerticesByValue: Unsupported object Type"));
    return;
  }

  emit updatedObject(object->id(), UPDATE_SELECTION);

  /// \todo emit scriptinfo

}


//=========================================================

void MeshObjectSelectionPlugin::clearModelingVertices(int _objectId) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("clearModelingVertices: unable to get object"));
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::setArea(PluginFunctions::triMesh(object), AREA, false);
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::setArea(PluginFunctions::polyMesh(object), AREA, false);
    else{
        emit log(LOGERR,tr("clearModelingVertices: Unsupported object Type"));
        return;
    }

    emit updatedObject(object->id(), UPDATE_ALL);
    emit scriptInfo("clearModelingVertices(ObjectId(" + QString::number(_objectId) + "))");
}

//=========================================================

void MeshObjectSelectionPlugin::setAllModelingVertices(int _objectId) {
  
    BaseObjectData* object;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("setAllModelingVertices: unable to get object"));
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        MeshSelection::setArea(PluginFunctions::triMesh(object), AREA, true);
    else if (object->dataType() == DATA_POLY_MESH)
        MeshSelection::setArea(PluginFunctions::polyMesh(object), AREA, true);
    else{
        emit log(LOGERR,tr("setAllModelingVertices: Unsupported object Type"));
        return;
    }

    emit updatedObject(object->id(), UPDATE_ALL);
    emit scriptInfo("setAllModelingVertices(ObjectId(" + QString::number(_objectId) + "))");
}

//=========================================================
IdList MeshObjectSelectionPlugin::getModelingVertices(int _objectId) {
  BaseObjectData* object;
  if (!PluginFunctions::getObject(_objectId,object)) {
    emit log(LOGERR,tr("getModelingVertices: unable to get object"));
    return IdList(0);
  }

  emit scriptInfo("getModelingVertices(ObjectId(" + QString::number(_objectId) + "))");

  if (object->dataType() == DATA_TRIANGLE_MESH)
      return MeshSelection::getArea(PluginFunctions::triMesh(object), AREA);
  else if (object->dataType() == DATA_POLY_MESH)
      return MeshSelection::getArea(PluginFunctions::polyMesh(object), AREA);
  else{
      emit log(LOGERR,tr("getModelingVertices: Unsupported object Type"));
      return IdList(0);
  }

  return IdList(0);
}

//=========================================================

void MeshObjectSelectionPlugin::loadFlipperModelingSelection(int _objectId, QString _filename) {
    
    QFile file(_filename);

    if (!file.exists()) {
        emit log(LOGERR,tr("Unable to find file: ") + _filename);
        return;
    }

    if (file.open(QFile::ReadOnly)) {
        
        QTextStream input(&file);
        QString header = input.readLine();

        if (!header.contains("Selection")) {
            emit log(LOGERR,tr("Wrong file header!should be Selection but is ") + header);
            return;
        }

        header = input.readLine();

        bool ok = false;
        uint vertexCount = header.toUInt(&ok);

        if (!ok) {
            emit log(LOGERR,tr("Unable to parse header. Cant get vertex count from string: ") + header);
            return;
        }

        //compare VertexCount
        BaseObjectData* object;
        if (!PluginFunctions::getObject(_objectId, object)) {
            emit log(LOGERR,tr("loadSelection: unable to get object"));
            return;
        }

        if (object->dataType() == DATA_TRIANGLE_MESH) {
            if (PluginFunctions::triMesh(object)->n_vertices() != vertexCount)
                return;
        } else if (object->dataType() == DATA_POLY_MESH) {
            if (PluginFunctions::polyMesh(object)->n_vertices() != vertexCount)
                return;
        } else {
            return;
        }

        //get selected handles
        IdList handleVertices;
        IdList modelingVertices;

        uint vertexId = 0;

        do {
            // Split into two substrings
            QStringList inputList = input.readLine().split(" ");

            if (inputList.size() != 2) {
                emit log(LOGERR,tr("Unable to parse entry at vertex index ") + QString::number(vertexId));
                return;
            }

            if (inputList[0] == "1")
            modelingVertices.push_back(vertexId);

            if (inputList[1] == "1")
            handleVertices.push_back(vertexId);

            ++vertexId;

        } while (!input.atEnd());

        clearModelingVertices(_objectId);
        selectModelingVertices(_objectId,modelingVertices);

        clearHandleVertices(_objectId);
        selectHandleVertices(_objectId,handleVertices);

    } else {
        emit log(LOGERR,tr("Unable to open selection file!"));
    }
}

//=========================================================

void MeshObjectSelectionPlugin::saveFlipperModelingSelection(int _objectId, QString _filename) {
  
    QFile file(_filename);

    if (file.open(QFile::WriteOnly)) {
        QTextStream input(&file);

        //get the object
        BaseObjectData* object;
        if (!PluginFunctions::getObject(_objectId,object)) {
            emit log(LOGERR,tr("saveFlipperModelingSelection: unable to get object"));
            return;
        }

        if (object->dataType() == DATA_TRIANGLE_MESH) {
            TriMesh* mesh = PluginFunctions::triMesh(object);

            //header
            input << "Selection" << endl;
            input << mesh->n_vertices() << endl;

            std::vector< int > modelingVertices = MeshSelection::getArea(mesh, AREA);
            std::vector< int > handleVertices   = MeshSelection::getArea(mesh, HANDLEAREA);

            std::vector< bool > modelingAll(mesh->n_vertices(), false);
            std::vector< bool > handleAll(mesh->n_vertices(), false);

            for (uint i=0; i < modelingVertices.size(); i++)
                modelingAll[ modelingVertices[i] ] = true;

            for (uint i=0; i < handleVertices.size(); i++)
                handleAll[ handleVertices[i] ] = true;

            for (uint i=0; i < mesh->n_vertices(); i++)
                input << (int) modelingAll[i] << " " << (int) handleAll[i] << endl;

        } else if (object->dataType() == DATA_POLY_MESH){

            PolyMesh* mesh = PluginFunctions::polyMesh(object);

            //header
            input << "Selection" << endl;
            input << mesh->n_vertices() << endl;

            std::vector< int > modelingVertices = MeshSelection::getArea(mesh, AREA);
            std::vector< int > handleVertices   = MeshSelection::getArea(mesh, HANDLEAREA);

            std::vector< bool > modelingAll(mesh->n_vertices(), false);
            std::vector< bool > handleAll(mesh->n_vertices(), false);

            for (uint i=0; i < modelingVertices.size(); i++)
                modelingAll[ modelingVertices[i] ] = true;

            for (uint i=0; i < handleVertices.size(); i++)
                handleAll[ handleVertices[i] ] = true;

            for (uint i=0; i < mesh->n_vertices(); i++)
                input << (int) modelingAll[i] << " " << (int) handleAll[i] << endl;

        } else {
            emit log(LOGERR, tr("saveFlipperModelingSelection: Unsupported Type."));
        }
    } else {
        emit log(LOGERR,tr("Unable to open selection file!"));
    }
}
