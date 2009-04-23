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
//  CLASS QtMaterialDialog
//
//=============================================================================


#ifndef ACG_QTMATERIALDIALOG_HH
#define ACG_QTMATERIALDIALOG_HH


//== INCLUDES =================================================================


#include "ui_QtMaterialDialogUi.hh"

#include "../Math/VectorT.hh"
#include "../GL/gl.hh"

#include <QColor>
#include <QDialog>

//== FORWARDDECLARATIONS ======================================================


namespace ACG {
  namespace SceneGraph {
    class MaterialNode;
    class BaseNode;
  }
}


//== NAMESPACE ================================================================


namespace ACG {
namespace QtWidgets {


//== CLASS DEFINITION =========================================================


class ACGDLLEXPORT QtMaterialDialog : public QDialog
{
  Q_OBJECT

public:

  QtMaterialDialog( QWidget                  * _parent,
		    SceneGraph::MaterialNode * _node );

  ~QtMaterialDialog() {}


signals:

  void signalNodeChanged( ACG::SceneGraph::BaseNode * _node );

private:


  QColor convertColor( Vec4f  _color);
  Vec4f  convertColor( QColor _color);

  void setButtonColor( QPushButton * _button,
		       const Vec4f & _color );

private slots:

  void changeBaseColor();
  void changeAmbientColor();
  void changeDiffuseColor();
  void changeSpecularColor();
  void changeShine(int _new);
  void changePointSize(int _new);
  void changeLineWidth(int _new);
  void changeRoundPoints(bool _b);
  void changeBackfaceCulling(bool _b);
  void changeAlphaTest(bool _b);
  void changeAlphaValue(int _new);
  void changeBlending(bool _b);
  void changeBlendingParam1(const QString& _name);
  void changeBlendingParam2(const QString& _name);
  void changeColorMaterial(bool _b);

  void changeActive(bool toggle);

  void enableProperty();
  void enableProperty(int i);

  QString paramToStr(GLenum param);

  void applyChanges();
  void undoChanges();

  void reject();

private:



  unsigned int  applyProperties_;

  Vec4f    color_,            bak_color_,
           ambient_,          bak_ambient_,
           diffuse_,          bak_diffuse_,
           specular_,         bak_specular_;
  float    shine_,            bak_shine_;
  float    point_size_,       bak_point_size_;
  float    line_width_,       bak_line_width_;
  bool     round_points_,     bak_round_points_;
  bool     backfaceCulling_,  bak_backfaceCulling_;
  bool     alphaTest_,        bak_alphaTest_;
  float    alphaValue_,       bak_alphaValue_;
  bool     blending_,         bak_blending_;
  GLenum   blendParam1_,      bak_blendParam1_;
  GLenum   blendParam2_,      bak_blendParam2_;
  bool     colorMaterial_,    bak_colorMaterial_;

  bool     baseColorActive_,       bak_baseColorActive_;
  bool     materialActive_,        bak_materialActive_;
  bool     pointSizeActive_,       bak_pointSizeActive_;
  bool     lineWidthActive_,       bak_lineWidthActive_;
  bool     roundPointsActive_,     bak_roundPointsActive_;
  bool     alphaTestActive_,       bak_alphaTestActive_;
  bool     blendingActive_,        bak_blendingActive_;
  bool     backfaceCullingActive_, bak_backfaceCullingActive_;
  bool     colorMaterialActive_,   bak_colorMaterialActive_;

  SceneGraph::MaterialNode * node_;

  Ui::QtMaterialDialogUi ui_;

};


//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
#endif // ACG_QTMATERIALDIALOG_HH defined
//=============================================================================

