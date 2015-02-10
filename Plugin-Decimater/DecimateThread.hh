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


#ifndef DECIMATETHREAD_HH
#define DECIMATETHREAD_HH


#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/McDecimaterT.hh>
#include <OpenMesh/Tools/Decimater/MixedDecimaterT.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include <QObject>
#include <OpenFlipper/threads/OpenFlipperThread.hh>

class DecimateThread : public OpenFlipperThread, public OpenMesh::Decimater::Observer
{
  Q_OBJECT
public:
  typedef OpenMesh::Decimater::BaseDecimaterT< TriMesh >              BaseDecimaterType;
  typedef OpenMesh::Decimater::DecimaterT< TriMesh >                  DecimaterType;
  typedef OpenMesh::Decimater::McDecimaterT< TriMesh >                McDecimaterType;
  typedef OpenMesh::Decimater::MixedDecimaterT< TriMesh >             MixedDecimaterType;

  struct Params
  {
    DecimaterType* dec;
    McDecimaterType* mcDec;
    MixedDecimaterType* mixedDec;
    bool useIncremental;
    bool useMC;
    bool useMixed;
    float mc_factor;
    int verticesCount; // -1 if not decimate to vertices
    int facesCount; // -1 if not decimate to faces
    int samples;
  };

private:

  bool abort_;
  int maxCollapses_;
  Params param_;
  QString jobId_;
  BaseDecimaterType* baseDec_;
  int objId_;

public:
  virtual void notify(size_t _step);
  virtual bool abort() const;

public slots:
  void decimate();

public:
  void cancel();
  DecimateThread(Params _param, QString _jobId, int _objId);
  ~DecimateThread();

  BaseDecimaterType* baseDecimater();

  int objectId() const;

};

#endif // DECIMATETHREAD_HH
