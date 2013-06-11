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




#include "textureProperties.hh"

#include <cfloat>

#include <QtGui>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include "ImageStorage.hh"

texturePropertiesWidget::texturePropertiesWidget(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    connect(buttonBox, SIGNAL( clicked(QAbstractButton*) ), this , SLOT ( slotButtonBoxClicked(QAbstractButton*) ) );
    connect(textureList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(textureChanged(QTreeWidgetItem*,int)) );
    connect(textureList, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(textureAboutToChange(QTreeWidgetItem*,int)) );

    //remember changes
    connect(repeatBox, SIGNAL( clicked() ), this , SLOT ( slotPropertiesChanged() ) );
    connect(clampBox, SIGNAL( clicked() ), this , SLOT ( slotPropertiesChanged() ) );
    connect(centerBox, SIGNAL( clicked() ), this , SLOT ( slotPropertiesChanged() ) );
    connect(absBox, SIGNAL( clicked() ), this , SLOT ( slotPropertiesChanged() ) );
    connect(scaleBox, SIGNAL( clicked() ), this , SLOT ( slotPropertiesChanged() ) );

    connect(max_val, SIGNAL( valueChanged(double) ), this , SLOT ( slotPropertiesChanged(double) ) );
    connect(clamp_min, SIGNAL( valueChanged(double) ), this , SLOT ( slotPropertiesChanged(double) ) );
    connect(clamp_max, SIGNAL( valueChanged(double) ), this , SLOT ( slotPropertiesChanged(double) ) );

    connect(changeImageButton, SIGNAL( clicked() ), this, SLOT( slotChangeImage() ) );


    texData_ = 0;

    #ifdef WITH_QWT
      QGridLayout* layout = new QGridLayout( originalData);

      functionPlot_ = new ACG::QwtFunctionPlot(0);

      layout->addWidget( functionPlot_ , 0,0  );

    #endif

}

void texturePropertiesWidget::show(TextureData* _texData, int _id, QString _name){

  texData_ = _texData;
  id_ = _id;

  textureList->clear();

  QTreeWidgetItem* activeItem = 0;
  
  for (uint i=0; i < texData_->textures().size(); i++)
    if ( ! texData_->textures()[i].hidden() ) {
      if (  texData_->textures()[i].type() != MULTITEXTURE ) {
        
        QTreeWidgetItem* item = 0;
        
        if ( !texData_->textures()[i].visibleName().isEmpty() )
          item = new QTreeWidgetItem((QTreeWidget*)0, QStringList( texData_->textures()[i].visibleName() ) );
        else
          item = new QTreeWidgetItem((QTreeWidget*)0, QStringList( texData_->textures()[i].name() ) );
        
        textureList->addTopLevelItem( item );
        
        if (texData_->textures()[i].enabled())
          activeItem = item;
         
      } else {
        QTreeWidgetItem* parent = 0;
        if ( !texData_->textures()[i].visibleName().isEmpty() )
          parent = new QTreeWidgetItem((QTreeWidget*)0, QStringList( texData_->textures()[i].visibleName() ) );
        else
          parent = new QTreeWidgetItem((QTreeWidget*)0, QStringList( texData_->textures()[i].name() ) );

        textureList->addTopLevelItem( parent ) ;
        for ( int j = 0 ; j < texData_->textures()[i].multiTextureList.size() ; ++j )
          textureList->addTopLevelItem( new QTreeWidgetItem(parent, QStringList(texData_->textures()[i].multiTextureList[j] )) );
        
        if (texData_->textures()[i].enabled())
          activeItem = parent;
      }
    }

    if ( textureList->invisibleRootItem()->childCount() == 0 )  {
      QMessageBox msgBox(this);
      msgBox.setText("Cannot show Properties. No Textures available!");
      msgBox.exec();
      return;
    }

  if (id_ == -1)
    textureLabel->setText("<B>Global Textures</B>");
  else
    textureLabel->setText("<B>Textures for object '" + _name + "'</B>");

  propChanged_ = false;

  if (activeItem == 0){
    
    textureList->setCurrentItem( textureList->topLevelItem(0) );
    textureChanged( textureList->topLevelItem(0), 0 );
    
  } else {
    textureList->setCurrentItem( activeItem );
    textureChanged( activeItem, 0 );
  }
  
  QDialog::show();
}

void texturePropertiesWidget::textureAboutToChange(QTreeWidgetItem* _item, int _column){

  if (propChanged_){
    QMessageBox msgBox(this);
    msgBox.setText("The properties of the current texture have been changed.");
    msgBox.setInformativeText("Do you want to apply these changes?");
    msgBox.setStandardButtons(QMessageBox::Apply | QMessageBox::Discard );
    msgBox.setDefaultButton(QMessageBox::Apply);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Apply){
      //just hit the applyButton ;)
      for (int i=0; i < buttonBox->buttons().count(); i++)
        if ( buttonBox->standardButton( buttonBox->buttons()[i] ) == QDialogButtonBox::Apply )
          slotButtonBoxClicked( buttonBox->buttons()[i] );

      textureList->setCurrentItem( _item );
      textureChanged( _item,_column );

    } else {
      propChanged_ = false;

      textureList->setCurrentItem( _item );
      textureChanged( _item,_column );
    }
  }
}

void texturePropertiesWidget::textureChanged(QTreeWidgetItem* _item, int _column){

  // ================================================================================
  // opened changes for the last texture so switch back
  // ================================================================================
  if ( propChanged_ ){
    textureList->setCurrentItem( curItem_ );
    return;
  }

  // ================================================================================
  // Unable to find the right texture
  // ================================================================================
  if ( !texData_->textureExists( _item->text(_column) ) )
    return;

  // ================================================================================
  // Set name of the texture
  // ================================================================================
  textureName_ = _item->text(_column);

  // ================================================================================
  // Get Object to parse Properties
  // ================================================================================
//   BaseObjectData* obj;
//   if ( PluginFunctions::getObject(  id_ , obj ) ) {
//     if( obj->dataType( DATA_TRIANGLE_MESH ) ){
//       TriMesh* mesh = PluginFunctions::triMeshObject(obj)->mesh();
//       std::string fprops;
//       mesh->fprop_stats(fprops);
//       QString facePropertyString(fprops.c_str());
//       QStringList faceProperties = facePropertyString.split(QRegExp("\n"));
//
//       std::cerr << "Got : \n" ;
//       for ( int i = 0 ; i < faceProperties.size(); ++i ) {
//         faceProperties[i] = faceProperties[i].trimmed();
//         if ( ( ! faceProperties[i].size() == 0 ) && faceProperties[i] != "<fprop>" )
//           std::cerr << faceProperties[i].toStdString() << std::endl;
//         else
//           continue;
//
//         OpenMesh::FPropHandleT< int > indexPropertyTest;
//         if ( mesh->get_property_handle(indexPropertyTest,faceProperties[i].toStdString()) ) {
//           std::cerr << "Got handle : " << faceProperties[i].toStdString() << std::endl;
//         } else {
//           std::cerr << "Unable to get Handle : " << faceProperties[i].toStdString() << std::endl;
//         }
//       }
//
//     } else if( obj->dataType( DATA_POLY_MESH ) ){
//       PolyMesh* mesh = PluginFunctions::polyMeshObject(obj)->mesh();
//     }
//   }

  // ================================================================================
  // Update the dialog
  // ================================================================================
  Texture& texture = texData_->texture(textureName_);

  repeatBox->setChecked(texture.parameters.repeat);
  clampBox->setChecked(texture.parameters.clamp);
  centerBox->setChecked(texture.parameters.center);
  absBox->setChecked(texture.parameters.abs);

  max_val->setValue( texture.parameters.repeatMax );
  clamp_min->setValue( texture.parameters.clampMin );
  clamp_max->setValue( texture.parameters.clampMax );

  switch (texture.type()) {
    case MULTITEXTURE:
      typeLabel->setText("Type:   MultiTexture");
      indexLabel->setEnabled(true);
      indexBox->setEnabled(true);
      indexBox->clear();
      indexBox->addItem("TODO");
      break;
    case HALFEDGEBASED:
      typeLabel->setText("Type:   HalfedgeBased");
      indexLabel->setEnabled(false);
      indexBox->setEnabled(false);
      indexBox->clear();
      break;
    case VERTEXBASED:
      typeLabel->setText("Type:   VertexBased");
      indexLabel->setEnabled(false);
      indexBox->setEnabled(false);
      indexBox->clear();
      break;
    case ENVIRONMENT:
      typeLabel->setText("Type:   Environment Map");
      indexLabel->setEnabled(false);
      indexBox->setEnabled(false);
      indexBox->clear();
      break;
    case UNSET:
      typeLabel->setText("Type:   Unset");
      indexLabel->setEnabled(false);
      indexBox->setEnabled(false);
      indexBox->clear();
      break;
  }

  // Show the texture Image
  bool ok = false;
  imageLabel->setPixmap(QPixmap::fromImage( imageStore().getImage(texture.textureImageId(),&ok) ));

  if ( !ok ) {
    std::cerr<< imageStore().error().toStdString();
  }

  imageLabel->setScaledContents(true);

  if ( texture.filename().startsWith("/") )
    fileLabel->setText( "File: " + texture.filename() );
  else
    fileLabel->setText( "File: " + OpenFlipper::Options::textureDirStr() + QDir::separator() + texture.filename() );

  currentImage_ = texture.filename();

  // update plot only when dimension is 1
  if ( texture.dimension() == 1 && id_ != -1) {

    #ifdef WITH_QWT

      std::vector< double > coords;

      emit getCoordinates1D(textureName_, id_, coords);

      if ( ! coords.empty() ){

        functionPlot_->setFunction( coords );

        functionPlot_->setParameters(repeatBox->isChecked(), max_val->value(),
                                     clampBox->isChecked(),  clamp_min->value(), clamp_max->value(),
                                     centerBox->isChecked(),
                                     absBox->isChecked(),
                                     scaleBox->isChecked());

        bool ok = false;
        image_ = imageStore().getImage(texture.textureImageId(),&ok);
        if ( !ok ) {
          std::cerr << imageStore().error().toStdString();
        }


        functionPlot_->setImage( &image_ );

        functionPlot_->replot();
      }

    #endif
  }

  propChanged_ = false;
  curItem_ = textureList->currentItem();

}

void texturePropertiesWidget::slotChangeImage()
{

  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open Image"),
                                                  OpenFlipper::Options::currentTextureDirStr(),
                                                  tr("Images (*.png *.xpm *.jpg, *.tga)"));

  if (QFile(fileName).exists()) {
    QFileInfo fileInfo(fileName);
    OpenFlipper::Options::currentTextureDir(fileInfo.absolutePath());

    imageLabel->setPixmap(fileName);
    imageLabel->setScaledContents(true);

    fileLabel->setText("File: " + fileName);

    currentImage_ = fileName;
    image_ = imageLabel->pixmap()->toImage();

    #ifdef WITH_QWT
      functionPlot_->setImage(&image_);
      functionPlot_->replot();
    #endif

    propChanged_ = true;
  }

}

void texturePropertiesWidget::slotButtonBoxClicked(QAbstractButton* _button){

  QDialogButtonBox::StandardButton btn = buttonBox->standardButton(_button);

  if ( btn == QDialogButtonBox::Apply || btn == QDialogButtonBox::Ok){

    //applySettings
    bool changed = false;

    Texture& texture = texData_->texture(textureName_);

    if ( texture.parameters.repeat != repeatBox->isChecked() ){
      texture.parameters.repeat=repeatBox->isChecked();
      changed = true;
    }
    if ( texture.parameters.clamp != clampBox->isChecked() ){
      texture.parameters.clamp=clampBox->isChecked();
      changed = true;
    }
    if ( texture.parameters.center != centerBox->isChecked() ){
      texture.parameters.center=centerBox->isChecked();
      changed = true;
    }
    if ( texture.parameters.abs != absBox->isChecked() ){
      texture.parameters.abs=absBox->isChecked();
      changed = true;
    }
    if ( texture.parameters.scale != scaleBox->isChecked() ){
      texture.parameters.scale=scaleBox->isChecked();
      changed = true;
    }

    if ( texture.parameters.repeatMax != max_val->value() ){
      texture.parameters.repeatMax = max_val->value();
      changed = true;
    }

    if ( texture.parameters.clampMin != clamp_min->value() ){
      texture.parameters.clampMin = clamp_min->value();
      changed = true;
    }

    if ( texture.parameters.clampMax != clamp_max->value() ){
      texture.parameters.clampMax = clamp_max->value();
      changed = true;
    }

    if ( texture.filename() != currentImage_ ){
      // Set the new filename of the image
      texture.filename( currentImage_ );

      // Add it to the imagestore and set the index in the texture description
      texture.textureImageId( imageStore().addImageFile(currentImage_) );

      changed = true;
    }

    //inform the plugin about the update
    if (changed)
      emit applyProperties(texData_, textureName_, id_ );

    propChanged_ = false;
  }

  if ( btn == QDialogButtonBox::Apply )
    return;
  else
    hide();
}

void texturePropertiesWidget::slotPropertiesChanged(double /*_value*/){
  propChanged_ = true;

  #ifdef WITH_QWT
  functionPlot_->setParameters(repeatBox->isChecked(), max_val->value(),
                              clampBox->isChecked(),  clamp_min->value(), clamp_max->value(),
                              centerBox->isChecked(),
                              absBox->isChecked(),
                              scaleBox->isChecked());

  functionPlot_->replot();
  #endif
}
