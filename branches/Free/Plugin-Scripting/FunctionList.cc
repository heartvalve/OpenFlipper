#include "FunctionList.hh"
#include <iostream>

FunctionList::FunctionList(QWidget *parent)
    : QListWidget(parent)
{

}

void FunctionList::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton){
    dragStartPosition_ = event->pos();
    QListWidgetItem* item = itemAt(event->pos().x(), event->pos().y());
    if (item) 
      currentFn_ = item->text();
    else
      currentFn_ = "";
  }

  QListWidget::mousePressEvent(event);
}

void FunctionList::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - dragStartPosition_).manhattanLength()
          < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setText( currentFn_ );
    drag->setMimeData(mimeData);

    drag->exec(Qt::CopyAction);
}

void FunctionList::filter(QString _filter){

  for (int i=0; i < count(); i++){

    QString fnName = item(i)->text();
    QString slotDescription;
    QStringList params;
    QStringList descriptions;

    emit getDescription(fnName, slotDescription, params, descriptions);

    if ( fnName.contains(_filter) || slotDescription.contains(_filter) )
      item(i)->setHidden(false);
    else
      item(i)->setHidden(true);
  }
}

void FunctionList::reset(){
  for (int i=0; i < count(); i++)
    item(i)->setHidden(false);
}