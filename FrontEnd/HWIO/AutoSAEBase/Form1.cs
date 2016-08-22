using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

/*
 * Author: Thomas H. Drage
 * Created: 8/6/2013
 * Updated: 22/8/2016 - replaced DataQ with Arduino interface
 * TEMPORARY VERSION WITHOUT SERIAL INTERFACE
 */

namespace AutoSAEBase
{
    public partial class MainWindow : Form
    {

        private NetworkClient Network;
        private JoystickIO jsio;

        private bool PulseState = false;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {

            InputTimer.Interval=20;

            InputTimer.Enabled = true;


        }

        private void DataqStop_Click(object sender, EventArgs e)
        {
            InputTimer.Enabled = false;
            OutputTimer.Enabled = false;
            //axDataqSdk1.Stop();
        }


        private void InputTimer_Tick_1(object sender, EventArgs e)
        {

            // TEMPORARY SHORT CIRCUIT OF SERIAL INTERFACE

            bool ButtonPosition, PulseRXState;

            ButtonPosition = true;
            PulseRXState = PulseState;

            if (ButtonPosition == false) { ButtonPosPanel.BackColor = Color.Red; }
            if (ButtonPosition == true) { ButtonPosPanel.BackColor = Color.Green; }
            if (PulseRXState == false) { PulseRXPanel.BackColor = Color.Red; }
            if (PulseRXState == true) { PulseRXPanel.BackColor = Color.Green; }

            if ((Network != null) && Network.Connected)
            {

                if (PulseRXState) { Network.Send("HBT +"); }
                else { Network.Send("HBT -"); }
                if (!ButtonPosition) { Network.Send("ESTOP"); }

            }

            

        }

      



        private void OutputTimer_Tick(object sender, EventArgs e)
        {
            if (PulseState == false) { PulsePanel.BackColor = Color.Green; }
            if (PulseState == true) { PulsePanel.BackColor = Color.Red; }
            if (PulseState == false) { PulseState = true; }
            else { PulseState = false; }
        }

        private void Form1_Load(object sender, EventArgs e)
        {

            OutputTimer.Interval = 100;
            OutputTimer.Enabled = true;

        }

        private void Form1_Closing(object sender, EventArgs e)
        {
            if ((Network != null) && Network.Connected) { Network.Close(); }
            //axDataqSdk1.Stop();
        }

        private void ConnectButton_Click(object sender, EventArgs e)
        {
            Network = new NetworkClient(CarIPBox.Text,NetworkStatusPanel,RXPanel,TXPanel);
            bool success = Network.Connect();
        }

        private void joystickStart_Click(object sender, EventArgs e)
        {
            jsio = new JoystickIO();
            bool success = jsio.Connect();
            if(success) { 
                JoystickTimer.Interval = 40;
                JoystickTimer.Enabled = true; 
                jsStatusPanel.BackColor = Color.Green; 
            }
        }

        private void JoystickTimer_Tick(object sender, EventArgs e)
        {

            int[] xy = jsio.getXY();
            if (xy[0] == -1) { JoystickTimer.Enabled = false; jsStatusPanel.BackColor = Color.Red; SendEStop(); MessageBox.Show("Joystick error!!!"); }
            else
            {
                XBar.Value = xy[0] - 32768;
                YBar.Value = -1 * (xy[1] - 32768);
                
                if (xy[1] > 32767)
                {
                    BrakeLabel.ForeColor = Color.Red;
                    BrakeLabel.Text = "Brake " +  Math.Round((double)(xy[1] - 32767) / 327.68,1) + "%";
                    ThrottleLabel.ForeColor = Color.Gray;
                    ThrottleLabel.Text = "Throttle 0%";
                }
                else if (xy[1] < 32767)
                {
                    ThrottleLabel.ForeColor = Color.Green;
                    ThrottleLabel.Text = "Throttle " + Math.Round((double)(32768 - xy[1]) / 327.68,1) + "%";
                    BrakeLabel.ForeColor = Color.Gray;
                    BrakeLabel.Text = "Brake 0%";

                }
                else
                {
                    ThrottleLabel.ForeColor = Color.Gray;
                    BrakeLabel.ForeColor = Color.Gray;
                    BrakeLabel.Text = "Brake 0%";
                    ThrottleLabel.Text = "Throttle 0%";
                }

                if ((Network != null) && Network.Connected)
                {
                    Network.Send("ACL " + ((32767 - xy[1]) / 128).ToString());
                    Network.Send("STR " + ((xy[0] - 32768) / 256).ToString());
                }

            }

            bool[] Buttons = jsio.getButtons();

            if (Buttons == null) { JoystickTimer.Enabled = false; jsStatusPanel.BackColor = Color.Red; }

            try
            {
                if (Buttons[2])
                {
                    SendEStop();
                    EStopButton.BackColor = Color.Yellow;
                }
                else { EStopButton.BackColor = Color.Red; }




                if (Buttons[6] || Buttons[7])
                {
                    if ((Network != null) && Network.Connected)
                    {
                        Network.Send("ALM");
                        AlarmPanel.BackColor = Color.Red;
                    }
                }
                else { AlarmPanel.BackColor = Color.Gray; }
            }
            catch { SendEStop(); MessageBox.Show("Joystick error!!!"); }

        }

        private void button2_Click(object sender, EventArgs e)
        {
            if ((Network != null) && Network.Connected)
            {
                Network.Send("MAN");
                Network.Send("ACL 0");
                Network.Send("STR 0");
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if ((Network != null) && Network.Connected)
            {
                Network.Send("BIL");
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            SendEStop();
        }

        private void SendEStop()
        {
            if ((Network != null) && Network.Connected)
            {
                Network.Send("ESTOP");
            }
            else { MessageBox.Show("ESTOP Send failed - no network conn!"); }

        }
    }
}
