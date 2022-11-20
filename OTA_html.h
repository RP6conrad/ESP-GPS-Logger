//html files readable

//header file readable
const char html_header[] PROGMEM = R"=====(
<html><head><meta charset="UTF-8"><title>ESP-GPS-Logger</title>
 <style>
        .button {
            background-color: #04aa6d; /* Green */
            border: none;
            color: white;
            padding: 10px 24px;
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
  webpage += "</select>\n</td><td>cal_bat is the calibration <br> of the battery voltage measurement (1.7-1.8).</td>\n</tr>\n";

  //cal_speed
  webpage += "<tr><td>cal_speed</td><td>\n<select id='cal_speed' name='cal_speed'>\n";
  if(config.cal_speed == 3.6) webpage += "<option value='3.6' selected>3.6 km/h</option>\n"; else webpage += "<option value='3.6'>3.6 km/h</option>\n";
  if(config.cal_speed == 1,94) webpage += "<option value='1.94' selected>1.94 knots</option>\n"; else webpage += "<option value='1.94'>1.94 knots</option>\n";
  webpage += "</select>\n</td><td>cal_speed is for the conversion from gps unit m/s to km/h (3.6)or knots (1.94).</td>\n</tr>\n";
  
  //sample_rate
  webpage += "<tr>\n<td>sample_rate(Hz)</td><td>\n<select id='sample_rate' name='sample_rate'>";
  if(config.sample_rate == 1) webpage += "<option value='1' selected>1</option>\n"; else  webpage += "<option value='1'>1 Hz</option>\n";
  if(config.sample_rate == 2) webpage += "<option value='2' selected>2</option>\n"; else webpage += "<option value='2'>2 Hz</option>\n";
  if(config.sample_rate == 5) webpage += "<option value='5' selected>5</option>\n"; else webpage += "<option value='5'>5 Hz</option>\n";
  if(config.sample_rate == 10) webpage += "<option value='10' selected>10</option>\n"; else webpage += "<option value='10'>10 Hz</option>\n";
  webpage += "</select>\n</td><td>sample_rate can be 1,2,5 or 10Hz. The higher, the more accurate,<br> but also the larger thefiles become! One UBX NavPVT message is 100byte, <br>so at 1Hz this gives a file of 360kb/hour, at 10Hz 3.6Mb/hour!</td>\n</tr>\n";

}

 const char html_config_footer[] PROGMEM = R"=====(
     </table>
    <button class="button" type="submit">Submit</button>
</FORM>
  )=====";
