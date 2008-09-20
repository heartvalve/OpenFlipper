 
#include <QtGui>

#include "ColorPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh> 

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

void ColorPlugin::pluginsInitialized()
{
  // =============================================================================
  // Add a Menu for color selection
  // =============================================================================
  QMenu *colorMenu = new QMenu(tr("&Colors"));
  QIcon icon;
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"format-stroke-color.png");
  colorMenu->setIcon(icon);  
  emit addMenu( colorMenu, VIEWMENU );
  
  // =============================================================================
  // Add Menu entries
  // =============================================================================
  
  QAction* AC_set_Default_color = new QAction(tr("&Default colors"), this);
  AC_set_Default_color->setStatusTip(tr("Set Default Colors"));
  AC_set_Default_color->setWhatsThis("Set colors to default colors");
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"flag.png");
  AC_set_Default_color->setIcon(icon);
  connect(AC_set_Default_color, SIGNAL(triggered()), this, SLOT(setDefaultColor()));
  colorMenu->addAction(AC_set_Default_color);
  
  QAction* AC_set_Powerpoint_color = new QAction(tr("&Powerpoint colors"), this);
  AC_set_Powerpoint_color->setStatusTip(tr("Set Powerpoint Colors"));
  AC_set_Powerpoint_color->setWhatsThis("Set colors to Powerpoint presentation colors");
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"video-projector.png");
  AC_set_Powerpoint_color->setIcon(icon);  
  connect(AC_set_Powerpoint_color, SIGNAL(triggered()), this, SLOT(setPowerpointColor()));
  colorMenu->addAction(AC_set_Powerpoint_color);
  
  QAction* AC_set_Paper_color = new QAction(tr("&Paper colors"), this);
  AC_set_Paper_color->setStatusTip(tr("Set Paper Colors"));
  AC_set_Paper_color->setWhatsThis("Set colors to colors for papers");
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"view-pim-news.png");
  AC_set_Paper_color->setIcon(icon);
  connect(AC_set_Paper_color, SIGNAL(triggered()), this, SLOT(setPaperColor()));
  colorMenu->addAction(AC_set_Paper_color);
  
  // =============================================================================
  // Create symbol bar with same contents
  // =============================================================================  
  
  QToolBar* toolbar = new QToolBar("Color Toolbar");
  toolbar->addAction(AC_set_Default_color);
  toolbar->addAction(AC_set_Powerpoint_color);
  toolbar->addAction(AC_set_Paper_color);
  emit addToolbar( toolbar );
   
}

void ColorPlugin::setDefaultColor() {
  
  // =============================================================================
  // Give user feedback
  // =============================================================================
  emit scriptInfo( "setDefaultColor()" );
  emit showStatusMessage( "Set to default Colors", 4000 );
  
  // =============================================================================
  // Define the new colors
  // =============================================================================    
  OpenMesh::Vec4f back_color, 
                  base_color,
                  ambient_color, 
                  diffuse_color, 
                  specular_color,
                  mat_color;
  float           line_width(1.0);
  
  back_color     = ACG::GLState::default_clear_color;
  base_color     = ACG::GLState::default_base_color;
  ambient_color  = ACG::GLState::default_ambient_color;
  diffuse_color  = ACG::GLState::default_diffuse_color;
  specular_color = ACG::GLState::default_specular_color;
  line_width     = 1.0;
  
  // Set background color
  PluginFunctions::setBackColor(back_color);

  // =============================================================================
  // Apply to all objects
  // =============================================================================    
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objects_end(); ++o_it) {
    MaterialNode* mat = o_it->materialNode();
    mat->set_base_color(base_color);
    mat->set_ambient_color(ambient_color);
    mat->set_diffuse_color(diffuse_color);
    mat->set_specular_color(specular_color);
    mat->set_line_width(line_width);       
  }
  
  emit update_view();
}

void ColorPlugin::setPowerpointColor() {
  
  // =============================================================================
  // Give user feedback
  // =============================================================================
  emit scriptInfo( "setPowerpointColor()" );
  emit showStatusMessage( "Set to Powerpoint Colors", 4000 );
  
  // =============================================================================
  // Define the new colors
  // ============================================================================= 
  OpenMesh::Vec4f back_color, 
                  base_color, 
                  ambient_color, 
                  diffuse_color, 
                  specular_color,
                  mat_color;
  float           line_width(1.0);

  back_color     = OpenMesh::Vec4f(0.0, 0.188, 0.062, 1.0);
  base_color     = OpenMesh::Vec4f(0.88, 0.88,  0.188, 1.0);
  mat_color      = OpenMesh::Vec4f(0.941, 0.439, 0.31, 1.0);
  ambient_color  = 0.2f * mat_color;
  diffuse_color  = 0.6f * mat_color;
  specular_color = 0.8f * mat_color;
  line_width     = 1.0;
  
  // Set background color
  PluginFunctions::setBackColor(back_color);

  // =============================================================================
  // Apply to all objects
  // =============================================================================
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objects_end(); ++o_it) {
    MaterialNode* mat = o_it->materialNode();
    mat->set_base_color(base_color);
    mat->set_ambient_color(ambient_color);
    mat->set_diffuse_color(diffuse_color);
    mat->set_specular_color(specular_color);
    mat->set_line_width(line_width);      
  }
  
  emit update_view();
}

void ColorPlugin::setPaperColor() {
  
  // =============================================================================
  // Give user feedback
  // =============================================================================
  emit scriptInfo( "setPaperColor()" );
  emit showStatusMessage( "Set to Paper Colors", 4000 );
  
  // =============================================================================
  // Define the new colors
  // ============================================================================= 
  OpenMesh::Vec4f back_color, 
                  base_color, 
                  ambient_color, 
                  diffuse_color, 
                  specular_color,
                  mat_color;
  float     line_width(1.0);
  
  back_color     = OpenMesh::Vec4f(1.00, 1.00, 1.00, 1.0);
  base_color     = OpenMesh::Vec4f(0.00, 0.00, 0.00, 1.0);
  mat_color      = OpenMesh::Vec4f(0.12, 0.76, 0.62, 1.0);
  ambient_color  = 0.2f * mat_color;
  diffuse_color  = 0.5f * mat_color;
  specular_color = 0.8f * mat_color;
  line_width     = 2.0;
  
  // Set background color
  PluginFunctions::setBackColor(back_color);
    
  // =============================================================================
  // Apply to all objects
  // =============================================================================
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objects_end(); ++o_it) {
    MaterialNode* mat = o_it->materialNode();
    mat->set_base_color(base_color);
    mat->set_ambient_color(ambient_color);
    mat->set_diffuse_color(diffuse_color);
    mat->set_specular_color(specular_color);
    mat->set_line_width(line_width);
  }
  
  emit update_view();
}

//-----------------------------------------------------------------------------

void ColorPlugin::setBackgroundColor( Vector _color ) {
  emit scriptInfo( "setBackgroundColor()" );
  emit showStatusMessage( "Set background color", 4000 );
  
  ACG::Vec4f color;
  color[0] = _color[0];
  color[1] = _color[1];
  color[2] = _color[2];
  color[3] = 1.0;
  
  PluginFunctions::setBackColor(color);
}


Q_EXPORT_PLUGIN2( colorplugin , ColorPlugin );

