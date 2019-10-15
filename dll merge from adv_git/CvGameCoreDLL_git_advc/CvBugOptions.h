#pragma once

/**********************************************************************

File:		CvBugOptions.h
Author:		EmperorFool
Created:	2009-01-21

Calls out to the CvAppInterface Python module to check user options.

		Copyright (c) 2009 The BUG Mod. All rights reserved.

**********************************************************************/

#ifndef BUG_OPTIONS_H
#define BUG_OPTIONS_H

// Must use existing module because the DLL cannot see new modules in CustomAssets
#define PYBugOptionsModule PYCivModule

// Text prepended to option name if no XML key given
// advc: unused
//#define OPTION_XML_PREFIX "BULL__"

// advc: No implementation for these
/*void logMsg(const char* format, ...);
bool isBug();
void setIsBug(bool bIsBug);*/
// advc.003t: Replaced by functions in CvGlobals
/*bool getDefineBOOL(const char* xmlKey, bool bDefault = false);
int getDefineINT(const char* xmlKey, int iDefault = 0);*/

/*  advc: Removed unused param char const* xmlKey = NULL
	bWarn added */
bool getBugOptionBOOL(const char* id, bool bDefault = true, bool bWarn = true);
int getBugOptionINT(const char* id, int iDefault = 0, bool bWarn = true);
CvString getUserDirPath(); // advc.003d

#endif