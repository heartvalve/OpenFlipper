/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 83 $                                                         *
 *   $Author: kremer $                                                       *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
 *                                                                           *
\*===========================================================================*/

#ifndef PROCESSITEM_HH
#define PROCESSITEM_HH

#include <QString>
#include <QList>
#include <vector>

#include <OpenFlipper/common/Types.hh>

class ProcessItem {

  public :

    ProcessItem(QString _name, QString _description, float _progress, float _progressMax, ProcessItem* _parent) :
    name_(_name),
    description_(_description),
    progress_(_progress),
    progressMax_(_progressMax),
    parent_(_parent) {};

  // dynamic members
  public:
  
    /// name
    QString name( ) { return name_; };
    void name( QString _name ) { name_ = _name; };
    
    /// description
    QString description( ) { return description_; };
    void description( QString _description ) { description_ = _description; };
    
    float progress() { return progress_; }
    void progress(float _progress) { progress_ = _progress; };
    
    float progressMax() { return progressMax_; }
    void progressMax(float _progressMax) { progressMax_ = _progressMax; };
    
  private:
    QString name_;
    QString description_;
    float progress_;
    float progressMax_;

  private:
    /// Parent item or 0 if rootnode
    ProcessItem *parent_;

    /// Children of this node
    QList<ProcessItem*> children_;

  public:
    //===========================================================================
    /** @name Tree : Parent nodes
    * @{ */
    //===========================================================================

    /// get the row of this item from the parent
    int row() const {
        if (parent_)
            return parent_->children_.indexOf(const_cast<ProcessItem*>(this));
        
        return 0;
    };

    /// Get the parent item ( 0 if rootitem )
    ProcessItem *parent() { return parent_; };

    /// Set the parent pointer
    void setParent(ProcessItem* _parent) { parent_ = _parent; };

    /** @} */

    //===========================================================================
    /** @name Tree : Children
    * @{ */
    //===========================================================================

    /// Remove a child from this object
    void removeChild( ProcessItem* _item ) {
        
        bool found = false;
        QList<ProcessItem*>::iterator i;
        for (i = children_.begin(); i != children_.end(); ++i) {
            if ( *i == _item ) {
                found = true;
                break;
            }
        }
        
        if ( !found ) {
            std::cerr << "ProcessItem: Illegal remove request" << std::endl;
            return;
        }
        
        children_.erase(i);
    };
    
    ProcessItem* child(int row)
    {
        return children_.value(row);
    }
    
    int childCount() const
    {
        return children_.count();
    }
    
    /** @} */

};


//=============================================================================
#endif // PROCESSITEM_HH defined
//=============================================================================
