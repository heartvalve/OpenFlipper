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
//   $Revision: 3468 $
//   $Author: moebius $
//   $Date: 2008-10-17 14:58:52 +0200 (Fr, 17. Okt 2008) $
//
//=============================================================================




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


//== FORWARDDECLARATIONS ======================================================


class QWheel;
class QtQGLWidget_Internal;
class QtSceneGraphWidget;


//== CLASS DEFINITION =========================================================


/** \class QtExaminerViewer QtExaminerViewer.hh <OpenFlipper/widgets/glWidget/QtExaminerViewer.hh>

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
  bool mapToSphere(const QPoint& _p, ACG::Vec3d& _result) const;


  // mouse interaction
  ACG::Vec3d    lastPoint3D_;
  bool          lastPoint_hitSphere_;
  bool          allowRotation_;


  // animation stuff
  ACG::Vec3d                   lastRotationAxis_;
  double                       lastRotationAngle_;
  QTime                        lastMoveTime_;
  QTimer*                      timer_;
};


//=============================================================================
//=============================================================================
#endif // ACG_QTEXAMINERWIDGET_HH defined
//=============================================================================

