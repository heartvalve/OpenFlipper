#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/TextureInterface.hh>
#include <ACG/QtWidgets/QtExaminerViewer.hh>
#include <OpenFlipper/common/Types.hh>

class LaplaceLengthPlugin : public QObject, BaseInterface, TextureInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(TextureInterface)

  signals:

    void addTexture( QString _textureName , QString _filename , uint dimension );
    void updatedTextures( QString , int );
    void setTextureMode(QString _textureName ,QString _mode);

  private slots:
    void slotUpdateTexture( QString _textureName , int _identifier );

    void pluginsInitialized();

  public :

    ~LaplaceLengthPlugin() {};

    template< typename MeshT >
    void computeLaplaceLength(MeshT* _mesh);

    template< typename MeshT >
    void computeLaplaceSquaredLength(MeshT* _mesh);

    QString name() { return (QString("LaplaceLengthPlugin")); };
    QString description( ) { return (QString("Computes Laplace Vector length information")); };

  public slots:
    QString version() { return QString("1.0"); };
};
