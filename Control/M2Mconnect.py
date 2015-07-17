#!/usr/bin/python
"""
M2Mconnect - this program checks wireless modem cellular network connectivity
and Raspberry Pi wwan0 interface and connects if disconnected. If eth0 or 
wlan0 has an IP then program provides a warning about multiple IP interfaces 
on Raspberry Pi but continues to connect Raspberry Pi wwan0 and the modem 
cellular PDP network connection. The program stores the modem cellular network 
IP address. This information can be used to message that IP address to server 
if required.

By default the client script is using the (telstra.extranet) APN to connect to
the cellular network, this will mean that a publicly addressable IP is assigned 
to the modem. While the IP is public the IP address is not statically assigned 
and can change between reconnection's. Teams may also wish to use the 
(telstra.internet) APN which assigns a private IP and is thus more secure.

While the modem can be assigned a public IP the Raspberry Pi wwan0 interface 
is assigned a local IP address from the modem (typically 192.168.2.2). 
If you wish the Raspberry Pi to be reached from the internet you will need to 
log into the modem config web page (telstra.4g) and configure port forward 
settings.

This script is provided as an example building block only to show what can be 
done and to give teams a basic connection script to work from. Please feel 
free to change, modify, rebuild or completely redesign components as 
specifically suits your teams chosen M2M objective.

If your team finds bugs in this baseline script or identifies better ways to
implement the baseline code that is not specific to your solution, then please 
submit modifications via github. Teams will be recognised for their 
contributions in improving the baseline code.

24th April 2015
Edited the global APN to telstra.m2m as testra.extranet is deprecated.
Added an exit call to main() after RaspberryCheckIP() as REV does not need the rest of the functionality

"""

import random
import re
import subprocess
import sys
import time
import traceback
import urllib2
import lockfile


####################
# Global Variables #
####################

APN = 'telstra.m2m'  #Vaild settings are 'telstra.extranet' for pubic IP or 'telstra.internet' for private IP.

SetEcho = 'ATE1'
ModemRadioOn = 'AT+CFUN=1'                            #Turn on modem cellular radio
ModemRadioOff = 'AT+CFUN=0'                           #Turn off modem cellular radio
ModemPdpOpen = 'AT!SCACT=1,1'                         #Open cellular network PDP connection using profile 1
ModemPdpClose = 'AT!SCACT=0,1'                        #Close open cellular network PDP connections (profile 1)
setAPN = 'AT+CGDCONT=1,\"IP\",\"' + APN + '\"'        #Set profile1 APN
setDefaultAPN = 'AT!SCDFTPROF=1'                      #Set profile 1 stored in modem as defualt for connecting
setProf1Manual = 'AT!SCPROF=1,"m2mChallenge",0,0,0,0' #Set modem to manual with not autoconnect on powerup
setProf1Auto = 'AT!SCPROF=1,"m2mChallenge",1,0,0,0'   #Set modem to auto connect on powerup (default)
setProf1NDIS = 'AT!SCPROFSWOPT=1,0'                   #Set modem prof1 to Network Device Interface Services (NDIS)
#readModemNetworkIP = 'AT!SCPADDR=1'
DHCP_Release_wwan0 = 'sudo dhclient -r wwan0'
DHCP_Renew_wwan0 = 'sudo dhclient -nw wwan0'
readIPwwan0 = 'sudo ifconfig wwan0'
backoff = 10            # Sets the starting backoff time for attempting to reconnect (seconds)
backofftimer = backoff  # backofftimer is doubled every time the connection fails. See connectbackoff()
backoffStart = 0


##########################################################
# Code to manage connection and communication with modem #
##########################################################

def InterfaceCheck(interface):
    """
    Checks if the passed network interfaces has a IP address.
    Prints a warning if a IP address is detected as this could mean the 
    Raspberry Pi has multiple IP address if wwan0 interface is activated.
    And lead to IP routing issues without correct configuration.
    """
    time.sleep(2)
    command = 'sudo ifconfig ' + interface
    commandresult, commanderror = SendOS(command)
    if commandresult:
        IP = re.search('(?:[\d]{1,3})\.(?:[\d]{1,3})\.(?:[\d]{1,3})\.(?:[\d]{1,3})', commandresult)
        if IP:
            print "[ !! ] WARNING: Detected IP address", IP.group(0), "on", interface
            print "       If Raspberry wwan0 IP also activated this can cause routing issues"
            print "       Recommend only activating one IP interface or set routing table to handle multiple interfaces"
            result = IP.group(0)
        else:
            print "[ OK ] No IP address on", interface
            result = None
    else:
        if commanderror:
            print "[ OK ] No", interface, "interface detected"
        else:
            print "[FAIL] Error: Undefined response to ifconfig", interface, "command"
        result = None
    return result


def ModemDetect():
    """Checks if wireless modem is plugged into USB port and detected on wwan0 by RaspberryPi"""
    print "[ -- ] Checking Modem Present"
    commandresult, commanderror = SendOS('ls /dev/ttyUSB*')
    modemTTYinterface = re.search('ttyUSB3', commandresult) # If modem plugged in and detected there should be a ttyUSB3 interface to communicate AT commands
    if modemTTYinterface and not commanderror:
        commandresult, commanderror = SendOS('sudo ifconfig wwan0')
        modeminterface = re.search('wwan0', commandresult) # If modem plugged in and detected there should be a wwan0 interface created
        if modeminterface and not commanderror:
            result = True  # ttyUSB3 and wwan0 interface detected so assume modem is plugged in and correctly configured
        else:
            print "[FAIL] Modem ttyUSB3 interface detected but wwan0 interface is NOT detected"
            result = False
    else:
        print "[FAIL] Modem ttyUSB3 interface is NOT detected, check sierra modem is plugged into USB port"
        result = False
    return result


def SendAT(ATcom):
    """Sends AT commands to modem. Warning does not cheak for OK reply"""
    print "[ -- ]     Sending AT command:", ATcom
    command = 'sudo echo -e \'' + ATcom
    command = command + '\r\' > /dev/ttyUSB3'
    SendOS(command)
    time.sleep(2)


def SendOS(OScom):
    """Sends commands to Raspberry OS and returns output. """
    #print "[ -- ] Sending OS command:", OScom
    commandoutput = subprocess.Popen([OScom], shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    commandresult, commanderror = commandoutput.communicate()
    return commandresult, commanderror


############################################
# Code to manage wireless connection state #
############################################

def ModemPdpConnect():
    """Initiate new PDP connection"""
    print "[ -- ] Running PDP Connection Commands"
    SendAT(ModemRadioOn)
    time.sleep(10)
    SendAT(setAPN)
    time.sleep(3)
    SendAT(setDefaultAPN)
    time.sleep(3)
    SendAT(setProf1NDIS)
    time.sleep(3)
    SendAT(setProf1Auto)
    time.sleep(3)
    SendAT(ModemPdpOpen)
    time.sleep(6)


def RaspberryIPcheck():
    """
    Checks if IP address assigned on interface wwan0 of Raspberry Pi
    """
    #SendOS(DHCP_Release_wwan0)
    #time.sleep(2)
    SendOS(DHCP_Renew_wwan0)
    time.sleep(5)
    commandresult, commanderror = SendOS(readIPwwan0)
    wwanip = re.search('(?:[\d]{1,3})\.(?:[\d]{1,3})\.(?:[\d]{1,3})\.(?:[\d]{1,3})', commandresult)
    if wwanip and not commanderror:
        print "[ OK ] RaspberryPi wwan0 connected with IP address", wwanip.group(0)
        result = wwanip.group(0)
	
    else:
        print "[FAIL] RaspberryPi unable to get wwan0 IP address"
        result = None
    return result


def ModemIPcheck():
    """
    Checks if IP address assigned on the wireless modem from cellular network
    """
    zeroIP = "0.0.0.0"
    webresponse = urllib2.urlopen('http://192.168.1.1/index.html#data')
    modemwebpage = webresponse.read()
    modemip = re.search('(?:[\d]{1,3})\.(?:[\d]{1,3})\.(?:[\d]{1,3})\.(?:[\d]{1,3})', modemwebpage)
    if modemip:
        if modemip.group(0) != zeroIP:
            print "[ OK ] Modem connected with IP address", modemip.group(0)
            result = modemip.group(0)
        else:
            print "[ -- ] No Modem IP address"
            result = None
    else:
        print "[ -- ] No Modem IP address"
        result = None
    return result


def ModemIPchange(modemip):
    """
    Checks if modem IP has changed from the last acquired modem IP
    Saves current IP in file .current_ip for later reference
    """
    print "[ -- ] Checking if Modem IP Changed"
    lastIP = subprocess.Popen("cat  ~/.current_ip", shell=True, stdout=subprocess.PIPE)
    lastIP = lastIP.communicate()[0]
    if lastIP:
        lastIPclean = re.search('(?:[\d]{1,3})\.(?:[\d]{1,3})\.(?:[\d]{1,3})\.(?:[\d]{1,3})', lastIP)
        if lastIPclean:
            if modemip != lastIPclean.group(0):
                print "[ -- ] New Modem IP address detected", modemip, "was", lastIPclean.group(0)
                iplist = "echo \"" + modemip + "\" >|~/.current_ip"
                # Save IP address in file .current_ip
                SendOS(iplist)
                # Place code here if you want device to do something in response to IP change. Example send email.
            else:
                print "[ -- ] Modem IP address same as last acquired address", modemip
        else:
            # File .current_ip was empty
            print "[ -- ] New wwan0 IP address", modemip
            iplist = "echo \"" + modemip + "\" >|~/.current_ip"
            # Save IP address in file .current_ip
            SendOS(iplist)
    else:
        # File .current_ip did not exist
        print "[ -- ] New wwan0 IP address", modemip
        iplist = "echo \"" + modemip + "\" >|~/.current_ip"
        # Save IP address in file .current_ip
        SendOS(iplist)

def privateIP(currentIP):
    """
    Checks if current IP is in the private IP range or the public IP range
    Returns True if in private IP range or False if in public IP range
    """

    #Match against private IP ranges
    if re.search('10\.(?:[\d]{1,3})\.(?:[\d]{1,3})\.(?:[\d]{1,3})', currentIP):
        print "[ -- ] Private IP using 10.x.x.x range"
        result = True
    elif re.search('192\.168\.(?:[\d]{1,3})\.(?:[\d]{1,3})', currentIP):
        print "[ -- ] Private IP using 192.168.x.x range"
        result = True
    elif re.search('172\.(?:1[6-9]|2[0-9]|3[0-1])\.(?:[\d]{1,3})\.(?:[\d]{1,3})', currentIP):     
        print "[ -- ] Private IP using 172.16.x.x to 172.31.x.x range"
        result = True
    else:
        print "[ -- ] Public IP range"
        result = False
    return result


def correctIP(currentIP):
    """
    Checks if current IP matches set APN's IP range
    telstra.extranet APN is always public
    telstra.internet APN is always private
    Other APNs (eg telstra.m2m) are ignored by returning True
    """
    privateIPset = privateIP(currentIP)
    if privateIPset and APN == 'telstra.extranet':
        result = False
    elif not privateIPset and APN == 'telstra.internet':
        result = False
    else:
        result = True
    return result


def connectbackoff():
    """
    If failure to connect on wireless this method sets a growing backoff 
    period before attempting again
    """
    global backofftimer

    timepassed = time.time() - backoffStart
    if timepassed > backofftimer:
        #print "[ -- ] Backoff timer has expired so ok to try to connect"
        backofftimer = backofftimer + backofftimer + random.random()  # Increase time before trying again
        if (backofftimer > 1800): # Set max backoff timer at 30 minutes
            backofftimer = 1800 + random.randint(0, 300) # Backoff 30 minutes plus 0 to 5 minute random window
        return True
    else:
        #print "[ -- ] Backoff timer has not expired so cannot try to connect yet"
        timeremain = backofftimer - timepassed
        timeremain = round(timeremain, 1)
        print "[ -- ] System can attempt reconnect in", timeremain, "seconds"
        return False


class ExitProgram(Exception):
    """
    Exits the program by using ExitProgram exception handler.
    Ensures the M2MconnectLOCKfile is released before close.
    """
    pass


#############################################
# Main program loop with exception handlers #
#############################################

def main():
    """M2Mconnect main. Modem initialisation and exception handling"""
    print "[ -- ] System Initiating Connection Check"
    global backofftimer
    global backoffStart

    #Check lock file to see if another M2Mconnect program is already running. If already running then exit.
    M2MconnectLOCKfile = lockfile.FileLock('M2MconnectLOCKfile')
    try:
        M2MconnectLOCKfile.acquire(timeout=0)    # wait up to 0 seconds
    except lockfile.LockError:
        print " "
        print "**********"
        print "Exception (M2MconnectLOCKfile is locked)"
        print "Check another instance of M2Mconnect.py is not already running"
        print "**********"
        sys.exit()

    while True:
        try:
            if connectbackoff(): 
                #print "Start loop backofftimer ", backofftimer
                #Check what interfaces already have a IP address
                InterfaceCheck('eth0')
                InterfaceCheck('wlan0')
                #Begin wwan0 interface setup
                if ModemDetect():
                    SendAT('ATE1')
                    if RaspberryIPcheck():
			#QUIT PROGRAM IS IPCHECK IS VALID
			raise ExitProgram()

                        currentIP = ModemIPcheck()
                        if currentIP:
                            if not correctIP(currentIP):
                                # Disconnect currnet PDP session and start again
                                if APN == 'telstra.internet':
                                    print "[ -- ] APN set to", APN, "but current modem IP is public"
                                elif APN == 'telstra.extranet':
                                    print "[ -- ] APN set to", APN, "but current modem IP is private"
                                print "[ -- ] Closeing current network PDP connection"
                                SendAT(ModemPdpClose)
                                time.sleep(10)
                                ModemPdpConnect()
                                returnedIP = ModemIPcheck()
                                if returnedIP is not None:
                                    if correctIP(returnedIP):
                                        ModemIPchange(returnedIP)
                                        print "[ OK ] System IP Connection Complete"
                                        print "************************************"
                                        backofftimer = backoff
                                        raise ExitProgram()
                                    else:
                                        print "[FAIL] Modem did not get correct IP address for APN", APN
                                        SendAT(ModemPdpClose)
                                        time.sleep(10)
                                        backoffStart = time.time()
                                        print " "
                                else:
                                    print "[FAIL] Modem did not get IP address from network"
                                    backoffStart = time.time()
                                    print " "
                            else:
                                # Currnet IP and APN match or APN not set as either extranet/internet.
                                ModemIPchange(currentIP)
                                print "[ OK ] System IP Connection Complete"
                                print "************************************"
                                backofftimer = backoff
                                raise ExitProgram()
                        else:
                            ModemPdpConnect()
                            returnedIP = ModemIPcheck()
                            if returnedIP is not None:
                                if correctIP(returnedIP):
                                    ModemIPchange(returnedIP)
                                    print "[ OK ] System IP Connection Complete"
                                    print "************************************"
                                    backofftimer = backoff
                                    raise ExitProgram()
                                else:
                                    print "[FAIL] Modem did not get correct IP address for APN", APN
                                    SendAT(ModemPdpClose)
                                    time.sleep(10)
                                    backoffStart = time.time()
                                    print " "
                            else:
                                print "[FAIL] Modem could not get IP address from network"
                                backoffStart = time.time()
                                print " "
                    else:
                        print "[FAIL] RaspberryPi could not get IP address from modem\n"
                        backofftimer = backoff
                else:
                    print "[FAIL] Unable to open AT serial connection to Modem\n"
                    backofftimer = backoff
            time.sleep(5)

        except ExitProgram:
            M2MconnectLOCKfile.release()
            sys.exit()
        except urllib2.HTTPError as e:
            # Capture HTTPError errors
            print " "
            print "**********"
            print "Exception detected (HTTPError)"
            print "Code:", e.code
            print "Reason:", e.reason
            print "**********"
        except urllib2.URLError as e:
            # Capture URLError errors
            print " "
            print "**********"
            print "Exception detected (URLError)"
            print "Reason:", e.reason
            print "**********"
            print "[ -- ] Rebooting Modem"
        except OSError as e:
            # Capture OS command line errors
            print " "
            print "**********"
            print "Exception detected (OSError)"
            print " **********"
            print e.errno
            print e.filename
            print e.strerror
            traceback.print_exc(file=sys.stdout)
            print "**********"
            M2MconnectLOCKfile.release()
            sys.exit()
        except Exception as e:
            print " "
            print "**********"
            print "Exception detected"
            print type(e)
            print e
            traceback.print_exc(file=sys.stdout)
            print "**********"
            M2MconnectLOCKfile.release()
            sys.exit()
        except KeyboardInterrupt:
            print " "
            print "**********"
            print "Exception (Keyboard Interrupt)"
            print "**********"
            M2MconnectLOCKfile.release()
            sys.exit()
        except:
            print " "
            print "**********"
            print "Exception (Unknown)"
            traceback.print_exc(file=sys.stdout)
            print "**********"
            M2MconnectLOCKfile.release()
            sys.exit()

if __name__ == "__main__":
    main()




