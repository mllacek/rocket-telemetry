using GMap.NET;
using GMap.NET.MapProviders;
using GMap.NET.WindowsPresentation;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;

namespace MapsExample
{
    class MainViewModel : INotifyPropertyChanged, IDisposable
    {
        // try to change might be lower or higher than the rendering interval
        private const int UpdateInterval = 30;

        private bool disposed;
        private readonly DispatcherTimer timer;

        private static LocatorModel currentLocation;
        private static double distance;
        private static double bearing;

        private static bool _continue = true;
        private Thread readThread = new Thread(Update);

        private LocatorModel _currentLocation;
        public double Distance { get; private set; }
        public double Bearing { get; private set; }

        public GMapControl MainMap { get; private set; }

        public MainViewModel()
        {

            ButtonCommand = new RelayCommand(SelectFile);
            this.timer = new DispatcherTimer();
            this.timer.Tick += new EventHandler(OnTimerElapsed);
            this.SetupModel();
        }

        private void SetupModel()
        {
            SelectedPath = String.Empty;
            Distance = 0;
            Bearing = 0;

            // set cache mode only if no internet avaible
            //if (!Stuff.PingNetwork("pingtest.com"))
            //{
            //    MainMap.Manager.Mode = AccessMode.CacheOnly;
            //    MessageBox.Show("No internet connection available, going to CacheOnly mode.", "GMap.NET - Demo.WindowsPresentation", MessageBoxButton.OK, MessageBoxImage.Warning);
            //}

            // config map
            MainMap = new GMapControl();
            MainMap.MapProvider = GMapProviders.OpenStreetMap;
            MainMap.Manager.Mode = AccessMode.ServerAndCache;

            MainMap.MinZoom = 0;
            MainMap.MaxZoom = 24;
            MainMap.Zoom = 18;

            var lat2 = Nmea2DecDeg("4104.5567", "N");
            var long2 = Nmea2DecDeg("08130.8436", "W");
            MainMap.Position = new PointLatLng(lat2, long2);

            GMapMarker marker = new GMapMarker(new PointLatLng(0, 0));
            marker.ZIndex = -1;
            MainMap.Markers.Add(marker);

            this.timer.Interval = new TimeSpan(UpdateInterval);
            this.timer.Start();
            readThread.Start();
        }

        private ICommand _buttonCommand;
        public ICommand ButtonCommand
        {
            get { return _buttonCommand; }
            set { _buttonCommand = value; }
        }


        private static string _selectedPath;
        public string SelectedPath
        {
            get { return _selectedPath; }
            set { _selectedPath = value; _continue = false; } //TODO: check the selected file format
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

        private void SelectFile(object obj)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            if (openFileDialog.ShowDialog() == true)
            {
                SelectedPath = openFileDialog.FileName;
            }
        }

        private void OnTimerElapsed(object state, EventArgs e)
        {
            Distance = distance;
            Bearing = bearing;

            if (currentLocation != null)
            {
                CurrentLocation = currentLocation;
                MainMap.Position = CurrentLocation.RocketLocation;
                GMapMarker marker = new GMapMarker(CurrentLocation.RocketLocation);
                marker.ZIndex = -1;
                MainMap.Markers.Add(marker);

                MainMap.MinZoom = 0;
                MainMap.MaxZoom = 24;
                MainMap.Zoom = 18;

                //MainMap.InvalidateVisual();
            }

            this.RaisePropertyChanged("Distance");
            this.RaisePropertyChanged("Bearing");
            this.RaisePropertyChanged("CurrentLocation");
            this.RaisePropertyChanged("SelectedPath");
            this.RaisePropertyChanged("MainMap");
        }


        //From: http://www.drdobbs.com/windows/gps-programming-net/184405690
        // Converts NMEA formatted (DDMM.MMMMM) position (latitude or longitude)
        // to decimal degrees
        public static double Nmea2DecDeg(string NmeaLonLat, string Hemisphere)
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

        private static void Update()
        {
            while (true)
            {
                if (_selectedPath != String.Empty)
                {
                    var wh = new AutoResetEvent(false);
                    var fsw = new FileSystemWatcher(".");
                    fsw.Filter = _selectedPath;
                    fsw.EnableRaisingEvents = true;
                    fsw.Changed += (s, e) => wh.Set();
                    _continue = true;

                    var fs = new FileStream(_selectedPath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                    using (var sr = new StreamReader(fs))
                    {
                        var message = "";
                        while (_continue)
                        {
                            message = sr.ReadLine();
                            if (message != null && message[0] == '$')
                            {
                                RocketData rd = MainViewModel.ParseData(message);

                                int milliseconds = rd.Min * 60 + rd.Sec + rd.Msec / 1000;

                                //TODO: parse NMEA format? move to other place
                                var lat2 = Nmea2DecDeg("4104.5938", "N");
                                var long2 = Nmea2DecDeg("08130.7754", "W");

                                var lat1 = Nmea2DecDeg(rd.Lat.ToString(), rd.LatHemisphere.ToString());
                                var long1 = Nmea2DecDeg(rd.Long.ToString(), rd.LongHemisphere.ToString());

                                //var lat2 = Nmea2DecDeg("4104.5567", "N");
                                //var long2 = Nmea2DecDeg("08130.8436", "W");

                                LocatorModel locationData = new LocatorModel();
                                locationData.RocketLocation = new PointLatLng(lat1, long1);
                                locationData.UserLocation = new PointLatLng(lat2, long2);

                                //try
                                //{
                                //    double lat = Nmea2DecDeg("4104.5938", "N");
                                //    double lng = Nmea2DecDeg("08130.7754", "W");


                                //}
                                //catch (Exception ex)
                                //{
                                //    MessageBox.Show("incorrect coordinate format: " + ex.Message);
                                //}

                                distance = GMapProviders.EmptyProvider.Projection.GetDistance(locationData.UserLocation, locationData.RocketLocation) * 0.62137; //convert km to miles
                                bearing = GMapProviders.EmptyProvider.Projection.GetBearing(locationData.UserLocation, locationData.RocketLocation);

                                currentLocation = locationData;

                            }
                            else
                                wh.WaitOne(1000);
                        }
                    }
                    wh.Close();
                }
            }

        }

        public class RocketData
        {
            public int Min { get; set; }
            public int Sec { get; set; }
            public int Msec { get; set; }

            public double Lat { get; set; }
            public char LatHemisphere { get; set; }
            public double Long { get; set; }
            public char LongHemisphere { get; set; }

            public RocketData(int m, int s, int ms, double lat, char latHemi, double lon, char longHemi)
            {
                Min = m;
                Sec = s;
                Msec = ms;
                Lat = lat;
                LatHemisphere = latHemi;
                Long = lon;
                LongHemisphere = longHemi;
            }

        }

        //sample message format
        //$3:0.706,0,-3,-1,21.74,961.71,-96,-32,7856,4104.5615,N,08130.8343,W
        public static RocketData ParseData(string message)
        {
            var data = message.Split(',');
            var time = data[0].TrimStart(new char[] { '$' });

            if (!int.TryParse(time.Split(':')[0], out int Min))
                Min = 0;

            time = time.Split(':')[1];

            if (!int.TryParse(time.Split('.')[0], out int Sec))
                Sec = 0;

            if (!int.TryParse(time.Split('.')[1], out int Msec))
                Msec = 0;

            if (!double.TryParse(data[9], out double Lat))
                Lat = 0;
            //TODO: check if the length is long enough
            if (!char.TryParse(data[10], out char LatHemi))
                LatHemi = '\0';

            if (!double.TryParse(data[11], out double Long))
                Long = 0;

            if (!char.TryParse(data[12], out char LongHemi))
                LongHemi = '\0';

            //we can ignore the other data for this application

            RocketData parsedData = new RocketData(Min, Sec, Msec, Lat, LatHemi, Long, LongHemi);

            return parsedData;
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

        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            if (!this.disposed)
            {
                if (disposing)
                {
                    //this.timer.Dispose();
                }
            }

            this.disposed = true;
        }

    }
}
