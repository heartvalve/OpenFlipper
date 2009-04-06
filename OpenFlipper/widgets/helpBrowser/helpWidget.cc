#include "helpWidget.hh"
#include <QtGui>

#include <OpenFlipper/common/GlobalOptions.hh>

#include <iostream>

HelpWidget::HelpWidget(QWidget *parent, bool _user)
    : QMainWindow(parent)
{
  setupUi(this);

  QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();

  //create toolbar
  back_ = toolBar->addAction(QIcon(iconPath + "arrow-left.png"), "Back");
  forward_ = toolBar->addAction(QIcon(iconPath + "arrow-right.png"), "Forward");
  QAction* action = toolBar->addAction(QIcon(iconPath + "go-home.png"), "Home");
  connect(action, SIGNAL(triggered()), this,    SLOT(goHome()));
  toolBar->addSeparator();
  action = toolBar->addAction(QIcon(iconPath + "edit-find.png"), "Find text");
  connect(action, SIGNAL(triggered()), this,    SLOT(showFind()));

  //finding
  initFindFrame();

  //splitter position
  QList<int> wsizes( splitter->sizes() );
  wsizes[0] = 30;
  wsizes[1] = 70;
  splitter->setSizes(wsizes);

  //tab buttons
  QPushButton* closeTabButton = new QPushButton();
  QPushButton* addTabButton = new QPushButton();
  closeTabButton->setIcon(QIcon(iconPath+"tab-close.png"));
  addTabButton->setIcon(QIcon(iconPath+"tab-new.png"));
  addTabButton->setMaximumSize(26,26);

  tabs->setCornerWidget(closeTabButton, Qt::TopRightCorner);
  tabs->setCornerWidget(addTabButton, Qt::TopLeftCorner);
  connect( closeTabButton, SIGNAL(clicked()), this, SLOT(closeCurrentTab()) );
  connect( addTabButton, SIGNAL(clicked()), this, SLOT(duplicateCurrentTab()) );

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

  currentView_ = new webView();

  tabs->removeTab(0);
  newTab(currentView_);

  setConnections();

  currentView_->load( QUrl(startFile_) );

  connect( actionClose , SIGNAL(triggered() ) , this, SLOT(hide()) );

  generateTree();
  tree->setHeaderHidden( true );

  tabs->setContextMenuPolicy( Qt::CustomContextMenu );
  tree->setContextMenuPolicy( Qt::CustomContextMenu );

  contextMenu_ = new QMenu();

  connect( tree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeClicked(QTreeWidgetItem*,int)) );
  connect( tree, SIGNAL(customContextMenuRequested (const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));
  connect( tabs, SIGNAL(customContextMenuRequested (const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));
  connect( tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)) );
}

///update the signals/slots which are only active on the visible webView
void HelpWidget::setConnections()
{
  back_->disconnect();
  connect(back_, SIGNAL(triggered()), currentView_, SLOT(back()));
  forward_->disconnect();
  connect(forward_, SIGNAL(triggered()), currentView_, SLOT(forward()));
}

void HelpWidget::loadFinished(bool)
{

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
  currentView_->load( QUrl(startFile_) );
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
void HelpWidget::treeClicked(QTreeWidgetItem* item, int /*column*/ ){
  QTreeWidgetItem* parent = item->parent();

  QString path = baseDir_;

  while(parent != 0){
    path += parent->text(1) + OpenFlipper::Options::dirSeparator();
    parent = parent->parent();
  }

  currentView_->load( QUrl(path + item->text(1) + OpenFlipper::Options::dirSeparator() + "index.html") );
}

///the url changed
void HelpWidget::urlChanged(const QUrl& /*url*/){

  back_->setEnabled( currentView_->history()->canGoBack() );
  forward_->setEnabled( currentView_->history()->canGoForward() );
}

/// add a new Tab to the Tabs
void HelpWidget::newTab(QWebView* _webView){
  webView* view = dynamic_cast< webView* > (_webView);

  connect(view, SIGNAL(titleChanged(const QString&)), this, SLOT(titleChanged(const QString&)));
  connect(view, SIGNAL(windowCreated(QWebView*)), this, SLOT(newTab(QWebView*)) );

  connect(view, SIGNAL( loadFinished(bool) ), this, SLOT( loadFinished(bool) ));
  connect(view, SIGNAL( find() ), this, SLOT( showFind() ));
  connect(view, SIGNAL( findNext() ), this, SLOT( findNext1() ));

  connect(view, SIGNAL(urlChanged(const QUrl &)), this, SLOT(urlChanged(const QUrl &)) );

  tabs->addTab(view, "");
}

/// the title of one of the tabs changed
void HelpWidget::titleChanged(const QString & title)
{
  for(int i=0; i < tabs->count(); i++)
    if (tabs->widget(i) == sender())
      tabs->setTabText(i, title);
}

/// current tab changed
void HelpWidget::tabChanged(int _index)
{
  currentView_ = dynamic_cast< webView* > (tabs->widget(_index));
  setConnections();

  back_->setEnabled( currentView_->history()->canGoBack() );
  forward_->setEnabled( currentView_->history()->canGoForward() );
}

/// close the current tab
void HelpWidget::closeCurrentTab()
{
  if (tabs->count() > 1)
    tabs->removeTab( tabs->currentIndex() );
}

/// add a duplicate of the current Tab to the TabWidget
void HelpWidget::duplicateCurrentTab()
{
  webView* newView = new webView();

  newView->load( currentView_->url() );

  newTab(newView);

  tabs->setCurrentIndex(tabs->count()-1);
}

//----------------------------------------------------------------------------
// Finding
//----------------------------------------------------------------------------

///initialize the widgets corresponding to finding
void HelpWidget::initFindFrame()
{
  findText_ = new findEdit();

  //set icons
  QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();

  findClose->setIcon( QIcon(iconPath + "application-exit.png") );
  findPrev->setIcon(  QIcon(iconPath + "arrow-left.png") );
  findNext->setIcon(  QIcon(iconPath + "arrow-right.png") );

  //connect signals
  connect(findText_, SIGNAL(find()), this, SLOT(findNext1()));
  connect(findText_, SIGNAL(textEdited()), this, SLOT(findNext1()));
  connect(findText_, SIGNAL(close()), findFrame, SLOT(hide()));
  connect(findClose, SIGNAL(clicked()), findFrame, SLOT(hide()));
  connect(findNext, SIGNAL(clicked()), this, SLOT(findNext1()));
  connect(findPrev, SIGNAL(clicked()), this, SLOT(findPrev1()));

  //set layout
  QHBoxLayout *layout = new QHBoxLayout;

  layout->addWidget( findClose );
  layout->addWidget( findText_ );
  layout->addWidget( findPrev );
  layout->addWidget( findNext );
  layout->addWidget( findSensitive );
  layout->addStretch();

  findFrame->setLayout(layout);

  findFrame->hide();
}

///show the find frame under the current page
void HelpWidget::showFind(){
  findFrame->setVisible( true );
  findText_->selectAll();
  findText_->setFocus(Qt::OtherFocusReason);
}

/// find the next element in the text that fits the search term
void HelpWidget::findNext1()
{
  if (findText_->text() == ""){
    //TODO do something that clears the selection
  }

  if ( findSensitive->isChecked() )
    currentView_->findText( findText_->text() , QWebPage::FindCaseSensitively | QWebPage::FindWrapsAroundDocument);
  else
    currentView_->findText( findText_->text() , QWebPage::FindWrapsAroundDocument);
}

/// find the previous element in the text that fits the search term
void HelpWidget::findPrev1()
{
  if ( findSensitive->isChecked() )
    currentView_->findText( findText_->text() ,
                           QWebPage::FindBackward | QWebPage::FindCaseSensitively | QWebPage::FindWrapsAroundDocument);
  else
    currentView_->findText( findText_->text() , QWebPage::FindBackward | QWebPage::FindWrapsAroundDocument);
}

//----------------------------------------------------------------------------
// Context Menu
//----------------------------------------------------------------------------

///show the context menu
void HelpWidget::showContextMenu(const QPoint & _pos)
{

  QObject* treeObj = dynamic_cast< QObject* > (tree);

  if(sender() == treeObj){

    //sender was the treeWidget

    lastContextMenu_ = tree->mapToGlobal(_pos);
    contextMenu_->clear();
    QAction* open = contextMenu_->addAction("Open");
    QAction* openNew = contextMenu_->addAction("Open in new tab");
    connect(open, SIGNAL(triggered()), this, SLOT(contextOpenTab()));
    connect(openNew, SIGNAL(triggered()), this, SLOT(contextOpenNewTab()));

  }else{

    //sender was the tabWidget

    lastContextMenu_ = tabs->mapToGlobal(_pos);

    //first check if the tabBar was clicked
    QWidget* w = tabs->childAt(tabs->mapFromGlobal(lastContextMenu_));

    if(w == 0) return;

    QTabBar* tabBar = dynamic_cast< QTabBar* > (w);

    if (tabBar == 0) return;

    int id = tabBar->tabAt( tabBar->mapFromGlobal(lastContextMenu_) );

    if (id == -1) return;

    //set up the menu
    contextMenu_->clear();
    QAction* close = contextMenu_->addAction("Close Tab");
    connect(close, SIGNAL(triggered()), this, SLOT(contextCloseTab()));
  }

  //finally show the menu
  contextMenu_->popup(lastContextMenu_);

}

///open content in current tab initiated by contextMenu
void HelpWidget::contextOpenTab()
{
  QTreeWidgetItem* item = tree->itemAt(tree->mapFromGlobal(lastContextMenu_));

  if (item != 0)
    treeClicked(item, 0 );
}

///open content in new tab initiated by contextMenu
void HelpWidget::contextOpenNewTab()
{
  QTreeWidgetItem* item = tree->itemAt(tree->mapFromGlobal(lastContextMenu_));

  if (item != 0){
    duplicateCurrentTab();
    treeClicked(item, 0 );
  }

}

///close tab initiated by contextMenu
void HelpWidget::contextCloseTab()
{
  QWidget* w = tabs->childAt(tabs->mapFromGlobal(lastContextMenu_));

  if(w != 0){
    QTabBar* tabBar = dynamic_cast< QTabBar* > (w);

    if (tabBar != 0){
      int id = tabBar->tabAt( tabBar->mapFromGlobal(lastContextMenu_) );
      if (id != -1)
        tabs->removeTab( id );
    }
  }
}

