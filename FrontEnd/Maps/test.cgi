#!/bin/bash


# You must add following two lines before
# outputting data to the web browser from shell
# script
#~ <meta http-equiv='refresh' content='10;url=./index.html'><script type='text/javascript'>window.location.href = './index.html'</script>
#~ <meta http-equiv='refresh' content='10;url=./index.html#autoEstop'><script type='text/javascript'>window.location.href = './index.html#autoEstop'</script>



 echo "Content-type: text/html"
 echo ""
 echo "<html><head><title>Demo</title><link rel='stylesheet' type='text/css' href='index.css'><script>function link(h) {	window.location=h; }</script></head><body>"
 echo "<div id='container1'><div id='container2'>	<div id='ProcessingControl' class='page'><p class='center'>$(whoami)</p>s"

 echo "</div></div></div>"
 echo "</body></html>"
