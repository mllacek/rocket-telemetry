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
        private const int UpdateInterval = 30;

        private bool disposed;
        private readonly Timer timer;
        private readonly Stopwatch watch = new Stopwatch();
        private Thread readThread = new Thread(Update);
        private static PlotModel tempPlot = new PlotModel();
        private static PlotModel pressurePlot = new PlotModel();
        private static bool _continue = true;
        public PlotModel TempPlotModel { get; private set; }
        public PlotModel PressurePlotModel { get; private set; }
        public static double tempSum = 0;
        public static double pressureSum = 0;
        public static int count = 0;
        public double TempAverage { get; private set; }
        public double PressureAverage { get; private set; }


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
            }
        }

        private void SetupModel()
        {
            this.timer.Change(Timeout.Infinite, Timeout.Infinite);

            tempPlot.Axes.Add(new LinearAxis { Position = AxisPosition.Left, Title="Temperature (degrees C)" });
            tempPlot.Axes.Add(new LinearAxis { Position = AxisPosition.Bottom, Title = "Time (sec)" });
            tempPlot.Series.Add(new LineSeries { LineStyle = LineStyle.Solid});
            TempPlotModel = tempPlot;

            pressurePlot.Axes.Add(new LinearAxis { Position = AxisPosition.Left, Title = "Pressure (mBar)" });
            pressurePlot.Axes.Add(new LinearAxis { Position = AxisPosition.Bottom, Title = "Time (sec)" });
            pressurePlot.Series.Add(new LineSeries { LineStyle = LineStyle.Solid });
            PressurePlotModel = pressurePlot;

            TempAverage = 0;
            PressureAverage = 0;

            SelectedPath = String.Empty;
            this.watch.Start();

            this.RaisePropertyChanged("TempPlotModel");
            this.RaisePropertyChanged("PressurePlotModel");

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

            TempAverage = 0;
            PressureAverage = 0;

            if (count > 0)
            {
                TempAverage = tempSum / count;
                PressureAverage = pressureSum / count;
            }


            this.RaisePropertyChanged("TempAverage");
            this.RaisePropertyChanged("PressureAverage");
        }

        private static void Update()
        {
            while (true)
            {
                if (_selectedPath != String.Empty)
                {
                    tempPlot.Series[0] = new LineSeries { LineStyle = LineStyle.Solid}; //Remove old data
                    pressurePlot.Series[0] = new LineSeries { LineStyle = LineStyle.Solid };

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

                                int milliseconds = rd.Min * 60 + rd.Sec + rd.Msec/1000;

                                count++;
                                tempSum += rd.Temp;
                                pressureSum += rd.Pressure;

                                tempSeries.Points.Add(new DataPoint(milliseconds, rd.Temp));
                                tempPlot.Series[0] = tempSeries;

                                pressureSeries.Points.Add(new DataPoint(milliseconds, rd.Pressure));
                                pressurePlot.Series[0] = pressureSeries;

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