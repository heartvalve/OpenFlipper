#include "helpWidget.hh"
#include <QtGui>

#include <OpenFlipper/common/GlobalOptions.hh>

#include <QtWebKit> 
#include <iostream>

HelpWidget::HelpWidget(QWidget *parent, bool _user)
    : QMainWindow(parent)
{
  setupUi(this);

  QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();

  //create toolbar
  QAction* action = toolBar->addAction(QIcon(iconPath + "arrow-left.png"), "Back");
  connect(action, SIGNAL(triggered()), webView, SLOT(back()));
  action = toolBar->addAction(QIcon(iconPath + "arrow-right.png"), "Forward");
  connect(action, SIGNAL(triggered()), webView, SLOT(forward()));
  action = toolBar->addAction(QIcon(iconPath + "go-home.png"), "Home");
  connect(action, SIGNAL(triggered()), this,    SLOT(goHome()));

  QIcon icon;
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"window-close.png");
  actionClose->setIcon(icon);
  
  baseDir_ = OpenFlipper::Options::applicationDirStr() + 
             OpenFlipper::Options::dirSeparator() + "Docs";
      
  if ( _user ) {
     baseDir_ += OpenFlipper::Options::dirSeparator() + "User" +
                 OpenFlipper::Options::dirSeparator();
     
     startFile_ = baseDir_ + "OpenFlipperUserDoc.html";
     
  } else {
     baseDir_ += OpenFlipper::Options::dirSeparator() + "Developer" +
                 OpenFlipper::Options::dirSeparator();
     
     startFile_ = baseDir_ + "OpenFlipperDeveloperDoc.html";
  }
  
  connect(webView, SIGNAL( loadFinished(bool) ), this, SLOT( loadFinished(bool) ));

  webView->load( QUrl(startFile_) );

  webView->history()->clear();

  connect( actionClose , SIGNAL(triggered() ) , this, SLOT(hide()) );
  connect( closeButton , SIGNAL(clicked()   ) , this, SLOT(hide()) );

  generateTree();
  tree->setHeaderHidden( true );

  connect( tree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeClicked(QTreeWidgetItem*,int)) );
}

void HelpWidget::loadFinished(bool ok)
{

  if (ok)
    tabWidget->setTabText(tabWidget->currentIndex(), webView->page()->mainFrame()->title());

// std::cerr << "history " << webView->history()->items().count() << std::endl;
// 
//   if ( webView->url().path() != QUrl(startFile_).path() || !ok )
//     return;
// 
//   QDir docDir = QDir(baseDir_);
//   QStringList plugins = docDir.entryList ( QDir::AllDirs | QDir::NoDotAndDotDot);
// 
//   QString pluginHtml;
//   
//   for ( int i = 0 ; i < plugins.size(); ++i ) 
//     if ( plugins[i].contains("Plugin-") )
//       pluginHtml.append( "<a href=\"" + plugins[i] +  OpenFlipper::Options::dirSeparator()
//                                           + "index.html\">" + plugins[i] + "</a><br>" );
//   
//   QString source =  webView->page()->currentFrame()->toHtml();
// 
//   source = source.replace("REPLACEWITHPLUGINLINKS",pluginHtml);
// 
//   webView->page()->currentFrame()->setHtml(source, QUrl(baseDir_));

}

/// go to home dir
void HelpWidget::goHome()
{
  webView->load( QUrl(startFile_) );
}

///generate the documentation tree
void HelpWidget::generateTree(){

  QDir docDir = QDir(baseDir_);


  QStringList dirs  = docDir.entryList(QDir::Readable | QDir::AllDirs | QDir::NoDotAndDotDot);

  QStringList names = dirs;

  QList< QTreeWidgetItem* > parents;

  //prepend current path to dirs
  for ( int j = 0 ; j < dirs.size(); ++j ){
    dirs[j] = baseDir_ + dirs[j] ;
    parents.append(0);
  }

  while ( dirs.size() > 0) {

    QString indexHTML(dirs.first() + OpenFlipper::Options::dirSeparator() + "index.html");

    QTreeWidgetItem* currentItem = 0;

    if ( QFile(indexHTML).exists() ){
      QString title = getTitle(indexHTML);

      if (title == "")
        std::cerr << "HelpWidget: Warning index.html without title found!" << std::endl;
      else{

        currentItem = new QTreeWidgetItem(QString(title + "," + names.first()).split(","));
        
        if (parents.first() == 0)
          tree->addTopLevelItem( currentItem );
        else
          parents.first()->addChild( currentItem );
      }
    }

    QDir currentDir = QDir( dirs.first() );

    QStringList newDirs  = currentDir.entryList(QDir::Readable | QDir::AllDirs | QDir::NoDotAndDotDot);

    names.removeFirst();
    names  << newDirs;

    //prepend current path to dirs
    for ( int j = 0 ; j < newDirs.size(); ++j ){
      newDirs[j] = dirs.first() + OpenFlipper::Options::dirSeparator() + newDirs[j] ;
      if (currentItem == 0)
        parents.append( parents.first() );
      else
        parents.append( currentItem );
    }

    parents.removeFirst();

    dirs.removeFirst();
    dirs  << newDirs;
  }

}

///read the title from an html file
QString HelpWidget::getTitle(QString htmlFile){
  QFile data( htmlFile );
  if (data.open(QFile::ReadOnly)) {
    QTextStream stream(&data);

    QString source = stream.readAll();

    int s = source.indexOf("<title>", Qt::CaseInsensitive);
    int e = source.indexOf("</title>", Qt::CaseInsensitive);

    if (s == -1 || e == -1)
      return QString("");

    return source.mid(s + 7, e-(s+7));
  }

  return QString("");
}

/// the user clicked on an item in the treeWidget
void HelpWidget::treeClicked(QTreeWidgetItem* item, int column ){
  QTreeWidgetItem* parent = item->parent();

  QString path = baseDir_;

  while(parent != 0){
    path += parent->text(1) + OpenFlipper::Options::dirSeparator();
    parent = parent->parent();
  }
    
  webView->load( QUrl(path + item->text(1) + OpenFlipper::Options::dirSeparator() + "index.html") );
} 