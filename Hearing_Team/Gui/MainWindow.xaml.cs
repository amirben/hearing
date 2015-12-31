using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Runtime.InteropServices;     // DLL support

namespace Gui
{

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        [DllImport(@"..\..\..\Debug\C_Code.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void test(ref int value);


        public MainWindow()
        {
            InitializeComponent();
            //test
            
        }

        private void CSharp_button_Click(object sender, RoutedEventArgs e)
        {
            text.Text = "Click by C# code";
        }

        private void C_button_Click(object sender, RoutedEventArgs e)
        {
            int x = 0;
            test(ref x);
            
            if (x == 42)
            {
                text.Text = "This is a C code";
            }
            else
            {
                text.Text = "";
            }

        }
    }
}
