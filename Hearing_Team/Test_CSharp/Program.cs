using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System;
using System.Runtime.InteropServices;     // DLL support

class HelloWorld
{
    [DllImport(@"..\..\..\Debug\TestDLL_C.dll")]
    public static extern void DisplayHelloFromDLL();

    static void Main()
    {
        Console.WriteLine("This is C# program");
        DisplayHelloFromDLL();
    }
}