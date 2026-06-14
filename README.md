<h1>Arduino Nano Music Player</h1>

<p>A retro-style music player built with an Arduino Nano, DFPlayer Mini, SH1106 OLED display, and 5 physical buttons.</p>

<hr>

<h2>Features</h2>
<ul>
  <li>Play / Pause, Next, Previous controls</li>
  <li>Shuffle and Repeat modes</li>
  <li>Track number display</li>
  <li>Smooth progress bar</li>
  <li>Splash screen and boot-time system check</li>
</ul>

<h2>Hardware Requirements</h2>
<table>
  <tr>
    <th>Component</th>
    <th>Specification</th>
    <th>Connection</th>
  </tr>
  <tr>
    <td>Arduino Board</td>
    <td>Nano (ATmega328P)</td>
    <td>—</td>
  </tr>
  <tr>
    <td>Audio Module</td>
    <td>DFPlayer Mini</td>
    <td>RX=Pin 10, TX=Pin 11</td>
  </tr>
  <tr>
    <td>Display</td>
    <td>SH1106 1.3&quot; OLED (I2C)</td>
    <td>SDA=A4, SCL=A5</td>
  </tr>
  <tr>
    <td>Buttons</td>
    <td>5 tactile buttons (active LOW)</td>
    <td>D2–D6 with INPUT_PULLUP</td>
  </tr>
  <tr>
    <td>Storage</td>
    <td>SD card (FAT32, .mp3 files)</td>
    <td>In DFPlayer Mini slot</td>
  </tr>
</table>

<h2>Button Pinout</h2>
<table>
  <tr>
    <th>Button</th>
    <th>Pin</th>
    <th>Function</th>
  </tr>
  <tr>
    <td>Play/Pause</td>
    <td>D2</td>
    <td>Toggle play/pause</td>
  </tr>
  <tr>
    <td>Next</td>
    <td>D3</td>
    <td>Next track</td>
  </tr>
  <tr>
    <td>Prev</td>
    <td>D4</td>
    <td>Previous track</td>
  </tr>
  <tr>
    <td>Shuffle</td>
    <td>D5</td>
    <td>Toggle shuffle mode</td>
  </tr>
  <tr>
    <td>Repeat</td>
    <td>D6</td>
    <td>Toggle repeat mode</td>
  </tr>
</table>

<h2>Libraries Required</h2>
<ul>
  <li><strong>DFRobotDFPlayerMini</strong> by DFRobot</li>
  <li><strong>U8g2</strong> by olikraus</li>
</ul>

<h2>SD Card Setup</h2>
<ol>
  <li>Format the SD card as FAT32.</li>
  <li>Add your MP3 files, preferably named in order.</li>
  <li>Insert the card into the DFPlayer Mini.</li>
</ol>

<h2>How to Use</h2>
<ul>
  <li>Power on the device.</li>
  <li>Wait for the splash screen and system check.</li>
  <li>Press Play/Pause to start playback.</li>
  <li>Use Next/Prev to change tracks.</li>
  <li>Use Shuffle and Repeat as needed.</li>
</ul>

<h2>Troubleshooting</h2>
<ul>
  <li>If the display seems stuck, the refresh was adjusted to update more smoothly.</li>
  <li>If DFPlayer is not detected, verify RX/TX wiring and SD card formatting.</li>
  <li>If buttons do not respond, confirm they are wired to GND and set as INPUT_PULLUP.</li>
</ul>

<h2>Project by - Rebile Adarsh</h2>

<hr>

<p><em>Built with ❤️ for retro music lovers</em></p>
