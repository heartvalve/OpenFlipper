/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#ifndef JOINTT_HH
#define JOINTT_HH

#include <vector>

template<typename PointT>
class SkeletonT;

/**
 * @brief Represents a single joint in the skeleton
 *
 * The skeleton is made up by a hierarchical structure of joints. The joints don't store their position
 * themselves. Instead, the joint positions are stored in poses. The poses are managed by the AnimationT class
 * and the skeleton.
 */
template <class PointT>
class JointT
{
  template< class >
  friend class SkeletonT;

public:
  typedef PointT                                 Point;
  typedef typename Point::value_type             Scalar;
  typedef JointT<PointT>                         Joint;
  typedef typename std::vector<Joint*>::iterator ChildIter;

public:
  /// Constructor
  JointT(const Joint &_other);
  JointT(Joint *_parent, std::string _name = "");
  /// Destructor
  ~JointT();
  
  /// returns the joint id
  inline unsigned int id();

  /// access parent of the joint
  inline void setParent(Joint *_newParent, SkeletonT<PointT> &_skeleton);
  inline Joint *parent();
  
  inline bool isRoot();

public:
  /**
    * @name Child access
    * Use this iterator to access the joints child nodes
    */
  //@{
  inline ChildIter begin();
  inline ChildIter end();
  inline size_t size();
  inline Joint *child(size_t _index);
  //@}

  /**
   * @name Selections
   * change and access selection state
   */
  //@{
  inline bool selected();
  inline void setSelected(bool _selected);
  //@}

  /// Access the name of the joint
  inline std::string name();
  inline void setName(std::string _name);

private:
  /// An unique identifier, guaranteed to be part of a continuous sequence starting from 0
  unsigned int id_;
  bool         selected_;

protected:
  inline void setId(unsigned int _id);
  
  /// The parent joint; this joint is in its parents JointT::children_ vector. It's 0 for the root node.
  Joint *parent_;
  /// The joints children, use the JointT::getChild method to access them
  std::vector<Joint*> children_;
  /// the name of the joint
  std::string name_;
};

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(JOINTT_C)
#define JOINTT_TEMPLATES
#include "JointT.cc"
#endif
//=============================================================================
#endif
