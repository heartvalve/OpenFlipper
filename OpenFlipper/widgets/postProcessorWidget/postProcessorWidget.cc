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
*   $Revision: 15910 $                                                       *
*   $LastChangedBy: moeller $                                                *
*   $Date: 2012-12-05 12:53:39 +0100 (Mi, 05 Dez 2012) $                     *
*                                                                            *
\*===========================================================================*/

#if QT_VERSION >= 0x050000
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include "postProcessorWidget.hh"
#include <functional>
#include <algorithm>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/RendererInfo.hh>
#include <OpenFlipper/common/GlobalOptions.hh>


PostProcessorDialog::PostProcessorDialog(QWidget *_parent)
    : QDialog(_parent)
{
  setupUi(this);

  list->setContextMenuPolicy(Qt::CustomContextMenu);
  activeList->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(list,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(slotContextMenuActivate(const QPoint&)));
  connect(activeList,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(slotContextMenuDeactivate(const QPoint&)));
  connect(activateButton,SIGNAL(clicked()),this,SLOT(slotActivatePostProcessor()));
  connect(deactivateButton,SIGNAL(clicked()),this,SLOT(slotDeactivatePostProcessor()));
  connect(upButton,SIGNAL(clicked()),this,SLOT(slotMoveUp()));
  connect(downButton,SIGNAL(clicked()),this,SLOT(slotMoveDown()));
  connect(saveButton,SIGNAL(clicked()),this,SLOT(slotSaveActive()));
  connect(refreshButton,SIGNAL(clicked()), this,SLOT(refresh()));

  //set icons
  QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

  closeButton->setIcon( QIcon(iconPath + "window-close.png"));
  saveButton->setIcon( QIcon(iconPath + "document-save.png"));
  refreshButton->setIcon( QIcon(iconPath + "edit-redo.png"));

}

void PostProcessorDialog::closeEvent(QCloseEvent *_event)
{
  _event->accept();
  accept();
}

void PostProcessorDialog::showEvent ( QShowEvent * )
{
  initWindow();
}

void PostProcessorDialog::initWindow()
{
  currentExaminer_ = PluginFunctions::activeExaminer();
  list->clear();
  activeList->clear();
  activeRowToRow_.clear();

  //first, fill already activated processors in the right order
  for (int i = 0; i < postProcessorManager().numActive(currentExaminer_); ++i)
  {
    unsigned int id = postProcessorManager().activeId(currentExaminer_, i);
    activeRowToRow_.push_back(id);

    QListWidgetItem *activeItem = new QListWidgetItem("");
    activeList->addItem(activeItem);
    QFrame* frame = createFrame(*postProcessorManager()[id]);
    activeItem->setSizeHint(frame->sizeHint());
    activeList->setItemWidget(activeItem,frame);
  }

  //list all available post processors (hidden, if active)
  for ( unsigned int i = 0 ; i < postProcessorManager().available() ; ++i)
  {

    // Get and check post processor
    PostProcessorInfo* processor = postProcessorManager()[i];
    if ( ! processor )
      continue;

    QFrame* frame = createFrame(*processor);

    QListWidgetItem *item = new QListWidgetItem("");
    item->setSizeHint( frame->sizeHint() );

    list->addItem(item);

    list->setItemWidget(item, frame);

    //is the postProcess active? if so, hide it
    bool found = false;
    for (std::vector<unsigned>::iterator iter = activeRowToRow_.begin(); iter != activeRowToRow_.end() && !found; ++iter)
      found = (*iter == i);
    if ( found )
      list->setRowHidden(list->row(item),true);
  }

}

void PostProcessorDialog::slotActivatePostProcessor()
{
  QList<QListWidgetItem*> selectedItems = list->selectedItems();

  for (int i=0; i < selectedItems.size(); ++i)
  {
    QListWidgetItem* item = selectedItems[i];
    const int currentRow = list->row( item );

    postProcessorManager().append( currentRow, currentExaminer_);

    //disable in aviable list
    item->setHidden(true);
    item->setSelected(false);

    //add to active list
    QListWidgetItem *activeItem = new QListWidgetItem("");
    activeList->addItem(activeItem);
    activeItem->setSelected(true);

    QFrame* frame = createFrame(*postProcessorManager()[currentRow]);
    activeItem->setSizeHint( frame->sizeHint() );
    activeList->setItemWidget(activeItem,frame);
    activeRowToRow_.push_back(currentRow);
  }

  emit updateExaminer(currentExaminer_);

}

void PostProcessorDialog::slotDeactivatePostProcessor()
{
  QList<QListWidgetItem*> selectedItems = activeList->selectedItems();

  for (int i=0; i < selectedItems.size(); ++i)
  {
    QListWidgetItem* activeItem = selectedItems[i];

    const unsigned chainPos = activeList->row(activeItem);
    const unsigned activeID = activeRowToRow_[chainPos];
    QListWidgetItem* item = list->item(activeID);

    //remove postprocessor
    postProcessorManager().remove(currentExaminer_, chainPos);

    //enable in aviable list
    item->setHidden(false);
    item->setSelected(true);

    //remove from active list
    //update active row ids
    for (unsigned i = chainPos; i < activeRowToRow_.size()-1; ++i)
      activeRowToRow_[i] = activeRowToRow_[i+1];

    //from qt doc: Items removed from a list widget will not be managed by Qt, and will need to be deleted manually.
    activeItem = activeList->takeItem(activeList->row(activeItem));
    delete activeItem;
  }
  activeRowToRow_.erase( activeRowToRow_.end()-selectedItems.size(), activeRowToRow_.end());

  emit updateExaminer(currentExaminer_);
}

void PostProcessorDialog::slotMovePostProcessor(unsigned _from,unsigned _to)
{

  if (_from >= static_cast<unsigned>(activeList->count()))
    return;

  if (_to >= static_cast<unsigned>(activeList->count()))
    _to = activeList->count()-1;

  if (_from == _to)
    return;

  //swap widget
  QListWidgetItem* activeItem = activeList->takeItem(_from);
  activeList->insertItem(_to,activeItem);
  QFrame* frame = createFrame(*postProcessorManager()[activeRowToRow_[_from]]);
  activeItem->setSizeHint(frame->sizeHint());
  activeList->setItemWidget(activeItem,frame);
  activeList->setItemSelected(activeItem,true);

  //swap postprocessor
  const int chainPos = _from;
  const int activeID = activeRowToRow_[_from];
  postProcessorManager().remove(currentExaminer_, chainPos);
  postProcessorManager().insert(activeID,_to,currentExaminer_);

  //swap active ID to current chain position map
  int inc = (_from > _to)? -1: +1;
  for(unsigned int currentRow = _from;currentRow != _to; currentRow += inc)
    std::swap(activeRowToRow_[currentRow+inc],activeRowToRow_[currentRow]);

  emit updateExaminer(currentExaminer_);
}


void PostProcessorDialog::slotContextMenuActivate(const QPoint& _point)
{
  if (!list->count())
    return;

  QMenu *menu = new QMenu(list);
  QAction* action = 0;

  action = menu->addAction(tr("Activate"));
  connect(action,SIGNAL(triggered(bool)),this,SLOT(slotActivatePostProcessor()));

  menu->exec(list->mapToGlobal(_point),0);

}

void PostProcessorDialog::slotContextMenuDeactivate(const QPoint& _point)
{
  if (!activeList->count())
    return;

  QMenu *menu = new QMenu(activeList);
  QAction* action = 0;

  action = menu->addAction(tr("Up"));
  connect(action,SIGNAL(triggered(bool)),this,SLOT(slotMoveUp()));
  action = menu->addAction(tr("Down"));
  connect(action,SIGNAL(triggered(bool)),this,SLOT(slotMoveDown()));
  action = menu->addAction(tr("Deactivate"));
  connect(action,SIGNAL(triggered(bool)),this,SLOT(slotDeactivatePostProcessor()));

  menu->exec(activeList->mapToGlobal(_point),0);

}

QFrame* PostProcessorDialog::createFrame(const PostProcessorInfo& _pPI)
{
  QFrame* frame = new QFrame();
  QHBoxLayout* hlayout = new QHBoxLayout;

  QLabel* name = new QLabel( _pPI.name );
  QFont font;
  font.setBold(true);
  font.setPointSize(10);
  name->setFont(font);
  QLabel* version = new QLabel( _pPI.version );
  hlayout->addWidget(name);
  hlayout->addStretch();
  hlayout->addWidget(version);

  QVBoxLayout* vlayout = new QVBoxLayout;

  QLabel* description = new QLabel( _pPI.description );

  vlayout->addLayout(hlayout,20);
  vlayout->addWidget(description);
  frame->setLayout(vlayout);
  frame->adjustSize();

  return frame;
}



template<typename TCmp>
class QListWidgetRowCmp
{
  QListWidget* list_;
public:
  QListWidgetRowCmp(QListWidget* _list):list_(_list){}
  bool operator()(QListWidgetItem* left, QListWidgetItem* right)
  {
    return TCmp()(list_->row(left) , list_->row(right));
  }
};

void PostProcessorDialog::slotMoveDown()
{
  int start = 0;
  QList<QListWidgetItem*> selectedItems = activeList->selectedItems();

  //sort list, so the top is the last element
  std::sort(selectedItems.begin(), selectedItems.end(), QListWidgetRowCmp<std::greater<int> >(activeList));

  //dont move the last one
  //if the last one wasnt moved, dont move the direct followers
  for(int i=0; i < selectedItems.size() && activeList->row(selectedItems[i]) == activeList->count()-1-i;++i)
    --start;

  //move bottom first
  for (int i=selectedItems.size()-1+start; i >= 0 ; --i)
  {
    QListWidgetItem* activeItem = activeList->selectedItems()[i];
    unsigned selectedRow = activeList->row(activeItem);
    slotMovePostProcessor(selectedRow,selectedRow+1);
  }
}


void PostProcessorDialog::slotMoveUp()
{
  int start = 0;
  QList<QListWidgetItem*> selectedItems = activeList->selectedItems();

  //sort list, so the top is the first element
  std::sort(selectedItems.begin(), selectedItems.end(), QListWidgetRowCmp<std::less<int> >(activeList));

  //dont move the first one
  //if the first one wasnt moved, dont move the direct followers
  for(int i=0; i < selectedItems.size() && activeList->row(selectedItems[i]) == i;++i)
    ++start;

  //move top first
  for (int i=start; i < selectedItems.size(); ++i)
  {
    QListWidgetItem* activeItem = selectedItems[i];
    unsigned selectedRow = activeList->row(activeItem);
    slotMovePostProcessor(selectedRow,selectedRow-1);
  }
}

void PostProcessorDialog::slotSaveActive()
{
  QStringList activeList("");

  for (int i = 0; i < postProcessorManager().numActive(currentExaminer_); ++i)
  {
    unsigned int id = postProcessorManager().activeId(currentExaminer_, i);
    activeList.push_back(postProcessorManager()[id]->name);
  }

  OpenFlipperSettings().setValue(QString("PostProcessor/Viewer/%1").arg(currentExaminer_),activeList);
}

QStringList PostProcessorDialog::getSavedPostProcessorNames(const unsigned _examiner)
{
  return OpenFlipperSettings().value(QString("PostProcessor/Viewer/%1").arg(_examiner),QStringList("")).toStringList();
}

void PostProcessorDialog::loadSavedPostProcessors(const unsigned _examiner)
{
  QStringList active = getSavedPostProcessorNames(_examiner);
  for (QStringList::iterator iter = active.begin(); iter != active.end(); ++iter)
  {
    postProcessorManager().append(*iter,_examiner);
  }
}

void PostProcessorDialog::refresh()
{
  initWindow();
}
