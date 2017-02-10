using System;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.IO;
using System.Xml;
namespace BlueArduino
{
    class Program
    {
        static void Main(string[] args)
        {
            string rsspath = "https://news.google.com/news?cf=all&hl=pt-PT&pz=1&ned=pt-PT_pt&output=rss";
            string portname = "COM3";
            //System.IO.Ports.SerialPort port;
            //port = new System.IO.Ports.SerialPort();
            //port.PortName = portname;
            //port.Open();
            //port.BaudRate = 9600;
            while (true)
            {
                WebRequest rss = WebRequest.Create(rsspath);
                WebResponse rssResp = rss.GetResponse();
                Stream rssStream = rssResp.GetResponseStream();
                XmlDocument rssDoc = new XmlDocument();
                 rssDoc.Load(rssStream);
                Console.WriteLine(rssDoc.InnerText);
            }
        }
    }
}
