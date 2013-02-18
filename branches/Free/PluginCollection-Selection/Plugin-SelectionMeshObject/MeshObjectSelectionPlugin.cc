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
 *   $Revision$                                                      *
 *   $Author$                                                       *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include "MeshObjectSelectionPlugin.hh"

#include <set>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <MeshTools/MeshSelectionT.hh>

// Primitive type icons
#define VERTEX_TYPE         "selection_vertex.png"
#define EDGE_TYPE           "selection_edge.png"
#define HEDGE_TYPE          "selection_halfedge.png"
#define FACE_TYPE           "selection_face.png"
// =======================================
// Define operations
// =======================================
// General:
#define G_CLEAR_HANDLE  "Clear Handle Region"
#define G_CLEAR_MODEL   "Clear Modeling Region"
#define G_CONVERT       "Convert Selection"
// Vertices:
#define V_SELECT_ALL    "Select All Vertices"
#define V_CLEAR         "Clear Vertex Selection"
#define V_INVERT        "Invert Vertex Selection"
#define V_BOUNDARY      "Select Boundary Vertices"
#define V_SHRINK        "Shrink Vertex Selection"
#define V_GROW          "Grow Vertex Selection"
#define V_DELETE        "Delete selected Vertices"
#define V_COLORIZE      "Colorize selected Vertices"
#define V_COPYSELECTION "Create mesh from Vertex Selection"
#define V_HANDLE        "Set to Handle Region"
#define V_MODELING      "Set to Modeling Region"
#define V_LOAD_FLIPPER  "Load Flipper Selection"
// Edges
#define E_SELECT_ALL    "Select All Edges"
#define E_CLEAR         "Clear Edge Selection"
#define E_INVERT        "Invert Edge Selection"
#define E_DELETE        "Delete selected Edges"
#define E_BOUNDARY      "Select Boundary Edges"
#define E_COLORIZE      "Colorize selected Edges"
#define E_COPYSELECTION "Create mesh from Edge Selection"
// Halfedges
#define HE_SELECT_ALL   "Select All Halfedges"
#define HE_CLEAR        "Clear Halfedge Selection"
#define HE_INVERT       "Invert Halfedge Selection"
#define HE_BOUNDARY     "Select Boundary Halfedges"
#define HE_COLORIZE     "Colorize selected Halfedges"
// Faces
#define F_SELECT_ALL    "Select All Faces"
#define F_CLEAR         "Clear Face Selection"
#define F_INVERT        "Invert Face Selection"
#define F_DELETE        "Delete selected Faces"
#define F_BOUNDARY      "Select Boundary Faces"
#define F_SHRINK        "Shrink Face Selection"
#define F_GROW          "Grow Face Selection"
#define F_COLORIZE      "Colorize selected Faces"
#define F_COPYSELECTION "Create mesh from Face Selection"

/// Default constructor
MeshObjectSelectionPlugin::MeshObjectSelectionPlugin() :
vertexType_(0),
edgeType_(0),
halfedgeType_(0),
faceType_(0),
allSupportedTypes_(0u),
conversionDialog_(0) {
}
      
MeshObjectSelectionPlugin::~MeshObjectSelectionPlugin() {
    
    delete conversionDialog_;
}

void MeshObjectSelectionPlugin::initializePlugin() {

    // Tell core about all scriptable slots
    updateSlotDescriptions();
    
    // Create conversion dialog
    if(!OpenFlipper::Options::nogui()) {
        conversionDialog_ = new ConversionDialog(0);
        QRect scr = QApplication::desktop()->screenGeometry();
        conversionDialog_->setGeometry((int)scr.width()/2-(int)conversionDialog_->width()/2,
                                       (int)scr.height()/2-(int)conversionDialog_->height()/2,
                                       conversionDialog_->width(), conversionDialog_->height());
        conversionDialog_->hide();
        connect(conversionDialog_->convertButton, SIGNAL(clicked()), this, SLOT(conversionRequested()));
        // Fill in combo boxes
        conversionDialog_->convertFromBox->addItems(
            QString("Vertex Selection;Edge Selection;Halfedge Selection;Face Selection;" \
                    "Feature Vertices;Feature Edges;Feature Faces;Handle Region;Modeling Region").split(";"));
        conversionDialog_->convertToBox->addItems(
            QString("Vertex Selection;Edge Selection;Halfedge Selection;Face Selection;" \
                    "Feature Vertices;Feature Edges;Feature Faces;Handle Region;Modeling Region").split(";"));
    }
}

void MeshObjectSelectionPlugin::pluginsInitialized() {
    // Create new selection environment for mesh objects
    // and register mesh types tri- and polymeshes for
    // the environment.
    
    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    
    emit addSelectionEnvironment("Mesh Object Selections", "Select mesh object primitives such as vertices, (half-)edges and faces.",
                                 iconPath + "selections.png", environmentHandle_);

    // Register mesh object types
    emit registerType(environmentHandle_, DATA_POLY_MESH);
    emit registerType(environmentHandle_, DATA_TRIANGLE_MESH);
    
    // Register mesh primitive types
    emit addPrimitiveType(environmentHandle_, "Select Vertices",     iconPath + VERTEX_TYPE, vertexType_);
    emit addPrimitiveType(environmentHandle_, "Select Edges",        iconPath + EDGE_TYPE,   edgeType_);
    emit addPrimitiveType(environmentHandle_, "Select Halfedges",    iconPath + HEDGE_TYPE,  halfedgeType_);
    emit addPrimitiveType(environmentHandle_, "Select Faces",        iconPath + FACE_TYPE,   faceType_);
    
    // Combine all supported types
    allSupportedTypes_ = (vertexType_ | edgeType_ | halfedgeType_ | faceType_);
    
    // Determine, which selection modes are requested
    emit showToggleSelectionMode(environmentHandle_, true, allSupportedTypes_);
    emit showSphereSelectionMode(environmentHandle_, true, allSupportedTypes_);

    emit showLassoSelectionMode(environmentHandle_, true, allSupportedTypes_);
    emit showVolumeLassoSelectionMode(environmentHandle_, true, allSupportedTypes_);

    emit showFloodFillSelectionMode(environmentHandle_, true, allSupportedTypes_);
    emit showComponentsSelectionMode(environmentHandle_, true, allSupportedTypes_);
    emit showClosestBoundarySelectionMode(environmentHandle_, true, allSupportedTypes_);
    
    // Define general operations
    QStringList generalOperations;
    generalOperations.append(G_CLEAR_HANDLE);
    generalOperations.append(G_CLEAR_MODEL);
    generalOperations.append(G_CONVERT);
    
    // Define vertex operations
    QStringList vertexOperations;
    vertexOperations.append(V_SELECT_ALL);
    vertexOperations.append(V_CLEAR);
    vertexOperations.append(V_INVERT);
    vertexOperations.append(V_BOUNDARY);
    vertexOperations.append(V_SHRINK);
    vertexOperations.append(V_GROW);
    vertexOperations.append(V_DELETE);
    vertexOperations.append(V_COLORIZE);
    vertexOperations.append(V_COPYSELECTION);
    vertexOperations.append(V_HANDLE);
    vertexOperations.append(V_MODELING);
    vertexOperations.append(V_LOAD_FLIPPER);
    
    // Define edge operations
    QStringList edgeOperations;
    edgeOperations.append(E_SELECT_ALL);
    edgeOperations.append(E_CLEAR);
    edgeOperations.append(E_INVERT);
    edgeOperations.append(E_DELETE);
    edgeOperations.append(E_BOUNDARY);
    edgeOperations.append(E_COLORIZE);
    edgeOperations.append(E_COPYSELECTION);
    
    // Define halfedge operations
    QStringList hedgeOperations;
    hedgeOperations.append(HE_SELECT_ALL);
    hedgeOperations.append(HE_CLEAR);
    hedgeOperations.append(HE_INVERT);
    hedgeOperations.append(HE_BOUNDARY);
    hedgeOperations.append(HE_COLORIZE);
    
    // Define face operations
    QStringList faceOperations;
    faceOperations.append(F_SELECT_ALL);
    faceOperations.append(F_CLEAR);
    faceOperations.append(F_INVERT);
    faceOperations.append(F_DELETE);
    faceOperations.append(F_BOUNDARY);
    faceOperations.append(F_SHRINK);
    faceOperations.append(F_GROW);
    faceOperations.append(F_COLORIZE);
    faceOperations.append(F_COPYSELECTION);
    
    emit addSelectionOperations(environmentHandle_, generalOperations, "Selection Operations");
    emit addSelectionOperations(environmentHandle_, vertexOperations,  "Vertex Operations",   vertexType_);
    emit addSelectionOperations(environmentHandle_, edgeOperations,    "Edge Operations",     edgeType_);
    emit addSelectionOperations(environmentHandle_, hedgeOperations,   "Halfedge Operations", halfedgeType_);
    emit addSelectionOperations(environmentHandle_, faceOperations,    "Face Operations",     faceType_);
    
    // Register key shortcuts:
    
    // Select (a)ll
    emit registerKeyShortcut(Qt::Key_A, Qt::ControlModifier);
    // (C)lear selection
    emit registerKeyShortcut(Qt::Key_C,      Qt::NoModifier);
    // (I)nvert selection
    emit registerKeyShortcut(Qt::Key_I,      Qt::NoModifier);
    // Delete selected entities
    emit registerKeyShortcut(Qt::Key_Delete, Qt::NoModifier);
}

void MeshObjectSelectionPlugin::updateSlotDescriptions() {
    
    emit setSlotDescription("selectVertices(int,IdList)", tr("Select the specified vertices"),
                            QString("objectId,vertexList").split(","), QString("Id of object,List of vertices").split(","));
    emit setSlotDescription("unselectVertices(int,IdList)", tr("Unselect the specified vertices"),
                            QString("objectId,vertexList").split(","), QString("Id of object,List of vertices").split(","));
    emit setSlotDescription("selectAllVertices(int)", tr("Select all vertices of an object"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("clearVertexSelection(int)", tr("Clear vertex selection of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("invertVertexSelection(int)", tr("Invert vertex selection of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("selectBoundaryVertices(int)", tr("Select all boundary vertices of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("selectClosestBoundaryVertices(int,int)", tr("Select boundary vertices closest to a specific vertex"),
                            QString("objectId,vertexId").split(","), QString("Id of an object,Id of closest vertex").split(","));
    emit setSlotDescription("shrinkVertexSelection(int)", tr("Shrink vertex selection by outer selection ring"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("growVertexSelection(int)", tr("Grow vertex selection by an-ring of selection"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("deleteVertexSelection(int)", tr("Delete selected vertices"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("createMeshFromVertexSelection(int)", tr("Take vertex selection and create a new mesh from it"),
                            QString("objectId").split(","), QString("Id of an object where the selection should be used to create a new mesh").split(","));

    emit setSlotDescription("selectVerticesByValue(int,QString,bool,double)", tr("Select vertices based on the value of their component"),
                                QString("objectId,component,greater,value").split(","),
                                QString("Id of an object where the selection should be used to create a new mesh,component specification: \"x\" or \"y\" or \"z\" ,true: select vertex if component greater than value; false: select if component less than value ,value to test").split(","));

    emit setSlotDescription("colorizeVertexSelection(int,int,int,int)", tr("Colorize the selected vertices"),
                            QString("objectId,r,g,b").split(","), QString("Id of an object,Red,Green,Blue").split(","));
    emit setSlotDescription("selectHandleVertices(int,IdList)", tr("Add specified vertices to handle area"),
                            QString("objectId,vertexList").split(","), QString("Id of an object,List of vertices").split(","));
    emit setSlotDescription("unselectHandleVertices(int,IdList)", tr("Remove specified vertices from handle area"),
                            QString("objectId,vertexList").split(","), QString("Id of an object,List of vertices").split(","));
    emit setSlotDescription("clearHandleVertices(int)", tr("Clear handle area"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("setAllHandleVertices(int)", tr("Add all vertices of an object to handle area"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("selectModelingVertices(int,IdList)", tr("Add specified vertices to modeling area"),
                            QString("objectId,vertexList").split(","), QString("Id of an object,List of vertices").split(","));
    emit setSlotDescription("unselectModelingVertices(int,IdList)", tr("Remove specified vertices to modeling area"),
                            QString("objectId,vertexList").split(","), QString("Id of an object,List of vertices").split(","));
    emit setSlotDescription("clearModelingVertices(int)", tr("Clear modeling area"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("setAllModelingVertices(int)", tr("Add al vertices of an object to modeling area"),
                            QStringList("objectId"), QStringList("Id of an object"));
    
    emit setSlotDescription("loadSelection(int,QString)", tr("Load selection from selection file"),
                            QString("objectId,filename").split(","), QString("Id of an object,Selection file").split(","));

    emit setSlotDescription("loadFlipperModelingSelection(int,QString)", tr("Load selection from Flipper selection file"),
                            QString("objectId,filename").split(","), QString("Id of an object,Flipper selection file").split(","));
    emit setSlotDescription("saveFlipperModelingSelection(int,QString)", tr("Save selection into Flipper selection file"),
                            QString("objectId,filename").split(","), QString("Id of an object,Flipper selection file").split(","));
    
    emit setSlotDescription("selectEdges(int,IdList)", tr("Select the specified edges"),
                            QString("objectId,edgeList").split(","), QString("Id of an object,List of edges").split(","));
    emit setSlotDescription("unselectEdges(int,IdList)", tr("Unselect the specified edges"),
                            QString("objectId,edgeList").split(","), QString("Id of an object,List of edges").split(","));
    emit setSlotDescription("selectAllEdges(int)", tr("Select all edges of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("invertEdgeSelection(int)", tr("Invert edge selection of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("clearEdgeSelection(int)", tr("Clear edge selection of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("selectBoundaryEdges(int)", tr("Select all boundary edges of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    
    emit setSlotDescription("colorizeEdgeSelection(int,int,int,int)", tr("Colorize the selected edges"),
                            QString("objectId,r,g,b").split(","), QString("Id of an object,Red,Green,Blue").split(","));
    emit setSlotDescription("createMeshFromEdgeSelection(int)", tr("Take edge selection and create a new mesh from it"),
                            QString("objectId").split(","), QString("Id of an object where the selection should be used to create a new mesh").split(","));
    
    emit setSlotDescription("selectHalfedges(int,IdList)", tr("Select the specified halfedges"),
                            QString("objectId,halfedgeList").split(","), QString("Id of an object,List of halfedges").split(","));
    emit setSlotDescription("unselectHalfedges(int,IdList)", tr("Unselect the specified halfedges"),
                            QString("objectId,halfedgeList").split(","), QString("Id of an object,List of halfedges").split(","));
    emit setSlotDescription("selectAllHalfedges(int)", tr("Select all halfedges of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("invertHalfedgeSelection(int)", tr("Invert halfedge selection of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("clearHalfedgeSelection(int)", tr("Clear halfedge selection of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("selectBoundaryHalfedges(int)", tr("Select all boundary halfedges of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    
    emit setSlotDescription("colorizeHalfedgeSelection(int,int,int,int)", tr("Colorize the selected halfedges"),
                            QString("objectId,r,g,b").split(","), QString("Id of an object,Red,Green,Blue").split(","));
                            
    emit setSlotDescription("selectFaces(int,IdList)", tr("Select the specified faces"),
                            QString("objectId,faceList").split(","), QString("Id of an object,List of faces").split(","));
    emit setSlotDescription("unselectFaces(int,IdList)", tr("Unselect the specified faces"),
                            QString("objectId,faceList").split(","), QString("Id of an object,List of faces").split(","));
    emit setSlotDescription("selectAllFaces(int)", tr("Select all vertices of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("clearFaceSelection(int)", tr("Clear face selection of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("invertFaceSelection(int)", tr("Invert face selection of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("selectBoundaryFaces(int)", tr("Select all boundary faces of an object"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("shrinkFaceSelection(int)", tr("Shrink face selection by outer face ring of selection"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("growFaceSelection(int)", tr("Grow face selection by an-ring of faces around selection"),
                            QStringList("objectId"), QStringList("Id of an object"));
    emit setSlotDescription("colorizeFaceSelection(int,int,int,int)", tr("Colorize the selected faces"),
                            QString("objectId,r,g,b").split(","), QString("Id of an object,Red,Green,Blue").split(","));

    emit setSlotDescription("createMeshFromFaceSelection(int)", tr("Take face selection and create a new mesh from it"),
                            QString("objectId").split(","), QString("Id of an object where the selection should be used to create a new mesh").split(","));

}

void MeshObjectSelectionPlugin::slotSelectionOperation(QString _operation) {
    
    SelectionInterface::PrimitiveType type = 0u;
    emit getActivePrimitiveType(type);
    
    if((type & allSupportedTypes_) == 0)
        return;
    
    // Test if operation should be applied to target objects only
    bool targetsOnly = false;
    emit targetObjectsOnly(targetsOnly);
    PluginFunctions::IteratorRestriction restriction =
            (targetsOnly ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS);
    
    if(_operation == G_CLEAR_HANDLE) {
        // Clear handle region
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                clearHandleVertices(o_it->id());
        }
    } else if(_operation == G_CLEAR_MODEL) {
        // Clear modeling region
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                clearModelingVertices(o_it->id());
        }
    } else if(_operation == G_CONVERT) {
        // Convert vertex selection
        if(!OpenFlipper::Options::nogui())
            conversionDialog_->show();
    } else if(_operation == V_SELECT_ALL) {
        // Select all vertices
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                selectAllVertices(o_it->id());
        }
    } else if(_operation == V_CLEAR) {
        // Clear vertex selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                clearVertexSelection(o_it->id());
        }
    } else if(_operation == V_INVERT) {
        // Invert vertex selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                invertVertexSelection(o_it->id());
        }
    } else if(_operation == V_BOUNDARY) {
        // Select boundary vertices
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                selectBoundaryVertices(o_it->id());
        }
    } else if(_operation == V_SHRINK) {
        // Shrink vertex selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                shrinkVertexSelection(o_it->id());
        }
    } else if(_operation == V_GROW) {
        // Grow vertex selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                growVertexSelection(o_it->id());
        }
    } else if(_operation == V_DELETE) {
        // Delete vertex selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()){
              deleteVertexSelection(o_it->id());
              emit updatedObject(o_it->id(), UPDATE_GEOMETRY);
              emit createBackup(o_it->id(), "Delete Vertices", UPDATE_GEOMETRY);
            }
        }
    } else if(_operation == V_COLORIZE) {
        // Colorize vertex selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                setColorForSelection(o_it->id(), vertexType_);
            }
        }
    } else if(_operation == V_COPYSELECTION) {
        // Copy vertex selection
        std::vector<int> objects;
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
          if (o_it->visible()) {
            objects.push_back(o_it->id());
          }
        }

        for ( unsigned int i = 0 ; i < objects.size() ; ++i)
          createMeshFromSelection(objects[i],vertexType_);

    } else if(_operation == V_HANDLE) {
        // Set vertex selection to be handle region
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                std::vector<int> ids = getVertexSelection(o_it->id());
                selectHandleVertices(o_it->id(), ids);
                clearVertexSelection(o_it->id());
            }
        }
    } else if(_operation == V_MODELING) {
        // Set vertex selection to be modeling
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                std::vector<int> ids = getVertexSelection(o_it->id());
                selectModelingVertices(o_it->id(), ids);
                clearVertexSelection(o_it->id());
            }
        }
    } else if(_operation == V_LOAD_FLIPPER) {
        // Load Flipper selection file
        QString fileName = QFileDialog::getOpenFileName(0,
                tr("Open Flipper Selection File"), OpenFlipper::Options::dataDirStr(),
                tr("Flipper Selection Files (*.sel)"));
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                loadFlipperModelingSelection(o_it->id(), fileName);
            }
        }
    } else if(_operation == E_SELECT_ALL) {
        // Select all edges
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                selectAllEdges(o_it->id());
        }
    } else if(_operation == E_CLEAR) {
        // Clear edge selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                clearEdgeSelection(o_it->id());
        }
    } else if(_operation == E_INVERT) {
        // Invert edge selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                invertEdgeSelection(o_it->id());
        }
    } else if(_operation == E_DELETE) {
        // Delete edge selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                deleteEdgeSelection(o_it->id());
        }
    } else if(_operation == E_BOUNDARY) {
        // Select boundary edges
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                selectBoundaryEdges(o_it->id());
        }
    } else if(_operation == E_COLORIZE) {
        // Colorize edge selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                setColorForSelection(o_it->id(), edgeType_);
            }
        }
    } else if(_operation == E_COPYSELECTION) {
      // Copy edge selection
      std::vector<int> objects;
      for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL));
          o_it != PluginFunctions::objectsEnd(); ++o_it) {
        if (o_it->visible()) {
          objects.push_back(o_it->id());
        }
      }

      for ( unsigned int i = 0 ; i < objects.size() ; ++i)
        createMeshFromSelection(objects[i],edgeType_);

    } else if(_operation == HE_SELECT_ALL) {
        // Select all edges
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                selectAllHalfedges(o_it->id());
        }
    } else if(_operation == HE_CLEAR) {
        // Clear edge selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                clearHalfedgeSelection(o_it->id());
        }
    } else if(_operation == HE_INVERT) {
        // Invert edge selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                invertHalfedgeSelection(o_it->id());
        }
    } else if(_operation == HE_BOUNDARY) {
        // Select boundary edges
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                selectBoundaryHalfedges(o_it->id());
        }
    } else if(_operation == HE_COLORIZE) {
        // Colorize halfedge selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                setColorForSelection(o_it->id(), halfedgeType_);
            }
        }
    } else if(_operation == F_SELECT_ALL) {
        // Select all faces
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                selectAllFaces(o_it->id());
        }
    } else if(_operation == F_CLEAR) {
        // Clear face selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                clearFaceSelection(o_it->id());
        }
    } else if(_operation == F_INVERT) {
        // Invert face selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                invertFaceSelection(o_it->id());
        }
    } else if(_operation == F_DELETE) {
        // Delete face selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                deleteFaceSelection(o_it->id());
        }
    } else if(_operation == F_BOUNDARY) {
        // Select boundary faces
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                selectBoundaryFaces(o_it->id());
        }
    } else if(_operation == F_SHRINK) {
        // Shrink face selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                shrinkFaceSelection(o_it->id());
        }
    } else if(_operation == F_GROW) {
        // Grow face selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                growFaceSelection(o_it->id());
        }
    } else if(_operation == F_COLORIZE) {
        // Colorize face selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                setColorForSelection(o_it->id(), faceType_);
            }
        }
    } else if(_operation == F_COPYSELECTION) {
      // Copy face selection
      std::vector<int> objects;
      for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_ALL));
          o_it != PluginFunctions::objectsEnd(); ++o_it) {
        if (o_it->visible()) {
          objects.push_back(o_it->id());
        }
      }

      for ( unsigned int i = 0 ; i < objects.size() ; ++i)
        createMeshFromSelection(objects[i],faceType_);
    }
}

void MeshObjectSelectionPlugin::setColorForSelection(const int _objectId, const PrimitiveType _primitiveTypes) {
    
    QColor c = QColorDialog::getColor(Qt::red, 0, tr("Choose color"),QColorDialog::ShowAlphaChannel);
    
    if(c.isValid()) {
        if(_primitiveTypes & vertexType_) {
            // Vertex colorization
            colorizeVertexSelection(_objectId, c.red(), c.green(), c.blue(), c.alpha());
        } else if (_primitiveTypes & edgeType_) {
            // Edge colorization
            colorizeEdgeSelection(_objectId, c.red(), c.green(), c.blue(), c.alpha());
        } else if (_primitiveTypes & halfedgeType_) {
            // Edge colorization
            colorizeHalfedgeSelection(_objectId, c.red(), c.green(), c.blue(), c.alpha());
        } else if (_primitiveTypes & faceType_) {
            // Edge colorization
            colorizeFaceSelection(_objectId, c.red(), c.green(), c.blue(), c.alpha());
        }
    }
}

void MeshObjectSelectionPlugin::conversionRequested() {
    
    conversionDialog_->hide();
    
    QString from = conversionDialog_->convertFromBox->currentText();
    QString to = conversionDialog_->convertToBox->currentText();
    
    if(from == to) return;
    
    bool deselect = true;
    if(!OpenFlipper::Options::nogui()) {
        deselect = conversionDialog_->deselect->isChecked();
    }
    
    conversion(from, to, deselect);
}

void MeshObjectSelectionPlugin::conversion(const QString& _from, const QString& _to, bool _deselect) {

  for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH));
         o_it != PluginFunctions::objectsEnd(); ++o_it) {
    
        if(_from == "Vertex Selection") {
            if (_to == "Edge Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertVertexToEdgeSelection(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertVertexToEdgeSelection(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Halfedge Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertVertexToHalfedgeSelection(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertVertexToHalfedgeSelection(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Face Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertVertexToFaceSelection(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertVertexToFaceSelection(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Feature Vertices") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertVertexSelectionToFeatureVertices(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertVertexSelectionToFeatureVertices(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Handle Region") {
                selectHandleVertices(o_it->id(), getVertexSelection(o_it->id()));
            } else if (_to == "Modeling Region") {
                selectModelingVertices(o_it->id(), getVertexSelection(o_it->id()));
            }
            
            if(_deselect) {
                clearVertexSelection(o_it->id());
            }
            
        } else if (_from == "Edge Selection") {
            if(_to == "Vertex Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertEdgeToVertexSelection(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertEdgeToVertexSelection(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Halfedge Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertEdgeToHalfedgeSelection(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertEdgeToHalfedgeSelection(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Face Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertEdgeToFaceSelection(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertEdgeToFaceSelection(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Feature Edges") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertEdgeSelectionToFeatureEdges(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertEdgeSelectionToFeatureEdges(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Handle Region") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH) {
                    TriMesh* mesh = PluginFunctions::triMesh(o_it);
                    std::vector<int> ids;
                    for(TriMesh::EdgeIter e_it = mesh->edges_begin(); e_it != mesh->edges_end(); ++e_it) {
                        if(mesh->status(e_it).selected()) {
                            ids.push_back(mesh->to_vertex_handle(mesh->halfedge_handle(e_it, 0)).idx());
                            ids.push_back(mesh->to_vertex_handle(mesh->halfedge_handle(e_it, 1)).idx());
                        }
                    }
                    selectHandleVertices(o_it->id(), ids);
                } else if(o_it->dataType() == DATA_POLY_MESH) {
                    PolyMesh* mesh = PluginFunctions::polyMesh(o_it);
                    std::vector<int> ids;
                    for(PolyMesh::EdgeIter e_it = mesh->edges_begin(); e_it != mesh->edges_end(); ++e_it) {
                        if(mesh->status(e_it).selected()) {
                            ids.push_back(mesh->to_vertex_handle(mesh->halfedge_handle(e_it, 0)).idx());
                            ids.push_back(mesh->to_vertex_handle(mesh->halfedge_handle(e_it, 1)).idx());
                        }
                    }
                    selectHandleVertices(o_it->id(), ids);
                }
            } else if (_to == "Modeling Region") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH) {
                    TriMesh* mesh = PluginFunctions::triMesh(o_it);
                    std::vector<int> ids;
                    for(TriMesh::EdgeIter e_it = mesh->edges_begin(); e_it != mesh->edges_end(); ++e_it) {
                        if(mesh->status(e_it).selected()) {
                            ids.push_back(mesh->to_vertex_handle(mesh->halfedge_handle(e_it, 0)).idx());
                            ids.push_back(mesh->to_vertex_handle(mesh->halfedge_handle(e_it, 1)).idx());
                        }
                    }
                    selectModelingVertices(o_it->id(), ids);
                } else if(o_it->dataType() == DATA_POLY_MESH) {
                    PolyMesh* mesh = PluginFunctions::polyMesh(o_it);
                    std::vector<int> ids;
                    for(PolyMesh::EdgeIter e_it = mesh->edges_begin(); e_it != mesh->edges_end(); ++e_it) {
                        if(mesh->status(e_it).selected()) {
                            ids.push_back(mesh->to_vertex_handle(mesh->halfedge_handle(e_it, 0)).idx());
                            ids.push_back(mesh->to_vertex_handle(mesh->halfedge_handle(e_it, 1)).idx());
                        }
                    }
                    selectModelingVertices(o_it->id(), ids);
                }
            }
            
            if(_deselect) {
                clearEdgeSelection(o_it->id());
            }
        } else if (_from == "Halfedge Selection") {
            if(_to == "Vertex Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertHalfedgeToVertexSelection(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertHalfedgeToVertexSelection(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Edge Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertHalfedgeToEdgeSelection(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertHalfedgeToEdgeSelection(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Face Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertHalfedgeToFaceSelection(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertHalfedgeToFaceSelection(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Handle Region") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH) {
                    TriMesh* mesh = PluginFunctions::triMesh(o_it);
                    std::vector<int> ids;
                    for(TriMesh::HalfedgeIter h_it = mesh->halfedges_begin(); h_it != mesh->halfedges_end(); ++h_it) {
                        if(mesh->status(h_it).selected()) {
                            ids.push_back(mesh->to_vertex_handle(h_it).idx());
                            ids.push_back(mesh->from_vertex_handle(h_it).idx());
                        }
                    }
                    selectHandleVertices(o_it->id(), ids);
                } else if(o_it->dataType() == DATA_POLY_MESH) {
                    PolyMesh* mesh = PluginFunctions::polyMesh(o_it);
                    std::vector<int> ids;
                    for(PolyMesh::HalfedgeIter h_it = mesh->halfedges_begin(); h_it != mesh->halfedges_end(); ++h_it) {
                        if(mesh->status(h_it).selected()) {
                            ids.push_back(mesh->to_vertex_handle(h_it).idx());
                            ids.push_back(mesh->from_vertex_handle(h_it).idx());
                        }
                    }
                    selectHandleVertices(o_it->id(), ids);
                }
            } else if (_to == "Modeling Region") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH) {
                    TriMesh* mesh = PluginFunctions::triMesh(o_it);
                    std::vector<int> ids;
                    for(TriMesh::HalfedgeIter h_it = mesh->halfedges_begin(); h_it != mesh->halfedges_end(); ++h_it) {
                        if(mesh->status(h_it).selected()) {
                            ids.push_back(mesh->to_vertex_handle(h_it).idx());
                            ids.push_back(mesh->from_vertex_handle(h_it).idx());
                        }
                    }
                    selectModelingVertices(o_it->id(), ids);
                } else if(o_it->dataType() == DATA_POLY_MESH) {
                    PolyMesh* mesh = PluginFunctions::polyMesh(o_it);
                    std::vector<int> ids;
                    for(PolyMesh::HalfedgeIter h_it = mesh->halfedges_begin(); h_it != mesh->halfedges_end(); ++h_it) {
                        if(mesh->status(h_it).selected()) {
                            ids.push_back(mesh->to_vertex_handle(h_it).idx());
                            ids.push_back(mesh->from_vertex_handle(h_it).idx());
                        }
                    }
                    selectModelingVertices(o_it->id(), ids);
                }
            }
            
            if(_deselect) {
                clearHalfedgeSelection(o_it->id());
            }
        } else if (_from == "Face Selection") {
            if(_to == "Vertex Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertFaceToVertexSelection(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertFaceToVertexSelection(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Edge Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertFaceToEdgeSelection(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertFaceToEdgeSelection(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Feature Faces") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertFaceSelectionToFeatureFaces(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertFaceSelectionToFeatureFaces(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Halfedge Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertFaceToHalfedgeSelection(PluginFunctions::triMesh(o_it));
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertFaceToHalfedgeSelection(PluginFunctions::polyMesh(o_it));
            } else if (_to == "Handle Region") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH) {
                    TriMesh* mesh = PluginFunctions::triMesh(o_it);
                    std::vector<int> ids;
                    for(TriMesh::FaceIter f_it = mesh->faces_begin(); f_it != mesh->faces_end(); ++f_it) {
                        if(mesh->status(f_it).selected()) {
                            for(TriMesh::FaceVertexIter fv_it = mesh->fv_iter(f_it); fv_it; ++fv_it) {
                                ids.push_back(fv_it.handle().idx());
                            }
                        }
                    }
                    selectHandleVertices(o_it->id(), ids);
                } else if(o_it->dataType() == DATA_POLY_MESH) {
                    PolyMesh* mesh = PluginFunctions::polyMesh(o_it);
                    std::vector<int> ids;
                    for(PolyMesh::FaceIter f_it = mesh->faces_begin(); f_it != mesh->faces_end(); ++f_it) {
                        if(mesh->status(f_it).selected()) {
                            for(PolyMesh::FaceVertexIter fv_it = mesh->fv_iter(f_it); fv_it; ++fv_it) {
                                ids.push_back(fv_it.handle().idx());
                            }
                        }
                    }
                    selectHandleVertices(o_it->id(), ids);
                }
            } else if (_to == "Modeling Region") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH) {
                    TriMesh* mesh = PluginFunctions::triMesh(o_it);
                    std::vector<int> ids;
                    for(TriMesh::FaceIter f_it = mesh->faces_begin(); f_it != mesh->faces_end(); ++f_it) {
                        if(mesh->status(f_it).selected()) {
                            for(TriMesh::FaceVertexIter fv_it = mesh->fv_iter(f_it); fv_it; ++fv_it) {
                                ids.push_back(fv_it.handle().idx());
                            }
                        }
                    }
                    selectModelingVertices(o_it->id(), ids);
                } else if(o_it->dataType() == DATA_POLY_MESH) {
                    PolyMesh* mesh = PluginFunctions::polyMesh(o_it);
                    std::vector<int> ids;
                    for(PolyMesh::FaceIter f_it = mesh->faces_begin(); f_it != mesh->faces_end(); ++f_it) {
                        if(mesh->status(f_it).selected()) {
                            for(PolyMesh::FaceVertexIter fv_it = mesh->fv_iter(f_it); fv_it; ++fv_it) {
                                ids.push_back(fv_it.handle().idx());
                            }
                        }
                    }
                    selectModelingVertices(o_it->id(), ids);
                }
            }
            
            if(_deselect) {
                clearFaceSelection(o_it->id());
            }
        } else if (_from == "Feature Vertices") {

            if (_to == "Vertex Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH) {
                    MeshSelection::convertFeatureVerticesToVertexSelection(PluginFunctions::triMesh(o_it));
                    if (_deselect) {
                        MeshSelection::clearFeatureVertices(PluginFunctions::triMesh(o_it));
                    }
                } else if(o_it->dataType() == DATA_POLY_MESH) {
                    MeshSelection::convertFeatureVerticesToVertexSelection(PluginFunctions::polyMesh(o_it));
                    if (_deselect) {
                        MeshSelection::clearFeatureVertices(PluginFunctions::polyMesh(o_it));
                    }
                }
            }
        } else if (_from == "Feature Edges") {

            if (_to == "Edge Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH) {
                    MeshSelection::convertFeatureEdgesToEdgeSelection(PluginFunctions::triMesh(o_it));
                    if (_deselect) {
                        MeshSelection::clearFeatureEdges(PluginFunctions::triMesh(o_it));
                    }
                } else if(o_it->dataType() == DATA_POLY_MESH) {
                    MeshSelection::convertFeatureEdgesToEdgeSelection(PluginFunctions::polyMesh(o_it));
                    if (_deselect) {
                        MeshSelection::clearFeatureEdges(PluginFunctions::polyMesh(o_it));
                    }
                }
            }
        } else if (_from == "Feature Faces") {

            if (_to == "Face Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH) {
                    MeshSelection::convertFeatureFacesToFaceSelection(PluginFunctions::triMesh(o_it));
                    if (_deselect) {
                        MeshSelection::clearFeatureFaces(PluginFunctions::triMesh(o_it));
                    }
                } else if(o_it->dataType() == DATA_POLY_MESH) {
                    MeshSelection::convertFeatureFacesToFaceSelection(PluginFunctions::polyMesh(o_it));
                    if (_deselect) {
                        MeshSelection::clearFeatureFaces(PluginFunctions::polyMesh(o_it));
                    }
                }
            }
        } else if (_from == "Handle Region") {
            std::vector<int> ids = getHandleVertices(o_it->id());
            if(_to == "Vertex Selection") {
                selectVertices(o_it->id(), ids);
            } else if (_to == "Edge Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertVertexToEdgeSelection(PluginFunctions::triMesh(o_it), ids);
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertVertexToEdgeSelection(PluginFunctions::polyMesh(o_it), ids);
            } else if (_to == "Halfedge Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertVertexToHalfedgeSelection(PluginFunctions::triMesh(o_it), ids);
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertVertexToHalfedgeSelection(PluginFunctions::polyMesh(o_it), ids);
            } else if (_to == "Face Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertVertexToFaceSelection(PluginFunctions::triMesh(o_it), ids);
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertVertexToFaceSelection(PluginFunctions::polyMesh(o_it), ids);
            } else if (_to == "Modeling Region") {
                selectModelingVertices(o_it->id(), ids);
            }
            
            if(_deselect) {
                clearHandleVertices(o_it->id());
            }
            
        } else if (_from == "Modeling Region") {
            std::vector<int> ids = getModelingVertices(o_it->id());
            if(_to == "Vertex Selection") {
                selectVertices(o_it->id(), ids);
            } else if (_to == "Edge Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertVertexToEdgeSelection(PluginFunctions::triMesh(o_it), ids);
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertVertexToEdgeSelection(PluginFunctions::polyMesh(o_it), ids);
            } else if (_to == "Halfedge Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertVertexToHalfedgeSelection(PluginFunctions::triMesh(o_it), ids);
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertVertexToHalfedgeSelection(PluginFunctions::polyMesh(o_it), ids);
            } else if (_to == "Face Selection") {
                if(o_it->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertVertexToFaceSelection(PluginFunctions::triMesh(o_it), ids);
                else if(o_it->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertVertexToFaceSelection(PluginFunctions::polyMesh(o_it), ids);
            } else if (_to == "Handle Region") {
                selectHandleVertices(o_it->id(), ids);
            }
            
            if(_deselect) {
                clearModelingVertices(o_it->id());
            }
        }
        
        emit updatedObject(o_it->id(), UPDATE_SELECTION);
        emit createBackup(o_it->id(), "Selection Conversion", UPDATE_SELECTION);
    }
}

void MeshObjectSelectionPlugin::slotToggleSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {
    
    // Return if none of the currently active types is handled by this plugin
    if((_currentType & allSupportedTypes_) == 0) return;
    
    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;

    // First of all, pick anything to find all possible objects
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,
        _event->pos(), node_idx, target_idx, &hit_point)) {

        BaseObjectData* object(0);
        PluginFunctions::getPickedObject(node_idx, object);
        if(!object) return;
    
        if (object->dataType() == DATA_TRIANGLE_MESH) {
            // Pick triangle meshes
            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {

                if (object->dataType(DATA_TRIANGLE_MESH)) {
                    toggleMeshSelection(object->id(), PluginFunctions::triMesh(object), target_idx, hit_point, _currentType);
                }
            }
        } else if (object->dataType() == DATA_POLY_MESH) {
            // Pick poly meshes
            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {

                if (object->dataType(DATA_POLY_MESH)) {
                    toggleMeshSelection(object->id(), PluginFunctions::polyMesh(object), target_idx, hit_point, _currentType);
                }
            }
        }
        emit updatedObject(object->id(), UPDATE_SELECTION);
        emit createBackup(object->id(), "Toggle Selection", UPDATE_SELECTION);
    }
}

void MeshObjectSelectionPlugin::slotLassoSelection(QMouseEvent* _event, PrimitiveType _currentType, bool _deselect) {
    
    // Return if none of the currently active types is handled by this plugin
    if((_currentType & allSupportedTypes_) == 0) return;
   
    if(_event->type() == QEvent::MouseButtonPress) {
 
        // Add picked point
        lasso_2Dpoints_.push_back(_event->pos());
        
        return;

    } else if(_event->type() == QEvent::MouseButtonDblClick) {
        
        // Finish surface lasso selection
        if (lasso_2Dpoints_.size() > 2) {

            QRegion region(lasso_2Dpoints_);
            
            lassoSelect(region, _currentType, _deselect);
        }
        
        // Clear points
        lasso_2Dpoints_.clear();
    }
}

void MeshObjectSelectionPlugin::slotVolumeLassoSelection(QMouseEvent* _event, PrimitiveType _currentType, bool _deselect) {
    
    // Return if none of the currently active types is handled by this plugin
    if((_currentType & allSupportedTypes_) == 0) return;
    
    if(_event->type() == QEvent::MouseButtonPress) {

        // Add point on viewing plane to selection polygon
        volumeLassoPoints_.append(_event->pos());
        
        return;

    } else if(_event->type() == QEvent::MouseButtonDblClick) {

        ACG::GLState &state = PluginFunctions::viewerProperties().glState();
        bool updateGL = state.updateGL();
        state.set_updateGL (false);

        QPolygon p(volumeLassoPoints_);
        QRegion region = QRegion(p);

        SelectVolumeAction action(region, this, _currentType, _deselect, state);
        ACG::SceneGraph::traverse (PluginFunctions::getRootNode(), action);

        state.set_updateGL(updateGL);

        // Clear lasso points
        volumeLassoPoints_.clear();
    }
}

void MeshObjectSelectionPlugin::slotSphereSelection(QMouseEvent* _event, double _radius, PrimitiveType _currentType, bool _deselect) {
    
    // Return if none of the currently active types is handled by this plugin
    if((_currentType & allSupportedTypes_) == 0) return;
    
    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;
    
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), node_idx, target_idx, &hit_point)) {
        
        BaseObjectData* object = 0;

        if (PluginFunctions::getPickedObject(node_idx, object)) {

            if (object->picked(node_idx) && object->dataType(DATA_TRIANGLE_MESH)) {
                paintSphereSelection(PluginFunctions::triMesh(object), target_idx, hit_point, _radius, _currentType, _deselect);
            } else if (object->picked(node_idx) && object->dataType(DATA_POLY_MESH)) {
                paintSphereSelection(PluginFunctions::polyMesh(object), target_idx, hit_point, _radius, _currentType, _deselect);
            }

            emit updatedObject(object->id(), UPDATE_SELECTION);
            if ( _event->type() == QEvent::MouseButtonRelease )
              emit  createBackup(object->id(), "Sphere Selection", UPDATE_SELECTION);
        }
    }
}

void MeshObjectSelectionPlugin::slotClosestBoundarySelection(QMouseEvent* _event, PrimitiveType _currentType, bool _deselect) {
    
    // Return if none of the currently active types is handled by this plugin
    if((_currentType & allSupportedTypes_) == 0) return;

    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;

    if(PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos() ,node_idx, target_idx, &hit_point)) {
       
        BaseObjectData* object = 0;

        if(PluginFunctions::getPickedObject(node_idx, object)) {

            if(object->dataType(DATA_TRIANGLE_MESH)) {

                TriMesh* m = PluginFunctions::triMesh(object);
                TriMesh::VertexHandle vh = m->fv_iter(m->face_handle(target_idx)).handle();

                closestBoundarySelection(m, vh.idx(), _currentType, _deselect);

                emit updatedObject(object->id(), UPDATE_SELECTION);
                emit  createBackup(object->id(), "Boundary Selection", UPDATE_SELECTION);

            } else if(object->dataType(DATA_POLY_MESH)) {

                PolyMesh* m = PluginFunctions::polyMesh(object);
                PolyMesh::VertexHandle vh = m->fv_iter(m->face_handle(target_idx)).handle();

                closestBoundarySelection(m, vh.idx(), _currentType, _deselect);

                emit updatedObject(object->id(), UPDATE_SELECTION);
                emit  createBackup(object->id(), "Boundary Selection", UPDATE_SELECTION);
            }
            
            emit updateView();
        }
    }
}

void MeshObjectSelectionPlugin::slotFloodFillSelection(QMouseEvent* _event, double _maxAngle, PrimitiveType _currentType, bool _deselect) {
    
    // Return if none of the currently active types is handled by this plugin
    if((_currentType & allSupportedTypes_) == 0) return;
    
    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;

    // pick Anything to find all possible objects
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,
                                        _event->pos(), node_idx, target_idx, &hit_point)) {

        BaseObjectData* object = 0;
        
        if(PluginFunctions::getPickedObject(node_idx, object)) {

            // TRIANGLE MESHES
            if(object->dataType() == DATA_TRIANGLE_MESH) {

                if(PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), node_idx, target_idx, &hit_point)) {

                    if(PluginFunctions::getPickedObject(node_idx, object)) {

                        if(object->dataType(DATA_TRIANGLE_MESH)) {
                            floodFillSelection(PluginFunctions::triMesh(object), target_idx, _maxAngle, _currentType, _deselect);
                            emit updatedObject(object->id(), UPDATE_SELECTION);
                            emit  createBackup(object->id(), "FloodFill Selection", UPDATE_SELECTION);
                        }
                    }
                }

            // POLY MESHES
            } else if(object->dataType() == DATA_POLY_MESH) {

                if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), node_idx, target_idx, &hit_point)) {

                    if(PluginFunctions::getPickedObject(node_idx, object) ) {

                        if(object->dataType(DATA_POLY_MESH)) {
                            floodFillSelection(PluginFunctions::polyMesh(object), target_idx, _maxAngle, _currentType, _deselect);
                            emit updatedObject(object->id(), UPDATE_SELECTION);
                            emit  createBackup(object->id(), "FloodFill Selection", UPDATE_SELECTION);
                        }
                    }
                }
            }
            else {
                emit log(LOGERR,tr("floodFillSelection: Unsupported dataType"));
            }
        }
    }
}

void MeshObjectSelectionPlugin::slotComponentsSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {

    // Return if none of the currently active types is handled by this plugin
    if((_currentType & allSupportedTypes_) == 0) return;

    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;

    // First of all, pick anything to find all possible objects
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,
        _event->pos(), node_idx, target_idx, &hit_point)) {

        BaseObjectData* object(0);
        PluginFunctions::getPickedObject(node_idx, object);
        if(!object) return;

        if (object->dataType() == DATA_TRIANGLE_MESH) {
            // Pick triangle meshes
            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {

                if (object->dataType(DATA_TRIANGLE_MESH)) {
                    componentsMeshSelection(PluginFunctions::triMesh(object), target_idx, hit_point, _currentType);
                }
            }
        } else if (object->dataType() == DATA_POLY_MESH) {
            // Pick poly meshes
            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {

                if (object->dataType(DATA_POLY_MESH)) {
                    componentsMeshSelection(PluginFunctions::polyMesh(object), target_idx, hit_point, _currentType);
                }
            }
        }
        emit updatedObject(object->id(), UPDATE_SELECTION);
        emit createBackup(object->id(), "Connected Components Selection", UPDATE_SELECTION);
    }
}

void MeshObjectSelectionPlugin::loadSelection(int _objId, const QString& _filename) {

    // Load ini file
    INIFile file;

    if(!file.connect(_filename, false)) {
        emit log(LOGERR, QString("Could not read file '%1'!").arg(_filename));
        return;
    }

    // Load selection from file
    loadIniFile(file, _objId);
}

void MeshObjectSelectionPlugin::loadIniFile(INIFile& _ini, int _id) {

    BaseObjectData* object = 0;
    if (!PluginFunctions::getObject(_id,object)) {
        emit log(LOGERR,tr("Cannot find object for id ") + QString::number(_id) + tr(" in saveFile") );
        return;
    }

    std::vector<int> ids;
    bool invert = false;

    bool updated_selection = false;
    bool updated_modeling_regions = false;

    QString sectionName = object->name();

    if (_ini.get_entry(ids, sectionName ,"ModelingRegion")) {
        invert = false;
        _ini.get_entry(invert, sectionName, "InvertModeling");

        if (invert) {
            setAllModelingVertices(object->id());
            unselectModelingVertices(object->id() , ids);
        } else {
            clearModelingVertices(object->id());
            selectModelingVertices(object->id(), ids);
        }

        if(object->dataType(DATA_TRIANGLE_MESH)) {
            update_regions(PluginFunctions::triMesh(object));
        }

        if(object->dataType(DATA_POLY_MESH)) {
            update_regions(PluginFunctions::polyMesh(object));
        }

        updated_modeling_regions = true;
    }

    if(_ini.get_entry(ids, sectionName, "HandleRegion")) {
        invert = false;
        _ini.get_entry(invert, sectionName, "InvertHandle");

        if(invert) {
            setAllHandleVertices(object->id());
            unselectHandleVertices(object->id(), ids);
        } else {
            clearHandleVertices(object->id());
            selectHandleVertices(object->id(), ids);
        }

        if (object->dataType(DATA_TRIANGLE_MESH)) {
            update_regions(PluginFunctions::triMesh(object));
        }

        if(object->dataType(DATA_POLY_MESH)) {
            update_regions(PluginFunctions::polyMesh(object));
        }
        
        updated_modeling_regions = true;
    }

    if(_ini.get_entry(ids, sectionName, "VertexSelection")) {
        invert = false;
        _ini.get_entry(invert, sectionName, "InvertVertexSelection");

        if(invert) {
            selectAllVertices(object->id());
            unselectVertices(object->id(),ids);
        } else {
            clearVertexSelection(object->id());
            selectVertices(object->id(),ids);
        }

        updated_selection = true;
    }

    if(_ini.get_entry(ids, sectionName, "EdgeSelection")) {
        invert = false;
        _ini.get_entry(invert, sectionName, "InvertEdgeSelection");

        ids = convertVertexPairsToEdges(_id, ids);

        if(invert) {
            selectAllEdges(object->id());
            unselectEdges(object->id(),ids);
        } else {
            clearEdgeSelection(object->id());
            selectEdges(object->id(),ids);
        }

        updated_selection = true;
    }

    if(_ini.get_entry(ids, sectionName, "FaceSelection")) {
        invert = false;
        _ini.get_entry(invert, sectionName, "InvertFaceSelection");

        if(invert) {
            selectAllFaces(object->id());
            unselectFaces(object->id(),ids);
        } else {
            clearFaceSelection(object->id());
            selectFaces(object->id(),ids);
        }

        updated_selection = true;
    }

    if(updated_modeling_regions) {

        emit updatedObject(object->id(), UPDATE_ALL);
        emit updateView();

    } else if(updated_selection) {

        emit updatedObject(object->id(), UPDATE_SELECTION);
        emit updateView();
    }

    if ( updated_modeling_regions || updated_selection )
      emit  createBackup(object->id(), "Load Selection", UPDATE_SELECTION);
}

void MeshObjectSelectionPlugin::saveIniFile(INIFile& _ini, int _id) {
    
    BaseObjectData* object = 0;
    if ( !PluginFunctions::getObject(_id,object) ) {
        emit log(LOGERR,tr("Cannot find object for id ") + QString::number(_id) + tr(" in saveFile") );
        return;
    }

    // The objects section should already exist
    QString sectionName = object->name();
    if(!_ini.section_exists(sectionName)) {
        emit log(LOGERR,tr("Cannot find object section id ") + QString::number(_id) + tr(" in saveFile") );
        return;
    }

    _ini.add_entry(sectionName, "VertexSelection", getVertexSelection(object->id()));
    _ini.add_entry(sectionName, "EdgeSelection", convertEdgesToVertexPairs(_id, getEdgeSelection(object->id())));

    if(object->dataType(DATA_POLY_MESH ) || object->dataType( DATA_TRIANGLE_MESH)) {
        _ini.add_entry(sectionName, "FaceSelection", getFaceSelection(object->id()));
        _ini.add_entry(sectionName, "HandleRegion", getHandleVertices(object->id()));
        _ini.add_entry(sectionName, "ModelingRegion", getModelingVertices(object->id()));
    }
}

void MeshObjectSelectionPlugin::slotLoadSelection(const INIFile& _file) {
    
    // Iterate over all mesh objects in the scene and save
    // the selections for all supported entity types
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)); 
         o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        // Read section for each object
        // Append object name to section identifier
        QString section = QString("MeshObjectSelection") + "//" + o_it->name();
        if(!_file.section_exists(section)) {
            continue;
        }
        
        std::vector<int> ids;
        // Load vertex selection:
        _file.get_entry(ids, section, "VertexSelection");
        selectVertices(o_it->id(), ids);
        ids.clear();
        // Load edge selection:
        _file.get_entry(ids, section, "EdgeSelection");
        selectEdges(o_it->id(), convertVertexPairsToEdges(o_it->id(), ids));
        ids.clear();
        // Load halfedge selection:
        _file.get_entry(ids, section, "HalfedgeSelection");
        selectHalfedges(o_it->id(), convertVertexPairsToHalfedges(o_it->id(), ids));
        ids.clear();
        // Load face selection:
        _file.get_entry(ids, section, "FaceSelection");
        selectFaces(o_it->id(), ids);
        ids.clear();
        // Load handle region:
        _file.get_entry(ids, section, "HandleRegion");
        selectHandleVertices(o_it->id(), ids);
        ids.clear();
        // Load modeling region:
        _file.get_entry(ids, section, "ModelingRegion");
        selectModelingVertices(o_it->id(), ids);
        ids.clear();
        
        emit updatedObject(o_it->id(), UPDATE_SELECTION);
        emit  createBackup(o_it->id(), "Load Selection", UPDATE_SELECTION);
    }
}

void MeshObjectSelectionPlugin::slotSaveSelection(INIFile& _file) {
    
    // Iterate over all mesh objects in the scene and save
    // the selections for all supported entity types
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)); 
         o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        // Create section for each object
        // Append object name to section identifier
        QString section = QString("MeshObjectSelection") + "//" + o_it->name();

        // Store vertex selection:
        _file.add_entry(section, "VertexSelection", getVertexSelection(o_it->id()));
        // Store edge selection:
        _file.add_entry(section, "EdgeSelection", convertEdgesToVertexPairs(o_it->id(), getEdgeSelection(o_it->id())));
        // Store halfedge selection:
        _file.add_entry(section, "HalfedgeSelection", convertHalfedgesToVertexPairs(o_it->id(), getHalfedgeSelection(o_it->id())));
        // Store face selection:
        _file.add_entry(section, "FaceSelection", getFaceSelection(o_it->id()));
        // Store handle region:
        _file.add_entry(section, "HandleRegion", getHandleVertices(o_it->id()));
        // Store modeling region:
        _file.add_entry(section, "ModelingRegion", getModelingVertices(o_it->id()));
    }
}

void MeshObjectSelectionPlugin::slotKeyShortcutEvent(int _key, Qt::KeyboardModifiers _modifiers) {
    
    SelectionInterface::PrimitiveType type = 0u;
    emit getActivePrimitiveType(type);
    
    if((type & allSupportedTypes_) == 0) {
        // No supported type is active
        return;
    }
    
    bool targetsOnly = false;
    emit targetObjectsOnly(targetsOnly);
    PluginFunctions::IteratorRestriction restriction =
            (targetsOnly ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS);
    
    if(_key == Qt::Key_A && _modifiers == Qt::ControlModifier) {
        
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                
                if(type & vertexType_)
                    selectAllVertices(o_it->id());
                if(type & edgeType_)
                    selectAllEdges(o_it->id());
                if(type & halfedgeType_)
                    selectAllHalfedges(o_it->id());
                if(type & faceType_)
                    selectAllFaces(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Select All", UPDATE_SELECTION);
        }
    } else if (_key == Qt::Key_C && _modifiers == Qt::NoModifier) {
        
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                
                if(type & vertexType_)
                    clearVertexSelection(o_it->id());
                if(type & edgeType_)
                    clearEdgeSelection(o_it->id());
                if(type & halfedgeType_)
                    clearHalfedgeSelection(o_it->id());
                if(type & faceType_)
                    clearFaceSelection(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Clear Selection", UPDATE_SELECTION);
        }
    } else if(_key == Qt::Key_I && _modifiers == Qt::NoModifier) {
        
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                
                if(type & vertexType_)
                    invertVertexSelection(o_it->id());
                if(type & edgeType_)
                    invertEdgeSelection(o_it->id());
                if(type & halfedgeType_)
                    invertHalfedgeSelection(o_it->id());
                if(type & faceType_)
                    invertFaceSelection(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Invert Selection", UPDATE_SELECTION);
        }
    } else if (_key == Qt::Key_Delete && _modifiers == Qt::NoModifier) {
        
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                // Delete all selected primitives
                if(type & vertexType_)
                    deleteVertexSelection(o_it->id());
                if(type & edgeType_)
                    deleteEdgeSelection(o_it->id());
                if(type & faceType_)
                    deleteFaceSelection(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_TOPOLOGY);
            emit  createBackup(o_it->id(), "Delete Selection", UPDATE_TOPOLOGY);
        }
    }
}

void MeshObjectSelectionPlugin::slotMouseWheelEvent(QWheelEvent* event, std::string const& mode) {

  // Get currently active primitive type
  SelectionInterface::PrimitiveType type = 0u;
  emit getActivePrimitiveType(type);

  // Only handle supported primitive types
  if((type & allSupportedTypes_) == 0) {
    // No supported type is active
    return;
  }

  // Decide, if all or only target objects should be handled
  bool targetsOnly = false;
  emit targetObjectsOnly(targetsOnly);
  PluginFunctions::IteratorRestriction restriction =
      (targetsOnly ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS);

  if(event->modifiers() == Qt::ShiftModifier) {

    if (event->delta() > 0) {
      for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH));
          o_it != PluginFunctions::objectsEnd(); ++o_it) {
        if (o_it->visible()) {

          if(type & vertexType_)
            growVertexSelection(o_it->id());
          if(type & faceType_)
            growFaceSelection(o_it->id());
        }
        emit updatedObject(o_it->id(), UPDATE_SELECTION);
        emit createBackup(o_it->id(), "Grow Selection", UPDATE_SELECTION);
      }
    } else {

      for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH));
          o_it != PluginFunctions::objectsEnd(); ++o_it) {
        if (o_it->visible()) {

          if(type & vertexType_)
            shrinkVertexSelection(o_it->id());
          if(type & faceType_)
            shrinkFaceSelection(o_it->id());
        }
        emit updatedObject(o_it->id(), UPDATE_SELECTION);
        emit createBackup(o_it->id(), "Shrink Selection", UPDATE_SELECTION);
      }
    }
  }
}

void MeshObjectSelectionPlugin::lassoSelect(QRegion&      _region,
                                            PrimitiveType _primitiveType,
                                            bool          _deselection) {

    // <object id, primitive id>
    QList <QPair<unsigned int, unsigned int> > list;
    
    if(_primitiveType & vertexType_) {
        PluginFunctions::scenegraphRegionPick(ACG::SceneGraph::PICK_FRONT_VERTEX, _region, list);
        
        std::set<int> alreadySelectedObjects;
        
        for(int i = 0; i < list.size(); ++i) {
            
            if(alreadySelectedObjects.count(list[i].first) != 0)
                continue;
            
            BaseObjectData* bod = 0;
            PluginFunctions::getPickedObject(list[i].first, bod);
            
            if(bod && (bod->dataType() == DATA_TRIANGLE_MESH || bod->dataType() == DATA_POLY_MESH)) {
                IdList elements;
                for(int j = 0; j < list.size(); ++j) {
                    if(list[j].first == list[i].first) {
                        
                        elements.push_back(list[j].second);
                    }
                }
                selectVertices(bod->id(), elements);
                alreadySelectedObjects.insert(list[i].first);
                emit updatedObject(bod->id(), UPDATE_SELECTION);
                emit  createBackup(bod->id(), "Lasso Selection", UPDATE_SELECTION);
            }
        }
    }
    if(_primitiveType & edgeType_) {
        PluginFunctions::scenegraphRegionPick(ACG::SceneGraph::PICK_FRONT_EDGE, _region, list);
        
        std::set<int> alreadySelectedObjects;
        
        for(int i = 0; i < list.size(); ++i) {
            
            if(alreadySelectedObjects.count(list[i].first) != 0)
                continue;
            
            BaseObjectData* bod = 0;
            PluginFunctions::getPickedObject(list[i].first, bod);
            
            if(bod && (bod->dataType() == DATA_TRIANGLE_MESH || bod->dataType() == DATA_POLY_MESH)) {
                IdList elements;
                for(int j = 0; j < list.size(); ++j) {
                    if(list[j].first == list[i].first) {
                        
                        elements.push_back(list[j].second);
                    }
                }
                selectEdges(bod->id(), elements);
                alreadySelectedObjects.insert(list[i].first);
                emit updatedObject(bod->id(), UPDATE_SELECTION);
                emit  createBackup(bod->id(), "Lasso Selection", UPDATE_SELECTION);
            }
        }
    }
    if(_primitiveType & halfedgeType_) {
        PluginFunctions::scenegraphRegionPick(ACG::SceneGraph::PICK_FRONT_EDGE, _region, list);
        
        std::set<int> alreadySelectedObjects;
        
        for(int i = 0; i < list.size(); ++i) {
            
            if(alreadySelectedObjects.count(list[i].first) != 0)
                continue;
            
            BaseObjectData* bod = 0;
            PluginFunctions::getPickedObject(list[i].first, bod);
            
            if(bod && (bod->dataType() == DATA_TRIANGLE_MESH || bod->dataType() == DATA_POLY_MESH)) {
                IdList elements;
                for(int j = 0; j < list.size(); ++j) {
                    if(list[j].first == list[i].first) {
                        
                        elements.push_back(list[j].second);
                    }
                }
                IdList oldEdgeSelection = getEdgeSelection(bod->id());
                clearEdgeSelection(bod->id());
                selectEdges(bod->id(), elements);
                
                if(bod->dataType() == DATA_TRIANGLE_MESH)
                    MeshSelection::convertEdgeToHalfedgeSelection(PluginFunctions::triMesh(bod));
                else if(bod->dataType() == DATA_POLY_MESH)
                    MeshSelection::convertEdgeToHalfedgeSelection(PluginFunctions::polyMesh(bod));
                
                clearEdgeSelection(bod->id());
                selectEdges(bod->id(), oldEdgeSelection);
                
                alreadySelectedObjects.insert(list[i].first);
                emit updatedObject(bod->id(), UPDATE_SELECTION);
                emit  createBackup(bod->id(), "Lasso Selection", UPDATE_SELECTION);
            }
        }
    }
    if(_primitiveType & faceType_) {
        PluginFunctions::scenegraphRegionPick(ACG::SceneGraph::PICK_FACE, _region, list);
        
        std::set<int> alreadySelectedObjects;
        
        for(int i = 0; i < list.size(); ++i) {
            
            if(alreadySelectedObjects.count(list[i].first) != 0)
                continue;
            
            BaseObjectData* bod = 0;
            PluginFunctions::getPickedObject(list[i].first, bod);
            
            if(bod && (bod->dataType() == DATA_TRIANGLE_MESH || bod->dataType() == DATA_POLY_MESH)) {
                IdList elements;
                for(int j = 0; j < list.size(); ++j) {
                    if(list[j].first == list[i].first) {
                        
                        elements.push_back(list[j].second);
                    }
                }
                selectFaces(bod->id(), elements);
                alreadySelectedObjects.insert(list[i].first);
                emit updatedObject(bod->id(), UPDATE_SELECTION);
                emit  createBackup(bod->id(), "Lasso Selection", UPDATE_SELECTION);
            }
        }
    }
}

/// Traverse the scenegraph and call the selection function for all mesh nodes
bool SelectVolumeAction::operator()(BaseNode* _node) {
    
    BaseObjectData* object = 0;
    if(PluginFunctions::getPickedObject(_node->id(), object)) {
        
        bool selected = false;
        if (object->dataType(DATA_TRIANGLE_MESH)) {

            TriMesh* m = PluginFunctions::triMesh(object);
            selected = plugin_->volumeSelection(m, state_, &region_, type_, deselection_);

        } else if(object->dataType(DATA_POLY_MESH)) {

            PolyMesh* m = PluginFunctions::polyMesh(object);
            selected = plugin_->volumeSelection(m, state_, &region_, type_, deselection_);
        }

        if (selected){
            emit plugin_->updatedObject(object->id(), UPDATE_SELECTION);
            emit plugin_->createBackup( object->id(), "Lasso Selection", UPDATE_SELECTION);
        }
    }
    return true;
}

/// Create a mesh containing the selection of the given mesh
int MeshObjectSelectionPlugin::createMeshFromSelection(int _objectId, PrimitiveType _primitiveType)
{

  // get object
  BaseObjectData *obj = 0;
  PluginFunctions::getObject(_objectId, obj);

  if (obj == 0) {
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  if (obj->dataType(DATA_TRIANGLE_MESH)) {
    TriMesh* mesh = PluginFunctions::triMesh(obj);

    if (mesh == 0) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    //add an empty mesh
    int id = -1;
    emit addEmptyObject(DATA_TRIANGLE_MESH, id);

    if (id == -1) {
      emit log(LOGERR, tr("Unable to add empty object"));
      return -1;
    }

    BaseObjectData *newObj;
    PluginFunctions::getObject(id, newObj);

    TriMesh* newMesh = PluginFunctions::triMesh(newObj);

    if (newMesh == 0) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    //fill the empty mesh with the selection
    createMeshFromSelection(*mesh, *newMesh,_primitiveType);

    emit updatedObject(id, UPDATE_ALL);

    return id;

  } else if (obj->dataType(DATA_POLY_MESH)) {
    PolyMesh* mesh = PluginFunctions::polyMesh(obj);

    if (mesh == 0) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    //add an empty mesh
    int id;
    emit addEmptyObject(DATA_POLY_MESH, id);

    if (id == -1) {
      emit log(LOGERR, tr("Unable to add empty object"));
      return -1;
    }

    BaseObjectData *newObj;
    PluginFunctions::getObject(id, newObj);

    PolyMesh* newMesh = PluginFunctions::polyMesh(newObj);

    if (newMesh == 0) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    //fill the empty mesh with the selection
    createMeshFromSelection(*mesh, *newMesh,_primitiveType);

    emit updatedObject(id, UPDATE_ALL);

    return id;
  } else {
    emit log(LOGERR, tr("DataType not supported"));
    return -1;
  }
}


Q_EXPORT_PLUGIN2(meshobjectselectionplugin, MeshObjectSelectionPlugin);
