/////////////////////////////////////////////////////////////////////////////
// Name:        setup.h
// Purpose:     Configuration for the library
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SETUP_H_
#define _WX_SETUP_H_

/*
 * General features
 *
 */

#define USE_CONFIG           1
                                // Use wxConfig, with CreateConfig in wxApp
#define _WX_GOODCOMPILER__
                                // gcc can have problems, but Windows compilers
                                // are generally OK.
#define WXWIN_COMPATIBILITY  1
                                // Compatibility with 1.66 API.
                                // Level 0: no backward compatibility, all new features
                                // Level 1: wxDC, OnSize (etc.) compatibility, but
                                //          some new features such as event tables

#define USE_POSTSCRIPT  1
                        	// 0 for no PostScript device context
#define USE_AFM_FOR_POSTSCRIPT 0
                                // 1 to use font metric files in GetTextExtent
#define USE_METAFILE    1
                                // 0 for no Metafile and metafile device context
#define USE_IPC         1
                                // 0 for no interprocess comms
#define USE_HELP        1
                                // 0 for no help facility
#define USE_RESOURCES   1
                        	// 0 for no wxGetResource/wxWriteResource
#define USE_CLIPBOARD   1
                        	// 0 for no clipboard functions
#define USE_SPLINES     1
                            	// 0 for no splines
#define USE_XFIG_SPLINE_CODE  1
                           	// 1 for XFIG spline code, 0 for AIAI spline code.
// AIAI spline code is slower, but freer of copyright issues.

#define USE_DRAG_AND_DROP 1
                        	// 0 for no drag and drop

#define USE_WX_RESOURCES        1
                                  // Use .wxr resource mechanism (requires PrologIO library)

#define USE_DOC_VIEW_ARCHITECTURE 1
                                  // Set to 0 to disable document/view architecture
#define USE_PRINTING_ARCHITECTURE  1
                                    // Set to 0 to disable print/preview architecture code
#define USE_POSTSCRIPT_ARCHITECTURE_IN_MSW 1
                                    // Set to 0 to disable PostScript print/preview architecture code
                                    // under Windows (just use Windows printing).
#define USE_DYNAMIC_CLASSES     1
                                  // If 1, enables provision of run-time type information.
                                  // NOW MANDATORY: don't change.
#define USE_MEMORY_TRACING      1
                                  // If 1, enables debugging versions of wxObject::new and
                                  // wxObject::delete *IF* WXDEBUG is also defined.
                                  // WARNING: this code may not work with all architectures, especially
                                  // if alignment is an issue.
#define USE_DEBUG_CONTEXT       1
                                  // If 1, enables wxDebugContext, for
                                  // writing error messages to file, etc. 
                                  // If WXDEBUG is not defined, will still use
                                  // normal memory operators.
                                  // It's recommended to set this to 1,
                                  // since you may well need to output
                                  // an error log in a production
                                  // version (or non-debugging beta)
#define USE_GLOBAL_MEMORY_OPERATORS 1
                                  // In debug mode, cause new and delete to be redefined globally.
                                  // If this causes problems (e.g. link errors), set this to 0.

#define REMOVE_UNUSED_ARG 1
                                  // Set this to 0 if your compiler can't cope
                                  // with omission of prototype parameters.

#define USE_C_MAIN 0
                                  // Set to 1 to use main.c instead of main.cpp (UNIX only)

#define USE_ODBC                   1
                                    // Define 1 to use ODBC classes

#define USE_IOSTREAMH     1
                                  // VC++ 4.2 and above allows <iostream> and <iostream.h>
                                  // but you can't mix them. Set to 1 for <iostream.h>,
                                  // 0 for <iostream>

#define USE_WXCONFIG      1
                                  // if enabled, compiles built-in OS independent wxConfig
                                  // class and it's file (any platform) and registry (Win)
                                  // based implementations
/*
 * Finer detail
 *
 */

#define USE_APPLE_IEEE	  1
				// if enabled, the float codec written by Apple
				// will be used to write, in a portable way,
				// float on the disk

#endif
    // _WX_SETUP_H_
