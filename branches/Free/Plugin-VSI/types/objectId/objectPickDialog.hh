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

#ifndef VSI_OBJECTPICKDIALOG_HH
#define VSI_OBJECTPICKDIALOG_HH

//== INCLUDES =================================================================

#include <QDialog>
#include <QModelIndex>
#include "SelectionObjectMarker.hh"

//== FORWARDDECLARATIONS ======================================================
class QPushButton;
class QTreeView;
class QMouseEvent;

class TreeModel;

class SimpleViewer;

//== NAMESPACES ===============================================================
namespace VSI {

//== CLASS DEFINITION =========================================================

/** Widget to pick a object
 */

class ObjectPickDialog : public QDialog {
  Q_OBJECT

  public:
    /// Constructor
    ObjectPickDialog (QStringList _flags, QStringList _types, bool _withGroups);

    /// Destructor
    ~ObjectPickDialog ();

    /// Current selected object
    unsigned int selectedId ();

    /// Set selected object
    void selectedId (unsigned int _id);

  private slots:
    void activated (const QModelIndex &_index);
    void slotMouseEventClick (QMouseEvent* _event, bool _double);

  private:
    void setForGroup (BaseObject *_obj, QString _flag, bool _enabled);

  private:

    SimpleViewer *viewer_;

    QTreeView *treeView_;
    QPushButton *okButton_;
    QPushButton *cancelButton_;

    TreeModel *model_;

    SelectionObjectMarker marker_;

    unsigned int selectedId_;
};
//=============================================================================
}
//=============================================================================

#endif
