#include "ui_pluginDialog.hh"
#include <QtGui>

#include <OpenFlipper/Core/PluginInfo.hh>

class PluginDialog : public QDialog, public Ui::PluginDialog
{
  Q_OBJECT

  public:
    PluginDialog(std::vector<PluginInfo>& _plugins, QWidget *parent = 0);

    int exec();

  private:
    std::vector<PluginInfo>& plugins_;
    QVector< QFrame* > frames_;

  private slots:
    void slotUnload();

  protected:
    void closeEvent(QCloseEvent *event);

  signals:
    void unloadPlugin(QString _name);
    void dontLoadPlugins(QStringList _dontLoad);
    void loadPlugin();
};

