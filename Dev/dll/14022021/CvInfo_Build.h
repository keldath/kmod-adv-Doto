#pragma once

#ifndef CV_INFO_BUILD_H
#define CV_INFO_BUILD_H

/*  advc.003x: Cut from CvInfos.h. Just CvBuildInfo. That class is needed by
	.cpp files that deal with units and .cpp files that deal with terrain;
	don't want to bundle the unit and terrain info classes in a single big header,
	so CvBuilInfo needs to be singled out. */

class CvBuildInfo : public CvHotkeyInfo
{
public: /*	All the const functions are exposed to Python. advc.inl: inlined most of them.
			Integers in signatures replaced with enum types. */
	CvBuildInfo();
	~CvBuildInfo();

	inline int getTime() const { return m_iTime; }
	inline int getCost() const { return m_iCost; }
	inline TechTypes getTechPrereq() const { return (TechTypes)m_iTechPrereq; }
	inline ImprovementTypes getImprovement() const { return (ImprovementTypes)m_iImprovement; }
	inline RouteTypes getRoute() const { return (RouteTypes)m_iRoute; }
	DllExport int getEntityEvent() const;
	DllExport int getMissionType() const;
	void setMissionType(int iNewType);

	inline bool isKill() const { return m_bKill; }

	TechTypes getFeatureTech(FeatureTypes eFeature) const;
	int getFeatureTime(FeatureTypes eFeature) const;
	int getFeatureProduction(FeatureTypes eFeature) const;
	bool isFeatureRemove(FeatureTypes eFeature) const;

	friend void CyInfoPythonInterface2(); // advc.003x
	iPY_WRAP(FeatureTech, Feature)
	iPY_WRAP(FeatureTime, Feature)
	iPY_WRAP(FeatureProduction, Feature)
	bPY_WRAP(FeatureRemove, Feature)

	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iTime;
	int m_iCost;
	int m_iTechPrereq;
	int m_iImprovement;
	int m_iRoute;
	int m_iEntityEvent;
	int m_iMissionType;

	bool m_bKill;

	int* m_paiFeatureTech;
	int* m_paiFeatureTime;
	int* m_paiFeatureProduction;

	bool* m_pabFeatureRemove;
};

#endif
