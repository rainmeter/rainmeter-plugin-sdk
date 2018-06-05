/* Copyright (C) 2017 Rainmeter Project Developers
 *
 * This Source Code Form is subject to the terms of the GNU General Public
 * License; either version 2 of the License, or (at your option) any later
 * version. If a copy of the GPL was not distributed with this file, You can
 * obtain one at <https://www.gnu.org/licenses/gpl-2.0.html>. */

#include <Windows.h>
#include "../../API/RainmeterAPI.h"
#include <ctime>
#include <string>

// Overview: This is an example of how to make an option be read and execute the bang within.

// Sample skin:
/*
	[Rainmeter]
	Update=1000
	DynamicWindowSize=1
	BackgroundMode=2
	SolidColor=255,255,255

	[Variables]
	Count=0
	Timer=5

	[mTimer]
	Measure=Plugin
	Plugin=RmExecute
	Timer=#Timer#
	OnTimer=[!SetOption Example1 Text "This text has changed!"]

	[mTimer2]
	Measure=Plugin
	Plugin=RmExecute
	Timer=#Timer#
	OnTimer=[!SetVariable Count "(#Count# + 1)"]
	DynamicVariables=1

	[Example1]
	Meter=String
	Text=This text has not changed yet.

	[Example2]
	Meter=String
	Y=5R
	Text=This text has been updated #Count# times.
	DynamicVariables=1
*/

struct Measure
{
	std::wstring command;
	double updateRate;
	time_t timer;

	void* skin;  // Pointer to the skin (needed for RmExecute)

	Measure() :
		command(),
		updateRate(0),
		timer(std::time(nullptr)),  // You can also do this in the Initalize function
		skin(nullptr) {}
};

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* measure = new Measure;
	*data = measure;

	measure->skin = RmGetSkin(rm);
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	Measure* measure = (Measure*)data;
	measure->updateRate = RmReadDouble(rm, L"Timer", 1.0);

	// When reading an action, do not replace any section variables in the option so
	//  that when the action is executed, the most recent value of the measure will be
	//  used. Note the boolean parameter.
	measure->command = RmReadString(rm, L"OnTimer", L"", FALSE);
}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* measure = (Measure*)data;

	double difference = std::difftime(time(nullptr), measure->timer);
	if (difference >= measure->updateRate)
	{
		RmExecute(measure->skin, measure->command.c_str());
		measure->timer = std::time(nullptr);
	}

	return measure->updateRate - difference;
}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;
	delete measure;
}
