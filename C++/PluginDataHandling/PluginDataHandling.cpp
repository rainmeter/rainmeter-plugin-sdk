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

// Overview: This example demonstrates using both the data argument to keep data across
// functions calls and also storing data in the Rainmeter.Data file for presistance across
// skin reloads and even measures. In this example we will make a counter that counts the
// number of updates that happens and saves them on Finalize to the Rainmeter.data
// Note: You should never rely on Update happening at the exact time you specify

// Sample skin:
/*
[Rainmeter]
Update=1000
DynamicWindowSize=1
BackgroundMode=2
SolidColor=255,255,255

[mCount]
Measure=Plugin
Plugin=DataHandling.dll
StoreData=1

[TextCount]
Meter=String
MeasureName=mCount
X=0
Y=0R
FontSize=20
Text=%1

[mCountAt0]
Measure=Plugin
Plugin=DataHandling.dll
StartingValue=0

[TextCountAt0]
Meter=String
MeasureName=mCountAt0
X=0
Y=0R
FontSize=20
Text=%1
*/

struct Measure
{
	Measure() {}
	int myCounter = 0;
	bool storeData = false;
};

LPCWSTR PluginName = L"DataHandling"; //Always match this to your plugin name
LPCWSTR KeyName = L"StoredCount";
const int MAXSIZE = 256;

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* measure = new Measure;
	*data = measure;
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	Measure* measure = (Measure*)data;

	//Get starting value if one is defined
	int startValue = RmReadInt(rm, L"StartingValue", -1);

	//If string from option was null read from file
	if (startValue == -1)
	{
		LPTSTR outString = new TCHAR[MAXSIZE];

		//Read from Rainmeter.data file, if no instace exists start at 0
		GetPrivateProfileString(PluginName, KeyName, L"0", outString, MAXSIZE, RmGetSettingsFile());
		measure->myCounter = _wtoi(outString);
	}
	else
	{
		measure->myCounter = startValue;
	}
	//If store data is 1 then save data when measure is unloaded
	measure->storeData = RmReadInt(rm, L"StoreData", 0) == 1 ? true : false;
}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* measure = (Measure*)data;

	//Notice how the every measure has its own counter since it gets it from data
	return ++measure->myCounter;
}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;

	//Note that in this example if multiple measures have store data set to true whichever unloads last's data will persist
	if (measure->storeData)
	{
		wchar_t buffer[MAXSIZE];
		wsprintfW(buffer, L"%d", measure->myCounter);
		WritePrivateProfileString(PluginName, KeyName, buffer, RmGetSettingsFile());
	}

	delete measure;
}
