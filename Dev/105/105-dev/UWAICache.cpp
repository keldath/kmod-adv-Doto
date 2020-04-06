// advc.104: New class; see UWAICache.h for description.

#include "CvGameCoreDLL.h"
#include "UWAICache.h"
#include "UWAIAgent.h"
#include "UWAIReport.h"
#include "MilitaryBranch.h"
#include "WarEvalParameters.h"
#include "WarEvaluator.h"
#include "CoreAI.h"
#include "CvSelectionGroupAI.h"
#include "CityPlotIterator.h"
#include "CvArea.h"
#include "CvInfo_Building.h"
#include "CvInfo_Unit.h"
#include "CvInfo_Terrain.h"

using std::vector;
using std::string;
using std::sort;

// Called only once per session (when starting or loading the first game)
UWAICache::UWAICache() : ownerId(NO_PLAYER), nNonNavyUnits(-1),
		goldPerProduction(-1), totalAssets(-1), bHasAggressiveTrait(false),
		bHasProtectiveTrait(false), canScrub(false), trainDeepSeaCargo(false),
		trainAnyCargo(false), focusOnPeacefulVictory(false) {}

// Called only on exit (to Desktop)
UWAICache::~UWAICache() {}

// Called when starting a new game (not when loading)
void UWAICache::init(PlayerTypes ownerId) {

	this->ownerId = ownerId;
	clear();
	updateTraits();
	// The order of the vector needs to match the enums in MilitaryBranch::Type
	militaryPower.push_back(new MilitaryBranch::HomeGuard(ownerId));
	militaryPower.push_back(new MilitaryBranch::Army(ownerId));
	militaryPower.push_back(new MilitaryBranch::Fleet(ownerId));
	militaryPower.push_back(new MilitaryBranch::Logistics(ownerId));
	militaryPower.push_back(new MilitaryBranch::Cavalry(ownerId));
	militaryPower.push_back(new MilitaryBranch::NuclearArsenal(ownerId));
}

void UWAICache::uninit() {

	clear();
}

void UWAICache::clear(bool beforeUpdate) {

	for(size_t i = 0; i < v.size(); i++)
		delete v[i];
	v.clear();
	cityMap.clear();

	nReachableCities.reset();
	targetMissionCounts.reset();
	threatRatings.reset();
	vassalTechScores.reset();
	vassalResourceScores.reset();
	adjacentLand.reset();
	relativeNavyPow.reset();
	wwAnger.reset();

	nReachableCities.set(ownerId, GET_PLAYER(ownerId).getNumCities());
	totalAssets = 0;
	goldPerProduction = 0;
	canScrub = false;
	trainDeepSeaCargo = trainAnyCargo = false;
	focusOnPeacefulVictory = false;

	lostTilesAtWar.reset(); // advc.035
	warUtilityIgnDistraction.reset();

	if(!beforeUpdate) {
		latestTurnReachableBySea.clear();
		nNonNavyUnits = 0;
		for(size_t i = 0; i < militaryPower.size(); i++)
			SAFE_DELETE(militaryPower[i]);
		militaryPower.clear();
		pastWarScores.reset();
		sponsorshipsAgainst.reset();
		sponsorsAgainst.reset();
		hireAgainst.reset();
	}
}

// Called when saving
void UWAICache::write(FDataStreamBase* stream) {

	PROFILE_FUNC();
	int savegameVersion = 1;
	savegameVersion = 2; // advc.035
	savegameVersion = 3; // hireAgainst added
	savegameVersion = 4; // granularity of pastWarScore increased
	/*  I hadn't thought of a version number in the initial release. Need
		to fold it into ownerId now to avoid breaking compatibility. */
	savegameVersion = 5; // focusOnPeacefulVictory added
	savegameVersion = 6; // advc.enum: Store as float
	stream->Write(ownerId + 100 * savegameVersion);
	int n = (int)v.size();
	stream->Write(n);
	for(int i = 0; i < n; i++)
		v[i]->write(stream);

	nReachableCities.Write(stream);
	targetMissionCounts.Write(stream);
	threatRatings.Write(stream, false, true);
	vassalTechScores.Write(stream);
	vassalResourceScores.Write(stream);
	adjacentLand.Write(stream);
	relativeNavyPow.Write(stream, false, true);
	wwAnger.Write(stream, false, true);
	lostTilesAtWar.Write(stream); // advc.035
	pastWarScores.Write(stream);
	sponsorshipsAgainst.Write(stream);
	sponsorsAgainst.Write(stream);
	warUtilityIgnDistraction.Write(stream);
	hireAgainst.Write(stream);

	stream->Write(bHasAggressiveTrait);
	stream->Write(bHasProtectiveTrait);
	stream->Write(canScrub);
	stream->Write(trainDeepSeaCargo);
	stream->Write(trainAnyCargo);
	stream->Write(focusOnPeacefulVictory);
	stream->Write(readyToCapitulate.size());
	for(std::set<TeamTypes>::const_iterator it = readyToCapitulate.begin();
			it != readyToCapitulate.end(); it++)
		stream->Write(*it);
	stream->Write(latestTurnReachableBySea.size());
	for(std::map<int,std::pair<int,int> >::iterator it = latestTurnReachableBySea.
			begin(); it != latestTurnReachableBySea.end(); it++) {
		stream->Write(it->first);
		stream->Write(it->second.first);
		stream->Write(it->second.second);
	}
	for(size_t i = 0; i < militaryPower.size(); i++)
		militaryPower[i]->write(stream);
	stream->Write(nNonNavyUnits);
	stream->Write(totalAssets);
	stream->Write(goldPerProduction);
}

// Called when loading
void UWAICache::read(FDataStreamBase* stream) {

	int tmp;
	stream->Read(&tmp);
	int savegameVersion = tmp / 100;
	ownerId = (PlayerTypes)(tmp % 100);
	// Important to set onwerId first b/c clear uses it
	clear();
	int n=0;
	stream->Read(&n);
	for(int i = 0; i < n; i++) {
		v.push_back(new City(ownerId));
		v[i]->read(stream);
		cityMap.insert(std::make_pair(v[i]->id(), v[i]));
	}
	/*  These used to be arrays with no value for the Barbarians.
		(The Barbarian value remains unused.) */
	nReachableCities.Read(stream);
	targetMissionCounts.Read(stream);
	threatRatings.ReadFloat(stream, savegameVersion < 6);
	vassalTechScores.Read(stream);
	vassalResourceScores.Read(stream);
	adjacentLand.Read(stream);
	if(savegameVersion >= 1)
		relativeNavyPow.ReadFloat(stream, savegameVersion < 6);
	wwAnger.ReadFloat(stream, savegameVersion < 6);
	// <advc.035>
	if(savegameVersion >= 2)
		lostTilesAtWar.Read(stream); // </advc.035>
	pastWarScores.Read(stream);
	if(savegameVersion < 4) {
		for (TeamIter<CIV_ALIVE> it; it.hasNext(); ++it)
			pastWarScores.multiply(it->getID(), 100);
	}
	sponsorshipsAgainst.Read(stream);
	sponsorsAgainst.Read(stream);
	warUtilityIgnDistraction.Read(stream);
	if(savegameVersion >= 3)
		hireAgainst.Read(stream);

	stream->Read(&bHasAggressiveTrait);
	stream->Read(&bHasProtectiveTrait);
	stream->Read(&canScrub);
	stream->Read(&trainDeepSeaCargo);
	stream->Read(&trainAnyCargo);
	if(savegameVersion >= 5)
		stream->Read(&focusOnPeacefulVictory);
	int sz=0;
	stream->Read(&sz);
	for(int i = 0; i < sz; i++) {
		int masterId;
		stream->Read(&masterId);
		FAssert(masterId >= 0 && masterId < MAX_CIV_TEAMS); // Sanity check
		readyToCapitulate.insert((TeamTypes)masterId);
	}
	stream->Read(&sz);
	for(int i = 0; i < sz; i++) {
		int key, firstVal, secondVal;
		stream->Read(&key);
		stream->Read(&firstVal);
		stream->Read(&secondVal);
		latestTurnReachableBySea[key] = std::make_pair(firstVal, secondVal);
	}
	militaryPower.push_back(new MilitaryBranch::HomeGuard(ownerId));
	militaryPower.push_back(new MilitaryBranch::Army(ownerId));
	militaryPower.push_back(new MilitaryBranch::Fleet(ownerId));
	militaryPower.push_back(new MilitaryBranch::Logistics(ownerId));
	militaryPower.push_back(new MilitaryBranch::Cavalry(ownerId));
	militaryPower.push_back(new MilitaryBranch::NuclearArsenal(ownerId));
	for(size_t i = 0; i < militaryPower.size(); i++)
		militaryPower[i]->read(stream);
	stream->Read(&nNonNavyUnits);
	stream->Read(&totalAssets);
	stream->Read(&goldPerProduction);
	if(savegameVersion < 1)
		updateRelativeNavyPower();
}

/*  Called each turn. Some data are also updated throughout the turn, e.g. through
	reportUnitCreated. */
void UWAICache::update() {

	PROFILE_FUNC();
	clear(true);
	focusOnPeacefulVictory = calculateFocusOnPeacefulVictory();
	// Needs to be done before updating cities
	updateTrainCargo();
	for(PlayerIter<MAJOR_CIV,KNOWN_TO> it(TEAMID(ownerId)); it.hasNext(); ++it)
		updateCities(it->getID());
	sortCitiesByAttackPriority();
	updateTotalAssetScore();
	/*  Disable latestTurnReachable fallback
		(The update is actually cheap, but I'm not sure about the very numerous
		getDistance calls.) */
	//updateLatestTurnReachableBySea();
	updateTargetMissionCounts();
	updateTypicalUnits();
	updateThreatRatings();
	updateVassalScores();
	updateAdjacentLand();
	updateLostTilesAtWar(); // advc.035
	updateRelativeNavyPower();
	updateGoldPerProduction();
	updateWarAnger();
	updateCanScrub();

	// Any values used by war evaluation need to be updated before this!
	if(GC.getGame().getElapsedGameTurns() > 0) { /* On turn 0, the
			other civs' caches aren't up to date, which can cause problems
			in scenarios. */
		updateWarUtility();
	}
}

void UWAICache::updateCities(PlayerTypes civId) {

	PROFILE_FUNC();
	CvPlayerAI& civ = GET_PLAYER(civId);
	FOR_EACH_CITY(c, civ) {
		// c.isRevealed() impedes the AI too much
		if(civId == ownerId || GET_TEAM(ownerId).AI_deduceCitySite(c)) {
			City* cacheCity = new City(ownerId, *c);
			v.push_back(cacheCity);
			cityMap.insert(std::make_pair(cacheCity->id(), cacheCity));
			if(civId != ownerId && cacheCity->canReach())
				nReachableCities.add(civId, 1);
		}
	}
}

void UWAICache::updateTotalAssetScore() {

	// For Palace; it's counted as a national wonder below, but it's worth another 5.
	totalAssets = 5;
	for(int i = size() - 1; i >= 0; i--) {
		City const& c = *getCity(i);
		if(!c.isOwnCity())
			break; // Sorted so that owner's cities are at the end
		CvCity* cp = c.city();
		if(cp != NULL && cp->getOwner() == ownerId)
			/*  National wonders aren't included in the per-city asset score b/c
				they shouldn't count for rival cities. */
			totalAssets += c.getAssetScore() + cp->getNumNationalWonders() * 4;
		/*  Cached cities have just been updated, so ownership info can't be
			out of date. */
		else FAssert(false);
	}
}

void UWAICache::updateGoldPerProduction() {

	goldPerProduction = std::max(goldPerProdBuildings(), goldPerProdSites());
	goldPerProduction *= GET_PLAYER(ownerId).uwai().amortizationMultiplier();
	goldPerProduction = std::max(goldPerProdVictory(), goldPerProduction);
}

double const UWAICache::goldPerProdUpperLimit = 4.5;
double UWAICache::goldPerProdBuildings() {

	PROFILE_FUNC();
	vector<double> buildingCounts; // excluding wonders
	vector<double> wonderCounts;
	CvPlayerAI const& owner = GET_PLAYER(ownerId);
	EraTypes ownerEra = owner.getCurrentEra();
	ReligionTypes ownerReligion = owner.getStateReligion();
	FOR_EACH_CITY(cp, owner) {
		CvCity const& c = *cp;
		int buildings = 0, wonders = 0;
		FOR_EACH_ENUM(Building) {
			CvBuildingInfo& b = GC.getInfo(eLoopBuilding);
			if(c.canConstruct(eLoopBuilding) && !b.isCapital() && // exclude Palace
					c.getProductionBuilding() != eLoopBuilding &&
					// Wonder in construction elsewhere:
					owner.getBuildingClassMaking(b.getBuildingClassType()) == 0) {
				if(b.getReligionType() != NO_RELIGION) {
					// No Monasteries when they're about to go obsolete
					TechTypes obsTech = b.getObsoleteTech();
					if(obsTech != NO_TECH && GC.getInfo(obsTech).getEra() <=
							ownerEra)
						continue;
					// If state religion, count only buildings of that religion.
					if(ownerReligion != NO_RELIGION && b.getReligionType() != ownerReligion)
						continue;
				}
				if(b.isLimited())
					wonders++;
				else buildings++;
			}
		}
		buildingCounts.push_back(buildings);
		wonderCounts.push_back(wonders);
	}
	if(buildingCounts.empty()) // No city founded yet
		return 2;
	// Cities about to be founded; will soon need buildings.
	for(int i = 0; i < ::round(0.6 * std::min(owner.AI_getNumAIUnits(UNITAI_SETTLE),
			owner.AI_getNumCitySites())); i++)
		buildingCounts.push_back(::dMax(buildingCounts));
	int era = owner.getCurrentEra();
	CvLeaderHeadInfo const& lh = GC.getInfo(owner.getPersonalityType());
	double missing = ::dMedian(buildingCounts) + std::max(0.0, ::dMedian(wonderCounts) -
			/*  Assume one useless (small or great) wonder per era, but two for
				Classical and none for Future. */
			(std::min(5, era) + (era > 0 ? 1 : 0))) *
			((lh.getWonderConstructRand() + 20) / 50); // wcr=30 is typical
	// Assume 6 buildings made available per era
	int maxBuildings = (era + 1) * 6;
	double missingRatio = missing / maxBuildings;
	missingRatio *= (100 - lh.getBuildUnitProb()) / 75.0; // bup=25 is typical
	missingRatio = std::min(missingRatio, 1.0);
	double r = std::max(1.0, goldPerProdUpperLimit * missingRatio);
	return r;
}
double UWAICache::goldPerProdSites() {

	PROFILE_FUNC();
	CvPlayerAI const& owner = GET_PLAYER(ownerId);
	std::vector<int> foundVals;
	for(int i = 0; i < owner.AI_getNumCitySites(); i++) {
		CvPlot* site = owner.AI_getCitySite(i);
		if(site != NULL)
			foundVals.push_back(site->getFoundValue(owner.getID()));
	}
	std::sort(foundVals.begin(), foundVals.end(), std::greater<int>());
	int const sitesMax = 5;
	double weights[sitesMax] = { 1, 0.8, 0.6, 0.4, 0.2 };
	double sites = 0;
	int const settlers = owner.AI_getNumAIUnits(UNITAI_SETTLE);
	for(int i = settlers; i < std::min(sitesMax, (int)foundVals.size()); i++) {
		/*  Once the settlers have been used, the found value of more remote sites
			may increase a bit (see shapeWeight in AI_foundValue_bulk) */
		sites += foundVals[i] * (weights[i] + settlers / 10.0) / 2500;
	}
	/*  Don't want to count faraway barb cities. Reference value set after
		looking at some sample targetCityValues; let's hope these generalize. */
	double const refVal = 50;
	FOR_EACH_CITY(c, GET_PLAYER(BARBARIAN_PLAYER)) {
		if(GET_TEAM(owner.getTeam()).AI_deduceCitySite(c)) {
			int const targetVal = owner.AI_targetCityValue(c, false, true);
			if (targetVal > 0)
				sites += std::pow(std::min((double)targetVal, refVal) / refVal, 3.0);
		}
	}
	CvGame& g = GC.getGame();
	int gameEra = g.getCurrentEra();
	/*  Rage makes it more worthwhile to focus on early expansion (at least for
		the AI), regardless of whether the additional cities are new or conquered
		from the barbs. */
	if(g.isOption(GAMEOPTION_RAGING_BARBARIANS) &&
			(gameEra < 2 || gameEra == g.getStartEra()))
		sites *= 1.25;
	double cities = std::max(1, owner.getNumCities());
	// Shouldn't expect to claim all sites with few cities
	sites = std::min(sites, cities);
	double r = std::min(goldPerProdUpperLimit, goldPerProdUpperLimit * sites / cities);
	return r;
}

double UWAICache::goldPerProdVictory() {

	CvTeamAI const& ourTeam = GET_TEAM(ownerId);
	int ourVictLevel = 0;
	if(ourTeam.AI_anyMemberAtVictoryStage(AI_VICTORY_CULTURE3 | AI_VICTORY_SPACE3))
		ourVictLevel = 3;
	else if(ourTeam.AI_anyMemberAtVictoryStage(AI_VICTORY_CULTURE4 | AI_VICTORY_SPACE4))
		ourVictLevel = 4;
	if(ourVictLevel < 3) {
		if(ourTeam.AI_anyMemberAtVictoryStage(AI_VICTORY_CULTURE2 | AI_VICTORY_SPACE2))
			return 1;
		return 0.5;
	}
	double r = ourVictLevel + 1;
	for (TeamIter<MAJOR_CIV,KNOWN_POTENTIAL_ENEMY_OF> it(TEAMID(ownerId)); it.hasNext(); ++it) {
		CvTeamAI const& rival = *it;
		int theirVictLevel = 0;
		if(rival.AI_anyMemberAtVictoryStage3())
			theirVictLevel = 3;
		else if(rival.AI_anyMemberAtVictoryStage4())
			theirVictLevel = 4;
		if(theirVictLevel >= ourVictLevel)
			r--;
		if(theirVictLevel > ourVictLevel)
			r--;
	}
	return r;
}

void UWAICache::updateWarUtility() {

	PROFILE_FUNC();
	if(!getUWAI.isUpdated())
		return;
	CvTeamAI const& ownerTeam = GET_TEAM(ownerId);
	/*  Not nice. War utility is computed per team, and the computation is
		somewhat costly. Want to do this only once per team, but don't have a
		class for AI values cached per team, and I don't want to create one
		just for this. Hence do it only for the leader. */
	if(ownerId != ownerTeam.getLeaderID())
		return;

	for(TeamIter<FREE_MAJOR_CIV> it; it.hasNext(); ++it) {
		TeamTypes targetId = it->getID();
		if(ownerTeam.uwai().isPotentialWarEnemy(targetId))
			updateWarUtilityIgnDistraction(targetId);
	}
}

void UWAICache::updateWarUtilityIgnDistraction(TeamTypes targetId) {

	CvTeamAI& agent = GET_TEAM(TEAMID(ownerId));
	if (agent.isAVassal()) { // Not needed for vassals
		warUtilityIgnDistraction.set(targetId, 0);
		return;
	}
	UWAIReport report(true); // silent
	// Ignoring Distraction cost
	WarEvalParameters params(agent.getID(), targetId, report, true);
	WarEvaluator eval(params);
	WarPlanTypes wp = agent.AI_getWarPlan(targetId);
	int prepTime = 0;
	// Just limited war and naval based on AI_isLandTarget is good enough here
	if(wp == NO_WARPLAN) {
		wp = WARPLAN_PREPARING_LIMITED;
		if(!agent.uwai().isPushover(targetId))
			prepTime = 5;
	}
	warUtilityIgnDistraction.set(targetId, eval.evaluate(
			wp, !agent.AI_isLandTarget(targetId), prepTime));
}

void UWAICache::updateWarAnger() {

	CvPlayerAI const& owner = GET_PLAYER(ownerId);
	if(owner.isAnarchy())
		return;
	double totalWWAnger = 0;
	FOR_EACH_CITY(cp, owner) {
		CvCity const& c = *cp;
		if(c.isDisorder())
			continue;
		/*  Disregard happiness from culture rate unless we need culture
			regardless of happiness */
		double angry = c.angryPopulation(0,
				!owner.AI_atVictoryStage(AI_VICTORY_CULTURE3) &&
				!owner.AI_atVictoryStage(AI_VICTORY_CULTURE4));
		totalWWAnger += std::min(angry, c.getWarWearinessPercentAnger()
				* c.getPopulation() / (double)GC.getPERCENT_ANGER_DIVISOR());
	}
	if(totalWWAnger < 0.01)
		return;
	// Who causes the wwAnger?
	double wwContribs[MAX_CIV_TEAMS];
	double totalWeight = 0;
	for(int i = 0; i < MAX_CIV_TEAMS; i++) {
		/*  Never mind all the modifiers in CvPlayer::updateWarWearinessPercentAnger;
			they apply equally to each contribution. */
		double contrib = GET_TEAM(ownerId).getWarWeariness((TeamTypes)i, true);
		wwContribs[i] = contrib;
		totalWeight += contrib;
	}
	for(PlayerIter<CIV_ALIVE> it; it.hasNext(); ++it) {
		PlayerTypes civId = it->getID();
		if(totalWeight == 0)
			wwAnger.set(civId, 0);
		else wwAnger.set(civId, (float)(
				totalWWAnger * (wwContribs[TEAMID(civId)] / totalWeight) /
				GET_TEAM(civId).getNumMembers())); // Turn per-team into per-civ
	}
}

void UWAICache::updateCanScrub() {

	FeatureTypes fallout = NO_FEATURE;
	FOR_EACH_ENUM(Feature) {
		if(GC.getInfo(eLoopFeature).getHealthPercent() <= -50) {
			fallout = eLoopFeature;
			break;
		}
	}
	if(fallout == NO_FEATURE) {
		FAssertMsg(false, "Fallout feature not found; should have -50 health");
		return;
	}
	FOR_EACH_ENUM(Build) {
		TechTypes featTech = GC.getInfo(eLoopBuild).getFeatureTech(fallout);
		if(featTech != NO_TECH && GET_TEAM(ownerId).isHasTech(featTech)) {
			canScrub = true;
			break;
		}
	}
}

void UWAICache::updateTrainCargo() {

	PROFILE_FUNC();
	CvPlayerAI const& owner = GET_PLAYER(ownerId);
	CvCivilization const& ownerCiv = owner.getCivilization();
	for(int i = 0; i < ownerCiv.getNumUnits(); i++) {
		UnitTypes ut = ownerCiv.unitAt(i);
		CvUnitInfo& info = GC.getInfo(ut);
		if(info.getUnitAIType(UNITAI_ASSAULT_SEA) && owner.canTrain(ut)) {
			trainAnyCargo = true;
			/*  A check for GC.getWATER_TERRAIN(false) would be better for
				future modding. However, counting impassable terrain types is
				used everywhere else as well. */
			if(owner.AI_unitImpassableCount(ut) == 0)
				trainDeepSeaCargo = true;
		}
	}
}

bool UWAICache::calculateFocusOnPeacefulVictory() {

	CvPlayerAI const& owner = GET_PLAYER(ownerId);
	if(owner.AI_atVictoryStage(AI_VICTORY_CULTURE4 | AI_VICTORY_SPACE4))
		return true;
	if(owner.AI_atVictoryStage4() || // (Diplo doesn't count as peaceful)
			(!owner.AI_atVictoryStage(AI_VICTORY_CULTURE3) &&
			!owner.AI_atVictoryStage(AI_VICTORY_SPACE3)))
		return false;
	bool const bHuman = owner.isHuman();
	// Space3 or Culture3 -- but is there a rival at stage 4?
	for(PlayerIter<MAJOR_CIV,KNOWN_POTENTIAL_ENEMY_OF> it(TEAMID(ownerId)); it.hasNext(); ++it) {
		CvPlayerAI const& rival = *it;
		if(!rival.AI_atVictoryStage(AI_VICTORY_CULTURE4) &&
				!rival.AI_atVictoryStage(AI_VICTORY_SPACE4))
			continue;
		// Could we possibly(!) stop them? Would we want to?
		if(!bHuman && owner.AI_getAttitude(rival.getID()) >= ATTITUDE_FRIENDLY)
			continue;
		CvTeamAI const& rivalTeam = GET_TEAM(rival.getTeam());
		if(rivalTeam.AI_getWarSuccessRating() < 0)
			return false;
		if(GET_TEAM(owner.getTeam()).getPower(true) * (bHuman ? 5 : 4) >
				GET_TEAM(rival.getTeam()).getPower(false) * 3)
			return false;
	}
	return true;
}

void UWAICache::setCanBeHiredAgainst(TeamTypes tId, bool b) {

	leaderCache().hireAgainst.set(tId, b);
}

void UWAICache::updateCanBeHiredAgainst(TeamTypes tId, int u, int thresh) {

	// Part of the update happens in UWAI::Team::doWar
	double pr = -1;
	if(!hireAgainst.get(tId))
		pr = std::min(0.58, (u - 1.5 * thresh) / 100.0);
	else pr = 1 - (thresh - u) / 100.0;
	hireAgainst.set(tId, ::bernoulliSuccess(pr, "advc.104 (can hire)"));
}

UWAICache const& UWAICache::leaderCache() const {

	if(ownerId == GET_TEAM(ownerId).getLeaderID())
		return *this;
	return GET_PLAYER(GET_TEAM(ownerId).getLeaderID()).uwai().getCache();
}
UWAICache& UWAICache::leaderCache() {

	if(ownerId == GET_TEAM(ownerId).getLeaderID())
		return *this;
	return GET_PLAYER(GET_TEAM(ownerId).getLeaderID()).uwai().getCache();
}

UWAICache::City* UWAICache::lookupCity(int plotIndex) const {

	std::map<int,City*>::const_iterator pos = cityMap.find(plotIndex);
	// Verify that the city still exists
	if(pos == cityMap.end() || pos->second->city() == NULL)
		return NULL;
	return pos->second;
}

UWAICache::City* UWAICache::lookupCity(CvCity const& cvCity) const {

	return lookupCity(cvCity.plotNum());
}

void UWAICache::sortCitiesByOwnerAndDistance() {

	FAssertMsg(false, "function no longer used");
	sort(v.begin(), v.end(), City::byOwnerAndDistance);
}

void UWAICache::sortCitiesByOwnerAndTargetValue() {

	FAssertMsg(false, "function no longer used");
	sort(v.begin(), v.end(), City::byOwnerAndTargetValue);
}

void UWAICache::sortCitiesByDistance() {

	FAssertMsg(false, "function no longer used");
	sort(v.begin(), v.end(), City::byDistance);
}

void UWAICache::sortCitiesByTargetValue() {

	FAssertMsg(false, "function no longer used");
	sort(v.begin(), v.end(), City::byTargetValue);
}

void UWAICache::sortCitiesByAttackPriority() {

	PROFILE_FUNC();
	//sort(v.begin(), v.end(), City::byAttackPriority);
	/*  Selection sort b/c I want to factor in city areas. An invading army tends
		to stay in one area until all cities there are conquered. */
	for(int i = 0; i < ((int)v.size()) - 1; i++) {
		CvCity* pPrevCity = NULL;
		if(i > 0)
			pPrevCity = v[i - 1]->city();
		CvArea const* pPrevArea = (pPrevCity == NULL ? NULL :& pPrevCity->getArea());
		double maxPriority = -100;
		int iMax = -1;
		for(size_t j = i; j < v.size(); j++) {
			CvCity const* pCity = v[j]->city();
			CvArea const* pArea = (pCity == NULL ? NULL : pCity->area());
			double priority = (pCity == NULL ? -100 : v[j]->attackPriority());
			if(i > 0 && pPrevArea != NULL && pPrevArea != pArea && priority > 0)
				priority /= 2;
			if(priority > maxPriority) {
				iMax = j;
				maxPriority = priority;
			}
		}
		if(iMax >= 0) {
			City* tmp = v[i];
			v[i] = v[iMax];
			v[iMax] = tmp;
		}
	}
}

int UWAICache::size() const {

	return (int)v.size();
}

void UWAICache::updateLatestTurnReachableBySea() {

	PROFILE_FUNC();
	/*  LatestTurnReachable is a fallback for City::reachable and City::distance.
		Not really necessary anymore b/c City::measureDistance is more reliable
		now than it used to be, but still nice to have for dealing with temporary
		changes in city population or yield rate. (measureDistance may skip small
		and low-production cities.) */
	for(size_t i = 0; i < v.size(); i++) {
		City const& c = *v[i];
		// No pair means not reachable (by sea)
		if(!c.canCurrentlyReachBySea())
			continue;
		latestTurnReachableBySea[c.id()] = std::make_pair(GC.getGame().
				getGameTurn(), c.getDistance());
	}
}

void UWAICache::updateTraits() {

	bHasAggressiveTrait = false;
	bHasProtectiveTrait = false;
	FOR_EACH_ENUM(Trait) {
		if(!GET_PLAYER(ownerId).hasTrait(eLoopTrait) ||
				!GC.getInfo(eLoopTrait).isAnyFreePromotion())
			continue;
		FOR_EACH_ENUM(Promotion) {
			if (!GC.getInfo(eLoopTrait).isFreePromotion(eLoopPromotion))
				continue;
			if(GC.getInfo(eLoopPromotion).getCombatPercent() > 0)
				bHasAggressiveTrait = true;
			else if(GC.getInfo(eLoopPromotion).getCityDefensePercent() > 0)
				bHasProtectiveTrait = true;
		}
	}
}

void UWAICache::updateTargetMissionCounts() {

	for(PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
		updateTargetMissionCount(it->getID());
}

void UWAICache::updateThreatRatings() {

	for(PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
		threatRatings.set(it->getID(), (float)calculateThreatRating(it->getID()));
}

void UWAICache::updateVassalScores() {

	for(PlayerIter<MAJOR_CIV,KNOWN_TO> it(TEAMID(ownerId)); it.hasNext(); ++it)
		updateVassalScore(it->getID());
}

void UWAICache::updateAdjacentLand() {

	CvMap& m = GC.getMap();
	for(int i = 0; i < m.numPlots(); i++) {
		CvPlot const& p = m.getPlotByIndex(i);
		if(p.isWater())
			continue;
		PlayerTypes o = p.getOwner();
		if(o == NO_PLAYER || !GET_PLAYER(o).isAlive() || o == BARBARIAN_PLAYER ||
				TEAMID(o) == TEAMID(ownerId) || GET_PLAYER(o).isMinorCiv())
			continue;
		if(p.isAdjacentPlayer(ownerId, true))
			adjacentLand.add(o, 1);
	}
}
// <advc.035>
void UWAICache::updateLostTilesAtWar() {

	//PROFILE_FUNC();
	if(!GC.getDefineBOOL(CvGlobals::OWN_EXCLUSIVE_RADIUS))
		return;
	CvTeam const& ownerTeam = GET_TEAM(ownerId);
	for (TeamIter<MAJOR_CIV> it; it.hasNext(); ++it) {
		TeamTypes tId = it->getID();
		std::vector<CvPlot*> flipped;
		::contestedPlots(flipped, ownerTeam.getID(), tId);
		int lost = 0;
		for(size_t j = 0; j < flipped.size(); j++) {
			TeamTypes plotTeamId = flipped[j]->getTeam(); // current tile owner
			// Count the tiles that ownerTeam loses when at war with tId
			if(plotTeamId == (ownerTeam.isAtWar(tId) ? tId : ownerTeam.getID()))
				lost++;
		}
		lostTilesAtWar.set(tId, lost);
	}
} // </advc.035>

void UWAICache::updateRelativeNavyPower() {

	/*PROFILE_FUNC();
	for(PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it) {
		PlayerTypes civId = it->getID();*/
		/*  Tbd.:
			Exact result: (their navy power) /
						  (their total power from navy, army, home guard)

			Intelligence ratio (100%: assume we know all their positions;
			0: we know nothing, in particular if
			!GET_TEAM(civId).isHasMet(TEAMID(ownerId))).

			-100%
			+100% * #(their cities visible to us) / #(their cities)
			+100% * #(their cities revealed to us) / #(their cities)
				+25% if OB, otherwise
				+max{0, -25% + 50% * #(our spies)/ #(cities revealed to us)}
			+10% * (our era) + 5% * (war plan age)
				+range(closeness%, 0, 100%) if same capital area, otherwise
				+#(their coastal cities revealed to us) / #(their cities) *
						(50 + 10 * #(our sea patrols) + #(our explorers))

			Result guessed based on revealed info on map:
			(0.7 * #(their revealed coastal cities) / #(their revealed cities))
			/ (2 + #(rivals on their continent)
			Watch out for div by 0.
			Return average weighted by int. ratio of exact and guessed result.
			Apply result after copying of the exact values in InvasionGraph, and
			in WarUtilityAspect::LossesFromBlockade.
			Int. ratio might have further uses in the future (would have to
			store it separately then though). */
	//}
	// CvTeamAI::AI_getRivalAirPower also cheats with per-branch power values
}

/* Copied and adapted from CvPlayerAI::AI_enemyTargetMissions.
   Needed target missions per civ, not per team. */
void UWAICache::updateTargetMissionCount(PlayerTypes civId) {

	int r = 0;
	CvPlayerAI& owner = GET_PLAYER(ownerId);
	FOR_EACH_GROUPAI(selGroup, owner) {
		if(selGroup->getNumUnits() <= 0) // Can be empty
			continue;
		CvPlot* missionPlot = selGroup->AI_getMissionAIPlot();
		/* Should work for human civs too. They don't (typically?)
		   have missions, so we only count actual "boots on the ground". */
		if(missionPlot == NULL)
			missionPlot = selGroup->plot();
		FAssert(missionPlot != NULL);
		if(missionPlot->isOwned() && missionPlot->getOwner() == civId) {
			r += selGroup->getNumUnits();
			r += selGroup->getCargo();
		}
	}
	targetMissionCounts.set(civId, r);
}

double UWAICache::calculateThreatRating(PlayerTypes civId) const {

	// Can't reliably estimate yield rates in the early game
	if(GC.getGame().getCurrentEra() <= GC.getGame().getStartEra())
		return 0;
	CvCity* cc = GET_PLAYER(ownerId).getCapitalCity();
	City* c = NULL;
	if(cc != NULL)
		c = GET_PLAYER(civId).uwai().getCache().lookupCity(*cc);
	if(c != NULL && !c->canReach())
		return 0;
	double r = 0;
	r += teamThreat(TEAMID(civId));
	return r;
}

double UWAICache::teamThreat(TeamTypes tId) const {

	CvTeamAI const& t = GET_TEAM(tId);
	TeamTypes ownerTeam = TEAMID(ownerId);
	AttitudeTypes towardsOwner = (t.isHuman() ? ATTITUDE_CAUTIOUS : t.AI_getAttitude(ownerTeam));
	if(t.isAVassal() || towardsOwner >= ATTITUDE_FRIENDLY ||
			// Don't worry about long-term threat if they're already close to victory
			t.AI_anyMemberAtVictoryStage3())
		return 0;
	double theirPow = longTermPower(tId);
	double ourPow = longTermPower(GET_TEAM(ownerTeam).getMasterTeam(), true);
	if(t.isHuman())
		ourPow *= GET_PLAYER(ownerId).uwai().confidenceAgainstHuman();
	double powFactor = ::dRange(theirPow / ourPow - 0.75, 0.0, 1.0);
	/*  If presently at war, attitude is likely to improve in the medium-term.
		That said, the present war suggests a clash of interests that may persist
		in the long run. Assume that these factors cancel out, and don't adjust
		attitude. */
	double diploFactor = (ATTITUDE_FRIENDLY - towardsOwner) * 0.25;
	FAssert(diploFactor > 0);
	if(t.AI_anyMemberAtVictoryStage(AI_VICTORY_CONQUEST2))
		diploFactor += 0.15;
	else if(t.AI_anyMemberAtVictoryStage(AI_VICTORY_DIPLOMACY2))
		diploFactor += 0.1;
	// Nuclear deterrent
	if(GET_PLAYER(ownerId).getNumNukeUnits() > 0)
		diploFactor -= 0.2;
	diploFactor = ::dRange(diploFactor, 0.0, 1.0);
	// Less likely to attack us if there are many targets to choose from
	double altTargetsDivisor = TeamIter<FREE_MAJOR_CIV>::count();
	return diploFactor * powFactor / std::max(0.35, altTargetsDivisor / 5);
}

double UWAICache::longTermPower(TeamTypes tId, bool defensive) const {

	double r = 0.001;
	for(PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it) {
		CvPlayerAI const& civ = *it;
		TeamTypes mId = civ.getMasterTeam();
		if(mId != tId && (!defensive || !GET_TEAM(mId).isDefensivePact(tId)))
			continue;
		UWAI::Civ const& uwai = civ.uwai();
		MilitaryBranch& army = *uwai.getCache().getPowerValues()[ARMY];
		int typicalUnitProd = army.getTypicalUnitCost();
		if(typicalUnitProd <= 0)
			continue;
		/*  Long-term power mostly depends on production capacity and willingness
			to produce units. That said, 50~100 turns are also enough to
			translate food yield into additional production and commerce
			into better units. */
		r += (civ.estimateYieldRate(YIELD_PRODUCTION) +
				0.35 * civ.estimateYieldRate(YIELD_FOOD) +
				0.25 * civ.estimateYieldRate(YIELD_COMMERCE)) *
				(uwai.buildUnitProb() + 0.15) * army.getTypicalUnitPower() /
				(double)typicalUnitProd;
	}
	return r;
}

void UWAICache::updateVassalScore(PlayerTypes civId) {

	int techScore = 0;
	FOR_EACH_ENUM(Tech) {
		if(GET_PLAYER(civId).canTradeItem(ownerId, TradeData(
				TRADE_TECHNOLOGIES, eLoopTech), false))
			techScore += GET_TEAM(ownerId).AI_techTradeVal(eLoopTech, TEAMID(civId), true);
	}
	vassalTechScores.set(civId, techScore);
	int nMasterResources = 0;
	int nTribute = 0;
	FOR_EACH_ENUM(Bonus) {
		if(GET_TEAM(ownerId).isBonusObsolete(eLoopBonus))
			continue;
		TechTypes revealTech = (TechTypes)GC.getInfo(eLoopBonus).getTechReveal();
		bool availableToMaster = false;
		if(GET_TEAM(ownerId).isHasTech(revealTech)) {
			if(GET_PLAYER(ownerId).getNumAvailableBonuses(eLoopBonus) > 0) {
				nMasterResources++;
				availableToMaster = true;
			}
		}
		/*  Don't mind if we can't use it yet (TechCityTrade), but we can't know
			that they have it if we can't see it. If we can see it, but they can't,
			we might know, or not (if the plot is unrevealed). */
		if(!GET_TEAM(ownerId).isHasTech(revealTech)
				|| !GET_TEAM(civId).isHasTech(revealTech))
			continue;
		if(GET_PLAYER(civId).getNumAvailableBonuses(eLoopBonus) && !availableToMaster)
			nTribute++;
	}
	vassalResourceScores.set(civId, ::round((25.0 * nTribute) /
			std::max(4, nMasterResources)));
}

void UWAICache::reportUnitCreated(CvUnitInfo const& u) {

	updateMilitaryPower(u, true);
}

void UWAICache::reportUnitDestroyed(CvUnitInfo const& u) {

	updateMilitaryPower(u, false);
}

void UWAICache::reportWarEnding(TeamTypes enemyId,
		CLinkList<TradeData> const* weReceive, CLinkList<TradeData> const* wePay) {

	// Forget sponsorship once a war ends
	sponsorshipsAgainst.set(enemyId, 0);
	sponsorsAgainst.set(enemyId, NO_PLAYER);
	// Evaluate reparations
	bool bForceSuccess = false;
	bool bForceFailure = false;
	bool bForceNoFailure = false;
	bool bForceNoSuccess = false;
	CLLNode<TradeData> const* node = NULL;
	TradeableItems ti;
	if(weReceive != NULL) {
		int iTechs = 0;
		int iCities = 0;
		// Ignore gold for simplicity (although a large sum could of course be relevant)
		for(node = weReceive->head(); node != NULL; node = weReceive->next(node)) {
			ti = node->m_data.m_eItemType;
			if(ti == TRADE_TECHNOLOGIES)
				iTechs++;
			else if(ti == TRADE_CITIES)
				iCities++;
		}
		if(iTechs + iCities > 0)
			bForceNoFailure = true;
		if(iTechs >= 2 || iCities > 0)
			bForceSuccess = true;
	} else if(wePay != NULL) {
		int iTechs = 0;
		int iCities = 0;
		for(node = wePay->head(); node != NULL; node = wePay->next(node)) {
			ti = node->m_data.m_eItemType;
			if(ti == TRADE_TECHNOLOGIES)
				iTechs++;
			else if(ti == TRADE_CITIES)
				iCities++;
		}
		if(iTechs + iCities > 0)
			bForceNoSuccess = true;
		if(iTechs >= 2 || iCities > 0)
			bForceFailure = true;
	}
	// Evaluate war success
	int iOurSuccess = GET_TEAM(ownerId).AI_getWarSuccess(enemyId);
	int iTheirSuccess = GET_TEAM(enemyId).AI_getWarSuccess(TEAMID(ownerId));
	if(iOurSuccess + iTheirSuccess < GC.getWAR_SUCCESS_CITY_CAPTURING() &&
			!bForceFailure && !bForceSuccess)
		return;
	// Use our era as the baseline for what is significant war success
	int iOurTechEra = GET_PLAYER(ownerId).getCurrentEra();
	double successRatio = iOurSuccess / (double)std::max(1, iTheirSuccess);
	double successThresh = GC.getWAR_SUCCESS_CITY_CAPTURING() * iOurTechEra * 0.7;
	if(	  (successRatio > 1 && iOurSuccess < successThresh) ||
		  (successRatio < 1 && iTheirSuccess < successThresh))
		successRatio = 1;
	// Be less critical about our performance if we fought a human
	if(GET_TEAM(enemyId).isHuman())
		successRatio *= 1.33;
	if(GET_PLAYER(ownerId).isHuman())
		successRatio /= 1.33;
	bool bChosenWar = GET_TEAM(ownerId).AI_isChosenWar(enemyId);
	int iDuration = GET_TEAM(ownerId).AI_getAtWarCounter(enemyId);
	double durationFactor = 0.365 * std::sqrt((double)std::max(1, iDuration));
	/*  Don't be easily emboldened by winning a defensive war. Past war score is
		intended to discourage war more than encourage it. */
	if((((successRatio > 1.3 && bChosenWar) || successRatio > 1.5) &&
			!bForceNoSuccess) || bForceSuccess)
		pastWarScores.add(enemyId, ::round(100 / durationFactor));
	// Equal war success not good enough if we started it
	else if(((bChosenWar || successRatio < 0.7) &&
			!bForceNoFailure) || bForceFailure)
		pastWarScores.add(enemyId, -::round(100 * durationFactor));
}

void UWAICache::reportCityOwnerChanged(CvCity* c, PlayerTypes oldOwnerId) {

	if(!GET_TEAM(ownerId).AI_deduceCitySite(c) || c->getOwner() == BARBARIAN_PLAYER)
		return;
	/*  I didn't think I'd need to update the city cache during turns, so this
		is awkward to write ...
		Necessary though b/c the AI needs to stay up to date with human conquests. */
	size_t vIndex = -1;
	if(GET_PLAYER(oldOwnerId).isMajorCiv()) {
		City* fromCache = NULL;
		std::map<int,City*>::iterator pos = cityMap.find(c->plotNum());
		if(pos != cityMap.end())
			fromCache = pos->second;
		if(fromCache != NULL) {
			if(fromCache->canReach())
				nReachableCities.add(oldOwnerId, -1);
			cityMap.erase(pos);
			for(vIndex = 0; vIndex < v.size(); vIndex++) {
				if(v[vIndex]->id() == fromCache->id()) {
					delete v[vIndex];
					v[vIndex] = NULL;
					break;
				}
			}
		}
	}
	/*	The insertion can't be conditional. If v[vIndex] has been deleted above,
		then some city has to be inserted there. Allowing NULL entries would make
		matters slower and more complicated elsewhere. */
	//if(GET_PLAYER(c->getOwner()).isMajorCiv())
	City* toCache = new City(ownerId, *c);
	if(vIndex < 0 || vIndex >= v.size()) {
		v.push_back(toCache);
		// (c could also have become revealed through map trade)
		//FAssertMsg(oldOwnerId == BARBARIAN_PLAYER);
	}
	else v[vIndex] = toCache;
	cityMap.insert(std::make_pair(toCache->id(), toCache));
	if(toCache->canReach())
		nReachableCities.add(c->getOwner(), 1);

	sortCitiesByAttackPriority();
}

void UWAICache::reportSponsoredWar(CLinkList<TradeData> const& sponsorship,
		PlayerTypes sponsorId, TeamTypes targetId) {

	if(targetId == NO_TEAM || sponsorId == NO_PLAYER) {
		FAssert(false);
		return;
	}
	CvPlayerAI const& owner = GET_PLAYER(ownerId);
	sponsorshipsAgainst.set(targetId, ::round(owner.uwai().
			/*  Need to remember the utility. The deal value may not seem much
				10 turns from now if our economy grows.
				Should perhaps cap dealVal at AI_declareWarTradeVal. As it is now,
				paying the AI more than it demands makes it a bit more reluctant
				to end the war. */
			tradeValToUtility(owner.AI_dealVal(sponsorId, sponsorship))));
	if(sponsorshipsAgainst.get(targetId) > 0)
		sponsorsAgainst.set(targetId, sponsorId);
	else {
		sponsorshipsAgainst.set(targetId, 0);
		sponsorsAgainst.set(targetId, NO_PLAYER);
	}
}

bool UWAICache::isReadyToCapitulate(TeamTypes masterId) const {

	FAssert(GET_TEAM(masterId).isHuman());
	if(GET_TEAM(ownerId).getLeaderID() == ownerId)
		return readyToCapitulate.count(masterId) > 0;
	/*  Not nice; if I add a few more team-related items, I should really put
		them in a separate class. */
	return leaderCache().isReadyToCapitulate(masterId);
}

void UWAICache::setReadyToCapitulate(TeamTypes masterId, bool b) {

	FAssert(GET_TEAM(masterId).isHuman());
	if(b == isReadyToCapitulate(masterId))
		return;
	if(GET_TEAM(ownerId).getLeaderID() == ownerId) {
		if(b)
			readyToCapitulate.insert(masterId);
		else readyToCapitulate.erase(masterId);
	}
	else GET_PLAYER(GET_TEAM(ownerId).getLeaderID()).uwai().getCache().
			setReadyToCapitulate(masterId, b);
}

void UWAICache::addTeam(PlayerTypes otherLeaderId) {

	// Get the team-related data from the other team's leader
	UWAICache& other = GET_PLAYER(otherLeaderId).uwai().getCache();
	// Fairly unimportant data
	for(TeamIter<MAJOR_CIV> it; it.hasNext(); ++it) {
		TeamTypes tId = it->getID();
		pastWarScores.add(tId, other.pastWarScores.get(tId));
		if(other.sponsorsAgainst.get(tId) != NO_PLAYER) {
			if(sponsorsAgainst.get(tId) == NO_PLAYER ||
					sponsorshipsAgainst.get(tId) < other.sponsorshipsAgainst.get(tId)) {
				sponsorsAgainst.set(tId, other.sponsorsAgainst.get(tId));
				sponsorshipsAgainst.set(tId, other.sponsorshipsAgainst.get(tId));
			}
		}
	}
}

void UWAICache::onTeamLeaderChanged(PlayerTypes formerLeaderId) {

	if(formerLeaderId == NO_PLAYER)
		return;
	PlayerTypes leaderId =  GET_TEAM(ownerId).getLeaderID();
	if(leaderId == NO_PLAYER || formerLeaderId == leaderId)
		return;
	for(TeamIter<MAJOR_CIV> it; it.hasNext(); ++it) {
		TeamTypes tId = it->getID();
		if(GET_TEAM(tId).isHuman()) {
			GET_PLAYER(GET_TEAM(ownerId).getLeaderID()).uwai().getCache().
					setReadyToCapitulate(tId, GET_PLAYER(formerLeaderId).
					uwai().getCache().readyToCapitulate.count(tId) > 0);
		}
		GET_PLAYER(GET_TEAM(ownerId).getLeaderID()).uwai().getCache().
				warUtilityIgnDistraction.set(tId, GET_PLAYER(formerLeaderId).
				uwai().getCache().warUtilityIgnDistraction.get(tId));
	}
}

void UWAICache::updateTypicalUnits() {

	for(size_t i = 0; i < militaryPower.size(); i++)
		militaryPower[i]->updateTypicalUnit();
}

void UWAICache::updateMilitaryPower(CvUnitInfo const& u, bool add) {

	/*  i=1: skip HOME_GUARD. Potential guard units are counted as Army and
		split later through the HomeGuard constructor. */
	for(size_t i = 1; i < militaryPower.size(); i++)
		militaryPower[i]->updatePower(u, add);
	if(u.getDomainType() != DOMAIN_SEA && u.isMilitaryProduction() &&
			// Exclude Recon
			u.getDefaultUnitAIType() != UNITAI_EXPLORE)
		nNonNavyUnits += (add ? 1 : -1);
}

UWAICache::City::City(PlayerTypes cacheOwnerId, CvCity const& c)
		: cacheOwnerId(cacheOwnerId) {

	canDeduce = GET_TEAM(cacheOwnerId).AI_deduceCitySite(&c);
	// Use plot index as city id (the pointer 'c' isn't serializable)
	plotIndex = c.plotNum();
	updateDistance(c);
	// AI_targetCityValue doesn't account for reachability (probably should)
	if(!canReach() || cacheOwnerId == c.getOwner())
		targetValue = -1;
	else targetValue = GET_PLAYER(cacheOwnerId).AI_targetCityValue(
			city(), false, true);
	updateAssetScore();
}

UWAICache::City::City(PlayerTypes cacheOwnerId)
		: cacheOwnerId(cacheOwnerId) {

	plotIndex = assetScore = distance = targetValue = -1;
	reachByLand = reachBySea = canDeduce = false;
}

CvCity* UWAICache::City::city() const {

	CvPlot* cityPlot = GC.getMap().plotByIndex(plotIndex);
	if(cityPlot == NULL)
		return NULL;
	return cityPlot->getPlotCity();
}

int UWAICache::City::id() const {

	return plotIndex;
}

int UWAICache::City::getAssetScore() const {

	return assetScore;
}

bool UWAICache::City::canReach() const {

	CvCity* const cp = city();
	if(cp == NULL ||
			// A bit slow:
			// !GET_TEAM(cacheOwnerId).AI_deduceCitySite(city())
			/*  Check isRevealed first b/c I'm only updating canDeduce
				once per turn */
			(!cp->isRevealed(TEAMID(cacheOwnerId)) &&
			!canDeduce))
		return false;
	if(distance >= 0)
		return true;
	return false; // Disable latestTurnReachable fallback

	// Should perhaps pass outer object in constructor instead
	/*std::map<int,std::pair<int,int> > const& ltr = GET_PLAYER(cacheOwnerId).
			uwai().getCache().latestTurnReachableBySea;
	std::map<int,std::pair<int,int> >::const_iterator pos = ltr.find(plotIndex);
	if(pos == ltr.end())
		return false;
	int turnsUnreachable = GC.getGame().gameTurn() - pos->second.first;
	return (turnsUnreachable < 10);*/
	/*  If this isn't enough time for naval stacks to reach their target,
		consider adding a stackEnRoute flag to City that is updated in
		updateTargetMissionCount. Not sure if AI_getMissionAIPlot points directly
		to cities though. Could check adjacent tiles as well I guess. */
}

int UWAICache::City::getDistance(bool forceCurrentVal) const {

	/*if(distance < 0 && !forceCurrentVal && !reachByLand) { // latestTurnReachable is only for reachability via sea
		std::map<int,std::pair<int,int> > const& ltr = GET_PLAYER(cacheOwnerId).
			uwai().getCache().latestTurnReachableBySea;
		std::map<int,std::pair<int,int> >::const_iterator pos = ltr.find(plotIndex);
		if(pos != ltr.end())
			return pos->second.second;
	}*/  // Disable latestTurnReachable fallback
	return distance;
}

int UWAICache::City::getTargetValue() const {

	return targetValue;
}

bool UWAICache::City::canReachByLand() const {

	return reachByLand;
}

bool UWAICache::City::canCurrentlyReachBySea() const {

	return reachBySea;
}

void UWAICache::City::write(FDataStreamBase* stream) {

	int savegameVersion = 1;
	stream->Write(plotIndex);
	stream->Write(assetScore);
	/*  I hadn't thought of a version number in the initial release.
		Fold it into 'distance' to avoid breaking compatibility. */
	FAssertMsg(distance >= -1 && distance < 10000, "-2 is OK if loaded from an "
			"old savegame version");
	distance = ::range(distance, -1, 9999);
	// Add 1 b/c distance can be -1
	stream->Write(distance + 1 + 10000 * savegameVersion);
	stream->Write(targetValue);
	stream->Write(reachByLand);
	stream->Write(reachBySea);
	stream->Write(canDeduce);
}

void UWAICache::City::read(FDataStreamBase* stream) {

	stream->Read(&plotIndex);
	stream->Read(&assetScore);
	int tmp;
	stream->Read(&tmp);
	int savegameVersion = tmp / 10000;
	distance = (tmp % 10000) - 1;
	stream->Read(&targetValue);
	stream->Read(&reachByLand);
	stream->Read(&reachBySea);
	if(savegameVersion >= 1)
		stream->Read(&canDeduce);
	else {
		/*  Can't call GET_TEAM(cacheOwnerId).AI_deduceCitySite(city()) here b/c
			City::city() calls CvPlot::getPlotCity, which requires the city owner
			(CvPlayer object) to be initialized. At this point, only the civs
			up to cacheOwnerId are initialized.
			canDeduce gets set properly on the next UWAICache::update. */
		CvPlot* cityPlot = GC.getMap().plotByIndex(plotIndex);
		if(cityPlot == NULL)
			canDeduce = false;
		else canDeduce = cityPlot->isRevealed(TEAMID(cacheOwnerId));
	}
}

CvCity* UWAICache::City::cityById(int id) {

	CvPlot* p = GC.getMap().plotByIndex(id);
	if(p == NULL)
		return NULL;
	return p->getPlotCity();
}

bool UWAICache::City::byOwnerAndDistance(City* one, City* two) {

	int cmp = byOwner(one, two);
	if(cmp < 0)
		return true;
	if(cmp > 0)
		return false;
	if(!two->canReach())
		return true;
	if(!one->canReach())
		return false;
	int dist1 = one->getDistance(),
		dist2 = two->getDistance();
	return dist1 < dist2;
}

bool UWAICache::City::byDistance(City* one, City* two) {

	int dist1 = one->getDistance(),
		dist2 = two->getDistance();
	if(dist1 < 0 && dist2 >= 0)
		return false;
	if(dist2 < 0 && dist1 >= 0)
		return true;
	return dist1 < dist2;
}

bool UWAICache::City::byOwnerAndTargetValue(City* one, City* two) {

	int cmp = byOwner(one, two);
	if(cmp < 0)
		return true;
	if(cmp > 0)
		return false;
	int v1 = one->getTargetValue(),
		v2 = two->getTargetValue();
	return v1 > v2;
}

bool UWAICache::City::byTargetValue(City* one, City* two) {

	int v1 = one->getTargetValue(),
		v2 = two->getTargetValue();
	return v1 > v2;
}

bool UWAICache::City::byAttackPriority(City* one, City* two) {

	double v1 = one->attackPriority(),
		   v2 = two->attackPriority();
	return v1 > v2;
}
double UWAICache::City::attackPriority() const {

	if(distance < 0)
		return -1;
	if(isOwnCity())
		return -2; // updateTotalAssetScore relies on own cities having minimal priority
	/*  targetValue is something like 10 to 100, distance 1 to 20 perhaps.
		Add 1000 b/c negative values should be reserved for error conditions. */
	return std::max(0.0, 1000 + getTargetValue() -
			std::min(100.0, 1.5 * std::pow((double)distance, 1.7)));
}

int UWAICache::City::byOwner(City* one, City* two) {

	PlayerTypes owner1 = one->cityOwner(),
				owner2 = two->cityOwner();
	if(owner1 < owner2)
		return -1;
	if(owner2 < owner1)
		return 1;
	return 0;
}

PlayerTypes UWAICache::City::cityOwner() const {

	if(city() == NULL)
		return NO_PLAYER;
	return city()->getOwner();
}

bool UWAICache::City::isOwnCity() const {

	// Check distance==0 first b/c that's faster
	return (distance == 0 && cityOwner() == cacheOwnerId);
}

void UWAICache::City::updateDistance(CvCity const& targetCity) {

	PROFILE_FUNC();
	/*  For each city of the agent (cacheOwner), compute a path to the target city
		assuming that war is declared. Derive from that length an estimated travel
		duration based on the typical speed of units and time for loading and
		unloading (seaPenalty).
		Set 'distance' to a weighted average of the pairwise travel durations.
		The average gives the nearest cities of the agent the greatest weight.
		Some cities of the agent are also skipped, both for performance reasons
		(the pathfinding is computationally expensive) and b/c I want distance to
		reflect typical deployment distances, and insignificant cities don't
		deploy units. Can fall back on latestTurnReachableBySea when an important
		coastal city becomes unproductive b/c of unrest or e.g. poisoned water;
		don't need to deal with these things here.

		NB: targetCity and this City refer to the same city (but targetCity has
		type CvCity*).

		Landlocked civs (0 coastal cities) are treated as unreachable by sea.
		It's not that hard to compute mixed paths to such civs, but CvUnitAI
		isn't capable of carrying out naval assaults on non-coastal cities.

		Unreachable targets are indicated by a distance of -1. */

	CvPlayerAI& cacheOwner = GET_PLAYER(cacheOwnerId);

	// Our own cities have 0 distance from themselves
	if(targetCity.getOwner() == cacheOwnerId) {
		distance = 0;
		reachByLand = true;
		reachBySea = true;
		return;
	}
	int const maxDist = getUWAI.maxSeaDist();
	distance = -1;
	reachByLand = false;
	reachBySea = false;
	bool human = cacheOwner.isHuman();
	EraTypes const era = cacheOwner.getCurrentEra();
	// Assume that humans can always locate cities
	if(!human && !cacheOwner.AI_deduceCitySite(&targetCity))
		return;
	bool trainDeepSeaCargo = cacheOwner.uwai().getCache().
			canTrainDeepSeaCargo();
	bool trainAnyCargo = cacheOwner.uwai().getCache().
			canTrainAnyCargo();
	int const seaPenalty = (human ? 2 : 4);
	vector<int> pairwDurations;
	/*  If we find no land path and no sea path from a city c to the target,
		but at least one other city that does have a path to the target, then there
		is most likely also some mixed path from c to the target. */
	double mixedPath = 0;
	CvCity* capital = cacheOwner.getCapitalCity();
	FOR_EACH_CITY(c, cacheOwner) {
		// Skip small and isolated cities
		if(!c->isCapital() && (c->getArea().getCitiesPerPlayer(cacheOwnerId) <= 1 ||
				c->getPopulation() < capital->getPopulation() / 3 ||
				c->getYieldRate(YIELD_PRODUCTION) < 5 + era))
			continue;
		CvPlot* p = c->plot();
		int pwd = -1; // pairwise (travel) duration
		int d = -1; // set by measureDistance
		if(measureDistance(cacheOwnerId, DOMAIN_LAND, *p, *targetCity.plot(), &d)) {
			double speed = estimateMovementSpeed(cacheOwnerId, DOMAIN_LAND, d);
			// Will practically always have to move through some foreign tiles
			d = std::min(d, 2) + ::round((d - std::min(d, 2)) / speed);
			if(d == 0) // Make sure 0 is reserved for own cities
				d = 1;
			pwd = d;
			/*  reachByLand refers to our (AI) capital. This is to ensure that the
				AI can still detect the need for a naval assault when it has a
				colony near the target civ. */
			if(c->at(cacheOwner.getCapitalCity()->plot()))
				reachByLand = true;
		}
		if(trainAnyCargo) {
			DomainTypes dom = DOMAIN_SEA;
			if(!trainDeepSeaCargo)
				dom = DOMAIN_IMMOBILE; // Encode non-ocean as IMMOBILE
			if(measureDistance(cacheOwnerId, dom, *p, *targetCity.plot(), &d)) {
				FAssert(d >= 0);
				d = (int)std::ceil(d / estimateMovementSpeed(cacheOwnerId, DOMAIN_SEA, d)) +
						seaPenalty;
				if(pwd < 0 || d < pwd) {
					pwd = d;
					reachBySea = true;
				}
			}
		}
		if(pwd >= 0) {
			pairwDurations.push_back(pwd);
			// Extra weight for our capital
			if(c->isCapital())
				pairwDurations.push_back(pwd);
		}
		/*  No path from c, but we assume that there is a path from c to every other
			city of ours. If we find a path for some other city, c is assumed to
			have a mixed path. */
		else mixedPath++;
	}
	if((!reachByLand && !reachBySea) || pairwDurations.empty())
		return; // May leave distance at -1
	FAssert(cacheOwner.getNumCities() > mixedPath);
	sort(pairwDurations.begin(), pairwDurations.end());
	FAssert(pairwDurations[0] >= 0);
	double sumOfWeights = 0;
	double weightedSum = 0;
	// Allow distances to increase at most by 10 per rank
	int cap = pairwDurations[0] + 10;
	for(size_t i = 0; i < pairwDurations.size(); i++) {
		double w = 2.0 / (3 * (i + 1) - 1);
		sumOfWeights += w;
		int d = std::min(pairwDurations[i], cap);
		cap = d + 10;
		weightedSum += d * w;
	}
	weightedSum /= sumOfWeights; // Normalization
	/*  Hard to estimate the mixed paths. Their lengths certainly depend on
		the lengths of the other paths. */
	distance = std::min(maxDist, ::round(weightedSum + 2 * mixedPath));
	// The portion of mixed paths doesn't seem helpful after all; tends to be high
	//* std::max(1.0, 0.75 + mixedPath / ((double)pairwDistances.size() + mixedPath)));
}


double UWAICache::City::estimateMovementSpeed(PlayerTypes civId,
		DomainTypes dom, int dist) {

	CvPlayerAI const& civ = GET_PLAYER(civId);
	if(dom != DOMAIN_LAND)
		return civ.uwai().shipSpeed();
	EraTypes const era = civ.getCurrentEra();
	EraTypes const gameEra = GC.getGame().getCurrentEra();
	double r = 1;
	if(era >= 6) // Future era; to account for very high mobility in endgame
		r++;
	/*  Difficult to account for routes; c could be a border city, but the
		route could still lead through friendly territory and the owner
		of that territory may or may not have Engineering or Railroad. */
		// 4 is Industrial era
	if((era >= 4 && gameEra >= 4) || era >= 5)
		r *= 4.5; // Railroads are faster than this, but don't expect them everywhere.
	/*  Some roads in Classical era; the longer the path, the higher the
		chance of it traversing unpaved ground. */
	else if(era >= 1) {
		r *= ::dRange((20.0 + 6 * gameEra) / (dist + 10), 1.0, 2.0);
		if(GET_TEAM(civId).uwai().isFastRoads())
			r *= 1.4;
	}
	return r;
}

// <advc.104b>
bool UWAICache::City::measureDistance(PlayerTypes civId, DomainTypes dom,
		CvPlot const& start, CvPlot const& dest, int* r) {

	PROFILE_FUNC();
	/*  Caveat: dom can be IMMOBILE, which means Galley. Should compare dom
		only with DOMAIN_LAND in this function, not DOMAIN_SEA. */
	if(dom == DOMAIN_LAND && !start.sameArea(dest))
		return false;
	// Can't plot mixed-domain paths
	if(dom != DOMAIN_LAND && !start.isCoastalLand(-1))
		return false;
	int maxDist = (dom == DOMAIN_LAND ? getUWAI.maxLandDist() :
			getUWAI.maxSeaDist());
	// AI needs to be able to target even very remote rivals eventually
	if(GET_PLAYER(civId).getCurrentEra() >= 4)
		maxDist = (4 * maxDist) / 3;
	/*  stepDistance sanity check to avoid costly distance measurement
		(::teamStepValid_advc now performs the same check through ::stepHeuristic,
		but still need stepDistance here for the speed estimate.) */
	int stepDist = ::stepDistance(&start, &dest);
	double speedEstimate = estimateMovementSpeed(civId, dom, stepDist);
	if(stepDist / speedEstimate > maxDist)
		return false;
	CvPlot const* newDest = &dest;
	if(dom != DOMAIN_LAND && !dest.isCoastalLand(-1)) {
		/*  A naval assault drops the units off on a tile adjacent to the city;
			try to find an adjacent coastal tile. */
		int x = dest.getX();
		int y = dest.getY();
		newDest = NULL;
		int shortestStepDist = MAX_INT;
		FOR_EACH_ENUM(Direction) {
			CvPlot* adj = ::plotDirection(x, y, eLoopDirection);
			if(adj != NULL && adj->isCoastalLand(-1)) {
				int d = ::stepDistance(&start, adj);
				if(d < shortestStepDist) {
					newDest = adj;
					shortestStepDist = d;
				}
			}
		}
		if(newDest == NULL)
			return false;
	}
	if(dom != DOMAIN_LAND) {
		// The transports move onto a water tile adjacent to the coastal tile
		int destx = newDest->getX();
		int desty = newDest->getY();
		int shortestStepDist = MAX_INT;
		FOR_EACH_ENUM(Direction) {
			CvPlot* adj = ::plotDirection(destx, desty, eLoopDirection);
			if(adj != NULL && adj->isWater()) {
				int d = ::stepDistance(&start, adj);
				if(d < shortestStepDist) {
					newDest = adj;
					shortestStepDist = d;
				}
			}
		}
	}
	/*  This covers pack ice too (due to change 030). The PathDistance below
		won't take detours around ice into account though. */
	if(dom != DOMAIN_LAND && !start.isAdjacentToArea(newDest->getArea()))
		return false;
	// The original dest is guaranteed to be owned
	*r = start.calculatePathDistanceToPlot(start.getTeam(), *newDest, dest.getTeam(),
			/*  Path distance counts each step as 1 move; upper bound needs to
				account for faster movement. */
			dom, (int)::ceil(maxDist * speedEstimate));
	return (*r >= 0);
} // </advc.104b>

void UWAICache::City::updateAssetScore() {

	PROFILE_FUNC();
	if(city() != NULL) {
		/*  Scale: Same as CvPlayerAI::AI_cityWonderVal, i.e. approx. 50% GPT.
			Would rather use 100% GPT, but war evaluation can't easily be
			adjusted to that. */
		assetScore = (GET_PLAYER(cacheOwnerId).AI_assetVal(city()->AI(), true) / 2).round();
	}
}
