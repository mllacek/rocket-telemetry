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
using GMap.NET;
using GMap.NET.MapProviders;

namespace MapsExample
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            var lat1 = Nmea2DecDeg("4104.5938", "N");
            var long1 = Nmea2DecDeg("08130.7754", "W");

            var lat2 = Nmea2DecDeg("4104.5567", "N");
            var long2 = Nmea2DecDeg("08130.8436", "W");
            PointLatLng pt1 = new PointLatLng(lat1, long1);
            PointLatLng pt2 = new PointLatLng(lat2, long2);

            double distance = GMapProviders.EmptyProvider.Projection.GetDistance(pt1, pt2);
            double bearing = GMapProviders.EmptyProvider.Projection.GetBearing(pt1, pt2);

            //GMap.NET.PureProjection.

            this.DataContext = new MainViewModel();
        }

        //From: http://www.drdobbs.com/windows/gps-programming-net/184405690
        // Converts NMEA formatted (DDMM.MMMMM) position (latitude or longitude)
        // to decimal degrees
        public double Nmea2DecDeg(string NmeaLonLat, string Hemisphere)
        {
            int inx = NmeaLonLat.IndexOf(".");
            if (inx == -1)
            {
                return 0;    // Invalid syntax
            }
            string minutes_str = NmeaLonLat.Substring(inx - 2);
            double minutes = Double.Parse(minutes_str, new
                      System.Globalization.CultureInfo("en-US"));
            string degrees_str = NmeaLonLat.Substring(0, inx - 2);
            double degrees = Convert.ToDouble(degrees_str) + minutes / 60.0;
            if (Hemisphere.Equals("W") || Hemisphere.Equals("S"))
            {
                degrees = -degrees;

            }
            return degrees;
        }
    }
}
