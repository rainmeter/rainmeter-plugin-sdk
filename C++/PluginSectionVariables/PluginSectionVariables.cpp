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
#include <algorithm>

// Overview: This example demonstrates using plugin section variables, aka calling a plugin inline.
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
;You can't get a Meter's text so use a variable
TextString=Click me to make me uppercase!

[mString]
Measure=Plugin
Plugin=SectionVariables.dll
Input=I come from the measure reference!

[TextLower]
Meter=String
X=0
Y=0
FontSize=20
;Notice how we need DynamicVariable=1 since section variables can not be called on initialize
Text=[&mString:ToLower()]
DynamicVariables=1

[TextUpper]
Meter=String
X=0
Y=0R
FontSize=20
Text=#TextString#
;Notice how we don't need DynamicVariables=1 if we set the text manually
LeftMouseUpAction=[!SetOption TextUpper Text "[&mString:ToUpper(#TextString#)]"]
*/

struct Measure
{
	Measure() {}
	std::wstring myString; //The string returned in GetString is stored here
};

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* measure = new Measure;
	*data = measure;
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	Measure* measure = (Measure*)data;

	//Read measure for an Input string
	measure->myString = RmReadString(rm, L"Input", L"");
}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* measure = (Measure*)data;
	return 0.0;
}

PLUGIN_EXPORT LPCWSTR GetString(void* data)
{
	Measure* measure = (Measure*)data;
	return measure->myString.c_str();
}

std::wstring stringBuffer; //String buffer is used to make sure that the string we return does not get deallocated before Rainmeter can use it

PLUGIN_EXPORT LPCWSTR ToUpper(void* data, const int argc, const WCHAR* argv[])
{
	Measure* measure = (Measure*)data;

	//If we are given one or more arguments transform the first one
	if (argc > 0)
	{
		stringBuffer = argv[0];
		std::transform(stringBuffer.begin(), stringBuffer.end(), stringBuffer.begin(), ::toupper);
		return stringBuffer.c_str();
	}

	//If we are given no arguments transform the string we recived with the input option
	stringBuffer = measure->myString;
	std::transform(stringBuffer.begin(), stringBuffer.end(), stringBuffer.begin(), ::toupper);
	return stringBuffer.c_str();
}
PLUGIN_EXPORT LPCWSTR ToLower(void* data, const int argc, const WCHAR* argv[])
{
	Measure* measure = (Measure*)data;

	//If we are given one or more arguments transform the first one
	if (argc > 0)
	{
		stringBuffer = argv[0];
		std::transform(stringBuffer.begin(), stringBuffer.end(), stringBuffer.begin(), ::tolower);
		return stringBuffer.c_str();
	}

	//If we are given no arguments transform the string we recived with the input option
	stringBuffer = measure->myString;
	std::transform(stringBuffer.begin(), stringBuffer.end(), stringBuffer.begin(), ::tolower);
	return stringBuffer.c_str();
}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;
	delete measure;
}
