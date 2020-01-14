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
            return new World(createDemoWorld("vulkan"));
        }

        private World(IntPtr handle)
        {
            _handle = handle;
        }

        public World(String configFile)
        {
            throw new NotSupportedException(
                "World can not yet read config file. This may be " + 
                "available in a later version of the library :)");
        }
        
        
        // Dll functions
        
        [DllImport("peace")]
        private static extern IntPtr createTestWorld();
        
        [DllImport("peace")]
        private static extern IntPtr createDemoWorld(string name);
    }
}