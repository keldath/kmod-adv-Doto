#pragma once
#ifndef CV_CITY_MACROS_H
#define CV_CITY_MACROS_H

//mylon doto version

#undef NUM_CITY_PLOTS
#define NUM_CITY_PLOTS numCityPlots()

#define FOR_EACH_CITYPLOT \
	 for (CityPlotTypes eLoopCityPlot = CITY_HOME_PLOT; \
         eLoopCityPlot < numCityPlots(); ++eLoopCityPlot)

#endif
