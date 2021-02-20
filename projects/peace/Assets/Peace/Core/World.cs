using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Peace.Serialization;
using UnityEngine;

namespace Peace
{
    public class World : IDisposable
    {
        internal IntPtr _handle;

        public static World CreateDemo(string demoId)
        {
            return new World(createDemoWorld(demoId));
        }

        private World(IntPtr handle)
        {
            _handle = handle;
        }

        ~World()
        {
            if (_handle != IntPtr.Zero)
                freeWorld(_handle);
        }

        public World(string configFile)
        {
            _handle = createWorldFromFile(configFile);
        }

        public World(WorldDef worldDef)
        {
            string jsonStr = WorldSerialization.ToJson(worldDef);
            _handle = createWorldFromJson(jsonStr);
        }

        public void Dispose()
        {
            if (_handle != IntPtr.Zero)
            {
                freeWorld(_handle);
                _handle = IntPtr.Zero;
            }
        }

        public void SetCacheLocation(string cacheLocation)
        {
            CheckAccess();
            setWorldCache(_handle, cacheLocation);
        }

        private void CheckAccess()
        {
            if (_handle == IntPtr.Zero)
            {
                throw new MethodAccessException("This object was deleted manually");
            }
        }



        // Dll functions

        [DllImport("peace")]
        private static extern IntPtr createTestWorld();

        [DllImport("peace")]
        private static extern IntPtr createDemoWorld(string name);

        [DllImport("peace")]
        private static extern IntPtr createWorldFromJson(string jsonStr);

        [DllImport("peace")]
        private static extern IntPtr createWorldFromFile(string name);

        [DllImport("peace")]
        private static extern void setWorldCache(IntPtr worldPtr, string cacheLocation);

        [DllImport("peace")]
        private static extern void freeWorld(IntPtr handle);
    }
}