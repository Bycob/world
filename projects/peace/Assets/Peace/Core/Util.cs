using System;
using System.Runtime.InteropServices;

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

        // Dll functions
        [DllImport("peace")]
        // TODO change [In, Out] to read only
        private static extern void freePtrs(int size, [In, Out] IntPtr[] array);
    }
}