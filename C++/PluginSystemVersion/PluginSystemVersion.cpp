/*
  Copyright (C) 2012 Birunthan Mohanathas

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
#include <cstdio>
#include "../../API/RainmeterAPI.h"

// Overview: This example demonstrates the basic concept of Rainmeter C++ plugins.

// Sample skin:
/*
	[Rainmeter]
	Update=1000
	BackgroundMode=2
	SolidColor=000000

	[mString]
	Measure=Plugin
	Plugin=SystemVersion.dll
	Type=String

	[mMajor]
	Measure=Plugin
	Plugin=SystemVersion.dll
	Type=Major

	[mMinor]
	Measure=Plugin
	Plugin=SystemVersion.dll
	Type=Minor

	[mNumber]
	Measure=Plugin
	Plugin=SystemVersion.dll
	Type=Number

	[Text1]
	Meter=STRING
	MeasureName=mString
	MeasureName2=mMajor
	MeasureName3=mMinor
	MeasureName4=mNumber
	X=5
	Y=5
	W=300
	H=70
	FontColor=FFFFFF
	Text="String: %1#CRLF#Major: %2#CRLF#Minor: %3#CRLF#Number: %4#CRLF#"

	[Text2]
	Meter=STRING
	MeasureName=mString
	MeasureName2=mMajor
	MeasureName3=mMinor
	MeasureName4=mNumber
	NumOfDecimals=1
	X=5
	Y=5R
	W=300
	H=70
	FontColor=FFFFFF
	Text="String: %1#CRLF#Major: %2#CRLF#Minor: %3#CRLF#Number: %4#CRLF#"
*/

enum MeasureType
{
	MEASURE_MAJOR,
	MEASURE_MINOR,
	MEASURE_NUMBER,
	MEASURE_STRING
};

struct Measure
{
	MeasureType type;

	Measure() : type(MEASURE_MAJOR) {}
};

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* measure = new Measure;
	*data = measure;
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	Measure* measure = (Measure*)data;

	LPCWSTR value = RmReadString(rm, L"Type", L"");
	if (_wcsicmp(value, L"Major") == 0)
	{
		measure->type = MEASURE_MAJOR;
	}
	else if (_wcsicmp(value, L"Minor") == 0)
	{
		measure->type = MEASURE_MINOR;
	}
	else if (_wcsicmp(value, L"Number") == 0)
	{
		measure->type = MEASURE_NUMBER;
	}
	else if (_wcsicmp(value, L"String") == 0)
	{
		measure->type = MEASURE_STRING;
	}
	else
	{
		RmLog(LOG_ERROR, L"SystemVersion.dll: Invalid Type=");
	}
}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* measure = (Measure*)data;

	OSVERSIONINFOEX osvi = {sizeof(OSVERSIONINFOEX)};
	if (!GetVersionEx((OSVERSIONINFO*)&osvi))
	{
		return 0.0;
	}

	switch (measure->type)
	{
	case MEASURE_MINOR:
		return (double)osvi.dwMinorVersion;

	case MEASURE_MAJOR:
		return (double)osvi.dwMajorVersion;

	case MEASURE_NUMBER:
		return (double)osvi.dwMajorVersion + ((double)osvi.dwMinorVersion / 10.0);
	}

	// MEASURE_STRING is not a number and and therefore will be returned in GetString.

	return 0.0;
}

PLUGIN_EXPORT LPCWSTR GetString(void* data)
{
	Measure* measure = (Measure*)data;
	static WCHAR buffer[128];

	// GetVersionEx is an inexpensive operation, so we repeat it here. If what you
	// measure requires an expensive operation, it is reccomended that you do it
	// in Update (which is called only once per update cycle), store it in the
	// Measure structure, and return it here. As GetString may be called multiple
	// times per update cycle, it is not reccomended to do expensive operations here.
	OSVERSIONINFOEX osvi = {sizeof(OSVERSIONINFOEX)};
	if (!GetVersionEx((OSVERSIONINFO*)&osvi))
	{
		return NULL;
	}

	switch (measure->type)
	{
	case MEASURE_STRING:
		_snwprintf_s(buffer, _TRUNCATE, L"%i.%i (Build %i)", (int)osvi.dwMajorVersion, (int)osvi.dwMinorVersion, (int)osvi.dwBuildNumber);
		return buffer;
	}

	// MEASURE_MAJOR, MEASURE_MINOR, and MEASURE_NUMBER are numbers. Therefore,
	// NULL is returned here for them. This is to inform Rainmeter that it can
	// treat those types as numbers.

	return NULL;
}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;
	delete measure;
}
