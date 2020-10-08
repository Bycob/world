using System;
using System.Runtime.InteropServices;

namespace Peace
{
    [StructLayout(LayoutKind.Sequential)]
    struct BBox
    {
        public double xmin, ymin, zmin;
        public double xmax, ymax, zmax;
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

        public static void FreePtrs(IntPtr[] array)
        {
            freePtrs(array.Length, array);
        }


        // Dll functions
        // C-like free
        [DllImport("peace")]
        // TODO change [In, Out] to read only
        private static extern void freePtrs(int size, [In, Out] IntPtr[] array);
    }
}