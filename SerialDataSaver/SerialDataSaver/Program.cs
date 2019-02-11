using System;
using System.IO;
using System.IO.Ports;
using System.Threading;

public class SerialDataSaver
{

    public static void Main()
    {
        string message;

        SerialPort serialPort = new SerialPort();

        serialPort.PortName = SetPortName(serialPort.PortName);

        serialPort.BaudRate = 9600;
        serialPort.Parity = Parity.None;
        serialPort.DataBits = 8;
        serialPort.StopBits = StopBits.One;
        serialPort.Handshake = Handshake.None;

        // Set the read/write timeouts
        serialPort.ReadTimeout = 500;
        serialPort.WriteTimeout = 500;

        serialPort.Open();

        while(true)
        {
            try
            {
                message = serialPort.ReadLine();
                Console.WriteLine(message);
                AppendToFile(message);

            }
            catch (TimeoutException) { }
        }

        serialPort.Close();
    }

    public static void AppendToFile(string message)
    {
        string path = @"c:\temp\SavedData.txt";
        using (StreamWriter sw = File.AppendText(path))
        {
            sw.WriteLine(message);
            sw.Close();
        }
    }

    // Display Port values and prompt user to enter a port.
    public static string SetPortName(string defaultPortName)
    {
        string portName;

        Console.WriteLine("Available Ports:");
        foreach (string s in SerialPort.GetPortNames())
        {
            Console.WriteLine("   {0}", s);
        }

        Console.Write("Enter COM port value (Default: {0}): ", defaultPortName);
        portName = Console.ReadLine();

        if (portName == "" || !(portName.ToLower()).StartsWith("com"))
        {
            portName = defaultPortName;
        }
        return portName;
    }
 
}