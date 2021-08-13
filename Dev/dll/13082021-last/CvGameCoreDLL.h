#pragma once

#ifndef CvGameCoreDLL_h
#define CvGameCoreDLL_h

// includes (pch) for gamecore dll files
// Author - Mustafa Thamer

// <advc.make>
#include "PragmaWarnings.h" // Customize compiler warnings
#pragma warning(push, 3) // Don't check win, std and boost headers with /W4
// </advc.make>

// WINDOWS ...
#define WIN32_LEAN_AND_MEAN
// <advc.fract> Otherwise, classes in the PCH can't have members named "max" and "min".
#ifndef NOMINMAX
	#define NOMINMAX
#endif // </advc.fract>
/*	<advc.make> That's Windows 2000, which is what Civ 4 requires anyway
	(according to the publisher's website). Enables some more macros;
	who knows, maybe also more optimized code. */
#define _WIN32_WINNT 0x0500
#include <windows.h>
// advc.fract: Commented out, originally in CvGameCoreUtils.h.
//#undef max
//#undef min
#include <MMSystem.h>
#if defined _DEBUG && !defined USE_MEMMANAGER
	#define USE_MEMMANAGER
	#include <crtdbg.h>
#endif
#include <vector>
#include <list>
#include <tchar.h>
//#include <math.h>
//#include <assert.h>
// <advc>
#include <cmath>
#include <cassert>
#include <iterator>
#include <sstream> // </advc>
#include <map>
#include <hash_map>
// K-Mod
#include <set>
#include <utility>
#include <algorithm>
#include <queue> // k146
#include <stack> // advc.030
// K-Mod end

#define DllExport   __declspec( dllexport )

// (advc.make: Some macros moved into new header Trigonometry.h)

// <advc.003s> For generating variable names. (The layer of indirection is necessary.)
#define CONCATVARNAME_IMPL(prefix, suffix) prefix##suffix
#define CONCATVARNAME(prefix, suffix) CONCATVARNAME_IMPL(prefix, suffix) // </advc.003s>

// <advc> Stuff moved into separate headers
#include "GameBryo.h"
#include "CvMemoryManager.h"
#include "BoostPythonPCH.h"
#pragma warning(pop) // advc.make: Restore project warning level
#include "IntegerTraits.h" // </advc>
#include "FAssert.h"
#include "CvGameCoreDLLDefNew.h"
#include "FDataStreamBase.h"
#include "FFreeListTrashArray.h" // advc.003s: includes FreeListTraversal.h
#include "LinkedList.h"
#include "CvString.h"
#include "BitUtil.h" // advc.enum
#include "CvDefines.h"
#include "CvEnums.h" // includes CvInfoEnum.h
/*  advc: Smaller numbers may already crash the EXE; the DLL assumes in some places
	that player ids fit in a single byte. */
BOOST_STATIC_ASSERT(MAX_PLAYERS < MAX_CHAR && MAX_TEAMS < MAX_CHAR);
#include "CvStructs.h"
#include "CvDLLUtilityIFaceBase.h"

//jason tests (advc.make: removed most of those)
#include "CvRandom.h"
#include "FProfiler.h"
#include "CvGameCoreUtils.h"
#include "ScaledNum.h" // Includes TypeChoice.h
#include "CvGlobals.h"
#include "EnumMap2D.h" // advc.enum: Includes EnumMap.h
#include "CvPythonCaller.h" // advc.003y
#include "CvDLLLogger.h" // advc.003t
// <advc.003x> Include only parts of the old CvInfos.h (caveat: the order of these matters)
#include "CvInfo_Base.h" // advc.enum: Includes CvInfo_EnumMap.h
#include "CvInfo_Asset.h"
#include "CvInfo_Tech.h"
#include "CvInfo_Civilization.h"
#include "CvInfo_Organization.h"
#include "CvInfo_Symbol.h"
#include "CvInfo_RandomEvent.h"
// For inlining in CvUnit.h. It's also big and needed rather frequently.
#include "CvInfo_Unit.h" // </advc.003x>
/*  advc.make: These I had removed (not _that_ frequently included),
	but decided to add them back. */
#include "CyGlobalContext.h" // Includes CvArtFileMgr.h
#include "CyCity.h"
#include "CvDLLEntityIFaceBase.h"
// advc.make: New additions
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvInitCore.h" // Rarely changes and useful to have for inlining
#include "CvEventReporter.h" // Includes CvStatistics.h and CvDllPythonEvents.h
#include "CyArgsList.h"
#include "CyPlot.h"
#include "CyUnit.h"

// Undefine OutputDebugString in release builds // advc.make: in non-debug builds
#ifdef _DEBUG
	/*	<advc.wine> Wrap a macro around OutputDebugString that prints to both the VS console
		(as before through WinBase.h) and to a regular console e.g. for Wine. */
	// Caveat: szMsg has to be zero-terminated -- no fixed-size char buffers!
	#define printToConsole(szMsg) \
		OutputDebugString(szMsg); \
		printf("OutputDebugString: %s", szMsg);
#else
	#define printToConsole(szMsg)
#endif // </advc.wine>

#endif
