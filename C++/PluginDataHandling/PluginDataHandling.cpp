/* Copyright (C) 2017 Rainmeter Project Developers
 *
 * This Source Code Form is subject to the terms of the GNU General Public
 * License; either version 2 of the License, or (at your option) any later
 * version. If a copy of the GPL was not distributed with this file, You can
 * obtain one at <https://www.gnu.org/licenses/gpl-2.0.html>. */

#include <Windows.h>
#include "../../API/RainmeterAPI.h"


// Overview: This example demonstrates using both the data argument to keep data across
// functions calls and also storing data in the |Rainmeter.data| file for presist access across
// skin reloads and even measures. In this example we will make a counter that counts the number
// of updates that happen and saves it when the skin unloads to the |Rainmeter.data| file.
// Note: Remember that the Update function will not be called when the measure is paused or disabled.

//Sample skin:
/*
	[Rainmeter]
	Update=1000
	DynamicWindowSize=1
	BackgroundMode=2
	SolidColor=255,255,255

	[mCount]
	Measure=Plugin
	Plugin=DataHandling
	SaveData=1

	[mCountAt0]
	Measure=Plugin
	Plugin=DataHandling
	StartingValue=0

	[MeterCount]
	Meter=String
	MeasureName=mCount

	[MeterCountAt0]
	Meter=String
	MeasureName=mCountAt0
	Y=5R
*/

struct Measure
{
	int counter;
	bool saveData;

	LPCWSTR dataFile;

	Measure() :
		counter(0),
		saveData(false),
		dataFile(nullptr) {}
};


PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* measure = new Measure;
	*data = measure;

	// Get the path to the settings data file
	measure->dataFile = RmGetSettingsFile();
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	Measure* measure = (Measure*)data;

	// Note: If |DynamicVariables=1| is set on the measure, this function will get called
	//  on every update cycle - meaning the counter values will use the starting value and
	//  appear to *not* update at all.

	// Get the starting value if one is defined
	measure->counter = RmReadInt(rm, L"StartingValue", -1);

	// If |StartingValue| was not defined with with measure, read the |Count| from the data file
	if (measure->counter < 0)
	{
		// Get |Count| from the Rainmeter data file. If the key does not exist
		//  in the data file, 0 will be used as the default value.
		measure->counter = (int)GetPrivateProfileInt(L"Plugin_DataHandling", L"Count", 0, measure->dataFile);
	}

	// If the counter is to be saved in the data file, set SaveData=1 in the measure (in the
	//  skin file).  The counter will be saved to the |Count| key in the data file when the
	//  skin is unloaded or refreshed.
	measure->saveData = RmReadInt(rm, L"SaveData", 0) == 1 ? true : false;
}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* measure = (Measure*)data;

	// Every measure will have its own counter separate from other measures.
	// Increase the counter and reset if needed.
	if (++measure->counter < 0)	measure->counter = 0;

	return measure->counter;
}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;

	if (measure->saveData)
	{
		// Note: In this example multiple |DataHandling| plugin measures (from any skin) will
		//  overwrite this value. The counter from the last measure unloaded will persist.
		WCHAR buffer[SHRT_MAX];
		_itow_s(measure->counter, buffer, 10);
		WritePrivateProfileString(L"Plugin_DataHandling", L"Count", buffer, measure->dataFile);
	}

	delete measure;
}
