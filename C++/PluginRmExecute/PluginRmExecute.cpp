/*
Copyright (C) 2017 Trevor Hamilton

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <Windows.h>
#include "../../API/RainmeterAPI.h"
#include <string>
#include <ctime>

// Overview: This is an example of how to make an option be read and execute the bang within.
// It also showcases how you can get some of the affects of DynamicVariables=1 without as much
// of a performance hit. As well it also shows why you should be careful of skins turning on DV=1

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

;This measure will change TextCount after #Timer# seconds
[mTimer]
Measure=Plugin
Plugin=RmExecute.dll
Timer=#Timer#
OnTimer=[!SetOption TextCount Text "This text has changed!"]

[TextCount]
Meter=String
X=0
Y=0R
FontSize=20
Text=This text has not changed yet.

[mTimerC]
Measure=Plugin
Plugin=RmExecute.dll
Timer=#Timer#
;Notice how we can even do more complex bangs.
;If you uncomment the code in update the variable reference will even autoupdate without DynamicVariables=1
;Or you could just uncomment DynamicVariables=1
OnTimer=[!SetVariable Count "(#Count#+1)"][!SetOption TextCountC Text "This text has been updated #Count# times"]
;DynamicVariables=1

[TextCountC]
Meter=String
X=0
Y=0R
FontSize=20
Text=This text has been updated #Count# times
*/

struct Measure
{
	Measure() {}
	std::wstring myCommand = L"";
	int updateRate;
	time_t timer;

	void* skin;
	void* rm;
};

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* measure = new Measure;
	*data = measure;
	measure->skin = RmGetSkin(rm);
	measure->rm = rm;

	//We do this in Initialize so that if a skin were to turn on DynamicVariables it will not break the plugin
	measure->timer = std::time(0);
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	Measure* measure = (Measure*)data;
	measure->updateRate = RmReadInt(rm, L"Timer", 1); 
	//We dont have to replace measures here as they will be replaced during RmExecute so we can pass false. 
    //Note though that doing that measures will always then have their current info in update but variables will not. See the commented out code below to have both always act like DynamicVariables=1
	measure->myCommand = RmReadString(rm, L"OnTimer", L"", false);
}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* measure = (Measure*)data;

	int timePassed = std::time(0) - measure->timer;

	if (timePassed >= measure->updateRate)
	{
		//We could also do something like this to get DynamicVariables without the need to turn them on. Uncomment this to see the second example work
		//measure->myCommand = RmReadString(measure->rm, L"OnTimer", L"", false);

		RmExecute(measure->skin, measure->myCommand.c_str());
		measure->timer = std::time(0);
	}

	//Even if you don't plan for users to use it returning a value can be useful for helping with skin debugging
	return measure->updateRate - timePassed;
}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;
	delete measure;
}
