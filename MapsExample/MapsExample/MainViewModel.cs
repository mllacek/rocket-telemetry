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
