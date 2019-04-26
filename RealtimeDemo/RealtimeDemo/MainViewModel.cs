// --------------------------------------------------------------------------------------------------------------------
// <copyright file="MainViewModel.cs" company="OxyPlot">
//   Copyright (c) 2014 OxyPlot contributors
// </copyright>
// --------------------------------------------------------------------------------------------------------------------

namespace RealtimeDemo
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.IO;
    using System.Threading;
    using System.Windows.Input;
    using Microsoft.Win32;
    using OxyPlot;
    using OxyPlot.Axes;
    using OxyPlot.Series;

    public class MainViewModel : INotifyPropertyChanged, IDisposable
    {
        // try to change might be lower or higher than the rendering interval
        private const int UpdateInterval = 300;

        private bool disposed;
        private readonly Timer timer;
        private readonly Stopwatch watch = new Stopwatch();
        private Thread readThread = new Thread(Update);
        private static PlotModel tempPlot = new PlotModel();
        private static PlotModel pressurePlot = new PlotModel();
        private static PlotModel altPlot = new PlotModel();
        private static PlotModel accelXPlot = new PlotModel();
        private static PlotModel accelYPlot = new PlotModel();
        private static PlotModel accelZPlot = new PlotModel();
        private static bool _continue = true;
        public PlotModel TempPlotModel { get; private set; }
        public PlotModel PressurePlotModel { get; private set; }
        public PlotModel AltPlotModel { get; private set; }
        public PlotModel AccelXPlotModel { get; private set; }
        public PlotModel AccelYPlotModel { get; private set; }
        public PlotModel AccelZPlotModel { get; private set; }
        public static double tempSum = 0;
        public static double pressureSum = 0;
        public static double maxAlt = 0;
        public static int count = 0;
        public double TempAverage { get; private set; }
        public double PressureAverage { get; private set; }
        public double MaxAltitude { get; private set; }

        public static double P0 = 0; //reference pressure


        public MainViewModel()
        {
            ButtonCommand = new RelayCommand(SelectFile);
            this.timer = new Timer(OnTimerElapsed);
            this.SetupModel();
        }

        private void SelectFile(object obj)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            if (openFileDialog.ShowDialog() == true)
            {
                SelectedPath = openFileDialog.FileName;
                P0 = 0; //reset reference temp
                maxAlt = 0;
                MaxAltitude = 0;
            }
        }

        private void SetupModel()
        {
            this.timer.Change(Timeout.Infinite, Timeout.Infinite);

            tempPlot.Axes.Add(new LinearAxis { Position = AxisPosition.Left, Title="Temperature (degrees C)" });
            tempPlot.Axes.Add(new LinearAxis { Position = AxisPosition.Bottom, Title = "Time (sec)" });
            tempPlot.Series.Add(new LineSeries { LineStyle = LineStyle.Solid});
            tempPlot.DefaultColors = new List<OxyColor> { OxyColors.IndianRed };
            TempPlotModel = tempPlot;

            pressurePlot.Axes.Add(new LinearAxis { Position = AxisPosition.Left, Title = "Pressure (mBar)" });
            pressurePlot.Axes.Add(new LinearAxis { Position = AxisPosition.Bottom, Title = "Time (sec)" });
            pressurePlot.Series.Add(new LineSeries { LineStyle = LineStyle.Solid });
            pressurePlot.DefaultColors = new List<OxyColor> { OxyColors.CornflowerBlue };
            PressurePlotModel = pressurePlot;

            altPlot.Axes.Add(new LinearAxis { Position = AxisPosition.Left, Title = "Altitude (ft)" });
            altPlot.Axes.Add(new LinearAxis { Position = AxisPosition.Bottom, Title = "Time (sec)" });
            altPlot.Series.Add(new LineSeries { LineStyle = LineStyle.Solid });
            AltPlotModel = altPlot;

            accelXPlot.Axes.Add(new LinearAxis { Position = AxisPosition.Left, Title = "Acceleration X (g)" });
            accelXPlot.Axes.Add(new LinearAxis { Position = AxisPosition.Bottom, Title = "Time (sec)" });
            accelXPlot.Series.Add(new LineSeries { LineStyle = LineStyle.Solid });
            accelXPlot.DefaultColors = new List<OxyColor> { OxyColors.Purple };
            AccelXPlotModel = accelXPlot;

            //accelYPlot.Axes.Add(new LinearAxis { Position = AxisPosition.Left, Title = "Acceleration Y (g)" });
            //accelYPlot.Axes.Add(new LinearAxis { Position = AxisPosition.Bottom, Title = "Time (sec)" });
            //accelYPlot.Series.Add(new LineSeries { LineStyle = LineStyle.Solid });
            //accelYPlot.DefaultColors = new List<OxyColor> { OxyColors.Purple };
            //AccelYPlotModel = accelYPlot;

            //accelZPlot.Axes.Add(new LinearAxis { Position = AxisPosition.Left, Title = "Acceleration Z (g)" });
            //accelZPlot.Axes.Add(new LinearAxis { Position = AxisPosition.Bottom, Title = "Time (sec)" });
            //accelZPlot.Series.Add(new LineSeries { LineStyle = LineStyle.Solid });
            //accelZPlot.DefaultColors = new List<OxyColor> { OxyColors.Purple };
            //AccelZPlotModel = accelZPlot;

            TempAverage = 0;
            PressureAverage = 0;
            MaxAltitude = 0;

            SelectedPath = String.Empty;
            this.watch.Start();

            this.RaisePropertyChanged("TempPlotModel");
            this.RaisePropertyChanged("PressurePlotModel");
            this.RaisePropertyChanged("AltPlotModel");
            //this.RaisePropertyChanged("AccelXPlotModel");
            //this.RaisePropertyChanged("AccelYPlotModel");
            //this.RaisePropertyChanged("AccelZPlotModel");
            this.RaisePropertyChanged("MaxAltitude");

            this.timer.Change(1000, UpdateInterval);

            readThread.Start();
        }

        private ICommand _buttonCommand;
        public ICommand ButtonCommand
        {
            get { return _buttonCommand;  }
            set { _buttonCommand = value;}
        }

 
        private static string _selectedPath;
        public string SelectedPath
        {
            get { return _selectedPath; }
            set { _selectedPath = value; _continue = false; } //TODO: check the selected file format
        }

        private void OnTimerElapsed(object state)
        {
            TempPlotModel = tempPlot;
            TempPlotModel.InvalidatePlot(true);

            PressurePlotModel = pressurePlot;
            PressurePlotModel.InvalidatePlot(true);

            AltPlotModel = altPlot;
            AltPlotModel.InvalidatePlot(true);

            AccelXPlotModel = accelXPlot;
            AccelXPlotModel.InvalidatePlot(true);

            //AccelYPlotModel = accelYPlot;
            //AccelYPlotModel.InvalidatePlot(true);

            //AccelZPlotModel = accelZPlot;
            //AccelZPlotModel.InvalidatePlot(true);

            //TempAverage = 0;
            //PressureAverage = 0;
            MaxAltitude = maxAlt;

            if (count > 0)
            {
                TempAverage = tempSum / count;
                PressureAverage = pressureSum / count;
            }

            this.RaisePropertyChanged("TempAverage");
            this.RaisePropertyChanged("PressureAverage");
            this.RaisePropertyChanged("MaxAltitude");
        }

        private static void Update()
        {
            while (true)
            {
                if (_selectedPath != String.Empty)
                {
                    tempPlot.Series[0] = new LineSeries { LineStyle = LineStyle.Solid}; //Remove old data
                    pressurePlot.Series[0] = new LineSeries { LineStyle = LineStyle.Solid};
                    altPlot.Series[0] = new LineSeries { LineStyle = LineStyle.Solid };
                    accelXPlot.Series[0] = new LineSeries { LineStyle = LineStyle.Solid };
                    //accelYPlot.Series[0] = new LineSeries { LineStyle = LineStyle.Solid };
                    //accelZPlot.Series[0] = new LineSeries { LineStyle = LineStyle.Solid };

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
                                var tempSeries = (LineSeries)tempPlot.Series[0];
                                var pressureSeries = (LineSeries)pressurePlot.Series[0];
                                var altitudeSeries = (LineSeries)altPlot.Series[0];
                                var accelXSeries = (LineSeries)accelXPlot.Series[0];
                                //var accelYSeries = (LineSeries)accelYPlot.Series[0];
                                //var accelZSeries = (LineSeries)accelZPlot.Series[0];

                                int milliseconds = rd.Min * 60 + rd.Sec + rd.Msec/1000;

                                count++;
                                
                                tempSum += rd.Temp;
                                pressureSum += rd.Pressure;

                                //h - meters
                                //p - pressure at altitude (need units in inHg)
                                double pressureHg = rd.Pressure * 0.02952998751; //convert mBar to inHg

                                if (P0 == 0)
                                    P0 = pressureHg; //set first reference pressure

                                //P0 is the pressure at the reference level h0
                                //double P0 = 29.6; // inHg
                                //double h0 = 307.848; //meters % Akron
                                double h0 = 0; //TODO: try detecting altitude change for now
                                //convert C to Kelvin
                                //T = Temperature at altitude h(need units in Kelvin) 
                                double tempKelvin = rd.Temp + 273.15;

                                const double g = 9.80665; //m / s ^ 2(Gravitational Acceleration Due To Earth)
                                const double M = 0.0289644; //kg / mol % M is the molar mass of Air
                                const double R = 8.31432; //N * m / (mol * K) % Univeral Gas Constant

                                //Barometric Formula:
                                //P = P0 * exp(-g * M * (h - h0) / (R * tempKelvin));

                                double h = 0;
                                if (P0 != 0 && pressureHg/P0 > 0) //TODO: I think this can only pick up on altitude changes....
                                    h = ((Math.Log(pressureHg / P0)) * (R * tempKelvin) / (-g * M))*3.28084 + h0;

                                tempSeries.Points.Add(new DataPoint(milliseconds, rd.Temp));
                                tempPlot.Series[0] = tempSeries;

                                pressureSeries.Points.Add(new DataPoint(milliseconds, rd.Pressure));
                                pressurePlot.Series[0] = pressureSeries;

                                altitudeSeries.Points.Add(new DataPoint(milliseconds, h)); //convert to feet
                                altPlot.Series[0] = altitudeSeries;

                                accelXSeries.Points.Add(new DataPoint(milliseconds, rd.AccelX));
                                accelXPlot.Series[0] = accelXSeries;

                                //accelYSeries.Points.Add(new DataPoint(milliseconds, rd.AccelY));
                                //accelYPlot.Series[0] = accelYSeries;

                                //accelZSeries.Points.Add(new DataPoint(milliseconds, rd.AccelZ));
                                //accelZPlot.Series[0] = accelZSeries;

                                if (h > maxAlt)
                                    maxAlt = h;

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

            public int GyroX { get; set; }
            public int GyroY { get; set; }
            public int GyroZ { get; set; }

            public double Pressure { get; set; }
            public double Temp { get; set; }

            public int AccelX { get; set; }
            public int AccelY { get; set; }
            public int AccelZ { get; set; }

            public RocketData(int m, int s, int ms, int gx, int gy, int gz, double p, double t, int ax, int ay, int az)
            {
                Min = m;
                Sec = s;
                Msec = ms;
                GyroX = gx;
                GyroY = gy;
                GyroZ = gz;
                Pressure = p;
                Temp = t;
                AccelX = ax;
                AccelY = ay;
                AccelZ = az;
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

            if (!int.TryParse(data[1], out int GyroX))
                GyroX = 0;

            if (!int.TryParse(data[2], out int GyroY))
                GyroY = 0;

            if (!int.TryParse(data[3], out int GyroZ))
                GyroZ = 0;

            if (!double.TryParse(data[4], out double Temp))
                Temp = 0.0;

            if (!double.TryParse(data[5], out double Pressure))
                Pressure = 0.0;

            if (!int.TryParse(data[6], out int AccelX))
                AccelX = 0;

            if (!int.TryParse(data[7], out int AccelY))
                AccelY = 0;

            if (!int.TryParse(data[8], out int AccelZ))
                AccelZ = 0;

            //we can ignore the gps data for this application

            RocketData parsedData = new RocketData(Min, Sec, Msec, GyroX, GyroY, GyroZ, Pressure, Temp, AccelX, AccelY,AccelZ);

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
                    this.timer.Dispose();
                }
            }

            this.disposed = true;
        }
    }
}