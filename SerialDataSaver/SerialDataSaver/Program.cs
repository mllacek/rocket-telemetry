using System;
using System.IO;
using System.IO.Ports;
using System.Threading;

public class SerialDataSaver
{

    public static void Main()
    {
        string message, fileName;

        SerialPort serialPort = new SerialPort();

        serialPort.PortName = SetPortName(serialPort.PortName);

        fileName = SetFileName("SavedData.txt");

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
                AppendToFile(fileName, message);

            }
            catch (TimeoutException) { }
        }

        serialPort.Close();
    }

    public static void AppendToFile(string fileName, string message)
    {
        string path = @"c:\temp\" + fileName;
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

    // Prompt the user to enter a file name to create or append to.
    public static string SetFileName(string defaultFileName)
    {
        string fileName;

        Console.WriteLine("Enter the name of the file to save to:");
        fileName = Console.ReadLine();

        if (fileName == "" || !(fileName.ToLower()).EndsWith(".txt"))
        {
            fileName = defaultFileName;
        }

        return fileName;
    }

}