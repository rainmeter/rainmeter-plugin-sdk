/*
  Copyright (C) 2011 Kimmo Pekkola, Birunthan Mohanathas

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

#ifndef __RAINMETERAPI_H__
#define __RAINMETERAPI_H__

#ifdef LIBRARY_EXPORTS
#define LIBRARY_EXPORT EXTERN_C
#else
#define LIBRARY_EXPORT EXTERN_C __declspec(dllimport)
#endif // LIBRARY_EXPORTS

#define PLUGIN_EXPORT EXTERN_C __declspec(dllexport)

//
// Exported functions
//

#ifdef __cplusplus
/// <summary>
/// Returns a string representation of an option.
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
/// <param name="option">Option name to read string from</param>
/// <param name="defValue">Default value for option (returned if option is not found)</param>
/// <param name="replaceMeasures">If TRUE, replaces [MeasureNames] in returned string.</param>
LIBRARY_EXPORT LPCWSTR __stdcall RmReadString(void* rm, LPCWSTR option, LPCWSTR defValue, BOOL replaceMeasures = TRUE);
#else
/// <summary>
/// Returns a string representation of an option.
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
/// <param name="option">Option name to read string from</param>
/// <param name="defValue">Default value for option (returned if option is not found)</param>
/// <param name="replaceMeasures">If TRUE, replaces [MeasureNames] in returned string</param>
LIBRARY_EXPORT LPCWSTR __stdcall RmReadString(void* rm, LPCWSTR option, LPCWSTR defValue, BOOL replaceMeasures);
#endif // __cplusplus

/// <summary>
/// Returns a string representation of an option.
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
/// <param name="option">Option name to read string from</param>
/// <param name="defValue">Default value for option (returned if option is not found)</param>
LIBRARY_EXPORT double __stdcall RmReadFormula(void* rm, LPCWSTR option, double defValue);

/// <summary>
/// Parse a string, replacing any #variables# or [sectionVariables] within it.
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
/// <param name="str">String to parse variables into their values</param>
LIBRARY_EXPORT LPCWSTR __stdcall RmReplaceVariables(void* rm, LPCWSTR str);

/// <summary>
/// Parse a relative path from the root of the skin
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
/// <param name="relativePath">Realative path to parse</param>
LIBRARY_EXPORT LPCWSTR __stdcall RmPathToAbsolute(void* rm, LPCWSTR relativePath);

/// <summary>
/// Executes a command.
/// </summary>
/// <param name="skin">Pointer to current skin (obtained from RmGetSkin)</param>
/// <param name="command">Bang to execute</param>
LIBRARY_EXPORT void __stdcall RmExecute(void* skin, LPCWSTR command);

/// <summary>
/// Get one of the things available from RmGetType, use the helper functions instead of calling directly
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
/// <param name="type">RmGetType you would like to get</param>
LIBRARY_EXPORT void* __stdcall RmGet(void* rm, RmGetType type);

enum RmGetType
{
	RMG_MEASURENAME      = 0,
	RMG_SKIN             = 1,
	RMG_SETTINGSFILE     = 2,
	RMG_SKINNAME         = 3,
	RMG_SKINWINDOWHANDLE = 4
};

/// <summary>
/// Sends a message to the Rainmeter log, if no plugin measure is given the source will be blank.
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
/// <param name="level">Log level (LOG_ERROR, LOG_WARNING, LOG_NOTICE, or LOG_DEBUG). LOG_DEBUG messages are logged only when Rainmeter is in debug mod.</param>
/// <param name="message">Message to be logged</param>
LIBRARY_EXPORT void __stdcall RmLog(void* rm, int level, LPCWSTR message);

/// <summary>
/// Sends a message to the Rainmeter log, if no plugin measure is given the source will be blank.
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
/// <param name="level">Log level (LOG_ERROR, LOG_WARNING, LOG_NOTICE, or LOG_DEBUG). LOG_DEBUG messages are logged only when Rainmeter is in debug mod.</param>
/// <param name="format">Message to be logged, follows printf syntax</param>
/// <param name="...">Specifier values</param>
LIBRARY_EXPORT void __cdecl RmLogF(void* rm, int level, LPCWSTR format, ...);

/// <summary>
/// DEPRECATED: Use RmLog. Sends a message to the Rainmeter log.
/// </summary>
LIBRARY_EXPORT BOOL LSLog(int type, LPCWSTR unused, LPCWSTR message);

//
// Wrapper functions
//

#ifndef LIBRARY_EXPORTS
/// <summary>
/// Returns a string representation of an option. If the option is an absolute path to a folder or file, it is returned as is. If the option is a relative path, a converted absolute path is returned.
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
/// <param name="option">Option name to read path from</param>
/// <param name="defValue">Default value for option (returned after conversion to absolute path if option is not found)</param>
__inline LPCWSTR RmReadPath(void* rm, LPCWSTR option, LPCWSTR defValue)
{
	LPCWSTR relativePath = RmReadString(rm, option, defValue, TRUE);
	return RmPathToAbsolute(rm, relativePath);
}

/// <summary>
/// Returns a int representation of an option. If the option is a formula, the return value is the result of the parsed formula.
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
/// <param name="option">Option name to read integer from</param>
/// <param name="defValue">Default return value (returned if option is not found, option is not an number, or if formula could not be parsed)</param>
__inline int RmReadInt(void* rm, LPCWSTR option, int defValue)
{
	return (int)RmReadFormula(rm, option, defValue);
}

/// <summary>
/// Returns a double representation of an option. If the option is a formula, the return value is the result of the parsed formula.
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
/// <param name="option">Option name to read double from</param>
/// <param name="defValue">Default return value (returned if option is not found, option is not an number, or if formula could not be parsed)</param>
__inline double RmReadDouble(void* rm, LPCWSTR option, double defValue)
{
	return RmReadFormula(rm, option, defValue);
}

/// <summary>
/// Returns the name of the current measure.
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
__inline LPCWSTR RmGetMeasureName(void* rm)
{
	return (LPCWSTR)RmGet(rm, RMG_MEASURENAME);
}

/// <summary>
/// Returns the path to the Rainmeter settings file.
/// </summary>
__inline LPCWSTR RmGetSettingsFile()
{
	return (LPCWSTR)RmGet(NULL, RMG_SETTINGSFILE);
}

/// <summary>
/// Returns an internal pointer to the current skin.
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
__inline void* RmGetSkin(void* rm)
{
	return (void*)RmGet(rm, RMG_SKIN);
}

/// <summary>
/// Returns the full name of the skin. (Not just the file name)
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
__inline LPCWSTR RmGetSkinName(void* rm)
{
	return (LPCWSTR)RmGet(rm, RMG_SKINNAME);
}

/// <summary>
/// Returns the HWND of the skin window.
/// </summary>
/// <param name="rm">Pointer to the plugin measure</param>
__inline HWND RmGetSkinWindow(void* rm)
{
	return (HWND)RmGet(rm, RMG_SKINWINDOWHANDLE);
}

/// <summary>
/// Sends a message to the Rainmeter log, if no plugin measure is given the source will be blank.
/// </summary>
/// <param name="level">Log level (LOG_ERROR, LOG_WARNING, LOG_NOTICE, or LOG_DEBUG). LOG_DEBUG messages are logged only when Rainmeter is in debug mod.</param>
/// <param name="message">Message to be logged</param>
__inline void RmLog(int level, LPCWSTR message)
{
	LSLog(level, NULL, message);
}

enum LOGLEVEL
{
	LOG_ERROR   = 1,
	LOG_WARNING = 2,
	LOG_NOTICE  = 3,
	LOG_DEBUG   = 4
};
#endif // LIBRARY_EXPORTS

#define RmReadFormula _ReadFormula

#endif
