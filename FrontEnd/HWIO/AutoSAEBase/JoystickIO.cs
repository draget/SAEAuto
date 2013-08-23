using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows.Forms;

using SharpDX.DirectInput;

namespace AutoSAEBase
{
    class JoystickIO
    {
        private Joystick joystick;
        private DirectInput di;

        public JoystickIO()
        {
            di = new DirectInput();
        }

        public Boolean Connect()
        {

            // Joystick finder code adapted from SharpDX Samples - (c) Alexandre Mutel 2012
            Guid joystickGuid = Guid.Empty;

            foreach (DeviceInstance deviceInstance in di.GetDevices(DeviceType.Gamepad, DeviceEnumerationFlags.AllDevices))
                joystickGuid = deviceInstance.InstanceGuid;

            if (joystickGuid == Guid.Empty)
                foreach (DeviceInstance deviceInstance in di.GetDevices(DeviceType.Joystick, DeviceEnumerationFlags.AllDevices))
                    joystickGuid = deviceInstance.InstanceGuid;

            if(joystickGuid == Guid.Empty) {
                MessageBox.Show("No joystick found."); return false;
            }

            joystick = new Joystick(di, joystickGuid);
            joystick.Properties.BufferSize = 128;
            joystick.Acquire();

            return true;

        }

        private JoystickState getCurrentState()
        {
            JoystickState jss;
            try { joystick.Poll(); jss = joystick.GetCurrentState(); return jss; }
            catch { return null; }
        }

        public int[] getXY()
        {
            JoystickState jss = getCurrentState();
            if (jss != null) { return new int[2] { jss.X, jss.RotationZ }; }
            else return new int[2] { -1, -1 };
        }

        public bool[] getButtons()
        {
            JoystickState jss = getCurrentState();
            if (jss != null) { return jss.Buttons; }
            else return null;

        }

    }
}
