/*
 * ============================================================
 *  Arduino Nano Music Player
 *
 *  Hardware:
 *    DFPlayer Mini  → SoftwareSerial RX=10, TX=11
 *    SH1106 1.3" OLED I2C → A4 (SDA), A5 (SCL)
 *    Buttons (active LOW, INPUT_PULLUP)
 *      Play/Pause → D2 | Next → D3 | Prev → D4
 *      Shuffle    → D5 | Repeat → D6
 *
 *  Libraries (Library Manager):
 *    DFRobotDFPlayerMini  (by DFRobot)
 *    U8g2                 (by olikraus)
 * ============================================================
 */

#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <U8g2lib.h>
#include <Wire.h>

// ── Pins ─────────────────────────────────────────────────────
#define BTN_PLAY_PAUSE  2
#define BTN_NEXT        3
#define BTN_PREV        4
#define BTN_SHUFFLE     5
#define BTN_REPEAT      6
#define DF_RX           10
#define DF_TX           11

// ── Objects ──────────────────────────────────────────────────
SoftwareSerial dfSerial(DF_RX, DF_TX);
DFRobotDFPlayerMini dfPlayer;
U8G2_SH1106_128X64_NONAME_1_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

// ── State ────────────────────────────────────────────────────
bool     dfOK         = false;
bool     isPlaying    = false;
bool     shuffleOn    = false;
bool     repeatOn     = false;
uint8_t  currentTrack = 1;
uint8_t  totalTracks  = 1;
uint32_t trackStartMs = 0;

// ── Button debounce ──────────────────────────────────────────
const uint8_t PINS[]        = {BTN_PLAY_PAUSE, BTN_NEXT, BTN_PREV, BTN_SHUFFLE, BTN_REPEAT};
const uint8_t NUM_BTNS      = 5;
bool          lastState[5]  = {HIGH, HIGH, HIGH, HIGH, HIGH};
uint32_t      lastChange[5] = {0, 0, 0, 0, 0};

bool buttonPressed(uint8_t i) {
  bool reading = digitalRead(PINS[i]);
  if (reading != lastState[i]) {
    lastChange[i] = millis();
    lastState[i]  = reading;
  }
  if ((millis() - lastChange[i]) > 60 && reading == LOW) {
    while (digitalRead(PINS[i]) == LOW);
    return true;
  }
  return false;
}

// ── Track control ────────────────────────────────────────────
void playTrack(uint8_t t) {
  currentTrack = t;
  dfPlayer.play(t);
  isPlaying    = true;
  trackStartMs = millis();
}

uint8_t randomTrack() {
  uint8_t t;
  do { t = random(1, totalTracks + 1); } while (t == currentTrack && totalTracks > 1);
  return t;
}

// ── Draw one eye — square, bounds-checked ────────────────────
// size: 0 = normal square, 1 = small square
void drawEye(int cx, int cy, int8_t pupilX, bool blink, int size) {
  const int eyeSize_normal = 40;
  const int eyeSize_small  = 30;
  const int radius_normal = 6;
  const int radius_small  = 5;

  int eyeSize = (size == 1) ? eyeSize_small : eyeSize_normal;
  int radius  = (size == 1) ? radius_small  : radius_normal;
  int eyeW = eyeSize;
  int eyeH = eyeSize;

  if (blink) {
    int x = cx - eyeW / 2;
    int y = cy - 1;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + eyeW > 128) x = 128 - eyeW;
    if (y + 3 > 64) y = 64 - 3;
    oled.drawRBox(x, y, eyeW, 3, 1);
    return;
  }

  int x = cx - eyeW / 2 + (pupilX * 3);
  int y = cy - eyeH / 2;

  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (x + eyeW > 128) x = 128 - eyeW;
  if (y + eyeH > 64) y = 64 - eyeH;

  oled.drawRBox(x, y, eyeW, eyeH, radius);
}

// Helper with default size (normal square)
void drawEyeNormal(int cx, int cy, int8_t pupilX, bool blink) {
  drawEye(cx, cy, pupilX, blink, 0);
}

// ── Splash screen ─────────────────────────────────────────────
void showSplash() {
  struct Step { int8_t px; bool blink; uint16_t ms; };
  const Step steps[] = {
    { 0, false, 600 },
    {-1, false, 600 },
    {-1, false, 500 },
    { 1, false, 600 },
    { 1, false, 500 },
    { 0, false, 500 },
    { 0, true,  200 },
    { 0, false, 200 },
    { 0, true,  200 },
    { 0, false, 300 },
  };
  const uint8_t numSteps = sizeof(steps) / sizeof(steps[0]);

  for (uint8_t s = 0; s < numSteps; s++) {
    oled.firstPage();
    do {
      drawEyeNormal(32, 32, steps[s].px, steps[s].blink);
      drawEyeNormal(96, 32, steps[s].px, steps[s].blink);
    } while (oled.nextPage());
    delay(steps[s].ms);
  }

  oled.firstPage();
  do {
    oled.setFont(u8g2_font_profont15_tf);
    oled.drawStr(4, 20, "Project by");
    oled.setFont(u8g2_font_profont17_tf);
    oled.drawStr(4, 36, "Rebile Adarsh");
  } while (oled.nextPage());
  delay(2500);
}

// ── Boot checklist ────────────────────────────────────────────
void showChecklist(bool dfOK, bool sdOK) {
  bool btnsOK = false;
  for (uint8_t i = 0; i < NUM_BTNS; i++) {
    if (digitalRead(PINS[i]) == HIGH) { btnsOK = true; break; }
  }

  oled.firstPage();
  do {
    oled.setFont(u8g2_font_profont10_tf);
    oled.drawStr(0, 9,  "-- SYSTEM CHECK --");
    oled.drawHLine(0, 11, 128);
    oled.drawStr(0, 22, "[OK] OLED Display");
    oled.drawStr(0, 33, dfOK   ? "[OK] DFPlayer Mini" : "[!!] DFPlayer Mini");
    oled.drawStr(0, 44, sdOK   ? "[OK] SD Card"       : "[!!] SD Card");
    oled.drawStr(0, 55, btnsOK ? "[OK] Buttons"      : "[!!] Buttons");
  } while (oled.nextPage());

  delay((!dfOK || !sdOK || !btnsOK) ? 5000 : 3000);
}

// ── Main display ─────────────────────────────────────────────
void drawDisplay() {
  char buf[16];
  oled.firstPage();
  do {
    oled.setFont(u8g2_font_profont10_tf);
    oled.drawStr(0, 9, "MUSIC PLAYER");
    oled.drawHLine(0, 11, 128);

    oled.setFont(u8g2_font_profont17_tf);
    snprintf(buf, sizeof(buf), "Track %02d/%02d", currentTrack, totalTracks);
    oled.drawStr(0, 32, buf);

    oled.setFont(u8g2_font_profont10_tf);
    oled.drawStr(0, 48, isPlaying ? ">> PLAYING" : "|| PAUSED ");
    oled.drawStr(74, 48, shuffleOn ? "SHF" : "---");
    oled.drawStr(100, 48, repeatOn ? "RPT" : "---");

    uint8_t filled = 0;
    if (isPlaying) {
      uint16_t elapsed = (uint16_t)((millis() - trackStartMs) / 1000UL);
      if (elapsed > 180) elapsed = 180;
      filled = (uint8_t)((uint32_t)elapsed * 124UL / 180UL);
    }
    oled.drawFrame(2, 55, 124, 8);
    if (filled > 0) oled.drawBox(2, 55, filled, 8);
  } while (oled.nextPage());
}

// ── Setup ─────────────────────────────────────────────────────
void setup() {
  for (uint8_t i = 0; i < NUM_BTNS; i++) pinMode(PINS[i], INPUT_PULLUP);

  oled.begin();
  oled.setFont(u8g2_font_profont10_tf);

  showSplash();

  dfSerial.begin(9600);
  delay(1000);
  dfPlayer.begin(dfSerial, false);
  dfPlayer.setTimeOut(500);

  bool sdOK = false;
  uint8_t n = dfPlayer.readFileCounts();
  if (n > 0 && n < 200) {
    dfOK = sdOK = true;
    totalTracks = n;
    dfPlayer.volume(20);
    dfPlayer.EQ(DFPLAYER_EQ_NORMAL);
    dfPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  } else if (n == 0) {
    dfOK = true;
  }

  showChecklist(dfOK, sdOK);

  randomSeed(analogRead(A0));
  isPlaying = false;

  drawDisplay();
}

// ── Loop ──────────────────────────────────────────────────────
uint32_t lastDraw = 0;

void loop() {
  if (buttonPressed(0)) {
    if (dfOK) {
      if (isPlaying) { dfPlayer.pause(); isPlaying = false; }
      else { playTrack(currentTrack); }
    }
  }
  if (buttonPressed(1) && dfOK) {
    uint8_t next = shuffleOn ? randomTrack() : (currentTrack < totalTracks ? currentTrack + 1 : 1);
    playTrack(next);
  }
  if (buttonPressed(2) && dfOK) {
    uint8_t prev = shuffleOn ? randomTrack() : (currentTrack > 1 ? currentTrack - 1 : totalTracks);
    playTrack(prev);
  }
  if (buttonPressed(3)) shuffleOn = !shuffleOn;
  if (buttonPressed(4)) {
    repeatOn = !repeatOn;
    if (dfOK) {
      if (repeatOn) dfPlayer.loop(currentTrack);
      else dfPlayer.disableLoop();
    }
  }

  if (dfOK && isPlaying && dfPlayer.available()) {
    if (dfPlayer.readType() == DFPlayerPlayFinished) {
      if (repeatOn) playTrack(currentTrack);
      else if (shuffleOn) playTrack(randomTrack());
      else playTrack(currentTrack < totalTracks ? currentTrack + 1 : 1);
    }
  }

  if (millis() - lastDraw >= 500) {
    lastDraw = millis();
    drawDisplay();
  }
}