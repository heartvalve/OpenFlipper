//=============================================================================
//
//  CLASS Type BSpline Surface Plugin
//
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//  $Date$
//
//=============================================================================


#ifndef TYPEBSPLINESURFACEPLUGIN_HH
#define TYPEBSPLINESURFACEPLUGIN_HH

#include <QObject>


#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>

#include <ObjectTypes/BSplineSurface/BSplineSurface.hh>

class TypeBSplineSurfacePlugin : public QObject, BaseInterface, TypeInterface, LoggingInterface, LoadSaveInterface
{
   Q_OBJECT
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(TypeInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(LoadSaveInterface)

  signals:

    void log(Logtype _type, QString _message);
    void log(QString _message);
    
    // LoadSave Interface
    void emptyObjectAdded( int _id );

  private slots:

    void noguiSupported( ) {} ;

  public :

     ~TypeBSplineSurfacePlugin() {};
     TypeBSplineSurfacePlugin();

     QString name() { return (QString("TypeBSplineSurface")); };
     QString description( ) { return (QString(tr("Register BSpline Surface type "))); };
     
     bool registerType();

     DataType supportedType();
     
     int addEmpty();
     
  public slots:

    QString version() { return QString("1.0"); };

};

#endif //TYPEBSPLINESURFACEPLUGIN_HH
