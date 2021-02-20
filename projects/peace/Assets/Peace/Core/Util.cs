using System;
using System.IO;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Peace
{
    [StructLayout(LayoutKind.Sequential)]
    public struct BBox
    {
        public double xmin, ymin, zmin;
        public double xmax, ymax, zmax;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct FirstPersonView
    {
        public double X, Y, Z;
        public double eyeResolution;
        public double maxDistance;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ZoneView
    {
        public BBox bbox;
        public double resolution;
    }

    public static class Util
    {
        public static string[] ToStringArray(IntPtr arrayPtr, int size)
        {
            IntPtr[] array = new IntPtr[size];
            Marshal.Copy(arrayPtr, array, 0, size);
            return ToStringArray(array);
        }

        public static string[] ToStringArray(IntPtr[] ptrArray)
        {
            string[] strArray = new string[ptrArray.Length];

            for (int i = 0; i < ptrArray.Length; ++i)
            {
                strArray[i] = Marshal.PtrToStringAnsi(ptrArray[i]);
            }

            return strArray;
        }

        /** Remove path recursively */
        public static void DeleteTree(DirectoryInfo path)
        {
            if (!path.Exists)
            {
                return;
            }

            foreach (var dir in path.EnumerateDirectories())
            {
                DeleteTree(dir);
            }
            path.Delete(true);
        }
    }
}