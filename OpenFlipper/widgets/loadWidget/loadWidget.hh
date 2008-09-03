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




#include <QtGui>
#include <QDirModel>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/Core/Core.hh>

class LoadWidget : public QFileDialog
{
  Q_OBJECT
  public:
    LoadWidget(std::vector<fileTypes>& _supportedTypes , QWidget *parent = 0 );
  signals:
    void load(QString _filename, DataType _type, int& _id);
    void save(int _id, QString _filename);
    
  private slots:
    void slotSetLoadFilters(int _typeIndex);
    void slotSetSaveFilters(int _typeIndex);
    
  private:
    int id_;
    bool loadMode_;
    QStringList lastPaths;
    std::vector<fileTypes>& supportedTypes_;
    

    
  public:
    int showLoad();
    int showSave(int _id, QString _filename);
    
  public slots :
    virtual void accept();
    
  private :
    void loadFile();
    void saveFile();
    
    /// combo box for choosing file type to load/save
    QComboBox* typeBox_;
    
    /// QFrame on the right
    QGroupBox*    box_;
    QHBoxLayout*  boxLayout_;

    bool step_;
    
};

