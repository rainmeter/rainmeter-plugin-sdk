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

namespace PluginRmExecute
{
    class Measure
    {
        static public implicit operator Measure(IntPtr data)
        {
            return (Measure)GCHandle.FromIntPtr(data).Target;
        }
        public string myCommand = "";
        public int updateRate;
        public DateTime timer;

        public Rainmeter.API api;
    }

    public class Plugin
    {
        [DllExport]
        public static void Initialize(ref IntPtr data, IntPtr rm)
        {
            Measure measure = new Measure();
            Rainmeter.API api = (Rainmeter.API)rm;

            measure.api = api;

            //We do this in Initialize so that if a skin were to turn on DynamicVariables it will not break the plugin
            measure.timer = DateTime.UtcNow;

            data = GCHandle.ToIntPtr(GCHandle.Alloc(measure));
        }

        [DllExport]
        public static void Reload(IntPtr data, IntPtr rm, ref double maxValue)
        {
            Measure measure = (Measure)data;
            Rainmeter.API api = (Rainmeter.API)rm;

            measure.updateRate = api.ReadInt("Timer", 1);
            //We dont have to replace measures here as they will be replaced during Execute so we can pass false. 
            //Note though that doing that measures will always then have their current info in update but variables will not. See the commented out code below to have both always act like DynamicVariables=1
            measure.myCommand = api.ReadString("OnTimer", "", false);
        }

        [DllExport]
        public static double Update(IntPtr data)
        {
            Measure measure = (Measure)data;

            int timePassed = (int)(DateTime.UtcNow - measure.timer).TotalSeconds;

            if(timePassed >= measure.updateRate)
            {
                //We could also do something like this to get DynamicVariables without the need to turn them on. Uncomment this to see the second example work
                //measure.myCommand = measure.api.ReadString("OnTimer", "", false);

                measure.api.Execute(measure.myCommand);
                measure.timer = DateTime.UtcNow;
            }

            //Even if you don't plan for users to use it returning a value can be useful for helping with skin debugging
            return measure.updateRate - timePassed;
        }

        [DllExport]
        public static void Finalize(IntPtr data)
        {
            GCHandle.FromIntPtr(data).Free();
        }
    }
}
