using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using FTD2XX_NET;

namespace Silnik
{
    class Program
    {
        enum sterowanie { Jednofazowe, Dwufazowe, Polkrokowe };
        FTD2XX_NET.FTDI.FT_STATUS ftstatus;
        FTD2XX_NET.FTDI.FT_DEVICE_INFO_NODE[] devicelist = new FTD2XX_NET.FTDI.FT_DEVICE_INFO_NODE[1];
        FTDI device = new FTDI();
        byte[] jednofazowe = { 0x04, 0x02, 0x03, 0x01 };
        byte[] dwufazowe = { 0x05, 0x06, 0x0A, 0x09 };


        sterowanie control = sterowanie.Jednofazowe;
        int okres = 1000;

        private void Polacz() {
            ftstatus = device.GetDeviceList(devicelist);
            ftstatus = device.OpenByDescription(devicelist[0].Description);
            ftstatus = device.SetBitMode(0xff, 1);
        }

        private void koniec()
        {
            byte[] zero = { 0x00 };
            Int32 bytesToWrite = 1;
            UInt32 bytesWritten = 0;
            ftstatus = device.Write(zero, bytesToWrite, ref bytesWritten);
        }

        private int przelicz(int kat)
        {
            return ((kat * 12) / 90);
        }

        public void funkcja()
        {
            //TODO
            // Pytanie o okres
            Polacz();
            Console.WriteLine("Polaczono!");
            Console.WriteLine("Wybierz tryb sterowania\n\t1 - jednofazowe\n\t2 - dwufazowe\n");
            string linijka = Console.ReadLine();
            int pom = int.Parse(linijka);
            if (pom == 1)
            {
                control = sterowanie.Jednofazowe;
            }
            else if (pom == 2)
            {
                control = sterowanie.Dwufazowe;
            }
            Console.WriteLine("\t1 - kroki\n\t2 - kat\n");
            linijka = Console.ReadLine();
            int pom5 = int.Parse(linijka);
            int loops = 0;
            if (pom5 == 1)
            {
                Console.WriteLine("Ile krokow?");
                linijka = Console.ReadLine();
                int pom6 = int.Parse(linijka);
                loops = pom6;
            }
            else
            {
                Console.WriteLine("jaki kat?");
                linijka = Console.ReadLine();
                int pom7 = int.Parse(linijka);
                pom7 = przelicz(pom7);
                loops = pom7;
            }
            // Ile krokow
            
            Console.WriteLine("Ile czekac?");
            linijka = Console.ReadLine();
            int pom4 = int.Parse(linijka);
            int okres = pom4;

            int steps = 4;
            switch (control)
            {
                case sterowanie.Jednofazowe:
                    {
                        Console.WriteLine("Ktora strona?\n\t1 - prawa\n\t2 - lewa\n");
                        linijka = Console.ReadLine();
                        int pom3 = int.Parse(linijka);
                        switch(pom3)
                        {
                            case 1:
                                {
                                    byte[][] controlByte = new byte[steps][];

                                    for (int i = 0; i < steps; i++)
                                        controlByte[i] = new byte[] { dwufazowe[i] };

                                    for (int k = 0, i = steps - 1; k < loops; k++)
                                    {
                                        Int32 bytesToWrite = 1;
                                        UInt32 bytesWritten = 0;

                                        ftstatus = device.Write(controlByte[i], bytesToWrite, ref bytesWritten);
                                        System.Threading.Thread.Sleep(okres);

                                        if (i == 0)
                                            i = steps - 1;
                                        else
                                            i--;
                                    }
                                    koniec();
                                    break;
                                }
                            case 2:
                                {
                                    byte[][] controlByte = new byte[steps][];

                                    for (int i = 0; i < steps; i++)
                                        controlByte[i] = new byte[] { jednofazowe[i] };

                                    for (int k = 0, i = 0; k < loops; k++)
                                    {
                                        Int32 bytesToWrite = 1;
                                        UInt32 bytesWritten = 0;


                                        ftstatus = device.Write(controlByte[i], bytesToWrite, ref bytesWritten);
                                        System.Threading.Thread.Sleep(okres);

                                        if (i == steps - 1)
                                            i = 0;
                                        else
                                            i++;
                                    }


                                    koniec();
                                    break;
                                }
                        }
                     koniec();
                     break;
                    }
                case sterowanie.Dwufazowe:
                    {
                        Console.WriteLine("Ktora strona?\n\t1 - prawa\n\t2 - lewa\n");
                        linijka = Console.ReadLine();
                        int pom3 = int.Parse(linijka);
                        switch (pom3)
                        {
                            case 1:
                                {
                                    byte[][] controlByte = new byte[steps][];

                                    for (int i = 0; i < steps; i++)
                                        controlByte[i] = new byte[] { dwufazowe[i] };

                                    for (int k = 0, i = steps - 1; k < loops; k++)
                                    {
                                        Int32 bytesToWrite = 1;
                                        UInt32 bytesWritten = 0;

                                        ftstatus = device.Write(controlByte[i], bytesToWrite, ref bytesWritten);
                                        System.Threading.Thread.Sleep(okres);

                                        if (i == 0)
                                            i = steps - 1;
                                        else
                                            i--;
                                    }
                                    koniec();
                                    break;
                                }
                            case 2:
                                {
                                    byte[][] controlByte = new byte[steps][];

                                    for (int i = 0; i < steps; i++)
                                        controlByte[i] = new byte[] { dwufazowe[i] };

                                    for (int k = 0, i = 0; k < loops; k++)
                                    {
                                        Int32 bytesToWrite = 1;
                                        UInt32 bytesWritten = 0;


                                        ftstatus = device.Write(controlByte[i], bytesToWrite, ref bytesWritten);
                                        System.Threading.Thread.Sleep(okres);

                                        if (i == steps - 1)
                                            i = 0;
                                        else
                                            i++;
                                    }


                                    koniec();
                                    break;
                                }
                        }
                        koniec();
                        break;
                    }
            }


        }
        static void Main(string[] args)
        {
            Program a = new Program();
            a.funkcja();
            Console.ReadKey();
        }
    }
}
