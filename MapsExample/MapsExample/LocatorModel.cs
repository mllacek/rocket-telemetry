using GMap.NET;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MapsExample
{
    public class LocatorModel
    {
        private PointLatLng _rocketLocation;
        private PointLatLng _userLocation;

        public PointLatLng RocketLocation
        {
            get { return _rocketLocation; }
            set
            {
                if (value != _rocketLocation)
                {
                    _rocketLocation = value;
                    RaisePropertyChanged("RocketLocation");
                }
            }
        }

        public PointLatLng UserLocation
        {
            get { return _userLocation; }
            set
            {
                if (value != _userLocation)
                {
                    _userLocation = value;
                    RaisePropertyChanged("UserLocation");
                }
            }
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
