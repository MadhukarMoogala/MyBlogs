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
using System.Windows.Shapes;
namespace PWDProtectionPlot
{
    public partial class PasswordWindow : Window
    {
        public PasswordWindow()
        {
            InitializeComponent();
        }
        /*Password Changed Handler*/
        private void passwordBox_PasswordChanged(object sender, RoutedEventArgs e)
        {
            if (passwordBox.Password.Length == 8)
                passwordBox.PasswordChanged -= passwordBox_PasswordChanged;
        }
        /*We 'll close our password window when Enter key is pressed */
        private void passwordBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter) if (passwordBox.Password.Length < 8)
                    MessageBox.Show("Password Should be of \"Eight\"(8) Characters");
                else
                {
                    e.Handled = true;
                    this.Close();
                }
        }
        /*We'll trigger password changed handler when we get focus*/
        private void passwordBox_GotFocus(object sender, RoutedEventArgs e)
        {
            passwordBox.PasswordChanged += passwordBox_PasswordChanged;
        }
    }
}