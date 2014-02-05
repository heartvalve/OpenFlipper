/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision: 17392 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2013-08-26 15:03:53 +0200 (Mon, 26 Aug 2013) $                   *
 *                                                                           *
\*===========================================================================*/

//=============================================================================
//
//  CLASS PolyLinePlugin - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "PolyLinePlugin.hh"

ACG::Vec3d
PolyLinePlugin::
getPointOnMesh(PolyLineBezierSplineData* _SplineData, ACG::Vec3d _point, ACG::Vec3d* _nor)
{
	TriMeshObject* mesh;
	if(!PluginFunctions::getObject(_SplineData->meshIndex_, mesh))
		return _point;
	OpenMeshTriangleBSPT<TriMesh>* bsp = mesh->requestTriangleBsp();
	OpenMeshTriangleBSPT<TriMesh>::NearestNeighbor neigh = bsp->nearest(_point);
	ACG::Vec3d nor = mesh->mesh()->normal(neigh.handle);
	if(_nor)
		*_nor = nor;
	OpenMeshTriangleBSPT<TriMesh>::RayCollision rayInt = bsp->raycollision(_point, nor);
	if(rayInt.size())
		return _point + nor * rayInt[0].second;
	return _point + nor.normalize() * neigh.dist;
}

void
PolyLinePlugin::
updatePolyBezierHandles(PolyLineObject* _lineObject, ACG::SceneGraph::LineNode* _line)
{
	PolyLineBezierSplineData* splineData = dynamic_cast<PolyLineBezierSplineData*>(_lineObject->objectData(BEZSPLINE_DATA));
	if(!splineData)
		return;
	_line->clear();
	for(unsigned int i = 0; i < splineData->handles_.size(); i++) {
		ACG::SceneGraph::GlutPrimitiveNode* node = 0;
		if(_lineObject->getAdditionalNode(node, name(), "handle", i))
			node->set_position(splineData->handles_[i]);
		const PolyLineBezierSplineData::InterpolatePoint& control = splineData->getInterpolatePoint(i);
		_line->add_line(control.position, splineData->handles_[i]);
		_line->add_color(ACG::Vec4f(1,0,0,1));
	}
	for(unsigned int i = 0; i < splineData->points_.size(); i++) {
		ACG::SceneGraph::GlutPrimitiveNode* node = 0;
		if(_lineObject->getAdditionalNode(node, name(), "control", i))
			node->set_position(splineData->points_[i].position);
	}
}

void
PolyLinePlugin::
updatePolyBezierSpline(PolyLineObject* _lineObject, unsigned int _pointCount)
{
	PolyLineBezierSplineData* splineData = dynamic_cast<PolyLineBezierSplineData*>(_lineObject->objectData(BEZSPLINE_DATA));
	TriMeshObject* mesh;
	if(!splineData || !PluginFunctions::getObject(splineData->meshIndex_, mesh)) {
		return;//no mesh -> do nothing
	}
	_lineObject->line()->clear();
	unsigned int segCount = (splineData->points_.size() + splineData->handles_.size() - 1) / 3;
	for(unsigned int s = 0; s < segCount; s++) {
		const ACG::Vec3d a = splineData->points_[s].position, d = splineData->points_[s + 1].position,
						 b = splineData->handles_[s * 2], c = splineData->handles_[s * 2 + 1];
		//the last segment will get one more point to close the spline
		unsigned int n = (s != segCount - 1) ? _pointCount : (_pointCount + 1);
		for(unsigned int i = 0; i < n; i++) {
			float alpha = float(i) / float(_pointCount);
			const ACG::Vec3d e = a + (b - a) * alpha, f = c + (d - c) * alpha;
			ACG::Vec3d g = e + (f - e) * alpha;
			g = getPointOnMesh(splineData, g);
			_lineObject->line()->add_point(g);
		}
	}
	emit updatedObject(_lineObject->id(), UPDATE_GEOMETRY | UPDATE_TOPOLOGY);
}
