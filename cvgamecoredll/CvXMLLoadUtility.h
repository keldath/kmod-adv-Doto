#pragma once

//  FILE:    CvXMLLoadUtility.h
//  AUTHOR:  Eric MacDonald  --  8/2003
//  PURPOSE: Group of functions to load in the xml files for Civilization 4
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
/*  advc: Deleted most of the comments in this file and moved some to the
	implementation files (CvXMLLoadUtility.cpp, CvXMLLoadUtilityInit.cpp,
	CvXMLLoadUtilityGet.cpp and CvXMLLoadUtilitySet.cpp). Also deleted many
	non-informative comments from the implementation files. */
#ifndef XML_LOAD_UTILITY_H
#define XML_LOAD_UTILITY_H

class FXmlSchemaCache;
class FXml;
class CvGameText;
class CvCacheObject;
class CvImprovementBonusInfo;


class CvXMLLoadUtility
{
public:
	DllExport CvXMLLoadUtility();
	DllExport ~CvXMLLoadUtility(void);

	bool CreateFXml();
	void DestroyFXml();

	DllExport bool LoadPostMenuGlobals();
	DllExport bool LoadPreMenuGlobals();
	DllExport bool LoadBasicInfos();
	DllExport bool LoadPlayerOptions();
	DllExport bool LoadGraphicOptions();
	// <advc.003v>
	bool LoadOptionalGlobals();
	bool LoadThroneRoomInfo(); // </advc.003v>

	bool ReadGlobalDefines(const TCHAR* szXMLFileName, CvCacheObject* cache);
	
	DllExport bool SetGlobalDefines();
	DllExport bool SetGlobalTypes();
	// advc.003j: Unused and has no definition
	//bool SetGlobals(); // calls various functions that load xml files that in turn load relevant global variables
	DllExport bool SetPostGlobalsGlobalDefines();
	DllExport void CleanUpGlobalVariables();

	DllExport void ResetLandscapeInfo();
	DllExport bool SetupGlobalLandscapeInfo();
	DllExport bool SetGlobalArtDefines();
	DllExport bool LoadGlobalText();
	bool SetHelpText();
	DllExport void ResetGlobalEffectInfo();

	// for progress bars
	typedef void (*ProgressCB)(int iStepNum, int iTotalSteps, const char* szMessage);
	static int GetNumProgressSteps();
	// advc (note): unused
	void RegisterProgressCB(ProgressCB cbFxn);

	bool SkipToNextVal();

	void MapChildren();	// call this before GetChildXMLValByName for fast search
	bool GetChildXmlValByName(std::string& r, TCHAR const* szName, char const* szDefault = NULL);
	bool GetChildXmlValByName(std::wstring& r, TCHAR const* szName, wchar const* szDefault = NULL);
	bool GetChildXmlValByName(char* r, TCHAR const* szName, char const* szDefault = NULL);
	bool GetChildXmlValByName(wchar* r, TCHAR const* szName, wchar const* szDefault = NULL);
	/*  (advc: Returning by reference would be nicer than by pointer, but I don't want to change
		hundreds of call locations.) */
	bool GetChildXmlValByName(int* r, TCHAR const* szName,
			/*  advc.006b: Was 0. Instead use a value that no one wants to use so that
				the callee can check if the param was set. */
			int iDefault = MIN_INT);
	bool GetChildXmlValByName(float* r, TCHAR const* szName,
			float fDefault = FLT_MIN); // advc.006b: was 0.0f
	bool GetChildXmlValByName(bool* r, TCHAR const* szName,
			/*  advc.006b: Caller will have to set this to false to avoid an error
				if szName isn't found */
			bool bMandatory = true,
			bool bDefault = false);
	/*  advc.006b: Unused for now. Can use this to disable the assertions added to
		GetChildXmlValByName temporarily, e.g. while loading a CvInfo element that
		doesn't have tags which are normally mandatory. */
	void setAssertMandatoryEnabled(bool b);

	bool LoadCivXml(FXml* pFXml, const TCHAR* szFilename);

	bool GetXmlVal(std::wstring& r, wchar const* szDefault = NULL);
	bool GetXmlVal(std::string& r, char const* szDefault = NULL);
	bool GetXmlVal(wchar* r, wchar const* szDefault = NULL);
	bool GetXmlVal(char* r, char const* szDefault = NULL);
	// advc: Return by reference (was pointer)
	bool GetXmlVal(int& r, int iDefault = 0);
	bool GetXmlVal(float& r, float fDefault = 0.0f);
	bool GetXmlVal(bool& r, bool bDefault = false);

	bool GetNextXmlVal(std::string& r, char const* szDefault = NULL);
	bool GetNextXmlVal(std::wstring& r, wchar const* szDefault = NULL);
	bool GetNextXmlVal(char* r, char const* szDefault = NULL);
	bool GetNextXmlVal(wchar* r, wchar const* szDefault = NULL);
	// advc: Return by reference (was pointer)
	bool GetNextXmlVal(int& r, int iDefault = 0);
	bool GetNextXmlVal(float& r, float fDefault = 0.0f);
	bool GetNextXmlVal(bool& r, bool bDefault = false);

	bool GetChildXmlVal(std::string& r, char const* szDefault = NULL);
	bool GetChildXmlVal(std::wstring& r, wchar const* szDefault = NULL);
	bool GetChildXmlVal(char* r, char const* szDefault = NULL);
	bool GetChildXmlVal(wchar* r, wchar const* szDefault = NULL);
	// advc: Return by reference (was pointer)
	bool GetChildXmlVal(int& r, int iDefault = 0);
	bool GetChildXmlVal(float& r, float fDefault = 0.0f);
	bool GetChildXmlVal(bool& r, bool bDefault = false);

	#ifdef _USRDLL
	FXml* GetXML() { return m_pFXml; }
	#endif

	int SetYields(int** ppiYield);

	#ifdef _USRDLL
	template <class T>
	int SetCommerce(T** ppiCommerce);
	#endif

	void SetFeatureStruct(int** ppiFeatureTech, int** ppiFeatureTime, int** ppiFeatureProduction, bool** ppbFeatureRemove);
	void SetImprovementBonuses(CvImprovementBonusInfo** ppImprovementBonus);

	static int FindInInfoClass(const TCHAR* pszVal, bool hideAssert = false);

	#ifdef _USRDLL
	template <class T>
	void InitList(T **ppList, int iListLen, T val = 0);
	#endif
	void InitStringList(CvString **ppszList, int iListLen, CvString szString);

	void InitImprovementBonusList(CvImprovementBonusInfo** ppImprovementBonus, int iListLen);
	void InitBuildingDefaults(int **ppiDefaults);
	void InitUnitDefaults(int **ppiDefaults);
	void Init2DIntList(int*** pppiList, int iSizeX, int iSizeY);
	void Init2DFloatList(float*** pppfList, int iSizeX, int iSizeY);
	void Init2DDirectionTypesList(DirectionTypes*** pppiList, int iSizeX, int iSizeY);
	void InitPointerIntList(int*** pppiList, int iSizeX);
	void InitPointerFloatList(float*** pppfList, int iSizeX);

	// allocate and initialize a list from a tag pair in the xml
	// advc.003x: Unused param iInfoBaseSize removed from these four
	template<typename T> // advc: Replaced three functions with a template (inspired by rheinig's mod)
	void SetVariableListTagPair(T** pptList, const TCHAR* szRootTagName,
			int iInfoBaseLength, T tDefaultListVal = 0);
	void SetVariableListTagPair(CvString** ppszList, const TCHAR* szRootTagName,
			int iInfoBaseLength, CvString szDefaultListVal = "");
/************************************************************************************************/
/* RevDCM  XMLloading                             05/05/10             phungus420               */
/*                                                                                              */
/*   added by f1 - might not be needed - xml modularload?                                                                                           */
//Well, who knows. Live and let live. :) - Amen brother!
/************************************************************************************************/
	// allocate and initialize a list from a tag pair in the xml by tag name
	void SetVariableListTagPairRevDCM(int **ppiList, const TCHAR* szRootTagName,
		int iInfoBaseSize, int iInfoBaseLength,
		const TCHAR* szValueTagName, int iValueInfoBaseLength, int iDefaultListVal = -1);
/************************************************************************************************/
/* RevDCM	                                 END                                                */
/************************************************************************************************/	

	void SetVariableListTagPair(int **ppiList, const TCHAR* szRootTagName,
		CvString* m_paszTagList, int iTagListLength, int iDefaultListVal = 0);
	void SetVariableListTagPairForAudioScripts(int **ppiList, const TCHAR* szRootTagName,
		CvString* m_paszTagList, int iTagListLength, int iDefaultListVal = -1);
	void SetVariableListTagPairForAudioScripts(int **ppiList, const TCHAR* szRootTagName,
		int iInfoBaseLength, int iDefaultListVal = -1);
	void SetVariableListTagPair(bool **ppbList, const TCHAR* szRootTagName,
		CvString* m_paszTagList, int iTagListLength, bool bDefaultListVal = false);
	void SetVariableListTagPair(CvString **ppszList, const TCHAR* szRootTagName,
		CvString* m_paszTagList, int iTagListLength, CvString szDefaultListVal = "");

	CvWString HotKeyFromDescription(const TCHAR* pszHotKey, bool bShift = false, bool bAlt = false, bool bCtrl = false);
	bool SetAndLoadVar(int** ppiVar, int iDefault=0);
	bool SetStringList(CvString** ppszStringArray, int* piSize);
	int GetHotKeyInt(const TCHAR* pszHotKeyVal);
/************************************************************************************************/
/* TGA_INDEXATION                          01/21/08                                MRGENIE      */
/*                                                                                              */
/* reorganizing the Corporations and Religions vectors                                          */
/************************************************************************************************/
	template <class T>
	void ArrangeTGA(std::vector<T*>& aInfos, const char* szInfo);
	template <class T>
	void AddTGABogus(std::vector<T*>& aInfos, const char* szInfo);
	void cleanTGA();
	template <class T>
	void RemoveTGABogusReligion(std::vector<T*>& aInfos);
	template <class T>
	void RemoveTGABogusCorporation(std::vector<T*>& aInfos);
/************************************************************************************************/
/* TGA_INDEXATION                          END                                                  */
/************************************************************************************************/


private:
	FXml* m_pFXml;
	// keep a single schema cache, instead of loading the same schemas multiple times
	FXmlSchemaCache* m_pSchemaCache;
	int m_iCurProgressStep;
	ProgressCB m_pCBFxn;

	bool m_bEventsLoaded, m_bThroneRoomLoaded; // advc.003v
	// <advc.006b>
	bool m_bAssertMandatory;
	static CvString szAssertMsg;
	// </advc.006b>

	void UpdateProgressCB(const char* szMessage=NULL);

	void SetGlobalStringArray(CvString** ppszString, char* szTagName, int* iNumVals, bool bUseEnum = false);
	void SetDiplomacyCommentTypes(CvString** ppszString, int* iNumVals);
	//void MakeMaskFromString(unsigned int *puiMask, char* szMask); // advc.003j (unused)
	//void SetGlobalUnitScales(float& fLargeScale, float& fSmallScale, char const* szTagName); // advc.003j (unused)

	#ifdef _USRDLL
	template <class T>
		void SetGlobalDefine(const char* szDefineName, T*& piDefVal)
	{
		GC.getDefinesVarSystem()->GetValue(szDefineName, piDefVal); }
	// template which can handle all info classes
	// a dynamic value for the list size
	template <class T>
	void SetGlobalClassInfo(std::vector<T*>& aInfos, const char* szTagName, bool bTwoPass);
	template <class T>
	void LoadGlobalClassInfo(std::vector<T*>& aInfos, const char* szFileRoot,
			const char* szFileDirectory, const char* szXmlPath, bool bTwoPass,
			CvCacheObject* (CvDLLUtilityIFaceBase::*pArgFunction)(const TCHAR*) = NULL);
	#endif
	void SetDiplomacyInfo(std::vector<CvDiplomacyInfo*>& DiploInfos, const char* szTagName);
	void LoadDiplomacyInfo(std::vector<CvDiplomacyInfo*>& DiploInfos, const char* szFileRoot,
			const char* szFileDirectory, const char* szXmlPath,
			CvCacheObject* (CvDLLUtilityIFaceBase::*pArgFunction) (const TCHAR*));
	//
	// special cases of set class info which don't use the template because of extra code they have
	//
	void SetGlobalActionInfo();
	// advc:
	template <class T, typename E> void setActionData(T& kInfo, int iAction, E eMissionCommand);
	void SetGlobalAnimationPathInfo(CvAnimationPathInfo** ppAnimationPathInfo, char* szTagName, int* iNumVals);
	//void SetGameText(const char* szTextGroup, const char* szTagName);
	void SetGameText(const char* szTextGroup, const char* szTagName, const std::string& language_name); // K-Mod

	CvWString KeyStringFromKBCode(const TCHAR* pszHotKey);

	void orderHotkeyInfo(int** ppiSortedIndex, int* pHotkeyIndex, int iLength);
	/*	<advc.006g> (The BtS code sometimes said "XML Error", sometimes "XML Load Error"
		not sure if that's meanigful, but I'm going to preserve it.)*/
	enum XMLErrorTypes
	{
		GENERAL_XML_ERROR,
		XML_LOAD_ERROR,
	};
	static void errorMessage(char const* szMessage, XMLErrorTypes eErrType = GENERAL_XML_ERROR);
	// </advc.006g>
	void logMsg(char* format, ...);
};

#ifdef _USRDLL
// inlines / templates ...

template <class T>
void CvXMLLoadUtility::InitList(T **ppList, int iListLen, T val)
{
	FAssertMsg( 0 <= iListLen, "list size to allocate is less than 0");
	*ppList = new T[iListLen];
	for (int i=0;i<iListLen;i++)
		(*ppList)[i] = val;
}

template <class T>
int CvXMLLoadUtility::SetCommerce(T** ppbCommerce)
{
	T *pbCommerce; // local pointer for the Commerce memory
	// Skip any comments and stop at the next value we might want
	if (!SkipToNextVal())
		return 0;

	int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
	InitList(ppbCommerce, NUM_COMMERCE_TYPES);
	pbCommerce = *ppbCommerce;
	if (iNumSibs > 0)
	{
		if (GetChildXmlVal(pbCommerce[0]))
		{
			FAssertMsg((iNumSibs <= NUM_COMMERCE_TYPES) , "For loop iterator is greater than array size");
			// loop through all the siblings, we start at 1 since we already have the first value
			for (int i = 1; i < iNumSibs; i++)
			{
				if (!GetNextXmlVal(pbCommerce[i]))
					break;
			}
			gDLL->getXMLIFace()->SetToParent(m_pFXml);
		}
	}
	return iNumSibs;
}
#endif // _USRDLL

#endif
