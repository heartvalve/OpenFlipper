

#ifndef KEYGENWINDOW_HH
#define KEYGENWINDOW_HH

#include <QMainWindow>
#include <QTextEdit> 


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();

    QTextEdit* textEdit_;

};

 #endif // KEYGENWINDOW_HH