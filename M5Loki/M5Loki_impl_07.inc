      eyesClosed = false;
      nextBlinkAt = millis() + random(2200, 4800);
      drawFace();

    } else if (screen == MENU) {
      if (menuIndex == 0) {
        scanNetworks();
      } else if (menuIndex == 1) {
        int h, m; getClock(h, m);
        setHour = h; setMinute = m; timeField = 0;
        screen = TIME_SET; drawTimeSet();
      } else if (menuIndex == 2) {
        appearanceIndex = 0;
        screen = APPEARANCE_MENU;
        drawAppearanceMenu();
      } else if (menuIndex == 3) {
        screen = ABOUT_SCREEN;
        drawAbout();
      } else {
        screen = FACE;
        drawFace();
      }

    } else if (screen == APPEARANCE_MENU) {
      if (appearanceIndex == 0) {
        screen = COLOR_SET;
        drawColorSet();
      } else if (appearanceIndex == 1) {
        screen = THEME_SET;
        drawThemeSet();
      } else if (appearanceIndex == 2) {
        screen = BRIGHTNESS_SET;
        drawBrightnessSet();
      } else {
        screen = MENU;
        drawMenu();
      }

    } else if (screen == TIME_SET) {
      if (timeField == 0) {
        timeField = 1;
        drawTimeSet();
      } else {
        clockBaseMinutes = setHour * 60 + setMinute;
        clockBaseMillis = millis();
        saveSettings();
        screen = MENU; drawMenu();
      }

    } else if (screen == COLOR_SET) {
      saveSettings();
      screen = APPEARANCE_MENU;
      drawAppearanceMenu();

    } else if (screen == THEME_SET) {
      saveSettings();
      screen = APPEARANCE_MENU;
      drawAppearanceMenu();

    } else if (screen == BRIGHTNESS_SET) {
      saveSettings();
      screen = APPEARANCE_MENU;
      drawAppearanceMenu();

    } else if (screen == ABOUT_SCREEN) {
      screen = MENU; drawMenu();

    } else if (screen == WIFI_LIST) {
      if (!networkCount) scanNetworks();
      else {
        chosenSSID = ssids[networkCursor];
        wifiPassword = "";
        if (secured[networkCursor]) {
          keyRow = keyCol = 0;
          screen = KEYBOARD;
          drawKeyboard();
        } else connectWiFi(chosenSSID, "", true);
      }

    } else if (screen == KEYBOARD) {
      if (keyRow == CONNECT_ROW) connectWiFi(chosenSSID, wifiPassword, true);
      else {
        if (wifiPassword.length() < 63) wifiPassword += rows[keyRow][keyCol];
        drawKeyboard();
      }

    } else if (screen == ERROR_SCREEN) {
      mood = NORMAL;
      screen = FACE;
      drawFace();
    }
  }

  // Top side click = open menu from normal UI, otherwise up/decrease.
  if (topWasClicked() && !pHeld) {
    activity();

    if (screen == FACE || screen == CHAT) {
      openMenu();
    } else if (screen == MENU) {
      menuIndex = (menuIndex - 1 + MENU_COUNT) % MENU_COUNT;
      drawMenu();
    } else if (screen == APPEARANCE_MENU) {
      appearanceIndex = (appearanceIndex - 1 + APPEARANCE_COUNT) % APPEARANCE_COUNT;
      drawAppearanceMenu();
    } else if (screen == TIME_SET) {
      if (timeField == 0) setHour = (setHour + 1) % 24;
      else setMinute = (setMinute + 1) % 60;
      drawTimeSet();
    } else if (screen == COLOR_SET) {
      colorIndex = (colorIndex - 1 + 5) % 5;
      applyTheme();
      drawColorSet();
    } else if (screen == THEME_SET) {
      lightMode = !lightMode;
      applyTheme();
      drawThemeSet();
    } else if (screen == BRIGHTNESS_SET) {
      brightnessLevel += 20;
      if (brightnessLevel > 255) brightnessLevel = 255;
      StickCP2.Display.setBrightness((uint8_t)brightnessLevel);
      drawBrightnessSet();
    } else if (screen == WIFI_LIST && networkCount) {
      networkCursor = (networkCursor - 1 + networkCount) % networkCount;
      drawNetworks();
    } else if (screen == KEYBOARD) {
      keyRow = (keyRow - 1 + 5) % 5;
      if (keyRow == CONNECT_ROW) keyCol = 0;
      else keyCol = min(keyCol, (int)strlen(rows[keyRow]) - 1);
      drawKeyboard();
    }
  }

  // Bottom side click = down/increase/right.
  if (StickCP2.BtnB.wasClicked() && !bHeld) {
    activity();

    if (screen == MENU) {
      menuIndex = (menuIndex + 1) % MENU_COUNT;
      drawMenu();
    } else if (screen == APPEARANCE_MENU) {
      appearanceIndex = (appearanceIndex + 1) % APPEARANCE_COUNT;
      drawAppearanceMenu();
    } else if (screen == TIME_SET) {
      if (timeField == 0) setHour = (setHour + 23) % 24;
      else setMinute = (setMinute + 59) % 60;
      drawTimeSet();
    } else if (screen == COLOR_SET) {
      colorIndex = (colorIndex + 1) % 5;
      applyTheme();
      drawColorSet();
    } else if (screen == THEME_SET) {
      lightMode = !lightMode;
      applyTheme();
      drawThemeSet();
    } else if (screen == BRIGHTNESS_SET) {
      brightnessLevel -= 20;
      if (brightnessLevel < 40) brightnessLevel = 40;
      StickCP2.Display.setBrightness((uint8_t)brightnessLevel);
      drawBrightnessSet();
    } else if (screen == WIFI_LIST && networkCount) {
      networkCursor = (networkCursor + 1) % networkCount;
      drawNetworks();
    } else if (screen == KEYBOARD) {
      if (keyRow != CONNECT_ROW) keyCol = (keyCol + 1) % rowLength();
      drawKeyboard();
    }
  }

  if (!StickCP2.BtnA.isPressed()) aHeld = false;
  if (!StickCP2.BtnB.isPressed()) bHeld = false;
  if (!topIsPressed()) pHeld = false;
}

// --------------------- Setup/loop ---------------------

void setup() {
  Serial.begin(115200);

  auto cfg = M5.config();
  StickCP2.begin(cfg);

  pinMode(TOP_BUTTON_PIN, INPUT);

  StickCP2.Display.setRotation(1);
  StickCP2.Display.setBrightness(80);
  StickCP2.Display.setTextWrap(false);

  if (!StickCP2.Speaker.isEnabled()) StickCP2.Speaker.begin();
  StickCP2.Speaker.setVolume(150);

  randomSeed(esp_random());
  loadSettings();

  audio = (int16_t*)heap_caps_malloc(
    MAX_SAMPLES * sizeof(int16_t),
    MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
  );
  if (!audio) audio = (int16_t*)malloc(MAX_SAMPLES * sizeof(int16_t));

  if (!audio) {
    errorText = "Audio memory allocation failed";
    screen = ERROR_SCREEN;
    drawError();
    while (true) delay(100);
  }

  activity();
  nextBlinkAt = millis() + random(2200, 4800);
  drawFace();
  trySavedWiFi();

  if (WiFi.status() != WL_CONNECTED) {
    scanNetworks();
  } else {
    mood = NORMAL;
    replyText = "Ready!";
    screen = FACE;
    activity();
    drawFace();
  }
}

void loop() {
  StickCP2.update();
  updateTopButton();
  handleButtons();

  if (recording) {
    recordFrame();

    if (!recording && audioCount)
      finishRecording();
  }



  static uint32_t lastClockRefresh = 0;
  if (millis() - lastClockRefresh > 30000) {
    lastClockRefresh = millis();
    if (screen == FACE) drawTopBar();
    else if (screen == CHAT) drawTopBar();
    else if (screen == MENU) drawTopBar();
  }

  updateBirthdaySong();

  animateFace();

  if (screen == ERROR_SCREEN) {
    static uint32_t lastDraw = 0;
    if (millis() - lastDraw > 500) {
      lastDraw = millis();
      drawError();
    }
  }

  delay(5);
}
