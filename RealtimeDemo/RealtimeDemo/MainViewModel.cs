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
        private const int UpdateInterval = 20;

        private bool disposed;
        private readonly Timer timer;
        private readonly Stopwatch watch = new Stopwatch();
        private Thread readThread = new Thread(Update);
        private static PlotModel plot = new PlotModel();
        private static List<double> data = new List<double>();
        private static bool _continue = true;
        public PlotModel PlotModel { get; private set; }
        public double Average { get; private set; }


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

            plot.Axes.Add(new LinearAxis { Position = AxisPosition.Left });

            plot.Series.Add(new LineSeries { LineStyle = LineStyle.Solid });

            PlotModel = plot;
            Average = 0;

            SelectedPath = String.Empty;
            this.watch.Start();

            this.RaisePropertyChanged("PlotModel");

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
            PlotModel = plot;
            PlotModel.InvalidatePlot(true);

            double sum = 0;
            foreach (var d in data)
            {
                sum += d;
            }
            if (data.Count > 0)
            {
                Average = sum / data.Count;
            }
            else
            {
                Average = 0;
            }

            this.RaisePropertyChanged("Average");
        }

        private static void Update()
        {
            while (true)
            {
                if (_selectedPath != String.Empty)
                {
                    plot.Series[0] = new LineSeries { LineStyle = LineStyle.Solid }; //Remove old data

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
                            if (message != null)
                            {
                                rocketData data = ParseData(message);
                                var s = (LineSeries)plot.Series[0];
                                double x = s.Points.Count > 0 ? s.Points[s.Points.Count - 1].X + 1 : 0;
                                data.Add(Convert.ToDouble(message));
                                s.Points.Add(new DataPoint(x, Convert.ToDouble(message)));
                                plot.Series[0] = s;

                            }
                            else
                                wh.WaitOne(1000);
                        }
                    }
                    wh.Close();
                }
            }
            
        }

        public class rocketData
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

            public rocketData(int Min, int Sec, int Msec, int gx, int gy, int gz, double p, double t, int ax, int ay, int az)
            {
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

        private rocketData ParseData(string message)
        {
            rocketData parsedData = new rocketData();

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