//---------------------------------------------------------------------------------------
//
//  *****************   Civilization IV   ********************
//
//  FILE:    CvArtFileMgr.cpp
//
//  AUTHOR:  Jesse Smith / Mustafa Thamer	9/2004
//
//  PURPOSE: Interfaces with Civ4ArtDefines.xml to manage the paths of art files
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvArtFileMgr.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLUtilityIFaceBase.h"

// Macro for Building Art Info Maps
#if 0	// DEBUGGING
#define BUILD_INFO_MAP(map, infoArray, numInfos) \
{ \
	int iI; \
	for (iI = 0; iI < numInfos; iI++) \
	{ \
		char temp[256];	\
		sprintf(temp, "type = %s\n", infoArray(iI).getType()); \
		OutputDebugString(temp); \
		sprintf(temp, "description = %S\n", infoArray(iI).getDescription()); \
		OutputDebugString(temp); \
		(map)[infoArray(iI).getTag()] = &infoArray(iI); \
	} \
}
#else
#define BUILD_INFO_MAP(map, infoArray, numInfos) \
{ \
	int iI; \
	for (iI = 0; iI < numInfos; iI++) \
	{ \
	(map)[infoArray(iI).getTag()] = &infoArray(iI); \
	} \
}
#endif

//
// creates a derived artItem class which automatically registers itself with the ARTFILEMGR upon construction.
// creates a static var of that artItem type which constructs (and registers) at startup.
// creates a getFooArtInfo() function that searches the map based on the id provided and returns the artInfo struct or null.
//
#define ART_INFO_DEFN(name) \
\
class Cv##name##ArtInfoItem : public CvArtFileMgr::ArtInfoItem \
{ \
	void init() { ARTFILEMGR.m_map##name##ArtInfos = new CvArtFileMgr::ArtInfo##name##MapType; } \
	void deInit(); \
	void buildMap() \
	{ \
		BUILD_INFO_MAP(*ARTFILEMGR.m_map##name##ArtInfos, ARTFILEMGR.get##name##ArtInfo, ARTFILEMGR.getNum##name##ArtInfos()); \
	} \
}; \
\
static Cv##name##ArtInfoItem* g##name##ArtInfoItem; \
\
CvArtInfo##name* CvArtFileMgr::get##name##ArtInfo(char const* szArtDefineTag) const \
{ \
	FAssertMsg(szArtDefineTag, "NULL string on art info lookup?"); \
	ArtInfo##name##MapType::const_iterator it = m_map##name##ArtInfos->find(szArtDefineTag);\
	if (it == m_map##name##ArtInfos->end()) \
	{\
		char szErrorMsg[256]; \
		/* advc.001: Need to stringize for the error msg */ \
		sprintf(szErrorMsg, "get" #name "ArtInfo: %s was not found", szArtDefineTag); \
		FErrorMsg(szErrorMsg); \
		if (strcmp(szArtDefineTag, "ERROR") == 0) \
			return NULL; \
		return get##name##ArtInfo("ERROR"); \
	} \
	return it->second; \
} \
/* <advc> */\
TCHAR const* CvArtFileMgr::get##name##ArtPath(char const* szArtDefineTag) const \
{ \
	return get##name##ArtInfo(szArtDefineTag)->getPath(); \
} \
/* </advc> */ \
void Cv##name##ArtInfoItem::deInit() \
{ \
	SAFE_DELETE(ARTFILEMGR.m_map##name##ArtInfos); \
	for (uint i = 0; i < ARTFILEMGR.m_pa##name##ArtInfo.size(); i++) \
	{ \
		SAFE_DELETE(ARTFILEMGR.m_pa##name##ArtInfo[i]); \
	} \
	ARTFILEMGR.m_pa##name##ArtInfo.clear(); \
} \
CvArtInfo##name& CvArtFileMgr::get##name##ArtInfo(int i) { return *(m_pa##name##ArtInfo[i]); }

#define ART_INFO_INST(name) g##name##ArtInfoItem = new Cv##name##ArtInfoItem();


// Macros the declaration of the art file info maps
ART_INFO_DEFN(Asset);
ART_INFO_DEFN(Misc);
ART_INFO_DEFN(Unit);
ART_INFO_DEFN(Building);
ART_INFO_DEFN(Civilization);
ART_INFO_DEFN(Leaderhead);
ART_INFO_DEFN(Bonus);
ART_INFO_DEFN(Improvement);
ART_INFO_DEFN(Terrain);
ART_INFO_DEFN(Feature);
ART_INFO_DEFN(Movie);
ART_INFO_DEFN(Interface);

//----------------------------------------------------------------------------
//
//	FUNCTION:	GetInstance()
//
//	PURPOSE:	Get the instance of this class.
//
//----------------------------------------------------------------------------
static CvArtFileMgr* gs_ArtFileMgr = NULL;

CvArtFileMgr& CvArtFileMgr::GetInstance()
{
	if (gs_ArtFileMgr == NULL)
	{
		gs_ArtFileMgr = new CvArtFileMgr();

		ART_INFO_INST(Asset);
		ART_INFO_INST(Misc);
		ART_INFO_INST(Unit);
		ART_INFO_INST(Building);
		ART_INFO_INST(Civilization);
		ART_INFO_INST(Leaderhead);
		ART_INFO_INST(Bonus);
		ART_INFO_INST(Improvement);
		ART_INFO_INST(Terrain);
		ART_INFO_INST(Feature);
		ART_INFO_INST(Movie);
		ART_INFO_INST(Interface);
	}

	return *gs_ArtFileMgr;
}

// Initializes the maps
void CvArtFileMgr::Init()
{
	for(size_t i = 0; i < m_artInfoItems.size(); i++)
		m_artInfoItems[i]->init();
}

// Deletes the maps
void CvArtFileMgr::DeInit()
{
	for(size_t i = 0; i < m_artInfoItems.size(); i++)
		m_artInfoItems[i]->deInit();
}

// Reloads the XML and rebuilds the maps
void CvArtFileMgr::Reset()
{	// <advc.007b> Reloading Art Defines (Ctrl+Alt+R) is broken; would crash.
	if(GC.IsGraphicsInitialized())
		return; // </advc.007b>
	DeInit(); // Cleans Art Defines
	CvXMLLoadUtility XMLLoadUtility;
	XMLLoadUtility.SetGlobalArtDefines(); // Reloads/allocs Art Defines
	Init(); // reallocs maps
	buildArtFileInfoMaps();
}

// advc.enum: (for CvGlobals::infosReset)
void CvArtFileMgr::resetInfo()
{
	#define RESET_ART_INFO(Name) \
		for (int i = 0; i < getNum##Name##ArtInfos(); i++) \
			get##Name##ArtInfo(i).reset();
	// These are the ones that used to be added to CvGlobals::m_aInfoVectors
	RESET_ART_INFO(Misc);
	RESET_ART_INFO(Unit);
	RESET_ART_INFO(Building);
	RESET_ART_INFO(Civilization);
	RESET_ART_INFO(Leaderhead);
	RESET_ART_INFO(Bonus);
	RESET_ART_INFO(Improvement);
	RESET_ART_INFO(Terrain);
	RESET_ART_INFO(Feature);
	RESET_ART_INFO(Movie);
	RESET_ART_INFO(Interface);
	#undef RESET_ART_INFO
}

// Builds the art file maps
void CvArtFileMgr::buildArtFileInfoMaps()
{
	for(size_t i = 0; i < m_artInfoItems.size(); i++)
		m_artInfoItems[i]->buildMap();
}
