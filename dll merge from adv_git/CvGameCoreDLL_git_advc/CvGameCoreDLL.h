#pragma once

#ifndef CvGameCoreDLL_h
#define CvGameCoreDLL_h

//
// includes (pch) for gamecore dll files
// Author - Mustafa Thamer
//

//
// WINDOWS
//
#pragma warning( disable: 4530 )	// C++ exception handler used, but unwind semantics are not enabled

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <MMSystem.h>
#if defined _DEBUG && !defined USE_MEMMANAGER
	#define USE_MEMMANAGER
	#include <crtdbg.h>
#endif
#include <vector>
#include <list>
#include <tchar.h>
#include <math.h>
#include <assert.h>
#include <map>
#include <hash_map>
// K-Mod
#include <set>
#include <utility>
#include <algorithm>
// K-Mod end

#define DllExport   __declspec( dllexport )

//
// GameBryo
//
class NiColor
{
public:
	float r, g, b;
};
class NiColorA
{
public:
	NiColorA(float fr, float fg, float fb, float fa) : r(fr), g(fg), b(fb), a(fa) {}
	NiColorA() {}
	float r, g, b, a;
};
class NiPoint2
{
public:
	NiPoint2() {}
	NiPoint2(float fx, float fy) : x(fx),y(fy) {}

	float x, y;
};
class NiPoint3
{
public:
	NiPoint3() {}
	NiPoint3(float fx, float fy, float fz) : x(fx),y(fy),z(fz) {}

	bool NiPoint3::operator== (const NiPoint3& pt) const
	{	return (x == pt.x && y == pt.y && z == pt.z);	}

	inline NiPoint3 NiPoint3::operator+ (const NiPoint3& pt) const
	{	return NiPoint3(x+pt.x,y+pt.y,z+pt.z);	}

	inline NiPoint3 NiPoint3::operator- (const NiPoint3& pt) const
	{	return NiPoint3(x-pt.x,y-pt.y,z-pt.z);	}

	inline float NiPoint3::operator* (const NiPoint3& pt) const
	{	return x*pt.x+y*pt.y+z*pt.z;	}

	inline NiPoint3 NiPoint3::operator* (float fScalar) const
	{	return NiPoint3(fScalar*x,fScalar*y,fScalar*z);	}

	inline NiPoint3 NiPoint3::operator/ (float fScalar) const
	{
		float fInvScalar = 1.0f/fScalar;
		return NiPoint3(fInvScalar*x,fInvScalar*y,fInvScalar*z);
	}

	inline NiPoint3 NiPoint3::operator- () const
	{	return NiPoint3(-x,-y,-z);	}

	inline float Length() const
	{ return sqrt(x * x + y * y + z * z); }

	inline float Unitize()
	{
		float length = Length();
		if(length != 0)
		{
			x /= length;
			y /= length;
			z /= length;
		}
		return length;
	}

//	inline NiPoint3 operator* (float fScalar, const NiPoint3& pt)
//	{	return NiPoint3(fScalar*pt.x,fScalar*pt.y,fScalar*pt.z);	}
	float x, y, z;
};

namespace NiAnimationKey
{
	enum KeyType
	{
		NOINTERP,
		LINKEY,
		BEZKEY,
		TCBKEY,
		EULERKEY,
		STEPKEY,
		NUMKEYTYPES
	};
};

typedef unsigned char    byte;
typedef unsigned short   word;
typedef unsigned int     uint;
typedef unsigned long    dword;
typedef unsigned __int64 qword;
typedef wchar_t          wchar;

#define MAX_CHAR                            (0x7f)
#define MIN_CHAR                            (0x80)
#define MAX_SHORT                           (0x7fff)
#define MIN_SHORT                           (0x8000)
#define MAX_INT                             (0x7fffffff)
#define MIN_INT                             (0x80000000)
#define MAX_UNSIGNED_CHAR                   (0xff)
#define MIN_UNSIGNED_CHAR                   (0x00)
#define MAX_UNSIGNED_SHORT                  (0xffff)
#define MIN_UNSIGNED_SHORT                  (0x0000)
#define MAX_UNSIGNED_INT                    (0xffffffff)
#define MIN_UNSIGNED_INT                    (0x00000000)

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define SQR(x)      ( (x) * (x) )
#define DEGTORAD(x) ( (float)( (x) * (M_PI / 180) ))
#define LIMIT_RANGE(low, value, high) value = (value < low ? low : (value > high ? high : value));
#define M_PI       3.14159265358979323846
#define fM_PI		3.141592654f		//!< Pi (float)

__forceinline DWORD FtoDW( float f ) { return *(DWORD*)&f; }
__forceinline float DWtoF( dword n ) { return *(float*)&n; }
__forceinline float MaxFloat() { return DWtoF(0x7f7fffff); }

void startProfilingDLL(bool longLived);
void stopProfilingDLL(bool longLived);

#ifdef USE_INTERNAL_PROFILER
struct ProfileSample;
void IFPBeginSample(ProfileSample* sample);
void IFPEndSample(ProfileSample* sample);
void dumpProfileStack(void);
void EnableDetailedTrace(bool enable);
#endif

#ifdef _DEBUG
//#define MEMORY_TRACKING
#endif

#ifdef MEMORY_TRACKING
class CMemoryTrack
{
#define	MAX_TRACKED_ALLOCS	1000
	void*	m_track[MAX_TRACKED_ALLOCS];
	int		m_highWater;
	const char* m_name;
	bool	m_valid;
#define MAX_TRACK_DEPTH		50
	static	CMemoryTrack*	trackStack[MAX_TRACK_DEPTH];
	static	m_trackStackDepth;

public:
	CMemoryTrack(const char* name, bool valid);

	~CMemoryTrack();

	void NoteAlloc(void* ptr);
	void NoteDeAlloc(void* ptr);

	static CMemoryTrack* GetCurrent(void);
};

class CMemoryTrace
{
	int		m_start;
	const char* m_name;

public:
	CMemoryTrace(const char* name);

	~CMemoryTrace();
};

void DumpMemUsage(const char* fn, int line);

#define DUMP_MEMORY_USAGE()	DumpMemUsage(__FUNCTION__,__LINE__);
#define MEMORY_TRACK()	CMemoryTrack __memoryTrack(__FUNCTION__, true);
#define MEMORY_TRACK_EXEMPT()	CMemoryTrack __memoryTrackExemption(NULL, false);
#define MEMORY_TRACE_FUNCTION()	CMemoryTrace __memoryTrace(__FUNCTION__);
#else
#define DUMP_MEMORY_USAGE()
#define	MEMORY_TRACK()
#define MEMORY_TRACK_EXEMPT()
#define MEMORY_TRACE_FUNCTION()
#endif


//
// Boost Python
//
// <advc.make> (This prevents many error markers in the VS code editor, but not all.)
#ifdef _CODE_EDITOR
namespace boost
{
	namespace python {
		class tuple;
		template<typename x, typename y> class class_;
		template<typename x> class return_value_policy;
		class reference_existing_object;
	}
	class noncopyable {}; // advc.003e
}
class PyObject;
#else // </advc.make>
# include <boost/python/list.hpp>
# include <boost/python/tuple.hpp>
# include <boost/python/class.hpp>
# include <boost/python/manage_new_object.hpp>
# include <boost/python/return_value_policy.hpp>
# include <boost/python/object.hpp>
# include <boost/python/def.hpp>

namespace python = boost::python;
#endif // advc.make

#include "FAssert.h"
#include "CvGameCoreDLLDefNew.h"
#include "FDataStreamBase.h"
#include "FFreeListArrayBase.h"
#include "FFreeListTrashArray.h"
#include "FFreeListArray.h"
//#include "FVariableSystem.h"
#include "CvString.h"
#include "CvEnums.h" // includes CvDefines.h
#include "CvStructs.h"
#include "CvDLLUtilityIFaceBase.h" // includes LinkedList.h

//jason tests
// advc.make: Removed most of these. (Don't know what the comment above means.)
#include "CvGameCoreUtils.h"
#include "CvGlobals.h"
#include "CvPythonCaller.h" // advc.003y
#include "CvDLLLogger.h" // advc.003t
#include "FProfiler.h"
#include "CvRandom.h"
// <advc.003x> Include only parts of the old CvInfos.h (caveat: the order of these matters)
#include "CvInfo_Base.h"
#include "CvInfo_Asset.h"
#include "CvInfo_Tech.h"
#include "CvInfo_Civilization.h"
#include "CvInfo_Organization.h"
#include "CvInfo_Symbol.h"
#include "CvInfo_RandomEvent.h" // </advc.003x>
/*  advc.make: These I had removed (not _that_ frequently included),
	but decided to add them back. */
#include "CyGlobalContext.h" // includes CvArtFileMgr.h
#include "CyCity.h"
#include "CvDLLEntityIFaceBase.h"
// advc.make: New additions
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvInitCore.h"
#include "CvEventReporter.h" // includes CvStatistics.h and CvDllPythonEvents.h
#include "CyArgsList.h"
#include "CyPlot.h"
#include "CyUnit.h"

#ifdef FINAL_RELEASE
// Undefine OutputDebugString in final release builds
#undef OutputDebugString
#define OutputDebugString(x)
#endif //FINAL_RELEASE

/*  <advc.make> Cut from CvGameCoreUtils.h. The undefs make more sense to me here b/c
	this is the file that includes windows.h, which contains the bothersome min/max defines. */
#undef max
#undef min
#ifndef _USRDLL
// use non inline functions when not in the dll  advc.003f: Renamed all these
#define getMap	getMapExternal
#define getGridHeight	getGridHeightExternal
#define getGridWidth	getGridWidthExternal
#define isWrapY	isWrapYExternal
#define isWrapX	isWrapXExternal
#define plot	plotExternal
#define getX	getXExternal
#define getY	getYExternal
#endif // </advc.make>

#endif	// CvGameCoreDLL_h