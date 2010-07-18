var pageRefreshPeriod = 5000; // ms
var svg_element = null;
var g_element = null;
var title_element = null;
var value_element = null;
var graph_element = null;
var pad_element = null;
var age_element = null;
var coords_element = null;
var follow_element = null;
var zoom_element = null;
var span1_element = null;
var span2_element = null;
var span3_element = null;
var alertCount;
var dragging = false;
var dragStart = { x:0, y:0 };
var cursorOverGraph = false;
var following   = true;
var autoZooming = true;
var oldTransform;
var previousPoint = { time:null, value:null };
var monitorables = null;

var xAxis;
var yAxis;

var XmlDoc = document.implementation.createDocument("", "", null);
XmlDoc.addEventListener("load", xmlDocLoaded, false);

var Clock     = null;
var Subsystem = null;
// var XmlUrl    = document.URL;
var DataURL   = null; // = document.URL;

var ProgBar = null;

function onLoad( refreshPeriod, dataURL ) {

  //alert('onLoad:' + dataURL );
  pageRefreshPeriod = refreshPeriod;
  DataURL = dataURL;

  try{ span1_element = document.getElementById("span1"); } catch(ex){}
  try{ span2_element = document.getElementById("span2"); } catch(ex){}
  try{ span3_element = document.getElementById("span3"); } catch(ex){}
  alertCount = 0;
  document.onmousemove = mouseMove;
  document.onmouseup = onMouseUp;

  // Time development graph, if any
  graph_element = document.getElementById("graph");
  if ( graph_element ){
    svg_element = document.getElementById("svgRoot");
    g_element = document.getElementById("transformer");
    if ( !g_element ) g_element = document.getElementById("scaler");
    title_element = document.getElementById("title");
    value_element = document.getElementById("lastValue");
    pad_element = document.getElementById("pad");
    age_element = document.getElementById("ageOfGraph");
    follow_element = document.getElementById("follow");
    zoom_element = document.getElementById("zoom");

    var d = new Date();
    var t = d.getTime();

    // In FF 2 we need an additional scaler as viewBox doesn't work :-(
    var xScale = 1.;
    var yScale = 1.;
    try { 
      var scaler_element = document.getElementById("scaler");
      xScale = scaler_element.transform.animVal.getItem(0).matrix.a;
      yScale = scaler_element.transform.animVal.getItem(0).matrix.d;
    } catch(ex) {}

    try{	
      // Times in millisec!
      xAxis = new Axis( t-300000, t+300000, 5, true,
			Number(pad_element.getAttribute('x')),
			Number(pad_element.getAttribute('x'))+Number(pad_element.getAttribute('width')),
			1./svg_element.getScreenCTM().a/xScale );
      yAxis = new Axis( 0., 1., 5, false,
			Number(pad_element.getAttribute('y'))+Number(pad_element.getAttribute('height')),
			Number(pad_element.getAttribute('y')),
			1./svg_element.getScreenCTM().d/yScale );
      redrawAxes();
    } catch(ex) {
      //alert( ex );
    }

    attachListeners();
  }

  // Progress bar, if any
  var progBar_td_element = document.getElementById('td_progBar');
  if ( progBar_td_element ) ProgBar = new ProgressBar( progBar_td_element );
  window.onresize = resizeWindow;

  ageOfPageClock(0);

  if ( DataURL.search('fmt=json') > 0 ) autoReloadJsonDoc();
  else                                  autoReloadXmlDoc();
}

function resizeWindow(){
  if ( ProgBar ) ProgBar.widenToMax();
}

function attachListeners(){
    coords_element = document.getElementById("pointerCoords");
    pad_element = document.getElementById("pad");
    pad_element.onmousedown = onMouseDownOnPad;
    graph_element.onmouseover = onMouseOverGraph;
    graph_element.onmouseout = onMouseOffGraph;
    pad_element.addEventListener('DOMMouseScroll', onMouseWheelOnPad, false);
    follow_element.onmousedown = onMouseDownOnFollowButton;
    zoom_element.onmousedown = onMouseDownOnZoomButton;
}

function onMouseDownOnPad(e){
    dragStart = { x:e.pageX, y:e.pageY };
    if ( document.body ) document.body.style.cursor = 'move';
    dragging = true;
    graph_element.transform.animVal.consolidate();
    oldTransform = graph_element.getAttribute( "transform" );
    if (span1_element) span1_element.innerHTML = 'mousedown: ' + oldTransform + xAxis.print() + '   ' + yAxis.print();
    printSvgSvg();
}

function onMouseDownOnFollowButton(){
    var follow_rect  = document.getElementById("follow_rect");
    var follow_text  = document.getElementById("follow_text");
    var follow_edge1 = document.getElementById("follow_edge1");
    var follow_edge2 = document.getElementById("follow_edge2");
    if ( !following ){
	// Start following
	follow_rect.setAttribute( 'fill', '#dddddd' );
	follow_edge1.setAttribute( 'fill', '#888888' );
	follow_edge2.setAttribute( 'fill', '#ffffff' );
	follow_text.setAttribute( 'fill', 'black' );
	follow_text.setAttribute( 'title', 'click to stop following last point' );
	follow_rect.setAttribute( 'title', 'click to stop following last point' );
	follow_text.firstChild.nodeValue = 'freeze'
	following = true;
    } else {
	// Stop following
	follow_rect.setAttribute( 'fill', '#ffffaa' );
	follow_edge1.setAttribute( 'fill', '#996644' );
	follow_edge2.setAttribute( 'fill', '#ffffee' );
	follow_text.setAttribute( 'fill', '#ffaa55' );
	follow_text.setAttribute( 'title', 'click to keep last point in view' );
	follow_rect.setAttribute( 'title', 'click to keep last point in view' );
	follow_text.firstChild.nodeValue = 'follow'
	following = false;
    }
}

function onMouseDownOnZoomButton(){
    var zoom_rect = document.getElementById("zoom_rect");
    var zoom_text = document.getElementById("zoom_text");
    if ( autoZooming ){
	// Disable automatic zoom
	zoom_text.setAttribute( 'title', 'click to let vertical range adjust automatically' );
	zoom_rect.setAttribute( 'title', 'click to let vertical range adjust automatically' );
	zoom_text.firstChild.nodeValue = 'auto range'
	autoZooming = false;
    } else {
	// Enable automatic zoom
	zoom_text.setAttribute( 'title', 'click to fix vertical range' );
	zoom_rect.setAttribute( 'title', 'click to fix vertical range' );
	zoom_text.firstChild.nodeValue = 'fix range'
	autoZooming = true;
    }
}

function onMouseOffGraph(e){
    if ( document.body ) document.body.style.cursor = 'default';
    coords_element.style.visibility = 'hidden';
    cursorOverGraph = false;
}

function onMouseOverGraph(e){
    if ( document.body ) document.body.style.cursor = 'crosshair';
    coords_element.style.visibility = 'visible';
    cursorOverGraph = true;
}


function onMouseUp(e){
    if ( document.body ) document.body.style.cursor = 'default';
    dragging = false;
    //var msg = 'mouseup: ';
    if ( graph_element ){
	 graph_element.transform.baseVal.consolidate();
	 graph_element.transform.animVal.consolidate();
    }
    //msg += ' readout: ' + graph_element.getAttribute("transform") + ' or ' + printTransforms( graph_element.transform.baseVal ) +'   ' + printTransforms( graph_element.transform.animVal );
    if (span2_element) span2_element.innerHTML = xAxis.print() + '   ' + yAxis.print(); 
}

function printTransforms( transformList ){
    var printedList = '';
    for ( i=0; i<transformList.numberOfItems; i++ ){
	printedList = printedList + printSvgMatrix( transformList.getItem(i).matrix ) + ' ';
    }
    return printedList;
}

function printSvgMatrix( matrix ){
    return printedList =  
	'(' + matrix.a +
	' ' + matrix.b +
	' ' + matrix.c +
	' ' + matrix.d +
	' ' + matrix.e +
	' ' + matrix.f +
	    ')';
}


function mouseMove(ev){
    if ( cursorOverGraph ) coords_element.firstChild.nodeValue = '('+xAxis.pixelToValue(ev.pageX,true)+', '+yAxis.pixelToValue(ev.pageY,true)+')';
    if ( dragging ){
	var newTransform = " translate(" + (ev.pageX-dragStart.x)*xAxis.svgPerPixel + " " + (ev.pageY-dragStart.y)*yAxis.svgPerPixel + ") ";
	if ( oldTransform ) newTransform = oldTransform + newTransform;
	graph_element.setAttribute("transform", newTransform );
	try{
	    redrawAxes();
	} catch(ex) {
	  //alert( ex );
	}
	var msg = 'mousemove: ';
	msg += ' readout: ' + graph_element.getAttribute("transform") + ' or ' + printTransforms( graph_element.transform.baseVal ) +'   ' + printTransforms( graph_element.transform.animVal );
	msg += xAxis.print() + '   ' + yAxis.print();
	if (span2_element) span2_element.innerHTML = msg; 
    }
}

function onMouseWheelOnPad(event){
    var newTransform;
    var delta = 0;
    // Delta is multiple of +-3.
    // Or +-1 if Alt is pressed.
    delta = event.detail;
    if (delta){
	switch(delta){
	case -1:
	    // Scale up in X
	    if ( document.body ) document.body.style.cursor = 'w-resize';
 	    scale( 1.25, 1. );
	    break;
	case 1:
	    // Scale down in X
	    if ( document.body ) document.body.style.cursor = 'e-resize';
 	    scale( 0.8, 1. );
	    break;
	case -3:
	    // Scale up in Y
	    if ( document.body ) document.body.style.cursor = 'n-resize';
	    scale( 1., 1.25 );
	    break;    
	case 3:
	    // Scale down in Y
	    if ( document.body ) document.body.style.cursor = 's-resize';
	    scale( 1., 0.8 );
	    break;
	default:
	    if ( document.body ) document.body.style.cursor = 'default';
	}
    }
    // Prevent default actions caused by mouse wheel.
    event.preventDefault();
    event.returnValue = false;
}

function translate( xDistSVG, yDistSVG ){
    var newTransformation = ' translate('+xDistSVG+','+yDistSVG+') ';
    var oldTransformation = graph_element.getAttribute("transform");
    if ( oldTransformation ) newTransformation = oldTransformation + newTransformation;
    graph_element.setAttribute("transform", newTransformation );
    redrawAxes();
}

function scale( xFactor, yFactor ){
    var xToOrigin = 0;
    var yToOrigin = 0;
    if ( graph_element.transform.animVal.numberOfItems ){
	graph_element.transform.baseVal.consolidate();
	graph_element.transform.animVal.consolidate();
	try{ var matrix = graph_element.transform.animVal.getItem(0).matrix; } catch(ex){}// { alert(ex); }
	xToOrigin = matrix.e/matrix.a;
	yToOrigin = matrix.f/matrix.d;
    }
    // Go to the origin, scale, and get back
    var newTransformation = ' translate('+(-xToOrigin)+','+(-yToOrigin)+') scale( '+xFactor+','+yFactor+' ) translate('+xToOrigin+','+yToOrigin+') ';
    var oldTransformation = graph_element.getAttribute("transform");
    if ( oldTransformation ) newTransformation = oldTransformation + newTransformation;
    graph_element.setAttribute("transform", newTransformation );
    redrawAxes();    
}

function displayAxes(){
    var labelElements = document.getElementById("xAxis").childNodes;
    //var dbg = labelElements.length + ' ';
    //try{
    for ( i=0; i<labelElements.length / 2; i++ ){
	labelElements[i                       ].firstChild.nodeValue = xAxis.labels[i].stringValue1;
	labelElements[i+labelElements.length/2].firstChild.nodeValue = xAxis.labels[i].stringValue2;
	//dbg = dbg + ' ' + xAxis.labels[i].value + ' ' + labelElements[i].firstChild.nodeValue;
    }
    //} catch(ex) {
    //	alert(ex + '    ' + dbg);
    //}
    //alert(dbg);
    labelElements = document.getElementById("yAxis").childNodes;
    for ( i=0; i<labelElements.length; i++ ){
	labelElements[i].firstChild.nodeValue = yAxis.labels[i].stringValue1;
    }
}

function redrawAxes(){
  if ( graph_element ){
    graph_element.transform.baseVal.consolidate();
    graph_element.transform.animVal.consolidate();
    if ( graph_element.transform.animVal.numberOfItems ){
	var matrix = graph_element.transform.animVal.getItem(0).matrix
	xAxis.transform( matrix );
	yAxis.transform( matrix );
    } else {
	xAxis.transform( null ); // just for updating everything (will do identity transformation)
	yAxis.transform( null ); // just for updating everything (will do identity transformation)
    }
    displayAxes();
  }
}

function printSvgSvg(){
    var msg = 
	'<table>' +
	'  <tr><td>currentScale </td><td>' + svg_element.currentScale  + '</td></tr>' +
// 	'  <tr><td>pixelUnitToMillimeterX </td><td>' + svg_element.pixelUnitToMillimeterX  + '</td></tr>' +
// 	'  <tr><td>pixelUnitToMillimeterY </td><td>' + svg_element.pixelUnitToMillimeterY  + '</td></tr>' +
// 	'  <tr><td>screenPixelToMillimeterX </td><td>' + svg_element.screenPixelToMillimeterX  + '</td></tr>' +
// 	'  <tr><td>screenPixelToMillimeterY </td><td>' + svg_element.screenPixelToMillimeterY  + '</td></tr>' +
	'  <tr><td>width </td><td>' + svg_element.width.animVal.value  + '</td></tr>' +
	'  <tr><td>height </td><td>' + svg_element.height.animVal.value  + '</td></tr>' +
	'  <tr><td>CTM </td><td>' + printSvgMatrix( svg_element.getCTM() ) + '</td></tr>' +
	'  <tr><td>ScreenCTM </td><td>' + printSvgMatrix( svg_element.getScreenCTM() ) + '</td></tr>' +
	'</table>';
    if (span3_element) span3_element.innerHTML = msg;
}

function followLastPoint( xSVGLastPoint ){
    if ( xSVGLastPoint > xAxis.hiSVG ) translate( xAxis.hiSVG-xSVGLastPoint, 0 );
}

function centerXAxisOnValue( xSVG ){
  translate( 0.5*(xAxis.loSVG+xAxis.hiSVG) - xSVG, 0 );
}

function transformYToFit(){
    // Zoom and pan in y so that each point in range can be seen.
    var minYSVG =  0.1*Number.MAX_VALUE;
    var maxYSVG = -0.1*Number.MAX_VALUE;
    var noPointInRange = true;
    for ( i=0; i<graph_element.points.numberOfItems; i++ ){
	if ( xAxis.loSVG <= graph_element.points.getItem(i).x && graph_element.points.getItem(i).x <= xAxis.hiSVG ){
	    if ( graph_element.points.getItem(i).y < minYSVG ) minYSVG = graph_element.points.getItem(i).y;
	    if ( graph_element.points.getItem(i).y > maxYSVG ) maxYSVG = graph_element.points.getItem(i).y;
	    noPointInRange = false;
	}
    }
    if ( noPointInRange ) return;
    var yShiftSVG = 0.5* ( ( yAxis.loSVG + yAxis.hiSVG ) - ( minYSVG + maxYSVG ) ); // midpoint shift
    var yFactor   = ( yAxis.loSVG - yAxis.hiSVG ) / Math.max( 300., 1.1 * ( maxYSVG -  minYSVG ) );
    translate( 0., yShiftSVG );
    scale( 1., yFactor );
}

function Axis(loValue,hiValue,nLabels,isX,loSVG,hiSVG,svgPerPixel){
    this.loValueRef = loValue; // initial value to be preserved for reference
    this.hiValueRef = hiValue; // initial value to be preserved for reference
    this.loValue = loValue; // current value of lower end
    this.hiValue = hiValue; // current value of higher end

    this.nLabels = nLabels;
    this.isX = isX;

    this.loSVGRef = loSVG; // initial SVG coord to be preserved for reference
    this.hiSVGRef = hiSVG; // initial SVG coord  to be preserved for reference
    this.loSVG = loSVG; // current SVG coord of lower end
    this.hiSVG = hiSVG; // current SVG coord of higher end

    this.svgPerPixelRef   = svgPerPixel;
    this.lengthSVGRef     = this.hiSVGRef - this.loSVGRef;
    this.valuePerPixelRef = ( this.hiValueRef - this.loValueRef ) / this.lengthSVGRef * this.svgPerPixelRef;
    this.svgPerPixel   = this.svgPerPixelRef;
    this.lengthSVG     = this.lengthSVGRef;
    this.valuePerPixel = this.valuePerPixelRef;

    this.labels = new Array(this.nLabels);

    this.updateLabels = function(){
	var increment = ( this.hiValue - this.loValue ) / ( this.nLabels - 1 );
    	for ( i=0; i<this.labels.length; i++ ){
	    if ( this.labels[i] ) this.labels[i].setValue( this.loValue + i * increment );
	    else this.labels[i] = new Label( this.loValue + i * increment, this.isX );
	}
    }

    this.updateScaling = function(){
	this.lengthSVG     = this.hiSVG - this.loSVG;
	this.svgPerPixel   = this.svgPerPixelRef * this.lengthSVG / this.lengthSVGRef;
	this.valuePerPixel = ( this.hiValue - this.loValue ) / this.lengthSVG * this.svgPerPixel;
    }

    this.toSVG = function( value ){
	return this.loSVGRef + ( value - this.loValueRef ) * ( this.hiSVGRef - this.loSVGRef ) / ( this.hiValueRef - this.loValueRef );
    }

    this.toValue = function( SVG ){
	return this.loValueRef + ( SVG - this.loSVGRef ) / ( this.hiSVGRef - this.loSVGRef ) * ( this.hiValueRef - this.loValueRef );
    }

    this.transform = function( svgMatrix ){
	if ( svgMatrix ){
	    if ( this.isX ){
		this.loSVG = ( this.loSVGRef - svgMatrix.e ) / svgMatrix.a;
		this.hiSVG = ( this.hiSVGRef - svgMatrix.e ) / svgMatrix.a;
	    } else {
		this.loSVG = ( this.loSVGRef - svgMatrix.f ) / svgMatrix.d;
		this.hiSVG = ( this.hiSVGRef - svgMatrix.f ) / svgMatrix.d;
	    }
	}
	this.loValue = this.toValue( this.loSVG );
	this.hiValue = this.toValue( this.hiSVG );
	this.updateScaling();
	this.updateLabels();
    }

    this.formatValue = function( value ){
	if ( this.isX ){
	    var d = new Date();
	    d.setTime( value );
	    return d.getFullYear()+'-'+(d.getMonth()+1)+'-'+d.getDate()+' '+d.getHours()+':'+d.getMinutes()+':'+d.getSeconds();
	}
	else { return value.toFixed(4); }
    }

    this.pixelToValue = function( pixel, formatted ){
	var svgMatrix = svg_element.getScreenCTM();
	g_element.transform.baseVal.consolidate();
	g_element.transform.animVal.consolidate();
	var gMatrix = g_element.transform.baseVal.getItem(0).matrix;
	var graphMatrix = null;
	graph_element.transform.baseVal.consolidate();
	graph_element.transform.animVal.consolidate();
	if ( graph_element.transform.animVal.numberOfItems){
	    graphMatrix = graph_element.transform.animVal.getItem(0).matrix
	}
	var SVG;
	if ( this.isX ){
	    // Transform from screen to svg:svg...
	    SVG = ( pixel - svgMatrix.e ) / svgMatrix.a;
	    // ...then from svg:svg to svg:g...
	    SVG = ( SVG - gMatrix.e ) / gMatrix.a;
	    // ...then from svg:g to svg:polyline (the graph)...
	    if ( graphMatrix ) SVG = ( SVG - graphMatrix.e ) / graphMatrix.a;
	} else {
	    // Transform from screen to svg:svg...
	    SVG = ( pixel - svgMatrix.f ) / svgMatrix.d;
	    // ...then from svg:svg to svg:g...
	    SVG = ( SVG - gMatrix.f ) / gMatrix.d;
	    // ...then from svg:g to svg:polyline (the graph)...
	    if ( graphMatrix ) SVG = ( SVG - graphMatrix.f ) / graphMatrix.d;
	}
	// ... and finally to value:
	if ( formatted ) return this.formatValue( this.toValue( SVG ) );
	return this.toValue( SVG );
    }

    this.print = function(){
	msg = 
	    '<table>' +
	    '  <tr><td>isX           </td><td>' + this.isX            + '</td></tr>' +
	    '  <tr><td>loValue       </td><td>' + this.loValue        + '</td></tr>' +
	    '  <tr><td>hiValue       </td><td>' + this.hiValue        + '</td></tr>' +
	    '  <tr><td>loValueRef    </td><td>' + this.loValueRef     + '</td></tr>' +
	    '  <tr><td>hiValueRef    </td><td>' + this.hiValueRef     + '</td></tr>' +
	    '  <tr><td>loSVG         </td><td>' + this.loSVG          + '</td></tr>' +
	    '  <tr><td>hiSVG         </td><td>' + this.hiSVG          + '</td></tr>' +
	    '  <tr><td>loSVGRef      </td><td>' + this.loSVGRef       + '</td></tr>' +
	    '  <tr><td>hiSVGRef      </td><td>' + this.hiSVGRef       + '</td></tr>' +
	    '  <tr><td>lengthSVG     </td><td>' + this.lengthSVG      + '</td></tr>' +
	    '  <tr><td>svgPerPixel   </td><td>' + this.svgPerPixel    + '</td></tr>' +
	    '  <tr><td>valuePerPixel </td><td>' + this.valuePerPixel  + '</td></tr>' +
	    '</table>';
	return msg;
    }

}


function Label(value,isTime){
    this.value = 0;
    this.isTime = isTime;
    this.stringValue1 = '';
    this.stringValue2 = ''; // YYYY-MM-DD if isTime; null otherwise

    this.zeroPadTo2 = function( number ){
	if ( number <= 9 ) return '0'+number.toString();
	return number.toString();
    }

    this.setValue = function(v){
	this.value = v;
  	if ( isTime ){
//  	if ( false ){
	    var d = new Date();
	    d.setTime(this.value);
	    this.stringValue1 = this.zeroPadTo2(d.getHours())+':'+this.zeroPadTo2(d.getMinutes())+':'+this.zeroPadTo2(d.getSeconds());
	    this.stringValue2 = d.getFullYear().toString().substr(2,2)+'-'+this.zeroPadTo2(d.getMonth()+1)+'-'+this.zeroPadTo2(d.getDate());
	} else {
	    if      ( Math.abs( this.value ) > 1000. ) this.stringValue1 = this.value.toFixed(0);
	    else if ( Math.abs( this.value ) >  100. ) this.stringValue1 = this.value.toFixed(1);
	    else if ( Math.abs( this.value ) >   10. ) this.stringValue1 = this.value.toFixed(2);
	    else                                       this.stringValue1 = this.value.toFixed(3);

	}
    }

    this.setValue( value );
}

function appendPoint( p ){
    if ( !graph_element ) return;

    var xSVG = xAxis.toSVG( p.time );
    var ySVG;
    ySVG = yAxis.toSVG( p.value );
    title_element.firstChild.nodeValue = p.name;
    value_element.firstChild.nodeValue = Number( p.value ).toFixed(4);

    var oldPoints = graph_element.getAttribute("points");
    if ( oldPoints ) graph_element.setAttribute("points", oldPoints+' '+xSVG+','+ySVG );
    else             graph_element.setAttribute("points", xSVG+','+ySVG );
    //var newPoints = graph_element.getAttribute("points");
    previousPoint.time  = p.time;
    previousPoint.value = p.value;
    // Jump to the first point (in case the client's clock is not set correctly or is in another time zone):
    if ( oldPoints.length == 0 ) centerXAxisOnValue( xSVG );
    if ( following ) followLastPoint( xSVG );
    if ( autoZooming ) transformYToFit();
}

function toUnixTime( dateTime ){
    var d = new Date();
    if ( dateTime.length == 19 ){
	 // YYYY-MM-DD hh:mm:ss
	 d.setFullYear( Number( dateTime.substr( 0,4) )     );
	 d.setMonth   ( Number( dateTime.substr( 5,2) ) - 1 );
	 d.setDate    ( Number( dateTime.substr( 8,2) )     );
	 d.setHours   ( Number( dateTime.substr(11,2) )     );
	 d.setMinutes ( Number( dateTime.substr(14,2) )     );
	 d.setSeconds ( Number( dateTime.substr(17,2) )     );
    }
    else {
	 var months = { 'Jan':1, 'Feb':2, 'Mar':3, 'Apr':4, 'May':5, 'Jun':6, 'Jul':7, 'Aug':8, 'Sep':9, 'Oct':10, 'Nov':11, 'Dec':12 }
	 // Wed, Jun 10 2009 21:47:35 GMT
	 d.setUTCFullYear( Number( dateTime.substr(12,4) )     );
	 d.setUTCMonth   ( months[ dateTime.substr( 5,3) ] - 1 );
	 d.setUTCDate    ( Number( dateTime.substr( 9,2) )     );
	 d.setUTCHours   ( Number( dateTime.substr(17,2) )     );
	 d.setUTCMinutes ( Number( dateTime.substr(20,2) )     );
	 d.setUTCSeconds ( Number( dateTime.substr(23,2) )     );
    }
    return d.getTime();
}


function Monitorable( time, name, value, nameDescr, valueDescr, nameURL, valueURL ){
  this.time       = time;
  this.name       = name;
  this.value      = value;
  this.nameDescr  = nameDescr;
  this.valueDescr = valueDescr;
  this.nameURL    = nameURL;
  this.valueURL   = valueURL;
  
  this.previousTime  = this.time;
  this.previousValue = this.value;
  this.prevPrevTime  = this.time;
  this.prevPrevValue = this.value;
  
  this.set = function( time, name, value, nameDescr, valueDescr, nameURL, valueURL ){
    this.prevPrevTime  = this.previousTime;
    this.prevPrevValue = this.previousValue;
    this.previousTime  = this.time;
    this.previousValue = this.value;

    this.time       = time;
    this.value      = value;
    this.nameDescr  = nameDescr;
    this.valueDescr = valueDescr;
    this.nameURL    = nameURL;
    this.valueURL   = valueURL;
  };

  this.rate = function(){
    if ( this.time == this.previousTime ) return 0;
    return Math.max( 0, (this.value-this.previousValue) * 1000 / (this.time-this.previousTime) ); // ms --> s
  };

  this.rate2 = function(){ // rate calculated over two samplings
    if ( this.time == this.prevPrevTime ) return 0;
    return Math.max( 0, (this.value-this.prevPrevValue) * 1000 / (this.time-this.prevPrevTime) ); // ms --> s
  };
}


function ageOfPageClock(ageOfPage){
    hours=Math.floor(ageOfPage/3600);
    minutes=Math.floor(ageOfPage/60)%60;
    // graph
    var age="";
    if (hours) age+=hours+"h ";
    if (minutes) age+=minutes+"m ";
    age+=ageOfPage%60+"s ";
    if ( age_element ) age_element.firstChild.nodeValue = age+'ago';
    // table
    try{
	age="";
	if (hours) age+=hours+"h&nbsp;";
	if (minutes) age+=minutes+"m&nbsp;";
	age+=ageOfPage%60+"s&nbsp;";
	document.getElementById('td_ageOfPage').innerHTML='Loaded&nbsp;'+age+'ago';
	var mainTableElem = document.getElementById('fadingTable');
	if      ( ageOfPage < 0.003 * pageRefreshPeriod ) mainTableElem.className = 'fresh';
	else if ( ageOfPage < 0.010 * pageRefreshPeriod ) mainTableElem.className = 'aging';
	else                                              mainTableElem.className = 'stale';
    } catch(ex){}// { alert(ex); }
    ageOfPage=ageOfPage+1;
    Clock = setTimeout('ageOfPageClock('+ageOfPage+')',1000);
}

function ProgressBar( containing_td_element ){
  this.containing_td_element = containing_td_element;
  this.g_element = document.getElementById('gProgBarScaler');
  this.table_element = this.containing_td_element.parentNode.parentNode;
  this.reference_row_element;
  this.saved_row_element;
  this.rowIndex = this.containing_td_element.parentNode.rowIndex;

  this.baseContainerWidth  = this.containing_td_element.clientWidth;
  this.viewPortWidth = document.getElementById('svgProgressBar').width.baseVal.value;
  this.baseRectWidth  = document.getElementById('rectProgFrame').width.baseVal.value;

  this.widenToMax = function(){
    // Widen the progress bars to the width of the containing able cell, but not wider than the SVG viewport
    var animContainerWidth = this.containing_td_element.clientWidth;
    //this.table_element.getElementsByTagName('th')[0].innerHTML = this.containing_td_element+' '+this.baseRectWidth+' '+animContainerWidth+' '+this.viewPortWidth;
    this.g_element.setAttribute( 'transform', 'scale('+Math.min(animContainerWidth,this.viewPortWidth)/this.baseRectWidth+',1.0)' );
  };

  this.show = function(){
    //alert('ProgBar.show() '+this.saved_row_element+' '+this.reference_row_element);
    if ( this.saved_row_element ){
      if ( this.reference_row_element ) this.table_element.insertBefore( this.saved_row_element, this.reference_row_element );
      else                              this.table_element.appendChild( this.saved_row_element );
      // Update the element pointers whose pointees have been recreated:
      this.containing_td_element = this.saved_row_element.getElementsByTagName('td')[0];
      this.g_element = document.getElementById('gProgBarScaler');
      // Widen it to the max avaliable width
      this.widenToMax();
    }
  };

  this.hide = function(){
    //alert('ProgBar.hide() '+this.rowIndex+' '+this.table_element.rows.length+' '+this.table_element.rows[this.rowIndex]+' '+this.table_element.rows[this.rowIndex].getElementsByTagName('td')[0].id );
    if ( this.table_element.rows.length > this.rowIndex+1 ){
      if ( this.table_element.rows[this.rowIndex].getElementsByTagName('td')[0].id == 'td_progBar' ) this.reference_row_element = this.table_element.rows[this.rowIndex+1];
      this.saved_row_element = this.table_element.rows[this.rowIndex].cloneNode(true);
      this.table_element.deleteRow( this.rowIndex );
    }
  };

  this.set = function( iRun, nRuns, iStep, nSteps ){
    if ( document.getElementById('barRuns' ) ){
      //this.table_element.getElementsByTagName('th')[0].innerHTML = 'Setting bars to'+' '+iRun+' '+nRuns+' '+iStep+' '+nSteps;
      document.getElementById('barRuns' ).setAttribute('width',this.baseRectWidth/Math.max(1,nRuns) *iRun );
      document.getElementById('barSteps').setAttribute('width',this.baseRectWidth/Math.max(1,nSteps)*iStep);
      document.getElementById('textRuns' ).firstChild.nodeValue = 'run ' +iRun +'/'+nRuns;
      document.getElementById('textSteps').firstChild.nodeValue = 'step '+iStep+'/'+nSteps;
      this.containing_td_element.title = 'Progress of calibration runs: run '+iRun +'/'+nRuns+', step '+iStep+'/'+nSteps;
      this.g_element.title = 'Progress of calibration runs: run '+iRun +'/'+nRuns+', step '+iStep+'/'+nSteps;
    }
    //else this.table_element.getElementsByTagName('th')[0].innerHTML = 'No progress bar';
  };

  this.widenToMax();
}

//================
// XML data access
//================


function autoReloadXmlDoc(){
    XmlDoc.load( DataURL );
    setTimeout('autoReloadXmlDoc()', pageRefreshPeriod);
}

function xmlDocLoaded(e){
    //alert('xmlDocLoaded: event type '+e.type+' target '+e.target);
    var xmlIsOK = false;
    try{
	xmlIsOK = valuesFromXml();
    } catch(e) {
      //alert('Caught exception thrown from valuesFromXml: '+e.message);
    }
    if ( xmlIsOK ){
	clearTimeout(Clock);
	ageOfPageClock(0);
	redrawAxes();
    }
}

function findArrayElementByName( name, array ){
    for (i=0; i<array.length; i++) if ( array[i].name == name ) return array[i];
    return null;
}

function valuesFromXml(){
    var rootElement = XmlDoc.getElementsByTagName('ForEmuPage1');
    if ( !rootElement.length ) return false;
    document.getElementById('td_localDateTime').innerHTML = rootElement[0].getAttribute('localDateTime');
    var time;

    if ( rootElement[0].getAttribute('localUnixTime') ) time = Number( rootElement[0].getAttribute('localUnixTime') ) * 1000; // ms in JavaScript! 
    else time = toUnixTime( rootElement[0].getAttribute('localDateTime') );

    var mElements = XmlDoc.getElementsByTagName('monitorable');
    //alert( mElements.length );
    if ( !mElements.length ) return false;

    var graphPoint = { name:'', time:time, value:Number(0) };

    var state = '';

    if ( monitorables == null ) monitorables = new Array( mElements.length );

    for ( i=0; i<mElements.length; i++ ){
      if ( monitorables[i] == null ) monitorables[i] = new Monitorable( time,
									mElements[i].getAttribute('name'),
									mElements[i].getAttribute('value'),
									mElements[i].getAttribute('nameDescription'),
									mElements[i].getAttribute('valueDescription'),
									mElements[i].getAttribute('nameURL'),
									mElements[i].getAttribute('valueURL') );
      else                          monitorables[i].set( time,
							 mElements[i].getAttribute('name'),
							 mElements[i].getAttribute('value'),
							 mElements[i].getAttribute('nameDescription'),
							 mElements[i].getAttribute('valueDescription'),
							 mElements[i].getAttribute('nameURL'),
							 mElements[i].getAttribute('valueURL') );
      if ( monitorables[i].name == 'state' ){ state = monitorables[i].value; }
    }

    for ( i=0; i<monitorables.length; i++ ){
        // Graph:
	if ( monitorables[i].name == 'max events' || monitorables[i].name == 'Heartbeat' ){
	  graphPoint.name  = monitorables[i].name + ' rate [Hz]';
	  graphPoint.value = monitorables[i].rate();
	}
	else if ( monitorables[i].name == 'quality' ){
	  graphPoint.name  = monitorables[i].name;
	  graphPoint.value = monitorables[i].value;
	}
	else if ( monitorables[i].name == 'TF Errors'  || monitorables[i].name == 'ME- Errors' || monitorables[i].name == 'ME+ Errors' ){
	  graphPoint.name   = 'all errors'
	  graphPoint.value += Number(monitorables[i].value);
	}
	else if ( monitorables[i].name.indexOf(' Input Rate') > 0 ){
	  graphPoint.name  = 'DCC input [kB/s]';
	  graphPoint.value = Number(monitorables[i].value)*0.001;
	}
	// Table:
	// name
	var th_name = document.getElementById( 'th_name_'+i );
	if ( th_name ) th_name.className = monitorables[i].name;
	var a_name = document.getElementById( 'a_name_'+i );
	if ( a_name ){
	  a_name.href = monitorables[i].nameURL;
	  a_name.title = monitorables[i].nameDescr;
	  if ( monitorables[i].name.indexOf(' Rate') < 0 ) a_name.innerHTML = monitorables[i].name.replace(/ME-/g,'ME&#8211;').replace(/^([^ ]*) (Errors|Configuration)$/,'$1').replace(/^([^ ]*) events$/,'$1').replace(/^([^ ]*)rate$/,'$1').replace(/ /g,'&#160;');
	}
	// value
	var td_value = document.getElementById( 'td_value_'+i );
	if ( td_value ){
	  td_value.className = monitorables[i].value;
	  if ( ( monitorables[i].name == 'min events' || monitorables[i].name.indexOf('rate') > 0 ) 
	       && state == 'Enabled' ) td_value.className += ( monitorables[i].value == 0 ? ' WARN' : '' );
	  if ( monitorables[i].name == 'Heartbeat' ) td_value.className += ( monitorables[i].rate2() == 0 ? ' WARN' : ' ON' );
	  else if ( monitorables[i].name == 'TF Errors'  || monitorables[i].name == 'ME- Errors' || monitorables[i].name == 'ME+ Errors' ){
	    td_value.className += ( monitorables[i].rate() > 0 ? ' WARN' : '' );
	  }
	}
	var a_value = document.getElementById( 'a_value_'+i );
	if ( a_value ){
	  a_value.href = monitorables[i].valueURL;
	  a_value.title = monitorables[i].valueDescr;
	  if ( monitorables[i].name == 'Heartbeat' ) a_value.innerHTML = monitorables[i].rate().toFixed(4) + '&#160;Hz';
	  else if ( monitorables[i].name.indexOf('rate') > 0 ) a_value.innerHTML = monitorables[i].value + '&#160;Hz';
	  else if ( monitorables[i].name.indexOf(' Rate') > 0 ) a_value.innerHTML = (Number(monitorables[i].value)*0.001).toFixed(3) + '&#160;kB/s';
	  else a_value.innerHTML = monitorables[i].value;
	}
        // progress bar
	if ( monitorables[i].name == 'type' ){
            //ProgBar.table_element.getElementsByTagName('th')[0].innerHTML += ' '+monitorables[i].name;
            if ( monitorables[i].value.indexOf('Calib') >= 0 ){	      
              if ( ProgBar ){
		ProgBar.show();
		ProgBar.set( findArrayElementByName( 'calib runIndex' , monitorables ).value,
			     findArrayElementByName( 'calib nRuns'    , monitorables ).value,
			     findArrayElementByName( 'calib stepIndex', monitorables ).value,
			     findArrayElementByName( 'calib nSteps'   , monitorables ).value  );
	      }
            }
	    else if ( ProgBar ) ProgBar.hide();
        }


    }
    // Append point to graph
    appendPoint( graphPoint );
    return true;
}

//=================
// JSON data access
//=================

function autoReloadJsonDoc(){
  //alert('autoReloadJsonDoc()');
  if ( DataURL.search('diskInfo') > 0 ) diskUsageFromJson();
  else                                  TrackFinderFromJson();
  setTimeout('autoReloadJsonDoc()', pageRefreshPeriod);
}

function Disk( host, mount, time, state, usage, free ){
  this.host  = host ;
  this.mount = mount;
  this.time  = time ;
  this.state = state;
  this.usage = usage; // [%]
  this.free  = free ; // [MB]
}

function byUsage( a, b ){
  return a.usage < b.usage;
}

function byHost( a, b ){
  return a.host > b.host;
}

function diskUsageFromJson(){
  //alert( 'diskUsageFromJson' );


  // Get data disk info
  $.getJSON( DataURL, function(json) {
    var time = toUnixTime( json.table.properties.LastUpdate );
    $('#td_localDateTime').text( timeToString( time ) );
    var disks  = new Array();
    $.each( json.table.rows, function(i,contextRow){
      if ( contextRow.context.lastIndexOf(':9999') > 0 ){
	$.each( contextRow.diskUsage.rows, function(j,fsRow){ 
	  if ( fsRow.fileSystem == '/data' ){
	    var host = contextRow.context.match('^http://([^:]+):[0-9]+')[1];
	    disks.push( new Disk( host, '/data', fsRow.sampleTime, fsRow.state, 
				  fsRow.usePercent, (1.-0.01*fsRow.usePercent)*fsRow.totalMB ) );
	  }
	});
      }
    });
    // Display disk of highest usage
    var d = disks.sort(byUsage)[0];
    var klass = 'ON';
    if      ( d.usage > 80 ) klass = 'WARN';
    else if ( d.usage > 95 ) klass = 'OFF';
    $('#td_value_0').attr('class',klass);
    $('#a_value_0').attr('title',d.host+':'+d.mount+' has '+d.free.toFixed(0)+' MB free left at '+d.time+'.').text(d.usage.toFixed(0)+' %');

    clearTimeout(Clock);
    ageOfPageClock(0);
  });

}

function zeroPadTo2( number ){
     return ( number < 10 ? '0' : '' )+number.toString();
}

function timeToString( time ){
     var d = new Date();
     d.setTime( time );
     return    zeroPadTo2( d.getFullYear() )
	  +'-'+zeroPadTo2( d.getMonth()+1  )
	  +'-'+zeroPadTo2( d.getDate()     )
	  +' '+zeroPadTo2( d.getHours()    )
	  +':'+zeroPadTo2( d.getMinutes()  )
	  +':'+zeroPadTo2( d.getSeconds()  );
}

function TrackFinderFromJson(){
     //alert( 'TrackFinderFromJson' );


  // Get info on Track Finder
  $.getJSON( DataURL, function(json) {
    var time = toUnixTime( json.table.properties.LastUpdate );
    $('#td_localDateTime').text( timeToString( time ) );
    var msg = '';
    $.each( json.table.rows, function(i,row){
      if ( i == 0 ){
	msg += 'FSM_STATE.rows.length='+row.FSM_STATE.rows.length+'   EMUPAGEONE_RATES.rows.length='+row.EMUPAGEONE_RATES.rows.length;
	var validConfPatterns = ['^EmuLocal$','^CSCTF-Conf'];
	var foundGlobalConf = false;
	var foundLocalConf  = false;
	for ( p=0; p<validConfPatterns.length; p++ ){
	  $.each( row.FSM_STATE.rows, function(j,configRow){
	    if ( configRow['id'].search(validConfPatterns[p])==0 ){
	      $('#td_value_state').attr( 'class', configRow['state'] );
	      $('#a_value_state').text( configRow['state'] );
	      $('#a_value_state').attr( 'title', 'Operation \"'+configRow['id']+'\" is in '+configRow['state']+' state.' );
	      $('#td_value_confkey').attr( 'class', configRow['conf_key'] );
	      $('#a_value_confkey').text( configRow['conf_key'] );
	      $('#a_value_confkey').attr( 'title', 'Operation \"'+configRow['id']+'\" has configuration key '+configRow['conf_key']+'.' );
	      if ( p==0 ) foundGlobalConf = true;
	      else        foundLocalConf  = true;
	    }
	  });
	}
	if ( !foundGlobalConf && !foundLocalConf ){
	  $('#td_value_state').attr( 'class', 'UNKNOWN' );
	  $('#a_value_state').text( 'UNKNOWN' );
	  $('#a_value_state').attr( 'title', (row.FSM_STATE.rows.length==0?'No ':' Only invalid ')+'operation found.' );
	  $('#td_value_confkey').attr( 'class', 'UNKNOWN' );
	  $('#a_value_confkey').text( 'UNKNOWN' );
	  $('#a_value_confkey').attr( 'title', (row.FSM_STATE.rows.length==0?'No ':' Only invalid ')+'operation found.' );
	}
// 	if ( foundGlobalConf && foundLocalConf ){
// 	  $('#td_value_state').attr( 'class', 'INDEFINITE' );
// 	  $('#a_value_state').text( 'INDEFINITE' );
// 	  $('#a_value_state').attr( 'title', 'Both global and local operations found. Click to destroy \"EmuLocal\", or consult the Trigger shifter to have the global one destroyed.' );
// 	  $('#td_value_confkey').attr( 'class', 'INDEFINITE' );
// 	  $('#a_value_confkey').text( 'INDEFINITE' );
// 	  $('#a_value_confkey').attr( 'title', 'Both global and local operations found. Click to destroy \"EmuLocal\", or consult the Trigger shifter to have the global one destroyed.' );
// 	}
	$.each( row.EMUPAGEONE_RATES.rows, function(j,ratesRow){ 
	  if ( j == 0 ){
	    var graphPoint = { name:'total SP input rate [Hz]', time:time, value:ratesRow['Total SPs Rate'] };
	    appendPoint( graphPoint );
	    $('#a_value_min').text( ratesRow['Min Single SP Rate'] + ' Hz' );
	    if ( ratesRow['Min Single SP Rate'] == 0 && ratesRow['Total SPs Rate'] > 100 ){
	      $('#td_value_min').attr('class', 'WARN' );
	      $('#a_value_min').attr('title', 'One or more SPs may be dead. Click to check.' );
	    }
	    else{
	      $('#td_value_min').attr('class', '');
	      $('#a_value_min').attr('title', '');
	    }
	    $('#a_value_total').text( ratesRow['Total SPs Rate'] + ' Hz' );
	  }
	});
      }
    });

    //if ( alertCount == 0 ){ alert( msg ); alertCount=1; }

    clearTimeout(Clock);
    ageOfPageClock(0);
  });

}
