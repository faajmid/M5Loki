/*
  M5Loki - Open-Source AI Pet Firmware for M5StickC Plus2
  ---------------------------------------------------------

  Author: Fahad AlAjmi
  Email: faajmid@gmail.com
  GitHub: https://github.com/faajmid
  Instagram: @faajmid
  TikTok: @faajmid

  License: MIT

  IMPORTANT:
  Paste YOUR OWN Gemini API key below before uploading.
  Never publish or share a real API key.
*/

// ===================== USER CONFIG ======================
// Get your key from: https://aistudio.google.com/
#define GEMINI_API_KEY "PASTE_YOUR_GEMINI_API_KEY_HERE"

// Tested/default lightweight model for M5Loki.
#define M5LOKI_GEMINI_MODEL "gemini-flash-lite-latest"
// =======================================================

/*
  v8.1 fixes:
  - No separate config.h is required from the user.
  - M5 immediately stops the birthday melody and returns to Happy face.
  - Status bar follows Light/Dark theme dynamically.
  - About page includes project/author details.
*/

#include "M5Loki_globals_01.inc"
#include "M5Loki_globals_02.inc"
#include "M5Loki_globals_03.inc"
#include "M5Loki_globals_04.inc"
#include "M5Loki_prototypes.h"
#include "M5Loki_impl_01.inc"

// Keep legacy theme/top-bar implementations internally, then use the
// corrected versions below.
#define applyTheme m5lokiLegacyApplyTheme
#define drawTopBar m5lokiLegacyDrawTopBar
#include "M5Loki_impl_02.inc"
#undef drawTopBar
#undef applyTheme

void applyTheme() {
  C_MAIN = LOKI_COLORS[colorIndex];

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

void drawTopBarBattery() {
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
  StickCP2.Display.fillRect(0, 0, 240, 24, C_BG);

  String t = clockText();

  StickCP2.Display.setTextSize(2);
  StickCP2.Display.setTextDatum(middle_left);
  StickCP2.Display.setTextColor(C_MAIN, C_BG);
  StickCP2.Display.drawString(t, 4, 12);

  StickCP2.Display.setTextDatum(middle_center);
  StickCP2.Display.drawString("LOKI", 120, 12);

  StickCP2.Display.setTextSize(1);
  drawTopBarBattery();
}

#include "M5Loki_impl_03.inc"

// Keep the old About page internally and replace it with the final one.
#define drawAbout m5lokiLegacyDrawAbout
#include "M5Loki_impl_04.inc"
#undef drawAbout

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

#include "M5Loki_impl_05.inc"
#include "M5Loki_impl_06.inc"

// Keep the original loop body, but wrap it with a highest-priority
// birthday-stop check.
#define loop m5lokiOriginalLoop
#include "M5Loki_impl_07.inc"
#undef loop

void loop() {
  StickCP2.update();

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

    // Consume this press so it cannot immediately start a recording.
    while (StickCP2.BtnA.isPressed()) {
      StickCP2.update();
      delay(5);
    }
  }

  m5lokiOriginalLoop();
}
