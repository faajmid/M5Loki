/*
  M5Loki - Open-Source AI Pet Firmware for M5StickC Plus2
  Main Arduino sketch entry point.

  Copy config.example.h to config.h and add YOUR OWN Gemini API key.
  Never commit config.h.

  v8.0 fixes:
  - M5 immediately stops the birthday melody and returns to Happy face.
  - Status bar now follows Light/Dark theme dynamically.
*/

#include "M5Loki_globals_01.inc"
#include "M5Loki_globals_02.inc"
#include "M5Loki_globals_03.inc"
#include "M5Loki_globals_04.inc"
#include "M5Loki_prototypes.h"
#include "M5Loki_impl_01.inc"

// Keep the original implementations available internally, then provide
// corrected public versions below. This avoids duplicating the large
// embedded font/source files in the repository.
#define applyTheme m5lokiLegacyApplyTheme
#define drawTopBar m5lokiLegacyDrawTopBar
#include "M5Loki_impl_02.inc"
#undef drawTopBar
#undef applyTheme

// Correct, deterministic theme handling.
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

// Draw the battery directly in the status bar so its background always
// matches the selected Light/Dark theme.
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

// Status bar background follows the whole UI theme.
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
#include "M5Loki_impl_04.inc"
#include "M5Loki_impl_05.inc"
#include "M5Loki_impl_06.inc"

// Keep the original loop body, but wrap it with a highest-priority
// birthday-stop check. Using isPressed() makes this reliable even if a
// one-frame click/press event is missed while audio is playing.
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

    // Consume this press completely so it cannot immediately trigger
    // hold-to-record after stopping the birthday song.
    while (StickCP2.BtnA.isPressed()) {
      StickCP2.update();
      delay(5);
    }
  }

  m5lokiOriginalLoop();
}
