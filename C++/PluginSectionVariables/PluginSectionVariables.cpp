/* Copyright (C) 2017 Rainmeter Project Developers
 *
 * This Source Code Form is subject to the terms of the GNU General Public
 * License; either version 2 of the License, or (at your option) any later
 * version. If a copy of the GPL was not distributed with this file, You can
 * obtain one at <https://www.gnu.org/licenses/gpl-2.0.html>. */

#include <Windows.h>
#include "../../API/RainmeterAPI.h"
#include <algorithm>
#include <cwctype>
#include <string>

// Overview: This example demonstrates using plugin section variables.
// In this example we build a ToUpper and a ToLower section variable that can either get the measure text 
// or transform the string passed to uppercase/lowercase

// Sample skin:
/*
	[Rainmeter]
	Update=1000
	DynamicWindowSize=1
	BackgroundMode=2
	SolidColor=255,255,255

	[Variables]
	TextString=click me to make me uppercase!

	[mString]
	Measure=Plugin
	Plugin=SectionVariables
	Input=I AM A SECTION VARIABLE!

	[TextLower]
	Meter=String
	Text=[mString:ToLower()]

	; Because regular options will replace section variables when
	;  read, DynamicVariables is required in this case.
	DynamicVariables=1

	[TextUpper]
	Meter=String
	Y=5R
	Text=#TextString#

	; Because actions do not replace section variables when read, DynamicVariables=1 is not needed
	LeftMouseUpAction=[!SetOption TextUpper Text "[mString:ToUpper(#TextString#)]"]
*/

struct Measure
{
	std::wstring strValue;

	Measure() :
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
	measure->strValue = RmReadString(rm, L"Input", L"");
}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* measure = (Measure*)data;
	return 0.0;
}

PLUGIN_EXPORT LPCWSTR GetString(void* data)
{
	Measure* measure = (Measure*)data;
	return  measure->strValue.c_str();  // Might be an empty string if no |Input| option is defined
}

PLUGIN_EXPORT LPCWSTR ToUpper(void* data, const int argc, const WCHAR* argv[])
{
	static std::wstring buffer;
	Measure* measure = (Measure*)data;

	if (argc > 0)
	{
		buffer = argv[0];  // Only transform the first argument
	}
	else
	{
		buffer = measure->strValue;
	}

	std::transform(buffer.begin(), buffer.end(), buffer.begin(), std::towupper);
	return buffer.c_str();
}

PLUGIN_EXPORT LPCWSTR ToLower(void* data, const int argc, const WCHAR* argv[])
{
	static std::wstring buffer;
	Measure* measure = (Measure*)data;

	if (argc > 0)
	{
		buffer = argv[0];  // Only transform the first argument
	}
	else
	{
		buffer = measure->strValue;
	}

	std::transform(buffer.begin(), buffer.end(), buffer.begin(), std::towlower);
	return buffer.c_str();
}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;
	delete measure;
}
