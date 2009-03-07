#include <QtGui>

class FunctionList : public QListWidget
{
  Q_OBJECT

  signals:
  void getDescription(QString      _function,   QString&     _description,
                      QStringList& _parameters, QStringList& _descriptions);

  private:
    QPoint dragStartPosition_;
    QString currentFn_;

  protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

  public:
    FunctionList(QWidget *parent = 0);

    void filter(QString _filter);
    void reset();
};

