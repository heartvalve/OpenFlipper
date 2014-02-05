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

//== INCLUDES =================================================================
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QPainter>
#include <QPalette>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsGridLayout>
#include <QGraphicsView>
#include <QRegExp>

#include <QDomText>
#include <QXmlResultItems>

#include "sceneElement.hh"
#include "graphicsScene.hh"

#include "text.hh"
#include "button.hh"
#include "connectionPoint.hh"
#include "connection.hh"

#include "elementInput.hh"
#include "elementOutput.hh"
#include "elementFunction.hh"

#include "config/configDialog.hh"

#include "../parser/element.hh"
#include "../parser/input.hh"
#include "../parser/type.hh"
#include "../parser/context.hh"
#include "../parser/function.hh"

#define BACKGROUND_RED   0x00
#define BACKGROUND_GREEN 0x00
#define BACKGROUND_BLUE  0x00
#define BACKGROUND_ALPHA 0xff

#define SELECTED_BACKGROUND_RED   0x00
#define SELECTED_BACKGROUND_GREEN 0x00
#define SELECTED_BACKGROUND_BLUE  0x6f
#define SELECTED_BACKGROUND_ALPHA 0xff


//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::SceneElement - IMPLEMENTATION
//
//=============================================================================


SceneElement::SceneElement (GraphicsScene *_scene, Element *_element) :
  scene_ (_scene),
  element_ (_element),
  dataIn_ (0),
  dataOut_ (0),
  id_ (_element->getNewId ())
{
  int index = 1;
  bool hasConfig = false;

  QGraphicsLinearLayout *layout = new QGraphicsLinearLayout (Qt::Vertical);
  layout->setSpacing (0);

  nameLayout_ = new QGraphicsLinearLayout (Qt::Vertical);
  nameLayout_->setSpacing(0);
  nameLayout_->setContentsMargins(15, 0, 15, 0);
  // add name text
  name_ = new Text (element_->shortDescription (), this);

  name_->setAlignment (Qt::AlignHCenter);

  QFont font = name_->font ();
  if (font.pointSize () != -1)
    font.setPointSize (font.pointSize () + 1);
  else
    font.setPixelSize (font.pixelSize () + 1);
  font.setBold (true);
  name_->setFont (font);

  nameLayout_->addItem(name_);
  nameLayout_->setAlignment (name_, Qt::AlignCenter);

  elementName_ = new Text (element_->shortDescription (), this);
  font = elementName_->font ();
  if (font.pointSize () != -1)
    font.setPointSize (font.pointSize () - 2);
  else
    font.setPixelSize (font.pixelSize () - 2);
  elementName_->setFont (font);

  nameLayout_->addItem(elementName_);
  nameLayout_->setAlignment (elementName_, Qt::AlignCenter);

  elementName_->hide ();

  layout->addItem(nameLayout_);
  layout->setAlignment (nameLayout_, Qt::AlignCenter);

  // add inputs
  QGraphicsGridLayout *inGrid = new QGraphicsGridLayout;
  inGrid->setContentsMargins (0,0,5,0);
  int row = 0;
  qreal typeWidth = 0;
  qreal height = 1000;

  foreach (Input *in, element_->inputs ())
  {
    ElementInput *i = new ElementInput (in, this);
    inputs_.append (i);

    if (!(in->state () & Input::NoUserInput))
    {
      hasConfig = true;
      configInputs_.append (i);
    }

    if (in->state () & Input::NoExternalInput)
    {
      i->connectionPointItem ()->hide ();
      i->typeTextItem ()->hide ();
      i->descriptionTextItem ()->hide ();
      continue;
    }

    inGrid->addItem (i->connectionPointItem (), row, 0, Qt::AlignLeft | Qt::AlignVCenter);
    inGrid->addItem (i->typeTextItem (), row, 1, Qt::AlignLeft | Qt::AlignVCenter);
    inGrid->addItem (i->descriptionTextItem (), row, 2, Qt::AlignLeft | Qt::AlignVCenter);

    height = qMin (height, i->typeTextItem ()->preferredHeight () / 2);
    typeWidth = qMax (i->typeTextItem ()->preferredWidth (), typeWidth);
    row ++;
  }

  if (!element_->inputs ().isEmpty ())
  {
    inGrid->setColumnMinimumWidth (1, qMin (typeWidth, 100.0));
    inGrid->setHorizontalSpacing (1);
    layout->insertItem (index, inGrid);
    layout->setAlignment (inGrid, Qt::AlignLeft);
    layout->setItemSpacing (index - 1, 10);
    index++;
  }

  // add outputs
  QGraphicsGridLayout *outGrid = new QGraphicsGridLayout;
  outGrid->setContentsMargins (5,0,0,0);
  row = 0;
  typeWidth = 0;
  height = 1000;

  foreach (Output *out, element_->outputs ())
  {
    ElementOutput *o = new ElementOutput (out, this);

    outGrid->addItem (o->connectionPointItem (), row, 2, Qt::AlignRight | Qt::AlignVCenter);
    outGrid->addItem (o->typeTextItem (), row, 1, Qt::AlignRight | Qt::AlignVCenter);
    outGrid->addItem (o->descriptionTextItem (), row, 0, Qt::AlignRight | Qt::AlignVCenter);

    height = qMin (height, o->typeTextItem ()->preferredHeight () / 2);
    typeWidth = qMax (o->typeTextItem ()->preferredWidth (), typeWidth);
    row ++;

    outputs_.append (o);
  }

  if (!element_->outputs ().isEmpty ())
  {
    outGrid->setColumnMinimumWidth (1, qMin (typeWidth, 100.0));
    outGrid->setHorizontalSpacing (1);
    layout->insertItem(index, outGrid);
    layout->setAlignment (outGrid, Qt::AlignRight);
    layout->setItemSpacing (index - 1, 10);
    index++;
  }

  if (hasConfig || !element_->functions ().isEmpty ())
  {
    QGraphicsLinearLayout *hl = new QGraphicsLinearLayout (Qt::Horizontal);
    QGraphicsLinearLayout *vl = new QGraphicsLinearLayout (Qt::Vertical);
    hl->setContentsMargins(15, 0, 15, 0);

    // add input config button
    if (hasConfig)
    {
      Button *configButton = new Button ("Configure", this);
      configButton->setBrush (Qt::black);
      configButton->setBackgroundBrush (Qt::lightGray);
      configButton->setBackground (true, true);
      configButton->setHorizontalStretch (true);
      configButton->setAlignment (Qt::AlignHCenter);

      vl->addItem (configButton);

      connect (configButton, SIGNAL (pressed()), this, SLOT (showInputConfig()));
    }

    // add function edit buttons
    foreach (Function *f, element_->functions ())
    {
      ElementFunction *ef = new ElementFunction (f, this);

      Button *config = new Button ("Edit:" + f->shortDescription (), this);
      config->setBrush (Qt::black);
      config->setBackgroundBrush (Qt::lightGray);
      config->setBackground (true, true);
      config->setAlignment (Qt::AlignHCenter);

      vl->addItem (config);

      connect (config, SIGNAL (pressed()), ef, SLOT (editFunction ()));

      functions_.append (ef);
    }

    hl->addStretch ();
    hl->addItem (vl);
    hl->addStretch ();

    layout->insertItem (index, hl);
    layout->setAlignment (hl, Qt::AlignCenter);
    layout->setItemSpacing (index - 1, 10);
    index++;
  }

  // add scenegraph input / output
  if (element_->dataOut () || element_->dataIn ())
  {
    QGraphicsGridLayout *sIO = new QGraphicsGridLayout ();
    int col = 0;
    int lo = 0, ro = 0;

    // Start node has no scene graph input
    if (element_->dataIn ())
    {
      dataIn_ = new ElementInput (element_->dataIn (), this);
      dataIn_->typeTextItem ()->hide ();
      sIO->addItem (dataIn_->connectionPointItem (), 0, col++, Qt::AlignLeft | Qt::AlignVCenter);
      sIO->addItem (dataIn_->descriptionTextItem (), 0, col++, Qt::AlignCenter);
    } else
      lo = 5;

    if (element_->dataOut ())
    {
      dataOut_ = new ElementOutput (element_->dataOut (), this);
      dataOut_->typeTextItem ()->hide ();
      if (dataIn_)
      {
        dataOut_->descriptionTextItem ()->hide ();
        dataIn_->descriptionTextItem ()->setBackground (false, false);
        dataIn_->descriptionTextItem ()->setAlignment (Qt::AlignHCenter);
      }
      else
      {
        sIO->addItem (dataOut_->descriptionTextItem (), 0, col++, Qt::AlignCenter);
      }
      sIO->addItem (dataOut_->connectionPointItem (), 0, col, Qt::AlignRight | Qt::AlignVCenter);
    } else
      ro = 5;

    sIO->setContentsMargins (lo, 0, ro, 0);
    sIO->setHorizontalSpacing (1);

    layout->insertItem (index, sIO);
    layout->setAlignment (sIO, Qt::AlignCenter);
    layout->setItemSpacing (index - 1, 10);
    index++;
  }

  layout->setItemSpacing (0, 1);

  layout->setContentsMargins (0, 2, 0, 6);

  setLayout (layout);

  if (layout->preferredWidth() < 200)
    setMinimumWidth (layout->preferredWidth());
  else
    setMinimumWidth (200);

  setZValue (scene_->getNewZ ());

  setFlag (QGraphicsItem::ItemIsSelectable, true);
  setFlag (QGraphicsItem::ItemIsMovable, true);

  setMaximumWidth (200);

  setToolTip (element_->longDescription ());
}

//------------------------------------------------------------------------------

/// Destructor
SceneElement::~SceneElement ()
{
  foreach (ElementInOut *e, inputs_)
    delete e;
  foreach (ElementInOut *e, outputs_)
    delete e;

  if (dataIn_)
    delete dataIn_;
  if (dataOut_)
    delete dataOut_;
}

//------------------------------------------------------------------------------

/// Background painting
void SceneElement::paint (QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget)
{
  int w = geometry().width() - 20;
  int h = geometry().height();

  _painter->setRenderHint(QPainter::Antialiasing, true);
  if (isSelected ())
    _painter->setBrush(QBrush(QColor(SELECTED_BACKGROUND_RED,
                                     SELECTED_BACKGROUND_GREEN,
                                     SELECTED_BACKGROUND_BLUE,
                                     SELECTED_BACKGROUND_ALPHA)));
  else
    _painter->setBrush(QBrush(QColor(BACKGROUND_RED,
                                     BACKGROUND_GREEN,
                                     BACKGROUND_BLUE,
                                     BACKGROUND_ALPHA)));

  _painter->setPen (Qt::NoPen);
  _painter->drawRoundedRect(10, 0, w, h, 5, 5);

  float wH = w / 2.0;
  float wT = w / 3.0;

  QPainterPath path;
  QRadialGradient rG;
  QLinearGradient lG;

  path.moveTo (10, 20);
  path.lineTo (10, 5);
  path.arcTo (10, 0, 10, 10, 180, -90);
  path.lineTo (w + 6, 0);
  path.arcTo (w, 0, 10, 10, 90, -90);
  path.lineTo (w + 10, 10);

  path.cubicTo (QPointF (10 + wH, 10), QPointF (10 + wH, 20), QPointF (10, 20));

  rG.setCenter (10 + wT, 0);
  rG.setFocalPoint(10 + wT, 0);
  rG.setRadius (w);
  rG.setColorAt(0, QColor (255, 255, 255, 128));
  rG.setColorAt(1, Qt::transparent);
  _painter->setBrush (rG);
  _painter->setPen (Qt::NoPen);
  _painter->drawPath(path);

  path = QPainterPath ();
  path.moveTo (w + 5, 0);
  path.arcTo (w, 0, 10, 10, 90, -90);
  path.lineTo (w + 10, h - 5);
  path.lineTo (w + 5, h - 5);
  path.lineTo (w + 5, 0);

  lG.setStart (w + 5, 0);
  lG.setFinalStop(w + 10, 0);
  lG.setColorAt(0, Qt::transparent);
  lG.setColorAt(1, QColor (255, 255, 255, 128));
  _painter->setBrush (lG);
  _painter->drawPath(path);

  path = QPainterPath ();
  path.moveTo (10, h - 5);
  path.arcTo (10, h - 10, 10, 10, 180, 90);
  path.lineTo (w + 5, h);
  path.lineTo (w + 5, h - 5);
  path.lineTo (10, h - 5);
  lG.setStart (0, h - 5);
  lG.setFinalStop (0, h);
  _painter->setBrush (lG);
  _painter->drawPath (path);

  path = QPainterPath ();
  path.moveTo (w + 5, h);
  path.arcTo (w, h - 10, 10, 10, -90, 90);
  path.lineTo (w + 5, h - 5);
  path.lineTo (w + 5, h);
  rG.setCenter (w + 5, h - 5);
  rG.setFocalPoint(w + 5, h - 5);
  rG.setRadius (5);
  rG.setColorAt (0, Qt::transparent);
  rG.setColorAt (1, QColor (255, 255, 255, 128));
  _painter->setBrush (rG);
  _painter->drawPath(path);

  QGraphicsWidget::paint (_painter, _option, _widget);
}

//------------------------------------------------------------------------------

// Start draging on mouse press
void SceneElement::mousePressEvent (QGraphicsSceneMouseEvent *_event)
{
  QGraphicsWidget::mousePressEvent (_event);
  setZValue (scene_->getNewZ ());
}

//------------------------------------------------------------------------------

// move all selected elements
void SceneElement::mouseMoveEvent (QGraphicsSceneMouseEvent *_event)
{
  QGraphicsWidget::mouseMoveEvent (_event);
}

//------------------------------------------------------------------------------

// stop moving
void SceneElement::mouseReleaseEvent (QGraphicsSceneMouseEvent *_event)
{
  QGraphicsWidget::mouseReleaseEvent (_event);
  scene_->mouseRelease (_event->scenePos (), this);
}

//------------------------------------------------------------------------------

/// Double click occured. We can't use mouseDoubleClickEvent because we won't get one for the ConecctionPoints
void SceneElement::mouseDoubleClickEvent (QGraphicsSceneMouseEvent *_event)
{
  QPointF _scenePos = _event->scenePos ();
  if (name_->boundingRect ().contains (name_->mapFromScene (_scenePos)) ||
      elementName_->boundingRect ().contains (elementName_->mapFromScene (_scenePos)))
  {
    ConfigDialog d (QVector<ElementInput *> (), name_->text (), scene ()->views ()[0]);
    d.setWindowTitle ("Element Name Configuration");
    if (d.exec () == QDialog::Accepted)
    {
      if (d.name ().isEmpty())
        name_->setText (element_->shortDescription ());
      else
        name_->setText (d.name ());

      if (name_->text () == element_->shortDescription ())
        elementName_->hide ();
      else
        elementName_->show ();

      nameLayout_->invalidate ();
    }
    return;
  }

  foreach (ElementInput *i, inputs_)
  {
    bool click = false;
    if (i->connectionPointItem ()->isVisible () &&
        i->connectionPointItem ()->shape ().contains (i->connectionPointItem ()->mapFromScene (_scenePos)))
      click = true;
    if (i->typeTextItem ()->isVisible () &&
        i->typeTextItem ()->shape ().contains (i->typeTextItem ()->mapFromScene (_scenePos)))
      click = true;
    if (i->descriptionTextItem ()->isVisible () &&
        i->descriptionTextItem ()->shape ().contains (i->descriptionTextItem ()->mapFromScene (_scenePos)))
      click = true;

    if (!click)
      continue;

    if (configInputs_.contains (i))
    {
      QVector<ElementInput *> inputs;
      inputs.append (i);

      ConfigDialog d (inputs, QString (), scene ()->views ()[0]);
      d.setWindowTitle (i->inOut ()->shortDescription () + " Configuration");
      d.exec ();
    } else
    {
      QMessageBox msgBox (scene ()->views ()[0]);
      msgBox.setText("No configuration available.");
      msgBox.exec();
    }
  }

}

//------------------------------------------------------------------------------

// update connection on move
void SceneElement::moveEvent (QGraphicsSceneMoveEvent *_event)
{
  QGraphicsWidget::moveEvent (_event);

  foreach (ElementInput *e, inputs_)
    foreach (Connection *c, e->connections ())
      c->updatePositions ();

  foreach (ElementOutput *e, outputs_)
    foreach (Connection *c, e->connections ())
      c->updatePositions ();

  if (dataIn_)
    foreach (Connection *c, dataIn_->connections ())
      c->updatePositions ();

  if (dataOut_)
    foreach (Connection *c, dataOut_->connections ())
      c->updatePositions ();

  scene_->contentChange ();
}

//------------------------------------------------------------------------------

/// Reset code block for code generation
void SceneElement::resetCodeGeneration()
{
  foreach (ElementInput *i, inputs ())
    i->setValid (false);
  if (dataIn ())
    dataIn ()->setValid (false);

  code_ = element_->code ();
}

//------------------------------------------------------------------------------

/// Replace block with name _name and id _id with _value
void SceneElement::replaceCodeBlock(QString _name, QString _id, QString _value)
{
  QString regex = "\\[\\s*" + _name + "\\s*=\\s*\"" + _id + "\"\\s*\\]";
  code_ = code_.replace (QRegExp (regex), _value);
}

//------------------------------------------------------------------------------

/// Unique variable name for code generation
QString SceneElement::variableId()
{
  return element_->name () + "_" + QString::number (id_);
}

//------------------------------------------------------------------------------

// show input configuration dialog
void SceneElement::showInputConfig()
{
  ConfigDialog d (configInputs_, name_->text (), scene ()->views ()[0]);
  d.setWindowTitle (name_->text () + " Input Configuration");
  if (d.exec () == QDialog::Accepted)
  {
    if (d.name ().isEmpty())
      name_->setText (element_->shortDescription ());
    else
      name_->setText (d.name ());

    if (name_->text () == element_->shortDescription ())
      elementName_->hide ();
    else
      elementName_->show ();

    nameLayout_->invalidate ();
  }
}

//------------------------------------------------------------------------------

/// Will this element be executed before _e bacause of its connections?
bool SceneElement::isBefore(SceneElement * _e)
{
  foreach (ElementOutput *o, outputs_)
  {
    foreach (Connection *c, o->connections ())
    {
      if (c->input ()->element () == _e)
        return true;
      bool rv = c->input ()->element ()->isBefore (_e);
      if (rv)
        return true;
    }
  }

  if (dataOut ())
    foreach (Connection *c, dataOut ()->connections ())
    {
      if (c->input ()->element () == _e)
        return true;
      bool rv = c->input ()->element ()->isBefore (_e);
      if (rv)
        return true;
    }
  return false; 
}

//------------------------------------------------------------------------------

/// Will this element be executed after _e bacause of its connections?
bool SceneElement::isAfter(SceneElement * _e)
{
  foreach (ElementInput *i, inputs_)
  {
    foreach (Connection *c, i->connections ())
    {
      if (c->output ()->element () == _e)
        return true;
      bool rv = c->output ()->element ()->isAfter (_e);
      if (rv)
        return true;
    }
  }

  if (dataIn ())
    foreach (Connection *c, dataIn ()->connections ())
    {
      if (c->output ()->element () == _e)
        return true;
      bool rv = c->output ()->element ()->isAfter (_e);
      if (rv)
        return true;
    }
  return false;
}

//------------------------------------------------------------------------------

// invalidate (reset) all connections of this element
void VSI::SceneElement::invalidateConnections()
{
  
  foreach (ElementInput *e, inputs_)
    foreach (Connection *c, e->connections ())
      c->invalidate ();

  foreach (ElementOutput *e, outputs_)
    foreach (Connection *c, e->connections ())
      c->invalidate ();

  if (dataIn_)
    foreach (Connection *c, dataIn_->connections ())
      c->invalidate ();

  if (dataOut_)
    foreach (Connection *c, dataOut_->connections ())
      c->invalidate ();
}

//------------------------------------------------------------------------------

/// Save to xml
void SceneElement::saveToXml(QDomDocument & _doc, QDomElement & _root)
{
  QDomText t;

  QDomElement main = _doc.createElement("element");
  _root.appendChild(main);
  main.setAttribute ("name",element_->name ());

  if (name_->text () != element_->shortDescription ())
  {
    QDomElement name = _doc.createElement("visible_name");
    main.appendChild(name);
    t = _doc.createTextNode(name_->text ());
    name.appendChild(t);
  }

  QDomElement id = _doc.createElement("id");
  main.appendChild(id);
  t = _doc.createTextNode(QString::number (id_));
  id.appendChild(t);

  QDomElement px = _doc.createElement("x");
  main.appendChild(px);
  t = _doc.createTextNode(QString::number (pos ().x ()));
  px.appendChild(t);

  QDomElement py = _doc.createElement("y");
  main.appendChild(py);
  t = _doc.createTextNode(QString::number (pos ().y ()));
  py.appendChild(t);

  if (!inputs_.isEmpty() || dataIn_)
  {
    QDomElement inputs = _doc.createElement("inputs");
    main.appendChild(inputs);
    foreach (ElementInput *i, inputs_)
      i->saveToXml (_doc, inputs);
    if (dataIn_)
      dataIn_->saveToXml (_doc, inputs);
  }
  if (!functions_.isEmpty ())
  {
    QDomElement functions = _doc.createElement("functions");
    main.appendChild(functions);
    foreach (ElementFunction *ef, functions_)
      ef->saveToXml (_doc, functions);
  }
}

//------------------------------------------------------------------------------

/// Load from xml
void SceneElement::loadFromXml(QXmlQuery & _xml)
{
  bool ok1, ok2;
  int id;
  double x, y;
  QString val;

  val = Context::getXmlString (_xml, "visible_name/string(text())");

  if (!val.isEmpty ())
    name_->setText (val);

  if (name_->text () == element_->shortDescription ())
    elementName_->hide ();
  else
    elementName_->show ();

  nameLayout_->invalidate ();

  val = Context::getXmlString (_xml, "id/string(text())");

  id = val.toUInt (&ok1);

  if (ok1)
  {
    id_ = id;
    element_->setMinId(id + 1);
  }

  val = Context::getXmlString (_xml, "x/string(text())");
  x = val.toDouble (&ok1);

  val = Context::getXmlString (_xml, "y/string(text())");
  y = val.toDouble (&ok2);

  if (ok1 && ok2)
    setPos (x, y);

  val = Context::getXmlString (_xml, "is_set/string(text())");

  _xml.setQuery ("inputs/input");

  QXmlResultItems i;

  if (_xml.isValid ())
  {
    _xml.evaluateTo (&i);
    QXmlItem item (i.next ());
    while (!item.isNull ())
    {
      QXmlQuery q (_xml);
      q.setFocus (item);

      val = Context::getXmlString (q, "string(@name)");

      if (!val.isEmpty())
      {
        if (val == "data" && dataIn_)
          dataIn_->loadFromXml (q);
        else
        {
          foreach (ElementInput *i, inputs_)
            if (i->inOut()->name () == val)
            {
              i->loadFromXml (q);
              break;
            }
        }
      }

      item = i.next ();
    }

  }

  _xml.setQuery ("functions/function");

  if (_xml.isValid ())
  {
    _xml.evaluateTo (&i);
    QXmlItem item (i.next ());
    while (!item.isNull ())
    {
      QXmlQuery q (_xml);
      q.setFocus (item);

      val = Context::getXmlString (q, "string(@name)");

      if (!val.isEmpty())
      {
        foreach (ElementFunction *ef, functions_)
          if (ef->function ()->name () == val)
          {
            ef->loadFromXml (q);
            break;
          }
      }

      item = i.next ();
    }

  }
}

//------------------------------------------------------------------------------
}



