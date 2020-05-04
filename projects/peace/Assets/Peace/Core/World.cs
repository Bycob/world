using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Peace
{
    public class World
    {
        internal IntPtr _handle;
        
        public static World CreateDemo()
        {
            return new World(createDemoWorld(""));
        }

        private World(IntPtr handle)
        {
            _handle = handle;
        }

        ~World()
        {
            freeWorld(_handle);
        }

        public World(String configFile)
        {
            _handle = createWorldFromFile(configFile);
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
        private static extern void freeWorld(IntPtr handle);
    }
}