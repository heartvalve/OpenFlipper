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

#include <OpenMesh/Tools/Smoother/JacobiLaplaceSmootherT.hh>
#include <OpenFlipper/common/perObjectData.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>


class SmootherObject : public PerObjectData
{
  public:
    
    SmootherObject();
    ~SmootherObject();
    
    // Copy function
    PerObjectData* copyPerObjectData();
    
    private:
        
        typedef OpenMesh::Smoother::SmootherT< TriMesh >::Component     SmoothingComponent;
        typedef OpenMesh::Smoother::SmootherT< TriMesh >::Continuity    SmoothingContinuity;
    
    public:
    
    // Get/Set methods
    void component(SmoothingComponent _comp)    { component_ = _comp; }
    void continuity(SmoothingContinuity _cont)  { continuity_ = _cont; }
    void distance(float _distance)             { distance_ = _distance; }
    void features(bool _features)               { respectFeatures_ = _features; }
    void iterations(uint _iterations)           { iterations_ = _iterations; }
    void initialized(bool _initialized)         { initialized_ = _initialized; }
    
    SmoothingComponent component()      { return component_; }
    SmoothingContinuity continuity()    { return continuity_; }
    float distance()                   { return distance_; }
    bool features()                     { return respectFeatures_; }
    uint iterations()                   { return iterations_; }
    bool initialized()                  { return initialized_;}
    
  private:
    // Smoothing attributes
    // Component:
    SmoothingComponent component_;
    
    // Continuity
    SmoothingContinuity continuity_;
    
    // Constraints
    float distance_;
    bool respectFeatures_;
    
    // Iterations
    uint iterations_;

    // initialization necessary?
    bool initialized_;
};

