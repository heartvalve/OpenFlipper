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
