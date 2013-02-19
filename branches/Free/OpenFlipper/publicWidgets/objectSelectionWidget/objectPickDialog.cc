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
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTreeView>
#include <QMouseEvent>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/widgets/glWidget/simpleViewer.hh>
#include <OpenFlipper/widgets/glWidget/QtBaseViewer.hh>

#include "objectPickDialog.hh"
#include "TreeModelObjectSelection.hh"
#include "TreeItemObjectSelection.hh"

//== NAMESPACES ===============================================================

//=============================================================================
//
//  CLASS ObjectPickDialog - IMPLEMENTATION
//
//=============================================================================

/// Constructor
ObjectPickDialog::ObjectPickDialog(QStringList _flags, QStringList _types, bool _withGroups) :
  QDialog (),
  selectedId_(0)
{
  QHBoxLayout *hL = new QHBoxLayout;
  QHBoxLayout *bL = new QHBoxLayout;
  QVBoxLayout *vL = new QVBoxLayout;

  model_ = new TreeModelObjectSelection ();

  treeView_ = new QTreeView;
  treeView_->setModel (model_);
  treeView_->resizeColumnToContents (0);
  treeView_->setSelectionMode (QAbstractItemView::SingleSelection);
  treeView_->setSelectionBehavior (QAbstractItemView::SelectRows);

  viewer_ = new SimpleViewer ();
  viewer_->properties ().objectMarker (&marker_);

  okButton_ = new QPushButton (tr("OK"));
  cancelButton_ = new QPushButton (tr("Cancel"));

  connect (okButton_, SIGNAL (pressed()), this, SLOT (accept()));
  connect (cancelButton_, SIGNAL (pressed()), this, SLOT (reject()));

  hL->addWidget (viewer_);
  hL->setStretchFactor (viewer_, 1);
  hL->addWidget (treeView_);

  bL->addStretch (1);
  bL->addWidget (okButton_);
  bL->addWidget (cancelButton_);

  vL->addLayout(hL);
  vL->addLayout(bL);

  setLayout (vL);

  resize (700, 400);

  setWindowTitle(tr("Click on object or select from list..."));

  connect (treeView_, SIGNAL (activated( const QModelIndex& )),
           this, SLOT (activated(const QModelIndex&)));
  connect (viewer_->viewer(), SIGNAL (signalMouseEventClick(QMouseEvent*, bool)),
           this, SLOT (slotMouseEventClick(QMouseEvent*, bool)));

  for (PluginFunctions::BaseObjectIterator o_it; o_it != PluginFunctions::baseObjectsEnd(); ++o_it)  {

    bool ok = true;

    if (!_flags.empty ())
    {
      bool found = false;
      foreach (QString flag, _flags)
        if (o_it->flag (flag))
        {
          found = true;
          break;
        }

      if (!found)
        ok = false;
    }

    if (!_types.empty ())
    {
      if (!_types.contains (typeName (o_it->dataType())))
        ok = false;
    }

    if (o_it->isGroup() && !_withGroups)
      continue;

    if (ok)
    {
      if (!_withGroups)
        model_->objectAdded(o_it, PluginFunctions::objectRoot());
      else
        model_->objectAdded (o_it);
    }
  }
}

//------------------------------------------------------------------------------

/// Destructor
ObjectPickDialog::~ ObjectPickDialog()
{
  for (PluginFunctions::BaseObjectIterator o_it; o_it != PluginFunctions::baseObjectsEnd(); ++o_it)  {
    o_it->setFlag("vsi_objectId_selected", false);
  }

  delete model_;
}

//------------------------------------------------------------------------------

void ObjectPickDialog::activated(const QModelIndex & _index)
{
  if (_index.isValid()) {

    TreeItemObjectSelection *item = static_cast<TreeItemObjectSelection*>(_index.internalPointer());
    if (item)
    {
      selectedId (item->id());
    }
  }
}

//------------------------------------------------------------------------------

void ObjectPickDialog::slotMouseEventClick(QMouseEvent * _event, bool /*_double*/)
{
  unsigned int nodeIdx, targetIdx;


  if (viewer_->viewer()->pick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), nodeIdx, targetIdx))
  {
    BaseObjectData *obj = 0;
    if (PluginFunctions::getPickedObject (nodeIdx, obj))
    {
      if (!obj->flag ("vsi_objectId_disabled"))
      {
        selectedId (obj->id());
      }
    }
  }
}

//------------------------------------------------------------------------------

unsigned int ObjectPickDialog::selectedId()
{
  return selectedId_;
}

//------------------------------------------------------------------------------

void ObjectPickDialog::selectedId(unsigned int _id)
{
  BaseObject* obj = 0;

  if (PluginFunctions::getObject(_id, obj))
  {
    BaseObject* obj2 = 0;

    if (selectedId_ != _id && PluginFunctions::getObject(selectedId_, obj2))
    {
      obj2->setFlag ("vsi_objectId_selected", false);
      if (obj2->isGroup())
        setForGroup (obj2, "vsi_objectId_selected", false);
    }
    obj->setFlag ("vsi_objectId_selected", true);
    if (obj->isGroup())
      setForGroup (obj, "vsi_objectId_selected", true);

    selectedId_ = _id;
    viewer_->viewer()->updateGL ();
    treeView_->setCurrentIndex (model_->getModelIndex(_id, 0));
  }
}

//------------------------------------------------------------------------------

void ObjectPickDialog::setForGroup(BaseObject *_obj, QString _flag, bool _enabled)
{
  for (PluginFunctions::BaseObjectIterator o_it; o_it != PluginFunctions::baseObjectsEnd(); ++o_it)  {
    if (o_it->id () == _obj->id ())
      continue;
    if (o_it->isInGroup (_obj->id ()))
    {
      o_it->setFlag(_flag, _enabled);
      if (o_it->isGroup())
        setForGroup (o_it, _flag, _enabled);
    }
  }
}

//------------------------------------------------------------------------------



