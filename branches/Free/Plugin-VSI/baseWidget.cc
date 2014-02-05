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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialog>
#include <QTextEdit>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QIcon>
#include <QMessageBox>

#include <QApplication>
#include <QClipboard>
#include <QScriptEngine>

#include <QDomDocument>
#include <QDomElement>

#include <QXmlQuery>
#include <QXmlResultItems>
#include <QRegExp>

#include <OpenFlipper/common/GlobalOptions.hh>

#include "baseWidget.hh"
#include "toolBoxElement.hh"
#include "scene/sceneElement.hh"

#include "parser/context.hh"
#include "parser/element.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS BaseWidget - IMPLEMENTATION
//
//=============================================================================

// static variable for singleton
BaseWidget * BaseWidget::base_ = NULL;

//------------------------------------------------------------------------------

// Constructor
BaseWidget::BaseWidget (Context *_ctx, QWidget *_parent) :
  QMainWindow (_parent),
  ctx_ (_ctx),
  fileName_ (),
  changedContent_ (false)
{
  setWindowIcon (OpenFlipper::Options::OpenFlipperIcon ());
  updateTitle ();

  splitter_ = new QSplitter (Qt::Horizontal, this);
  toolbox_ = new QToolBox ();
  views_ = new QStackedWidget ();
 

  toolbox_->setMinimumWidth (150);

  QVBoxLayout *layout = new QVBoxLayout;

  layout->addWidget (toolbox_);

  QPushButton *execute = new QPushButton ("Execute");
  layout->addWidget (execute);

  QWidget *w = new QWidget;
  w->setLayout (layout);

  mainScene_ = new GraphicsScene (_ctx);
  views_->addWidget(mainScene_->graphicsView ());

  splitter_->addWidget (w);
  splitter_->addWidget (views_);
  QList<int> sizes;
  sizes << 150 << 10000;
  splitter_->setSizes (sizes);

  setCentralWidget (splitter_);

  resize (1000, 700);

  setupUi ();

  connect (execute, SIGNAL (clicked (bool)), this, SLOT (executeCode ()));

  connect (mainScene_, SIGNAL (contentChanged()), this, SLOT (contentChanged()));


  QMenu *menu = new QMenu (tr("&File"));
  QIcon icon;

  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"add-empty-object.png");
  menu->addAction (icon, tr("New"), this, SLOT (newFile()), QKeySequence::New);

  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-open.png");
  menu->addAction (icon, tr("Open ..."), this, SLOT (load()), QKeySequence::Open);

  menu->addSeparator ();

  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-save.png");
  menu->addAction (icon, tr("Save"), this, SLOT (save()), QKeySequence::Save);

  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-save-as.png");
  menu->addAction (icon, tr("Save as ..."), this, SLOT (saveAs()), QKeySequence::SaveAs);
  menu->addSeparator ();

  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"window-close.png");
  menu->addAction (icon, tr("Close"), this, SLOT (close()), QKeySequence::Close);

  menuBar()->addMenu (menu);

  menu = new QMenu (tr("&Script"));
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"scriptEditor.png");
  menu->addAction (icon, tr("Open in script editor"), this, SLOT (codeToScript()));

  menuBar()->addMenu (menu);

  base_ = this;
}

//------------------------------------------------------------------------------

/// Destructor
BaseWidget::~BaseWidget ()
{
  base_ = NULL;
}

//------------------------------------------------------------------------------

/// Adds element widgets to toolbox
void BaseWidget::setupUi ()
{

  foreach (const QString &c, ctx_->categories ())
  {
    QWidget *widget = new QWidget (toolbox_);
    QVBoxLayout *layout = new QVBoxLayout ();
    foreach (Element *e, ctx_->elements (c))
    {
      if (!(e->flags () & ELEMENT_FLAG_SKIP_TOOLBOX))
      {
        ToolBoxElement *t = new ToolBoxElement (e, widget);
        layout->addWidget (t);
      }
    }
    layout->addStretch ();
    widget->setLayout (layout);
    toolbox_->addItem (widget, c);
  }
}

//------------------------------------------------------------------------------

/// Executes current visual script
void BaseWidget::executeCode ()
{
  QString errors = "";

  QString code = mainScene_->generateCode (errors);

  if (errors.isEmpty ())
  {
    ctx_->scriptEngine ()->pushContext ();
    ctx_->scriptEngine ()->evaluate (code);
    ctx_->scriptEngine ()->popContext ();
    return;
  }

  errors = "<h3> " + tr("Following Elements could not be processed:") + "</h3>" + errors;

  QMessageBox::warning (this, tr("Error during code generation"), errors,
                        QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
}

//------------------------------------------------------------------------------

/// signal to open textual script editor with current script
void BaseWidget::codeToScript()
{
  
  QString errors = "";

  QString code = mainScene_->generateCode (errors);

  if (errors.isEmpty ())
  {
    emit codeToScriptEditor (code);
    return;
  }

  errors = "<qt> " + tr("Following Elements could not be processed:") + errors + "</qt>";

  QMessageBox::warning (this, tr("Error during code generation"), errors,
                        QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
}

//------------------------------------------------------------------------------

/// Save to file
bool BaseWidget::saveAs()
{
  return save (true);
}

//------------------------------------------------------------------------------

/// Load file
void VSI::BaseWidget::load()
{
  if (!saveIfChanged ())
    return;

  QString filename = QFileDialog::getOpenFileName (this,
                                                   tr("Load Visual Script"),
                                                   OpenFlipperSettings().value("Core/CurrentDir").toString(),
                                                   tr("Visual Script File (*.ofvs)"));

  if (filename.isEmpty ())
    return;

  // Get the chosen directory and remember it.
  QFileInfo fileInfo(filename);
  OpenFlipperSettings().setValue("Core/CurrentDir", fileInfo.absolutePath() );

  QFile f (filename);
  if (!f.open (QFile::ReadOnly))
  {
    QMessageBox msgBox;
    msgBox.setText(tr("Unable to open file."));
    msgBox.setInformativeText(filename);
    msgBox.exec();
    return;
  }

  while (!scenes_.isEmpty())
    scenes_.pop ();
  views_->setCurrentWidget (mainScene_->graphicsView());

  QXmlQuery query;
  query.setFocus (&f);

  query.setQuery ("VisualScript");

  QXmlResultItems root;

  if (query.isValid ())
  {
    query.evaluateTo (&root);

    QXmlItem item (root.next ());
    if (!item.isNull ())
    {
      QXmlQuery q (query);
      q.setFocus (item);
      mainScene_->loadFromXml (q);
    }
  }

  changedContent_ = false;
  fileName_ = filename;
  updateTitle ();
}

//------------------------------------------------------------------------------

/// Save file
bool BaseWidget::save(bool _newName)
{

  QString filename;

  if (fileName_.isEmpty () || _newName)
  {
    QFileDialog *d = new QFileDialog (this, tr("Save Visual Script"),
                                            OpenFlipperSettings().value("Core/CurrentDir").toString(),
                                            tr("Visual Script File (*.ofvs)"));

    d->setAcceptMode (QFileDialog::AcceptSave);
    d->setDefaultSuffix ("ofvs");

    if (QDialog::Accepted == d->exec ())
      filename = d->selectedFiles ()[0];
  }
  else
    filename = fileName_;

  if (filename.isEmpty ())
    return false;

  // Get the chosen directory and remember it.
  QFileInfo fileInfo(filename);
  OpenFlipperSettings().setValue("Core/CurrentDir", fileInfo.absolutePath() );

  QFile f (filename);
  if (!f.open (QFile::WriteOnly))
  {
    QMessageBox msgBox;
    msgBox.setText(tr("Unable to write file."));
    msgBox.setInformativeText(filename);
    msgBox.exec();
    return false;
  }

  QDomDocument doc("VisualScript");
  QDomElement root = doc.createElement("VisualScript");
  doc.appendChild(root);

  mainScene_->saveToXml (doc, root);

  f.write (doc.toString().toUtf8 ());
  f.close ();

  changedContent_ = false;
  fileName_ = filename;
  updateTitle ();

  return true;
}

//------------------------------------------------------------------------------

/// New empty scene
void BaseWidget::newFile()
{
  if (!saveIfChanged ())
    return;

  while (!scenes_.isEmpty())
    scenes_.pop ();
  views_->setCurrentWidget (mainScene_->graphicsView());

  fileName_ = QString ();
  mainScene_->clearScene ();
  changedContent_ = false;
  updateTitle ();
}

//------------------------------------------------------------------------------

/// Update window title
void BaseWidget::updateTitle()
{
  if (fileName_.isEmpty ())
    setWindowTitle (tr("Untitled") + (changedContent_ ? " [" + tr("Modified") + "] " : "") + " - Visual Script Editor");
  else
    setWindowTitle (fileName_ + (changedContent_ ? " [" + tr("Modified") + "] " : "") + " - Visual Script Editor");
}

//------------------------------------------------------------------------------

/// Called if something it the scene has changed
void BaseWidget::contentChanged()
{
  if (!changedContent_)
  {
    changedContent_ = true;
    updateTitle ();
  }
}

//------------------------------------------------------------------------------

/// Asks the user if he want to save his changes
bool BaseWidget::saveIfChanged()
{
  if (changedContent_)
  {
    QMessageBox msgBox;
    msgBox.setText(tr("The visual script has been modified."));
    msgBox.setInformativeText(tr("Do you want to save your changes?"));
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret) {
     case QMessageBox::Save:
      // Save was clicked
      if (!save ())
        return false;
      break;
     case QMessageBox::Discard:
      // Don't Save was clicked
      break;
     case QMessageBox::Cancel:
      // Cancel was clicked
      return false;
      break;
     default:
      // should never be reached
      break;
   }
   return true;
  }
  else
    return true;
}

//------------------------------------------------------------------------------

/// Chatch close event
void BaseWidget::closeEvent(QCloseEvent *_event)
{
  if (!saveIfChanged ())
    _event->ignore ();
  else
    _event->accept();
}

//------------------------------------------------------------------------------

/// Singleton constructor
BaseWidget * BaseWidget::createBaseWidget (Context *_ctx, QWidget *_parent)
{
  if (!base_)
    base_ = new BaseWidget (_ctx, _parent);
  return base_;
}

//------------------------------------------------------------------------------

/// Returns singleton
BaseWidget * BaseWidget::getBaseWidget()
{
  return base_;
}

//------------------------------------------------------------------------------

/// Display a new scene (function) in editor
void VSI::BaseWidget::pushScene(GraphicsScene * _scene)
{
  scenes_.push (_scene);
  views_->setCurrentWidget (_scene->graphicsView());
}

//------------------------------------------------------------------------------

/// Go back to previous scene (function)
void VSI::BaseWidget::popScene()
{
  if (!scenes_.isEmpty())
    scenes_.pop ();

  if (!scenes_.isEmpty())
    views_->setCurrentWidget (scenes_.top ()->graphicsView());
  else
    views_->setCurrentWidget (mainScene_->graphicsView());
}

//------------------------------------------------------------------------------

/// Add scene to stack widget
void VSI::BaseWidget::addScene(GraphicsScene * _scene)
{
  views_->addWidget (_scene->graphicsView ());
}

//------------------------------------------------------------------------------

/// Remove scene from stack widget
void VSI::BaseWidget::removeScene(GraphicsScene * _scene)
{
  views_->removeWidget (_scene->graphicsView ());
}

//------------------------------------------------------------------------------
}

