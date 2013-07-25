namespace AutoSAEBase
{
    partial class MainWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainWindow));
            this.button1 = new System.Windows.Forms.Button();
            this.DataqStop = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.COMbox = new System.Windows.Forms.TextBox();
            this.InputTimer = new System.Windows.Forms.Timer(this.components);
            this.OutputTimer = new System.Windows.Forms.Timer(this.components);
            this.ButtonPosPanel = new System.Windows.Forms.Panel();
            this.PulseRXPanel = new System.Windows.Forms.Panel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.label8 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.panel5 = new System.Windows.Forms.Panel();
            this.PulsePanel = new System.Windows.Forms.Panel();
            this.label12 = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.label2 = new System.Windows.Forms.Label();
            this.axDataqSdk1 = new AxDATAQSDKLib.AxDataqSdk();
            this.ConnectButton = new System.Windows.Forms.Button();
            this.panel3 = new System.Windows.Forms.Panel();
            this.button3 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.label9 = new System.Windows.Forms.Label();
            this.NetworkStatusPanel = new System.Windows.Forms.Panel();
            this.label7 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.RXPanel = new System.Windows.Forms.Panel();
            this.TXPanel = new System.Windows.Forms.Panel();
            this.label3 = new System.Windows.Forms.Label();
            this.CarIPBox = new System.Windows.Forms.TextBox();
            this.panel4 = new System.Windows.Forms.Panel();
            this.ThrottleLabel = new System.Windows.Forms.Label();
            this.BrakeLabel = new System.Windows.Forms.Label();
            this.YBar = new System.Windows.Forms.TrackBar();
            this.XBar = new System.Windows.Forms.TrackBar();
            this.label11 = new System.Windows.Forms.Label();
            this.jsStatusPanel = new System.Windows.Forms.Panel();
            this.joystickStart = new System.Windows.Forms.Button();
            this.label10 = new System.Windows.Forms.Label();
            this.JoystickTimer = new System.Windows.Forms.Timer(this.components);
            this.EStopButton = new System.Windows.Forms.Button();
            this.AlarmPanel = new System.Windows.Forms.Panel();
            this.label13 = new System.Windows.Forms.Label();
            this.panel1.SuspendLayout();
            this.panel5.SuspendLayout();
            this.panel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.axDataqSdk1)).BeginInit();
            this.panel3.SuspendLayout();
            this.panel4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.YBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.XBar)).BeginInit();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(20, 84);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(37, 31);
            this.button1.TabIndex = 5;
            this.button1.Text = "Start";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // DataqStop
            // 
            this.DataqStop.Location = new System.Drawing.Point(63, 85);
            this.DataqStop.Name = "DataqStop";
            this.DataqStop.Size = new System.Drawing.Size(44, 30);
            this.DataqStop.TabIndex = 6;
            this.DataqStop.Text = "Stop";
            this.DataqStop.UseVisualStyleBackColor = true;
            this.DataqStop.Click += new System.EventHandler(this.DataqStop_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(22, 32);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(85, 13);
            this.label1.TabIndex = 7;
            this.label1.Text = "Dataq COM Port";
            // 
            // COMbox
            // 
            this.COMbox.Location = new System.Drawing.Point(20, 49);
            this.COMbox.Name = "COMbox";
            this.COMbox.Size = new System.Drawing.Size(87, 20);
            this.COMbox.TabIndex = 8;
            this.COMbox.Text = "3";
            // 
            // InputTimer
            // 
            this.InputTimer.Tick += new System.EventHandler(this.InputTimer_Tick_1);
            // 
            // OutputTimer
            // 
            this.OutputTimer.Tick += new System.EventHandler(this.OutputTimer_Tick);
            // 
            // ButtonPosPanel
            // 
            this.ButtonPosPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.ButtonPosPanel.Location = new System.Drawing.Point(81, 44);
            this.ButtonPosPanel.Name = "ButtonPosPanel";
            this.ButtonPosPanel.Size = new System.Drawing.Size(18, 19);
            this.ButtonPosPanel.TabIndex = 17;
            // 
            // PulseRXPanel
            // 
            this.PulseRXPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.PulseRXPanel.Location = new System.Drawing.Point(81, 69);
            this.PulseRXPanel.Name = "PulseRXPanel";
            this.PulseRXPanel.Size = new System.Drawing.Size(18, 19);
            this.PulseRXPanel.TabIndex = 18;
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel1.Controls.Add(this.label8);
            this.panel1.Controls.Add(this.label6);
            this.panel1.Controls.Add(this.label5);
            this.panel1.Controls.Add(this.PulseRXPanel);
            this.panel1.Controls.Add(this.ButtonPosPanel);
            this.panel1.Location = new System.Drawing.Point(15, 91);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(129, 99);
            this.panel1.TabIndex = 19;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(13, 75);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(54, 13);
            this.label8.TabIndex = 20;
            this.label8.Text = "Heartbeat";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(13, 50);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(39, 13);
            this.label6.TabIndex = 19;
            this.label6.Text = "E-Stop";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(13, 16);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(99, 13);
            this.label5.TabIndex = 0;
            this.label5.Text = "Heartbeat Relay";
            // 
            // panel5
            // 
            this.panel5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel5.Controls.Add(this.PulsePanel);
            this.panel5.Controls.Add(this.label12);
            this.panel5.Location = new System.Drawing.Point(15, 13);
            this.panel5.Name = "panel5";
            this.panel5.Size = new System.Drawing.Size(129, 63);
            this.panel5.TabIndex = 20;
            // 
            // PulsePanel
            // 
            this.PulsePanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.PulsePanel.Location = new System.Drawing.Point(51, 32);
            this.PulsePanel.Name = "PulsePanel";
            this.PulsePanel.Size = new System.Drawing.Size(18, 19);
            this.PulsePanel.TabIndex = 19;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label12.Location = new System.Drawing.Point(2, 9);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(123, 13);
            this.label12.TabIndex = 21;
            this.label12.Text = "Heartbeat Generator";
            // 
            // panel2
            // 
            this.panel2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel2.Controls.Add(this.label2);
            this.panel2.Controls.Add(this.label1);
            this.panel2.Controls.Add(this.button1);
            this.panel2.Controls.Add(this.COMbox);
            this.panel2.Controls.Add(this.DataqStop);
            this.panel2.Location = new System.Drawing.Point(150, 13);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(126, 125);
            this.panel2.TabIndex = 21;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(34, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(60, 13);
            this.label2.TabIndex = 22;
            this.label2.Text = "DataQ IO";
            // 
            // axDataqSdk1
            // 
            this.axDataqSdk1.Enabled = true;
            this.axDataqSdk1.Location = new System.Drawing.Point(159, 144);
            this.axDataqSdk1.Name = "axDataqSdk1";
            this.axDataqSdk1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axDataqSdk1.OcxState")));
            this.axDataqSdk1.Size = new System.Drawing.Size(100, 50);
            this.axDataqSdk1.TabIndex = 4;
            this.axDataqSdk1.NewData += new AxDATAQSDKLib._DDataqSdkEvents_NewDataEventHandler(this.axDataqSdk1_NewData);
            this.axDataqSdk1.ControlError += new AxDATAQSDKLib._DDataqSdkEvents_ControlErrorEventHandler(this.axDataqSdk1_ControlError);
            // 
            // ConnectButton
            // 
            this.ConnectButton.Location = new System.Drawing.Point(18, 58);
            this.ConnectButton.Name = "ConnectButton";
            this.ConnectButton.Size = new System.Drawing.Size(84, 30);
            this.ConnectButton.TabIndex = 22;
            this.ConnectButton.Text = "Connect";
            this.ConnectButton.UseVisualStyleBackColor = true;
            this.ConnectButton.Click += new System.EventHandler(this.ConnectButton_Click);
            // 
            // panel3
            // 
            this.panel3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel3.Controls.Add(this.button3);
            this.panel3.Controls.Add(this.button2);
            this.panel3.Controls.Add(this.label9);
            this.panel3.Controls.Add(this.NetworkStatusPanel);
            this.panel3.Controls.Add(this.label7);
            this.panel3.Controls.Add(this.label4);
            this.panel3.Controls.Add(this.RXPanel);
            this.panel3.Controls.Add(this.TXPanel);
            this.panel3.Controls.Add(this.label3);
            this.panel3.Controls.Add(this.CarIPBox);
            this.panel3.Controls.Add(this.ConnectButton);
            this.panel3.Location = new System.Drawing.Point(282, 13);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(118, 246);
            this.panel3.TabIndex = 23;
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(18, 203);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(75, 35);
            this.button3.TabIndex = 28;
            this.button3.Text = "Toggle Brake IL";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(18, 162);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 35);
            this.button2.TabIndex = 27;
            this.button2.Text = "Force Manual";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(23, 133);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(37, 13);
            this.label9.TabIndex = 26;
            this.label9.Text = "Status";
            // 
            // NetworkStatusPanel
            // 
            this.NetworkStatusPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.NetworkStatusPanel.Location = new System.Drawing.Point(66, 127);
            this.NetworkStatusPanel.Name = "NetworkStatusPanel";
            this.NetworkStatusPanel.Size = new System.Drawing.Size(18, 19);
            this.NetworkStatusPanel.TabIndex = 25;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(58, 102);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(21, 13);
            this.label7.TabIndex = 24;
            this.label7.Text = "TX";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(9, 102);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(22, 13);
            this.label4.TabIndex = 21;
            this.label4.Text = "RX";
            // 
            // RXPanel
            // 
            this.RXPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.RXPanel.Location = new System.Drawing.Point(34, 100);
            this.RXPanel.Name = "RXPanel";
            this.RXPanel.Size = new System.Drawing.Size(18, 19);
            this.RXPanel.TabIndex = 21;
            // 
            // TXPanel
            // 
            this.TXPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.TXPanel.Location = new System.Drawing.Point(84, 100);
            this.TXPanel.Name = "TXPanel";
            this.TXPanel.Size = new System.Drawing.Size(18, 19);
            this.TXPanel.TabIndex = 20;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(19, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(77, 13);
            this.label3.TabIndex = 23;
            this.label3.Text = "Car Network";
            // 
            // CarIPBox
            // 
            this.CarIPBox.Location = new System.Drawing.Point(18, 32);
            this.CarIPBox.Name = "CarIPBox";
            this.CarIPBox.Size = new System.Drawing.Size(78, 20);
            this.CarIPBox.TabIndex = 0;
            this.CarIPBox.Text = "10.1.1.3";
            // 
            // panel4
            // 
            this.panel4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel4.Controls.Add(this.label13);
            this.panel4.Controls.Add(this.AlarmPanel);
            this.panel4.Controls.Add(this.ThrottleLabel);
            this.panel4.Controls.Add(this.BrakeLabel);
            this.panel4.Controls.Add(this.YBar);
            this.panel4.Controls.Add(this.XBar);
            this.panel4.Controls.Add(this.label11);
            this.panel4.Controls.Add(this.jsStatusPanel);
            this.panel4.Controls.Add(this.joystickStart);
            this.panel4.Controls.Add(this.label10);
            this.panel4.Location = new System.Drawing.Point(415, 17);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(322, 173);
            this.panel4.TabIndex = 24;
            // 
            // ThrottleLabel
            // 
            this.ThrottleLabel.AutoSize = true;
            this.ThrottleLabel.ForeColor = System.Drawing.Color.Gray;
            this.ThrottleLabel.Location = new System.Drawing.Point(238, 44);
            this.ThrottleLabel.Name = "ThrottleLabel";
            this.ThrottleLabel.Size = new System.Drawing.Size(43, 13);
            this.ThrottleLabel.TabIndex = 32;
            this.ThrottleLabel.Text = "Throttle";
            // 
            // BrakeLabel
            // 
            this.BrakeLabel.AutoSize = true;
            this.BrakeLabel.ForeColor = System.Drawing.Color.Gray;
            this.BrakeLabel.Location = new System.Drawing.Point(238, 102);
            this.BrakeLabel.Name = "BrakeLabel";
            this.BrakeLabel.Size = new System.Drawing.Size(35, 13);
            this.BrakeLabel.TabIndex = 31;
            this.BrakeLabel.Text = "Brake";
            // 
            // YBar
            // 
            this.YBar.Location = new System.Drawing.Point(190, 15);
            this.YBar.Maximum = 32768;
            this.YBar.Minimum = -32768;
            this.YBar.Name = "YBar";
            this.YBar.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.YBar.Size = new System.Drawing.Size(42, 127);
            this.YBar.TabIndex = 30;
            this.YBar.TickFrequency = 1024;
            this.YBar.TickStyle = System.Windows.Forms.TickStyle.Both;
            // 
            // XBar
            // 
            this.XBar.Location = new System.Drawing.Point(27, 81);
            this.XBar.Maximum = 32768;
            this.XBar.Minimum = -32768;
            this.XBar.Name = "XBar";
            this.XBar.Size = new System.Drawing.Size(127, 42);
            this.XBar.TabIndex = 29;
            this.XBar.TickFrequency = 1024;
            this.XBar.TickStyle = System.Windows.Forms.TickStyle.Both;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(108, 35);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(37, 13);
            this.label11.TabIndex = 28;
            this.label11.Text = "Status";
            // 
            // jsStatusPanel
            // 
            this.jsStatusPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.jsStatusPanel.Location = new System.Drawing.Point(151, 29);
            this.jsStatusPanel.Name = "jsStatusPanel";
            this.jsStatusPanel.Size = new System.Drawing.Size(18, 19);
            this.jsStatusPanel.TabIndex = 27;
            // 
            // joystickStart
            // 
            this.joystickStart.Location = new System.Drawing.Point(15, 28);
            this.joystickStart.Name = "joystickStart";
            this.joystickStart.Size = new System.Drawing.Size(75, 23);
            this.joystickStart.TabIndex = 28;
            this.joystickStart.Text = "Connect";
            this.joystickStart.UseVisualStyleBackColor = true;
            this.joystickStart.Click += new System.EventHandler(this.joystickStart_Click);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label10.Location = new System.Drawing.Point(12, 5);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(97, 13);
            this.label10.TabIndex = 27;
            this.label10.Text = "Game Controller";
            // 
            // JoystickTimer
            // 
            this.JoystickTimer.Tick += new System.EventHandler(this.JoystickTimer_Tick);
            // 
            // EStopButton
            // 
            this.EStopButton.BackColor = System.Drawing.Color.Red;
            this.EStopButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.EStopButton.ForeColor = System.Drawing.Color.White;
            this.EStopButton.Location = new System.Drawing.Point(18, 211);
            this.EStopButton.Name = "EStopButton";
            this.EStopButton.Size = new System.Drawing.Size(240, 63);
            this.EStopButton.TabIndex = 25;
            this.EStopButton.Text = "EMERGENCY STOP";
            this.EStopButton.UseVisualStyleBackColor = false;
            this.EStopButton.Click += new System.EventHandler(this.button4_Click);
            // 
            // AlarmPanel
            // 
            this.AlarmPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.AlarmPanel.Location = new System.Drawing.Point(40, 143);
            this.AlarmPanel.Name = "AlarmPanel";
            this.AlarmPanel.Size = new System.Drawing.Size(18, 19);
            this.AlarmPanel.TabIndex = 33;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(64, 149);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(33, 13);
            this.label13.TabIndex = 29;
            this.label13.Text = "Alarm";
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(754, 288);
            this.Controls.Add(this.EStopButton);
            this.Controls.Add(this.panel4);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel5);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.axDataqSdk1);
            this.Name = "MainWindow";
            this.Text = "AutoSAE Base Station H/W IO";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel5.ResumeLayout(false);
            this.panel5.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.axDataqSdk1)).EndInit();
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            this.panel4.ResumeLayout(false);
            this.panel4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.YBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.XBar)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion


        private AxDATAQSDKLib.AxDataqSdk axDataqSdk1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button DataqStop;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox COMbox;
        private System.Windows.Forms.Timer InputTimer;
        private System.Windows.Forms.Timer OutputTimer;
        private System.Windows.Forms.Panel ButtonPosPanel;
        private System.Windows.Forms.Panel PulseRXPanel;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Panel panel5;
        private System.Windows.Forms.Panel PulsePanel;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button ConnectButton;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Panel RXPanel;
        private System.Windows.Forms.Panel TXPanel;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox CarIPBox;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Panel NetworkStatusPanel;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.Button joystickStart;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Panel jsStatusPanel;
        private System.Windows.Forms.TrackBar YBar;
        private System.Windows.Forms.TrackBar XBar;
        private System.Windows.Forms.Timer JoystickTimer;
        private System.Windows.Forms.Label ThrottleLabel;
        private System.Windows.Forms.Label BrakeLabel;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button EStopButton;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Panel AlarmPanel;
    }
}

