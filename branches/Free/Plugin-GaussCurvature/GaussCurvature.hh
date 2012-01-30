#ifndef GAUSSCURVATUREPLUGIN_HH
#define GAUSSCURVATUREPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/TextureInterface.hh>
#include <OpenFlipper/common/Types.hh>

class GaussCurvaturePlugin : public QObject, BaseInterface, TextureInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(TextureInterface)

  signals:

    void addTexture( QString _textureName , QString _filename , uint dimension );
    void updatedTextures( QString , int );
    void setTextureMode(QString _textureName ,QString _mode);

  private slots:
    void slotUpdateTexture( QString _textureName  , int _identifier);

    void pluginsInitialized();

    void exit();

  public :

    ~GaussCurvaturePlugin() {};

    template< typename MeshT >
    void computeGaussianCurvature( MeshT* _mesh);

    QString name() { return (QString("GaussCurvature")); };
    QString description( ) { return (QString("Generates Gauss Curvature information")); };

  public slots:
    QString version() { return QString("1.0"); };
};

#endif //GAUSSCURVATUREPLUGIN_HH
