using GMap.NET;
using GMap.NET.MapProviders;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;


namespace MapsExample
{
    class MainViewModel
    {
        private LocatorModel _currentLocation;
        private string _selectedPath;
        private double _distance;
        private double _bearing;
        private ICommand _selectFileCommand;

        public MainViewModel()
        {
            var lat1 = Nmea2DecDeg("4104.5938", "N");
            var long1 = Nmea2DecDeg("08130.7754", "W");

            var lat2 = Nmea2DecDeg("4104.5567", "N");
            var long2 = Nmea2DecDeg("08130.8436", "W");

            LocatorModel locationData = new LocatorModel();
            locationData.RocketLocation = new PointLatLng(lat1, long1);
            locationData.UserLocation = new PointLatLng(lat2, long2);

            Distance = GMapProviders.EmptyProvider.Projection.GetDistance(locationData.UserLocation, locationData.RocketLocation);
            Bearing = GMapProviders.EmptyProvider.Projection.GetBearing(locationData.UserLocation, locationData.RocketLocation);
        }

        public LocatorModel CurrentLocation
        {
            get { return _currentLocation; }
            set
            {
                if (value != _currentLocation)
                {
                    _currentLocation = value;
                    RaisePropertyChanged("CurrentLocation");
                }
            }
        }

        public string SelectedPath
        {
            get { return _selectedPath; }
            set
            {
                if (value != _selectedPath)
                {
                    _selectedPath = value;
                    RaisePropertyChanged("SelectedPath");
                }
            }
        }

        public double Distance
        {
            get { return _distance; }
            set
            {
                if (value != _distance)
                {
                    _distance = value;
                    RaisePropertyChanged("Distance");
                }
            }
        }

        public double Bearing
        {
            get { return _bearing; }
            set
            {
                if (value != _bearing)
                {
                    _bearing = value;
                    RaisePropertyChanged("Bearing");
                }
            }
        }

        public ICommand SelectFileCommand
        {
            get
            {
                if (_selectFileCommand == null)
                {
                    _selectFileCommand = new RelayCommand(param => SelectFile());
                }
                return _selectFileCommand;
            }
        }

        private void SelectFile()
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            if (openFileDialog.ShowDialog() == true)
            {
                SelectedPath = openFileDialog.FileName;
            }
        }

        //TODO: read the file as new data is added - get the latest GPS


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

        public event PropertyChangedEventHandler PropertyChanged;

        protected void RaisePropertyChanged(string property)
        {
            var handler = this.PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(property));
            }
        }

    }
}
