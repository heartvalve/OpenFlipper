/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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

#include <QtGui>

#include "ColorPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>


//-----------------------------------------------------------------------------

/** \brief Constructor
 *
 */
ColorPlugin::ColorPlugin() : optionsWidget_(0)
{

  //set default values for colors

  backgroundColorDefault_      = ACG::GLState::default_clear_color;
  backgroundColorPaper_        = OpenMesh::Vec4f(1.00, 1.00, 1.00, 1.0);
  backgroundColorPresentation_ = OpenMesh::Vec4f(0.0, 0.188, 0.062, 1.0);
  baseColorDefault_      = ACG::GLState::default_base_color;
  baseColorPaper_        = OpenMesh::Vec4f(0.00, 0.00, 0.00, 1.0);
  baseColorPresentation_ = OpenMesh::Vec4f(0.88, 0.88,  0.188, 1.0);
  materialColorDefault_      = ACG::GLState::default_diffuse_color;
  materialColorPaper_        = OpenMesh::Vec4f(0.12, 0.76, 0.62, 1.0);
  materialColorPresentation_ = OpenMesh::Vec4f(0.941, 0.439, 0.31, 1.0);

  lineWidthDefault_      = 1.0;
  lineWidthPaper_        = 2.0;
  lineWidthPresentation_ = 1.0;

}


//-----------------------------------------------------------------------------

/** \brief initialize the toolbar and menubar
 *
 */
void ColorPlugin::pluginsInitialized()
{
  // =============================================================================
  // Add a Menu for color selection
  // =============================================================================
  QMenu *colorMenu = new QMenu(tr("&Color schemes"));
  QIcon icon;
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"format-stroke-color.png");
  colorMenu->setIcon(icon);
  emit addMenubarAction(colorMenu->menuAction(), VIEWMENU );

  // =============================================================================
  // Add Menu entries
  // =============================================================================

  QAction* AC_set_Default_color = new QAction(tr("&Default colors"), this);
  AC_set_Default_color->setStatusTip(tr("Set Default Colors"));
  AC_set_Default_color->setWhatsThis(tr("Set colors to default colors"));
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"color-default.png");
  AC_set_Default_color->setIcon(icon);
  connect(AC_set_Default_color, SIGNAL(triggered()), this, SLOT(setDefaultColor()));
  colorMenu->addAction(AC_set_Default_color);

  QAction* AC_set_Presentation_color = new QAction(tr("&Presentation colors"), this);
  AC_set_Presentation_color->setStatusTip(tr("Set Presentation Colors"));
  AC_set_Presentation_color->setWhatsThis(tr("Set colors to presentation colors"));
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"color-powerpoint.png");
  AC_set_Presentation_color->setIcon(icon);
  connect(AC_set_Presentation_color, SIGNAL(triggered()), this, SLOT(setPresentationColor()));
  colorMenu->addAction(AC_set_Presentation_color);

  QAction* AC_set_Paper_color = new QAction(tr("&Paper colors"), this);
  AC_set_Paper_color->setStatusTip(tr("Set Paper Colors"));
  AC_set_Paper_color->setWhatsThis(tr("Set colors to colors for papers"));
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"color-paper.png");
  AC_set_Paper_color->setIcon(icon);
  connect(AC_set_Paper_color, SIGNAL(triggered()), this, SLOT(setPaperColor()));
  colorMenu->addAction(AC_set_Paper_color);

  // =============================================================================
  // Create Tool bar with same contents
  // =============================================================================

  QToolBar* toolbar = new QToolBar(tr("Color Toolbar"));
  toolbar->addAction(AC_set_Default_color);
  toolbar->addAction(AC_set_Presentation_color);
  toolbar->addAction(AC_set_Paper_color);
  emit addToolbar( toolbar );

}


//-----------------------------------------------------------------------------

/** \brief set the defaultColor preset
 *
 */
void ColorPlugin::setDefaultColor() {

  // =============================================================================
  // Give user feedback
  // =============================================================================
  emit scriptInfo( "setDefaultColor()" );
  emit showStatusMessage( tr("Set to default Colors"), 4000 );

  // =============================================================================
  // Define the new colors
  // =============================================================================
  OpenMesh::Vec4f ambient_color,
                  diffuse_color,
                  specular_color;

  ambient_color  = 0.2f * materialColorDefault_;
  diffuse_color  = 0.6f * materialColorDefault_;
  specular_color = 0.8f * materialColorDefault_;
  ambient_color[3] = 1.0f;
  diffuse_color[3] = 1.0f;
  specular_color[3] = 1.0f;

  // Set background color
  PluginFunctions::setBackColor( backgroundColorDefault_ );

  // =============================================================================
  // Apply to all objects
  // =============================================================================
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    MaterialNode* mat = o_it->materialNode();
    mat->set_base_color( baseColorDefault_ );
    mat->set_ambient_color(ambient_color);
    mat->set_diffuse_color(diffuse_color);
    mat->set_specular_color(specular_color);
    mat->set_line_width( lineWidthDefault_ );
  }

  emit updateView();
}


//-----------------------------------------------------------------------------

/** \brief Set the presentation color preset
 *
 */
void ColorPlugin::setPresentationColor() {

  // =============================================================================
  // Give user feedback
  // =============================================================================
  emit scriptInfo( "setPresentationColor()" );
  emit showStatusMessage( tr("Set to Presentation Colors"), 4000 );

  // =============================================================================
  // Define the new colors
  // =============================================================================
  OpenMesh::Vec4f ambient_color,
                  diffuse_color,
                  specular_color;

  ambient_color  = 0.2f * materialColorPresentation_;
  diffuse_color  = 0.6f * materialColorPresentation_;
  specular_color = 0.8f * materialColorPresentation_;
  ambient_color[3] = 1.0f;
  diffuse_color[3] = 1.0f;
  specular_color[3] = 1.0f;

  // Set background color
  PluginFunctions::setBackColor( backgroundColorPresentation_ );

  // =============================================================================
  // Apply to all objects
  // =============================================================================
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    MaterialNode* mat = o_it->materialNode();
    mat->set_base_color( baseColorPresentation_ );
    mat->set_ambient_color(ambient_color);
    mat->set_diffuse_color(diffuse_color);
    mat->set_specular_color(specular_color);
    mat->set_line_width( lineWidthPresentation_ );
  }

  emit updateView();
}


//-----------------------------------------------------------------------------

/** \brief set the paperColor Preset
 *
 */
void ColorPlugin::setPaperColor() {

  // =============================================================================
  // Give user feedback
  // =============================================================================
  emit scriptInfo( "setPaperColor()" );
  emit showStatusMessage( tr("Set to Paper Colors"), 4000 );

  // =============================================================================
  // Define the new colors
  // =============================================================================
  OpenMesh::Vec4f ambient_color,
                  diffuse_color,
                  specular_color;

  ambient_color  = 0.2f * materialColorPaper_;
  diffuse_color  = 0.5f * materialColorPaper_;
  specular_color = 0.8f * materialColorPaper_;
  ambient_color[3] = 1.0f;
  diffuse_color[3] = 1.0f;
  specular_color[3] = 1.0f;

  // Set background color
  PluginFunctions::setBackColor( backgroundColorPaper_ );

  // =============================================================================
  // Apply to all objects
  // =============================================================================
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    MaterialNode* mat = o_it->materialNode();
    mat->set_base_color( baseColorPaper_ );
    mat->set_ambient_color(ambient_color);
    mat->set_diffuse_color(diffuse_color);
    mat->set_specular_color(specular_color);
    mat->set_line_width( lineWidthPaper_ );
  }

  emit updateView();
}


//-----------------------------------------------------------------------------

/** \brief Set the backgroundColor
 *
 * @param _color a color
 */
void ColorPlugin::setBackgroundColor( Vector _color ) {
  emit scriptInfo( "setBackgroundColor()" );
  emit showStatusMessage( tr("Set background color"), 4000 );

  ACG::Vec4f color;
  color[0] = _color[0];
  color[1] = _color[1];
  color[2] = _color[2];
  color[3] = 1.0;

  PluginFunctions::setBackColor(color);
}


//-----------------------------------------------------------------------------

/** \brief Generate an icon with given color
 *
 * @param _color a color vector
 * @return an icon
 */
QIcon ColorPlugin::generateIcon(OpenMesh::Vec4f _color){

  QColor color;
  color.setRgbF(_color[0], _color[1], _color[2], _color[3] );

  QPixmap pic(32,32);
  pic.fill( color );

  return QIcon( pic );
}


//-----------------------------------------------------------------------------

/** \brief Generate an icon with given color
 *
 * @param _color a color
 * @return the icon
 */
QIcon ColorPlugin::generateIcon(QRgb _color){

  QColor color(_color);

  QPixmap pic(32,32);
  pic.fill( color );

  return QIcon( pic );
}


//-----------------------------------------------------------------------------

/** \brief Get the color of an icon
 *
 * @param _icon an icon
 * @return its color
 */
QRgb ColorPlugin::getIconColor(QIcon _icon){

  QRgb rgb = _icon.pixmap(32,32).toImage().pixel(0,0);

  return rgb;
}


//-----------------------------------------------------------------------------

/** \brief convert an icons color to a Vec4f
 *
 * @param _icon an icon
 * @param _color reference to a color vector
 */
void ColorPlugin::setNewColor(QIcon _icon, OpenMesh::Vec4f& _color){

  QColor color( getIconColor(_icon) );

  _color[0] = color.redF();
  _color[1] = color.greenF();
  _color[2] = color.blueF();
  _color[3] = color.alphaF();
}

//-----------------------------------------------------------------------------

/** \brief show a colorDialog and store the chosen value in the icon of the sender()
 *
 */
void ColorPlugin::getColorDialog(){

  QPushButton* button = dynamic_cast< QPushButton* > ( sender() );

  if (button != 0){

    bool ok;

    QRgb newColor = QColorDialog::getRgba ( getIconColor( button->icon() ) , &ok );

    if (ok)
      button->setIcon( generateIcon(newColor) );

  }
}


//-----------------------------------------------------------------------------

/** \brief generate an optionsWidget to change color presets
 *
 * @param _widget reference to the generated widget
 * @return return true if the widget was successfully generated
 */
bool ColorPlugin::initializeOptionsWidget(QWidget*& _widget){

  if (optionsWidget_ == 0){

    optionsWidget_ = new ColorOptions();

    //connect the signals
    connect(optionsWidget_->backgroundDefault,      SIGNAL( clicked() ), this, SLOT( getColorDialog() ) );
    connect(optionsWidget_->backgroundPaper,        SIGNAL( clicked() ), this, SLOT( getColorDialog() ) );
    connect(optionsWidget_->backgroundPresentation, SIGNAL( clicked() ), this, SLOT( getColorDialog() ) );

    connect(optionsWidget_->baseDefault,      SIGNAL( clicked() ), this, SLOT( getColorDialog() ) );
    connect(optionsWidget_->basePaper,        SIGNAL( clicked() ), this, SLOT( getColorDialog() ) );
    connect(optionsWidget_->basePresentation, SIGNAL( clicked() ), this, SLOT( getColorDialog() ) );

    connect(optionsWidget_->materialDefault,      SIGNAL( clicked() ), this, SLOT( getColorDialog() ) );
    connect(optionsWidget_->materialPaper,        SIGNAL( clicked() ), this, SLOT( getColorDialog() ) );
    connect(optionsWidget_->materialPresentation, SIGNAL( clicked() ), this, SLOT( getColorDialog() ) );
  }

  //put values into the optionsWidget
  optionsWidget_->backgroundDefault->setIcon( generateIcon( backgroundColorDefault_ ) );
  optionsWidget_->backgroundPaper->setIcon( generateIcon( backgroundColorPaper_ ) );
  optionsWidget_->backgroundPresentation->setIcon( generateIcon( backgroundColorPresentation_ ) );

  optionsWidget_->baseDefault->setIcon( generateIcon( baseColorDefault_ ) );
  optionsWidget_->basePaper->setIcon( generateIcon( baseColorPaper_ ) );
  optionsWidget_->basePresentation->setIcon( generateIcon( baseColorPresentation_ ) );

  optionsWidget_->materialDefault->setIcon( generateIcon( materialColorDefault_ ) );
  optionsWidget_->materialPaper->setIcon( generateIcon( materialColorPaper_ ) );
  optionsWidget_->materialPresentation->setIcon( generateIcon( materialColorPresentation_ ) );

  optionsWidget_->lineDefault->setValue( lineWidthDefault_ );
  optionsWidget_->linePaper->setValue( lineWidthPaper_ );
  optionsWidget_->linePresentation->setValue( lineWidthPresentation_ );

  _widget = optionsWidget_;

  return true;
}


//-----------------------------------------------------------------------------

/** \brief Store the changed color presets when the apply Button was hit
 *
 */
void ColorPlugin::applyOptions(){

  setNewColor(optionsWidget_->backgroundDefault->icon(),      backgroundColorDefault_);
  setNewColor(optionsWidget_->backgroundPaper->icon(),        backgroundColorPaper_);
  setNewColor(optionsWidget_->backgroundPresentation->icon(), backgroundColorPresentation_);

  setNewColor(optionsWidget_->baseDefault->icon(),      baseColorDefault_);
  setNewColor(optionsWidget_->basePaper->icon(),        baseColorPaper_);
  setNewColor(optionsWidget_->basePresentation->icon(), baseColorPresentation_);

  setNewColor(optionsWidget_->materialDefault->icon(),      materialColorDefault_);
  setNewColor(optionsWidget_->materialPaper->icon(),        materialColorPaper_);
  setNewColor(optionsWidget_->materialPresentation->icon(), materialColorPresentation_);

  lineWidthDefault_      = optionsWidget_->lineDefault->value();
  lineWidthPaper_        = optionsWidget_->linePaper->value();
  lineWidthPresentation_ = optionsWidget_->linePresentation->value();
}


//-----------------------------------------------------------------------------

/** \brief convert between uint and Vec4f
 *
 * @param _color given color
 * @param _vector color vector to convert to
 */
void ColorPlugin::convertColor(uint _color, OpenMesh::Vec4f& _vector){

  QColor color;
  color.setRgba( _color );

  _vector[0] = color.redF();
  _vector[1] = color.greenF();
  _vector[2] = color.blueF();
  _vector[3] = color.alphaF();
}


//-----------------------------------------------------------------------------

/** \brief convert between uint and Vec4f
 *
 * @param _vector reference to a color vector
 * @return same color as uint
 */
uint ColorPlugin::convertColor(OpenMesh::Vec4f& _vector){

  QColor color;
  color.setRgbF(_vector[0], _vector[1], _vector[2], _vector[3] );

  return (uint) color.rgba();
}


//-----------------------------------------------------------------------------

/** \brief read color preset from ini file
 *
 * @param _ini file to read from
 */
void ColorPlugin::loadIniFileOptions(INIFile& _ini){

  if ( _ini.section_exists("ColorPlugin") ){

    uint color = 0;

    if ( _ini.get_entry( color, "ColorPlugin" , "backgroundColorDefault") )
      convertColor( color, backgroundColorDefault_ );
    if ( _ini.get_entry( color, "ColorPlugin" , "backgroundColorPaper") )
      convertColor( color, backgroundColorPaper_ );
    if ( _ini.get_entry( color, "ColorPlugin" , "backgroundColorPresentation") )
      convertColor( color, backgroundColorPresentation_ );

    if ( _ini.get_entry( color, "ColorPlugin" , "baseColorDefault") )
      convertColor( color, baseColorDefault_ );
    if ( _ini.get_entry( color, "ColorPlugin" , "baseColorPaper") )
      convertColor( color, baseColorPaper_ );
    if ( _ini.get_entry( color, "ColorPlugin" , "baseColorPresentation") )
      convertColor( color, baseColorPresentation_ );

    if ( _ini.get_entry( color, "ColorPlugin" , "materialColorDefault") )
      convertColor( color, materialColorDefault_ );
    if ( _ini.get_entry( color, "ColorPlugin" , "materialColorPaper") )
      convertColor( color, materialColorPaper_ );
    if ( _ini.get_entry( color, "ColorPlugin" , "materialColorPresentation") )
      convertColor( color, materialColorPresentation_ );

    double width;

    if ( _ini.get_entry( width, "ColorPlugin" , "lineWidthDefault") )
      lineWidthDefault_ = width;
    if ( _ini.get_entry( width, "ColorPlugin" , "lineWidthPaper") )
      lineWidthPaper_ = width;
    if ( _ini.get_entry( width, "ColorPlugin" , "lineWidthPresentation") )
      lineWidthPresentation_ = width;

  }
}


//-----------------------------------------------------------------------------

/** \brief Store current color preset to an ini file
 *
 * @param _ini file to store color presets in
 */
void ColorPlugin::saveIniFileOptions(INIFile& _ini){
std::cerr << "save to ini file\n";


  _ini.add_entry("ColorPlugin","backgroundColorDefault",      convertColor( backgroundColorDefault_ )  );
  _ini.add_entry("ColorPlugin","backgroundColorPaper",        convertColor( backgroundColorPaper_ )  );
  _ini.add_entry("ColorPlugin","backgroundColorPresentation", convertColor( backgroundColorPresentation_ )  );

  _ini.add_entry("ColorPlugin","baseColorDefault",      convertColor( baseColorDefault_ )  );
  _ini.add_entry("ColorPlugin","baseColorPaper",        convertColor( baseColorPaper_ )  );
  _ini.add_entry("ColorPlugin","baseColorPresentation", convertColor( baseColorPresentation_ )  );

  _ini.add_entry("ColorPlugin","materialColorDefault",      convertColor( materialColorDefault_ )  );
  _ini.add_entry("ColorPlugin","materialColorPaper",        convertColor( materialColorPaper_ )  );
  _ini.add_entry("ColorPlugin","materialColorPresentation", convertColor( materialColorPresentation_ )  );

  _ini.add_entry("ColorPlugin","lineWidthDefault",      lineWidthDefault_ );
  _ini.add_entry("ColorPlugin","lineWidthPaper",        lineWidthPaper_   );
  _ini.add_entry("ColorPlugin","lineWidthPresentation", lineWidthPresentation_ );
}


Q_EXPORT_PLUGIN2( colorplugin , ColorPlugin );

