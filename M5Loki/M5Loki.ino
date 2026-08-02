/*
  M5Loki - Open-Source AI Pet Firmware for M5StickC Plus2
  ---------------------------------------------------------
  Author: Fahad AlAjmi
  GitHub: https://github.com/faajmid
  Email: faajmid@gmail.com
  Instagram / TikTok: @faajmid
  License: MIT

  QUICK START
  1) Create your own Gemini API key in Google AI Studio.
  2) Paste it below between the quotation marks.
  3) Open this file in Arduino IDE and upload it to M5StickC Plus2.

  Never publish or share your real API key.
*/

// ===================== USER CONFIG ======================
// Every user must paste THEIR OWN Gemini API key here.
#define GEMINI_API_KEY "PASTE_YOUR_GEMINI_API_KEY_HERE"
// ========================================================

/*
  The firmware is split into internal .inc files only to keep the repository
  readable. Users only need to edit/open THIS M5Loki.ino file.
*/

#include "M5Loki_globals_01.inc"
#include "M5Loki_globals_02.inc"
#include "M5Loki_globals_03.inc"
#include "M5Loki_globals_04.inc"
#include "M5Loki_prototypes.h"

// Forward declaration for the theme-aware battery renderer used by later
// implementation sections.
void drawBatteryThemeAware();

#include "M5Loki_impl_01.inc"

// Keep the old theme/status implementations internally, while later firmware
// calls resolve to the corrected v1.0 functions defined at the end of this file.
#define applyTheme applyTheme_legacy
#define drawTopBar drawTopBar_legacy
#include "M5Loki_impl_02.inc"
#undef applyTheme
#undef drawTopBar

// M5Loki_impl_02.inc and M5Loki_impl_03.inc split the battery function across
// the file boundary, so only preprocessor directives are placed between them.
// All later battery refresh calls use the theme-aware renderer.
#define drawBattery drawBatteryThemeAware
#include "M5Loki_impl_03.inc"

// Keep the old About implementation internally. Later menu calls resolve to
// the final About screen defined at the end of this file.
#define drawAbout drawAbout_legacy
#include "M5Loki_impl_04.inc"
#undef drawAbout

#include "M5Loki_impl_05.inc"

// Keep the original button handler internally. loop() in the next section
// resolves to the final wrapper defined at the end of this file.
#define handleButtons handleButtons_legacy
#include "M5Loki_impl_06.inc"
#undef handleButtons

#include "M5Loki_impl_07.inc"

#undef drawBattery

// ============================================================================
// FINAL v1.0 OVERRIDES
// ============================================================================

void applyTheme() {
  C_MAIN = LOKI_COLORS[colorIndex];

  // Explicit values keep repeated Light/Dark switching deterministic,
  // including changing accent color while Light mode is active.
  if (lightMode) {
    C_BG = TFT_WHITE;
    C_TEXT = TFT_BLACK;
    C_DIM = 0x7BEF;
  } else {
    C_BG = TFT_BLACK;
    C_TEXT = TFT_WHITE;
    C_DIM = 0x8410;
  }
}

void drawBatteryThemeAware() {
  int pct = batteryPercent();

  int blocks = 0;
  if (pct > 5)  blocks = 1;
  if (pct > 25) blocks = 2;
  if (pct > 50) blocks = 3;
  if (pct > 75) blocks = 4;

  const int x = 204;
  const int y = 6;
  const int w = 28;
  const int h = 12;

  StickCP2.Display.fillRect(x - 1, y - 1, 35, 15, C_BG);
  StickCP2.Display.drawRect(x, y, w, h, C_MAIN);
  StickCP2.Display.fillRect(x + w, y + 3, 3, 6, C_MAIN);

  for (int i = 0; i < 4; ++i) {
    int bx = x + 3 + i * 6;
    int by = y + 3;

    if (i < blocks)
      StickCP2.Display.fillRect(bx, by, 4, 6, C_MAIN);
    else
      StickCP2.Display.drawRect(bx, by, 4, 6, C_MAIN);
  }
}

void drawTopBar() {
  // Status bar background follows Dark/Light mode dynamically.
  StickCP2.Display.fillRect(0, 0, 240, 24, C_BG);

  String t = clockText();

  StickCP2.Display.setTextSize(2);
  StickCP2.Display.setTextDatum(middle_left);
  StickCP2.Display.setTextColor(C_MAIN, C_BG);
  StickCP2.Display.drawString(t, 4, 12);

  StickCP2.Display.setTextDatum(middle_center);
  StickCP2.Display.drawString("LOKI", 120, 12);

  StickCP2.Display.setTextSize(1);
  drawBatteryThemeAware();
}

void drawAbout() {
  StickCP2.Display.fillScreen(C_BG);
  drawTopBar();

  centerText("M5LOKI", 29, C_MAIN);
  centerText("AI Pet Firmware", 45, C_TEXT);
  centerText("for M5StickC Plus2", 59, C_TEXT);

  centerText("by Fahad AlAjmi", 76, C_MAIN);
  centerText("GitHub: @faajmid", 90, C_TEXT);

  centerText("v1.0", 104, C_DIM);
  centerText("M5: back", 122, C_DIM);
}

void handleButtons() {
  // Birthday gets highest priority. Any physical M5 press stops the melody
  // immediately and leaves Loki on the Happy reaction.
  if (birthdayPlaying && StickCP2.BtnA.isPressed()) {
    stopBirthdaySong();

    specialAction = ACTION_NONE;
    showingReaction = true;
    reactionMood = HAPPY;
    mood = NORMAL;
    screen = FACE;
    eyesClosed = false;
    nextBlinkAt = millis() + random(2200, 4800);
    drawFace();

    // Consume this press so it cannot immediately become hold-to-record.
    aHeld = true;
    return;
  }

  handleButtons_legacy();
}
