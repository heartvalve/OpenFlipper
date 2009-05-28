//=============================================================================
//
// CLASS GeometryToolsT
//
//
// Author:  Henrik Zimmer <henrik@zimmer.to>
//
// Version: $Revision: 1$
// Date:    $Date: XX-XX-200X$
//
//=============================================================================


#ifndef VSTOOLS_HH
#define VSTOOLS_HH


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

//== DEFINITION =========================================================

/** These functions are required for Visual Studio to work around missing 
    functions. Basic equivalent functions for double exist in the float 
    header but are named different. So this wrapper makes them standard compatible.
    */
#ifdef WIN32
 #include <float.h>

 namespace std {

   inline bool isnan(double x)
   {
     return _isnan(x);
   } 

   inline bool isinf(double x)
   {
     return !_finite(x);
   } 

  }

 inline double nearbyint(double x) {
   return double(int( x + 0.5));
 }

 inline double round ( double _value ) {
   return floor( _value + 0.5 );
 }


#endif


//=============================================================================
#endif // VSTOOLS_HH defined
//=============================================================================

