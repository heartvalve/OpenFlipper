/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS QtSceneGraphWidget
//
//=============================================================================


#ifndef ACG_QTSCENEGRAPHWIDGET_HH
#define ACG_QTSCENEGRAPHWIDGET_HH


//== INCLUDES =================================================================

#include <ACG/Config/ACGDefines.hh>
#include <ACG/Scenegraph/BaseNode.hh>
#include <QDialog>
#include <QKeyEvent>
#include <QMenu>
#include <QTreeWidget>
#include <QMap>


//== FORWARDDECLARATIONS ======================================================



//== NAMESPACES ===============================================================


namespace ACG {
namespace QtWidgets {


//== CLASS DEFINITION =========================================================

class ACGDLLEXPORT SceneGraphWidgetGenerator {

public:
  // constructor
  SceneGraphWidgetGenerator();

  // destructor
  virtual ~SceneGraphWidgetGenerator() {}

  /** \brief Get a widget for this Node
  *
  * Derive from this class to get a widget for the given node
  * you have to overload this function. First check, if your
  * widget can handle nodes of the given type (check for _node->className() )
  * Then generate your widget and handle the _node inside it.
  * Return 0 if you can not handle it and check if the map already contains
  * a widget for this node so that you generate one widget per node.
  */
  virtual QWidget* getWidget(SceneGraph::BaseNode* _node );

  /** \brief returns if the widgets can handle the given class
  *
  * This function has to check if the given classname is supported
  */
  virtual bool canHandle(std::string _className);

  /** \brief return the type this generator handles
  *
  * return the classname of the node this generator can handle
  */
  virtual std::string handles();

  /** \brief Return a name for your widget in the context menu
  *
  * This name will be the same for all passed in nodes
  */
  virtual QString contextMenuName();

protected:

  QMap< SceneGraph::BaseNode* , QWidget* > widgetMap_;
};



/** \class QtSceneGraphWidget QtSceneGraphWidget.hh <ACG/QtWidgets/QtSceneGraphWidget.hh>

    This class is used within the QtExaminerWidget to display and
    modify the current scenegraph.

    \see QtExaminerWidget
**/

class ACGDLLEXPORT QtSceneGraphWidget : public QTreeWidget
{
  Q_OBJECT

public:

  /// default constructor
  QtSceneGraphWidget(QWidget* _parent = 0,
                     SceneGraph::BaseNode* _rootNode = 0);


  /// destructor
  virtual ~QtSceneGraphWidget() {}

  enum Columns { Node, Type, Status, Mode };


  class Item : public QTreeWidgetItem
  {
  public:
    // root constructor
    Item(QTreeWidget* _parent,
         SceneGraph::BaseNode* _node);

    // child constructor
    Item(Item* _parent,
         SceneGraph::BaseNode* _node);

    // get node
    SceneGraph::BaseNode * node() { return node_; }

    // update text, etc.
    void update();

  private:
    SceneGraph::BaseNode*  node_;
  };



public slots:

  /// Update recursively from \c _rootNode on
  void update( ACG::SceneGraph::BaseNode* _rootNode );


signals:

  /** This signal is emitted when the user changes a node through
      its node dialog **/
  void signalNodeChanged(ACG::SceneGraph::BaseNode* _node);


private slots:

  void slotItemPressed( QTreeWidgetItem * _item,
			int            _col );

  void slotItemExpandedOrCollapsed( QTreeWidgetItem * _item );

  void slotNodeChanged( ACG::SceneGraph::BaseNode * _node );

  void slotModeMenu  ( QAction * _action );
  void slotStatusMenu( QAction * _action );

  void slotEditMaterial();
  void slotEditTexture();
  void slotEditShader();
  void slotEditClipPlanes();
  void slotEditCoordinateFrame();



private:

  /// copy constructor
  QtSceneGraphWidget( const QtSceneGraphWidget & _rhs );
  /// assignment operator
  QtSceneGraphWidget & operator=( const QtSceneGraphWidget & _rhs );

  /// update _node
  void update( SceneGraph::BaseNode * _node, Item * _parent );

  /// key events
  void keyPressEvent(QKeyEvent* _event);
  /// key events
  void keyReleaseEvent(QKeyEvent* _event);



  SceneGraph::BaseNode * rootNode_;

  QMenu *                modeMenu_;

  Item  *                curItem_;
  bool                   shiftPressed_; // store shift for popup menu

  struct StatusActions {
    QMenu   * menu_;
    QAction * actionActive_;
    QAction * actionHideNode_;
    QAction * actionHideChildren_;
    QAction * actionHideSubtree_;
  } statusActions_;
  
  public slots:
  
    void expandAll();

    void updateAll();

  public: 

    /** \brief Add a node widget handler
    * 
    * returns true, if the handler was sucessfully added.
    */
    bool addWidgetGenerator( SceneGraphWidgetGenerator* _generator );

    /** \brief Set a complete generator map (this will overwrite the existing one!
    */
    void setGeneratorMap( QMap< std::string , SceneGraphWidgetGenerator* > _map) { generatorMap_ = _map; }; 

  private:
    QMap< std::string , SceneGraphWidgetGenerator* > generatorMap_;
};



//== CLASS DEFINITION =========================================================


class ACGDLLEXPORT QtSceneGraphDialog : public QDialog
{
  Q_OBJECT

public:

  QtSceneGraphDialog( QWidget              * _parent = 0,
		      SceneGraph::BaseNode * _rootNode = 0 );


public slots:

  /// Update recursively from \c _rootNode on
  void update(ACG::SceneGraph::BaseNode* _rootNode);


signals:

  /** This signal is emitted when the user changes a node through
      its node dialog */
  void signalNodeChanged(ACG::SceneGraph::BaseNode* _node);


private slots:

  void slotNodeChanged(ACG::SceneGraph::BaseNode* _node);


public:
  /** \brief Set a complete generator map (this will overwrite the existing one!
  */
  void setGeneratorMap( QMap< std::string , SceneGraphWidgetGenerator* > _map) { sgw_->setGeneratorMap(_map); }; 

private:

  QtSceneGraphWidget * sgw_;

};


//=============================================================================
} // namespace ACG
} // namespace QtWidgets
//=============================================================================
#endif // ACG_QTSCENEGRAPHWIDGET_HH defined
//=============================================================================
