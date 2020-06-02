//
// XML Set functions
//

#include "CvGameCoreDLL.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvXMLLoadUtility.h"
#include "CvGameTextMgr.h"
#include "CvInfo_All.h"
#include "CvGameAI.h" // advc.104x
#include "FVariableSystem.h"
// <advc> Overwrite the definition in CvGlobals.h b/c a const GC is no use here
#undef GC
#define GC CvGlobals::getInstance() // </advc>

// Macro for Setting Global Art Defines
// advc.003j: unused
//#define INIT_XML_GLOBAL_LOAD(xmlInfoPath, infoArray, numInfos)  SetGlobalClassInfo(infoArray, xmlInfoPath, numInfos);

// read the global defines from a specific file
bool CvXMLLoadUtility::ReadGlobalDefines(const TCHAR* szXMLFileName, CvCacheObject* cache)
{
	// advc: Handle successful read upfront
	if (gDLL->cacheRead(cache, szXMLFileName)) // src data file name
	{
		logMsg("Read GlobalDefines from cache");
		return true;
	}
	// load normally
	if (!CreateFXml())
		return false;

	bool bLoaded = LoadCivXml(m_pFXml, szXMLFileName);
	if (!bLoaded)
	{
		char	szMessage[1024];
		sprintf(szMessage, "LoadXML call failed for %s.\nCurrent XML file is: %s", szXMLFileName, GC.getCurrentXMLFile().GetCString());
		errorMessage(szMessage, XML_LOAD_ERROR);
	}

	if (bLoaded)
	{
		// locate the first define tag in the xml
		if (gDLL->getXMLIFace()->LocateNode(m_pFXml,"Civ4Defines/Define"))
		{
			// get the number of other Define tags in the xml file
			int iNumDefines = gDLL->getXMLIFace()->GetNumSiblings(m_pFXml);
			// add one to the total in order to include the current Define tag
			iNumDefines++;

			// loop through all the Define tags
			for (int i = 0; i < iNumDefines; i++)
			{
				char szNodeType[256]; // holds the type of the current node
				char szName[256];

				// Skip any comments and stop at the next value we might want
				if (SkipToNextVal())
				{
					// call the function that sets the FXml pointer to the first non-comment child of
					// the current tag and gets the value of that new node
					if (GetChildXmlVal(szName))
					{
						// set the FXml pointer to the next sibling of the current tag``
						if (gDLL->getXMLIFace()->NextSibling(GetXML()))
						{
							// Skip any comments and stop at the next value we might want
							if (SkipToNextVal())
							{
								// if we successfuly get the node type for the current tag
								if (gDLL->getXMLIFace()->GetLastLocatedNodeType(GetXML(),szNodeType))
								{
									if (strcmp(szNodeType,"")!=0)
									{
										// if the node type of the current tag is a float then
										if (strcmp(szNodeType,"float")==0)
										{
											// get the float value for the define
											float fVal;
											GetXmlVal(fVal);
											GC.getDefinesVarSystem()->SetValue(szName, fVal);
										}
										// else if the node type of the current tag is an int then
										else if (strcmp(szNodeType,"int")==0)
										{
											// get the int value for the define
											int iVal;
											GetXmlVal(iVal);
											GC.getDefinesVarSystem()->SetValue(szName, iVal);
										}
										// else if the node type of the current tag is a boolean then
										else if (strcmp(szNodeType,"boolean")==0)
										{
											// get the boolean value for the define
											bool bVal;
											GetXmlVal(bVal);
											GC.getDefinesVarSystem()->SetValue(szName, bVal);
										}
										// otherwise we will assume it is a string/text value
										else
										{
											char szVal[256];
											// get the string/text value for the define
											GetXmlVal(szVal);
											GC.getDefinesVarSystem()->SetValue(szName, szVal);
										}
									}
									// otherwise we will default to getting the string/text value for the define
									else
									{
										char szVal[256];
										// get the string/text value for the define
										GetXmlVal(szVal);
										GC.getDefinesVarSystem()->SetValue(szName, szVal);
									}
								}
							}
						}
						// since we are looking at the children of a Define tag we will need to go up
						// one level so that we can go to the next Define tag.
						gDLL->getXMLIFace()->SetToParent(GetXML());
					}
				}
				// set the FXml pointer to the sibling of the current tag, which should be the next Define tag
				if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
					break;
			}

			// write global defines info to cache
			// advc.003i: Disabled
			/*bool bOk = gDLL->cacheWrite(cache);
			if (!bOk) {
				char	szMessage[1024];
				sprintf(szMessage, "Failed writing to global defines cache.\nCurrent XML file is: %s", GC.getCurrentXMLFile().GetCString());
				gDLL->MessageBox(szMessage, "XML Caching Error");
			}
			else logMsg("Wrote GlobalDefines to cache");*/
		}
	}
	gDLL->getXMLIFace()->DestroyFXml(m_pFXml);
	return true;
}

// Load GlobalDefines from XML
bool CvXMLLoadUtility::SetGlobalDefines()
{
	UpdateProgressCB("GlobalDefines");

	//
	// use disk cache if possible.
	// if no cache or cache is older than xml file, use xml file like normal, else read from cache
	//

	CvCacheObject* cache = gDLL->createGlobalDefinesCacheObject("GlobalDefines.dat");	// cache file name

	if (!ReadGlobalDefines("xml\\GlobalDefines.xml", cache))
		return false;

	if (!ReadGlobalDefines("xml\\GlobalDefinesAlt.xml", cache))
		return false;

	if (!ReadGlobalDefines("xml\\PythonCallbackDefines.xml", cache))
		return false;

	// <advc.009> Load additional GlobalDefines files
	if(!ReadGlobalDefines("xml\\GlobalDefines_devel.xml", cache))
		return false;
	if(!ReadGlobalDefines("xml\\GlobalDefines_advc.xml", cache))
		return false; // </advc.009>
/*************************************************************************************************/
/** TGA_INDEXATION                          02/19/08                                MRGENIE      */
/**                                                                                              */
/** reading the Defines to know the modded TGA icons                                             */
/*************************************************************************************************/
	if (!ReadGlobalDefines("xml\\GameFont_GlobalDefines.xml", cache))
	{
		FAssertMsg(false,"The \"GameFont_GlobalDefines.xml\" could not be found");
	}
/*************************************************************************************************/
/** TGA_INDEXATION                          END                                                  */
/*************************************************************************************************/
	// BETTER_BTS_AI_MOD, XML Options, 02/21/10, jdog5000: START
	ReadGlobalDefines("xml\\BBAI_Game_Options_GlobalDefines.xml", cache);
	// advc.104x: Removed the BBAI prefix from the file name
	ReadGlobalDefines("xml\\AI_Variables_GlobalDefines.xml", cache);
	ReadGlobalDefines("xml\\TechDiffusion_GlobalDefines.xml", cache);
	ReadGlobalDefines("xml\\LeadFromBehind_GlobalDefines.xml", cache);
	// BETTER_BTS_AI_MOD: END

	if (gDLL->isModularXMLLoading())
	{
		std::vector<CvString> aszFiles;
		gDLL->enumerateFiles(aszFiles, "modules\\*_GlobalDefines.xml");
		for (std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
		{
			if (!ReadGlobalDefines(*it, cache))
				return false;
		}

		std::vector<CvString> aszModularFiles;
		gDLL->enumerateFiles(aszModularFiles, "modules\\*_PythonCallbackDefines.xml");
		for (std::vector<CvString>::iterator it = aszModularFiles.begin(); it != aszModularFiles.end(); ++it)
		{
			if (!ReadGlobalDefines(*it, cache))
				return false;
		}
	}
	gDLL->destroyCache(cache);

	GC.cacheGlobals();
	return true;
}

// Loads those global defines that need to reference a global variable loaded by SetGlobalDefines
bool CvXMLLoadUtility::SetPostGlobalsGlobalDefines()
{
	const char* szVal=NULL; // holds the string value from the define queue
	int idx;
	if (GC.getDefinesVarSystem()->GetSize() > 0)
	{
		SetGlobalDefine("LAND_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("LAND_TERRAIN", idx);

		SetGlobalDefine("DEEP_WATER_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEEP_WATER_TERRAIN", idx);
		GC.setWATER_TERRAIN(false, idx); // advc.opt

		SetGlobalDefine("SHALLOW_WATER_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("SHALLOW_WATER_TERRAIN", idx);
		GC.setWATER_TERRAIN(true, idx); // advc.opt
		//GWMod Start M.A.
		SetGlobalDefine("FROZEN_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("FROZEN_TERRAIN", idx);

		SetGlobalDefine("COLD_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("COLD_TERRAIN", idx);

		SetGlobalDefine("TEMPERATE_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("TEMPERATE_TERRAIN", idx);

		SetGlobalDefine("DRY_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DRY_TERRAIN", idx);

		SetGlobalDefine("BARREN_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BARREN_TERRAIN", idx);

		SetGlobalDefine("COLD_FEATURE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("COLD_FEATURE", idx);

		SetGlobalDefine("TEMPERATE_FEATURE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("TEMPERATE_FEATURE", idx);

		SetGlobalDefine("WARM_FEATURE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("WARM_FEATURE", idx);
		//GWMod end M.A.

		SetGlobalDefine("LAND_IMPROVEMENT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("LAND_IMPROVEMENT", idx);

		SetGlobalDefine("WATER_IMPROVEMENT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("WATER_IMPROVEMENT", idx);

		SetGlobalDefine("RUINS_IMPROVEMENT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("RUINS_IMPROVEMENT", idx);
		GC.setRUINS_IMPROVEMENT(idx); // advc.opt

		SetGlobalDefine("NUKE_FEATURE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NUKE_FEATURE", idx);

		SetGlobalDefine("GLOBAL_WARMING_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("GLOBAL_WARMING_TERRAIN", idx);

		SetGlobalDefine("CAPITAL_BUILDINGCLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("CAPITAL_BUILDINGCLASS", idx);

		SetGlobalDefine("DEFAULT_SPECIALIST", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_SPECIALIST", idx);
		GC.setDEFAULT_SPECIALIST(idx); // advc.opt

		SetGlobalDefine("INITIAL_CITY_ROUTE_TYPE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("INITIAL_CITY_ROUTE_TYPE", idx);

		SetGlobalDefine("STANDARD_HANDICAP", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_HANDICAP", idx);

		SetGlobalDefine("STANDARD_HANDICAP_QUICK", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_HANDICAP_QUICK", idx);

		SetGlobalDefine("STANDARD_GAMESPEED", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_GAMESPEED", idx);

		SetGlobalDefine("STANDARD_TURNTIMER", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_TURNTIMER", idx);

		SetGlobalDefine("STANDARD_CLIMATE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_CLIMATE", idx);

		SetGlobalDefine("STANDARD_SEALEVEL", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_SEALEVEL", idx);

		SetGlobalDefine("STANDARD_ERA", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_ERA", idx);

		SetGlobalDefine("STANDARD_CALENDAR", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_CALENDAR", idx);

		SetGlobalDefine("AI_HANDICAP", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("AI_HANDICAP", idx);

		SetGlobalDefine("BARBARIAN_HANDICAP", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BARBARIAN_HANDICAP", idx);

		SetGlobalDefine("BARBARIAN_CIVILIZATION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BARBARIAN_CIVILIZATION", idx);

		SetGlobalDefine("BARBARIAN_LEADER", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BARBARIAN_LEADER", idx);

		return true;
	}

	char	szMessage[1024];
	sprintf(szMessage, "Size of Global Defines is not greater than 0.\nCurrent XML file is: %s", GC.getCurrentXMLFile().GetCString());
	errorMessage(szMessage, XML_LOAD_ERROR);

	return false;
}

// Load GlobalTypes.xml
bool CvXMLLoadUtility::SetGlobalTypes()
{
	UpdateProgressCB("GlobalTypes");

	if (!CreateFXml())
		return false;

	bool bLoaded = LoadCivXml(m_pFXml, "xml/GlobalTypes.xml");
	if (!bLoaded)
	{
		char	szMessage[1024];
		sprintf(szMessage, "LoadXML call failed for GlobalTypes.xml.\nCurrent XML file is: %s", GC.getCurrentXMLFile().GetCString());
		errorMessage(szMessage, XML_LOAD_ERROR);
	}

	if (bLoaded)
	{
		SetGlobalStringArray(&GC.getAnimationOperatorTypes(), "Civ4Types/AnimationOperatorTypes/AnimationOperatorType", &GC.getNumAnimationOperatorTypes());
		int iEnumVal = NUM_FUNC_TYPES;
		SetGlobalStringArray(&GC.getFunctionTypes(), "Civ4Types/FunctionTypes/FunctionType", &iEnumVal, true);
		SetGlobalStringArray(&GC.getFlavorTypes(), "Civ4Types/FlavorTypes/FlavorType", &GC.getNumFlavorTypes());
		SetGlobalStringArray(&GC.getArtStyleTypes(), "Civ4Types/ArtStyleTypes/ArtStyleType", &GC.getNumArtStyleTypes());
		iEnumVal = NUM_CITYSIZE_TYPES; // advc
		SetGlobalStringArray(&GC.getCitySizeTypes(), "Civ4Types/CitySizeTypes/CitySizeType", &iEnumVal, true);
		iEnumVal = NUM_CONTACT_TYPES;
		SetGlobalStringArray(&GC.getContactTypes(), "Civ4Types/ContactTypes/ContactType", &iEnumVal, true);
		iEnumVal = NUM_DIPLOMACYPOWER_TYPES;
		SetGlobalStringArray(&GC.getDiplomacyPowerTypes(), "Civ4Types/DiplomacyPowerTypes/DiplomacyPowerType", &iEnumVal, true);
		iEnumVal = NUM_AUTOMATE_TYPES;
		SetGlobalStringArray(&GC.getAutomateTypes(), "Civ4Types/AutomateTypes/AutomateType", &iEnumVal, true);
		iEnumVal = NUM_DIRECTION_TYPES;
		SetGlobalStringArray(&GC.getDirectionTypes(), "Civ4Types/DirectionTypes/DirectionType", &iEnumVal, true);
		SetGlobalStringArray(&GC.getFootstepAudioTypes(), "Civ4Types/FootstepAudioTypes/FootstepAudioType", &GC.getNumFootstepAudioTypes());

		gDLL->getXMLIFace()->SetToParent(m_pFXml);
		gDLL->getXMLIFace()->SetToParent(m_pFXml);
		SetVariableListTagPair(&GC.getFootstepAudioTags(), "FootstepAudioTags", GC.getFootstepAudioTypes(), GC.getNumFootstepAudioTypes(), "");
	}
	DestroyFXml();
	return true;
}

// Creates a full list of Diplomacy Comments
void CvXMLLoadUtility::SetDiplomacyCommentTypes(CvString** ppszString, int* iNumVals)
{
	FAssertMsg(false, "should never get here");
}

// Initialize the appropriate variables at CvGlobals with the values in Terrain\Civ4TerrainInfos.xml
bool CvXMLLoadUtility::SetupGlobalLandscapeInfo()
{
	if (!CreateFXml())
		return false;
	LoadGlobalClassInfo(GC.m_paLandscapeInfo, "CIV4TerrainSettings", "Terrain", "Civ4TerrainSettings/LandscapeInfos/LandscapeInfo", false);
	DestroyFXml();
	return true;
}

// Initialize the appropriate variables located at CvGlobals with the values in Civ4ArtDefines.xml
bool CvXMLLoadUtility::SetGlobalArtDefines()
{
	if (!CreateFXml())
		return false;

	LoadGlobalClassInfo(ARTFILEMGR.getInterfaceArtInfo(), "CIV4ArtDefines_Interface", "Art", "Civ4ArtDefines/InterfaceArtInfos/InterfaceArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getMovieArtInfo(), "CIV4ArtDefines_Movie", "Art", "Civ4ArtDefines/MovieArtInfos/MovieArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getMiscArtInfo(), "CIV4ArtDefines_Misc", "Art", "Civ4ArtDefines/MiscArtInfos/MiscArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getUnitArtInfo(), "CIV4ArtDefines_Unit", "Art", "Civ4ArtDefines/UnitArtInfos/UnitArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getBuildingArtInfo(), "CIV4ArtDefines_Building", "Art", "Civ4ArtDefines/BuildingArtInfos/BuildingArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getCivilizationArtInfo(), "CIV4ArtDefines_Civilization", "Art", "Civ4ArtDefines/CivilizationArtInfos/CivilizationArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getLeaderheadArtInfo(), "CIV4ArtDefines_Leaderhead", "Art", "Civ4ArtDefines/LeaderheadArtInfos/LeaderheadArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getBonusArtInfo(), "CIV4ArtDefines_Bonus", "Art", "Civ4ArtDefines/BonusArtInfos/BonusArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getImprovementArtInfo(), "CIV4ArtDefines_Improvement", "Art", "Civ4ArtDefines/ImprovementArtInfos/ImprovementArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getTerrainArtInfo(), "CIV4ArtDefines_Terrain", "Art", "Civ4ArtDefines/TerrainArtInfos/TerrainArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getFeatureArtInfo(), "CIV4ArtDefines_Feature", "Art", "Civ4ArtDefines/FeatureArtInfos/FeatureArtInfo", false);

	DestroyFXml();

	return true;
}

// Handles all Global Text Infos
bool CvXMLLoadUtility::LoadGlobalText()
{
	CvCacheObject* cache = gDLL->createGlobalTextCacheObject("GlobalText.dat");	// cache file name
	// advc: Handle successful read upfront
	if (gDLL->cacheRead(cache)) {
		logMsg("Read GlobalText from cache");
		gDLL->destroyCache(cache);
		return true;
	}

	if (!CreateFXml())
		return false;

	// load all files in the xml text directory ...

	std::vector<CvString> aszFiles;
	std::vector<CvString> aszModfiles;

	gDLL->enumerateFiles(aszFiles, "xml\\text\\*.xml");

	// K-Mod. Text files from mods may not have the same set of language as the base game.
	// When such a mismatch occurs, we cannot simply rely on "getCurrentLanguage()" to give us the correct text from the mod file. We should instead check the xml tags.
	// So, before we start loading text, we need to extract the current name of our language tag. This isn't as easy as I'd like. Here's what I'm going to do:
	// CIV4GameText_Misc1.xml contains the text for the language options dropdown menu in the settings screen; so I'm going to assume that particular text file is
	// well formed, and I'm going to use it to determine the current language name. (Note: I'd like to use the names from TXT_KEY_LANGUAGE_#, but that text isn't easy to access.)
	// label text for the currently selected language -- that should correspond to the xml label used for that language.
	std::string langauge_name;
	if (LoadCivXml(m_pFXml, "xml\\text\\CIV4GameText_Misc1.xml"))
	{
		bool bValid = true;
		bValid = bValid && gDLL->getXMLIFace()->LocateNode(m_pFXml, "Civ4GameText/TEXT");
		bValid = bValid && gDLL->getXMLIFace()->SetToChild(m_pFXml);

		if (bValid)
		{
			const int& iLanguage = GAMETEXT.getCurrentLanguage();
			const int iMax = GC.getDefineINT("MAX_NUM_LANGUAGES");
			int i;
			for (i = 0; i < iMax; i++)
			{
				SkipToNextVal();

				if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
					break;
				if (i == iLanguage)
				{
					char buffer[1024]; // no way to determine max tag name size. .. This is really bad; but what can I do about it?
					if (gDLL->getXMLIFace()->GetLastLocatedNodeTagName(m_pFXml, buffer))
					{
						buffer[1023] = 0; // just in case the buffer isn't even terminated!
						langauge_name.assign(buffer);
					}
				}
			}
			// this is stupid...
			// the number of languages is a static private variable which can only be set by a non-static function.
			CvGameText dummy;
			dummy.setNumLanguages(i);
		}
	}

	// Remove duplicate files. (Both will be loaded from the mod folder anyway, so this will save us some time.)
	// However, we must not disturb the order of the list, because it is important that the modded files overrule the unmodded files.
	for(std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
	{
		std::vector<CvString>::iterator jt = it+1;
		while (jt != aszFiles.end())
		{
			if (it->CompareNoCase(*jt) == 0)
				jt = aszFiles.erase(jt);
			else
				++jt;
		}
	}
	// K-Mod end

	if (gDLL->isModularXMLLoading())
	{
		gDLL->enumerateFiles(aszModfiles, "modules\\*_CIV4GameText.xml");
		aszFiles.insert(aszFiles.end(), aszModfiles.begin(), aszModfiles.end());
	}
	bool bLoaded = false;
	for(std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
	{
		bLoaded = LoadCivXml(m_pFXml, *it); // Load the XML
		if (!bLoaded)
		{
			char	szMessage[1024];
			sprintf(szMessage, "LoadXML call failed for %s.\nCurrent XML file is: %s", it->c_str(), GC.getCurrentXMLFile().GetCString());
			errorMessage(szMessage, XML_LOAD_ERROR);
		}
		if (bLoaded)
		{
			// if the xml is successfully validated
			SetGameText("Civ4GameText", "Civ4GameText/TEXT", langauge_name);
		}
	}

	DestroyFXml();

	// write global text info to cache
	// advc.003i: Disabled
	/*bool bOk = gDLL->cacheWrite(cache);
	if (!bOk) {
		char	szMessage[1024];
		sprintf(szMessage, "Failed writing to Global Text cache.\nCurrent XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Caching Error");
	}
	if (bOk)
		logMsg("Wrote GlobalText to cache");*/

	gDLL->destroyCache(cache);

	return true;
}

bool CvXMLLoadUtility::LoadBasicInfos()
{
	if (!CreateFXml())
		return false;

	/* advc: Replaced all GC.get...Info() calls with direct accesses; see the
		comment under the friend declaration in CvGlobals.h. */

	LoadGlobalClassInfo(GC.m_paConceptInfo, "CIV4BasicInfos", "BasicInfos", "Civ4BasicInfos/ConceptInfos/ConceptInfo", false);
	LoadGlobalClassInfo(GC.m_paNewConceptInfo, "CIV4NewConceptInfos", "BasicInfos", "Civ4NewConceptInfos/NewConceptInfos/NewConceptInfo", false);
	LoadGlobalClassInfo(GC.m_paCityTabInfo, "CIV4CityTabInfos", "BasicInfos", "Civ4CityTabInfos/CityTabInfos/CityTabInfo", false);
	LoadGlobalClassInfo(GC.m_paCalendarInfo, "CIV4CalendarInfos", "BasicInfos", "Civ4CalendarInfos/CalendarInfos/CalendarInfo", false);
	LoadGlobalClassInfo(GC.m_paSeasonInfo, "CIV4SeasonInfos", "BasicInfos", "Civ4SeasonInfos/SeasonInfos/SeasonInfo", false);
	LoadGlobalClassInfo(GC.m_paMonthInfo, "CIV4MonthInfos", "BasicInfos", "Civ4MonthInfos/MonthInfos/MonthInfo", false);
	LoadGlobalClassInfo(GC.m_paDenialInfo, "CIV4DenialInfos", "BasicInfos", "Civ4DenialInfos/DenialInfos/DenialInfo", false);
	LoadGlobalClassInfo(GC.m_paInvisibleInfo, "CIV4InvisibleInfos", "BasicInfos", "Civ4InvisibleInfos/InvisibleInfos/InvisibleInfo", false);
	LoadGlobalClassInfo(GC.m_paUnitCombatInfo, "CIV4UnitCombatInfos", "BasicInfos", "Civ4UnitCombatInfos/UnitCombatInfos/UnitCombatInfo", false);
	LoadGlobalClassInfo(GC.m_paDomainInfo, "CIV4DomainInfos", "BasicInfos", "Civ4DomainInfos/DomainInfos/DomainInfo", false);
	LoadGlobalClassInfo(GC.m_paUnitAIInfo, "CIV4UnitAIInfos", "BasicInfos", "Civ4UnitAIInfos/UnitAIInfos/UnitAIInfo", false);
	LoadGlobalClassInfo(GC.m_paAttitudeInfo, "CIV4AttitudeInfos", "BasicInfos", "Civ4AttitudeInfos/AttitudeInfos/AttitudeInfo", false);
	LoadGlobalClassInfo(GC.m_paMemoryInfo, "CIV4MemoryInfos", "BasicInfos", "Civ4MemoryInfos/MemoryInfos/MemoryInfo", false);

	DestroyFXml();
	return true;
}

/*  Globals which must be loaded before the main menus.
	Don't put anything in here unless it has to be loaded before the main menus;
	instead try to load things in LoadPostMenuGlobals(). */
bool CvXMLLoadUtility::LoadPreMenuGlobals()
{
	if (!CreateFXml())
		return false;

	/*	advc: Replaced all GC.get...Info() calls with direct accesses; see the
		comment under the friend declaration in CvGlobals.h. */

	LoadGlobalClassInfo(GC.m_paGameSpeedInfo, "CIV4GameSpeedInfo", "GameInfo", "Civ4GameSpeedInfo/GameSpeedInfos/GameSpeedInfo", false);
/************************************************************************************************/
/* REVDCM                                 02/16/10                                phungus420    */
/*                                                                                              */
/* CanTrain                                                                                     */
/************************************************************************************************/
	LoadGlobalClassInfo(GC.m_paGameOptionInfo, "CIV4GameOptionInfos", "GameInfo", "Civ4GameOptionInfos/GameOptionInfos/GameOptionInfo", false);
/************************************************************************************************/
/* REVDCM                                  END                                                  */
/************************************************************************************************/
	LoadGlobalClassInfo(GC.m_paTurnTimerInfo, "CIV4TurnTimerInfo", "GameInfo", "Civ4TurnTimerInfo/TurnTimerInfos/TurnTimerInfo", false);
	LoadGlobalClassInfo(GC.m_paWorldInfo, "CIV4WorldInfo", "GameInfo", "Civ4WorldInfo/WorldInfos/WorldInfo", false);
	LoadGlobalClassInfo(GC.m_paClimateInfo, "CIV4ClimateInfo", "GameInfo", "Civ4ClimateInfo/ClimateInfos/ClimateInfo", false);
	LoadGlobalClassInfo(GC.m_paSeaLevelInfo, "CIV4SeaLevelInfo", "GameInfo", "Civ4SeaLevelInfo/SeaLevelInfos/SeaLevelInfo", false);
	LoadGlobalClassInfo(GC.m_paAdvisorInfo, "CIV4AdvisorInfos", "Interface", "Civ4AdvisorInfos/AdvisorInfos/AdvisorInfo", false);
	LoadGlobalClassInfo(GC.m_paTerrainInfo, "CIV4TerrainInfos", "Terrain", "Civ4TerrainInfos/TerrainInfos/TerrainInfo", false);
	LoadGlobalClassInfo(GC.m_paEraInfo, "CIV4EraInfos", "GameInfo", "Civ4EraInfos/EraInfos/EraInfo", false);
	LoadGlobalClassInfo(GC.m_paUnitClassInfo, "CIV4UnitClassInfos", "Units", "Civ4UnitClassInfos/UnitClassInfos/UnitClassInfo", false);
	LoadGlobalClassInfo(GC.m_paSpecialistInfo, "CIV4SpecialistInfos", "GameInfo", "Civ4SpecialistInfos/SpecialistInfos/SpecialistInfo", false);
	LoadGlobalClassInfo(GC.m_paVoteSourceInfo, "CIV4VoteSourceInfos", "GameInfo", "Civ4VoteSourceInfos/VoteSourceInfos/VoteSourceInfo", false);
	LoadGlobalClassInfo(GC.m_paTechInfo, "CIV4TechInfos", "Technologies", "Civ4TechInfos/TechInfos/TechInfo", true, &CvDLLUtilityIFaceBase::createTechInfoCacheObject);
	LoadGlobalClassInfo(GC.m_paFeatureInfo, "Civ4FeatureInfos", "Terrain", "Civ4FeatureInfos/FeatureInfos/FeatureInfo", false);
	LoadGlobalClassInfo(GC.m_paReligionInfo, "CIV4ReligionInfo", "GameInfo", "Civ4ReligionInfo/ReligionInfos/ReligionInfo", false);
/*************************************************************************************************/
/** TGA_INDEXATION                          03/25/08                                MRGENIE      */
/**                                                                                              */
/** reorganizing the Religion vector by checking the TGAIndex of the xml and placing everything  */
/** properly                                                                                     */
/*************************************************************************************************/
	//ArrangeTGA(GC.getReligionInfo, "CIV4ReligionInfo");
	//advciv syntax
	ArrangeTGA(GC.m_paReligionInfo, "CIV4ReligionInfo");
/*************************************************************************************************/
/** TGA_INDEXATION                          END                                                  */
/*************************************************************************************************/
	LoadGlobalClassInfo(GC.m_paAnimationCategoryInfo, "CIV4AnimationInfos", "Units", "Civ4AnimationInfos/AnimationCategories/AnimationCategory", false);
	LoadGlobalClassInfo(GC.m_paAnimationPathInfo, "CIV4AnimationPathInfos", "Units", "Civ4AnimationPathInfos/AnimationPaths/AnimationPath", false);
	LoadGlobalClassInfo(GC.m_paPromotionInfo, "CIV4PromotionInfos", "Units", "Civ4PromotionInfos/PromotionInfos/PromotionInfo", true, &CvDLLUtilityIFaceBase::createPromotionInfoCacheObject);
	LoadGlobalClassInfo(GC.m_paTraitInfo, "CIV4TraitInfos", "Civilizations", "Civ4TraitInfos/TraitInfos/TraitInfo", false);
	LoadGlobalClassInfo(GC.m_paGoodyInfo, "CIV4GoodyInfo", "GameInfo", "Civ4GoodyInfo/GoodyInfos/GoodyInfo", false);
	LoadGlobalClassInfo(GC.m_paHandicapInfo, "CIV4HandicapInfo", "GameInfo", "Civ4HandicapInfo/HandicapInfos/HandicapInfo", false, &CvDLLUtilityIFaceBase::createHandicapInfoCacheObject);
	LoadGlobalClassInfo(GC.m_paCursorInfo, "CIV4CursorInfo", "GameInfo", "Civ4CursorInfo/CursorInfos/CursorInfo", false);
	LoadGlobalClassInfo(GC.m_paCivicOptionInfo, "CIV4CivicOptionInfos", "GameInfo", "Civ4CivicOptionInfos/CivicOptionInfos/CivicOptionInfo", false);
	LoadGlobalClassInfo(GC.m_paUpkeepInfo, "CIV4UpkeepInfo", "GameInfo", "Civ4UpkeepInfo/UpkeepInfos/UpkeepInfo", false);
	LoadGlobalClassInfo(GC.m_paHurryInfo, "CIV4HurryInfo", "GameInfo", "Civ4HurryInfo/HurryInfos/HurryInfo", false);
	LoadGlobalClassInfo(GC.m_paSpecialBuildingInfo, "CIV4SpecialBuildingInfos", "Buildings", "Civ4SpecialBuildingInfos/SpecialBuildingInfos/SpecialBuildingInfo", false);
	LoadGlobalClassInfo(GC.m_paCultureLevelInfo, "CIV4CultureLevelInfo", "GameInfo", "Civ4CultureLevelInfo/CultureLevelInfos/CultureLevelInfo", false);
	LoadGlobalClassInfo(GC.m_paBonusClassInfo, "CIV4BonusClassInfos", "Terrain", "Civ4BonusClassInfos/BonusClassInfos/BonusClassInfo", false);
	LoadGlobalClassInfo(GC.m_paVictoryInfo, "CIV4VictoryInfo", "GameInfo", "Civ4VictoryInfo/VictoryInfos/VictoryInfo", false);
	LoadGlobalClassInfo(GC.m_paBonusInfo, "CIV4BonusInfos", "Terrain", "Civ4BonusInfos/BonusInfos/BonusInfo", false, &CvDLLUtilityIFaceBase::createBonusInfoCacheObject);
	LoadGlobalClassInfo(GC.m_paCorporationInfo, "CIV4CorporationInfo", "GameInfo", "Civ4CorporationInfo/CorporationInfos/CorporationInfo", false);
/*************************************************************************************************/
/** TGA_INDEXATION                          03/25/08                                MRGENIE      */
/**                                                                                              */
/** reorganizing the Corporations vector by checking the TGAIndex of the xml and placing         */
/** everything properly                                                                          */
/*************************************************************************************************/
	//ArrangeTGA(GC.getCorporationInfo, "CIV4CorporationInfo");
	//syntax for advciv
	ArrangeTGA(GC.m_paCorporationInfo, "CIV4CorporationInfo");
/*************************************************************************************************/
/** TGA_INDEXATION                          END                                                  */
/*************************************************************************************************/
	LoadGlobalClassInfo(GC.m_paRouteInfo, "Civ4RouteInfos", "Misc", "Civ4RouteInfos/RouteInfos/RouteInfo", false);
	LoadGlobalClassInfo(GC.m_paImprovementInfo, "CIV4ImprovementInfos", "Terrain", "Civ4ImprovementInfos/ImprovementInfos/ImprovementInfo", true, &CvDLLUtilityIFaceBase::createImprovementInfoCacheObject);
	LoadGlobalClassInfo(GC.m_paBuildingClassInfo, "CIV4BuildingClassInfos", "Buildings", "Civ4BuildingClassInfos/BuildingClassInfos/BuildingClassInfo", false);
	LoadGlobalClassInfo(GC.m_paBuildingInfo, "CIV4BuildingInfos", "Buildings", "Civ4BuildingInfos/BuildingInfos/BuildingInfo", false, &CvDLLUtilityIFaceBase::createBuildingInfoCacheObject);
	for (int i=0; i < GC.getNumBuildingClassInfos(); ++i)
	{
		GC.getInfo((BuildingClassTypes)i).readPass3();
	}
	LoadGlobalClassInfo(GC.m_paSpecialUnitInfo, "CIV4SpecialUnitInfos", "Units", "Civ4SpecialUnitInfos/SpecialUnitInfos/SpecialUnitInfo", false);
	// davidlallen: project civilization and free unit next line
	// CIV4ProjectInfo was here
	//LoadGlobalClassInfo(GC.m_paProjectInfo, "CIV4ProjectInfo", "GameInfo", "Civ4ProjectInfo/ProjectInfos/ProjectInfo", true);
	LoadGlobalClassInfo(GC.m_paCivicInfo, "CIV4CivicInfos", "GameInfo", "Civ4CivicInfos/CivicInfos/CivicInfo", false, &CvDLLUtilityIFaceBase::createCivicInfoCacheObject);
	for (int i=0; i < GC.getNumVoteSourceInfos(); ++i)
	{
		GC.getInfo((VoteSourceTypes)i).readPass3();
	}
	LoadGlobalClassInfo(GC.m_paLeaderHeadInfo, "CIV4LeaderHeadInfos", "Civilizations", "Civ4LeaderHeadInfos/LeaderHeadInfos/LeaderHeadInfo", false, &CvDLLUtilityIFaceBase::createLeaderHeadInfoCacheObject);
	LoadGlobalClassInfo(GC.m_paColorInfo, "CIV4ColorVals", "Interface", "Civ4ColorVals/ColorVals/ColorVal", false);
	LoadGlobalClassInfo(GC.m_paPlayerColorInfo, "CIV4PlayerColorInfos", "Interface", "Civ4PlayerColorInfos/PlayerColorInfos/PlayerColorInfo", false);
	LoadGlobalClassInfo(GC.m_paEffectInfo, "CIV4EffectInfos", "Misc", "Civ4EffectInfos/EffectInfos/EffectInfo", false);
	LoadGlobalClassInfo(GC.m_paEntityEventInfo, "CIV4EntityEventInfos", "Units", "Civ4EntityEventInfos/EntityEventInfos/EntityEventInfo", false);
	LoadGlobalClassInfo(GC.m_paBuildInfo, "CIV4BuildInfos", "Units", "Civ4BuildInfos/BuildInfos/BuildInfo", false);
	LoadGlobalClassInfo(GC.m_paUnitInfo, "CIV4UnitInfos", "Units", "Civ4UnitInfos/UnitInfos/UnitInfo", false, &CvDLLUtilityIFaceBase::createUnitInfoCacheObject);
	for (int i=0; i < GC.getNumUnitClassInfos(); ++i)
	{
		GC.getInfo((UnitClassTypes)i).readPass3();
	}
	LoadGlobalClassInfo(GC.m_paUnitArtStyleInfo, "CIV4UnitArtStyleTypeInfos", "Civilizations", "Civ4UnitArtStyleTypeInfos/UnitArtStyleTypeInfos/UnitArtStyleTypeInfo", false);
	LoadGlobalClassInfo(GC.m_paCivilizationInfo, "CIV4CivilizationInfos", "Civilizations", "Civ4CivilizationInfos/CivilizationInfos/CivilizationInfo", true, &CvDLLUtilityIFaceBase::createCivilizationInfoCacheObject);
	//hated civs additions - f1rpo dune wars aln
	FOR_EACH_ENUM(LeaderHead)
	{
    	GC.getInfo(eLoopLeaderHead).readPass3();
	}
	LoadGlobalClassInfo(GC.m_paHintInfo, "CIV4Hints", "GameInfo", "Civ4Hints/HintInfos/HintInfo", false);
	// davidlallen: project civilization and free unit next line
	LoadGlobalClassInfo(GC.m_paProjectInfo, "CIV4ProjectInfo", "GameInfo", "Civ4ProjectInfo/ProjectInfos/ProjectInfo", true);
    LoadGlobalClassInfo(GC.m_paMainMenuInfo, "CIV4MainMenus", "Art", "Civ4MainMenus/MainMenus/MainMenu", false);
	LoadGlobalClassInfo(GC.m_paSlideShowInfo, "CIV4SlideShowInfos", "Interface", "Civ4SlideShowInfos/SlideShowInfos/SlideShowInfo", false);
	LoadGlobalClassInfo(GC.m_paSlideShowRandomInfo, "CIV4SlideShowRandomInfos", "Interface", "Civ4SlideShowRandomInfos/SlideShowRandomInfos/SlideShowRandomInfo", false);
	LoadGlobalClassInfo(GC.m_paWorldPickerInfo, "CIV4WorldPickerInfos", "Interface", "Civ4WorldPickerInfos/WorldPickerInfos/WorldPickerInfo", false);
	LoadGlobalClassInfo(GC.m_paSpaceShipInfo, "Civ4SpaceShipInfos", "Interface", "Civ4SpaceShipInfos/SpaceShipInfos/SpaceShipInfo", false);
	LoadGlobalClassInfo(GC.m_paYieldInfo, "CIV4YieldInfos", "Terrain", "Civ4YieldInfos/YieldInfos/YieldInfo", false);
	LoadGlobalClassInfo(GC.m_paCommerceInfo, "CIV4CommerceInfo", "GameInfo", "Civ4CommerceInfo/CommerceInfos/CommerceInfo", false);
	LoadGlobalClassInfo(GC.m_paGameOptionInfo, "CIV4GameOptionInfos", "GameInfo", "Civ4GameOptionInfos/GameOptionInfos/GameOptionInfo", false);
	LoadGlobalClassInfo(GC.m_paMPOptionInfo, "CIV4MPOptionInfos", "GameInfo", "Civ4MPOptionInfos/MPOptionInfos/MPOptionInfo", false);
	LoadGlobalClassInfo(GC.m_paForceControlInfo, "CIV4ForceControlInfos", "GameInfo", "Civ4ForceControlInfos/ForceControlInfos/ForceControlInfo", false);

/****************************************
 *  Archid Mod: 10 Jun 2012
 *  Functionality: Unit Civic Prereq - Archid
 *		Based on code by Afforess
 *	Source:
 *	  http://forums.civfanatics.com/downloads.php?do=file&id=15508
 *
 ****************************************/
	for (int i=0; i < GC.getNumUnitInfos(); ++i)
	{
		GC.getUnitInfo((UnitTypes)i).readPass3();
	}
/**
 ** End: Unit Civic Prereq
 **/
	// add types to global var system
	for (int i = 0; i < GC.getNumCursorInfos(); ++i)
	{
		int iVal;
		CvString szType = GC.getInfo((CursorTypes)i).getType();
		if (GC.getDefinesVarSystem()->GetValue(szType, iVal))
		{
			char szMessage[1024];
			sprintf(szMessage, "cursor type already set?\nCurrent XML file is: %s", GC.getCurrentXMLFile().GetCString());
			errorMessage(szMessage);
		}
		GC.getDefinesVarSystem()->SetValue(szType, i);
	}

	// Check Playables
	for (int i=0; i < GC.getNumCivilizationInfos(); ++i)
	{
		// if the civilization is playable we will increment the playable var
		if (GC.getInfo((CivilizationTypes) i).isPlayable())
			GC.getNumPlayableCivilizationInfos() += 1;

		// if the civilization is playable by AI increments num playable
		if (GC.getInfo((CivilizationTypes) i).isAIPlayable())
			GC.getNumAIPlayableCivilizationInfos() += 1;
	}
/*************************************************************************************************/
/** TGA_INDEXATION                          03/25/08                                MRGENIE      */
/**                                                                                              */
/** Filling up empty spots(i.e. because module not loaded)                                       */
/*************************************************************************************************/
	//AddTGABogus(GC.getCorporationInfo(), "CIV4CorporationInfo");
	//AddTGABogus(GC.getCorporationInfo(), "CIV4CorporationInfo");
	//advc syntax
	AddTGABogus(GC.m_paCorporationInfo, "CIV4CorporationInfo");
	AddTGABogus(GC.m_paReligionInfo, "CIV4ReligionInfo");
/*************************************************************************************************/
/** TGA_INDEXATION                          END                                                  */
/*************************************************************************************************/

	UpdateProgressCB("GlobalOther");
	DestroyFXml();
	// <advc.enum>
	void TestEnumMap();
	TestEnumMap(); // </advc.enum>
	// <advc.fract>
	void TestScaledNum();
	TestScaledNum(); // </advc.fract>
	getUWAI.doXML(); // advc.104x
	GC.setXMLLoadUtility(this); // advc.003v

	return true;
}

/*  Loads global XML data which isn't needed for the main menus.
	This data is loaded as a second stage when the game is launched. */
bool CvXMLLoadUtility::LoadPostMenuGlobals()
{
	PROFILE_FUNC();
	if (!CreateFXml())
		return false;

	/* advc: Replaced all GC.get...Info() calls with direct accesses; see the
		comment under the friend declaration in CvGlobals.h. */

	// advc.003v: Moved into LoadThroneRoomInfo
	//UpdateProgressCB("Global Throne Room");
	// ...

	// advc.003v: Moved into LoadOptionalGlobals
	//UpdateProgressCB("Global Events");
	// ...

	UpdateProgressCB("Global Routes");
	LoadGlobalClassInfo(GC.m_paRouteModelInfo, "Civ4RouteModelInfos", "Art", "Civ4RouteModelInfos/RouteModelInfos/RouteModelInfo", false);

	UpdateProgressCB("Global Rivers");
	//LoadGlobalClassInfo(GC.m_paRiverInfo, "CIV4RiverInfos", "Misc", "Civ4RiverInfos/RiverInfos/RiverInfo", false); // advc.003j: unused
	LoadGlobalClassInfo(GC.m_paRiverModelInfo, "CIV4RiverModelInfos", "Art", "Civ4RiverModelInfos/RiverModelInfos/RiverModelInfo", false);

	UpdateProgressCB("Global Other");
	LoadGlobalClassInfo(GC.m_paWaterPlaneInfo, "CIV4WaterPlaneInfos", "Misc", "Civ4WaterPlaneInfos/WaterPlaneInfos/WaterPlaneInfo", false);
	LoadGlobalClassInfo(GC.m_paTerrainPlaneInfo, "CIV4TerrainPlaneInfos", "Misc", "Civ4TerrainPlaneInfos/TerrainPlaneInfos/TerrainPlaneInfo", false);
	LoadGlobalClassInfo(GC.m_paCameraOverlayInfo, "CIV4CameraOverlayInfos", "Misc", "Civ4CameraOverlayInfos/CameraOverlayInfos/CameraOverlayInfo", false);

	UpdateProgressCB("Global Process");
	LoadGlobalClassInfo(GC.m_paProcessInfo, "CIV4ProcessInfo", "GameInfo", "Civ4ProcessInfo/ProcessInfos/ProcessInfo", false);

	UpdateProgressCB("Global Emphasize");
	LoadGlobalClassInfo(GC.m_paEmphasizeInfo, "CIV4EmphasizeInfo", "GameInfo", "Civ4EmphasizeInfo/EmphasizeInfos/EmphasizeInfo", false);

	UpdateProgressCB("Global Other");
	LoadGlobalClassInfo(GC.m_paMissionInfo, "CIV4MissionInfos", "Units", "Civ4MissionInfos/MissionInfos/MissionInfo", false);
	LoadGlobalClassInfo(GC.m_paControlInfo, "CIV4ControlInfos", "Units", "Civ4ControlInfos/ControlInfos/ControlInfo", false);
	LoadGlobalClassInfo(GC.m_paCommandInfo, "CIV4CommandInfos", "Units", "Civ4CommandInfos/CommandInfos/CommandInfo", false);
	LoadGlobalClassInfo(GC.m_paAutomateInfo, "CIV4AutomateInfos", "Units", "Civ4AutomateInfos/AutomateInfos/AutomateInfo", false);

	UpdateProgressCB("Global Vote");
	LoadGlobalClassInfo(GC.m_paVoteInfo, "CIV4VoteInfo", "GameInfo", "Civ4VoteInfo/VoteInfos/VoteInfo", false);

	UpdateProgressCB("Global Interface");
	// advc.003j:
	//LoadGlobalClassInfo(GC.m_paCameraInfo, "CIV4CameraInfos", "Interface", "Civ4CameraInfos/CameraInfos/CameraInfo", false);
	LoadGlobalClassInfo(GC.m_paInterfaceModeInfo, "CIV4InterfaceModeInfos", "Interface", "Civ4InterfaceModeInfos/InterfaceModeInfos/InterfaceModeInfo", false);

	SetGlobalActionInfo();

	LoadGlobalClassInfo(GC.m_paUnitFormationInfo, "CIV4FormationInfos", "Units", "UnitFormations/UnitFormation", false);
	LoadGlobalClassInfo(GC.m_paAttachableInfo, "CIV4AttachableInfos", "Misc", "Civ4AttachableInfos/AttachableInfos/AttachableInfo", false);

	// Special Case Diplomacy Info due to double vectored nature and appending of Responses
	LoadDiplomacyInfo(GC.m_paDiplomacyInfo, "CIV4DiplomacyInfos", "GameInfo", "Civ4DiplomacyInfos/DiplomacyInfos/DiplomacyInfo", &CvDLLUtilityIFaceBase::createDiplomacyInfoCacheObject);
	// advc.003j:
	//LoadGlobalClassInfo(GC.QuestInfo, "Civ4QuestInfos", "Misc", "Civ4QuestInfos/QuestInfo", false);

	LoadGlobalClassInfo(GC.m_paTutorialInfo, "Civ4TutorialInfos", "Misc", "Civ4TutorialInfos/TutorialInfo", false);

	/*  advc.003v (comment): Could probably move this to LoadOptionalGlobals, but,
		since the NO_ESPIONAGE option isn't even visible anymore in AdvCiv, I'm
		not going to bother to try it. */
	LoadGlobalClassInfo(GC.m_paEspionageMissionInfo, "CIV4EspionageMissionInfo", "GameInfo", "Civ4EspionageMissionInfo/EspionageMissionInfos/EspionageMissionInfo", false);

	DestroyFXml();
	GC.setCurrentXMLFile(NULL); // advc.006e

	// <advc.enum>
	#define ASSERT_SIZE_EQUALS_ENUM_LENGTH(Name, INFIX) \
		FAssertMsg(GC.m_pa##Name##Info.size() == NUM_ENUM_TYPES(INFIX), \
		"The number of "#Name " types loaded from XML does not match "#Name"Types");
	DO_FOR_EACH_STATIC_INFO_TYPE(ASSERT_SIZE_EQUALS_ENUM_LENGTH);
	#define ASSERT_FITS_IN_SIGNED_CHAR(Name, INFIX) \
		FAssertMsg(GC.m_pa##Name##Info.size() <= MAX_CHAR, \
		"The number of "#Name " types loaded from XML exceeds the limit assumed by EnumMap");
	DO_FOR_EACH_SMALL_DYN_INFO_TYPE(ASSERT_FITS_IN_SIGNED_CHAR);
	// </advc.enum>
	return true;
}

// <advc.003v>
bool CvXMLLoadUtility::LoadOptionalGlobals()
{
	if (m->bEventsLoaded || GC.getGame().isOption(GAMEOPTION_NO_EVENTS))
		return true;

	if (!CreateFXml())
		return false;

	LoadGlobalClassInfo(GC.m_paEventInfo, "CIV4EventInfos", "Events", "Civ4EventInfos/EventInfos/EventInfo", true, &CvDLLUtilityIFaceBase::createEventInfoCacheObject);
	LoadGlobalClassInfo(GC.m_paEventTriggerInfo, "CIV4EventTriggerInfos", "Events", "Civ4EventTriggerInfos/EventTriggerInfos/EventTriggerInfo", false, &CvDLLUtilityIFaceBase::createEventTriggerInfoCacheObject);
	DestroyFXml();
	m->bEventsLoaded = true;

	return true;
}


bool CvXMLLoadUtility::LoadThroneRoomInfo()
{
	if (m->bThroneRoomLoaded)
		return true;

	if (!CreateFXml())
		return false;
	FAssert(gDLL->getChtLvl() > 0); // (Ctrl+D unfortunately doesn't require DebugMode)
	LoadGlobalClassInfo(GC.m_paThroneRoomCameraInfo, "CIV4ThroneRoomCameraInfos", "Interface", "Civ4ThroneRoomCameraInfos/ThroneRoomCameraInfos/ThroneRoomCamera", false);
	LoadGlobalClassInfo(GC.m_paThroneRoomInfo, "CIV4ThroneRoomInfos", "Interface", "Civ4ThroneRoomInfos/ThroneRoomInfos/ThroneRoomInfo", false);
	LoadGlobalClassInfo(GC.m_paThroneRoomStyleInfo, "CIV4ThroneRoomStyleInfos", "Interface", "Civ4ThroneRoomStyleInfos/ThroneRoomStyleInfos/ThroneRoomStyleInfo", false);
	DestroyFXml();
	m->bThroneRoomLoaded = true;

	return true;
} // </advc.003v>

/*	Looks for szTagName in XML and, if it exists, loads its value into ppszString
	and sets iNumVals to the total number of occurrences of szTagName in XML. */
void CvXMLLoadUtility::SetGlobalStringArray(CvString **ppszString, char* szTagName, int* iNumVals, bool bUseEnum)
{
	PROFILE_FUNC();
	logMsg("SetGlobalStringArray %s\n", szTagName);

	CvString* pszString = NULL; // checked at the end of the function

	if (gDLL->getXMLIFace()->LocateNode(m_pFXml,szTagName))
	{
		if (!bUseEnum)
		{
			// get the total number of times this tag appears in XML
			*iNumVals = gDLL->getXMLIFace()->NumOfElementsByTagName(m_pFXml,szTagName);
		}
		// initialize the memory based on the total number of tags in XML and the 256 character length we selected
		*ppszString = new CvString[*iNumVals];
		pszString = *ppszString; // set the local pointer to the memory just allocated

		for (int i = 0; i < *iNumVals; i++)
		{
			// get the string value at the current node
			GetXmlVal(pszString[i]);
			GC.setTypesEnum(pszString[i], i);
			// set the current node to a sibling node
			if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
				break;
		}
	}
	// if we didn't find the tag name in XML, then we never set the local pointer to the
	// newly allocated memory. let people know this most interesting fact.
	if (!pszString)
	{
		char szMessage[1024];
		sprintf(szMessage, "Error locating tag node in SetGlobalStringArray function.\nCurrent XML file is: %s", GC.getCurrentXMLFile().GetCString());
		errorMessage(szMessage);
	}
}

// <advc> Get rid of some duplicate code
#define NOT_GREATER_ZERO_MESSAGE_BOX(VAR) \
	if ((VAR) <= 0) \
	{ \
		char szMessage[1024]; \
		sprintf(szMessage, #VAR" is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo.\nCurrent XML file is: %s", \
				GC.getCurrentXMLFile().GetCString()); \
		errorMessage(szMessage); \
	} // </advc>


/*	Looks for szTagName in XML and, if it exists, loads its value into ppActionInfo
	and sets iNumVals to the total number of occurrences of szTagName in XML. */
void CvXMLLoadUtility::SetGlobalActionInfo()
{
	PROFILE_FUNC();
	logMsg("SetGlobalActionInfo\n");

	NOT_GREATER_ZERO_MESSAGE_BOX(NUM_INTERFACEMODE_TYPES);
	NOT_GREATER_ZERO_MESSAGE_BOX(GC.getNumBuildInfos());
	NOT_GREATER_ZERO_MESSAGE_BOX(GC.getNumPromotionInfos());
	NOT_GREATER_ZERO_MESSAGE_BOX(GC.getNumUnitClassInfos());
	NOT_GREATER_ZERO_MESSAGE_BOX(GC.getNumSpecialistInfos());
	NOT_GREATER_ZERO_MESSAGE_BOX(GC.getNumBuildingInfos());
	NOT_GREATER_ZERO_MESSAGE_BOX(NUM_CONTROL_TYPES);
	NOT_GREATER_ZERO_MESSAGE_BOX(NUM_AUTOMATE_TYPES);
	NOT_GREATER_ZERO_MESSAGE_BOX(NUM_COMMAND_TYPES);
	NOT_GREATER_ZERO_MESSAGE_BOX(NUM_MISSION_TYPES);

	int* piOrderedIndex=NULL;

	int iNumOrigVals = GC.getNumActionInfos();

	int iNumActionInfos = iNumOrigVals +
		NUM_INTERFACEMODE_TYPES +
		GC.getNumBuildInfos() +
		GC.getNumPromotionInfos() +
		GC.getNumReligionInfos() +
		GC.getNumCorporationInfos() +
		GC.getNumUnitInfos() +
		GC.getNumSpecialistInfos() +
		GC.getNumBuildingInfos() +
		NUM_CONTROL_TYPES +
		NUM_COMMAND_TYPES +
		NUM_AUTOMATE_TYPES +
		NUM_MISSION_TYPES;

	int* piIndexList = new int[iNumActionInfos];
	int* piPriorityList = new int[iNumActionInfos];
	int* piActionInfoTypeList = new int[iNumActionInfos];

	int iTotalActionInfoCount = 0;

	for (int i = 0; i < NUM_COMMAND_TYPES; i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInfo((CommandTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_COMMAND;
		iTotalActionInfoCount++;
	}
	for (int i = 0; i < NUM_INTERFACEMODE_TYPES; i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInfo((InterfaceModeTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_INTERFACEMODE;
		iTotalActionInfoCount++;
	}
	for (int i = 0; i < GC.getNumBuildInfos(); i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInfo((BuildTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_BUILD;
		iTotalActionInfoCount++;
	}
	for (int i = 0; i < GC.getNumPromotionInfos(); i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInfo((PromotionTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_PROMOTION;
		iTotalActionInfoCount++;
	}
	for (int i = 0; i < GC.getNumUnitInfos(); i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInfo((UnitTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_UNIT;
		iTotalActionInfoCount++;
	}
	for (int i = 0; i < GC.getNumReligionInfos(); i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInfo((ReligionTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_RELIGION;
		iTotalActionInfoCount++;
	}
	for (int i = 0; i < GC.getNumCorporationInfos(); i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInfo((CorporationTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_CORPORATION;
		iTotalActionInfoCount++;
	}
	for (int i = 0; i < GC.getNumSpecialistInfos(); i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInfo((SpecialistTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_SPECIALIST;
		iTotalActionInfoCount++;
	}
	for (int i = 0; i < GC.getNumBuildingInfos(); i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInfo((BuildingTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_BUILDING;
		iTotalActionInfoCount++;
	}
	for (int i = 0; i < NUM_CONTROL_TYPES; i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInfo((ControlTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_CONTROL;
		iTotalActionInfoCount++;
	}
	for (int i = 0; i < NUM_AUTOMATE_TYPES; i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInfo((AutomateTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_AUTOMATE;
		iTotalActionInfoCount++;
	}
	for (int i = 0; i < NUM_MISSION_TYPES; i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInfo((MissionTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_MISSION;
		iTotalActionInfoCount++;
	}

	SAFE_DELETE_ARRAY(piOrderedIndex);
	piOrderedIndex = new int[iNumActionInfos];

	orderHotkeyInfo(&piOrderedIndex, piPriorityList, iNumActionInfos);
	for (int i = 0; i < iNumActionInfos; i++)
	{
		CvActionInfo* pActionInfo = new CvActionInfo;
		// <advc>
		int const iOrdered = piOrderedIndex[i];
		int const iInfo = piIndexList[iOrdered]; // <advc>
		pActionInfo->setOriginalIndex(iInfo);
		pActionInfo->setSubType((ActionSubTypes)piActionInfoTypeList[iOrdered]);
		if ((ActionSubTypes)piActionInfoTypeList[iOrdered] == ACTIONSUBTYPE_COMMAND)
		{
			GC.getInfo((CommandTypes)iInfo).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[iOrdered] == ACTIONSUBTYPE_INTERFACEMODE)
		{
			GC.getInfo((InterfaceModeTypes)iInfo).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[iOrdered] == ACTIONSUBTYPE_BUILD)
		{
			GC.getInfo((BuildTypes)iInfo).setMissionType(FindInInfoClass("MISSION_BUILD"));
			GC.getInfo((BuildTypes)iInfo).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[iOrdered] == ACTIONSUBTYPE_CONTROL)
		{
			GC.getControlInfo((ControlTypes)iInfo).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[iOrdered] == ACTIONSUBTYPE_AUTOMATE)
		{
			GC.getAutomateInfo((AutomateTypes)iInfo).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[iOrdered] == ACTIONSUBTYPE_MISSION)
		{
			GC.getMissionInfo((MissionTypes)iInfo).setActionInfoIndex(i + iNumOrigVals);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[iOrdered] == ACTIONSUBTYPE_PROMOTION)
		{
			CvPromotionInfo& kPromo = GC.getInfo((PromotionTypes)iInfo);
			kPromo.setCommandType(FindInInfoClass("COMMAND_PROMOTION"));
			setActionData(kPromo, i, (CommandTypes)kPromo.getCommandType());
		}
		else if ((ActionSubTypes)piActionInfoTypeList[iOrdered] == ACTIONSUBTYPE_UNIT)
		{
			CvUnitInfo& kUnit = GC.getInfo((UnitTypes)iInfo);
			kUnit.setCommandType((CommandTypes)FindInInfoClass("COMMAND_UPGRADE"));
			kUnit.setActionInfoIndex(i);
			setActionData(kUnit, i, kUnit.getCommandType());
		}
		else if ((ActionSubTypes)piActionInfoTypeList[iOrdered] == ACTIONSUBTYPE_RELIGION)
		{
			CvReligionInfo& kReligion = GC.getInfo((ReligionTypes)iInfo);
			kReligion.setMissionType(FindInInfoClass("MISSION_SPREAD"));
			kReligion.setActionInfoIndex(i);
			setActionData(kReligion, i, (MissionTypes)kReligion.getMissionType());
		}
		else if ((ActionSubTypes)piActionInfoTypeList[iOrdered] == ACTIONSUBTYPE_CORPORATION)
		{
			CvCorporationInfo& kCorp = GC.getInfo((CorporationTypes)iInfo);
			kCorp.setMissionType(FindInInfoClass("MISSION_SPREAD_CORPORATION"));
			kCorp.setActionInfoIndex(i);
			setActionData(kCorp, i, (MissionTypes)kCorp.getMissionType());
		}
		else if ((ActionSubTypes)piActionInfoTypeList[iOrdered] == ACTIONSUBTYPE_SPECIALIST)
		{
			CvSpecialistInfo& kSpecialist = GC.getInfo((SpecialistTypes)iInfo);
			kSpecialist.setMissionType(FindInInfoClass("MISSION_JOIN"));
			kSpecialist.setActionInfoIndex(i);
			setActionData(kSpecialist, i, (MissionTypes)kSpecialist.getMissionType());
		}
		else if ((ActionSubTypes)piActionInfoTypeList[iOrdered] == ACTIONSUBTYPE_BUILDING)
		{
			CvBuildingInfo& kBuilding = GC.getInfo((BuildingTypes)iInfo);
			kBuilding.setMissionType((MissionTypes)FindInInfoClass("MISSION_CONSTRUCT"));
			kBuilding.setActionInfoIndex(i);
			//setActionData(kBuilding, i, (MissionTypes)kBuilding.getMissionType());
		}

		GC.m_paActionInfo.push_back(pActionInfo);
	}

	SAFE_DELETE_ARRAY(piOrderedIndex);
	SAFE_DELETE_ARRAY(piIndexList);
	SAFE_DELETE_ARRAY(piPriorityList);
	SAFE_DELETE_ARRAY(piActionInfoTypeList);
}

// advc: To get rid of some duplicate code in SetGlobalActionInfo
template <class T, typename /* enum */ E>
void CvXMLLoadUtility::setActionData(T& kInfo, int iAction, E eMissionCommand)
{
	kInfo.setHotKeyDescription(kInfo.getTextKeyWide(),
			GC.getInfo(eMissionCommand).getTextKeyWide(),
			HotKeyFromDescription(kInfo.getHotKey(),
			kInfo.isShiftDown(), kInfo.isAltDown(), kInfo.isCtrlDown()));
}

/*	Looks for szTagName in XML and, if it exists, loads its value into ppAnimationPathInfo
	and sets iNumVals to the total number of occurrences of szTagName in XML. */
void CvXMLLoadUtility::SetGlobalAnimationPathInfo(CvAnimationPathInfo** ppAnimationPathInfo, char* szTagName, int* iNumVals)
{
	PROFILE_FUNC();
	logMsg("SetGlobalAnimationPathInfo %s\n", szTagName);

	CvAnimationPathInfo * pAnimPathInfo = NULL;	// local pointer to the domain info memory

	if (gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName))
	{
		// get the number of times the szTagName tag appears in the xml file
		*iNumVals = gDLL->getXMLIFace()->NumOfElementsByTagName(m_pFXml,szTagName);

		// allocate memory for the domain info based on the number above
		*ppAnimationPathInfo = new CvAnimationPathInfo[*iNumVals];
		pAnimPathInfo = *ppAnimationPathInfo;

		gDLL->getXMLIFace()->SetToParent(m_pFXml);
		gDLL->getXMLIFace()->SetToChild(m_pFXml);
		gDLL->getXMLIFace()->SetToChild(m_pFXml);

		for (int i = 0; i < *iNumVals; i++)
		{
			SkipToNextVal(); // skip to the next non-comment node
			if (!pAnimPathInfo[i].read(this))
				break;
			GC.setInfoTypeFromString(pAnimPathInfo[i].getType(), i); // add type to global info type hash map
			if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
				break;
		}
	}
	// if we didn't find the tag name in XML, then we never set the local pointer to the
	// newly allocated memory. let people know this most interesting
	if(!pAnimPathInfo)
	{
		char szMessage[1024];
		sprintf(szMessage, "Error finding tag node in SetGlobalAnimationPathInfo function\nCurrent XML file is: %s",
				GC.getCurrentXMLFile().GetCString());
		errorMessage(szMessage);
	}
}

// advc.003j: unused
/*	Looks for szTagName in XML and, if it exists, loads its value into ppPromotionInfo
	and sets iNumVals to the total number of occurrences of szTagName in XML. */
/*void CvXMLLoadUtility::SetGlobalUnitScales(float& fLargeScale, float& fSmallScale, char const* szTagName)
{
	PROFILE_FUNC();
	logMsg("SetGlobalUnitScales %s\n", szTagName);
	// if we successfully locate the szTagName node
	if (gDLL->getXMLIFace()->LocateNode(m_pFXml,szTagName))
	{
		// set the FXml pointer to the first non-comment child of
		// the current tag and get the value of that new node
		if (GetChildXmlVal(fLargeScale))
		{
			// set the current xml node to it's next sibling and then
			// get the sibling's TCHAR value
			GetNextXmlVal(fSmallScale);
			gDLL->getXMLIFace()->SetToParent(m_pFXml); // set the current xml node to its parent
		}
	}
	else
	{
		// if we didn't find the tag name in XML, then we never set the local pointer to the
		// newly allocated memory. let people know this most interesting fact.
		char szMessage[1024];
		sprintf(szMessage, "Error finding tag node in SetGlobalUnitScales function\nCurrent XML file is: %s",
				GC.getCurrentXMLFile().GetCString());
		errorMessage(szMessage);
	}
}*/


// Reads game text info from XML and adds it to the translation manager
void CvXMLLoadUtility::SetGameText(const char* szTextGroup, const char* szTagName, const std::string& language_name)
{
	PROFILE_FUNC();
	logMsg("SetGameText %s\n", szTagName);

	if (gDLL->getXMLIFace()->LocateNode(m_pFXml, szTextGroup)) // Get the Text Group 1st
	{
		int iNumVals = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);	// Get the number of Children that the Text Group has
		gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName); // Now switch to the TEXT Tag
		gDLL->getXMLIFace()->SetToParent(m_pFXml);
		gDLL->getXMLIFace()->SetToChild(m_pFXml);
		for (int i = 0; i < iNumVals; i++)
		{
			CvGameText textInfo;
			//textInfo.read(this);
			textInfo.read(this, language_name); // K-Mod

			gDLL->addText(textInfo.getType() /*id*/, textInfo.getText(), textInfo.getGender(), textInfo.getPlural());
			if (!gDLL->getXMLIFace()->NextSibling(m_pFXml) && i!=iNumVals-1)
			{
				char	szMessage[1024];
				sprintf(szMessage, "Failed to find sibling.\nCurrent XML file is: %s", GC.getCurrentXMLFile().GetCString());
				errorMessage(szMessage);
				break;
			}
		}
	}
}

/*  This is a template function that is USED FOR ALMOST ALL INFO CLASSES.
	Each info class should have a read(CvXMLLoadUtility*) function that is responsible
	for initializing the class from XML data.
	Takes the szTagName parameter and loads the ppszString with the text values
	under the tags. This will be the hints displayed during game initialization and load. */
template <class T>
void CvXMLLoadUtility::SetGlobalClassInfo(std::vector<T*>& aInfos, const char* szTagName,
	bool bPassTwo, // advc.rh: Renamed from bTwoPass
	bool bFinalCall) // advc.xmldefault
{
	char szLog[256];
	sprintf(szLog, "SetGlobalClassInfo (%s)", szTagName);
	PROFILE(szLog);
	logMsg(szLog);

	// locate the tag name in the xml file
	if (!gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName))
		return; // advc

	if (bPassTwo)
	{
		// if we successfully locate the szTagName node
		if (gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName))
		{	/*	<advc.rh> Comment by rheinig: "Major bug:
				The following maps records as currently present in the info array,
				beginning with the first, to elements in the XML, beginning with the
				first, one by one, and does a (<T>).readPass2() on these pairs.
				Well, without Modular XML that may have worked most of the time...
				With modular XML, it will write partial properties of a new record
				over the first existing records... As this only hits Stuff loaded with
				the 'Two-Pass' flag on, and then only properties that are delayed to
				the second pass, this is seldomly visible - best examples:
				Techs or Promotions and their Prerequisite trees." */
			/*gDLL->getXMLIFace()->SetToParent(m_pFXml);
			gDLL->getXMLIFace()->SetToChild(m_pFXml);
			for (std::vector<T*>::iterator it = aInfos.begin(); it != aInfos.end(); ++it) {
				SkipToNextVal(); // skip to the next non-comment node
				(*it)->readPass2(this);
				if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
					break;
			}*/
			do
			{
				//MapChildren(); // advc.opt: I doubt that this amortizes here
				// advc.xmldefault: Moved into new function
				int iIndex = GetChildTypeIndex();
				if (iIndex >= 0)
					aInfos[iIndex]->readPass2(this);
			} while (gDLL->getXMLIFace()->NextSibling(m_pFXml) && SkipToNextVal());
		}
		/*  Comment by rheinig: "Because the mixing of pass 1 and pass 2 with
			modular XML was a major source of insidious bugs, the passes are now
			decoupled by the LoadGlobalClassInfo routine. I have replaced the old
			bTwoPass parameter with bPassTwo, which chooses the pass to perform,
			instead of enabling both passes here." */
		return; // </advc.rh>
	}

	static T* pDefaultClassInfo = NULL; // advc.xmldefault
	do // loop through each tag
	{
		//SkipToNextVal(); // K-Mod: Moved into termination check
		//T* pClassInfo = new T;
		// <advc.xmldefault>
		T* pClassInfo = NULL;
		{
			// Functionality akin to MRGENIE's XMLCopy. Requires copy-ctors though.
			/*int iIndex = GetChildTypeIndex();
			if (iIndex >= 0)
				pClassInfo = new T(*aInfos[iIndex]);*/
		}
		if (pDefaultClassInfo == NULL)
			pClassInfo = new T;
		else pClassInfo = new T(*pDefaultClassInfo);
		// </advc.xmldefault>
		bool bSuccess = pClassInfo->read(this);
		if (!bSuccess)
		{
			FAssert(bSuccess);
			delete pClassInfo;
			break;
		}
		// <advc.xmldefault>
		if (pClassInfo->isDefaultsType())
		{
			SAFE_DELETE(pDefaultClassInfo);
			pDefaultClassInfo = pClassInfo;
			continue;
		} // </advc.xmldefault>
		int iIndex = -1;
		if (pClassInfo->getType() != NULL)
			iIndex = GC.getInfoTypeForString(pClassInfo->getType(), true);
		if (iIndex < 0)
		{
			aInfos.push_back(pClassInfo);
			if (pClassInfo->getType() != NULL)
			{
				// add type to global info type hash map
				GC.setInfoTypeFromString(pClassInfo->getType(), (int)aInfos.size() - 1);
			}
		}
		else
		{
			SAFE_DELETE(aInfos[iIndex]);
			aInfos[iIndex] = pClassInfo;
		}
	} while (gDLL->getXMLIFace()->NextSibling(m_pFXml) /* K-Mod: */ && SkipToNextVal());
	// <advc.xmldefault>
	if (bFinalCall) // Otherwise keep it for info elements loaded from modules
		SAFE_DELETE(pDefaultClassInfo); // </advc.xmldefault>
}

void CvXMLLoadUtility::SetDiplomacyInfo(std::vector<CvDiplomacyInfo*>& DiploInfos, const char* szTagName)
{
	char szLog[256];
	sprintf(szLog, "SetDiplomacyInfo (%s)", szTagName);
	PROFILE(szLog);
	logMsg(szLog);

	// ocate the tag name in the xml file
	if (!gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName))
		return;

	do // loop through each tag
	{
		//SkipToNextVal(); // skip to the next non-comment node
		CvString szType;
		GetChildXmlValByName(szType, "Type");
		int iIndex = GC.getInfoTypeForString(szType, true);

		if (iIndex == -1)
		{
			CvDiplomacyInfo* pClassInfo = new CvDiplomacyInfo;
			if (pClassInfo == NULL)
			{
				FAssert(false);
				break;
			}
			pClassInfo->read(this);
			if (NULL != pClassInfo->getType())
			{
				// add type to global info type hash map
				GC.setInfoTypeFromString(pClassInfo->getType(), (int)DiploInfos.size());
			}
			DiploInfos.push_back(pClassInfo);
		}
		else DiploInfos[iIndex]->read(this);

		//} while (gDLL->getXMLIFace()->NextSibling(m_pFXml));
	} while (gDLL->getXMLIFace()->NextSibling(m_pFXml) && SkipToNextVal()); // K-Mod
}

template <class T>
void CvXMLLoadUtility::LoadGlobalClassInfo(std::vector<T*>& aInfos,
	const char* szFileRoot, const char* szFileDirectory,
	const char* szXmlPath, bool bTwoPass,
	CvCacheObject* (CvDLLUtilityIFaceBase::*pArgFunction) (const TCHAR*))
{
	pArgFunction = NULL; // advc.003i: Disable XML cache
	bool bLoaded = false;
	bool bWriteCache = true;
	CvCacheObject* pCache = NULL;
	//GC.addToInfosVectors(aInfos); // advc.enum (no longer needed)

	if (pArgFunction != NULL)
	{
		pCache = (gDLL->*pArgFunction)(CvString::format("%s.dat", szFileRoot));	// cache file name

		if (gDLL->cacheRead(pCache, CvString::format("xml\\\\%s\\\\%s.xml", szFileDirectory, szFileRoot)))
		{
			logMsg("Read %s from cache", szFileDirectory);
			bLoaded = true;
			bWriteCache = false;
		}
	}
	if (!bLoaded)
	{
		bLoaded = LoadCivXml(m_pFXml, CvString::format("xml\\%s/%s.xml", szFileDirectory, szFileRoot));
		if (!bLoaded)
		{
			char szMessage[1024];
			sprintf(szMessage, "LoadXML call failed for %s.", CvString::format("%s/%s.xml", szFileDirectory, szFileRoot).GetCString());
			errorMessage(szMessage, XML_LOAD_ERROR);
		}
		else
		{
			/*	advc.rh (comment by rheinig): "Because the Event tables contain
				member arrays dimensioned to their own count, we need to delay pass 2
				for the monolithic XML until after pass 1 has run for the modules.
				Thus, the 'pass' parameter of SetGlobalClassInfo now indicates the
				actual pass requested, not the need to execute both passes. */
			// advc.xmldefault: Enumeration of modules moved up
			std::vector<CvString> aszFiles;
			if (gDLL->isModularXMLLoading())
			{
				// search for the modular files
				gDLL->enumerateFiles(aszFiles, CvString::format("modules\\*_%s.xml", szFileRoot));
				/*	advc.rh: Comment by rheinig: "Ensure predictable load order
					(anyone else working on a modular mod suddenly have unit or
					building types swapped around after a non-xml change?)
					DO NOT use stable_sort (mem allocator issue).
					Repeat the loop to de-couple pass 2 (allows crossreferences
					between different modules, which are problematic anyway,
					but I like the XML code as clever as possible) Thus, this
					first loop will *not* call pass 2. " */
				std::sort(aszFiles.begin(), aszFiles.end());
			}
			SetGlobalClassInfo(aInfos, szXmlPath, /*bTwoPass*/ false,
					aszFiles.empty()); // advc.xmldefault
			for (std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
			{
				bLoaded = LoadCivXml(m_pFXml, *it);
				if (!bLoaded)
				{
					char szMessage[1024];
					sprintf(szMessage, "LoadXML call failed for %s.", it->GetCString());
					errorMessage(szMessage, XML_LOAD_ERROR);
				}
				else SetGlobalClassInfo(aInfos, szXmlPath, /*bTwoPass*/ false, // advc.rh
						(it + 1 == aszFiles.end())); // advc.xmldefault
			}
			// <advc.rh>
			if (bTwoPass)
			{
				if (!gDLL->isModularXMLLoading())
				{
					/*	"Do Pass 2 for the monolithic XML *now*
						(no need to reload the XML, no intervening modules)" */
					SetGlobalClassInfo(aInfos, szXmlPath, true);
				}
				else
				{
					/*	"Do Pass 2 for the monolithic XML *now*
						(reload the XML, intervening modules)
						For this second pass, we forgo any loading error messages -
						they hav already been output on pass 1" */
					bLoaded = LoadCivXml(m_pFXml, CvString::format("xml\\%s/%s.xml", szFileDirectory, szFileRoot));
					if (bLoaded)
						SetGlobalClassInfo(aInfos, szXmlPath, true);
					// "... followed by pass 2 for the modules:"
					for (std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
					{
						bLoaded = LoadCivXml(m_pFXml, *it);
						if (bLoaded)
							SetGlobalClassInfo(aInfos, szXmlPath, true);
					}
				}
			} // </advc.rh>
			// advc.003i: Disabled
			/*if (NULL != pArgFunction && bWriteCache) {
				// write info to cache
				bool bOk = gDLL->cacheWrite(pCache);
				if (!bOk) {
					char szMessage[1024];
					sprintf(szMessage, "Failed writing to %s cache.\nCurrent XML file is: %s", szFileDirectory, GC.getCurrentXMLFile().GetCString());
					gDLL->MessageBox(szMessage, "XML Caching Error");
				}
				if (bOk)
					logMsg("Wrote %s to cache", szFileDirectory);
			}*/
		}
	}
	if (pArgFunction != NULL)
		gDLL->destroyCache(pCache);
}


void CvXMLLoadUtility::LoadDiplomacyInfo(std::vector<CvDiplomacyInfo*>& DiploInfos, const char* szFileRoot, const char* szFileDirectory, const char* szXmlPath, CvCacheObject* (CvDLLUtilityIFaceBase::*pArgFunction) (const TCHAR*))
{
	bool bLoaded = false;
	bool bWriteCache = true;
	CvCacheObject* pCache = NULL;

	if (pArgFunction != NULL)
	{
		pCache = (gDLL->*pArgFunction)(CvString::format("%s.dat", szFileRoot));	// cache file name
		if (gDLL->cacheRead(pCache, CvString::format("xml\\\\%s\\\\%s.xml", szFileDirectory, szFileRoot)))
		{
			logMsg("Read %s from cache", szFileDirectory);
			bLoaded = true;
			bWriteCache = false;
		}
	}

	if (!bLoaded)
	{
		bLoaded = LoadCivXml(m_pFXml, CvString::format("xml\\%s/%s.xml", szFileDirectory, szFileRoot));
		if (!bLoaded)
		{
			char szMessage[1024];
			sprintf(szMessage, "LoadXML call failed for %s.", CvString::format("%s/%s.xml", szFileDirectory, szFileRoot).GetCString());
			errorMessage(szMessage, XML_LOAD_ERROR);
		}
		else
		{
			SetDiplomacyInfo(DiploInfos, szXmlPath);
			if (gDLL->isModularXMLLoading())
			{
				std::vector<CvString> aszFiles;
				// search for the modular files
				gDLL->enumerateFiles(aszFiles, CvString::format("modules\\*_%s.xml", szFileRoot));
				for (std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
				{
					bLoaded = LoadCivXml(m_pFXml, *it);
					if (!bLoaded)
					{
						char szMessage[1024];
						sprintf(szMessage, "LoadXML call failed for %s.", (*it).GetCString());
						errorMessage(szMessage, XML_LOAD_ERROR);
					}
					else SetDiplomacyInfo(DiploInfos, szXmlPath);
				}
			}
			// advc.003i: Disabled
			/*if (NULL != pArgFunction && bWriteCache) {
				// write info to cache
				bool bOk = gDLL->cacheWrite(pCache);
				if (!bOk) {
					char szMessage[1024];
					sprintf(szMessage, "Failed writing to %s cache.\nCurrent XML file is: %s", szFileDirectory, GC.getCurrentXMLFile().GetCString());
					gDLL->MessageBox(szMessage, "XML Caching Error");
				}
				if (bOk)
					logMsg("Wrote %s to cache", szFileDirectory);
			}*/
		}
	}

	if (pArgFunction != NULL)
		gDLL->destroyCache(pCache);
}

// helper sort predicate
struct OrderIndex {int m_iPriority; int m_iIndex;};
bool sortHotkeyPriority(const OrderIndex orderIndex1, const OrderIndex orderIndex2)
{
	return (orderIndex1.m_iPriority > orderIndex2.m_iPriority);
}

void CvXMLLoadUtility::orderHotkeyInfo(int** ppiSortedIndex, int* pHotkeyIndex, int iLength)
{
	std::vector<OrderIndex> viOrderPriority;
	viOrderPriority.resize(iLength);
	int* piSortedIndex = *ppiSortedIndex;

	for(int iI = 0; iI < iLength; iI++) // set up vector
	{
		viOrderPriority[iI].m_iPriority = pHotkeyIndex[iI];
		viOrderPriority[iI].m_iIndex = iI;
	}
	// sort the array
	std::sort(viOrderPriority.begin(), viOrderPriority.end(), sortHotkeyPriority);

	// insert new order into the array to return
	for (int iI = 0; iI < iLength; iI++)
		piSortedIndex[iI] = viOrderPriority[iI].m_iIndex;
}

// helper sort predicate
/*bool sortHotkeyPriorityOld(const CvHotkeyInfo* hotkey1, const CvHotkeyInfo* hotkey2)
{
	return (hotkey1->getOrderPriority() < hotkey2->getOrderPriority());
}*/
// order a hotkey info derived class
// ppHotkeyInfos is a hotkey info derived class
// iLength is the length of the hotkey info derived class array
/*template <class T>
void CvXMLLoadUtility::orderHotkeyInfoOld(T **ppHotkeyInfos, int iLength)
{
	std::vector<T*> vHotkeyInfo;
	T* pHotkeyInfo;	// local pointer to the hotkey info memory
	for (int iI=0;iI<iLength;iI++)
	{
		pHotkeyInfo = new T;
		*pHotkeyInfo = (*ppHotkeyInfos)[iI];
		vHotkeyInfo.push_back(pHotkeyInfo);
	}
	std::sort(vHotkeyInfo.begin(), vHotkeyInfo.end(), sortHotkeyPriority);
	for (int iI=0;iI<iLength;iI++)
		(*ppHotkeyInfos)[iI] = *vHotkeyInfo[iI];
	for (int iI=0;iI<(int)vHotkeyInfo.size();iI++)
	{
		vHotkeyInfo[iI]->reset();
		SAFE_DELETE(vHotkeyInfo[iI]);
	}
	vHotkeyInfo.clear();
}*/

/*  Allocate memory for the yield parameter and set it to the values in XML.
	The current/last located node must be the first child of the yield changes node */
int CvXMLLoadUtility::SetYields(int** ppiYield)
{
	if (!SkipToNextVal()) // Skip any comments and stop at the next value we might want
		return 0;

	int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
	InitList(ppiYield, NUM_YIELD_TYPES);
	int *piYield = *ppiYield; // set the local pointer to the memory we just allocated
	if (iNumSibs <= 0)
		return 0;
	// set the current xml node to its first non-comment child and set the parameter to the new node's value
	if (!GetChildXmlVal(piYield[0]))
		return 0;

	if(iNumSibs > NUM_YIELD_TYPES)
	{
		char szMessage[1024];
		sprintf(szMessage, "For loop iterator is greater than array size.\nCurrent XML file is: %s",
				GC.getCurrentXMLFile().GetCString());
		errorMessage(szMessage);
	}
	// loop through all the siblings, we start at 1 since we already have the first value
	for (int i = 1; i < iNumSibs; i++)
	{
		// set the current node to it's first non-comment sibling and
		// set the parameter to the new node's value
		if (!GetNextXmlVal(piYield[i]))
			break;
	}
	gDLL->getXMLIFace()->SetToParent(m_pFXml); // set the current xml node to its parent
	return iNumSibs;
}

// allocate and set the feature struct variables for the CvBuildInfo class
void CvXMLLoadUtility::SetFeatureStruct(int** ppiFeatureTech, int** ppiFeatureTime, int** ppiFeatureProduction, bool** ppbFeatureRemove)
{
	if(GC.getNumFeatureInfos() < 1)
	{
		char szMessage[1024];
		sprintf(szMessage, "No feature infos set yet!\nCurrent XML file is: %s", GC.getCurrentXMLFile().GetCString());
		errorMessage(szMessage);
	}
	InitList(ppiFeatureTech, GC.getNumFeatureInfos(), -1);
	InitList(ppiFeatureTime, GC.getNumFeatureInfos());
	InitList(ppiFeatureProduction, GC.getNumFeatureInfos());
	InitList(ppbFeatureRemove, GC.getNumFeatureInfos());

	int* paiFeatureTech = *ppiFeatureTech;
	int* paiFeatureTime = *ppiFeatureTime;
	int* paiFeatureProduction = *ppiFeatureProduction;
	bool* pabFeatureRemove = *ppbFeatureRemove;

	if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,"FeatureStructs"))
	{
		int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
		if (0 < iNumSibs)
		{
			if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,"FeatureStruct"))
			{
				if(iNumSibs > GC.getNumFeatureInfos())
				{
					char szMessage[1024];
					sprintf(szMessage, "iNumSibs is greater than GC.getNumFeatureInfos in SetFeatureStruct function.\nCurrent XML file is: %s",
							GC.getCurrentXMLFile().GetCString());
					errorMessage(szMessage);
				}
				TCHAR szTextVal[256]; // temporarily hold the text value of the current xml node
				for (int i = 0; i < iNumSibs; i++)
				{
					GetChildXmlValByName(szTextVal, "FeatureType");
					int iFeatureIndex = FindInInfoClass(szTextVal);
					if(iFeatureIndex == -1)
					{
						char	szMessage[1024];
						sprintf(szMessage, "iFeatureIndex is -1 inside SetFeatureStruct function.\nCurrent XML file is: %s", GC.getCurrentXMLFile().GetCString());
						errorMessage(szMessage);
					}
					GetChildXmlValByName(szTextVal, "PrereqTech");
					paiFeatureTech[iFeatureIndex] = FindInInfoClass(szTextVal);
					GetChildXmlValByName(&paiFeatureTime[iFeatureIndex], "iTime");
					GetChildXmlValByName(&paiFeatureProduction[iFeatureIndex], "iProduction");
					GetChildXmlValByName(&pabFeatureRemove[iFeatureIndex], "bRemove");

					if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
					{
						break;
					}
				}
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
			}
		}
		gDLL->getXMLIFace()->SetToParent(m_pFXml);
	}
}

// Allocate memory for the improvement bonus pointer and fill with the values in XML.
void CvXMLLoadUtility::SetImprovementBonuses(CvImprovementBonusInfo** ppImprovementBonus)
{
	CvImprovementBonusInfo* paImprovementBonus;	// local pointer to the bonus type struct in memory
	if (SkipToNextVal()) // Skip any comments and stop at the next value we might want
	{
		// initialize the boolean list to the correct size and all the booleans to false
		InitImprovementBonusList(ppImprovementBonus, GC.getNumBonusInfos());
		// set the local pointer to the memory we just allocated
		paImprovementBonus = *ppImprovementBonus;

		int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
		// if we can set the current xml node to the child of the one it is at now
		if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
		{
			if(iNumSibs > GC.getNumBonusInfos())
			{
				char szMessage[1024];
				sprintf(szMessage, "For loop iterator is greater than array size.\nCurrent XML file is: %s",
						GC.getCurrentXMLFile().GetCString());
				errorMessage(szMessage);
			}
			TCHAR szNodeVal[256]; // temporarily holds the string value of the current xml node
			for (int i = 0; i < iNumSibs; i++)
			{
				if (SkipToNextVal()) // skip to the next non-comment node
				{
					// set the FXml pointer to the first non-comment child of
					// the current tag and get the value of that new node
					if (GetChildXmlVal(szNodeVal))
					{
						// index of the match in the bonus types list; -1 if no value is found.
						int iBonusIndex = FindInInfoClass(szNodeVal);
						// if we found a match we will get the next sibling's boolean value at that match's index
						if (iBonusIndex >= 0)
						{
							GetNextXmlVal(paImprovementBonus[iBonusIndex].m_bBonusMakesValid);
							GetNextXmlVal(paImprovementBonus[iBonusIndex].m_bBonusTrade);
							GetNextXmlVal(paImprovementBonus[iBonusIndex].m_iDiscoverRand);
							gDLL->getXMLIFace()->SetToParent(m_pFXml);

							SAFE_DELETE_ARRAY(paImprovementBonus[iBonusIndex].m_piYieldChange);	// free memory - MT, since we are about to reallocate
							if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,"YieldChanges"))
							{
								SetYields(&paImprovementBonus[iBonusIndex].m_piYieldChange);
								gDLL->getXMLIFace()->SetToParent(m_pFXml);
							}
							else InitList(&paImprovementBonus[iBonusIndex].m_piYieldChange, NUM_YIELD_TYPES);
						}
						else gDLL->getXMLIFace()->SetToParent(m_pFXml);
					}
					if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
						break;
				}
			}
			gDLL->getXMLIFace()->SetToParent(m_pFXml); // set the current xml node to its parent
		}
	}
}

// set the variable to a default and load it from XML if there are any children
/*  advc.003j (comment): Unused, and apparently never was used. But sounds like it
	could be useful. */
bool CvXMLLoadUtility::SetAndLoadVar(int** ppiVar, int iDefault)
{
	if (!SkipToNextVal()) // Skip any comments and stop at the next value we might want
		return false;

	int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
	InitList(ppiVar, iNumSibs, iDefault); // allocate memory
	int* piVar = *ppiVar; // set the a local pointer to the newly allocated memory
	// set the current xml node to it's first non-comment
	// and set the parameter to the new node's value
	if (GetChildXmlVal(piVar[0]))
	{
		// loop through all the siblings, we start at 1 since we already got the first sibling
		for (int i = 1; i < iNumSibs; i++)
		{
			// if the call to the function that sets the current xml node to it's next non-comment
			// sibling and sets the parameter with the new node's value does not succeed
			// we will break out of this for loop
			if (!GetNextXmlVal(piVar[i]))
				break;
		}
		gDLL->getXMLIFace()->SetToParent(m_pFXml); // set the current xml node to its parent
	}
	return true;
}

/*  advc.003t: Will set the array that pptList points to to NULL if
	tDefaultListVal is 0 and no pairs are found or if all (index,value) pairs
	have the value 0. */
template<typename T>
void CvXMLLoadUtility::SetVariableListTagPair(T** pptList, const TCHAR* szRootTagName,
	int iInfoBaseLength, T tDefaultListVal) // (advc.003x: Unused param iInfoBaseSize removed)
{
	if (iInfoBaseLength <= 0)
	{
		char szMessage[1024];
		sprintf(szMessage, "Allocating zero or less memory.\nCurrent XML file is: %s",
				GC.getCurrentXMLFile().GetCString());
		errorMessage(szMessage);
	}
	bool bListModified = (*pptList != NULL); // advc.003t, advc.xmldefault
	InitList(pptList, iInfoBaseLength, tDefaultListVal);
	if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml, szRootTagName))
	{
		if (SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
			T* ptList = *pptList;
			if (iNumSibs > 0)
			{
				if(iNumSibs > iInfoBaseLength)
				{
					char szMessage[1024];
					sprintf(szMessage, "There are more siblings than memory allocated for them.\nCurrent XML file is: %s",
							GC.getCurrentXMLFile().GetCString());
					errorMessage(szMessage);
				}
				if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
				{
					TCHAR szTextVal[256];
					for (int i = 0; i < iNumSibs; i++)
					{
						if (SkipToNextVal() && // K-Mod. (without this, a comment in the xml could break this)
							GetChildXmlVal(szTextVal))
						{
							int iIndexVal = FindInInfoClass(szTextVal);
							if (iIndexVal >= 0)
							{
								GetNextXmlVal(ptList[iIndexVal]);
								// <advc.003t>
								if (ptList[iIndexVal] != tDefaultListVal)
									bListModified = true; // </advc.003t>
							}
							gDLL->getXMLIFace()->SetToParent(m_pFXml);
						}
						if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
							break;
					}
					gDLL->getXMLIFace()->SetToParent(m_pFXml);
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(m_pFXml);
	}
	// <advc.003t>
	if (!bListModified && tDefaultListVal == 0)
		SAFE_DELETE_ARRAY(*pptList); // </advc.003t>
}
/************************************************************************************************/
/* RevDCM  XMLloading                             05/05/10             phungus420               */
/*                         cantrain                                                                     */
/*                        i think this is for the tga index or the unit tags from revdcm? - keldath                                                                      */
/************************************************************************************************/
//allows loading of a variable pair into an array by tag name
//NOTE THAT THIS FUNC IS IDENTICAL ALMOST TO THE ABOVE - keldath
void CvXMLLoadUtility::SetVariableListTagPairRevDCM(int **ppiList, const TCHAR* szRootTagName,
		int iInfoBaseSize, int iInfoBaseLength,
		const TCHAR* szValueTagName, int iValueInfoBaseLength, int iDefaultListVal)
{
	int i;
	int iIndexVal;
	int iNumSibs;
	int iValue;
	TCHAR szTextPosition[256];
	TCHAR szTextVal[256];
	int* piList = NULL;

	if (0 > iInfoBaseLength)
	{
		char	szMessage[1024];
		sprintf( szMessage, "Allocating zero or less memory in CvXMLLoadUtility::SetVariableListTagPair \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	InitList(ppiList, iInfoBaseLength, iDefaultListVal);
	piList = *ppiList;

	if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szRootTagName))
	{
		if (SkipToNextVal())
		{
			iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
			if (0 < iNumSibs)
			{
				if(!(iNumSibs <= iInfoBaseLength))
				{
					char	szMessage[1024];
					sprintf( szMessage, "There are more siblings than memory allocated for them in CvXMLLoadUtility::SetVariableListTagPair \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
					gDLL->MessageBox(szMessage, "XML Error");
				}
				if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
				{
					for (i=0;i<iNumSibs;i++)
					{
						if (GetChildXmlVal(szTextPosition))
						{
							iIndexVal = FindInInfoClass(szTextPosition);

							if (iIndexVal != -1)
							{
								gDLL->getXMLIFace()->SetToParent(m_pFXml);
								GetChildXmlValByName(szTextVal, szValueTagName);
								iValue = FindInInfoClass(szTextVal);
								if( (iValue < -1) || (iValue >= iValueInfoBaseLength) )
								{
									char	szMessage[1024];
									sprintf( szMessage, "A defined value for an array is outside of the accepted size of the infoclass!\n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
									gDLL->MessageBox(szMessage, "XML Error");
								} else
								{
									piList[iIndexVal] = iValue;
								}
							} else
							{
								gDLL->getXMLIFace()->SetToParent(m_pFXml);
							}
						}

						if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
						{
							break;
						}
					}

					gDLL->getXMLIFace()->SetToParent(m_pFXml);
				}
			}
		}

		gDLL->getXMLIFace()->SetToParent(m_pFXml);
	}
}
/************************************************************************************************/
/* RevDCM	                                 END                                                */
/************************************************************************************************/
// <advc> Explicit instantiations of member function template
template void CvXMLLoadUtility::SetVariableListTagPair(int**, const TCHAR*, int, int);
template void CvXMLLoadUtility::SetVariableListTagPair(bool**, const TCHAR*, int, bool);
template void CvXMLLoadUtility::SetVariableListTagPair(float**, const TCHAR*, int, float);
// </advc>

// advc.003t: See SetVariableListTagPair(int**,...) above
void CvXMLLoadUtility::SetVariableListTagPair(CvString **ppszList, const TCHAR* szRootTagName,
	int iInfoBaseLength, CvString szDefaultListVal)
{
	// <advc.xmldefault>
	if (*ppszList != NULL)
		return; // </advc.xmldefault>
	if(iInfoBaseLength <= 0)
	{
		char szMessage[1024];
		sprintf(szMessage, "Allocating zero or less memory.\nCurrent XML file is: %s",
				GC.getCurrentXMLFile().GetCString());
		errorMessage(szMessage);
	}
	bool bListModified = (*ppszList != NULL); // advc.003t, advc.xmldefault
	InitStringList(ppszList, iInfoBaseLength, szDefaultListVal);
	if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szRootTagName))
	{
		if (SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
			CvString* pszList = *ppszList;
			if (iNumSibs > 0)
			{
				if(iNumSibs > iInfoBaseLength)
				{
					char szMessage[1024];
					sprintf(szMessage, "There are more siblings than memory allocated for them.\nCurrent XML file is: %s",
							GC.getCurrentXMLFile().GetCString());
					errorMessage(szMessage);
				}
				if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
				{
					TCHAR szTextVal[256];
					for (int i = 0; i < iNumSibs; i++)
					{
						//if (GetChildXmlVal(szTextVal))
						if (SkipToNextVal() && GetChildXmlVal(szTextVal)) // K-Mod. (without this, a comment in the xml could break this)
						{
							int iIndexVal = FindInInfoClass(szTextVal);
							if (iIndexVal >= 0)
							{
								GetNextXmlVal(pszList[iIndexVal]);
								/*  <advc.003t> Since bListModified will only matter
									if szDefaultListVal is an empty string, let's
									simply check: */
								if (!pszList[iIndexVal].empty())
									bListModified = true;
								/*  For the primitive types, 0-entries in XML are usually
									deliberate (for readability), but empty strings
									would be strange. */
								else FAssertMsg(false, "Empty string in list of tag pairs");
								// </advc.003t>
							}
							gDLL->getXMLIFace()->SetToParent(m_pFXml);
						}
						if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
							break;
					}
					gDLL->getXMLIFace()->SetToParent(m_pFXml);
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(m_pFXml);
	}
	// <advc.003t>
	if (!bListModified && szDefaultListVal.empty())
		SAFE_DELETE_ARRAY(*ppszList); // </advc.003t>
}

// allocate and initialize a list from a tag pair in XML
void CvXMLLoadUtility::SetVariableListTagPair(int **ppiList, const TCHAR* szRootTagName,
	CvString* m_paszTagList, int iTagListLength, int iDefaultListVal)
{
	if(iTagListLength <= 0)
	{
		char szMessage[1024];
		sprintf(szMessage, "Allocating zero or less memory.\nCurrent XML file is: %s",
				GC.getCurrentXMLFile().GetCString());
		errorMessage(szMessage);
	}
	InitList(ppiList, iTagListLength, iDefaultListVal);
	if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szRootTagName))
	{
		if (SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
			int* piList = *ppiList;
			if (iNumSibs > 0)
			{
				if(iNumSibs > iTagListLength)
				{
					char szMessage[1024];
					sprintf(szMessage, "There are more siblings than memory allocated for them.\nCurrent XML file is: %s",
							GC.getCurrentXMLFile().GetCString());
					errorMessage(szMessage);
				}
				if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
				{
					TCHAR szTextVal[256];
					for (int i = 0; i < iNumSibs; i++)
					{
						if (SkipToNextVal() && // K-Mod. (without this, a comment in the xml could break this)
							GetChildXmlVal(szTextVal))
						{
							int iIndexVal = GC.getTypesEnum(szTextVal);
							if (iIndexVal >= 0)
								GetNextXmlVal(piList[iIndexVal]);
							gDLL->getXMLIFace()->SetToParent(m_pFXml);
						}
						if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
							break;
					}
					gDLL->getXMLIFace()->SetToParent(m_pFXml);
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(m_pFXml);
	}
}

// allocate and initialize a list from a tag pair in the xml for audio scripts
void CvXMLLoadUtility::SetVariableListTagPairForAudioScripts(int **ppiList, const TCHAR* szRootTagName,
	CvString* m_paszTagList, int iTagListLength, int iDefaultListVal)
{
	if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szRootTagName))
	{
		if (SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
			if(iTagListLength <= 0)
			{
				char szMessage[1024];
				sprintf(szMessage, "Allocating zero or less memory.\nCurrent XML file is: %s",
						GC.getCurrentXMLFile().GetCString());
				errorMessage(szMessage);
			}
			InitList(ppiList, iTagListLength, iDefaultListVal);
			int* piList = *ppiList;
			if (iNumSibs > 0)
			{
				if(iNumSibs > iTagListLength)
				{
					char szMessage[1024];
					sprintf(szMessage, "There are more siblings than memory allocated for them.\nCurrent XML file is: %s",
							GC.getCurrentXMLFile().GetCString());
					errorMessage(szMessage);
				}
				if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
				{
					TCHAR szTextVal[256];
					for (int i = 0; i < iNumSibs; i++)
					{
						if (SkipToNextVal() && // K-Mod. (without this, a comment in the xml could break this)
							GetChildXmlVal(szTextVal))
						{
							int iIndexVal =	GC.getTypesEnum(szTextVal);
							if (iIndexVal >= 0)
							{
								CvString szTemp;
								GetNextXmlVal(szTemp);
								if (szTemp.GetLength() > 0)
									piList[iIndexVal] = gDLL->getAudioTagIndex(szTemp);
								else piList[iIndexVal] = -1;
							}

							gDLL->getXMLIFace()->SetToParent(m_pFXml);
						}
						if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
							break;
					}
					gDLL->getXMLIFace()->SetToParent(m_pFXml);
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(m_pFXml);
	}
}

// allocate and initialize a list from a tag pair in the xml for audio scripts
void CvXMLLoadUtility::SetVariableListTagPairForAudioScripts(int **ppiList, const TCHAR* szRootTagName, int iInfoBaseLength, int iDefaultListVal)
{
	if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szRootTagName))
	{
		if (SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
			if(iInfoBaseLength <= 0)
			{
				char szMessage[1024];
				sprintf(szMessage, "Allocating zero or less memory.\nCurrent XML file is: %s",
						GC.getCurrentXMLFile().GetCString());
				errorMessage(szMessage);
			}
			InitList(ppiList, iInfoBaseLength, iDefaultListVal);
			int* piList = *ppiList;
			if (iNumSibs > 0)
			{
				if(iNumSibs > iInfoBaseLength)
				{
					char szMessage[1024];
					sprintf(szMessage, "There are more siblings than memory allocated for them.\nCurrent XML file is: %s",
							GC.getCurrentXMLFile().GetCString());
					errorMessage(szMessage);
				}
				if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
				{
					TCHAR szTextVal[256];
					for (int i = 0; i < iNumSibs; i++)
					{
						if (SkipToNextVal() && // K-Mod. (without this, a comment in the xml could break this)
							GetChildXmlVal(szTextVal))
						{
							int iIndexVal = FindInInfoClass(szTextVal);
							if (iIndexVal >= 0)
							{
								CvString szTemp;
								GetNextXmlVal(szTemp);
								if (szTemp.GetLength() > 0)
									piList[iIndexVal] = gDLL->getAudioTagIndex(szTemp);
								else piList[iIndexVal] = -1;
							}
							gDLL->getXMLIFace()->SetToParent(m_pFXml);
						}
						if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
							break;
					}
					gDLL->getXMLIFace()->SetToParent(m_pFXml);
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(m_pFXml);
	}
}

// allocate and initialize a list from a tag pair in the xml
void CvXMLLoadUtility::SetVariableListTagPair(bool **ppbList, const TCHAR* szRootTagName,
	CvString* m_paszTagList, int iTagListLength, bool bDefaultListVal)
{
	if(iTagListLength <= 0)
	{
		char szMessage[1024];
		sprintf(szMessage, "Allocating zero or less memory.\nCurrent XML file is: %s",
				GC.getCurrentXMLFile().GetCString());
		errorMessage(szMessage);
	}
	InitList(ppbList, iTagListLength, bDefaultListVal);
	if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szRootTagName))
	{
		if (SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
			bool* pbList = *ppbList;
			if (iNumSibs > 0)
			{
				if(iNumSibs > iTagListLength)
				{
					char szMessage[1024];
					sprintf(szMessage, "There are more siblings than memory allocated for them.\nCurrent XML file is: %s",
							GC.getCurrentXMLFile().GetCString());
					errorMessage(szMessage);
				}
				if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
				{
					TCHAR szTextVal[256];
					for (int i = 0; i < iNumSibs; i++)
					{
						//if (GetChildXmlVal(szTextVal))
						if (SkipToNextVal() && GetChildXmlVal(szTextVal)) // K-Mod. (without this, a comment in the xml could break this)
						{
							int iIndexVal =	GC.getTypesEnum(szTextVal);
							if (iIndexVal >= 0)
								GetNextXmlVal(pbList[iIndexVal]);
							gDLL->getXMLIFace()->SetToParent(m_pFXml);
						}
						if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
							break;
					}
					gDLL->getXMLIFace()->SetToParent(m_pFXml);
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(m_pFXml);
	}
}

// allocate and initialize a list from a tag pair in the xml
void CvXMLLoadUtility::SetVariableListTagPair(CvString **ppszList, const TCHAR* szRootTagName,
	CvString* m_paszTagList, int iTagListLength, CvString szDefaultListVal)
{
	// <advc.xmldefault>
	if (*ppszList != NULL)
		return; // </advc.xmldefault>
	if(iTagListLength <= 0)
	{
		char szMessage[1024];
		sprintf(szMessage, "Allocating zero or less memory.\nCurrent XML file is: %s",
				GC.getCurrentXMLFile().GetCString());
		errorMessage(szMessage);
	}
	InitStringList(ppszList, iTagListLength, szDefaultListVal);
	if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szRootTagName))
	{
		if (SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
			CvString* pszList = *ppszList;
			if (iNumSibs > 0)
			{
				if(iNumSibs > iTagListLength)
				{
					char szMessage[1024];
					sprintf(szMessage, "There are more siblings than memory allocated for them.\nCurrent XML file is: %s",
							GC.getCurrentXMLFile().GetCString());
					errorMessage(szMessage);
				}
				if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
				{
					TCHAR szTextVal[256];
					for (int i = 0; i < iNumSibs; i++)
					{
						//if (GetChildXmlVal(szTextVal))
						if (SkipToNextVal() && GetChildXmlVal(szTextVal)) // K-Mod. (without this, a comment in the xml could break this)
						{
							int iIndexVal =	GC.getTypesEnum(szTextVal);
							if (iIndexVal >= 0)
								GetNextXmlVal(pszList[iIndexVal]);
							gDLL->getXMLIFace()->SetToParent(m_pFXml);
						}
						if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
							break;
					}
					gDLL->getXMLIFace()->SetToParent(m_pFXml);
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(m_pFXml);
	}
}

DllExport bool CvXMLLoadUtility::LoadPlayerOptions()
{
	if (!CreateFXml())
		return false;

	LoadGlobalClassInfo(GC.m_paPlayerOptionInfo, "CIV4PlayerOptionInfos", "GameInfo", "Civ4PlayerOptionInfos/PlayerOptionInfos/PlayerOptionInfo", false);
	FAssert(GC.getNumPlayerOptionInfos() == NUM_PLAYEROPTION_TYPES);
	DestroyFXml();
	return true;
}

DllExport bool CvXMLLoadUtility::LoadGraphicOptions()
{
	if (!CreateFXml())
		return false;

	LoadGlobalClassInfo(GC.m_paGraphicOptionInfo, "CIV4GraphicOptionInfos", "GameInfo", "Civ4GraphicOptionInfos/GraphicOptionInfos/GraphicOptionInfo", false);
	FAssert(GC.getNumGraphicOptions() == NUM_GRAPHICOPTION_TYPES);
	DestroyFXml();
	return true;
}
/*************************************************************************************************/
/** TGA_INDEXATION                          01/21/08                                MRGENIE      */
/**                                                                                              */
/** reorganizing the Corporations and Religions vectors                                          */
/*************************************************************************************************/
template <class T>
void CvXMLLoadUtility::ArrangeTGA(std::vector<T*>& aInfos, const char* szInfo)
{
	int iMinIndex;
	CvString szDebugBuffer;

	T* pTempClassInfo = new T;	// Temp to store while swapping positions

	FAssert(NULL != pTempClassInfo);
	if (NULL == pTempClassInfo)
	{
		return;
	}

	CvString verify = CvString::format("%s", szInfo).GetCString();  //string comparison

	if ( verify == "CIV4ReligionInfo" )
	{
#ifdef DEBUG_TGA_LOADING
		GC.logInfoTypeMap("Before religion game font rearranging");
		// loop through the religions see what we've got
		szDebugBuffer.Format("=== BEFORE %s TGA indices. ===", szInfo);
		gDLL->logMsg("CvXMLLoadUtility_ArrangeTGA.log", szDebugBuffer.c_str());
		for (std::vector<T*>::const_iterator it = aInfos.begin(); it != aInfos.end(); it++)
		{
			szDebugBuffer.Format("* Religion %s: TGA index %i; type ID %i", (*it)->getType(), (*it)->getTGAIndex(), GC.getInfoTypeForString((*it)->getType()));
			gDLL->logMsg("CvXMLLoadUtility_ArrangeTGA.log", szDebugBuffer.c_str());

		}
#endif
		for ( int iReligion = 0; iReligion < (int)aInfos.size(); iReligion++ )
		{
			iMinIndex = iReligion;

			// if we found the Religion with the proper GTAIndex, make sure it's on the right spot in the aInfos vector
			for ( int iReligionCheck = iReligion + 1; iReligionCheck < (int)aInfos.size(); iReligionCheck++ )
			{
				if ( GC.getReligionInfo((ReligionTypes)iReligionCheck).getTGAIndex() < GC.getReligionInfo((ReligionTypes)iMinIndex).getTGAIndex() )
				{
					iMinIndex = iReligionCheck;
				}
			}

			if ( iMinIndex != iReligion )
			{
#ifdef DEBUG_TGA_LOADING
				szDebugBuffer.Format(" * Found candidates to swap: %i <-> %i", iMinIndex, iReligion);
				gDLL->logMsg("CvXMLLoadUtility_ArrangeTGA.log", szDebugBuffer.c_str());
#endif

				int iOldType1 = GC.getInfoTypeForString(aInfos[iReligion]->getType());
				int iOldType2 = GC.getInfoTypeForString(aInfos[iMinIndex]->getType());

#ifdef DEBUG_TGA_LOADING
				szDebugBuffer.Format(" -- Old info types: %s: %i <-> %s: %i", aInfos[iReligion]->getType(), iOldType1, aInfos[iMinIndex]->getType(), iOldType2);
				gDLL->logMsg("CvXMLLoadUtility_ArrangeTGA.log", szDebugBuffer.c_str());
#endif

				GC.setInfoTypeFromString(aInfos[iReligion]->getType(), iOldType2);
				GC.setInfoTypeFromString(aInfos[iMinIndex]->getType(), iOldType1);

				pTempClassInfo = aInfos[iReligion];			//store in temp value
				aInfos[iReligion] = aInfos[iMinIndex];		//move iReligion to proper place
				aInfos[iMinIndex] = pTempClassInfo;			//finish the swap

				// Set the InfoTypeFromString map properly according to the new alphabetical order
				/*if (NULL != aInfos[iReligion]->getType())
				{*/
					// overwrite existing info maps with the proper id's
					//GC.setInfoTypeFromString(aInfos[iReligion]->getType(), iMinIndex, true);
				/*}
				if (NULL != aInfos[iMinIndex]->getType())
				{*/
					// overwrite existing info maps with the proper id's
					//GC.setInfoTypeFromString(aInfos[iMinIndex]->getType(), iReligion, true);
				//}

#ifdef DEBUG_TGA_LOADING
				szDebugBuffer.Format(" -- NEW info types: %s: %i <-> %s: %i", aInfos[iReligion]->getType(), GC.getInfoTypeForString(aInfos[iReligion]->getType()), aInfos[iMinIndex]->getType(), GC.getInfoTypeForString(aInfos[iMinIndex]->getType()));
				gDLL->logMsg("CvXMLLoadUtility_ArrangeTGA.log", szDebugBuffer.c_str());
#endif
			}
		}
#ifdef DEBUG_TGA_LOADING
		szDebugBuffer.Format("=== AFTER %s TGA indices. ===", szInfo);
		gDLL->logMsg("CvXMLLoadUtility_ArrangeTGA.log", szDebugBuffer.c_str());
		for (std::vector<T*>::const_iterator it = aInfos.begin(); it != aInfos.end(); it++)
		{
			szDebugBuffer.Format("* Religion %s: TGA index %i; type ID %i", (*it)->getType(), (*it)->getTGAIndex(), GC.getInfoTypeForString((*it)->getType()));
			gDLL->logMsg("CvXMLLoadUtility_ArrangeTGA.log", szDebugBuffer.c_str());

		}

		GC.logInfoTypeMap("After religion game font rearranging");
#endif
	}
	

	if ( verify == "CIV4CorporationInfo" )
	{		
		// loop through the religions see what we've got
		for ( int iCorporation = 0; iCorporation < (int)aInfos.size(); iCorporation++ )
		{
			iMinIndex = iCorporation;

			// if we found the Religion with the proper GTAIndex, make sure it's on the right spot in the aInfos vector
			for ( int iCorporationCheck = iCorporation + 1; iCorporationCheck < (int)aInfos.size(); iCorporationCheck++ )
			{
				if ( GC.getCorporationInfo((CorporationTypes)iCorporationCheck).getTGAIndex() < GC.getCorporationInfo((CorporationTypes)iMinIndex).getTGAIndex() )
				{
					iMinIndex = iCorporationCheck;
				}
			}

			if ( iMinIndex != iCorporation )
			{
				pTempClassInfo = aInfos[iCorporation];			//store in temp value
				aInfos[iCorporation] = aInfos[iMinIndex];		//move iReligion to proper place
				aInfos[iMinIndex] = pTempClassInfo;			//finish the swap
				// Set the InfoTypeFromString map properly according to the new alphabetical order
				if (NULL != aInfos[iCorporation]->getType())
				{
					// overwrite existing info maps with the proper id's
					GC.setInfoTypeFromString(aInfos[iCorporation]->getType(), iMinIndex);
				}
				if (NULL != aInfos[iMinIndex]->getType())
				{
					// overwrite existing info maps with the proper id's
					GC.setInfoTypeFromString(aInfos[iMinIndex]->getType(), iCorporation);
				}
			}
		}
	}
}

template <class T>
void CvXMLLoadUtility::AddTGABogus(std::vector<T*>& aInfos, const char* szInfo)
{
	bool bTGAInfoTypeValid = false;

	T* pTempClassInfo = new T;	// Temp to store while swapping positions
	T* pClassInfo = new T;		// Bogus InfoType

	FAssert(NULL != pTempClassInfo);
	FAssert(NULL != pClassInfo);
	if (NULL == pClassInfo || NULL == pTempClassInfo)
	{
		return;
	}

	CvString verify = CvString::format("%s", szInfo).GetCString();  //string comparison

	if ( verify == "CIV4ReligionInfo" )
	{
		for ( int iI = 0; iI < GC.getDefineINT(CvGlobals::GAMEFONT_TGA_RELIGIONS); iI++ )
		{
			bTGAInfoTypeValid = false;
			// loop through the religions see what we've got
			for ( int iReligion = 0; iReligion < (int)aInfos.size(); iReligion++ )
			{
				// if we found the Religion with the proper GTAIndex, make sure it's on the right spot in the aInfos vector
				if ( GC.getReligionInfo((ReligionTypes)iReligion).getTGAIndex() == iI )
				{
					bTGAInfoTypeValid = true;
				}
			}
			if ( !bTGAInfoTypeValid)
			{
				aInfos.insert(aInfos.begin() + iI, pClassInfo);
			}
		}
	}

	if ( verify == "CIV4CorporationInfo" )
	{
		for ( int iI = 0; iI < GC.getDefineINT(CvGlobals::GAMEFONT_TGA_CORPORATIONS) ; iI++ )
		{
			bTGAInfoTypeValid = false;
			// loop through the religions see what we've got
			for ( int iCorporation = 0; iCorporation < (int)aInfos.size(); iCorporation++ )
			{
				// if we found the Religion with the proper GTAIndex, make sure it's on the right spot in the aInfos vector
				if ( GC.getCorporationInfo((CorporationTypes)iCorporation).getTGAIndex() == iI )
				{
					bTGAInfoTypeValid = true;
				}
			}
			if ( !bTGAInfoTypeValid)
			{
				aInfos.insert(aInfos.begin() + iI, pClassInfo);
			}
		}
	}
}

// Made an own little function for this one in case we have to clean up any further info vectors in future
// principly any one can be added here in future, if everything is ok, nothing will be done anyway
void CvXMLLoadUtility::cleanTGA()
{
//	RemoveTGABogusReligion(GC.getReligionInfo());
//	RemoveTGABogusCorporation(GC.getCorporationInfo());
	RemoveTGABogusReligion(GC.m_paReligionInfo);
	RemoveTGABogusCorporation(GC.m_paCorporationInfo);

}

template <class T>
void CvXMLLoadUtility::RemoveTGABogusReligion(std::vector<T*>& aInfos)
{
	std::vector <int> viDeletionVector;

	for ( int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if ( NULL == GC.getReligionInfo((ReligionTypes)iI).getType() )
		{
			viDeletionVector.push_back(iI);
		}
	}

	for ( int iI = (int)viDeletionVector.size() - 1; iI >= 0; iI--)	// gotta do this backwards
	{
		aInfos.erase( aInfos.begin()+viDeletionVector.at(iI));
	}
}

template <class T>
void CvXMLLoadUtility::RemoveTGABogusCorporation(std::vector<T*>& aInfos)
{
	std::vector <int> viDeletionVector;

	for ( int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if ( NULL == GC.getCorporationInfo((CorporationTypes)iI).getType() )
		{
			viDeletionVector.push_back(iI);
		}
	}

	for ( int iI = (int)viDeletionVector.size() - 1; iI >= 0; iI--)	// gotta do this backwards
	{
		aInfos.erase( aInfos.begin()+viDeletionVector.at(iI));
	}
}
/*************************************************************************************************/
/** TGA_INDEXATION                          END                                                  */
/*************************************************************************************************/
