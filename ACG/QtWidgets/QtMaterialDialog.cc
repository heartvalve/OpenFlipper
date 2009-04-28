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
//  CLASS QtMaterialDialog - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtMaterialDialog.hh"
#include "../Scenegraph/MaterialNode.hh"

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QLayout>
#include <QVariant>
#include <QToolTip>
#include <QColorDialog>
#include <QSlider>


//== NAMESPACES ==============================================================


namespace ACG {
namespace QtWidgets {


//== IMPLEMENTATION ==========================================================


QtMaterialDialog::QtMaterialDialog( QWidget                  * _parent,
				    SceneGraph::MaterialNode * _node )
  : QDialog( _parent ),
    node_(_node)
{
  ui_.setupUi( this );
  //  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // get initial values from node
  color_           = bak_color_           = node_->base_color();
  ambient_         = bak_ambient_         = node_->ambient_color();
  diffuse_         = bak_diffuse_         = node_->diffuse_color();
  specular_        = bak_specular_        = node_->specular_color();
  shine_           = bak_shine_           = node_->shininess();
  point_size_      = bak_point_size_      = node_->point_size();
  line_width_      = bak_line_width_      = node_->line_width();
  round_points_    = bak_round_points_    = node_->round_points();
  line_smooth_     = bak_line_smooth_     = node_->line_smooth();
  backfaceCulling_ = bak_backfaceCulling_ = node_->backface_culling();
  alphaTest_       = bak_alphaTest_       = node_->alpha_test();
  alphaValue_      = bak_alphaValue_      = node_->alpha_value();
  blending_        = bak_blending_        = node_->blending();
  blendParam1_     = bak_blendParam1_     = node_->blending_param1();
  blendParam2_     = bak_blendParam2_     = node_->blending_param2();
  colorMaterial_   = bak_colorMaterial_   = node_->colorMaterial();

  baseColorActive_       = bak_baseColorActive_       = node_->applyProperties() & SceneGraph::MaterialNode::BaseColor;
  materialActive_        = bak_materialActive_        = node_->applyProperties() & SceneGraph::MaterialNode::Material;
  pointSizeActive_       = bak_pointSizeActive_       = node_->applyProperties() & SceneGraph::MaterialNode::PointSize;
  lineWidthActive_       = bak_lineWidthActive_       = node_->applyProperties() & SceneGraph::MaterialNode::LineWidth;
  roundPointsActive_     = bak_roundPointsActive_     = node_->applyProperties() & SceneGraph::MaterialNode::RoundPoints;
  lineSmoothActive_      = bak_lineSmoothActive_      = node_->applyProperties() & SceneGraph::MaterialNode::LineSmooth;
  alphaTestActive_       = bak_alphaTestActive_       = node_->applyProperties() & SceneGraph::MaterialNode::AlphaTest;
  blendingActive_        = bak_blendingActive_        = node_->applyProperties() & SceneGraph::MaterialNode::Blending;
  backfaceCullingActive_ = bak_backfaceCullingActive_ = node_->applyProperties() & SceneGraph::MaterialNode::BackFaceCulling;
  colorMaterialActive_   = bak_colorMaterialActive_   = node_->applyProperties() & SceneGraph::MaterialNode::ColorMaterial;

  setButtonColor( ui_.baseColorButton, color_ );
  setButtonColor( ui_.ambientColorButton, ambient_ );
  setButtonColor( ui_.diffuseColorButton, diffuse_ );
  setButtonColor( ui_.specularColorButton, specular_ );

  ui_.shininessSlider->setValue((int)shine_);
  ui_.pointSizeSpinBox->setValue((int)point_size_);
  ui_.lineWidthSpinBox->setValue((int)line_width_);
  ui_.roundPointsCheckBox->setChecked(round_points_);
  ui_.lineSmoothCheckBox->setChecked(line_smooth_);
  ui_.backfaceCulling->setChecked( backfaceCulling_ );
  ui_.alphaTest->setChecked( alphaTest_ );
  ui_.alpha->setValue((int) alphaValue_ * 100.0f );
  ui_.colorMaterial->setChecked( colorMaterial_ );
  ui_.blending->setChecked( blending_ );

  for (int i=0; i < ui_.blendParam1->count(); i++)
    if ( ui_.blendParam1->itemText(i) == paramToStr(blendParam1_) )
      ui_.blendParam1->setCurrentIndex( i );

  for (int i=0; i < ui_.blendParam2->count(); i++)
    if ( ui_.blendParam2->itemText(i) == paramToStr(blendParam2_) )
      ui_.blendParam2->setCurrentIndex( i );

  applyProperties_ = node_->applyProperties();

  ui_.baseColorActive->setChecked( baseColorActive_ );
  ui_.materialActive->setChecked( materialActive_ );
  ui_.pointSizeActive->setChecked( pointSizeActive_ );
  ui_.lineWidthActive->setChecked( lineWidthActive_ );
  ui_.roundPointsActive->setChecked( roundPointsActive_ );
  ui_.lineSmoothActive->setChecked( lineSmoothActive_ );
  ui_.alphaTestActive->setChecked( alphaTestActive_ );
  ui_.blendingActive->setChecked( blendingActive_ );
  ui_.backfaceCullingActive->setChecked( backfaceCullingActive_ );
  ui_.colorMaterialActive->setChecked( colorMaterialActive_ );

  connect( ui_.baseColorButton,     SIGNAL( clicked() ), this, SLOT( enableProperty() ) );
  connect( ui_.ambientColorButton,  SIGNAL( clicked() ), this, SLOT( enableProperty() ) );
  connect( ui_.diffuseColorButton,  SIGNAL( clicked() ), this, SLOT( enableProperty() ) );
  connect( ui_.specularColorButton, SIGNAL( clicked() ), this, SLOT( enableProperty() ) );
  connect( ui_.shininessSlider,     SIGNAL( sliderPressed() ), this, SLOT( enableProperty() ) );
  connect( ui_.pointSizeSpinBox,    SIGNAL( valueChanged(int) ), this, SLOT( enableProperty(int) ) );
  connect( ui_.lineWidthSpinBox,    SIGNAL( valueChanged(int) ), this, SLOT( enableProperty(int) ) );
  connect( ui_.roundPointsCheckBox, SIGNAL( pressed() ), this, SLOT( enableProperty() ) );
  connect( ui_.lineSmoothCheckBox,  SIGNAL( pressed() ), this, SLOT( enableProperty() ) );
  connect( ui_.backfaceCulling,     SIGNAL( pressed() ), this, SLOT( enableProperty() ) );
  connect( ui_.alphaTest,           SIGNAL( pressed() ), this, SLOT( enableProperty() ) );
  connect( ui_.blending,            SIGNAL( pressed() ), this, SLOT( enableProperty() ) );
  connect( ui_.colorMaterial,       SIGNAL( pressed() ), this, SLOT( enableProperty() ) );

  connect( ui_.baseColorButton, SIGNAL( clicked() ),
	   this, SLOT( changeBaseColor() ) );
  connect( ui_.ambientColorButton, SIGNAL( clicked() ),
	   this, SLOT( changeAmbientColor() ) );
  connect( ui_.diffuseColorButton, SIGNAL( clicked() ),
	   this, SLOT( changeDiffuseColor() ) );
  connect( ui_.specularColorButton, SIGNAL( clicked() ),
	   this, SLOT( changeSpecularColor() ) );
  connect( ui_.shininessSlider, SIGNAL( sliderMoved(int) ),
	   this, SLOT( changeShine(int) ) );

  connect( ui_.pointSizeSpinBox, SIGNAL( valueChanged(int) ),
	   this, SLOT( changePointSize(int) ) );
  connect( ui_.lineWidthSpinBox, SIGNAL( valueChanged(int) ),
	   this, SLOT( changeLineWidth(int) ) );
  connect( ui_.roundPointsCheckBox, SIGNAL( toggled(bool) ),
	   this, SLOT( changeRoundPoints(bool) ) );
  connect( ui_.lineSmoothCheckBox, SIGNAL( toggled(bool) ),
      this, SLOT( changeLineSmooth(bool) ) );
  connect( ui_.backfaceCulling, SIGNAL( toggled(bool) ),
     this, SLOT( changeBackfaceCulling(bool) ) );
  connect( ui_.alphaTest, SIGNAL( toggled(bool) ),
     this, SLOT( changeAlphaTest(bool) ) );
  connect( ui_.alpha, SIGNAL( valueChanged(int) ),
     this, SLOT( changeAlphaValue(int) ) );
  connect( ui_.colorMaterial, SIGNAL( toggled(bool) ),
     this, SLOT( changeColorMaterial(bool) ) );
  connect( ui_.blending, SIGNAL( toggled(bool) ),
     this, SLOT( changeBlending(bool) ) );
  connect( ui_.blendParam1, SIGNAL( currentIndexChanged(const QString&) ),
     this, SLOT( changeBlendingParam1(const QString&) ) );
  connect( ui_.blendParam2, SIGNAL( currentIndexChanged(const QString&) ),
     this, SLOT( changeBlendingParam2(const QString&) ) );


  connect( ui_.baseColorActive, SIGNAL( toggled(bool) ),
     this, SLOT( changeActive(bool) ) );
  connect( ui_.materialActive, SIGNAL( toggled(bool) ),
     this, SLOT( changeActive(bool) ) );
  connect( ui_.pointSizeActive, SIGNAL( toggled(bool) ),
     this, SLOT( changeActive(bool) ) );
  connect( ui_.lineWidthActive, SIGNAL( toggled(bool) ),
     this, SLOT( changeActive(bool) ) );
  connect( ui_.roundPointsActive, SIGNAL( toggled(bool) ),
     this, SLOT( changeActive(bool) ) );
  connect( ui_.lineSmoothActive, SIGNAL( toggled(bool) ),
     this, SLOT( changeActive(bool) ) );
  connect( ui_.alphaTestActive, SIGNAL( toggled(bool) ),
     this, SLOT( changeActive(bool) ) );
  connect( ui_.blendingActive, SIGNAL( toggled(bool) ),
     this, SLOT( changeActive(bool) ) );
  connect( ui_.backfaceCullingActive, SIGNAL( toggled(bool) ),
     this, SLOT( changeActive(bool) ) );
  connect( ui_.colorMaterialActive, SIGNAL( toggled(bool) ),
     this, SLOT( changeActive(bool) ) );

  connect( ui_.alphaTest,  SIGNAL( toggled(bool) ),
           ui_.alpha,        SLOT( setEnabled(bool) ) );
  connect( ui_.blending,  SIGNAL( toggled(bool) ),
           ui_.blendParam1,        SLOT( setEnabled(bool) ) );
  connect( ui_.blending,  SIGNAL( toggled(bool) ),
           ui_.blendParam2,        SLOT( setEnabled(bool) ) );


  ui_.alpha->setEnabled( ui_.alphaTest->isChecked() );
  ui_.blendParam1->setEnabled( ui_.blending->isChecked() );
  ui_.blendParam2->setEnabled( ui_.blending->isChecked() );

  ui_.baseColorActive->setChecked(baseColorActive_);
  ui_.materialActive->setChecked(materialActive_);
  ui_.pointSizeActive->setChecked(pointSizeActive_);
  ui_.lineWidthActive->setChecked(lineWidthActive_);
  ui_.roundPointsActive->setChecked(roundPointsActive_);
  ui_.lineWidthActive->setChecked(lineWidthActive_);
  ui_.alphaTestActive->setChecked(alphaTestActive_);
  ui_.blendingActive->setChecked(blendingActive_);
  ui_.backfaceCullingActive->setChecked(backfaceCullingActive_);
  ui_.colorMaterialActive->setChecked(colorMaterialActive_);

  connect( ui_.okButton, SIGNAL( clicked() ),
	   this, SLOT( accept() ) );
  connect( ui_.cancelButton, SIGNAL( clicked() ),
	   this, SLOT( reject() ) );

  layout()->setSizeConstraint( QLayout::SetFixedSize );
}


//-----------------------------------------------------------------------------


void
QtMaterialDialog::setButtonColor( QPushButton * _button,
				  const Vec4f & _color )
{
  QPalette p( _button->palette() );
  p.setColor( QPalette::Button, convertColor( _color ) );
  _button->setPalette( p );
}


//-----------------------------------------------------------------------------


QColor
QtMaterialDialog::convertColor( Vec4f _color)
{
 return QColor ((int)(_color[0]*255.0),
                (int)(_color[1]*255.0),
                (int)(_color[2]*255.0),
                (int)(_color[3]*255.0));
}


//-----------------------------------------------------------------------------


Vec4f
QtMaterialDialog::convertColor( QColor _color)
{
  return Vec4f (_color.redF(),
		          _color.greenF(),
		          _color.blueF(),
                _color.alphaF() );
}


//-----------------------------------------------------------------------------


void QtMaterialDialog::reject()
{
  undoChanges();
  QDialog::reject();
}


//-----------------------------------------------------------------------------


void QtMaterialDialog::applyChanges()
{

  unsigned int properties = 0;

  if (baseColorActive_) properties = properties | SceneGraph::MaterialNode::BaseColor;
  if (materialActive_) properties = properties | SceneGraph::MaterialNode::Material;
  if (pointSizeActive_) properties = properties | SceneGraph::MaterialNode::PointSize;
  if (lineWidthActive_) properties = properties | SceneGraph::MaterialNode::LineWidth;
  if (roundPointsActive_) properties = properties | SceneGraph::MaterialNode::RoundPoints;
  if (lineSmoothActive_) properties = properties | SceneGraph::MaterialNode::LineSmooth;
  if (alphaTestActive_) properties = properties | SceneGraph::MaterialNode::AlphaTest;
  if (blendingActive_) properties = properties | SceneGraph::MaterialNode::Blending;
  if (backfaceCullingActive_) properties = properties | SceneGraph::MaterialNode::BackFaceCulling;
  if (colorMaterialActive_) properties = properties | SceneGraph::MaterialNode::ColorMaterial;

  node_->applyProperties(properties);

  node_->set_base_color(color_);
  node_->set_ambient_color(ambient_);
  node_->set_diffuse_color(diffuse_);
  node_->set_specular_color(specular_);
  node_->set_shininess(shine_);
  node_->set_point_size(point_size_);
  node_->set_line_width(line_width_);
  node_->set_round_points(round_points_);
  node_->set_line_smooth(line_smooth_);

  if(backfaceCulling_)
    node_->enable_backface_culling();
  else
    node_->disable_backface_culling();

  if(alphaTest_)
    node_->enable_alpha_test( alphaValue_ );
  else
    node_->disable_alpha_test();

  if(blending_)
    node_->enable_blending( blendParam1_, blendParam2_ );
  else
    node_->disable_blending();

  if ( colorMaterial_ )
    node_->enable_color_material();
  else
    node_->disable_color_material();

  // this is not optimal !
  if(round_points_ || line_smooth_ )
    node_->enable_alpha_test(0.5);
  else
    node_->disable_alpha_test();

  setButtonColor( ui_.diffuseColorButton, diffuse_ );
  setButtonColor( ui_.ambientColorButton, ambient_ );
  setButtonColor( ui_.specularColorButton, specular_ );
  setButtonColor( ui_.baseColorButton, color_ );

  emit signalNodeChanged(node_);
}


//-----------------------------------------------------------------------------


void QtMaterialDialog::undoChanges()
{

  unsigned int properties = 0;

  if (bak_baseColorActive_) properties = properties | SceneGraph::MaterialNode::BaseColor;
  if (bak_materialActive_) properties = properties | SceneGraph::MaterialNode::Material;
  if (bak_pointSizeActive_) properties = properties | SceneGraph::MaterialNode::PointSize;
  if (bak_lineWidthActive_) properties = properties | SceneGraph::MaterialNode::LineWidth;
  if (bak_roundPointsActive_) properties = properties | SceneGraph::MaterialNode::RoundPoints;
  if (bak_lineSmoothActive_) properties = properties | SceneGraph::MaterialNode::LineSmooth;
  if (bak_alphaTestActive_) properties = properties | SceneGraph::MaterialNode::AlphaTest;
  if (bak_blendingActive_) properties = properties | SceneGraph::MaterialNode::Blending;
  if (bak_backfaceCullingActive_) properties = properties | SceneGraph::MaterialNode::BackFaceCulling;
  if (bak_colorMaterialActive_) properties = properties | SceneGraph::MaterialNode::ColorMaterial;

  node_->applyProperties(properties);

  node_->set_base_color(bak_color_);
  node_->set_ambient_color(bak_ambient_);
  node_->set_diffuse_color(bak_diffuse_);
  node_->set_specular_color(bak_specular_);
  node_->set_shininess(bak_shine_);
  node_->set_point_size(bak_point_size_);
  node_->set_line_width(bak_line_width_);
  node_->set_round_points(bak_round_points_);
  node_->set_line_smooth(bak_line_smooth_);

  if(bak_backfaceCulling_)
    node_->enable_backface_culling();
  else
    node_->disable_backface_culling();

  if(bak_alphaTest_)
    node_->enable_alpha_test( bak_alphaValue_ );
  else
    node_->disable_alpha_test();

  if(bak_blending_)
    node_->enable_blending( bak_blendParam1_, bak_blendParam2_ );
  else
    node_->disable_blending();

  if ( bak_colorMaterial_ )
    node_->enable_color_material();
  else
    node_->disable_color_material();

  setButtonColor( ui_.diffuseColorButton, diffuse_ );
  setButtonColor( ui_.ambientColorButton, ambient_ );
  setButtonColor( ui_.specularColorButton, specular_ );
  setButtonColor( ui_.baseColorButton, color_ );

  emit signalNodeChanged(node_);
}


//-----------------------------------------------------------------------------


void QtMaterialDialog::changeDiffuseColor()
{
  bool* ok = 0;

  QColor newColor = QColor( QColorDialog::getRgba(convertColor(diffuse_).rgba(), ok, this));

  if( newColor.isValid())
  {
    diffuse_ = convertColor( newColor );
    applyChanges();
  }
}


//-----------------------------------------------------------------------------


void QtMaterialDialog::changeAmbientColor()
{
  bool* ok = 0;

  QColor newColor = QColor( QColorDialog::getRgba(convertColor(ambient_).rgba(), ok, this));

  if( newColor.isValid())
  {
    ambient_ = convertColor( newColor );
    applyChanges();
  }
}


//-----------------------------------------------------------------------------


void QtMaterialDialog::changeSpecularColor()
{
  bool* ok = 0;

  QColor newColor = QColor( QColorDialog::getRgba(convertColor(specular_).rgba(), ok, this));

  if( newColor.isValid())
  {
    specular_ = convertColor( newColor );
    applyChanges();
  }
}


//-----------------------------------------------------------------------------


void QtMaterialDialog::changeBaseColor()
{
  bool* ok = 0;

  QColor newColor = QColor( QColorDialog::getRgba(convertColor(color_).rgba(), ok, this));

  if( newColor.isValid())
  {
    color_ = convertColor( newColor );
    applyChanges();
  }
}


//-----------------------------------------------------------------------------


void
QtMaterialDialog::changeShine(int _new)
{
  shine_      = _new;
  applyChanges();
}


//-----------------------------------------------------------------------------


void
QtMaterialDialog::changePointSize(int _new)
{
  point_size_ = _new;
  applyChanges();
}


//-----------------------------------------------------------------------------


void
QtMaterialDialog::changeLineWidth(int _new)
{
  line_width_ = _new;
  applyChanges();
}


//-----------------------------------------------------------------------------


void
QtMaterialDialog::changeRoundPoints(bool _b)
{
  round_points_ = (bool)_b;
  applyChanges();
}

//-----------------------------------------------------------------------------


void
QtMaterialDialog::changeLineSmooth(bool _b)
{
  line_smooth_ = (bool)_b;
  applyChanges();
}

//-----------------------------------------------------------------------------


void
QtMaterialDialog::changeBackfaceCulling(bool _b)
{
  backfaceCulling_ = (bool)_b;
  applyChanges();
}

//-----------------------------------------------------------------------------


void
QtMaterialDialog::changeAlphaTest(bool _b)
{
  alphaTest_ = (bool)_b;
  applyChanges();
}

//-----------------------------------------------------------------------------

void
QtMaterialDialog::changeColorMaterial(bool _b)
{
  colorMaterial_ = (bool)_b;
  applyChanges();
}

//-----------------------------------------------------------------------------

void
QtMaterialDialog::changeAlphaValue(int _new)
{
  alphaValue_ = _new / 100.0f;
  applyChanges();
}

//-----------------------------------------------------------------------------


void
QtMaterialDialog::changeBlending(bool _b)
{
  blending_ = (bool)_b;
  applyChanges();
}

//-----------------------------------------------------------------------------

void
QtMaterialDialog::changeBlendingParam1(const QString& _name)
{
  if      (_name == "GL_ZERO")                     blendParam1_ = GL_ZERO;
  else if (_name == "GL_ONE")                      blendParam1_ = GL_ONE;
  else if (_name == "GL_SRC_COLOR")                blendParam1_ = GL_SRC_COLOR;
  else if (_name == "GL_ONE_MINUS_SRC_COLOR")      blendParam1_ = GL_ONE_MINUS_SRC_COLOR;
  else if (_name == "GL_DST_COLOR")                blendParam1_ = GL_DST_COLOR;
  else if (_name == "GL_ONE_MINUS_DST_COLOR")      blendParam1_ = GL_ONE_MINUS_DST_COLOR;
  else if (_name == "GL_ONE_MINUS_SRC_ALPHA")      blendParam1_ = GL_ONE_MINUS_SRC_ALPHA;
  else if (_name == "GL_DST_ALPHA")                blendParam1_ = GL_DST_ALPHA;
  else if (_name == "GL_ONE_MINUS_DST_ALPHA")      blendParam1_ = GL_ONE_MINUS_DST_ALPHA;
  else if (_name == "GL_CONSTANT_COLOR")           blendParam1_ = GL_CONSTANT_COLOR;
  else if (_name == "GL_ONE_MINUS_CONSTANT_COLOR") blendParam1_ = GL_ONE_MINUS_CONSTANT_COLOR;
  else if (_name == "GL_CONSTANT_ALPHA")           blendParam1_ = GL_CONSTANT_ALPHA;
  else if (_name == "GL_ONE_MINUS_CONSTANT_ALPHA") blendParam1_ = GL_ONE_MINUS_CONSTANT_ALPHA;
  else{
    std::cerr << "Blending Param unknown!" << std::endl;
    return;
  }

  applyChanges();
}

//-----------------------------------------------------------------------------

void
QtMaterialDialog::changeBlendingParam2(const QString& _name)
{
  if      (_name == "GL_ZERO")                     blendParam2_ = GL_ZERO;
  else if (_name == "GL_ONE")                      blendParam2_ = GL_ONE;
  else if (_name == "GL_SRC_COLOR")                blendParam2_ = GL_SRC_COLOR;
  else if (_name == "GL_ONE_MINUS_SRC_COLOR")      blendParam2_ = GL_ONE_MINUS_SRC_COLOR;
  else if (_name == "GL_DST_COLOR")                blendParam2_ = GL_DST_COLOR;
  else if (_name == "GL_ONE_MINUS_DST_COLOR")      blendParam2_ = GL_ONE_MINUS_DST_COLOR;
  else if (_name == "GL_ONE_MINUS_SRC_ALPHA")      blendParam2_ = GL_ONE_MINUS_SRC_ALPHA;
  else if (_name == "GL_DST_ALPHA")                blendParam2_ = GL_DST_ALPHA;
  else if (_name == "GL_ONE_MINUS_DST_ALPHA")      blendParam2_ = GL_ONE_MINUS_DST_ALPHA;
  else if (_name == "GL_CONSTANT_COLOR")           blendParam2_ = GL_CONSTANT_COLOR;
  else if (_name == "GL_ONE_MINUS_CONSTANT_COLOR") blendParam2_ = GL_ONE_MINUS_CONSTANT_COLOR;
  else if (_name == "GL_CONSTANT_ALPHA")           blendParam2_ = GL_CONSTANT_ALPHA;
  else if (_name == "GL_ONE_MINUS_CONSTANT_ALPHA") blendParam2_ = GL_ONE_MINUS_CONSTANT_ALPHA;
  else{
    std::cerr << "Blending Param unknown!" << std::endl;
    return;
  }

  applyChanges();
}

//-----------------------------------------------------------------------------

void
QtMaterialDialog::changeActive(bool /*toggle*/)
{

  baseColorActive_       = ui_.baseColorActive->isChecked();
  materialActive_        = ui_.materialActive->isChecked();
  pointSizeActive_       = ui_.pointSizeActive->isChecked();
  lineWidthActive_       = ui_.lineWidthActive->isChecked();
  roundPointsActive_     = ui_.roundPointsActive->isChecked();
  lineSmoothActive_      = ui_.lineSmoothActive->isChecked();
  blendingActive_        = ui_.blendingActive->isChecked();
  backfaceCullingActive_ = ui_.backfaceCullingActive->isChecked();
  colorMaterialActive_   = ui_.colorMaterialActive->isChecked();

  applyChanges();
}

//-----------------------------------------------------------------------------

void
QtMaterialDialog::enableProperty(int /*i*/)
{
  if      (sender() == ui_.baseColorButton)     ui_.baseColorActive->setChecked( true );
  else if (sender() == ui_.ambientColorButton)  ui_.materialActive->setChecked( true );
  else if (sender() == ui_.diffuseColorButton)  ui_.materialActive->setChecked( true );
  else if (sender() == ui_.specularColorButton) ui_.materialActive->setChecked( true );
  else if (sender() == ui_.shininessSlider)     ui_.materialActive->setChecked( true );
  else if (sender() == ui_.pointSizeSpinBox)    ui_.pointSizeActive->setChecked( true );
  else if (sender() == ui_.lineWidthSpinBox)    ui_.lineWidthActive->setChecked( true );
  else if (sender() == ui_.roundPointsCheckBox) ui_.roundPointsActive->setChecked( true );
  else if (sender() == ui_.lineSmoothCheckBox)  ui_.lineSmoothActive->setChecked( true );
  else if (sender() == ui_.backfaceCulling)     ui_.backfaceCullingActive->setChecked( true );
  else if (sender() == ui_.alphaTest)           ui_.alphaTestActive->setChecked( true );
  else if (sender() == ui_.blending)            ui_.blendingActive->setChecked( true );
  else if (sender() == ui_.colorMaterial)       ui_.colorMaterialActive->setChecked( true );
}

//-----------------------------------------------------------------------------

void
QtMaterialDialog::enableProperty()
{

  if      (sender() == ui_.baseColorButton)     ui_.baseColorActive->setChecked( true );
  else if (sender() == ui_.ambientColorButton)  ui_.materialActive->setChecked( true );
  else if (sender() == ui_.diffuseColorButton)  ui_.materialActive->setChecked( true );
  else if (sender() == ui_.specularColorButton) ui_.materialActive->setChecked( true );
  else if (sender() == ui_.shininessSlider)     ui_.materialActive->setChecked( true );
  else if (sender() == ui_.pointSizeSpinBox)    ui_.pointSizeActive->setChecked( true );
  else if (sender() == ui_.lineWidthSpinBox)    ui_.lineWidthActive->setChecked( true );
  else if (sender() == ui_.roundPointsCheckBox) ui_.roundPointsActive->setChecked( true );
  else if (sender() == ui_.lineSmoothCheckBox)  ui_.lineSmoothActive->setChecked( true );
  else if (sender() == ui_.backfaceCulling)     ui_.backfaceCullingActive->setChecked( true );
  else if (sender() == ui_.alphaTest)           ui_.alphaTestActive->setChecked( true );
  else if (sender() == ui_.blending)            ui_.blendingActive->setChecked( true );
  else if (sender() == ui_.colorMaterial)       ui_.colorMaterialActive->setChecked( true );

}

//-----------------------------------------------------------------------------

QString QtMaterialDialog::paramToStr(GLenum _param)
{
  if      (_param == GL_ZERO)                     return "GL_ZERO";
  else if (_param == GL_ONE)                      return "GL_ONE";
  else if (_param == GL_SRC_COLOR)                return "GL_SRC_COLOR";
  else if (_param == GL_ONE_MINUS_SRC_COLOR)      return "GL_ONE_MINUS_SRC_COLOR";
  else if (_param == GL_DST_COLOR)                return "GL_DST_COLOR";
  else if (_param == GL_ONE_MINUS_DST_COLOR)      return "GL_ONE_MINUS_DST_COLOR";
  else if (_param == GL_ONE_MINUS_SRC_ALPHA)      return "GL_ONE_MINUS_SRC_ALPHA";
  else if (_param == GL_DST_ALPHA)                return "GL_DST_ALPHA";
  else if (_param == GL_ONE_MINUS_DST_ALPHA)      return "GL_ONE_MINUS_DST_ALPHA";
  else if (_param == GL_CONSTANT_COLOR)           return "GL_CONSTANT_COLOR";
  else if (_param == GL_ONE_MINUS_CONSTANT_COLOR) return "GL_ONE_MINUS_CONSTANT_COLOR";
  else if (_param == GL_CONSTANT_ALPHA)           return "GL_CONSTANT_ALPHA";
  else if (_param == GL_ONE_MINUS_CONSTANT_ALPHA) return "GL_ONE_MINUS_CONSTANT_ALPHA";
  else
    return "UNKOWN PARAM";
}

//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
