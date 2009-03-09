 
#include <QtGui>

#include "PrintPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh> 

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

void PrintPlugin::pluginsInitialized()
{
  // color menu
  QMenu *printMenu = new QMenu(tr("&Printing"));
  
  QIcon icon;
   
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-print.png");
  printMenu->setIcon(icon);  
  
  emit addMenu( printMenu, FILEMENU );
  
  QAction* AC_Print = new QAction(tr("&Print"), this);;
  AC_Print->setStatusTip(tr("Print the current view"));
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-print.png");
  AC_Print->setIcon(icon);
  connect(AC_Print, SIGNAL(triggered()), this, SLOT(printView()));
  printMenu->addAction(AC_Print);
}

void PrintPlugin::printView () {
  
  
  QPrinter printer(QPrinter::HighResolution);
  
  QPrintDialog *dialog = new QPrintDialog(&printer);
  dialog->setWindowTitle("Print Current View");
  if (dialog->exec() != QDialog::Accepted)
      return;
  
  QImage image;
  
  // create the snapshot
  PluginFunctions::getCurrentViewImage(image);
  
  QPainter painter;
  painter.begin(&printer);

  painter.save();
  
  QRect rect = painter.viewport();
  QSize size = image.size();
  size.scale(rect.size(), Qt::KeepAspectRatio);
  painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
  painter.setWindow(image.rect());
         
  painter.drawImage(0, 0, image);

  painter.restore();
  painter.end();
  
}   

void PrintPlugin::slotKeyEvent( QKeyEvent* _event ) {
  if (_event->modifiers() == Qt::ControlModifier ) {
    switch (_event->key()) 
    {
      case Qt::Key_P :          
              printView();
              return;
    }
  }
}


Q_EXPORT_PLUGIN2( printplugin , PrintPlugin );

