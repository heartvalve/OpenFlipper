

#ifndef KEYGENWINDOW_HH
#define KEYGENWINDOW_HH

#include <ui_keygen.hh>


class KeyGenWidget : public QMainWindow, public Ui::keyWindow
{
  Q_OBJECT

public:
  KeyGenWidget(QMainWindow *parent);
  virtual ~KeyGenWidget();

public slots:
  void slotGenerateButton();

};

 #endif // KEYGENWINDOW_HH