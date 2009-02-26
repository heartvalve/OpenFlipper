#ifndef ACGDLLEXPORT
	#ifdef WIN32
		#ifdef ACGDLL
			#ifdef USEACG
				#define ACGDLLEXPORT __declspec(dllimport)				
				#define ACGDLLEXPORTONLY 
			#else
				#define ACGDLLEXPORT __declspec(dllexport)
				#define ACGDLLEXPORTONLY __declspec(dllexport)
			#endif
		#else		
			#define ACGDLLEXPORT	
			#define ACGDLLEXPORTONLY
		#endif
	#else
		#define ACGDLLEXPORT
		#define ACGDLLEXPORTONLY
	#endif
#endif
