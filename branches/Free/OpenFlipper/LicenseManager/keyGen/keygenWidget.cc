
#include <QtGui>
#include "keygenWidget.hh"

MainWindow::MainWindow()
{

  textEdit_ = new QTextEdit(this);
  setCentralWidget(textEdit_);

}

MainWindow::~MainWindow() {

}
