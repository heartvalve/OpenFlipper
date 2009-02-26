/*******************************************************************************
 * gldebug.h
 *
 * OpenGL debugging utilities.
 *
 * (C)2007
 * Lehrstuhl I8 RWTH-Aachen, http://www-i8.informatik.rwth-aachen.de
 * Author: Markus Tavenrath <speedygoo@speedygoo.de>
 *
 ******************************************************************************/

#ifndef GLDEBUG_H
#define GLDEBUG_H

//==============================================================================

#include <iostream>

/** \brief
 * This file contains two macros for openGL error checking
 ***/
#ifndef NDEBUG
#define checkGLError() \
    { int error; if ( (error = glGetError()) != GL_NO_ERROR ) std::cout << "GLError " << __FILE__ << ":" << __LINE__ << " - " << error << std::endl; }

#define checkGLError2(str) \
    { int error; if ( (error = glGetError()) != GL_NO_ERROR ) std::cout << "GLError " << __FILE__ << ":" << __LINE__ << " - " << error << " - " << str <<std::endl; }
    
#else 

#define checkGLError()
#define checkGLError2(str)

#endif


//==============================================================================

// Local Variables:
// mode: C++
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:

//==============================================================================

/* GLDEBUG_H */
#endif
