using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SeniorDesignDemo
{
    public class FlightData
    {
        public double Time { get; set; }
        public double Altitude { get; set; }
        public double Temp { get; set; }
        public double Voltage { get; set; }

        public FlightData (double time, double alt, double temp, double voltage)
        {
            Time = time;
            Altitude = alt;
            Temp = temp;
            Voltage = voltage;
        }

    }

}
