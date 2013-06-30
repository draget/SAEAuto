using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Windows.Forms;
using System.Drawing;

/*
 * Thomas Drage, 2013.
 * 
 * Reasonably tolerant network interface for E-stop.
 * 
 * Based on example code from MSDN.
 * 
 */

namespace AutoSAEBase
{
    public class NetworkClient
    {

        private Socket sender;
        private IPEndPoint remoteEP;

        public Boolean Connected = false;

        private Panel StatusPanel;
        private Panel RXPanel;
        private Panel TXPanel;

        public NetworkClient(string IPString, Panel setStatusPanel, Panel setRXPanel, Panel setTXPanel) {

            StatusPanel = setStatusPanel;
            RXPanel = setRXPanel;
            TXPanel = setTXPanel;

        try {
            IPAddress ipAddress = IPAddress.Parse(IPString);
            remoteEP = new IPEndPoint(ipAddress,1100);

            // Create a TCP/IP  socket.
            sender = new Socket(AddressFamily.InterNetwork, 
                SocketType.Stream, ProtocolType.Tcp );
             } 
            catch (Exception e) {
                MessageBox.Show(e.ToString());          
        }

        }

        public Boolean Connect() {
            // Connect the socket to the remote endpoint. Catch any errors.
            try {
                sender.ReceiveTimeout = 1000;
                sender.SendTimeout = 1000;
                sender.Connect(remoteEP);

                } 
            catch (SocketException se) {
                MessageBox.Show("SocketException on Connect: " + se.ToString()); return false; 
            }
            catch (ArgumentNullException ane) {
                MessageBox.Show("ArgumentNullException on Connect: " +  ane.ToString()); return false; 
            }
            catch (Exception e) {
                MessageBox.Show("Unexpected exception on Connect: " +  e.ToString()); return false;
            }

            Connected = true;
            StatusPanel.BackColor = Color.Green;
            return true;

        }

        public string Send(string message) {

            byte[] rxbyte = new byte[1];
            string response = String.Empty;
            int bytesRec = 0;

                // Encode the data string into a byte array.
                byte[] msg = Encoding.ASCII.GetBytes(message + "\n");
              

                try
                { // Send the data through the socket.
                    PanelTimerSet(TXPanel);
                    int bytesSent = sender.Send(msg);
                }
    
              catch (SocketException se) {
                  Close(); MessageBox.Show("SocketException on Send: " + se.ToString()); 
            }
            catch (ArgumentNullException ane) {
                Close(); MessageBox.Show("ArgumentNullException on Send: " + ane.ToString()); 
            }
            catch (Exception e) {
                Close(); MessageBox.Show("Unexpected exception on Send: " + e.ToString()); 
            }

                    bool endOfMsg = false;
                    int msgPosn = 0;
                    while(endOfMsg == false) {
                        PanelTimerSet(RXPanel);
                        try { bytesRec = sender.Receive(rxbyte); }
                        catch (SocketException se)
                        {
                            Close(); MessageBox.Show("SocketException on recv: " + se.ToString());
                        }
                        catch (Exception e)
                        {
                            Close(); MessageBox.Show("Unexpected exception on receive: " + e.ToString());
                        }
                        
                        if (bytesRec == 0) { endOfMsg = true; Close(); }
                        else
                        {
                            if (rxbyte[0] == Convert.ToByte(10)) { endOfMsg = true; }
                            else { response = response + Encoding.Default.GetString(rxbyte, 0, bytesRec); }
                         //   MessageBox.Show(msgPosn.ToString() + endOfMsg.ToString() + response + rxbyte[0].ToString());
                        }
                        msgPosn++;
                    }




                    if (response != "ACK|" + message) { Close(); } // MessageBox.Show("Sent " + message + "got " + response); }

                return response;
        }


        public Boolean Close()
        {

            Connected = false;
            StatusPanel.BackColor = Color.Red;

            try{
                // Release the socket.
            sender.Shutdown(SocketShutdown.Both);
            sender.Close();
            }
                catch (Exception e)
                {
                    MessageBox.Show("Unexpected exception on close: " +  e.ToString()); return false;
                }


            return true;

        }

        private void PanelTimerSet(Panel thePanel) {
            thePanel.BackColor = Color.YellowGreen;
            System.Timers.Timer PanelTimer = new System.Timers.Timer(6);
            PanelTimer.Elapsed += new System.Timers.ElapsedEventHandler((sender,args) => PanelTimerTimeout(sender,thePanel));
            PanelTimer.Enabled = true;

        }

        private void PanelTimerTimeout(object sender, Panel thePanel)
        {
            thePanel.BackColor = Color.LightGray;
        }
        
    }

}


    

