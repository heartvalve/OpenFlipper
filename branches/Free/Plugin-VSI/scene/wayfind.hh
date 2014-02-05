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

#ifndef VSI_WAYFIND_HH
#define VSI_WAYFIND_HH

//== INCLUDES =================================================================
#include <QRegion>
#include <QPolygonF>
#include <QPoint>
#include <QLinkedList>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class GraphicsScene;
class Connection;

//== CLASS DEFINITION =========================================================

/** A-Star way finding implementation with dynamic growing area
  */
class WayFind {

  public:
    /// Constructor
    WayFind (GraphicsScene *_scene);

    /// Destructor
    ~WayFind ();

    /// Finds a way from _from to _to ignoring any already existent connections from _conn
    QPolygonF findWay (Connection *_conn, QPoint _from, QPoint _to);

  private:

    // Area node class
    class Node {
      public:

        // node direction
        enum Type {
          Vertical,
          Horizontal
        };

        Node (unsigned int _counter);
        ~Node ();

      public:
        // last used counter
        unsigned int counter_;

        // current direction
        Type type_;

        // neighbor nodes
        Node *n_[4];

        // linked list handling
        Node *prev_;
        Node *next_;

        // node position
        QPoint pos_;

        // initialized from
        Node *from_;

        // A-Star consts
        unsigned int g_;
        unsigned int f_;
        unsigned int h_;

        // node cost
        unsigned int cost_;

        // already traversed
        bool closed_;
    };

    // Next position in distance _step from _pnt in direction _dir
    QPoint validPos (unsigned int _dir, int _step, QPoint _pnt);

    // Heuristic distance from _from to _to
    int heuristicDistance (const QPoint &_from, const QPoint &_to) const;

    // cleanup ununsed nodes
    void cleanup ();

  private:

    GraphicsScene *scene_;

    unsigned int counter_;

    QLinkedList<Node *> nodes_;
    QHash<QPair<int, int>, Node*> map_;

    Node *ll_;
    QPoint oldFrom_;
    QRegion oldReg_;
};

}

#endif
