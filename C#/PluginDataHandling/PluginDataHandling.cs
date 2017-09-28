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
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Rainmeter;

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

namespace PluginDataHandling
{
    class Measure
    {
        static public implicit operator Measure(IntPtr data)
        {
            return (Measure)GCHandle.FromIntPtr(data).Target;
        }
        public int myCounter = 0;
        public bool storeData = false;
    }

    public class Plugin
    {
        //Used for reading and writing values from the rainmeter settings file
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
        static extern int GetPrivateProfileString(string section, string key, string defaultValue,
            [In, Out] char[] value, int size, string filePath);

        [DllImport("kernel32", CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool WritePrivateProfileString(string section, string key,
            string value, string filePath);

        static string PluginName = "DataHandling"; //Always match this to your plugin name
        static string KeyName = "StoredCount"; //You can have multiple keys for your plugin that you store different info under
        const int MAXSIZE = 256;

        [DllExport]
        public static void Initialize(ref IntPtr data, IntPtr rm)
        {
            data = GCHandle.ToIntPtr(GCHandle.Alloc(new Measure()));
        }

        [DllExport]
        public static void Reload(IntPtr data, IntPtr rm, ref double maxValue)
        {
            Measure measure = (Measure)data;
            Rainmeter.API api = (Rainmeter.API)rm;

            //Get starting value if one is defined
            int startValue = api.ReadInt("StartingValue", -1);

            //If string from option was null read from file
            if (startValue == -1)
            {
                char[] outString = new char[MAXSIZE];

                //Read from Rainmeter.data file, if no instace exists start at 0
                GetPrivateProfileString(PluginName, KeyName, "0", outString, MAXSIZE, API.GetSettingsFile());

                try
                {
                    //Need try catch just in case someone tampers with the file and the value stored is not a number
                    string intString = new string(outString);
                    measure.myCounter = Convert.ToInt32(intString);
                }
                catch
                {
                    api.Log(API.LogType.Error, "Error converting value stored in Rainmeter.data to integer");
                }
            }
            else
            {
                measure.myCounter = startValue;
            }

            //If store data is 1 then save data when measure is unloaded
            measure.storeData = api.ReadInt("StoreData", 0) == 1 ? true : false;
        }

        [DllExport]
        public static double Update(IntPtr data)
        {
            Measure measure = (Measure)data;

            return ++measure.myCounter;
        }

        [DllExport]
        public static void Finalize(IntPtr data)
        {
            Measure measure = (Measure)data;

            //Note that in this example if multiple measures have store data set to true whichever unloads last's data will persist
            if (measure.storeData)
            {
                WritePrivateProfileString(PluginName, KeyName, measure.myCounter.ToString(), API.GetSettingsFile());
            }

            GCHandle.FromIntPtr(data).Free();
        }
    }
}
