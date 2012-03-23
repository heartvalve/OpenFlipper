//================================================================
//
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

//================================================================
//
//  CLASS FilePTSPlugin - IMPLEMENTATION
//
//================================================================
//== INCLUDES ====================================================
#include "FilePTS.hh"

#include <QtGui>
#include <QFileInfo>
#include <QSettings>

#include <fstream>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <OpenMesh/Core/IO/IOManager.hh>

//== CONSTANTS ===================================================

// constants of color range drop down box
static const int COLORRANGE_0_1 = 0;
static const int COLORRANGE_0_255 = 1;

//== IMPLEMENTATION ==============================================

FilePTSPlugin::FilePTSPlugin() :
        loadOptions_(0),
        saveOptions_(0),
        loadBinaryFile_(0),
        loadNormals_(0),
        loadPointsizes_(0),
        loadColors_(0),
        loadColorRange_(0),
        loadNormalizeSize_(0),
        saveBinaryFile_(0),
        saveNormals_(0),
        savePointsizes_(0),
        saveColors_(0),
        saveColorRange_(0),
        loadMakeDefaultButton_(0),
        saveMakeDefaultButton_(0)

{

}


bool FilePTSPlugin::readBinaryFile( std::ifstream &_instream, SplatCloud *_splatCloud )
{
  // set default options
  bool loadNormals = true;
  bool loadPointsizes = false;
  bool loadColors = false;
//	int  loadColorRange = 0;

// get options
  if (OpenFlipper::Options::gui() && loadOptions_) {
    loadNormals = loadNormals_->isChecked();
    loadPointsizes = loadPointsizes_->isChecked();
    loadColors = loadColors_->isChecked();
//		loadColorRange = loadColorRange_->currentIndex();
  }

  // read file type
  int fileType;
  _instream.read((char *) &fileType, sizeof(int));

  // check file type
  if (fileType != 1 && fileType != 2)
    return false; // return failure

  // read number of points
  unsigned int numPoints;
  _instream.read((char *) &numPoints, sizeof(unsigned int));

  // read points
  {
    unsigned int i;
    for (i = 0; i < numPoints; ++i) {
      float pnt[3];
      _instream.read((char *) pnt, 3 * sizeof(float));

      SplatCloud::Point point;
      point[0] = pnt[0];
      point[1] = pnt[1];
      point[2] = pnt[2];

      _splatCloud->addPoint(point);
    }
  }

  // read normals
  if (loadNormals) {
    unsigned int i;
    for (i = 0; i < numPoints; ++i) {
      float nrm[3];
      _instream.read((char *) nrm, 3 * sizeof(float));

      SplatCloud::Normal normal;
      normal[0] = nrm[0];
      normal[1] = nrm[1];
      normal[2] = nrm[2];

      _splatCloud->addNormal(normal);
    }
  }

  // read pointsizes
  if (loadPointsizes) {
    unsigned int i;
    for (i = 0; i < numPoints; ++i) {
      float ps;
      _instream.read((char *) &ps, sizeof(float));

      SplatCloud::Pointsize pointsize;
      pointsize = ps;

      _splatCloud->addPointsize(pointsize);
    }
  }

  // read colors
  if (loadColors) {
    unsigned int i;
    for (i = 0; i < numPoints; ++i) {
      unsigned int col;
      _instream.read((char *) &col, sizeof(unsigned int));

      SplatCloud::Color color; // ignore colorrange
      color[0] = (unsigned char) ((col >> 16) & 0xFF);
      color[1] = (unsigned char) ((col >> 8) & 0xFF);
      color[2] = (unsigned char) ((col) & 0xFF);

      _splatCloud->addColor(color);
    }
  }

  // return success
  return true;
}

//----------------------------------------------------------------

bool FilePTSPlugin::readTextFile( std::ifstream &_instream, SplatCloud *_splatCloud )
{
  // set default options
  bool loadNormals = true;
  bool loadPointsizes = false;
  bool loadColors = false;
  int loadColorRange = 0;

  // get options
  if (OpenFlipper::Options::gui() && loadOptions_) {
    loadNormals = loadNormals_->isChecked();
    loadPointsizes = loadPointsizes_->isChecked();
    loadColors = loadColors_->isChecked();
    loadColorRange = loadColorRange_->currentIndex();
  }

  char buffer[4096];

  while (true) {
    _instream.getline(buffer, 4096, '\n');
    std::string strbuffer(buffer);
    std::stringstream sstream(strbuffer);

    if (_instream.eof())
      break;

    // read point
    {
      float pnt[3];
      sstream >> pnt[0];
      sstream >> pnt[1];
      sstream >> pnt[2];

      SplatCloud::Point point;
      point[0] = pnt[0];
      point[1] = pnt[1];
      point[2] = pnt[2];

      _splatCloud->addPoint(point);
    }

    // read color
    if (loadColors) {
      float col[3];
      sstream >> col[0];
      sstream >> col[1];
      sstream >> col[2];

      SplatCloud::Color color;

      if (loadColorRange == COLORRANGE_0_1) {
        color[0] = (unsigned char) (255.999f * col[0]);
        color[1] = (unsigned char) (255.999f * col[1]);
        color[2] = (unsigned char) (255.999f * col[2]);
      } else // loadColorRange == COLORRANGE_0_255
      {
        color[0] = (unsigned char) col[0];
        color[1] = (unsigned char) col[1];
        color[2] = (unsigned char) col[2];
      }

      _splatCloud->addColor(color);
    }

    // read normal
    if (loadNormals) {
      float nrm[3];
      sstream >> nrm[0];
      sstream >> nrm[1];
      sstream >> nrm[2];

      SplatCloud::Normal normal;
      normal[0] = nrm[0];
      normal[1] = nrm[1];
      normal[2] = nrm[2];

      _splatCloud->addNormal(normal);
    }

    // read pointsize
    if (loadPointsizes) {
      float ps;
      sstream >> ps;

      SplatCloud::Pointsize pointsize;
      pointsize = ps;

      _splatCloud->addPointsize(pointsize);
    }
  }

  // return success
  return true;
}

//----------------------------------------------------------------

bool FilePTSPlugin::writeBinaryFile(std::ofstream &_outstream, const SplatCloudNode *_splatCloudNode)
{
  // set default options
  bool saveNormals = true;
  bool savePointsizes = false;
  bool saveColors = false;
//	int  saveColorRange = 0;

  // get current translation and scaling
  float s = 1.0f / _splatCloudNode->splatCloud().scaleFactor();
  SplatCloud::Point t = -_splatCloudNode->splatCloud().translation();

  // get options
  if (OpenFlipper::Options::gui() && saveOptions_) {
    saveNormals = saveNormals_->isChecked();
    savePointsizes = savePointsizes_->isChecked();
    saveColors = saveColors_->isChecked();
//		saveColorRange = saveColorRange_->currentIndex();
  }

  // write file type
  int fileType = 1;
  _outstream.write((char *) &fileType, sizeof(int));

  // write number of points
  unsigned int numPoints = _splatCloudNode->splatCloud().numPoints();
  _outstream.write((char *) &numPoints, sizeof(unsigned int));

  // write points
  {
    unsigned int i;
    for (i = 0; i < numPoints; ++i) {
      const SplatCloud::Point &point = _splatCloudNode->getPoint(i);

      float pnt[3];
      pnt[0] = s * point[0] + t[0];
      pnt[1] = s * point[1] + t[1];
      pnt[2] = s * point[2] + t[2];

      _outstream.write((char *) pnt, 3 * sizeof(float));
    }
  }

  // write normals
  if (saveNormals) {
    unsigned int i;
    for (i = 0; i < numPoints; ++i) {
      const SplatCloud::Normal &normal = _splatCloudNode->getNormal(i);

      float nrm[3];
      nrm[0] = normal[0];
      nrm[1] = normal[1];
      nrm[2] = normal[2];

      _outstream.write((char *) nrm, 3 * sizeof(float));
    }
  }

  // write pointsizes
  if (savePointsizes) {
    unsigned int i;
    for (i = 0; i < numPoints; ++i) {
      const SplatCloud::Pointsize &pointsize = _splatCloudNode->getPointsize(i);

      float ps;
      ps = s * pointsize;

      _outstream.write((char *) &ps, sizeof(float));
    }
  }

  // write colors
  if (saveColors) {
    unsigned int i;
    for (i = 0; i < numPoints; ++i) {
      const SplatCloud::Color &color = _splatCloudNode->getColor(i);

      unsigned int col; // ignore colorrange
      col = (0xFF << 24) | (color[0] << 16) | (color[1] << 8) | (color[2]);

      _outstream.write((char *) &col, sizeof(unsigned int));
    }
  }

  // return success
  return true;
}

//----------------------------------------------------------------

bool FilePTSPlugin::writeTextFile(std::ofstream &_outstream, const SplatCloudNode *_splatCloudNode)
{
  // set default options
  bool saveNormals = true;
  bool savePointsizes = false;
  bool saveColors = false;
  int saveColorRange = 0;

  // get current translation and scale factor
  float s = 1.0f / _splatCloudNode->splatCloud().scaleFactor();
  SplatCloud::Point t = -_splatCloudNode->splatCloud().translation();

  // get options
  if (OpenFlipper::Options::gui() && saveOptions_) {
    saveNormals = saveNormals_->isChecked();
    savePointsizes = savePointsizes_->isChecked();
    saveColors = saveColors_->isChecked();
    saveColorRange = saveColorRange_->currentIndex();
  }

  // for all points...
  unsigned int i, numPoints = _splatCloudNode->splatCloud().numPoints();
  for (i = 0; i < numPoints; ++i) {
    // write point
    {
      const SplatCloud::Point &point = _splatCloudNode->getPoint(i);

      float pnt[3];
      pnt[0] = s * point[0] + t[0];
      pnt[1] = s * point[1] + t[1];
      pnt[2] = s * point[2] + t[2];

      _outstream << pnt[0];
      _outstream << " " << pnt[1];
      _outstream << " " << pnt[2];
    }

    // write color
    if (saveColors) {
      const SplatCloud::Color &color = _splatCloudNode->getColor(i);

      if (saveColorRange == COLORRANGE_0_1) {
        static const float RCP255 = 1.0f / 255.0f;

        float col[3];
        col[0] = RCP255 * color[0];
        col[1] = RCP255 * color[1];
        col[2] = RCP255 * color[2];

        _outstream << " " << col[0];
        _outstream << " " << col[1];
        _outstream << " " << col[2];
      } else // saveColorRange == COLORRANGE_0_255
      {
        int col[3]; // use int, *not* unsigned char !
        col[0] = color[0];
        col[1] = color[1];
        col[2] = color[2];

        _outstream << " " << col[0];
        _outstream << " " << col[1];
        _outstream << " " << col[2];
      }
    }

    // write normal
    if (saveNormals) {
      const SplatCloud::Normal &normal = _splatCloudNode->getNormal(i);

      float nrm[3];
      nrm[0] = normal[0];
      nrm[1] = normal[1];
      nrm[2] = normal[2];

      _outstream << " " << nrm[0];
      _outstream << " " << nrm[1];
      _outstream << " " << nrm[2];
    }

    // write pointsize
    if (savePointsizes) {
      const SplatCloud::Pointsize &pointsize = _splatCloudNode->getPointsize(i);

      float ps;
      ps = s * pointsize;

      _outstream << " " << ps;
    }

    _outstream << std::endl;
  }

  // return success
  return true;
}

//----------------------------------------------------------------

int FilePTSPlugin::loadObject(QString _filename)
{
  // set default options
  bool loadBinaryFile = false;
  bool loadNormalizeSize = false;

  // get options
  if (OpenFlipper::Options::gui() && loadOptions_) {
    loadBinaryFile = loadBinaryFile_->isChecked();
    loadNormalizeSize = loadNormalizeSize_->isChecked();
  }

  // add a new, empty splatcloud object
  int id = -1;
  emit addEmptyObject(DATA_SPLATCLOUD, id);

  // get splatcloud-object by id
  SplatCloudObject *splatCloudObject = 0;
  if (!PluginFunctions::getObject(id, splatCloudObject))
    return 0; // failure, but empty object was added

  // check if splatcloud-object is okay
  if (!splatCloudObject)
    return 0; // failure, but empty object was added

  // get splatcloud and scenegraph splatcloud-node
  SplatCloud     *splatCloud     = splatCloudObject->splatCloud();
  SplatCloudNode *splatCloudNode = splatCloudObject->splatCloudNode();

  // check if splatcloud-node if okay
  if( !splatCloud || !splatCloudNode )
    return 0; // failure, but empty object was added

  // open file
  char *fn = QByteArray(_filename.toLatin1()).data();
  std::ifstream instream(fn, loadBinaryFile ? (std::ios::in | std::ios::binary) : std::ios::in);

  // check if file was opened
  if (!instream.is_open())
    return 0; // failure, but empty object was added

  // read file
  if (loadBinaryFile) {
    if (!readBinaryFile(instream, splatCloud))
      return 0; // failure, but empty object was added
  } else {
    if (!readTextFile(instream, splatCloud))
      return 0; // failure, but empty object was added
  }

  // check if something went wrong
  if (instream.bad())
    return 0; // failure, but empty object was added

  // close file
  instream.close();

  // normalize size
  if (loadNormalizeSize) {
    splatCloud->normalizeSize();
  }

  // remember filename
  splatCloudObject->setFromFileName(_filename);

  // emit signals that the object has to be updated and that a file was opened
  emit updatedObject(splatCloudObject->id(), UPDATE_ALL);
  emit openedFile(splatCloudObject->id());

  // get drawmodes
  ACG::SceneGraph::DrawModes::DrawMode splatsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode("Splats");
  ACG::SceneGraph::DrawModes::DrawMode dotsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode("Dots");
  ACG::SceneGraph::DrawModes::DrawMode pointsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode("Points");

  // if drawmodes don't exist something went wrong
  if (splatsDrawMode == ACG::SceneGraph::DrawModes::NONE || dotsDrawMode == ACG::SceneGraph::DrawModes::NONE
      || pointsDrawMode == ACG::SceneGraph::DrawModes::NONE) {
    emit log(LOGERR, tr("Shader DrawModes for SplatCloud not existent!"));
  } else {
    // get global drawmode
    ACG::SceneGraph::DrawModes::DrawMode drawmode = PluginFunctions::drawMode();

    // if global drawmode does *not* contain any of 'Splats', 'Dots' or 'Points' drawmode, add 'Points'
    if (!drawmode.containsAtomicDrawMode(splatsDrawMode) && !drawmode.containsAtomicDrawMode(dotsDrawMode)
        && !drawmode.containsAtomicDrawMode(pointsDrawMode)) {
      drawmode |= pointsDrawMode;
      PluginFunctions::setDrawMode(drawmode);
    }
  }

  // return the id of the new splatcloud object
  return id;
}

//----------------------------------------------------------------

bool FilePTSPlugin::saveObject(int _id, QString _filename)
{
  // set default options
  bool saveBinaryFile = false;

  // get options
  if (OpenFlipper::Options::gui() && saveOptions_) {
    saveBinaryFile = saveBinaryFile_->isChecked();
  }

  // get splatcloud-object by id
  SplatCloudObject *splatCloudObject = 0;
  if (!PluginFunctions::getObject(_id, splatCloudObject))
    return false; // failure

  // check if splatcloud-object is okay
  if (!splatCloudObject)
    return false; // failure

  // get scenegraph splatcloud-node
  const SplatCloudNode *splatCloudNode = splatCloudObject->splatCloudNode();

  // check if splatcloud-node if okay
  if( !splatCloudNode )
    return 0; // failure, but empty object was added

  // open file
  char *fn = QByteArray(_filename.toLatin1()).data();
  std::ofstream outstream(fn, saveBinaryFile ? (std::ios::out | std::ios::binary) : std::ios::out);

  // check if file was opened
  if (!outstream.is_open())
    return false; // failure

  // write file
  if (saveBinaryFile) {
    if (!writeBinaryFile(outstream, splatCloudNode))
      return false; // failure
  } else {
    if (!writeTextFile(outstream, splatCloudNode))
      return false; // failure
  }

  // check if something went wrong
  if (outstream.bad())
    return false;

  // close file
  outstream.close();

  // return success
  return true;
}

//----------------------------------------------------------------

QWidget *FilePTSPlugin::loadOptionsWidget(QString /*_currentFilter*/)
{
  if (loadOptions_ == 0) {
    // create new widget (including Load Options and buttons)

    loadBinaryFile_ = new QCheckBox("Load as Binary File");

    loadNormals_ = new QCheckBox("Contains Normals");
    loadPointsizes_ = new QCheckBox("Contains Pointsizes");
    loadColors_ = new QCheckBox("Contains Colors");

    loadColorRange_ = new QComboBox();
    loadColorRange_->addItem("[0..1]");
    loadColorRange_->addItem("[0..255]");
    slotUpdateLoadColorRange();

    QHBoxLayout *loadColorsLayout = new QHBoxLayout();
    loadColorsLayout->setSpacing(6);
    loadColorsLayout->addWidget(loadColors_);
    loadColorsLayout->addWidget(loadColorRange_);

    QVBoxLayout *loadStructureLayout = new QVBoxLayout();
    loadStructureLayout->setSpacing(6);
    loadStructureLayout->addWidget(loadNormals_);
    loadStructureLayout->addWidget(loadPointsizes_);
    loadStructureLayout->addItem(loadColorsLayout);

    QGroupBox *loadStructureGroupBox = new QGroupBox("Internal File Structure");
    loadStructureGroupBox->setLayout(loadStructureLayout);

    loadNormalizeSize_ = new QCheckBox("Normalize Size");

    loadMakeDefaultButton_ = new QPushButton("Make Default");

    QVBoxLayout *loadLayout = new QVBoxLayout();
    loadLayout->setAlignment(Qt::AlignTop);
    loadLayout->setSpacing(6);
    loadLayout->addWidget(loadBinaryFile_);
    loadLayout->addWidget(loadStructureGroupBox);
    loadLayout->addWidget(loadNormalizeSize_);
    loadLayout->addWidget(loadMakeDefaultButton_);

    loadOptions_ = new QWidget();
    loadOptions_->setLayout(loadLayout);

    // connect events to slots
    connect( loadBinaryFile_, SIGNAL( stateChanged(int) ), this, SLOT( slotUpdateLoadColorRange() ) );
    connect( loadColors_, SIGNAL( stateChanged(int) ), this, SLOT( slotUpdateLoadColorRange() ) );
    connect(loadMakeDefaultButton_, SIGNAL(clicked()), this, SLOT(slotLoadMakeDefaultButtonClicked()));

    // get Load Options from OpenFlipper (from disc)
    loadBinaryFile_->setChecked(OpenFlipperSettings().value("FilePTS/Load/BinaryFile", true).toBool());
    loadNormals_->setChecked(OpenFlipperSettings().value("FilePTS/Load/Normals", true).toBool());
    loadPointsizes_->setChecked(OpenFlipperSettings().value("FilePTS/Load/Pointsizes", true).toBool());
    loadColors_->setChecked(OpenFlipperSettings().value("FilePTS/Load/Colors", true).toBool());
    loadColorRange_->setCurrentIndex(OpenFlipperSettings().value("FilePTS/Load/ColorRange", 0).toInt());
    loadNormalizeSize_->setChecked(OpenFlipperSettings().value("FilePTS/Load/NormalizeSize", true).toBool());
  }

  return loadOptions_;
}

//----------------------------------------------------------------

QWidget *FilePTSPlugin::saveOptionsWidget(QString _currentFilter)
{
  if (saveOptions_ == 0) {
    // create new widget (including Save Options and buttons)

    saveBinaryFile_ = new QCheckBox("Save as Binary File");

    saveNormals_ = new QCheckBox("Save Normals");
    savePointsizes_ = new QCheckBox("Save Pointsizes");
    saveColors_ = new QCheckBox("Save Colors");

    saveColorRange_ = new QComboBox();
    saveColorRange_->addItem("[0..1]");
    saveColorRange_->addItem("[0..255]");
    slotUpdateSaveColorRange();

    QHBoxLayout *saveColorsLayout = new QHBoxLayout();
    saveColorsLayout->setSpacing(6);
    saveColorsLayout->addWidget(saveColors_);
    saveColorsLayout->addWidget(saveColorRange_);

    QVBoxLayout *saveStructureLayout = new QVBoxLayout();
    saveStructureLayout->setSpacing(6);
    saveStructureLayout->addWidget(saveNormals_);
    saveStructureLayout->addWidget(savePointsizes_);
    saveStructureLayout->addItem(saveColorsLayout);

    QGroupBox *saveStructureGroupBox = new QGroupBox("Internal File Structure");
    saveStructureGroupBox->setLayout(saveStructureLayout);

    saveMakeDefaultButton_ = new QPushButton("Make Default");

    QVBoxLayout *saveLayout = new QVBoxLayout();
    saveLayout->setAlignment(Qt::AlignTop);
    saveLayout->setSpacing(6);
    saveLayout->addWidget(saveBinaryFile_);
    saveLayout->addWidget(saveStructureGroupBox);
    saveLayout->addWidget(saveMakeDefaultButton_);

    saveOptions_ = new QWidget();
    saveOptions_->setLayout(saveLayout);

    // connect events to slots
    connect( saveBinaryFile_, SIGNAL( stateChanged(int) ), this, SLOT( slotUpdateSaveColorRange() ) );
    connect( saveColors_, SIGNAL( stateChanged(int) ), this, SLOT( slotUpdateSaveColorRange() ) );
    connect(saveMakeDefaultButton_, SIGNAL(clicked()), this, SLOT(slotSaveMakeDefaultButtonClicked()));

    // get Save Options from OpenFlipper (from disc)
    saveBinaryFile_->setChecked(OpenFlipperSettings().value("FilePTS/Save/BinaryFile", true).toBool());
    saveNormals_->setChecked(OpenFlipperSettings().value("FilePTS/Save/Normals", true).toBool());
    savePointsizes_->setChecked(OpenFlipperSettings().value("FilePTS/Save/Pointsizes", true).toBool());
    saveColors_->setChecked(OpenFlipperSettings().value("FilePTS/Save/Colors", true).toBool());
    saveColorRange_->setCurrentIndex(OpenFlipperSettings().value("FilePTS/Save/ColorRange", 0).toInt());
  }

  return saveOptions_;
}

//----------------------------------------------------------------

void FilePTSPlugin::slotUpdateLoadColorRange()
{
  loadColorRange_->setEnabled(loadColors_->isChecked() && !loadBinaryFile_->isChecked());
}

//----------------------------------------------------------------

void FilePTSPlugin::slotUpdateSaveColorRange()
{
  saveColorRange_->setEnabled(saveColors_->isChecked() && !saveBinaryFile_->isChecked());
}

//----------------------------------------------------------------

void FilePTSPlugin::slotLoadMakeDefaultButtonClicked()
{
  // pass our Load Options to OpenFlipper (to disc)
  OpenFlipperSettings().setValue("FilePTS/Load/BinaryFile", loadBinaryFile_->isChecked());
  OpenFlipperSettings().setValue("FilePTS/Load/Normals", loadNormals_->isChecked());
  OpenFlipperSettings().setValue("FilePTS/Load/Pointsizes", loadPointsizes_->isChecked());
  OpenFlipperSettings().setValue("FilePTS/Load/Colors", loadColors_->isChecked());
  OpenFlipperSettings().setValue("FilePTS/Load/ColorRange", loadColorRange_->currentIndex());
  OpenFlipperSettings().setValue("FilePTS/Load/NormalizeSize", loadNormalizeSize_->isChecked());

//	OpenFlipperSettings().setValue( "Core/File/UseLoadDefaults", true );
}

//----------------------------------------------------------------

void FilePTSPlugin::slotSaveMakeDefaultButtonClicked()
{
  // pass our Save Options to OpenFlipper (to disc)
  OpenFlipperSettings().setValue("FilePTS/Save/BinaryFile", saveBinaryFile_->isChecked());
  OpenFlipperSettings().setValue("FilePTS/Save/Normals", saveNormals_->isChecked());
  OpenFlipperSettings().setValue("FilePTS/Save/Pointsizes", savePointsizes_->isChecked());
  OpenFlipperSettings().setValue("FilePTS/Save/Colors", saveColors_->isChecked());
  OpenFlipperSettings().setValue("FilePTS/Save/ColorRange", saveColorRange_->currentIndex());
}

//================================================================

Q_EXPORT_PLUGIN2( fileptsplugin, FilePTSPlugin );
