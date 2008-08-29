#include "helpWidget.hh"
#include <QtGui>

#include <OpenFlipper/common/GlobalOptions.hh>

HelpWidget::HelpWidget(QWidget *parent, bool _user)
    : QMainWindow(parent)
{
  setupUi(this);
    
  QIcon icon;
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"window-close.png");
  actionClose->setIcon(icon);
  
  QString baseDir = OpenFlipper::Options::applicationDirStr() + 
                    OpenFlipper::Options::dirSeparator() + "Docs";
  
  QString startFile;
      
  if ( _user ) {
     baseDir += OpenFlipper::Options::dirSeparator() + "User" +
                OpenFlipper::Options::dirSeparator();
     
     startFile = baseDir + "OpenFlipperUserDoc.html";
     
  } else {
     baseDir += OpenFlipper::Options::dirSeparator() + "Developer" +
                OpenFlipper::Options::dirSeparator();
     
     startFile = baseDir + "OpenFlipperDeveloperDoc.html";
  }
  
  textBrowser->setSource( QUrl(startFile) );
  
  textBrowser->find( "REPLACEWITHPLUGINLINKS", QTextDocument::FindCaseSensitively );
  QTextCursor textCursor = textBrowser->textCursor();
  
  QDir docDir = QDir(baseDir);
  QStringList plugins = docDir.entryList ( QDir::AllDirs | QDir::NoDotAndDotDot);
  
  for ( int i = 0 ; i < plugins.size(); ++i ) 
    if ( plugins[i].contains("Plugin-") )
      textCursor.insertHtml( "<a href=\"" + plugins[i] +  OpenFlipper::Options::dirSeparator()
                                          + "index.html\">" + plugins[i] + "</a><br>" );
  
  connect( actionClose , SIGNAL(triggered() ) , this, SLOT(hide()) );
  connect( closeButton , SIGNAL(clicked()   ) , this, SLOT(hide()) );
}
