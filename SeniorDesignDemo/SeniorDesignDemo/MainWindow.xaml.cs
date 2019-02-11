using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.IO;
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

namespace SeniorDesignDemo
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            ListViewData.ItemsSource = ReadCSV("2018 NASA Subscale Launch Main Altimeter Data"); //TODO: create selection button
        }

        public IEnumerable<FlightData> ReadCSV(string fileName)
        {
            string[] lines = File.ReadAllLines(System.IO.Path.ChangeExtension(fileName, ".csv"));

            return lines.Select(line =>
            {
                string[] data = line.Split(',');
                return new FlightData(Convert.ToDouble(data[0]), Convert.ToDouble(data[1]), Convert.ToDouble(data[2].TrimEnd('F')), Convert.ToDouble(data[3]));
            });
        }

        private void btnOpenFile_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            //if (openFileDialog.ShowDialog() == true)
                //txtEditor.Text = File.ReadAllText(openFileDialog.FileName);
        }
    }
}
