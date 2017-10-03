/* Copyright (C) 2017 Rainmeter Project Developers
 *
 * This Source Code Form is subject to the terms of the GNU General Public
 * License; either version 2 of the License, or (at your option) any later
 * version. If a copy of the GPL was not distributed with this file, You can
 * obtain one at <https://www.gnu.org/licenses/gpl-2.0.html>. */

#include <Windows.h>
#include <cstdio>
#include <string>
#include "../../API/RainmeterAPI.h"

// Overview: This example demonstrates the basic concept of Rainmeter C++ plugins.

// Sample skin:
/*
	[Rainmeter]
	Update=1000
	DynamicWindowSize=1
	BackgroundMode=2
	SolidColor=255,255,255

	[mString]
	Measure=Plugin
	Plugin=SystemVersion
	Type=String

	[mMajor]
	Measure=Plugin
	Plugin=SystemVersion
	Type=Major

	[mMinor]
	Measure=Plugin
	Plugin=SystemVersion
	Type=Minor

	[mNumber]
	Measure=Plugin
	Plugin=SystemVersion
	Type=Number

	[Text1]
	Meter=String
	MeasureName=mString
	MeasureName2=mMajor
	MeasureName3=mMinor
	MeasureName4=mNumber
	Text="String: %1#CRLF#Major: %2#CRLF#Minor: %3#CRLF#Number: %4#CRLF#"

	[Text2]
	Meter=String
	MeasureName=mString
	MeasureName2=mMajor
	MeasureName3=mMinor
	MeasureName4=mNumber
	NumOfDecimals=1
	Y=5R
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
	std::wstring strValue;

	Measure() :
		type(MEASURE_MAJOR),
		strValue() {}
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
		RmLog(rm, LOG_ERROR, L"Invalid \"Type\"");
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

	case MEASURE_STRING:
		{
			WCHAR buffer[128];
			_snwprintf_s(buffer, _TRUNCATE, L"%i.%i (Build %i)",
				(int)osvi.dwMajorVersion, (int)osvi.dwMinorVersion, (int)osvi.dwBuildNumber);
			measure->strValue = buffer;
		}
		break;
	}

	// MEASURE_STRING is not a number and therefore will be returned in GetString. However it is
	//  recommended that you do any processing here in the Update function rather than the GetString
	//  function since GetString can be called multiple times during the update cycle. Store any
	//  string values in your Measure structure and return it in GetString.

	return 0.0;
}

PLUGIN_EXPORT LPCWSTR GetString(void* data)
{
	Measure* measure = (Measure*)data;

	// Although GetVersionEx is a rather inexpensive operation, it is recommended to
	// do any processing in the Update function. See the comments above.
	if (!measure->strValue.empty())
	{
		switch (measure->type)
		{
		case MEASURE_STRING:
			return measure->strValue.c_str();
		}
	}

	// MEASURE_MAJOR, MEASURE_MINOR, and MEASURE_NUMBER are numbers. Therefore,
	// |nullptr| is returned here for them. This is to inform Rainmeter that it can
	// treat those types as numbers.

	return nullptr;
}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;
	delete measure;
}
