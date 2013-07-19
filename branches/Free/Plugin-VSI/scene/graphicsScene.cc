/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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


//== INCLUDES =================================================================
#include "graphicsScene.hh"
#include "graphicsView.hh"
#include "sceneElement.hh"
#include "sceneTools.hh"
#include "elementArea.hh"

#include "elementInput.hh"
#include "elementOutput.hh"
#include "elementInOut.hh"
#include "elementFunction.hh"
#include "connection.hh"
#include "connectionPoint.hh"
#include "wayfind.hh"

#include "baseWidget.hh"

#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEngine>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QGraphicsRectItem>
#include <QSet>

#include <QXmlQuery>
#include <QXmlResultItems>
#include <QRegExp>

#include "../parser/context.hh"
#include "../parser/inout.hh"
#include "../parser/output.hh"
#include "../parser/function.hh"

#define SELECTION_RED   0x00
#define SELECTION_GREEN 0x00
#define SELECTION_BLUE  0x7f
#define SELECTION_ALPHA 0x7f

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::GraphicsScene - IMPLEMENTATION
//
//=============================================================================

/// Constructor
GraphicsScene::GraphicsScene (Context *_ctx, ElementFunction *_function) :
  QGraphicsScene (),
  ctx_ (_ctx),
  function_ (_function),
  dndElement_ (0),
  elementArea_ (0),
  selectionActive_ (false),
  blockChanges_ (false),
  activeConnection_ (0),
  currentZ_ (0)
{
  // tools area
  tools_ = new SceneTools (this);
  addItem (tools_);
  tools_->setZValue (10);

  // selecion item
  selection_ = new QGraphicsRectItem ();
  addItem (selection_);
  selection_->setZValue (1);
  selection_->hide ();

  selection_->setBrush(QBrush(QColor(SELECTION_RED,
                                     SELECTION_GREEN,
                                     SELECTION_BLUE,
                                     SELECTION_ALPHA / 2)));

  selection_->setPen(QColor(SELECTION_RED,
                            SELECTION_GREEN,
                            SELECTION_BLUE,
                            SELECTION_ALPHA));

  elementArea_ = new ElementArea (this);
  addItem (elementArea_);
  elementArea_->setZValue (0.0);

  // add start and end
  if (!function_)
  {
    SceneElement *end = new SceneElement (this, ctx_->element ("end"));
    addElement (end);
    SceneElement *start = new SceneElement (this, ctx_->element ("start"));
    addElement (start);
  }
  else
  {
    SceneElement *end = new SceneElement (this, function_->function ()->end ());
    addElement (end);
    SceneElement *start = new SceneElement (this, function_->function ()->start ());
    addElement (start);
  }

  // initalize wayfinding
  wayFind_ = new WayFind (this);

  view_ = new GraphicsView ();
  view_->setScene (this);
  view_->setRenderHint (QPainter::Antialiasing, true);

  if (BaseWidget::getBaseWidget ())
    BaseWidget::getBaseWidget ()->addScene (this);

  connect (this, SIGNAL(sceneRectChanged( const QRectF& )), this, SLOT(sceneResized(const QRectF&)));
  dontMoveStart_ = false;
}

//------------------------------------------------------------------------------

/// destructor
GraphicsScene::~GraphicsScene ()
{
  delete wayFind_;

  if (BaseWidget::getBaseWidget ())
    BaseWidget::getBaseWidget ()->removeScene (this);
  view_->deleteLater ();
}

//------------------------------------------------------------------------------

// draw background
void GraphicsScene::drawBackground(QPainter *_painter, const QRectF &_rect)
{
  QGraphicsScene::drawBackground (_painter, _rect);
}

//------------------------------------------------------------------------------

// drag enter
void GraphicsScene::dragEnterEvent (QGraphicsSceneDragDropEvent *_event)
{
  // create scene element if mimedata is valid
  if (validMimeData (_event->mimeData ()) && !dndElement_ && ctx_->element (mimeDataElementName (_event->mimeData ())))
  {
    dndElement_ = new SceneElement (this, ctx_->element (mimeDataElementName (_event->mimeData ())));
    dndElement_->setOpacity (0.5);
    addElement (dndElement_);

    QPoint off = mimeDataPoint (_event->mimeData ());

    QPointF p = elementArea_->mapFromScene (_event->scenePos ().x (), _event->scenePos ().y ());
    dndElement_->setPos (p.x () - (dndElement_->size ().width () / 2) - off.x(),
                         p.y () - (dndElement_->size ().height () / 2) - off.y());

  }
  else
    _event->ignore ();

}

//------------------------------------------------------------------------------

// drag leave
void GraphicsScene::dragLeaveEvent (QGraphicsSceneDragDropEvent *_event)
{
  // destroy created scene element
  if (validMimeData (_event->mimeData ()) && dndElement_)
  {
    tools_->mouseRelease (_event->scenePos (), dndElement_);
    removeElement (dndElement_);
    delete dndElement_;
    dndElement_ = NULL;
  }
  else
    _event->ignore ();
}

//------------------------------------------------------------------------------

// drag move
void GraphicsScene::dragMoveEvent (QGraphicsSceneDragDropEvent *_event)
{
  // update position of dragged element
  if (validMimeData (_event->mimeData ()) && dndElement_)
  {
    QPoint off = mimeDataPoint (_event->mimeData ());
    QPointF p = elementArea_->mapFromScene (_event->scenePos ().x (), _event->scenePos ().y ());
    dndElement_->setPos (p.x () - (dndElement_->size ().width () / 2) - off.x (),
                         p.y () - (dndElement_->size ().height () / 2) - off.y ());

    tools_->mouseMove (_event->scenePos ());
  }
  else
    _event->ignore ();
}

//------------------------------------------------------------------------------

// drop
void GraphicsScene::dropEvent (QGraphicsSceneDragDropEvent *_event)
{
  // leave dropped element in scene
  if (validMimeData (_event->mimeData ()) && dndElement_)
  {
    tools_->mouseRelease (_event->scenePos (), dndElement_);
    dndElement_->setOpacity (1.0);
    dndElement_ = NULL;
    contentChange ();
  }
  else
    _event->ignore ();
}

//------------------------------------------------------------------------------

/// Returns a new Z value that is above all elements
qreal GraphicsScene::getNewZ ()
{
  currentZ_ += 0.01;
  return currentZ_;
}

//------------------------------------------------------------------------------

/// Moves all elements
void GraphicsScene::moveElements (qreal _dx, qreal _dy, bool _selected)
{
  elementArea_->moveBy (_dx, _dy);
  if (_selected)
    return;

  contentChange ();
  foreach (QGraphicsItem *e, elementArea_->elements ())
    if (e->isSelected ())
    {
      QPointF p = elementArea_->mapFromScene (0, 0) -
                  elementArea_->mapFromScene (_dx, _dy);
      e->moveBy (p.x (), p.y ());
    }
}

//------------------------------------------------------------------------------

/// Scale all elements with scaling center in the center of the scene
void GraphicsScene::scaleElements (qreal _delta)
{
  QRectF rect = sceneRect();
  scaleElements (_delta, QPointF (rect.x () + (rect.width () / 2.0),
                                  rect.y () + (rect.height () / 2.0)));
}

//------------------------------------------------------------------------------

/// Scale all elements with scaling center _canter
void GraphicsScene::scaleElements (qreal _delta, QPointF _center)
{
  QPointF p(elementArea_->mapFromScene (_center.x (), _center.y ()));
  elementArea_->setTransform(QTransform().translate(p.x (), p.y ()).scale(_delta, _delta).translate(-p.x (), -p.y ()), true);
}

//------------------------------------------------------------------------------

/// Add the element to the scene
void GraphicsScene::addElement (SceneElement *_element)
{
  contentChange ();
  addItem (_element);
  elementArea_->addElement (_element);
}

//------------------------------------------------------------------------------

/// Remove the element from the scene
bool GraphicsScene::removeElement (SceneElement *_element)
{
  contentChange ();
  bool rv = elementArea_->removeElement (_element); 
  if (rv)
    removeItem (_element);

  return rv;
}

//------------------------------------------------------------------------------

/// Redirect mouse movement to tools area
void GraphicsScene::mouseMove (QPointF _pos)
{
  tools_->mouseMove (_pos);
}

//------------------------------------------------------------------------------

/// Redirect mouse release to tools area
void GraphicsScene::mouseRelease (QPointF _pos, QGraphicsItem *_item)
{
  tools_->mouseRelease (_pos, _item);
}

//------------------------------------------------------------------------------

QRectF GraphicsScene::elementsBoundingRect ()
{
  if (elementArea_)
    return elementArea_->mapToScene (elementArea_->childrenBoundingRect ()).boundingRect ();
  else
    return QRectF ();
}

//------------------------------------------------------------------------------

/// Bounding rectangle of all scene elements
const QList<SceneElement *>& GraphicsScene::elements () const
{
  return elementArea_->elements ();
}

//------------------------------------------------------------------------------

// start selection rectangle if pressing on empty area of scene
void GraphicsScene::mousePressEvent (QGraphicsSceneMouseEvent *_event)
{
  QPainterPath p;

  foreach (QGraphicsItem *e, selectedItems ())
    p += e->mapToScene (e->shape ());

  QGraphicsScene::mousePressEvent (_event);
  selectionStart_ = _event->scenePos ();

  if (itemAt (selectionStart_,QTransform()) != elementArea_ && !_event->isAccepted ())
  {
    if (_event->modifiers () & (Qt::ControlModifier | Qt::ShiftModifier))
      setSelectionArea (p);

    selectionActive_ = true;
    selection_->setRect (QRectF (selectionStart_, QSizeF (0, 0)));
    selection_->show ();
    _event->accept ();
  }

  dontMoveStart_ = true;
}

//------------------------------------------------------------------------------

// update selection rectangle
void GraphicsScene::mouseMoveEvent (QGraphicsSceneMouseEvent *_event)
{
  if (selectionActive_)
  {
    selection_->setRect (QRectF (selectionStart_, _event->scenePos ()).normalized ());
    _event->accept ();
  }
  else if (activeConnection_)
  {
    activeConnection_->mouseMoveEvent (_event);
  }
  else
    QGraphicsScene::mouseMoveEvent (_event);
}

//------------------------------------------------------------------------------

// select all elements in selection rectangle
void GraphicsScene::mouseReleaseEvent (QGraphicsSceneMouseEvent *_event)
{
  if (selectionActive_)
  {
    selection_->hide ();
    QPainterPath p;

    p.addRect (QRectF (selectionStart_, _event->scenePos ()).normalized ());

    if (_event->modifiers () == Qt::ControlModifier)
    {
      p += selectionArea ();
      setSelectionArea (p);
    }
    else if (_event->modifiers () == Qt::ShiftModifier)
    {
      foreach (QGraphicsItem *e, items (QRectF (selectionStart_, _event->scenePos ()).normalized ()))
        if (e->isSelected ())
          e->setSelected (false);
    }
    else
      setSelectionArea (p);

    selectionActive_ = false;

    _event->accept ();
  }
  if (activeConnection_)
  {
    activeConnection_->mouseReleaseEvent (_event);
  }

  QGraphicsScene::mouseReleaseEvent (_event);
}

//------------------------------------------------------------------------------

// returns all scene elements. Also all elements of sub-functions
QList<SceneElement *> GraphicsScene::getAllElements()
{
  QList<SceneElement *> rv;

  foreach (SceneElement *e, elementArea_->elements ())
  {
    rv.append (e);
    foreach (ElementFunction *ef, e->functions ())
    {
      rv.append (ef->scene ()->getAllElements ());
    }
  }

  return rv;
}

//------------------------------------------------------------------------------

/// Generate textual script code (_codeOnly = only pure code block)
QString GraphicsScene::generateCode (QString &errors, bool _codeOnly)
{
  QString rv = "";

  // only code block for functions
  if (!_codeOnly)
  {
    // collect all elements and functions
    QSet<Element *> set;
    QList<ElementFunction *> functions;

    foreach (SceneElement *e, getAllElements ())
    {
      set.insert (e->element ());
      foreach (ElementFunction *ef, e->functions ())
        functions.append (ef);
    }

    // add precode block for each element only once
    foreach (Element *e, set)
      if (!e->precode ().isEmpty ())
      {
        rv += "// - ";
        rv += e->shortDescription ();
        rv += " -\n";
        rv += e->precode ();
        rv += "\n";
      }

    // add functiond
    if (!functions.isEmpty ())
    {
      rv += "// ------- FUNCTIONS -----------\n\n";
    }

    foreach (ElementFunction *ef, functions)
    {
      rv += "// - ";
      rv += ef->element ()->element ()->shortDescription ();
      rv += " - ";
      rv += ef->function ()->shortDescription ();
      rv += " -\n";
      rv += "function func_" + ef->element ()->variableId () + "_" + ef->function ()->name ();

      QString param;
      foreach (Output *o, ef->function ()->start ()->outputs ())
      {
        param += o->name () + ", ";
      }

      if (!param.isEmpty ())
        param.remove (param.length () - 2, 2);

      rv += " (" + param + ")\n{\n";

      QString code = ef->scene ()->generateCode (errors, true);

      rv += code + "}\n\n";
    }

    if (!functions.isEmpty ())
    {
      rv += "// ------- END FUNCTIONS -------\n\n";
    }
  }


  QList<SceneElement *> elements = elementArea_->elements ();

  SceneElement *start = 0;
  SceneElement *end = 0;

  // reset code block of all elements and find start element
  foreach (SceneElement *e, elements)
  {
    e->resetCodeGeneration ();

    if ((!function_ && e->element ()->name () == "start") ||
        (function_ && function_->function ()->start () == e->element ()))
      start = e;
    if ((!function_ && e->element ()->name () == "end") ||
        (function_ && function_->function ()->end () == e->element ()))
      end = e;
  }

  if (!start)
  {
    // should never happen
    errors += "No start element!";
    return "";
  }

  elements.removeAll (start);
  elements.removeAll (end);

  // Ignore empty scenes
  if (elements.isEmpty() && end->inputs().isEmpty ())
    end = NULL;

  rv += "// --- CODE BEGIN ---\n\n";

  updateConnections (start, true);

  SceneElement *found = (SceneElement*)1;

  // iterate over all elements
  while ((!elements.empty () || end) && found)
  {
    found = NULL;

    // pick element where all inputs are set
    foreach (SceneElement *e, elements)
    {
      bool set = true;
      foreach (ElementInput *i, e->inputs ())
        if (!i->valid ())
          set = false;

      if (e->dataIn () && !e->dataIn ()->valid ())
        set = false;

      if (set)
        found = e;
    }

    if (!found)
    {
      bool set = true;
      foreach (ElementInput *i, end->inputs ())
        if (!i->valid ())
          set = false;

      if (end->dataIn () && !end->dataIn ()->valid ())
        set = false;

      // Ignore end elements without inputs
      if (set)
      {
        if (!end->inputs ().isEmpty ())
          found = end;
        end = NULL;
      }
    }

    if (found)
    {
      elements.removeAll (found);

      rv += "// - ";
      rv += found->element ()->shortDescription ();
      rv += " -\n";
      rv += updateConnections (found, false);

      QString dName = "ask_user_output_" + found->variableId();
      QString dInputs = "";

      // replace all inputs that are not connected with user defined value or prepare for asking during execution
      foreach (ElementInput *i, found->inputs ()){
        if (i->isSet ())
        {
          found->replaceCodeBlock ("is_set", i->inOut ()->name (), "true");
          found->replaceCodeBlock ("input", i->inOut ()->name (), i->value ());
        }
        else if (!i->connections ().isEmpty ())
        {
          found->replaceCodeBlock ("is_set", i->inOut ()->name (), "true");
        }
        else if (i->connections ().isEmpty ())
        {
          if (i->state () & Input::Optional && !i->isForceAskSet ())
          {
            found->replaceCodeBlock ("input", i->inOut ()->name (), "\"\"");
            found->replaceCodeBlock ("is_set", i->inOut ()->name (), "false");
          }
          else
          {
            found->replaceCodeBlock ("is_set", i->inOut ()->name (), "true");
            found->replaceCodeBlock ("input", i->inOut ()->name (), dName + "." + i->inOut ()->name ());
            dInputs += i->inOut ()->name () + ",";
          }
        }

        // Replace the is_connected block for inputs as well.
        if (i->connections ().isEmpty ())
          found->replaceCodeBlock ("is_connected", i->inOut ()->name (), "false");
        else
          found->replaceCodeBlock ("is_connected", i->inOut ()->name (), "true");
      }

      foreach (ElementFunction *ef, found->functions ())
        found->replaceCodeBlock ("function", ef->function ()->name (), "func_" + found->variableId () + "_" + ef->function ()->name ());

      if (!dInputs.isEmpty ())
      {
        // remove last ,
        dInputs.remove (dInputs.length () - 1, 1);
        rv += "// Ask user for missing input values\n";
        rv += "var " + dName + " = visualscripting.askForInputs (\"" + found->element ()->name ();
        rv += "\", \"" + dInputs + "\");\n";
      }

      rv += found->code ();
      rv += "\n";
    }
  }

  rv += "// --- CODE END ---\n";

  if (end)
    elements.append (end);

  if (!elements.isEmpty ())
  {
    // find all elements that have real missing inputs
    foreach (SceneElement *e, elements)
    {
      foreach (ElementOutput *o, e->outputs ())
      {
        foreach (Connection *c, o->connections ())
          if (c->input ())
          {
            c->input ()->setValid (true);
          }
      }

      if (e->dataOut ())
        foreach (Connection *c, e->dataOut ()->connections ())
          if (c->input ())
          {
            c->input ()->setValid (true);
            break;
          }
    }

    QList<SceneElement *> fixE;
    foreach (SceneElement *e, elements)
    {
      bool set = true;
      foreach (ElementInput *i, e->inputs ())
        if (!i->valid ())
          set = false;

      if (e->dataIn () && !e->dataIn ()->valid ())
        set = false;

      if (!set)
        fixE.append (e);
    }

    errors += "<dl><dt>";
 
    if (!function_)
      errors += "<b>Function: </b>Main";
    else
      errors += "<b>Function: </b>" + function_->function ()->shortDescription();

    errors += "</dt><dd>";

    foreach (SceneElement *e, fixE)
    {
      errors += "<b>Element: </b>" + e->element ()->shortDescription () + " <i>(";
      foreach (ElementInput *i, e->inputs ())
        if (!i->valid ())
          errors += i->inOut ()->shortDescription () + ", ";

      if (e->dataIn () && !e->dataIn ()->valid ())
        errors += e->dataIn ()->inOut ()->shortDescription ();

      if (errors.endsWith (", "))
        errors.remove (errors.length () - 2, 2);
      errors += ")</i><br>";
    }

    if (errors.endsWith ("<br>"))
        errors.remove (errors.length () - 4, 4);

    errors += "<dd></dl>";

    return "";
  }

  // indent code
  if (!_codeOnly)
  {
    QStringList code = rv.split ('\n');
    QString indent = "";
    rv.clear ();

    foreach (QString s, code)
    {
      if (s.contains ('}') && !indent.isEmpty ())
        indent.remove (indent.length () - 2, 2);
      rv += indent + s + "\n";
      if (s.contains ('{'))
        indent += "  ";
    }
  }

  return rv;
}

//------------------------------------------------------------------------------

// Mark all inputs that are connected to the outputs of _from as valid
QString GraphicsScene::updateConnections (SceneElement *_from, bool _isStart)
{
  QString rv;
  foreach (ElementOutput *o, _from->outputs ())
  {
    QString oName;

    if (!_isStart)
    {
      oName = "output_" + _from->variableId() + "_" + o->inOut ()->name ();
      rv += "var " + oName + "\n";

      _from->replaceCodeBlock ("output", o->inOut ()->name (), oName);

      if (o->connections ().isEmpty ())
        _from->replaceCodeBlock ("is_connected", o->inOut ()->name (), "false");
      else
        _from->replaceCodeBlock ("is_connected", o->inOut ()->name (), "true");
    }
    else
      oName = o->inOut ()->name ();

    foreach (Connection *c, o->connections ())
      if (c->input ())
      {
        c->input ()->setValid (true);
        c->input ()->element ()->replaceCodeBlock ("input", c->input ()->inOut ()->name (), oName);
      }
  }

  if (_from->dataOut ())
    foreach (Connection *c, _from->dataOut ()->connections ())
      if (c->input ())
      {
        c->input ()->setValid (true);
        break;
      }
  return rv;
}

//------------------------------------------------------------------------------

// Checks for supported mimetype
bool GraphicsScene::validMimeData(const QMimeData *_m)
{
  return _m->hasFormat ("application/x-openflipper.vsi");
}

//------------------------------------------------------------------------------

// gets the element name from the mimetype data
QString GraphicsScene::mimeDataElementName(const QMimeData *_m)
{
  if (!validMimeData (_m))
    return "";
  QStringList sl = QString (_m->data ("application/x-openflipper.vsi")).split (";");

  if (sl.length() != 3)
    return "";

  return sl[2];
}

//------------------------------------------------------------------------------

// gets the mouse hotspot from the mimetype data
QPoint GraphicsScene::mimeDataPoint(const QMimeData *_m)
{
  if (! validMimeData (_m))
    return QPoint (0, 0);

  QStringList sl = QString (_m->data ("application/x-openflipper.vsi")).split (";");

  if (sl.length() != 3)
    return QPoint (0, 0);

  bool ok1, ok2;
  int x = sl[0].toInt (&ok1);
  int y = sl[1].toInt (&ok2);

  if (!ok1 || !ok2)
    return QPoint (0, 0);

  return QPoint (x, y);
}

//------------------------------------------------------------------------------

/// Save to xml
void GraphicsScene::saveToXml (QDomDocument &_doc, QDomElement &_root)
{
  QTransform t = elementArea_->transform ();
  QDomElement s = _doc.createElement("transform");
  _root.appendChild(s);
  QDomText txt = _doc.createTextNode(
    QString::number (t.m11 ()) + "|" + QString::number (t.m12 ()) + "|" + QString::number (t.m13 ()) + "|" +
    QString::number (t.m21 ()) + "|" + QString::number (t.m22 ()) + "|" + QString::number (t.m23 ()) + "|" +
    QString::number (t.m31 ()) + "|" + QString::number (t.m32 ()) + "|" + QString::number (t.m33 ())
  );
  s.appendChild(txt);

  s = _doc.createElement("x");
  _root.appendChild(s);
  txt = _doc.createTextNode (QString::number(elementArea_->pos ().x ()));
  s.appendChild(txt);

  s = _doc.createElement("y");
  _root.appendChild(s);
  txt = _doc.createTextNode (QString::number(elementArea_->pos ().y ()));
  s.appendChild(txt);

  QDomElement el = _doc.createElement("elements");
  _root.appendChild(el);

  foreach (SceneElement *e, elements ())
    e->saveToXml (_doc, el);
}

//------------------------------------------------------------------------------

/// Load from xml
void VSI::GraphicsScene::loadFromXml (QXmlQuery &_xml)
{
  clearScene (true);

  QStringList sl = Context::getXmlString (_xml, "transform/string(text ())").split ("|");

  qreal m[9];

  if (sl.length () == 9)
  {
    bool ok;
    for (unsigned int i = 0; i < 9; i++)
    {
      m[i] = sl[i].toDouble (&ok);
      if (!ok)
        break;
    }
    if (ok)
      elementArea_->setTransform (QTransform (m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8]), false);
  }

  bool ok1, ok2;
  double x, y;
  QString val;

  val = Context::getXmlString (_xml, "x/string(text())");
  x = val.toDouble (&ok1);

  val = Context::getXmlString (_xml, "y/string(text())");
  y = val.toDouble (&ok2);

  if (ok1 && ok2)
    elementArea_->setPos (x, y);

  _xml.setQuery ("elements/element");

  QXmlResultItems el;

  if (_xml.isValid ())
  {
    _xml.evaluateTo (&el);
    QXmlItem item (el.next ());
    while (!item.isNull ())
    {
      QXmlQuery q (_xml);
      q.setFocus (item);
      loadElement (q);
      item = el.next ();
    }
  }

  _xml.setQuery ("elements/element/inputs/input/connection");

  if (_xml.isValid ())
  {
    _xml.evaluateTo (&el);
    QXmlItem item (el.next ());
    while (!item.isNull ())
    {
      QXmlQuery q (_xml);
      q.setFocus (item);
      loadConnection (q);
      item = el.next ();
    }
  }

  QTimer::singleShot(0, this, SLOT (updateConnections()));
}

//------------------------------------------------------------------------------

// load element from xml
void GraphicsScene::loadElement(QXmlQuery & _xml)
{
  QString name = Context::getXmlString (_xml, "string(@name)");
  if (name.isEmpty () || !ctx_->element (name))
    return;

  SceneElement *e = new SceneElement (this, ctx_->element (name));
  addElement (e);

  e->loadFromXml (_xml);
}

//------------------------------------------------------------------------------

// load connection from xml
void GraphicsScene::loadConnection(QXmlQuery & _xml)
{
  bool ok;
  int inId, outId;

  QString inEl = Context::getXmlString (_xml, "../../../string(@name)");
  QString inElId = Context::getXmlString (_xml, "../../../id/string(text ())");
  QString inIn = Context::getXmlString (_xml, "../string(@name)");

  inId = inElId.toInt (&ok);
  if (!ok)
    return;

  QString outEl = Context::getXmlString (_xml, "string(@element)");
  QString outElId = Context::getXmlString (_xml, "string(@element_id)");
  QString outOut = Context::getXmlString (_xml, "string(@output)");

  outId = outElId.toInt (&ok);
  if (!ok)
    return;

  SceneElement *in = NULL, *out = NULL;

  foreach (SceneElement *e, elements ())
  {
    if (e->element ()->name () == inEl && e->id () == inId)
      in = e;
    else if (e->element ()->name () == outEl && e->id () == outId)
      out = e;
  }

  if (!in || !out)
    return;

  ConnectionPoint *p1 = NULL, *p2 = NULL;

  if (in->dataIn () && in->dataIn ()->inOut ()->name () == inIn &&
      out->dataOut () && out->dataOut ()->inOut ()->name () == outOut)
  {
    p1 = in->dataIn ()->connectionPointItem ();
    p2 = out->dataOut ()->connectionPointItem ();
  }
  else
  {
    foreach (ElementInput *i, in->inputs())
      if (i->inOut ()->name () == inIn)
      {
        p1 = i->connectionPointItem ();
        break;
      }

    foreach (ElementOutput *o, out->outputs())
      if (o->inOut ()->name () == outOut)
      {
        p2 = o->connectionPointItem ();
        break;
      }
  }

  if (p1 && p2)
    new Connection (p1, p2, this);
}

//------------------------------------------------------------------------------

/// clear the whole scene (_start = keep start element)
void GraphicsScene::clearScene (bool _start)
{
  elementArea_->resetTransform ();
  elementArea_->setPos (0, 0);
  foreach (SceneElement *e, elements())
  {
    bool rv = elementArea_->removeElement (e, true);
    if (rv)
    {
      removeItem (e);
      delete e;
    }
  }
  if (!_start)
  {
  if (!function_)
    {
      SceneElement *start = new SceneElement (this, ctx_->element ("start"));
      addElement (start);

      start->setPos(5, (height () - start->size ().height ()) / 2);

      SceneElement *end = new SceneElement (this, ctx_->element ("end"));
      addElement (end);

      end->setPos(width () - end->size ().width () - 5, (height () - end->size ().height ()) / 2);
    }
    else
    {
      SceneElement *start = new SceneElement (this, function_->function ()->start ());
      addElement (start);

      start->setPos(5, (height () - start->size ().height ()) / 2);

      SceneElement *end = new SceneElement (this, function_->function ()->end ());
      addElement (end);

      end->setPos(width () - end->size ().width () - 5, (height () - end->size ().height ()) / 2);
    }
  }
  dontMoveStart_ = false;
}

//------------------------------------------------------------------------------

// update all connections in the scene
void GraphicsScene::updateConnections()
{
  foreach (SceneElement *e, elements ())
  {
    foreach (ElementInput *i, e->inputs ())
      foreach (Connection *c, i->connections ())
        c->updatePositions ();

    foreach (ElementOutput *o, e->outputs ())
      foreach (Connection *c, o->connections ())
        c->updatePositions ();

    if (e->dataIn ())
      foreach (Connection *c, e->dataIn ()->connections ())
        c->updatePositions ();

    if (e->dataOut ())
      foreach (Connection *c, e->dataOut ()->connections ())
        c->updatePositions ();
  }
}

//------------------------------------------------------------------------------

/// handle content changes
void GraphicsScene::contentChange ()
{
  if (blockChanges_)
    return;

  emit contentChanged ();
  dontMoveStart_ = true;
}

//------------------------------------------------------------------------------

void VSI::GraphicsScene::sceneResized(const QRectF & /*_rect*/)
{
  if (dontMoveStart_)
    return;

  blockChanges_ = true;

  QList<SceneElement *> elements = elementArea_->elements ();

  SceneElement *start = 0;
  SceneElement *end = 0;

  // reset code block of all elements and find start element
  foreach (SceneElement *e, elements)
  {
    e->resetCodeGeneration ();

    if ((!function_ && e->element ()->name () == "start") ||
        (function_ && function_->function ()->start () == e->element ()))
      start = e;
    if ((!function_ && e->element ()->name () == "end") ||
        (function_ && function_->function ()->end () == e->element ()))
      end = e;
  }

  if (start)
    start->setPos(5, (height () - start->size ().height ()) / 2);
  if (end)
    end->setPos(width () - end->size ().width () - 5, (height () - end->size ().height ()) / 2);

  blockChanges_ = false;
}

//=============================================================================
}
//=============================================================================










