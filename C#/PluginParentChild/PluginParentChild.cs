/*
  Copyright (C) 2014 Birunthan Mohanathas

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

// Overview: This example demonstrates a basic implementation of a parent/child
// measure structure. In this particular example, we have a "parent" measure
// which contains the values for the options "ValueA", "ValueB", and "ValueC".
// The child measures are used to return a specific value from the parent.

// Use case: You could, for example, have a "main" parent measure that queries
// information some data set. The child measures can then be used to return
// specific information from the data queried by the parent measure.

// Sample skin:
/*
    [Rainmeter]
    Update=1000
    BackgroundMode=2
    SolidColor=000000

    [mParent]
    Measure=Plugin
    Plugin=ParentChild.dll
    ValueA=111
    ValueB=222
    ValueC=333
    Type=A

    [mChild1]
    Measure=Plugin
    Plugin=ParentChild.dll
    ParentName=mParent
    Type=B

    [mChild2]
    Measure=Plugin
    Plugin=ParentChild.dll
    ParentName=mParent
    Type=C

    [Text]
    Meter=STRING
    MeasureName=mParent
    MeasureName2=mChild1
    MeasureName3=mChild2
    X=5
    Y=5
    W=200
    H=55
    FontColor=FFFFFF
    Text="mParent: %1#CRLF#mChild1: %2#CRLF#mChild2: %3"
*/

namespace PluginParentChild
{
    internal class Measure
    {
        internal enum MeasureType
        {
            A,
            B,
            C
        }

        internal MeasureType Type = MeasureType.A;

        internal virtual void Dispose()
        {
        }

        internal virtual void Reload(Rainmeter.API api, ref double maxValue)
        {
            string type = api.ReadString("Type", "");
            switch (type.ToLowerInvariant())
            {
                case "a":
                    Type = MeasureType.A;
                    break;

                case "b":
                    Type = MeasureType.B;
                    break;

                case "c":
                    Type = MeasureType.C;
                    break;

                default:
                    api.Log(API.LogType.Error, "ParentChild.dll: Type=" + type + " not valid");
                    break;
            }
        }

        internal virtual double Update()
        {
            return 0.0;
        }
    }

    internal class ParentMeasure : Measure
    {
        // This list of all parent measures is used by the child measures to find their parent.
        internal static List<ParentMeasure> ParentMeasures = new List<ParentMeasure>();

        internal string Name;
        internal IntPtr Skin;

        internal int ValueA;
        internal int ValueB;
        internal int ValueC;

        internal ParentMeasure()
        {
            ParentMeasures.Add(this);
        }

        internal override void Dispose()
        {
            ParentMeasures.Remove(this);
        }

        internal override void Reload(Rainmeter.API api, ref double maxValue)
        {
            base.Reload(api, ref maxValue);

            Name = api.GetMeasureName();
            Skin = api.GetSkin();

            ValueA = api.ReadInt("ValueA", 0);
            ValueB = api.ReadInt("ValueB", 0);
            ValueC = api.ReadInt("ValueC", 0);
        }

        internal override double Update()
        {
            return GetValue(Type);
        }

        internal double GetValue(MeasureType type)
        {
            switch (type)
            {
                case MeasureType.A:
                    return ValueA;

                case MeasureType.B:
                    return ValueB;

                case MeasureType.C:
                    return ValueC;
            }

            return 0.0;
        }
    }

    internal class ChildMeasure : Measure
    {
        private ParentMeasure ParentMeasure = null;

        internal override void Reload(Rainmeter.API api, ref double maxValue)
        {
            base.Reload(api, ref maxValue);

            string parentName = api.ReadString("ParentName", "");
            IntPtr skin = api.GetSkin();

            // Find parent using name AND the skin handle to be sure that it's the right one.
            ParentMeasure = null;
            foreach (ParentMeasure parentMeasure in ParentMeasure.ParentMeasures)
            {
                if (parentMeasure.Skin.Equals(skin) && parentMeasure.Name.Equals(parentName))
                {
                    ParentMeasure = parentMeasure;
                }
            }

            if (ParentMeasure == null)
            {
                api.Log(API.LogType.Error, "ParentChild.dll: ParentName=" + parentName + " not valid");
            }
        }

        internal override double Update()
        {
            if (ParentMeasure != null)
            {
                return ParentMeasure.GetValue(Type);
            }

            return 0.0;
        }
    }

    public static class Plugin
    {
        [DllExport]
        public static void Initialize(ref IntPtr data, IntPtr rm)
        {
            Rainmeter.API api = new Rainmeter.API(rm);

            string parent = api.ReadString("ParentName", "");
            Measure measure;
            if (String.IsNullOrEmpty(parent))
            {
                measure = new ParentMeasure();
            }
            else
            {
                measure = new ChildMeasure();
            }

            data = GCHandle.ToIntPtr(GCHandle.Alloc(measure));
        }

        [DllExport]
        public static void Finalize(IntPtr data)
        {
            Measure measure = (Measure)GCHandle.FromIntPtr(data).Target;
            measure.Dispose();
            GCHandle.FromIntPtr(data).Free();
        }

        [DllExport]
        public static void Reload(IntPtr data, IntPtr rm, ref double maxValue)
        {
            Measure measure = (Measure)GCHandle.FromIntPtr(data).Target;
            measure.Reload(new Rainmeter.API(rm), ref maxValue);
        }

        [DllExport]
        public static double Update(IntPtr data)
        {
            Measure measure = (Measure)GCHandle.FromIntPtr(data).Target;
            return measure.Update();
        }
    }
}
