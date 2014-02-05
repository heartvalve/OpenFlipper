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
//  CLASS QtExaminerViewer
//
//=============================================================================

#ifndef ACG_QTEXAMINERVIEWER_HH
#define ACG_QTEXAMINERVIEWER_HH


//== INCLUDES =================================================================


#include "QtBaseViewer.hh"

#include <QTime>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>


//== NAMESPACES ===============================================================


namespace ACG {
namespace QtWidgets {
  

//== FORWARDDECLARATIONS ======================================================


class QWheel; 
class QtQGLWidget_Internal;
class QtSceneGraphWidget;


//== CLASS DEFINITION =========================================================


/** \class QtExaminerViewer QtExaminerViewer.hh <ACG/QtWidgets/QtExaminerViewer.hh>

    Widgets used to display and examine scenegraphs.
    This widget is used in all viewers that want to explore scenegraphs.
    For WalkMode use "QtWalkViewer"
 **/
class ACGDLLEXPORT QtExaminerViewer : public QtBaseViewer
{
Q_OBJECT

public:


  /// Constructor
  QtExaminerViewer( QWidget* _parent=0, 
		    const char* _name=0, 
		    QStatusBar *_statusBar=0, 
		    const QGLFormat* _format=0,
		    const QtBaseViewer* _share=0,
		    Options _options=DefaultOptions );
 
  // Destructor.
  virtual ~QtExaminerViewer() {}


  /// Lock scene rotation 
  void allowRotation( bool _mode ) { allowRotation_ = _mode; };


  /// Factors for zooming with the wheel
public:
  double wheelZoomFactor();
  double wheelZoomFactorShift();
  void setWheelZoomFactor(double _factor);
  void setWheelZoomFactorShift(double _factor);
private:
  double wZoomFactor_;
  double wZoomFactorShift_;


private slots:

  void slotAnimation();


protected:

  /// handle new key events
  virtual bool viewKeyPressEvent(QKeyEvent* /* _event */ ) { return false; }
  /// handle wheel events
  virtual void viewWheelEvent(QWheelEvent* _event);
  /// handle mouse events
  virtual void viewMouseEvent(QMouseEvent* _event);
  /// handle mouse events
  virtual void lightMouseEvent(QMouseEvent* _event);


protected:

  // mouse interaction
  QPoint   lastPoint2D_;


private:

  /// virtual trackball: map 2D screen point to unit sphere
  bool mapToSphere(const QPoint& _p, Vec3d& _result) const;


  // mouse interaction
  Vec3d    lastPoint3D_;
  bool     lastPoint_hitSphere_;
  bool     allowRotation_;


  // animation stuff
  Vec3d                        lastRotationAxis_;
  double                       lastRotationAngle_;
  QTime                        lastMoveTime_;
  QTimer*                      timer_;
};


//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
#endif // ACG_QTEXAMINERWIDGET_HH defined
//=============================================================================

