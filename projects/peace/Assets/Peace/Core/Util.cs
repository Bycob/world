using System;
using System.Runtime.InteropServices;

namespace Peace
{
    public class Util
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
    }
}