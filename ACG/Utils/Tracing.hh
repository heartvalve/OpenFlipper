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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS Tracing
//
//=============================================================================


#ifndef ACG_TRACING_HH
#define ACG_TRACING_HH


//== INCLUDES =================================================================

#include "StopWatch.hh"
#include <iostream>
#include <string>
#include "../Config/ACGDefines.hh"


//== NAMESPACES ===============================================================

namespace ACG {


//== HELPER MACROS ============================================================


#define ACG_TRACE(_text) ACG::Tracing acg_tracer__(_text)
#define ACG_TRACE_CMD(_text, _cmd) { ACG_TRACE(_text); cmd; }

#define ACG_TIMED_TRACE(_text) ACG::TimedTracing acg_tracer__(_text)
#define ACG_TIMED_TRACE_CMD(_text, _cmd) { ACG_TIMED_TRACE(_text); _cmd; }

#define ACG_TRACE_PROGRESS acg_tracer__.progress()


//== CLASS DEFINITION =========================================================


/** \class Tracing Tracing.hh <ACG/Utils/Tracing.hh>

    Tracing outputs starting and finishing messages for some lengthy
    procedures. Its constructor will output a given string, the
    destructor will output "finished\\n". That means you have to
    start a new block and define a local trancing object in
    there. When entering the block the constructor is called, the
    start message is printed. When leaving the block the descructor
    displays the finishing message.  For convenience there are some
    macros defining the local object so you can write:

    \code
    { // start a new block
      ACG_TRACE("What's the meaning of like?");

      for (years=0; years <= ACG::NumLimitsT<int>::max(); ++years)
      {
        // ... lengthy computation, show some progress
	ACG_TRACE_PROGRESS;
      }

      std::cout << "42";

    } // leaving the block will display "finished\n"
    \endcode

    If you want to trace only one command you can also use:
    \code
    ACG_TRACE_CMD("What's the meaning of like?", calc_meaning_of_life());
    \endcode

    \see TimedTracing
**/
class ACGDLLEXPORT Tracing
{
public:

  /// Constructor
  Tracing(const std::string& _text,
	  std::ostream&      _os = std::cerr)
    : os_(_os)
  { os_ << _text << "  " << std::flush; }


  /// Destructor
  ~Tracing() { os_ << "finished.\n" << std::flush; }


  /** Show some progress (the next frame of a rotating bar is
      displayed for every call of this function) */
  void progress() {
    os_ << progress_[((++idx_)&=0x3)] << "\b" << std::flush;
  }


protected:

  std::ostream&  os_;
  static char    progress_[4];
  static unsigned char idx_;
};


//== CLASS DEFINITION =========================================================



/** \class TimedTracing Tracing.hh <ACG/Utils/Tracing.hh>

    This class is similar to ACG::Tracing. It just starts a timer
    at the start of the process to be traced and outputs the
    time the computation took when leaving the block. Again
    we provide some convenience macros.

    \code
    { // start a new block
      ACG_TIMES_TRACE("What's the meaning of like?");

      for (years=0; years <= ACG::NumLimitsT<int>::max(); ++years)
      {
        // ... lengthy computation, show some progress
	ACG_TRACE_PROGRESS;
      }

      std::cout << "42";

    } // leaving the block will display " <...> secs, finished\n"
    \endcode

    If you want to trace only one command you can also use:
    \code
    ACG_TIMED_TRACE_CMD("What's the meaning of like?", calc_meaning_of_life());
    \endcode

    \see TimedTracing
**/
class ACGDLLEXPORT TimedTracing : public Tracing
{
public:

  /// default constructor
  TimedTracing( const std::string& _text,
		std::ostream&      _os = std::cerr )
    : Tracing(_text, _os)
  { timer_.start(); }


  /// destructor
  ~TimedTracing()
  {
    os_ << timer_.stop()*0.001 << " secs, ";
  }

private:

  StopWatch timer_;
};



//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_TRACING_HH defined
//=============================================================================

