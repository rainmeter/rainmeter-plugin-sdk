using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Rainmeter;

// Overview: This is a blank canvas on which to build your plugin.

// Note: Measure.GetString, Plugin.GetString, Measure.ExecuteBang, and
// Plugin.ExecuteBang have been commented out. If you need GetString
// and/or ExecuteBang and you have read what they are used for from the
// SDK docs, uncomment the function(s). Otherwise leave them commented out
// (or get rid of them)!

namespace PluginEmpty
{
    internal class Measure
    {
        internal Measure()
        {
        }

        internal void Reload(Rainmeter.API api, ref double maxValue)
        {
        }

        internal double Update()
        {
            return 0.0;
        }

        //internal string GetString()
        //{
        //    return "";
        //}

        //internal void ExecuteBang(string args)
        //{
        //}
    }

    public static class Plugin
    {
        [DllExport]
        public unsafe static void Initialize(void** data, void* rm)
        {
            *data = (void*)GCHandle.ToIntPtr(GCHandle.Alloc(new Measure()));
        }

        [DllExport]
        public unsafe static void Finalize(void* data)
        {
            GCHandle.FromIntPtr((IntPtr)data).Free();
        }

        [DllExport]
        public unsafe static void Reload(void* data, void* rm, double* maxValue)
        {
            Measure measure = (Measure)GCHandle.FromIntPtr((IntPtr)data).Target;
            measure.Reload(new Rainmeter.API((IntPtr)rm), ref *maxValue);
        }

        [DllExport]
        public unsafe static double Update(void* data)
        {
            Measure measure = (Measure)GCHandle.FromIntPtr((IntPtr)data).Target;
            return measure.Update();
        }

        //[DllExport]
        //public unsafe static char* GetString(void* data)
        //{
        //    Measure measure = (Measure)GCHandle.FromIntPtr((IntPtr)data).Target;
        //    fixed (char* s = measure.GetString()) return s;
        //}

        //[DllExport]
        //public unsafe static void ExecuteBang(void* data, char* args)
        //{
        //    Measure measure = (Measure)GCHandle.FromIntPtr((IntPtr)data).Target;
        //    measure.ExecuteBang(new string(args));
        //}
    }
}
