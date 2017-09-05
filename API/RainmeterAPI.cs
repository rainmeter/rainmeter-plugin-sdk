/*
  Copyright (C) 2011 Birunthan Mohanathas

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

using System;
using System.Runtime.InteropServices;

namespace Rainmeter
{
    /// <summary>
    /// Wrapper around the Rainmeter C API.
    /// </summary>
    public class API
    {
        private IntPtr m_Rm;

        public API(IntPtr rm)
        {
            m_Rm = rm;
        }

        [DllImport("Rainmeter.dll", CharSet = CharSet.Unicode)]
        private extern static IntPtr RmReadString(IntPtr rm, string option, string defValue, bool replaceMeasures);

        [DllImport("Rainmeter.dll", CharSet = CharSet.Unicode)]
        private extern static double RmReadFormula(IntPtr rm, string option, double defValue);

        [DllImport("Rainmeter.dll", CharSet = CharSet.Unicode)]
        private extern static IntPtr RmReplaceVariables(IntPtr rm, string str);

        [DllImport("Rainmeter.dll", CharSet = CharSet.Unicode)]
        private extern static IntPtr RmPathToAbsolute(IntPtr rm, string relativePath);

        [DllImport("Rainmeter.dll", EntryPoint = "RmExecute", CharSet = CharSet.Unicode)]
        public extern static void Execute(IntPtr skin, string command);

        [DllImport("Rainmeter.dll")]
        private extern static IntPtr RmGet(IntPtr rm, RmGetType type);

        [DllImport("Rainmeter.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        private extern static int LSLog(LogType type, string unused, string message);

        [DllImport("Rainmeter.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
        private extern static int RmLog(IntPtr rm, LogType type, string message);

        [DllImport("Rainmeter.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        private extern static int RmLogF(IntPtr rm, LogType type, string format, params string[] message);

        private enum RmGetType
        {
            MeasureName = 0,
            Skin = 1,
            SettingsFile = 2,
            SkinName = 3,
            SkinWindowHandle = 4
        }

        public enum LogType
        {
            Error = 1,
            Warning = 2,
            Notice = 3,
            Debug = 4
        }

        /// <summary>
        /// Returns a string representation of an option.
        /// </summary>
        /// <param name="option">Option name to read string from</param>
        /// <param name="defValue">Default value for option (returned if option is not found)</param>
        /// <param name="replaceMeasures">If TRUE, replaces [MeasureNames] in returned string.</param>
        public string ReadString(string option, string defValue, bool replaceMeasures = true)
        {
            return Marshal.PtrToStringUni(RmReadString(m_Rm, option, defValue, replaceMeasures));
        }

        /// <summary>
        /// Returns a string representation of an option. If the option is an absolute path to a folder or file, it is returned as is. If the option is a relative path, a converted absolute path is returned.
        /// </summary>
        /// <param name="option">Option name to read path from</param>
        /// <param name="defValue">Default value for option (returned after conversion to absolute path if option is not found)</param>
        public string ReadPath(string option, string defValue)
        {
            return Marshal.PtrToStringUni(RmPathToAbsolute(m_Rm, ReadString(option, defValue)));
        }

        /// <summary>
        /// Returns a double representation of an option. If the option is a formula, the return value is the result of the parsed formula.
        /// </summary>
        /// <param name="option">Option name to read double from</param>
        /// <param name="defValue">Default return value (returned if option is not found, option is not an number, or if formula could not be parsed)</param>
        public double ReadDouble(string option, double defValue)
        {
            return RmReadFormula(m_Rm, option, defValue);
        }

        /// <summary>
        /// Returns a int representation of an option. If the option is a formula, the return value is the result of the parsed formula.
        /// </summary>
        /// <param name="option">Option name to read integer from</param>
        /// <param name="defValue">Default return value (returned if option is not found, option is not an number, or if formula could not be parsed)</param>
        public int ReadInt(string option, int defValue)
        {
            return (int)RmReadFormula(m_Rm, option, defValue);
        }

        /// <summary>
        /// Parse a string, replacing any #variables# or [sectionVariables] within it.
        /// </summary>
        /// <param name="str">String to parse variables into their values</param>
        public string ReplaceVariables(string str)
        {
            return Marshal.PtrToStringUni(RmReplaceVariables(m_Rm, str));
        }

        /// <summary>
        /// Returns the name of the current measure.
        /// </summary>
        public string GetMeasureName()
        {
            return Marshal.PtrToStringUni(RmGet(m_Rm, RmGetType.MeasureName));
        }

        /// <summary>
        /// Returns an internal pointer to the current skin.
        /// </summary>
        public IntPtr GetSkin()
        {
            return RmGet(m_Rm, RmGetType.Skin);
        }

        /// <summary>
        /// Returns the path to the Rainmeter settings file.
        /// </summary>
        public string GetSettingsFile()
        {
            return Marshal.PtrToStringUni(RmGet(m_Rm, RmGetType.SettingsFile));
        }

        /// <summary>
        /// Returns the full name of the skin. (Not just the file name)
        /// </summary>
        public string GetSkinName()
        {
            return Marshal.PtrToStringUni(RmGet(m_Rm, RmGetType.SkinName));
        }

        /// <summary>
        /// Returns the HWND of the skin window.
        /// </summary>
        public IntPtr GetSkinWindow()
        {
            return RmGet(m_Rm, RmGetType.SkinWindowHandle);
        }

        /// <summary>
        /// Sends a message to the Rainmeter log. Calls without plugin measure should be avoided as no source will be given
        /// </summary>
        /// <param name="type">Log level (LOG_ERROR, LOG_WARNING, LOG_NOTICE, or LOG_DEBUG). LOG_DEBUG messages are logged only when Rainmeter is in debug mod.</param>
        /// <param name="message">Message to be logged</param>
        public static void Log(LogType type, string message)
        {
            LSLog(type, null, message);
        }

        /// <summary>
        /// Sends a message to the Rainmeter log with source
        /// </summary>
        /// <param name="rm">Pointer to the plugin measure</param>
        /// <param name="level">Log level (LOG_ERROR, LOG_WARNING, LOG_NOTICE, or LOG_DEBUG). LOG_DEBUG messages are logged only when Rainmeter is in debug mod.</param>
        /// <param name="message">Message to be logged</param>m>
        public static void Log(IntPtr rm, LogType type, string message)
        {
            RmLog(rm, type, message);
        }

        /// <summary>
        /// Sends a message to the Rainmeter log, follows similar formating to printf
        /// </summary>
        /// <param name="rm">Pointer to the plugin measure</param>
        /// <param name="level">Log level (LOG_ERROR, LOG_WARNING, LOG_NOTICE, or LOG_DEBUG). LOG_DEBUG messages are logged only when Rainmeter is in debug mod.</param>
        /// <param name="format">Message to be logged, follows printf syntax</param>
        /// <param name="message">Specifier values</param>
        public static void LogF(IntPtr rm, LogType type, string format, params string[] message)
        {
            RmLogF(rm, type, format, message);
        }
    }

    /// <summary>
    /// Dummy attribute to mark method as exported for DllExporter.exe.
    /// </summary>
    [AttributeUsage(AttributeTargets.Method)]
    public class DllExport : Attribute
    {
        public DllExport()
        {
        }
    }
}
