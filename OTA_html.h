//html files readable

//header file readable
const char html_header[] PROGMEM = R"=====(
<html><head><meta charset="UTF-8"><title>ESP-GPS-Logger</title>
 <style>
        .button {
            background-color: #04aa6d; /* Green */
            border: none;
            color: white;
            padding: 8px 20px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 4px 2px;
            cursor: pointer;
            border-radius: 8px;
        }
        .button_del {
            background-color: #ab7373; /* red */
            border: none;
            color: white;
            padding: 8px 20px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 4px 2px;
            cursor: pointer;
            border-radius: 8px;
        }
        #navbar,
        #navbar-menu {
            position: fixed;
            left: 0;
            right: 0;
        }

        :root {
            --navbar-bg-color: hsl(0, 0%, 15%);
            --navbar-text-color: hsl(0, 0%, 85%);
            --navbar-text-color-focus: white;
            --navbar-bg-contrast: hsl(0, 0%, 25%);
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }

        body {
            height: 100vh;
            font-family: Arial, Helvetica, sans-serif;
            line-height: 1.6;
        }

        .container {
            max-width: 1000px;
            padding-left: 1.4rem;
            padding-right: 1.4rem;
            margin-left: auto;
            margin-right: auto;
        }

        #navbar {
            --navbar-height: 64px;
            height: var(--navbar-height);
            background-color: var(--navbar-bg-color);
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.15);
        }

        .navbar-container {
            display: flex;
            justify-content: space-between;
            height: 100%;
            align-items: center;
        }

        .navbar-item {
            margin: 0.4em;
            width: 100%;
        }

        .home-link,
        .navbar-link {
            color: var(--navbar-text-color);
            text-decoration: none;
            display: flex;
            font-weight: 400;
            align-items: center;
        }

        .home-link:is(:focus, :hover) {
            color: var(--navbar-text-color-focus);
        }

        .navbar-link {
            justify-content: center;
            width: 100%;
            padding: 0.4em 0.8em;
            border-radius: 5px;
        }

        .navbar-link:is(:focus, :hover) {
            color: var(--navbar-text-color-focus);
            background-color: var(--navbar-bg-contrast);
        }

        .navbar-logo {
            background-color: var(--navbar-text-color-focus);
            border-radius: 50%;
            width: 30px;
            height: 30px;
            margin-right: 0.5em;
        }

        #navbar-toggle {
            cursor: pointer;
            border: none;
            background-color: transparent;
            width: 40px;
            height: 40px;
            display: flex;
            align-items: center;
            justify-content: center;
            flex-direction: column;
        }

        .icon-bar {
            display: block;
            width: 25px;
            height: 4px;
            margin: 2px;
            background-color: var(--navbar-text-color);
        }

        #navbar-toggle:is(:focus, :hover) .icon-bar {
            background-color: var(--navbar-text-color-focus);
        }

        #navbar-toggle[aria-expanded="true"] .icon-bar:is(:first-child, :last-child) {
            position: absolute;
            margin: 0;
            width: 30px;
        }

        #navbar-toggle[aria-expanded="true"] .icon-bar:first-child {
            transform: rotate(45deg);
        }

        #navbar-toggle[aria-expanded="true"] .icon-bar:nth-child(2) {
            opacity: 0;
        }

        #navbar-toggle[aria-expanded="true"] .icon-bar:last-child {
            transform: rotate(-45deg);
        }

        #navbar-menu {
            top: var(--navbar-height);
            bottom: 0;
            opacity: 0;
            visibility: hidden;
        }

        #navbar-toggle[aria-expanded="true"] + #navbar-menu {
            background-color: rgba(0, 0, 0, 0.4);
            opacity: 1;
            visibility: visible;
        }

        .navbar-links {
            list-style: none;
            position: absolute;
            background-color: var(--navbar-bg-color);
            display: flex;
            flex-direction: column;
            align-items: center;
            left: 0;
            right: 0;
            margin: 1.4rem;
            border-radius: 5px;
            box-shadow: 0 0 20px rgba(0, 0, 0, 0.3);
        }

        #navbar-toggle[aria-expanded="true"] + #navbar-menu .navbar-links {
            padding: 1em;
        }

        @media screen and (min-width: 700px) {
            #navbar-toggle,
            #navbar-toggle[aria-expanded="true"] {
                display: none;
            }

            #navbar-menu,
            #navbar-toggle[aria-expanded="true"] #navbar-menu {
                visibility: visible;
                opacity: 1;
                position: static;
                display: block;
                height: 100%;
            }

            #navbar-toggle[aria-expanded="true"] #navbar-menu .navbar-links,
            .navbar-links {
                margin: 0;
                padding: 0;
                box-shadow: none;
                position: static;
                flex-direction: row;
                width: 100%;
                height: 100%;
            }
        }

        #esplogger {
            font-family: Arial, Helvetica, sans-serif;
            border-collapse: collapse;
            width: 100%;
        }

        #esplogger td,
        #esplogger th {
            border: 1px solid #ddd;
            padding: 8px;
        }

        #esplogger tr:nth-child(2n) {
            background-color: #f2f2f2;
        }

        #esplogger tr:hover {
            background-color: #ddd;
        }

        #esplogger th {
            padding-top: 12px;
            padding-bottom: 12px;
            text-align: left;
            background-color: #04aa6d;
            color: #fff;
        }
 </style>
 <meta name="viewport" content="width=device-width, initial-scale=1">
    <!--<meta name="description" content="ketwords" />-->
 </head><body><header id="navbar">
  <nav class="navbar-container container">
    <a href="/" class="home-link">
      <div class="navbar-logo"></div>
      ESP-GPS-Logger 
      </a>
    <button type="button" id="navbar-toggle" aria-controls="navbar-menu" aria-label="Toggle menu" aria-expanded="false">
      <span class="icon-bar"></span>
      <span class="icon-bar"></span>
      <span class="icon-bar"></span>
    </button>
    <div id="navbar-menu" aria-labelledby="navbar-toggle">
      <ul class="navbar-links">
        <li class="navbar-item"><a class="navbar-link" href='https://github.com/RP6conrad/ESP-GPS-Logger' target='_blank'>Github</a></li>
        <li class="navbar-item"><a class="navbar-link" href="/">Files</a></li>
        <li class="navbar-item"><a class="navbar-link" href="/upload">Upload</a></li>
        <li class="navbar-item"><a class="navbar-link" href="/config">Configuration</a></li>
        <li class="navbar-item"><a class="navbar-link" href="/firmware">Firmware</a></li>
      </ul>
    </div>
  </nav>
</header>
<script>
    const navbarToggle = navbar.querySelector("#navbar-toggle"),
        navbarMenu = document.querySelector("#navbar-menu"),
        navbarLinksContainer = navbarMenu.querySelector(".navbar-links");
    let isNavbarExpanded = "true" === navbarToggle.getAttribute("aria-expanded");
    const toggleNavbarVisibility = () => {
        (isNavbarExpanded = !isNavbarExpanded), navbarToggle.setAttribute("aria-expanded", isNavbarExpanded);
    };
    navbarToggle.addEventListener("click", toggleNavbarVisibility), navbarLinksContainer.addEventListener("click", (a) => a.stopPropagation()), navbarMenu.addEventListener("click", toggleNavbarVisibility);

    function confirmdelete() {
        return !0 == confirm("Want to delete?");
    }
</script>
<br><br><br>
  )=====";


//Config Part
const char html_config_header[] PROGMEM = R"=====(
<FORM action='/configupload' method='post'>
    <table id='esplogger'>
        <tr>
            <th>Name</th>
            <th>Value</th>
            <th>Info</th>
        </tr>
  )=====";

//config fill the current config Info
void html_config(String& webpage){
  //cal_bat
  webpage += "<tr>\n<td>cal_bat</td><td>\n";
  webpage += "<input size='4' type='number' required name='cal_bat' min='1.7' max='1.89' value="+String(config.cal_bat)+" step='0.01'>\n";
  webpage += "</select>\n</td><td>cal_bat: is the calibration <br> of the battery voltage measurement (1.7-1.8).</td>\n</tr>\n";
  
  //cal_speed 
  webpage += "<tr><td>cal_speed</td><td>\n<select id='cal_speed' name='cal_speed' type='number'>\n";
  if(config.cal_speed == 3.6) webpage += "<option value='3.60' selected>3.6 km/h</option>\n"; else webpage += "<option value=3.60>3.6 km/h</option>\n";
  if(config.cal_speed <2) webpage += "<option value='1.9438' selected>1.9438 knots</option>\n"; else webpage += "<option value=1.9438>1.9438 knots</option>\n";
  webpage += "</select>\n</td><td>cal_speed: is for the conversion from gps unit m/s to km/h (3.6)or knots (1.9438).</td>\n</tr>\n";
  
  //sample_rate
  webpage += "<tr>\n<td>sample_rate(Hz)</td><td>\n<select id='sample_rate' name='sample_rate'>";
  if(config.sample_rate == 1) webpage += "<option value='1' selected>1 Hz</option>\n"; else  webpage += "<option value='1'>1 Hz</option>\n";
  if(config.sample_rate == 2) webpage += "<option value='2' selected>2 Hz</option>\n"; else webpage += "<option value='2'>2 Hz</option>\n";
  if(config.sample_rate == 5) webpage += "<option value='5' selected>5 Hz</option>\n"; else webpage += "<option value='5'>5 Hz</option>\n";
  if(config.sample_rate == 10) webpage += "<option value='10' selected>10 Hz</option>\n"; else webpage += "<option value='10'>10 Hz</option>\n";
  webpage += "</select>\n</td><td>sample_rate: can be 1,2,5 or 10Hz. The higher, the more accurate,<br> but also the larger the files become! One UBX NavPVT message is 100byte, <br>so at 1Hz this gives a file of 360kb/hour, at 10Hz 3.6Mb/hour!</td>\n</tr>\n";

  //gnss
  webpage += "<tr><td>gnss</td><td>\n<select id='gnss' name='gnss'>\n";
  if(config.gnss == 2) webpage += "<option value='2' selected>GPS + GLONAS</option>\n"; else webpage += "<option value='2'>GPS + GLONAS</option>\n";
  if(config.gnss == 3 | config.gnss == 11) webpage += "<option value='3' selected>GPS + GLONAS + GALILEO</option>\n"; else webpage += "<option value='3'>GPS + GLONAS + GALILEO</option>\n";
  webpage += "</select>\n</td><td>gnss: gnss = 3 : GPS + GLONAS + GALILEO (only possible with ublox ROM version 3.01 !!). <br> Default is only GPS + GLONAS used. Some Beitian modules still have a old firmware, ROM 2.01. Here, Galileo can't be activated.</td>\n</tr>\n";
  Serial.println("gnss: "+String(config.gnss)+"\n");
  //speed_field
  webpage += "<tr><td>speed_field</td><td>\n<select id='speed_field' name='speed_field'>\n";
  if(config.field == 1) webpage += "<option value='1' selected>1</option>\n"; else webpage += "<option value='1'>1</option>\n";
  if(config.field == 2) webpage += "<option value='2' selected>2</option>\n"; else webpage += "<option value='2'>2</option>\n";
  if(config.field == 3) webpage += "<option value='3' selected>3</option>\n"; else webpage += "<option value='3'>3</option>\n";
  if(config.field == 4) webpage += "<option value='4' selected>4</option>\n"; else webpage += "<option value='4'>4</option>\n";
  if(config.field == 5) webpage += "<option value='5' selected>5</option>\n"; else webpage += "<option value='5'>5</option>\n";
  if(config.field == 6) webpage += "<option value='6' selected>6</option>\n"; else webpage += "<option value='6'>6</option>\n";
  if(config.field == 7) webpage += "<option value='7' selected>7</option>\n"; else webpage += "<option value='7'>7</option>\n";
  if(config.field == 8) webpage += "<option value='8' selected>8</option>\n"; else webpage += "<option value='8'>8</option>\n";
  webpage += "</select>\n</td><td>speed_field: The preferred value in the first line of the speed screen : 1=Auto switching between Run, Alfa & NM, 2=Run & NM, 3=Alfa, 4=NM, 5= Total distance, 6= 2s/10s, 7= 0.5h, 8= 1h</td>\n</tr>\n";
   //speed_large_font
  webpage += "<tr><td>speed_large_font</td><td>\n<select id='speed_large_font' name='speed_large_font'>\n";
  if(config.speed_large_font == 1) webpage += "<option value='1' selected>Large_Font ON</option>\n"; else webpage += "<option value='1'>Large Font ON</option>\n";
  if(config.speed_large_font == 0) webpage += "<option value='0' selected>Large Font OFF</option>\n"; else webpage += "<option value='0'>Large Font OFF</option>\n";
  webpage += "</select>\n</td><td>speed_large_font: To choose the font sizes of the first line in the speed screen</td>\n</tr>\n";  
  //bar_length
  webpage += "<tr>\n<td>bar_length</td><td>\n";
  webpage += "<input size='8' type='number' required name='bar_length' min='100' max='10000' value="+String(config.bar_length)+" step='1'>\n";
  webpage += "</select>\n</td><td>bar_length: Default length = 1852 m for 100% bar (=Nautical mile)</td>\n</tr>\n";
  //Stat_screens
  webpage += "<tr>\n<td>Stat_screens</td><td>\n";
  webpage += "<input size='8' type='number' required name='Stat_screens' min='0' max='1000' value="+String(config.Stat_screens_persist)+" step='1'>\n";
  webpage += "</select>\n</td><td>Stat_screens choice : every digit shows the according stat_screen after each other</td>\n</tr>\n";
  //Stat_speed
  webpage += "<tr>\n<td>stat_speed</td><td>\n";
  webpage += "<input size='8' type='number' required name='stat_speed' min='0' max='1000' value="+String(config.stat_speed)+" step='1'>\n";
  webpage += "</select>\n</td><td>If the actual speed(in m/s) is less then this stat_speed, stat_screens are active</td>\n</tr>\n";
  //GPIO12_screens
  webpage += "<tr>\n<td>GPIO12_screens</td><td>\n";
  webpage += "<input size='8' type='number' required name='GPIO12_screens' min='0' max='1000' value="+String(config.GPIO12_screens_persist)+" step='1'>\n";
  webpage += "</select>\n</td><td>GPIO12_screens choice : every digit shows the according GPIO_screen after each push</td>\n</tr>\n";
  //Board_Logo
  webpage += "<tr>\n<td>Board_Logo</td><td>\n";
  webpage += "<input size='8' type='number' required name='Board_Logo' min='0' max='1000' value="+String(config.Board_Logo)+" step='1'>\n";
  webpage += "</select>\n</td><td>Board_Logo: from 1 - 99. See the info on <a href='https://www.seabreeze.com.au/img/photos/windsurfing/19375156.jpg' target='_blank'>this Link</a> >100 are different single logos</td>\n</tr>\n";
  //Sail_Logo
  webpage += "<tr>\n<td>Sail_Logo</td><td>\n";
  webpage += "<input size='8' type='number' required name='Sail_Logo' min='0' max='1000' value="+String(config.Sail_Logo)+" step='1'>\n";
  webpage += "</select>\n</td><td>Sail_Logo: from 1 - 99. See the info on <a href='https://www.seabreeze.com.au/img/photos/windsurfing/19375156.jpg' target='_blank'>this Link</a> >100 are different single logos</td>\n</tr>\n";
  //sleep_off_screen
  webpage += "<tr>\n<td>sleep_off_screen</td><td>\n";
  webpage += "<input size='8' type='number' required name='sleep_off_screen' min='0' max='1000' value="+String(config.sleep_off_screen)+" step='1'>\n";
  webpage += "</select>\n</td><td>Choice for switch_off (first digit 0 or 1) and sleep_screen (second digit 0 or 1): </td>\n</tr>\n";
  //logSBP
  webpage += "<tr><td>logSBP</td><td>\n<select id='logSBP' name='logSBP'>\n";
  if(config.logSBP == 1) webpage += "<option value='1' selected>LOG SBP ON</option>\n"; else webpage += "<option value='1'>LOG SBP ON</option>\n";
  if(config.logSBP == 0) webpage += "<option value='0' selected>LOG SBP OFF</option>\n"; else webpage += "<option value='0'>LOG SBP OFF</option>\n";
  webpage += "</select>\n</td><td>logSBP: To save the GPS data in sbp format. This is also the file that you can upload to gp3s. The sbp format is ca 70% smaller then ubx.</td>\n</tr>\n";  
  //logUBX
  webpage += "<tr><td>logUBX</td><td>\n<select id='logUBX' name='logUBX'>\n";
  if(config.logUBX == 1) webpage += "<option value='1' selected>LOG UBX ON</option>\n"; else webpage += "<option value='1'>LOG UBX ON</option>\n";
  if(config.logUBX == 0) webpage += "<option value='0' selected>LOG UBX OFF</option>\n"; else webpage += "<option value='0'>LOG UBX OFF</option>\n";
  webpage += "</select>\n</td><td>logUBX: To save the GPS data in ubx format. This is also the file that you can upload to gp3s.</td>\n</tr>\n";  
  //logGPY  
  webpage += "<tr><td>logGPY</td><td>\n<select id='logGPY' name='logGPY'>\n";
  if(config.logGPY == 1) webpage += "<option value='1' selected>LOG GPY ON</option>\n"; else webpage += "<option value='1'>LOG GPY ON</option>\n";
  if(config.logGPY == 0) webpage += "<option value='0' selected>LOG GPY OFF</option>\n"; else webpage += "<option value='0'>LOG GPY OFF</option>\n";
  webpage += "</select>\n</td><td>logGPY: To save the GPS data in a new open source format .gpy, not yet defined. </td>\n</tr>\n";  
  //logGPX  
  webpage += "<tr><td>logGPX</td><td>\n<select id='logGPX' name='logGPX'>\n";
  if(config.logGPX == 1) webpage += "<option value='1' selected>LOG GPX ON</option>\n"; else webpage += "<option value='1'>LOG GPX ON</option>\n";
  if(config.logGPX == 0) webpage += "<option value='0' selected>LOG GPX OFF</option>\n"; else webpage += "<option value='0'>LOG GPX OFF</option>\n";
  webpage += "</select>\n</td><td>logGPX: To save the GPS data in gpx format @ 1Hz, for video overlay or other purposes.</td>\n</tr>\n";  
  //dynamic_model
  webpage += "<tr><td>dynamic_model</td><td>\n<select id='dynamic_model' name='dynamic_model'>\n";
  if(config.dynamic_model == 0) webpage += "<option value='0' selected>portable</option>\n"; else webpage += "<option value='0'>portable</option>\n";
  if(config.dynamic_model == 1) webpage += "<option value='1' selected>sea</option>\n"; else webpage += "<option value='1'>sea</option>\n";
  if(config.dynamic_model == 2) webpage += "<option value='2' selected>automotive</option>\n"; else webpage += "<option value='2'>automotive</option>\n";
  webpage += "</select>\n</td><td>dynamic_model: Here you can choose the dynamic model of the Ublox M8N (0=portable, 1=sea, 2=automotive). As 'sea' has some disadvantages (max speed is limited to 40 knots, only sea-level..), my advice is to stay with 'portable'. </td>\n</tr>\n"; 
  //timezone
  webpage += "<tr>\n<td>timezone</td><td>\n";
  webpage += "<input size='2' type='number' required name='timezone' min='-12' max='14' value="+String(config.timezone)+" step='1'>\n";
  webpage += "</select>\n</td><td>timezone: The local time difference in hours with UTC (can be negative ! )<a href='https://en.wikipedia.org/wiki/List_of_UTC_offsets' target='_blank'>this Link</a></td>\n</tr>\n";
  //UBXfile
  webpage += "<tr>\n<td>UBXfile</td><td>\n";
  webpage += "<input size='10' type='text' required name='UBXfile' value="+String(config.UBXfile)+">\n";
  webpage += "</select>\n</td><td>UBXfile: Here you can set the desired file name, this is completed with the (unique) MAC address of the ESP32 and a suffix from 000 to 999.</td>\n</tr>\n";
  //Sleep_info
  String Sleep_info="\"" + String(config.Sleep_info) + "\"";
  webpage += "<tr>\n<td>Sleep_info</td><td>\n";
  webpage += "<input size='21' type='text' required name='Sleep_info' value="+Sleep_info+">\n";//size 20 -> 21
  webpage += "</select>\n</td><td>Sleep_info:  Text appears in sleep_screen.</td>\n</tr>\n";

  //ssid
  webpage += "<tr>\n<td>ssid</td><td>\n";
  webpage += "<input size='20' type='text' required name='ssid' value="+String(config.ssid)+">\n";
  webpage += "</select>\n</td><td>ssid: the name of the wlan where the esp-logger should connect to</td>\n</tr>\n";

  //password
  webpage += "<tr>\n<td>password</td><td>\n";
  webpage += "<input size='20' type='text' required name='password' value="+String(config.password)+">\n";
  webpage += "</select>\n</td><td>password: the password of the wlan where the esp-logger should connect to</td>\n</tr>\n";

  //reboot the esp
  webpage += "<tr><td>reboot</td><td>\n<select id='reboot' name='reboot'>\n";
  webpage += "<option value='yes' selected>yes</option>\n";
  webpage += "<option value='no'>no</option>\n";
  webpage += "</select>\n</td><td>reboot: reboot after upload (new config will only load@boot)</td>\n</tr>\n";  
}

 const char html_config_footer[] PROGMEM = R"=====(
     </table>
    <button class="button" type="submit">Submit</button>
</FORM>
  )=====";
