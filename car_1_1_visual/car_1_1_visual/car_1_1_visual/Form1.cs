using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace car_1_1_visual
{
    public partial class Form1 : Form
    {
        bool alarm = true;
        public Form1()
        {
            InitializeComponent();
            alarm_button.BackColor = Color.LightGray;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            serialPort1.PortName = "COM4";          //Prepare the serial and start the timer
            serialPort1.BaudRate = 9600;
            serialPort1.Open();
            timer1.Start();
            textBox2.Text = "OFF";
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            timer1.Stop();
            serialPort1.Close();
        }

        private void alarm_button_Click(object sender, EventArgs e)
        {
            if(alarm == true)
            {
                serialPort1.WriteLine("ALARM");                     //Send "ALARM" in the serial if alarm is true
                alarm_button.BackColor = Color.Red;
                alarm = false;
            }
            else
            {
                serialPort1.WriteLine("NORMAL");                            //Else "NORMAL"
                alarm_button.BackColor = Color.LightGray;
                alarm = true;
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (serialPort1.BytesToRead > 0)            //Reading the serial
            {
                String message = serialPort1.ReadLine();
                message = message.Trim();
                if (message == "TEMP")                          //Reading the temp and show it
                {
                    message = serialPort1.ReadLine();
                    message = message.Trim();               
                    textBox1.Text = message;

                }
                if (message == "HEADLIGHT_ON")                      //Status of the yellow led here
                {
                    textBox2.Text = "ON";
                }
                if (message == "HEADLIGHT_OFF")
                {
                    textBox2.Text = "OFF";
                }

                if (message == "LEFT")                      //Print the car log
                {
                    listBox1.Items.Add("Turned left ");
                }
                else if (message == "RIGHT")
                {
                    listBox1.Items.Add("Turned right ");
                }
                else if (message == "STEADY")
                {
                    listBox1.Items.Add("Back to steady ");
                }
            }
        }
    }
}
