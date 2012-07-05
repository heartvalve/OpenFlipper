/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
 *                                                                           *
\*===========================================================================*/

/* Salt file for security interface 
*/

/** This Macro creates a salt (basically a key) for your plugin which is added to the 
    license files. You have to keep this salt file
    !!!SECRET!!!, otherwise everybody can create licenses for your Plugin. The special 
    form of this macro makes the salt string invisible inside the binary 
    of the plugin so that it is not easy to recover from it.

    Modify the Salts below to create your own plugin keys. Then
    copy this file to your plugin directory and name it "salt.hh".

    Details on how to use license management can be found here on the \ref securityInterfacePage . 
*/

/// Specify your contact mail address
#define CONTACTMAIL QString("contact@openflipper.org")

/// Specify part one of your salt 
#define ADD_SALT_PRE(s)                        \
      s += char(100);                          \
      s += char(99);                           \
      s += char(93);                           \
      s += char(110);                          \
      s += char(84);

/// Specify part two of your salt 
#define ADD_SALT_POST(s)                       \
       s += char(103);                         \
       s += char(82);                          \
       s += char(140);                         \
       s += char(96);                          \
       s += char(102);
       
/// Specify the filename of your plugin (not secured)
#define ADD_PLUGIN_FILENAME(s)                  \
s += "Plugin-PolyLine";


