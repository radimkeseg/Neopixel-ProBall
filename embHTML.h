/**The MIT License (MIT)
Copyright (c) 2017 by Radim Keseg
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

const char PAGE_INDEX[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8">
	<title>NeoPixel ProBall</title>

<script>
 var simplePicker = {
   colorize: function(color, destID){
     if(color.match(/^#[0-9a-f]{3,6}$/)){
     document.getElementById(destID).style.backgroundColor = color;
     document.getElementById(destID).className = "color-box";
   }else{
     document.getElementById(destID).style.backgroundColor = "";
     document.getElementById(destID).className = "color-box color-box-error";
   }
   },
   
   init: function(){
    this.colorize('{val-color-hours}','box-color-hours');
    this.colorize('{val-color-quarters}','box-color-quarters');
    this.colorize('{val-color-alarm}','box-color-alarm');
   }   
  };
  
  window.onload = function(){
    simplePicker.init();
  }
</script>
 
<style>
.color-box{
    width: 32px;
    height: 20px;
    display: inline-block;
    border: 1px solid #000;
    margin-bottom: -7px;
}
.color-box-error{
    border: 1px solid #f00;
}
</style>

<body>
<h1>NeoPixel ProBall</h1>
<p>
Neopixel ProBall with an ESP8266 IoT device
<p/>

<form method='post' action='offset'>
<label>UTC TimeOffset<br><input id='_timeoffset' name='_timeoffset' length=5 pattern='^[0-9-\\.]{1,5}$' required value='{timeoffset}'></label><br>
<label>DST<br><input id='_dst' name='_dst' type='checkbox' {dst}></label><br>
<br/>
<label>Brightness by daylight<br><input id='_brightness' name='_brightness' length=3 pattern='^[0-9]{3,0}$' value='{brightness}'></label><br>
<label>Brightness at night<br><input id='_brightness_night' name='_brightness_night' length=3 pattern='^[0-9]{3,0}$' value='{brightness_night}'>22:00-06:00</label><br>
<br/>
<lable>Clock colors:</label><br/>
<label><input id="input-color-hours" type="text" pattern="#[0-9a-f]{3,6}" name="_input-color-hours" title="e.g. #f00 or #ff0000" value="{val-color-hours}" onkeyup="simplePicker.colorize(this.value,'box-color-hours')" ><div class="color-box" id="box-color-hours"></div> hours</label><br/>
<label><input id="input-color-quarters" type="text" pattern="#[0-9a-f]{3,6}" name="_input-color-quarters" title="e.g. #0a0 or #00aa00" value="{val-color-quarters}" onkeyup="simplePicker.colorize(this.value,'box-color-quarters')"><div class="color-box" id="box-color-quarters"></div> quarters</label><br/>
<label>Animate<br><input id='_animate' name='_animate' type='checkbox' {animate}></label><br>
<br/>
<label>ALARM<br><input id='_alarm' name='_alarm' type='checkbox' {alarm}></label><br>
<label><input id="input-color-alarm" type="text" pattern="#[0-9a-f]{3,6}" name="_input-color-alarm" title="e.g. #f0f or #ff00ff" value="{val-color-alarm}" onkeyup="simplePicker.colorize(this.value,'box-color-alarm')" ><div class="color-box" id="box-color-alarm"></div> alarm</label><br/>
<label>Time<br><input id='_alarmHour' name='_alarmHour' length=2 pattern='^[0-9]{2,0}$' value='{alarmHour}'>:<input id='_alarmMins' name='_alarmMins' length=2 pattern='^[0-9]{2,0}$' value='{alarmMins}'></label>
<br/>
<input type='submit' value='Store'></form>


</body>
</html>
)=====";
