#include "findEdit.hh"

#include <QKeyEvent>

findEdit::findEdit(QWidget* parent) : QLineEdit(parent)
{
  connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(slotTextEdited(const QString&)) );
}

void findEdit::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    emit find();
  else if (event->key() == Qt::Key_F3)
    emit find();
  else if (event->key() == Qt::Key_Escape)
    emit close();
  else
    QLineEdit::keyPressEvent(event);
}

void findEdit::slotTextEdited(const QString&)
{
  emit textEdited();
}

