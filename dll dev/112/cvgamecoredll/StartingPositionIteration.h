#pragma once

#ifndef STARTING_POSITION_ITERATION_H
#define STARTING_POSITION_ITERATION_H

class CitySiteEvaluator;

/*  advc.027: New file. A "starting position" is a selection of one starting tile
	per civ player. This algorithm, inspired by Voronoi iteration, iteratively
	tries to improve the fairness of an initial starting position. */

class StartingPositionIteration
{
	class DistanceTable;
	class PotentialSites;
	class Step;
	struct SolutionAttributes
	{
		scaled m_rStartPosVal; // Optimization goal
		// For heuristic search
		EnumMap<PlayerTypes,scaled> m_startValues;
		EnumMap<PlayerTypes,scaled> m_volatilityValues;
		PlayerTypes m_eWorstOutlier;
		scaled m_rAvgError;
		// For updating start value during normalization step
		EnumMap<PlayerTypes,scaled> m_foundValues;
		EnumMap<PlayerTypes,scaled> m_foundWeights;
		EnumMap<PlayerTypes,scaled> m_rivalMultipliers;
	};
	friend class NormalizationTarget; // Just to make SolutionAttributes visible

public:
	StartingPositionIteration();
	~StartingPositionIteration();
	// To be (safe-)deleted by caller!
	NormalizationTarget* createNormalizationTarget() const;
	static bool isDebug();

private:
	bool m_bRestrictedAreas;
	CitySiteEvaluator* m_pEval;
	EnumMap<PlotNumTypes,scaled> const* m_pYieldValues;
	std::map<CvArea const*,scaled> const* m_pYieldsPerArea;
	DistanceTable const* m_pPathDists;
	PotentialSites const* m_pPotentialSites;
	SolutionAttributes m_currSolutionAttribs;
	scaled m_rMedianLandYieldVal;
	bool m_bNormalizationTargetReady;

	CitySiteEvaluator* createSiteEvaluator(bool bNormalize = false) const;
	void evaluateCurrPosition(SolutionAttributes& kResult, bool bLog = false) const;
	void computeStartValues(EnumMap<PlayerTypes,short> const& kFoundValues,
			SolutionAttributes& kResult, bool bLog = false) const;
	scaled computeRivalDistFactors(EnumMap<PlayerTypes,scaled>& kResult,
			bool bSameArea) const;
	scaled outlierValue(EnumMap<PlayerTypes,scaled> const& kStartValues,
			PlayerTypes eIndex, scaled& rPercentage,
			scaled rNegativeOutlierExtraWeight = 0,
			scaled const* pMedian = NULL, bool* pbNegativeOutlier = NULL) const;
	scaled startingPositionValue(SolutionAttributes& kResult) const;
	void currAltSites(PlayerTypes eCurrSitePlayer,
			std::vector<std::pair<short,PlotNumTypes> >& kAltSitesByPriority,
			bool bIncludeRemote = false, PlotNumTypes eTakenSite = NO_PLOT_NUM) const;
	void logStep(Step const& kStep, SolutionAttributes const& kOldSolution,
			SolutionAttributes& kNewSolution, bool bStepTaken) const;
	bool considerStep(Step& kStep, SolutionAttributes& kCurrSolutionAttribs) const;
	void doIterations(PotentialSites& kPotentialSites);

	static scaled weightedDistance(std::vector<short>& kDistances);

	typedef std::set<PlotNumTypes> VoronoiCell;
	class PotentialSites
	{
	public:
		PotentialSites(CitySiteEvaluator const& kEval, bool bRestrictedAreas);
		~PotentialSites();
		size_t numSites() const { return m_foundValuesPerSite.size(); }
		void updateCurrSites(bool bUpdateCells = false);
		void getPlots(std::vector<CvPlot const*>& r) const;
		VoronoiCell* getCell(PlayerTypes eCurrSite) const;
		PlotNumTypes getRemoteSite(int iIndex) const;
		void getCurrFoundValues(
				EnumMap<PlayerTypes,short>& kFoundValuesPerPlayer) const;

	private:
		CitySiteEvaluator const& m_kEval;
		std::map<PlotNumTypes,short> m_foundValuesPerSite;
		std::map<PlayerTypes,VoronoiCell*> m_sitesClosestToCurrSite;
		EnumMap<PlotNumTypes,scaled>* m_pVicinityPenaltiesPerPlot;
		EnumMap<PlayerTypes,short> m_foundValuesPerCurrSite;
		std::vector<std::pair<int,PlotNumTypes> > m_remoteSitesByAreaSize;

		scaled computeMinFoundValue();
		void recordSite(CvPlot const& kPlot, short iFoundValue, bool bAdd);
		PlayerTypes closestPlayer(CvPlot const& kPlot) const;
		int fewestPotentialSites() const;
	};

	class SpaceEvaluator
	{
	public:
		SpaceEvaluator(DistanceTable const& kDists,
				EnumMap<PlotNumTypes,scaled> const& kYieldValues, bool bLog);
		scaled getSpaceValue(PlayerTypes ePlayer) const { return m_spaceValues.get(ePlayer); }
	private:
		// Claims are inverted distances; pretty small.
		typedef ScaledNum<1024*32,uint> claim_t;
		void computeSpaceValue(PlayerTypes ePlayer);
		static std::vector<claim_t> cacheDelayFactors(word iMaxDist);
		DistanceTable const& m_kDists;
		EnumMap<PlotNumTypes,scaled> const& m_kYieldValues;
		EnumMap<PlayerTypes,scaled> m_spaceValues;
		EnumMap<PlotNumTypes,claim_t> m_sumOfClaims;
		word m_iDistThresh;
		word m_iDistSubtr;
		bool m_bLog;
	};

	class DistanceTable
	{
		/*	Internal ids so that m_aaiDistances can be a good deal smaller than
			the map dimensions */
		enum SourceID { NOT_A_SOURCE = -1 };
		enum DestinationID { NOT_A_DESTINATION = -1 };
		// Needs to be able to traverse the table w/o having to go through all plots
		friend void StartingPositionIteration::SpaceEvaluator::computeSpaceValue(PlayerTypes);
	public:
		DistanceTable(std::vector<CvPlot const*>& kSources,
				std::vector<CvPlot const*>& kDestinations);
		short d(CvPlot const& kSource, CvPlot const& kDestination) const;
		/*	Typical distance according to DistanceTable::stepDist
			between two friendly adjacent cities. */
		inline short getAvgCityDist() const { return 40; }
		inline short getLongDist() const { return 8 * getAvgCityDist(); }

	private:
		std::vector<std::vector<short> > m_distances;
		std::vector<SourceID> m_sourceIDs;
		std::vector<DestinationID> m_destinationIDs;
		std::vector<PlotNumTypes> m_destinationIDToPlotNum;
		short m_iFirstFrontierCost;
		short m_iSecondFrontierCost;

		void computeDistances(CvPlot const& kSource);
		void setDistance(CvPlot const& kSource, CvPlot const& kDestination,
				short iDistance);
		short stepDist(CvPlot const& kFrom, CvPlot const& kTo, bool bSourceCoastal) const;
	};

	class Step
	{
	public:
		Step();
		void move(PlayerTypes ePlayer, CvPlot& kTo);
		void take();
		void takeBack();
		int getNumMoves() const { return (int)m_moves.size(); }
		PlayerTypes getFirstMovePlayer() const;
		std::string debugStr() const;
	private:
		std::vector<std::pair<PlayerTypes,CvPlot*> > m_moves;
		std::vector<std::pair<PlayerTypes,CvPlot*> > m_originalPosition;
	};
};

class NormalizationTarget
{
public:
	NormalizationTarget(CitySiteEvaluator& kEval,
			StartingPositionIteration::SolutionAttributes const& kSolution);
	~NormalizationTarget();
	bool isReached(CvPlot const& kStartSite) const
	{
		return isReached(kStartSite, false);
	}
	bool isNearlyReached(CvPlot const& kStartSite) const
	{
		return isReached(kStartSite, true);
	}
	// (currently unused)
	bool isClearlyExceeded(CvPlot const& kStartSite) const
	{
		return isReached(kStartSite, false, true);
	}
	scaled getStartValue(CvPlot const& kStartSite) const;
	scaled getVolatilityValue(CvPlot const& kStartSite) const;

private:
	CitySiteEvaluator* m_pEval;
	struct StartValBreakdown
	{
		scaled rTotal;
		scaled rVolatility;
		scaled rFoundVal;
		scaled rFoundWeight;
		scaled rRivalMult;
		StartValBreakdown(scaled rTotal, scaled rVolatility,
				scaled rFoundVal, scaled rFoundWeight, scaled rRivalMult) :
				rTotal(rTotal), rVolatility(rVolatility),
				rFoundVal(rFoundVal), rFoundWeight(rFoundWeight), rRivalMult(rRivalMult) {}
	};
	std::map<PlotNumTypes,StartValBreakdown> m_startValData;

	bool isReached(CvPlot const& kStartSite,
			bool bNearlyReached, bool bClearlyExeeded = false) const;
	StartValBreakdown const* getBreakdown(CvPlot const& kSite) const;
};

#endif
