//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  MyTypes
//
//=============================================================================

#define BASEOBJECTDATA_C


//== INCLUDES =================================================================

#include "Types.hh"

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

// ===============================================================================
// Additional Nodes
// ===============================================================================

template< typename NodeT >
bool BaseObjectData::addAdditionalNode(NodeT* _node , QString _pluginName, QString _nodeName , int _id )
{
   if ( hasAdditionalNode(_pluginName,_nodeName,_id) )  {
     std::cerr << "Trying to inserted additional node twice in " <<  _pluginName.toStdString()
                   << " with Name " << _nodeName.toStdString() << " and id " << _id << std::endl;
     return false;
   }

   QString name = _pluginName+"#"+_nodeName+"#"+QString::number(_id);

   std::pair <BaseNode*,QString> newNode(dynamic_cast<BaseNode*>(_node) , name);
   additionalNodes_.push_back(newNode);

   return true;
}

template< typename NodeT >
bool BaseObjectData::getAdditionalNode(NodeT*& _node , QString _pluginName, QString _nodeName , int _id )
{
  QString searchname = _pluginName + "#" + _nodeName + "#" + QString::number(_id);

  for ( uint i =0 ; i < additionalNodes_.size() ; ++i ) {
    if ( additionalNodes_[i].second == searchname ) {
        _node = dynamic_cast<NodeT*>(additionalNodes_[i].first);
        return ( _node != NULL);
    }
  }

  return false;
}

template< typename NodeT >
bool BaseObjectData::removeAdditionalNode(NodeT*& _node, QString _pluginName, QString _nodeName , int _id )
{
  QString searchname = _pluginName + "#" + _nodeName + "#" + QString::number(_id);

  for ( uint i =0 ; i < additionalNodes_.size() ; ++i ) {
    if ( additionalNodes_[i].second == searchname ) {
        _node = dynamic_cast<NodeT*>(additionalNodes_[i].first);
        if (_node != NULL) //valid node delete subtree
        {
          _node->delete_subtree();
          additionalNodes_.erase (additionalNodes_.begin()+i);
        }
        return true;
    }
  }

  return false;
}

//=============================================================================
