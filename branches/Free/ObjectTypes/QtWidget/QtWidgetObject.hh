/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision: 14482 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2012-04-30 13:31:46 +0200 (Mon, 30 Apr 2012) $                     *
*                                                                            *
\*===========================================================================*/

//=============================================================================
//
//  Types
//
//=============================================================================

/**
 * \file QtWidgetObject.hh
 * This File contains the QtWidget Object
 */


#ifndef QT_WIDGET_OBJECT_HH
#define QT_WIDGET_OBJECT_HH



//== INCLUDES =================================================================

#include <OpenFlipper/common/BaseObjectData.hh>
#include <QWidget>

#include "QtWidgetTypes.hh"

//== TYPEDEFS =================================================================

// //== CLASS DEFINITION =========================================================

class DLLEXPORT QtWidgetObject : public BaseObjectData {

  public:
    /// constructor
    QtWidgetObject();

    /** \brief copy constructor
     *
     *  Create a copy of this object
     */
    QtWidgetObject(const QtWidgetObject& _object);

    /// destructor
    virtual ~QtWidgetObject();

    /// Reset current object, including all related nodes.
    virtual void cleanup();

    /** return a full copy of this object ( All scenegraph nodes will be created )
     *  but the object will not be a part of the object tree.
     */
    BaseObject* copy();


  protected:
    /// Initialize current object, including all related nodes.
    virtual void init(QWidget* _widget);

  //===========================================================================
  /** @name Name and Path handling
   * @{ */
  //===========================================================================
  public:

    /// Set the name of the Object
    void setName( QString _name );

  /** @} */

  //===========================================================================
  /** @name Contents
   * @{ */
  //===========================================================================
  public:

    QWidget* widget()const{return widgetNode_->widget();}

    void setWidget(QWidget* _widget){widgetNode_->setWidget(_widget);}

  /** @} */

  //===========================================================================
  /** @name Visualization
   * @{ */
  //===========================================================================

  public:
    /// Get the scenegraph Node
    ACG::SceneGraph::QtWidgetNode* qtWidgetNode();

    virtual bool hasNode(BaseNode* _node);

  private:
    QtWidgetNode* widgetNode_;

  /** @} */

  //===========================================================================
  /** @name Object Information
   * @{ */
  //===========================================================================
  public:
    /// Get all Info for the Object as a string
    QString getObjectinfo();

  /** @} */


  //===========================================================================
  /** @name Picking
   * @{ */
  //===========================================================================
  public:
    /// detect if the node has been picked
    bool picked( uint _node_idx );

    /// Enable or disable picking for this Object
    void enablePicking( bool _enable );

    /// Check if picking is enabled for this Object
    bool pickingEnabled();

    void mouseEvent(QMouseEvent* _event){widgetNode_->mouseEvent(_event);}

  /** @} */

  public:

    /// Show Node
    virtual void visible(bool _visible);

    /// Show Node
    virtual bool visible();

    /// Show Node
    virtual void show();

    /// Hide Node
    virtual void hide();

};

//=============================================================================
#endif // QT_WIDGET_OBJECT_HH defined
//=============================================================================
 
