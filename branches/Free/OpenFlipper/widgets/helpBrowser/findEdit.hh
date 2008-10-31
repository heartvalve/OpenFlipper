#ifndef FINDEDIT_HH
#define FINDEDIT_HH

#include <QLineEdit>

class findEdit : public QLineEdit
{
  Q_OBJECT

  signals:
    void find();
    void close();
    void textEdited();
  public:
    findEdit(QWidget *parent = 0);

  protected:
    void keyPressEvent( QKeyEvent* event );
  private slots:
    void slotTextEdited(const QString& /*text*/);
};


#endif //FINDEDIT_HH
