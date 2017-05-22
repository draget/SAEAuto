#!/bin/bash

COMMAND=`echo "$QUERY_STRING" | sed -n 's/^.*command=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"`

		
# You must add following two lines before
# outputting data to the web browser from shell
# script
#~ <meta http-equiv='refresh' content='10;url=./index.html'><script type='text/javascript'>window.location.href = './index.html'</script>
#~ <meta http-equiv='refresh' content='10;url=./index.html#autoEstop'><script type='text/javascript'>window.location.href = './index.html#autoEstop'</script>



 echo "Content-type: text/html"
 echo ""
 echo "<html><head><title>Demo</title><link rel='stylesheet' type='text/css' href='index.css'><script>function link(h) {	window.location=h; }</script></head><body>"
 echo "<div id='container1'><div id='container2'>	<div id='ProcessingControl' class='page'><p class='center'>"
 
#~ This section will start / stop control and print results to screen
if [ "$COMMAND" == "start" ];
then
	RESULT="$(/bin/bash ../../Control/StartControlNohup.sh)"
	PIDOF="$(pidof Control)"
	if [ "$PIDOF" != "" ];
	then
		echo "Successfully started Control software"
		echo "</p><button class='pos_right' onclick=\"link('./index.html#driveEstop')\">OK</button>"
	else
		echo "!! There was a problem starting Control software<p>"
		echo $PIDOF
		echo "</p><p>"
		echo $RESULT
		echo "</p><button class='pos_right' onclick=\"link('./index.html#welcome')\">OK</button>"
	fi
elif [ "$COMMAND" == "stop" ];
then
	RESULT="$(sudo killall Control)"
	PIDOF="$(pidof Control)"
	if [ "$PIDOF" == "" ];
	then
		echo "Successfully stopped Control software"
	else 
		echo "!! There was a problem stopping Control software"
		echo "</p><button class='pos_left' onclick=\"link('./controlcontrol.cgi?command=stop')\">Try Again</button>"
	fi
	echo "</p><button class='pos_right' onclick=\"link('./index.html#exitAfterEstop')\">OK</button>"
fi
 
 echo "</div></div></div>"
 echo "</body></html>"
