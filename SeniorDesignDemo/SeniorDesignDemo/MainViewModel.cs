using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OxyPlot;
using OxyPlot.Axes;
using OxyPlot.Series;

namespace SeniorDesignDemo
{
    public class MainViewModel
    {
        public MainViewModel()
        {
            this.MyModel = new PlotModel { Title = "Function Series Example", PlotAreaBackground = OxyColor.FromRgb(204, 255, 255) };
            this.MyModel.Series.Add(new FunctionSeries(Math.Cos, 0, 10, 0.1, "cox(x)"));
            this.MyModel.Series.Add(new FunctionSeries(Math.Sin, 0, 10, 0.1, "sin(x)"));

            this.MySecondModel = new PlotModel { Title = "LineSeries Example", PlotAreaBackground = OxyColor.FromRgb(204, 255, 255) };

            //Add three data series
            for (var i = 0; i < 3; i++)
            {
                var series = new LineSeries
                {
                    Title = string.Format("Series {0}", i + 1),
                    CanTrackerInterpolatePoints = false
                };

                for (var j = 0; j < 10; j++)
                {
                    series.Points.Add(new DataPoint(j, r.NextDouble()));
                }

                MySecondModel.Series.Add(series);
            }

            this.MyThirdModel = new PlotModel { Title = "Altimeter Data", PlotAreaBackground = OxyColor.FromRgb(204, 255, 255) };
            MyThirdModel.Axes.Add(new LinearAxis { Position = AxisPosition.Bottom, Title = "Time (seconds)" });
            MyThirdModel.Axes.Add(new LinearAxis { Position = AxisPosition.Left, Title = "Altitude (feet)" });

            IEnumerable<FlightData>  data = ReadCSV("2018 NASA Subscale Launch Main Altimeter Data");
            var series2 = new LineSeries { Title = "Altimeter Data", CanTrackerInterpolatePoints = false };
            foreach (var d in data)
            {
                series2.Points.Add(new DataPoint(d.Time, d.Altitude));
            }

            MyThirdModel.Series.Add(series2);

        }

        public PlotModel MyModel { get; private set; }

        public PlotModel MySecondModel { get; private set; }

        public PlotModel MyThirdModel { get; private set; }

        private static Random r = new Random(13);

        public IEnumerable<FlightData> ReadCSV(string fileName)
        {
            string[] lines = File.ReadAllLines(System.IO.Path.ChangeExtension(fileName, ".csv"));

            return lines.Select(line =>
            {
                string[] data = line.Split(',');
                return new FlightData(Convert.ToDouble(data[0]), Convert.ToDouble(data[1]), Convert.ToDouble(data[2].TrimEnd('F')), Convert.ToDouble(data[3]));
            });
        }

    }
}
