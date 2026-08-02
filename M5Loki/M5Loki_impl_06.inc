
  int status = http.POST((uint8_t*)body.c_str(), body.length());
  body = "";

  String response = http.getString();
  http.end();

  if (status < 200 || status >= 300) {
    Serial.println("Gemini error response:");
    Serial.println(response);

    DynamicJsonDocument errorDoc(2048);
    String apiMessage;
    if (!deserializeJson(errorDoc, response)) {
      apiMessage = String(errorDoc["error"]["message"] | "");
    }

    if (apiMessage.length())
      errorText = "HTTP " + String(status) + ": " + shorten(apiMessage, 70);
    else
      errorText = "Gemini HTTP " + String(status);

    screen = ERROR_SCREEN;
    return false;
  }

  DynamicJsonDocument outer(16384);
  if (deserializeJson(outer, response)) {
    Serial.println(response);
    errorText = "Invalid Gemini response";
    screen = ERROR_SCREEN;
    return false;
  }

  const char* text = outer["candidates"][0]["content"]["parts"][0]["text"] | "";
  DynamicJsonDocument result(2048);

  if (!strlen(text) || deserializeJson(result, text)) {
    Serial.println(text);
    errorText = "Invalid Loki JSON";
    screen = ERROR_SCREEN;
    return false;
  }

  transcriptText = String(result["transcript"] | "");
  replyText = String(result["reply"] | "I'm here!");

  reactionMood = parseMood(String(result["emotion"] | "happy"));
  String actionText = String(result["action"] | "none");
  actionText.toLowerCase();
  specialAction = (actionText == "birthday") ? ACTION_BIRTHDAY : ACTION_NONE;
  birthdaySongPlayed = false;
  mood = NORMAL;
  showingReaction = false;
  screen = CHAT;
  reactionStarted = 0;
  activity();

  if (specialAction == ACTION_BIRTHDAY) {
    // Birthday is a direct special action: no text reply.
    replyText = "";
    showingReaction = true;
    reactionMood = HAPPY;
    screen = FACE;
    drawFace();
    startBirthdaySong();
    return true;
  }

  playLokiResponseSound();
  drawChat();
  return true;
}


// --------------------- Loki cute response sound ---------------------

void playLokiResponseSound() {
  // Microphone and speaker share audio resources on this device,
  // so make sure the mic is stopped before enabling the buzzer/speaker.
  if (StickCP2.Mic.isEnabled()) StickCP2.Mic.end();

  if (!StickCP2.Speaker.isEnabled()) StickCP2.Speaker.begin();
  StickCP2.Speaker.setVolume(150);

  // Short cheerful "pet bot" chirp.
  StickCP2.Speaker.tone(1250, 65);
  delay(75);
  StickCP2.Speaker.tone(1650, 70);
  delay(80);
  StickCP2.Speaker.tone(2050, 95);
  delay(105);
  StickCP2.Speaker.stop();
}


// --------------------- Birthday melody ---------------------

static const uint16_t BDAY_NOTES[] = {
  988,988,1109,988,1319,1245,
  988,988,1109,988,1480,1319,
  988,988,1976,1661,1319,1245,1109,
  1760,1760,1661,1319,1480,1319
};

static const uint16_t BDAY_DUR[] = {
  180,180,360,360,360,650,
  180,180,360,360,360,650,
  180,180,360,360,360,360,650,
  180,180,360,360,360,760
};

constexpr int BDAY_COUNT = sizeof(BDAY_NOTES) / sizeof(BDAY_NOTES[0]);

void stopBirthdaySong() {
  if (birthdayPlaying) {
    StickCP2.Speaker.stop();
    birthdayPlaying = false;
  }
}

void startBirthdaySong() {
  birthdaySongPlayed = true;
  birthdayPlaying = true;
  birthdayNoteIndex = 0;
  birthdayNextAt = 0;

  if (StickCP2.Mic.isEnabled()) StickCP2.Mic.end();
  if (!StickCP2.Speaker.isEnabled()) StickCP2.Speaker.begin();

  // Tiny speaker is clearer at high frequencies and full volume.
  StickCP2.Speaker.setVolume(255);
}

void updateBirthdaySong() {
  if (!birthdayPlaying) return;

  uint32_t now = millis();
  if (now < birthdayNextAt) return;

  if (birthdayNoteIndex >= BDAY_COUNT) {
    StickCP2.Speaker.stop();
    birthdayPlaying = false;
    return;
  }

  StickCP2.Speaker.stop();
  delay(8);

  uint16_t note = BDAY_NOTES[birthdayNoteIndex];
  uint16_t dur = BDAY_DUR[birthdayNoteIndex];

  StickCP2.Speaker.tone(note, dur - 25);
  birthdayNextAt = now + dur;
  birthdayNoteIndex++;
}

// --------------------- Recording ---------------------

void beginRecording() {
  stopBirthdaySong();
  specialAction = ACTION_NONE;
  birthdaySongPlayed = false;
  if (recording) return;

  if (WiFi.status() != WL_CONNECTED) {
    errorText = "Hold lower side to set Wi-Fi";
    screen = ERROR_SCREEN;
    return;
  }

  audioCount = 0;
  recording = true;
  recordStarted = millis();
  activity();

  StickCP2.Speaker.end();
  if (!StickCP2.Mic.isEnabled()) StickCP2.Mic.begin();

  mood = LISTENING;
  screen = FACE;
  drawFace();
}

void recordFrame() {
  if (!recording) return;

  if (audioCount + FRAME_SAMPLES <= MAX_SAMPLES) {
    if (StickCP2.Mic.record(audio + audioCount, FRAME_SAMPLES, SAMPLE_RATE, false))
      audioCount += FRAME_SAMPLES;
  }

  if (millis() - recordStarted >= MAX_RECORD_MS ||
      audioCount + FRAME_SAMPLES > MAX_SAMPLES) {
    recording = false;
  }
}

void finishRecording() {
  bool hadAudio = audioCount > 0;
  recording = false;

  while (StickCP2.Mic.isRecording()) delay(1);
  StickCP2.Mic.end();

  if (!hadAudio || audioCount < SAMPLE_RATE / 3) {
    transcriptText = "";
    replyText = "Hold M5 a little longer.";
    mood = NORMAL;
    screen = CHAT;
    activity();
    drawChat();
    audioCount = 0;
    return;
  }

  askGemini();
  audioCount = 0;
}

// --------------------- Error ---------------------

void drawError() {
  StickCP2.Display.fillScreen(C_BG);
  drawBattery();
  centerText("LOKI", 12);
  centerText("Something went wrong", 44, TFT_RED);
  wrapped(errorText, 15, 61, 210, 4, C_TEXT);
  centerText("M5: back   Hold lower: Wi-Fi", 123, C_DIM);
}

// --------------------- GPIO35 top button ---------------------

void updateTopButton() {
  topClickedEvent = false;
  topHoldEvent = false;

  // The button is active LOW and has an external pull-up on the device.
  topNow = (digitalRead(TOP_BUTTON_PIN) == LOW);
  uint32_t now = millis();

  if (topNow && !topPrevious) {
    topPressedAt = now;
    topHoldSent = false;
  }

  if (topNow && !topHoldSent && now - topPressedAt >= 700) {
    topHoldSent = true;
    topHoldEvent = true;
  }

  if (!topNow && topPrevious) {
    if (!topHoldSent && now - topPressedAt >= 30) {
      topClickedEvent = true;
    }
  }

  topPrevious = topNow;
}

bool topWasHold() {
  return topHoldEvent;
}

bool topWasClicked() {
  return topClickedEvent;
}

bool topIsPressed() {
  return topNow;
}

// --------------------- Buttons ---------------------

void handleButtons() {
  // Birthday action gets first priority: pressing M5 stops the melody instantly
  // and leaves Loki on a happy face.
  if (birthdayPlaying && (StickCP2.BtnA.wasPressed() || StickCP2.BtnA.wasClicked())) {
    stopBirthdaySong();

    // After a birthday action Loki should always end on the happy reaction.
    specialAction = ACTION_NONE;
    showingReaction = true;
    reactionMood = HAPPY;
    mood = NORMAL;
    screen = FACE;
    eyesClosed = false;
    nextBlinkAt = millis() + random(2200, 4800);
    drawFace();
    return;
  }

  // Front M5 = select / click reaction / hold talk
  // Lower side = down/right
  // Top side GPIO35 = menu/up

  if (StickCP2.BtnA.wasPressed()) aHeld = false;

  if (StickCP2.BtnA.wasHold()) {
    aHeld = true;
    if ((screen == FACE || screen == CHAT) && !recording) {
      showingReaction = false;
      beginRecording();
    }
  }

  if (StickCP2.BtnA.wasReleased() && recording) {
    finishRecording();
    return;
  }

  if (topWasHold()) {
    pHeld = true;
    if (screen == KEYBOARD && wifiPassword.length()) {
      wifiPassword.remove(wifiPassword.length() - 1);
      drawKeyboard();
    }
  }

  if (StickCP2.BtnB.wasHold()) bHeld = true;

  // M5 short click
  if (StickCP2.BtnA.wasClicked() && !aHeld) {
    activity();

    if (screen == CHAT) {
      mood = NORMAL;
      showingReaction = true;
      screen = FACE;
