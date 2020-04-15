#pragma once

#ifndef CV_INFO_CITY_H
#define CV_INFO_CITY_H

/*  advc.003x: Cut from CvInfos.h. Info classes needed for city management:
	CvProcessInfo, CvSpecialistInfo, CvCultureLevelInfo, CvEmphasizeInfo, CvHurryInfo
	and building- and unit-related info classes (via include). */

#include "CvInfo_Building.h"
#include "CvInfo_Unit.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvProcessInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProcessInfo : public CvInfoBase
{
public:
	CvProcessInfo();
	~CvProcessInfo();

	int getTechPrereq() const; // Exposed to Python
	int getProductionToCommerceModifier(int i) const; // Exposed to Python
	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iTechPrereq;
	int* m_paiProductionToCommerceModifier;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvSpecialistInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialistInfo : public CvHotkeyInfo
{
public: // All the const functions are exposed to Python
	CvSpecialistInfo();
	~CvSpecialistInfo();

	int getGreatPeopleUnitClass() const;
	int getGreatPeopleRateChange() const;
	int getMissionType() const;
	void setMissionType(int iNewType);
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                                                                                              */
/**                                                                                              */
/*************************************************************************************************/
	int getHealth() const;							// Exposed to Python
	int getHappiness() const;							// Exposed to Python
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/
	int getExperience() const;

	bool isVisible() const;

	int getYieldChange(int i) const;
	const int* getYieldChangeArray() const; // For Moose - CvWidgetData
	int getCommerceChange(int i) const;
	int getFlavorValue(int i) const;

	const TCHAR* getTexture() const;
	void setTexture(const TCHAR* szVal);

	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iGreatPeopleUnitClass;
	int m_iGreatPeopleRateChange;
	int m_iMissionType;
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                                                                                              */
/**                                                                                              */
/*************************************************************************************************/
	int m_iHealth;
	int m_iHappiness;
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/ 
	int m_iExperience;

	bool m_bVisible;

	CvString m_szTexture;

	int* m_piYieldChange;
	int* m_piCommerceChange;
	int* m_piFlavorValue;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvCultureLevelInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCultureLevelInfo : public CvInfoBase
{
public:
	CvCultureLevelInfo();
	~CvCultureLevelInfo();

	int getCityDefenseModifier() const; // Exposed to Python
	int getSpeedThreshold(int i) const; // Exposed to Python
	bool read(CvXMLLoadUtility* pXML);
	//influence driven war - added by f1rpo
	static CultureLevelTypes finalCultureLevel()
	{
		FAssert(GC.getNumCultureLevelInfos() > 0);
		return (CultureLevelTypes)(GC.getNumCultureLevelInfos() - 1);
	};

protected:
	int m_iCityDefenseModifier;
	int* m_paiSpeedThreshold;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvEmphasizeInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEmphasizeInfo : public CvInfoBase
{
public: // The const functions are exposed to Python

	CvEmphasizeInfo();
	~CvEmphasizeInfo();

	bool isAvoidGrowth() const;
	bool isGreatPeople() const;	

	int getYieldChange(int i) const;
	int getCommerceChange(int i) const;	

	bool read(CvXMLLoadUtility* pXML);

protected:
	bool m_bAvoidGrowth;
	bool m_bGreatPeople;

	int* m_piYieldModifiers;
	int* m_piCommerceModifiers;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvHurryInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHurryInfo : public CvInfoBase
{
public: // The const functions are exposed to Python
	CvHurryInfo();

	int getGoldPerProduction() const;
	int getProductionPerPopulation() const;
	bool isAnger() const;

	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iGoldPerProduction;
	int m_iProductionPerPopulation;
	bool m_bAnger;
};

#endif
