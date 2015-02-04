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

#ifndef TEXTUREIMAGESTORAGE_HH
#define TEXTUREIMAGESTORAGE_HH

#include <QMap>
#include <QDateTime>
#include <QPair>
#include <QImage>

// ---------------------------------------------------------------------------

class ImageStore {
  public :
    ImageStore();

    int  addImageFile( QString _filename);
    int  addImage( QImage _image );

    int  getImageID(QString _filename);

    QImage& getImage(int _id, bool* _ok);

    QString error() {return errorString_; };

    void removeImage(int _id);

  private:
    QMap<int,QImage>  imageMap_;
    QMap< QString,QPair<int,QDateTime> > filenameMap_;
    QMap<int,QString> reverseFilenameMap_;
    QMap<int,unsigned int> refCount_;

    QString errorString_;

    QImage dummy_;
    int nextId_;
};

ImageStore& imageStore();



#endif //TEXTUREIMAGESTORAGE_HH
