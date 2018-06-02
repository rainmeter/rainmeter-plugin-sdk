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

using System;
using System.Runtime.InteropServices;
using Rainmeter;

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

namespace PluginSectionVariables
{
    class Measure
    {
        static public implicit operator Measure(IntPtr data)
        {
            return (Measure)GCHandle.FromIntPtr(data).Target;
        }
        public string inputStr; //The string returned in GetString is stored here
        public IntPtr buffer; //Prevent marshalAs from causing memory leaks by clearing this before assigning
    }

    public class Plugin
    {
        [DllExport]
        public static void Initialize(ref IntPtr data, IntPtr rm)
        {
            data = GCHandle.ToIntPtr(GCHandle.Alloc(new Measure()));
            Rainmeter.API api = (Rainmeter.API)rm;
        }

        [DllExport]
        public static void Reload(IntPtr data, IntPtr rm, ref double maxValue)
        {
            Measure measure = (Measure)data;
            Rainmeter.API api = (Rainmeter.API)rm;

            //Read measure for an Input string
            measure.inputStr = api.ReadString("Input", "");
        }

        [DllExport]
        public static double Update(IntPtr data)
        {
            Measure measure = (Measure)data;
            return 0.0;
        }

        [DllExport]
        public static IntPtr GetString(IntPtr data)
        {
            Measure measure = (Measure)data;
            if (measure.buffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(measure.buffer);
                measure.buffer = IntPtr.Zero;
            }

            measure.buffer = Marshal.StringToHGlobalUni(measure.inputStr);
            return measure.buffer;
        }

        [DllExport]
        public static IntPtr ToUpper(IntPtr data, int argc,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr, SizeParamIndex = 1)] string[] argv)
        {
            Measure measure = (Measure)data;
            if (measure.buffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(measure.buffer);
                measure.buffer = IntPtr.Zero;
            }

            //If we are given one or more arguments convert to uppercase the first one
            if (argc > 0)
            {
                measure.buffer = Marshal.StringToHGlobalUni(argv[0].ToUpper());
            }
            //If we are given no arguments  convert to uppercase the string we recived with the input option
            else
            {
                measure.buffer = Marshal.StringToHGlobalUni(measure.inputStr.ToUpper());
            }
            return measure.buffer;
        }

        [DllExport]
        public static IntPtr ToLower(IntPtr data, int argc,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr, SizeParamIndex = 1)] string[] argv)
        {
            Measure measure = (Measure)data;
            if (measure.buffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(measure.buffer);
                measure.buffer = IntPtr.Zero;
            }

            //If we are given one or more arguments convert to uppercase the first one
            if (argc > 0)
            {
                measure.buffer = Marshal.StringToHGlobalUni(argv[0].ToUpper());
            }
            //If we are given no arguments  convert to uppercase the string we recived with the input option
            else
            {
                measure.buffer = Marshal.StringToHGlobalUni(measure.inputStr.ToLower());
            }
            return measure.buffer;
        }

        [DllExport]
        public static void Finalize(IntPtr data)
        {
            Measure measure = (Measure)data;
            if (measure.buffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(measure.buffer);
            }
            GCHandle.FromIntPtr(data).Free();
        }
    }
}
