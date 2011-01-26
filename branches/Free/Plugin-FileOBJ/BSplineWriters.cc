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

#include "FileOBJ.hh"


#ifdef ENABLE_BSPLINECURVE_SUPPORT
bool FileOBJPlugin::writeCurve(std::ostream& _out, QString _filename, BSplineCurve* _curve )
{

  _out << "# " << _filename.toStdString() << "\n";
  
  // save control points (coordinates)
  for (uint i = 0; i < _curve->n_control_points(); ++i){
    ACG::Vec3d cp = _curve->get_control_point(i);
    _out << "v " << cp[0] << " " << cp[1] << " " << cp[2] << "\n";
  }

  _out << "cstype bspline\n";
  _out << "deg " << _curve->degree() << "\n";
  _out << "g " << _filename.toStdString() << "\n";

  // save control polygon
  _out << "curv " << _curve->get_knot(0) << " " << _curve->get_knot(_curve->n_knots()-1) << " ";
  // save control point indices
  for (unsigned int i = 0; i < _curve->n_control_points(); ++i)
    _out << i+1 << " "; // obj enumerates the cps starting with 1
  _out << "\n";

  _out << "parm u ";
  // save knotvector
  for (unsigned int i = 0; i < _curve->n_knots(); ++i)
    _out << _curve->get_knot(i) << " ";
  _out << "\n";

  _out << "end";

  return true;
}
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
bool FileOBJPlugin::writeSurface(std::ostream& _out, QString _filename, BSplineSurface* _surface ){

  _out << "# " << _filename.toStdString() << "\n";

  // save control net (coordinates)
  unsigned int num_cp_m = _surface->n_control_points_m();
  unsigned int num_cp_n = _surface->n_control_points_n();

  for (unsigned int i = 0; i < num_cp_m; ++i)
  {
    for (unsigned int j = 0; j < num_cp_n; ++j)
    {
      ACG::Vec3d cp = (*_surface)(i,j);
      _out << "v " << cp[0] << " " << cp[1] << " " << cp[2] << "\n";
    }
  }

  _out << "cstype bspline\n";
  _out << "deg " << _surface->degree_m() << " " << _surface->degree_n() << "\n";
  _out << "g " << _filename.toStdString() << "\n";

  // save control polygon
  _out << "surf " << _surface->get_knot_m(0) << " " << _surface->get_knot_m(_surface->n_knots_m()-1) << " "
                  << _surface->get_knot_n(0) << " " << _surface->get_knot_n(_surface->n_knots_n()-1) << " ";

  // save control point indices
  for (unsigned int j = 0; j < num_cp_n; ++j)
    for (unsigned int i = 0; i < num_cp_m; ++i)
      _out << (i*num_cp_n) + j+1 << " "; // obj enumerates the cps starting with 1

  _out << "\n";

  _out << "parm u ";
  // save knotvector in m direction
  for (unsigned int i = 0; i < _surface->n_knots_m(); ++i)
    _out << _surface->get_knot_m(i) << " ";
  _out << "\n";

  _out << "parm v ";
  // save knotvector in n direction
  for (unsigned int i = 0; i < _surface->n_knots_n(); ++i)
    _out << _surface->get_knot_n(i) << " ";
  _out << "\n";

  _out << "end";

  return true;
}

#endif

