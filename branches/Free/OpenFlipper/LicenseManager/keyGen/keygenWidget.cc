
#include <QtGui>
#include "keygenWidget.hh"
#include <iostream>

#include "salt.hh"

KeyGenWidget::KeyGenWidget(QMainWindow *parent)
    : QMainWindow(parent)
{
  setupUi(this);
  connect(generateButton,SIGNAL(clicked()),this,SLOT(slotGenerateButton()));
}

KeyGenWidget::~KeyGenWidget() {

}

void KeyGenWidget::slotGenerateButton() {

    QString inputData = requestData->toPlainText();
    QStringList data = inputData.split('\n',QString::SkipEmptyParts);

    if ( data.size() != 5) {
      QMessageBox::critical(this,"Wrong request data","The request has to contain 5 lines of data");
    } else {
      close();
    }

  
 
}
