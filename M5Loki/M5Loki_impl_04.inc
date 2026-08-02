  else
    centerText("Top: menu", 122, C_DIM);
}

void animateFace() {
  uint32_t now = millis();
  if (screen != FACE) return;

  // Refresh only the top bar, never the full screen.
  if (now - lastBatteryDraw > 15000) {
    lastBatteryDraw = now;
    drawTopBar();
  }

  // Recording waveform updates only its own small region.
  if (mood == LISTENING) {
    if (now - lastAnim > 70) {
      lastAnim = now;
      drawRecordingFace();
    }
    return;
  }

  // Typing bubble is fixed, so no redraw is needed.
  if (mood == THINKING) return;

  // Special birthday screen is static.
  if (showingReaction && specialAction == ACTION_BIRTHDAY) return;

  Mood visibleMood = showingReaction ? reactionMood : NORMAL;

  // Only the neutral face blinks. Other reaction faces stay perfectly static.
  if (visibleMood != NORMAL) return;

  if (!eyesClosed && now >= nextBlinkAt) {
    eyesClosed = true;
    blinkEndsAt = now + 140;
    redrawNeutralEyesOnly();
  } else if (eyesClosed && now >= blinkEndsAt) {
    eyesClosed = false;
    nextBlinkAt = now + random(2200, 4800);
    redrawNeutralEyesOnly();
  }
}

// --------------------- Chat ---------------------

void drawChat() {
  StickCP2.Display.fillScreen(C_BG);
  drawTopBar();

  bool lokiArabic = containsArabic(replyText);

  if (lokiArabic) {
    drawArabicWrappedCentered(replyText, 120, 37, 200, 4, C_MAIN);
  } else {
    drawEnglishWrappedCentered(replyText, 120, 43, 200, 4, C_MAIN);
  }

  centerText("Click: reaction   Hold: talk", 122, C_DIM);
}

// --------------------- Settings menu ---------------------

const char* MENU_ITEMS[] = {"Wi-Fi", "Time", "Appearance", "About", "Back"};
constexpr int MENU_COUNT = 5;

const char* APPEARANCE_ITEMS[] = {"Color", "Theme", "Brightness", "Back"};
constexpr int APPEARANCE_COUNT = 4;

const char* COLOR_NAMES[] = {"Cyan", "Green", "Magenta", "Yellow", "Orange"};

void drawMenu() {
  StickCP2.Display.fillScreen(C_BG);
  drawTopBar();

  StickCP2.Display.setTextSize(2);
  centerText("MENU", 31, C_MAIN);

  for (int i = 0; i < MENU_COUNT; ++i) {
    int y = 50 + i * 17;
    bool sel = (i == menuIndex);

    if (sel) {
      StickCP2.Display.fillRect(28, y - 7, 184, 16, C_MAIN);
      StickCP2.Display.setTextColor(C_BG, C_MAIN);
    } else {
      StickCP2.Display.setTextColor(C_TEXT, C_BG);
    }

    StickCP2.Display.setTextDatum(middle_center);
    StickCP2.Display.drawString(MENU_ITEMS[i], 120, y);
  }

  StickCP2.Display.setTextSize(1);
}

void drawAppearanceMenu() {
  StickCP2.Display.fillScreen(C_BG);
  drawTopBar();

  StickCP2.Display.setTextSize(2);
  centerText("APPEARANCE", 32, C_MAIN);

  for (int i = 0; i < APPEARANCE_COUNT; ++i) {
    int y = 55 + i * 19;
    bool sel = (i == appearanceIndex);

    if (sel) {
      StickCP2.Display.fillRect(24, y - 7, 192, 16, C_MAIN);
      StickCP2.Display.setTextColor(C_BG, C_MAIN);
    } else {
      StickCP2.Display.setTextColor(C_TEXT, C_BG);
    }

    StickCP2.Display.setTextDatum(middle_center);
    StickCP2.Display.drawString(APPEARANCE_ITEMS[i], 120, y);
  }

  StickCP2.Display.setTextSize(1);
}

void drawBrightnessSet() {
  StickCP2.Display.fillScreen(C_BG);
  drawTopBar();

  StickCP2.Display.setTextSize(2);
  centerText("BRIGHTNESS", 35, C_MAIN);

  int pct = map(brightnessLevel, 40, 255, 10, 100);
  centerText(String(pct) + "%", 61, C_MAIN);

  const int x = 35;
  const int y = 79;
  const int w = 170;
  const int h = 14;

  StickCP2.Display.drawRect(x, y, w, h, C_DIM);
  int fillW = map(brightnessLevel, 40, 255, 0, w - 4);
  StickCP2.Display.fillRect(x + 2, y + 2, fillW, h - 4, C_MAIN);

  StickCP2.Display.setTextSize(1);
  centerText("Top +   Bottom -   M5 save", 113, C_DIM);
}

void drawTimeSet() {
  StickCP2.Display.fillScreen(C_BG);
  drawTopBar();

  StickCP2.Display.setTextSize(2);
  centerText("SET TIME", 35, C_MAIN);

  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", setHour, setMinute);

  StickCP2.Display.setTextDatum(middle_center);
  StickCP2.Display.setTextColor(C_MAIN, C_BG);
  StickCP2.Display.setTextSize(3);
  StickCP2.Display.drawString(buf, 120, 70);

  int x = (timeField == 0) ? 80 : 137;
  StickCP2.Display.fillRect(x, 90, 32, 4, C_MAIN);

  StickCP2.Display.setTextSize(1);
  centerText("Top +  Bottom -  M5 next/save", 114, C_DIM);
}

void drawColorSet() {
  StickCP2.Display.fillScreen(C_BG);
  drawTopBar();
  StickCP2.Display.setTextSize(2);
  centerText("COLOR", 35, C_MAIN);
  StickCP2.Display.setTextSize(1);

  for (int i = 0; i < 5; ++i) {
    int x = 36 + i * 42;
    StickCP2.Display.fillCircle(x, 61, 11, LOKI_COLORS[i]);
    if (i == colorIndex)
      StickCP2.Display.drawCircle(x, 61, 15, C_TEXT);
  }
  centerText(COLOR_NAMES[colorIndex], 88, C_MAIN);
  centerText("Top/Bottom choose   M5 save", 112, C_DIM);
}

void drawThemeSet() {
  StickCP2.Display.fillScreen(C_BG);
  drawTopBar();
  StickCP2.Display.setTextSize(2);
  centerText("THEME", 35, C_MAIN);
  StickCP2.Display.setTextSize(1);
  centerText(lightMode ? "LIGHT" : "DARK", 66, C_MAIN);
  centerText("Top/Bottom toggle   M5 save", 108, C_DIM);
}

void drawAbout() {
  StickCP2.Display.fillScreen(C_BG);
  drawTopBar();
  StickCP2.Display.setTextSize(2);
  centerText("M5LOKI", 35, C_MAIN);
  StickCP2.Display.setTextSize(1);
  centerText("Tiny bilingual AI pet", 55, C_TEXT);
  centerText("English + Arabic", 72, C_TEXT);
  centerText("M5: back", 111, C_DIM);
}

void openMenu() {
  if (recording) return;
  menuIndex = 0;
  screen = MENU;
  drawMenu();
}

// --------------------- Wi-Fi UI ---------------------

void drawNetworks() {
  StickCP2.Display.fillScreen(C_BG);
  drawTopBar();

  StickCP2.Display.setTextSize(2);
  centerText("WI-FI", 32, C_MAIN);
  StickCP2.Display.setTextSize(1);

  if (networkCount == 0) {
    centerText("No networks found", 67, C_TEXT);
    centerText("M5: rescan", 104, C_DIM);
    return;
  }

  const int firstY = 40;
  const int rowH = 17;
  const int visibleRows = 5;

  int first = networkCursor - visibleRows / 2;
  if (first < 0) first = 0;
  if (first > networkCount - visibleRows) first = max(0, networkCount - visibleRows);

  for (int row = 0; row < visibleRows; ++row) {
    int i = first + row;
    if (i >= networkCount) break;

    int y = firstY + row * rowH;
    bool selected = (i == networkCursor);

    if (selected) {
      StickCP2.Display.fillRect(6, y - 2, 228, 15, C_MAIN);
      StickCP2.Display.setTextColor(TFT_BLACK, C_MAIN);
    } else {
      StickCP2.Display.setTextColor(C_TEXT, C_BG);
    }

    StickCP2.Display.setTextDatum(middle_left);
    String label = shorten(ssids[i], 28);
    StickCP2.Display.drawString(label, 11, y + 5);

    if (secured[i]) {
      StickCP2.Display.setTextDatum(middle_right);
      StickCP2.Display.drawString("*", 229, y + 5);
    }
  }
}

void scanNetworks() {
  screen = WIFI_LIST;
  activity();
  StickCP2.Display.fillScreen(C_BG);
  drawBattery();
  centerText("Scanning Wi-Fi...", 61);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(false, false);
  delay(100);

  int found = WiFi.scanNetworks(false, true);
  networkCount = min(found, MAX_WIFI);
  networkCursor = 0;

  for (int i = 0; i < networkCount; ++i) {
    ssids[i] = WiFi.SSID(i);
    rssis[i] = WiFi.RSSI(i);
    secured[i] = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
  }

  WiFi.scanDelete();
  drawNetworks();
}

// --------------------- Keyboard ---------------------

int rowLength() { return (keyRow == CONNECT_ROW) ? 1 : strlen(rows[keyRow]); }


void drawKeyboard() {
  StickCP2.Display.fillScreen(C_BG);
  drawTopBar();

  StickCP2.Display.setTextSize(2);
  centerText("PASSWORD", 32, C_MAIN);
  StickCP2.Display.setTextSize(1);

  StickCP2.Display.drawRect(7, 35, 226, 17, C_MAIN);
  String masked;
  for (size_t i = 0; i < wifiPassword.length(); ++i) masked += "*";

