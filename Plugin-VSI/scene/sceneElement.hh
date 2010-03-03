//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2009 by Computer Graphics Group, RWTH Aachen
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
//   $Revision: $
//   $Author: $
//   $Date: $
//
//=============================================================================

#ifndef VSI_SCENEELEMENT_HH
#define VSI_SCENEELEMENT_HH

//== INCLUDES =================================================================
#include <QGraphicsWidget>
#include <QString>
#include <QDomDocument>
#include <QDomElement>
#include <QXmlQuery>

//== FORWARDDECLARATIONS ======================================================
class QGraphicsLinearLayout;

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class GraphicsScene;
class ElementInput;
class ElementOutput;
class ElementFunction;

class Element;
class Text;

//== CLASS DEFINITION =========================================================

/** Widget that represents an context element in the scene
  */
class SceneElement : public QGraphicsWidget
{
  Q_OBJECT

  public:
    /// Constructor
    SceneElement (GraphicsScene *_scene, Element *_element);

    /// Destructor
    ~SceneElement ();

    /// Background painting
    void paint (QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget = 0);

    /// Context VSI::Element
    Element *element () const { return element_; };

    /// Inputs
    QVector<ElementInput *> inputs () { return inputs_; };

    /// Outputs
    QVector<ElementOutput *> outputs () { return outputs_; };

    /// Functions
    QVector<ElementFunction *> functions () { return functions_; };

    /// Scene input
    ElementInput *dataIn () { return dataIn_; };

    /// Scene output
    ElementOutput *dataOut () { return dataOut_; };

    /// Reset code block for code generation
    void resetCodeGeneration ();

    /// Replace block with name _name and id _id with _value
    void replaceCodeBlock (QString _name, QString _id, QString _value);

    /// Unique variable name for code generation
    QString variableId ();

    /// Unique id for identification
    int id () { return id_; };

    /// Code block
    QString code () { return code_; };

    /// Will this element be executed before _e bacause of its connections?
    bool isBefore (SceneElement *_e);

    /// Will this element be executed after _e bacause of its connections?
    bool isAfter (SceneElement *_e);

    /// Save to xml
    void saveToXml (QDomDocument &_doc, QDomElement &_root);

    /// Load from xml
    void loadFromXml (QXmlQuery &_xml);

    /// Scene
    GraphicsScene *graphicsScene () { return scene_; };

  protected:

    // mouse handling for draging
    void mousePressEvent (QGraphicsSceneMouseEvent *_event);
    void mouseMoveEvent (QGraphicsSceneMouseEvent *_event);
    void mouseReleaseEvent (QGraphicsSceneMouseEvent *_event);
    void mouseDoubleClickEvent (QGraphicsSceneMouseEvent *_event);

    // handle movement
    void moveEvent (QGraphicsSceneMoveEvent *_event);

  private slots:

    // show input configuration dialog
    void showInputConfig ();

  private:

    // invalidate (reset) all connections of this element
    void invalidateConnections ();

  private:

    GraphicsScene *scene_;
    Element *element_;

    QVector<ElementInput *> inputs_;
    QVector<ElementOutput *> outputs_;

    ElementInput *dataIn_;
    ElementOutput *dataOut_;

    QVector<ElementFunction *> functions_;

    QVector<ElementInput *> configInputs_;

    QString code_;

    int id_;

    Text *name_;
    Text *elementName_;
    QGraphicsLinearLayout *nameLayout_;
};

//=============================================================================
}
//=============================================================================

#endif
