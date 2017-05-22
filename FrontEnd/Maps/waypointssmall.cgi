#!/usr/bin/perl

use strict;

use CGI qw/:standard/;
use CGI::Carp qw(fatalsToBrowser warningsToBrowser);

use Math::Trig;

my $TwoPi = 4*acos(0);
my $EARTH_RADIUS = 6371000;


sub LatLongToXY {

	my($lat,$lng,$DatumLat,$DatumLong) = @_;

	my @Result;

	$Result[1] = -1*$EARTH_RADIUS*$TwoPi*($DatumLat - $lat)/360;
	$Result[0] = -1*$EARTH_RADIUS*cos(abs($lat))*$TwoPi*($DatumLong - $lng)/360;

	return @Result;

}

sub XYToLatLong {

	my($x,$y,$DatumLat,$DatumLong) = @_;

	my @Result;

	$Result[0] = 1*$y*360/($EARTH_RADIUS*$TwoPi) + $DatumLat;
	$Result[1] = 1*$x*360/($EARTH_RADIUS*$TwoPi*cos(abs($Result[0]))) + $DatumLong;

	return @Result;

}

print header;
warningsToBrowser(1);

my $Error;

my $Data;

my $CurrentName="none";

if(param("save") eq 'Save') {

	if(param("mapname") =~ /^[0-9a-z_]+$/i) {

		$CurrentName = param("mapname") . ".wyp";

		my ($DatumLat, $DatumLong);

		open (FILE,">./maps/" . $CurrentName);

		my @MarkersText = split(/\n/,param("markerstext"));
		
		foreach my $Line (@MarkersText) {

			my @LineParts = split(/,/, $Line);

			if($LineParts[0] eq 'D') { 
				$DatumLat = $LineParts[1]; 
				$DatumLong = $LineParts[2]; 
				print FILE $Line . "\n";			
			}
			else { next; }

		}

		foreach my $Line (@MarkersText) {

			my @LineParts = split(/,/, $Line);

			if($LineParts[0] eq 'D') { next; }
			else { 
				my @Point = LatLongToXY($LineParts[1], $LineParts[2], $DatumLat, $DatumLong);
				print FILE $LineParts[0] . "," . $Point[0] . "," . $Point[1] . "\n";
			}

		}



		close FILE;

	}

	else { $Error = "Bad map name"; }

}

if((param("openfilename") ne $CurrentName && param("openfilename") ne 'none') || param("save") eq 'Save') {

	#if(param("open") eq 'Open') { $CurrentName = param("openfilename"); }
	if((param("openfilename") ne $CurrentName && param("openfilename") ne 'none')) { $CurrentName = param("openfilename"); }


	open (FILE,"./maps/$CurrentName");
	my @lines = <FILE>;
	
	my ($DatumLat, $DatumLong);

	foreach my $line (@lines) {

		my @LineParts = split(/,/, $line);

		if($LineParts[0] eq 'D') { 
			$DatumLat = $LineParts[1]; 
			$DatumLong = $LineParts[2]; 	
		}
		else { next; }

	}

	foreach my $line (@lines) {

		$line =~ s/\n//g;
		my @LineParts = split(/,/, $line);

		if($LineParts[0] eq 'D') { $Data .= $line . "\n"; }
		else { 
			
			my ($Lat, $Long) = XYToLatLong($LineParts[1],$LineParts[2],$DatumLat,$DatumLong);
			$Data .= $LineParts[0] . "," . $Lat . "," . $Long . "\n";

		}

	}


	close FILE;

}




print <<END;

<!DOCTYPE html>
<html>
  <head>
	<title>Auto Interface Small</title>
    <meta name="viewport" content="initial-scale=1.0, user-scalable=no" />
	<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
    <style type="text/css">
      #map-canvas { height: 100%; }
    </style>

  <link rel="stylesheet" type="text/css" href="waypoints.css"/>


    <script type="text/javascript"
      src="https://maps.googleapis.com/maps/api/js?key=AIzaSyBUx74Zh1SWTbx4wxEjyWCxjJg4MaA8FUI&amp;sensor=true">
    </script>	
<script type="text/javascript" src="jquery-2.0.3.js">
  	</script>
    <script src="RGraph/libraries/RGraph.common.core.js" ></script>
    <script src="RGraph/libraries/RGraph.scatter_improved.js" ></script>
    <script src="RGraph/libraries/RGraph.common.dynamic.js" ></script>
    <script src="RGraph/libraries/RGraph.common.tooltips.js" ></script>
	<script src="jquery.tools.min.js"></script>
    <script type="text/javascript">

\$(function() {
    // setup ul.tabs to work as tabs for each div directly under div.panes
    \$("ul.tabs").tabs("div.panes > div");
});

window.onload = function ()
        {
        	setTimeout(updateLog,1000);	
		
        }

var map;

var markers = [];
var fencemarkers = [];
var baseframe = [];

var offsetLat = 0;
var offsetLong = 0;

var greenCross = {url: 'cross.png'};
var blueDot = {url: 'blue-dot.png'};

var redCross = {url: 'redcross.png'};

var currentLocationMarker;
var datumMarker;



function initialize() {

	var defaultPos = new google.maps.LatLng(-31.980569, 115.817807);
	
	
        
	var mapOptions = {
		center: defaultPos,
		zoom: 17,
		mapTypeId: google.maps.MapTypeId.SATELLITE
        };

	map = new google.maps.Map(document.getElementById("map-canvas"), mapOptions);

	google.maps.event.addListener(map, 'click', function(event) { logLocation(event.latLng); });

	var zeroposition = new google.maps.LatLng(0, 0);

	currentLocationMarker = new google.maps.Marker({
		position: zeroposition,
		map: map,
		raiseOnDrag: false,
		icon: blueDot,
	});

	datumMarker = new google.maps.Marker({
		position: zeroposition,
		map: map,
		raiseOnDrag: false,
	});

	if(document.getElementById("markerstext").value != "") { updateMarkersFromText(); }
	else { setDatumLocation(defaultPos); }


}

function logLocation(location) {

	if(document.getElementById("markertype").options[document.getElementById("markertype").selectedIndex].value == 0) {
		logMarkerLocation(location);
	}
	else if(document.getElementById("markertype").options[document.getElementById("markertype").selectedIndex].value == 3) {
		setDatumLocation(location);
	}
	else { logFenceLocation(location); }

}


function logMarkerLocation(location) {
 

	var marker = new google.maps.Marker({
		position: location,
		map: map,
		draggable: true,
		raiseOnDrag: false,
		icon: greenCross,
	});

	google.maps.event.addListener(marker, 'rightclick',  function() { removeMarker(marker); });

	markers.splice(document.getElementById("nextposn").value,0,marker);

	document.getElementById("nextposn").value++;

	updateMarkersText();

}

function logFenceLocation(location) {

	var circleOptions = {
      		strokeColor: '#FF0000',
      		strokeOpacity: 0.8,
      		strokeWeight: 2,
      		fillColor: '#FF0000',
      		fillOpacity: 0.35,
      		map: map,
      		center: location,
      		radius: 1.5
    	};

	var fenceCircle = new google.maps.Circle(circleOptions);

	google.maps.event.addListener(fenceCircle, 'rightclick',  function() { removeFenceMarker(fenceCircle); });

	fencemarkers.push(fenceCircle);

	updateMarkersText();

}

function setDatumLocation(location) {

	datumMarker.setPosition(location);
	map.setCenter(location);

	updateMarkersText();
	
	sendCommand('SETDATUM,' + (datumMarker.getPosition().lat() - offsetLat) + ',' + (datumMarker.getPosition().lng() - offsetLong));

}


function updateMarkersText() {

	var markersString = "D," + (datumMarker.getPosition().lat() - offsetLat) + "," + (datumMarker.getPosition().lng() - offsetLong) + "\\n";

	for(i in markers) {

		markersString += i + "," + (markers[i].getPosition().lat() - offsetLat) + "," + (markers[i].getPosition().lng() - offsetLong) + "\\n";

		markers[i].setTitle(i.toString());

	}

	for(i in fencemarkers) {

		markersString += "F" + i + "," + (fencemarkers[i].getCenter().lat() - offsetLat) + "," + (fencemarkers[i].getCenter().lng() - offsetLong) + "\\n";

	}



	document.getElementById("markerstext").value = markersString;

}

function updateMarkersFromText() {

	var lines = document.getElementById("markerstext").value.split("\\n");

	clearMap();

	for(i in lines) {

		if(lines[i] == '') { continue; }

		var lineParts = lines[i].split(",");

		var position = new google.maps.LatLng(parseFloat(lineParts[1]) + offsetLat,parseFloat(lineParts[2]) + offsetLong);

		if(lineParts[0].substring(0,1) == "F") { logFenceLocation(position); }
		else if(lineParts[0].substring(0,1) == "D") { setDatumLocation(position); }
		else { logMarkerLocation(position); }

	}

}


function clearMap() {

	document.getElementById("nextposn").value = 0;

	while(markers.length > 0) {
		for(i in markers) {
			removeMarker(markers[i]);
		}
	}

	while(fencemarkers.length > 0) {
		for(i in fencemarkers) {
			removeFenceMarker(fencemarkers[i]);
		}
	}

}

function removeMarker(marker) { 

	marker.setMap(null);

	var index = markers.indexOf(marker);

	markers.splice(index,1);

	updateMarkersText();
	
}

function removeFenceMarker(marker) { 

	marker.setMap(null);

	var index = fencemarkers.indexOf(marker);

	fencemarkers.splice(index,1);

	updateMarkersText();
	
}




google.maps.event.addDomListener(window, 'load', initialize);


var xyscale = 1;

function updateLog() {

	\$.ajax({
		type: "GET"
		,url: "getcarinfo.cgi"
		,dataType: "json"
		,cache: false
		,success: function(json){

				document.getElementById("logarea").innerHTML = json.log;

				document.getElementById("paramarea").innerHTML = "";

				var msgstrings = new Array(35).join('0').split('');

        			for(var key in json.params) {
         				var attrName = key;
            				var attrValue = json.params[key].content;
					msgstrings[json.params[key].order] = "<b>" + attrName + "</b>: " + attrValue + "<br />";
					
        			}

				document.getElementById("paramarea").innerHTML = msgstrings.join('');				

				var position = new google.maps.LatLng(parseFloat(json.gps.lat) - offsetLat, parseFloat(json.gps.long) - offsetLong);
				currentLocationMarker.setPosition(position);

				drawXYGraph(json);

                            }
		,error: function() { alert("AJAX Error!"); }
	});

	setTimeout(updateLog,1000);

}

function updateBaseFrame() {

	\$.ajax({
		type: "GET"
		,url: "getbaseframe.cgi"
		,dataType: "json"
		,cache: false
		,success: function(json){
			drawbaseframe(json);
                            }
		,error: function() { alert("AJAX Error!"); }
	});

}

function loadMap() {

	\$.ajax({
		type: "POST",
		url: "sendcommand.cgi",
		data: "command=LOADMAP," + document.getElementById("openfilename").options[document.getElementById("openfilename").selectedIndex].value,
		success: function() {
			updateBaseFrame();
			},
		dataType: "text",
		error: function() { alert("AJAX IPC send Error!"); }
	});


}

function setGPSOffset() {

	\$.ajax({
		type: "POST",
		url: "sendcommand.cgi",
		data: "command=SETGPSOFF," + document.getElementById("latoffset").value + "," + document.getElementById("longoffset").value,
		success: function() { },
		dataType: "text",
		error: function() { alert("AJAX IPC send Error!"); }
	});

}


function setMapOffset() {

	offsetLat = parseFloat(document.getElementById("maplatoffset").value);
	offsetLong = parseFloat(document.getElementById("maplongoffset").value);

	updateMarkersFromText();

}

function addCurPos() {

	logLocation(currentLocationMarker.getPosition());

}

function sendCommand(Command) {

	\$.ajax({
		type: "POST",
		url: "sendcommand.cgi",
		data: "command=" + Command,
		success: function() { },
		dataType: "text",
		error: function() { alert("AJAX IPC send Error!"); }
	});


}

function drawbaseframe(json) {
	
	while(baseframe.length > 0) {
		baseframe.pop();
	}
	
	for (i = 0; i < json.mapdata.length; i++) {
		baseframe[baseframe.length] = new google.maps.LatLng(json.mapdata[i][0],json.mapdata[i][1]);
	}
	var mapbaseframe = new google.maps.Polyline({
		path: baseframe,
		geodesic: true,
		strokeColor: '#FF0000',
		strokeOpacity: 1.0,
		strokeWeight: 2
	});

  mapbaseframe.setMap(map);
}

function drawXYGraph(json)
        {

		RGraph.Reset(document.getElementById('cvs'));
	var HeadingVector = [[parseFloat(json.params["Fused X Pos"].content,10),parseFloat(json.params["Fused Y Pos"].content,10),"clear"],[parseFloat(json.params["Fused X Pos"].content,10) + 3*Math.sin(2*Math.PI*parseFloat(json.params["Fused Heading"].content,10)/360.0),parseFloat(json.params["Fused Y Pos"].content,10) + 3*Math.cos(2*Math.PI*parseFloat(json.params["Fused Heading"].content,10)/360.0),"clear"]];
	
	if (json.params["Auto On"].content == 1) {
		var NextWPVector = [[parseFloat(json.params["Fused X Pos"].content,10),parseFloat(json.params["Fused Y Pos"].content,10),"clear"],[parseFloat(json.nextwp.x,10),parseFloat(json.nextwp.y,10),"clear"]];
	}
	else {var NextWPVector = [];}	

	var DesiredHeadingVector = [[parseFloat(json.params["Fused X Pos"].content,10),parseFloat(json.params["Fused Y Pos"].content,10),"clear"],[parseFloat(json.params["Fused X Pos"].content,10) + 3*Math.sin(2*Math.PI*parseFloat(json.params["Desired Bearing"].content,10)/360.0),parseFloat(json.params["Fused Y Pos"].content,10) + 3*Math.cos(2*Math.PI*parseFloat(json.params["Desired Bearing"].content,10)/360.0),"clear"]];

	var scatter = new RGraph.Scatter2('cvs', json.mapdata, HeadingVector,DesiredHeadingVector,NextWPVector)
		.Set('scale.decimals', 1)
		.Set('xscale.decimals', 1)
                .Set('xscale', true)
		.Set('chart.gutter.left', 60)
		.Set('chart.line', true)
		.Set('chart.line.linewidth', 1.5)
		.Set('chart.line.colors', [null,"blue","red","orange"])
                .Draw();

	xyscale = scatter.GetScale();

        }

function resizeMap() {

if(document.getElementById("map-canvas").style.display == "none") { setTimeout(resizeMap,50); }
else { google.maps.event.trigger(map, 'resize'); }

}

isRecording=0; // toggle for the toggleRecord function
function toggleRecord() {
	if(isRecording==0) // start recording
	{
		document.getElementById("recorder").src='img/recording.png';
		isRecording=1;
		sendCommand('STARTREC');
	} else {
		document.getElementById("recorder").src='img/notrecording.png';
		isRecording=0;
		var mapName = prompt("Would you like to save the map?", "MapName");
		
		if (mapName != null) {
			sendCommand('STOPREC,' + mapName);
		} else {
			sendCommand('STOPREC,DELETERECORDING');
		}
	}
}

isAuto=0; // toggle for the toggleAuto function
function toggleAuto() {
	if(isAuto==0) { // then start auto
		document.getElementById("auto").src='img/stopauto.png';
		isAuto=1;
		sendCommand('TOGBIL')
		sendCommand('AUTOSTART');
	} else {
		document.getElementById("auto").src='img/startauto.png';
		isAuto=0;
		sendCommand('AUTOSTOP');
		sendCommand('TOGBIL')
	}
}
	 
function link(h) {
	window.location=h;
}

    </script>
  </head>
  <body>

<form method="POST" action="waypointssmall.cgi">
<table  border="1"  width="100%">
<tr><td>



<select id="openfilename" name="openfilename" onchange="this.form.submit()"  style=" width: 55px;  height: 40px;">

END

opendir(DIR,"./maps/");
my @Files = readdir(DIR);
close DIR;

foreach my $File (@Files) {

	if($File =~ /^\.+/) { next; }

	else { 
		if($File eq $CurrentName) { print "<option selected=\"selected\" value=\"$File\">$File</option>"; }
		else { print "<option value=\"$File\">$File</option>"; }
	}

}

print <<END;

</select>

<br />
<button type="button" onclick="loadMap(); return false;"> <img id="load" src="img/loadmap.png" width="30" height="30"> </button>
<br />
<button type="button" onclick="toggleAuto(); return false;"> <img id="auto" src="img/startauto.png" width="30" height="30"> </button>
<br />
<button type="button" onclick="sendCommand('UNTRIP'); return false;"> <img id="untrip" src="img/untrip.png" width="30" height="30"> </button>
<br />
<button type="button" onclick="toggleRecord(); return false;"> <img id="recorder" src="img/notrecording.png" width="30" height="30"> </button>
<br />
<button type="button" onclick="link('./controlcontrol.cgi?command=stop');"> <img id="stopcontrol" src="img/stopcontrol.png" width="30" height="30"> </button>
<br />
<button type="button" onclick="sendCommand('TOGBIL'); return false;"> <img id="togbil" src="img/togbil.png" width="30" height="30"> </button>
<!-- ----------------  START OF HIDDEN CODE  -------------- -->

<input type="submit" style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>
<input type="button" onclick="loadMap()" name="load" value="Load into Control"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>
<input type="text" id="nextposn" size="4" value="0"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>
<input type="button" onclick="clearMap()" value="Clear"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/> <input type="button" onclick="updateMarkersFromText()" value="Update Map"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/> 

<select id="markertype" style="position: absolute; left: -9999px; width: 1px; height: 1px;"><option value="0">Waypoint</option><option value="1">Fence Post</option><option selected="selected" value="3">Datum</option></select>

<input type="button" value="Add current pos" onclick="addCurPos()"   style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>

<textarea name="markerstext" id="markerstext" rows="15" cols="45" style="position: absolute; left: -9999px; width: 1px; height: 1px;">
END

print $Data;

$CurrentName =~ s/\.wyp//;

print <<END; 
</textarea> 

<input type="text" size="20" name="mapname" value="$CurrentName"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>

<input type="submit" name="save" value="Save"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>

<input type="text" size="6" id="maplatoffset" name="maplatoffset" value="0.0"   style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>
<input type="text" size="6" id="maplongoffset" name="maplongoffset" value="0.0"   style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>
<input type="button" onclick="setMapOffset()" name="start" value="Set Map Offset"   style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>

<input type="button" onclick="sendCommand('AUTOSTART')" name="start" value="Start Auto"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>
<input type="button" onclick="sendCommand('AUTOSTOP')" name="stop" value="STOP Auto"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>
<input type="button" onclick="sendCommand('AUTOPAUSE')" name="pause" value="Pause Auto"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>
<input type="button" onclick="sendCommand('AUTOCONT')" name="cont" value="Continue Auto"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>

<input type="text" size="6" id="mapname" name="mapname" value=""  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>
<input type="button" onclick="sendCommand('STOPREC,' + document.getElementById('mapname').value)" name="cont" value="Finish Recording"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>
<input type="button" onclick="sendCommand('STARTREC')" name="pause" value="Start Recording" style="position: absolute; left: -9999px; width: 1px; height: 1px;"/>

<input type="button" onclick="sendCommand('UNTRIP')" name="cont" value="Reset Trip"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/> 

<input type="button" onclick="sendCommand('TOGBIL')" name="cont" value="Toggle BrakeIL"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/> 
<input type="button" onclick="sendCommand('SETDATUM,' + (datumMarker.getPosition().lat() - offsetLat) + ',' + (datumMarker.getPosition().lng() - offsetLong))" name="cont" value="Set Datum"  style="position: absolute; left: -9999px; width: 1px; height: 1px;"/> 
<input type="button" onclick="sendCommand('ESTOP')" name="estop" value="ESTOP Car"  style="background-color : red; position: absolute; left: -9999px; width: 1px; height: 1px;"/>

<!-- ----------------  END OF HIDDEN CODE  -------------- -->

</td>

    <td height="210px" width="395px">

<ul class="tabs" style="display: inline-block; ">
	<li><a href="#" onclick="resizeMap()">Map</a></li>
	<li><a href="#">Control</a></li>
	<li><a href="#">Log</a></li>
	<li><a href="#">Status</a></li>
</ul>

<span style="display: inline-block; float:right;" class="tabs"><a href="luxplot4.htm" target="_blank">IBEO</a></span>

<!-- tab "panes" -->
<div class="panes">
	<div style="display: block; height : 210px; width : 395px;" id="map-canvas"></div>
	<div><canvas id="cvs" width="395px" height="210px">[No canvas support]</canvas><br /><b><font color="blue">Car Heading</font> <font color="red">Desired Heading</font> <font color="orange">Desired Vector</font></b></div>
	<div style="display: block; height : 210px; width : 395px;" id="Log"><textarea style="height : 100%; width : 100%; overflow: auto;"  id="logarea"></textarea></div>
	<div style="height : 210px; width : 395px; overflow: auto;" id="paramarea"></div>
</div>

</td>
</tr>

</table>

</form>


    

END

if($Error) { 
print <<END;

<script type="text/javascript">alert("$Error");</script>

END

}


print <<END;

  </body>
</html>

END
