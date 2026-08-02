  int blocks = 0;
  if (pct > 5)  blocks = 1;
  if (pct > 25) blocks = 2;
  if (pct > 50) blocks = 3;
  if (pct > 75) blocks = 4;

  const int x = 204;
  const int y = 6;
  const int w = 28;
  const int h = 12;

  StickCP2.Display.fillRect(x - 1, y - 1, 35, 15, TFT_BLACK);

  StickCP2.Display.drawRect(x, y, w, h, C_MAIN);
  StickCP2.Display.fillRect(x + w, y + 3, 3, 6, C_MAIN);

  for (int i = 0; i < 4; ++i) {
    int bx = x + 3 + i * 6;
    int by = y + 3;

    if (i < blocks) {
      StickCP2.Display.fillRect(bx, by, 4, 6, C_MAIN);
    } else {
      StickCP2.Display.drawRect(bx, by, 4, 6, C_MAIN);
    }
  }
}

// --------------------- Loki face set ---------------------
// Face geometry follows the approved reference:
// solid cyan eyes/mouths, no robot/head frame.

void eyeDot(int x, int y) {
  StickCP2.Display.fillCircle(x, y, 10, C_MAIN);
}

void solidBar(int x, int y, int w, int h) {
  StickCP2.Display.fillRoundRect(x, y, w, h, max(1, h / 2), C_MAIN);
}

void solidOpenMouth(int cx, int topY, int w, int h) {
  // Solid rounded "open smile" from the reference.
  StickCP2.Display.fillRoundRect(cx - w / 2, topY, w, h, h / 2, C_MAIN);
}

void solidSmile(int cx, int cy, int w, int h) {
  // Filled crescent. The cutout is background, leaving a thick solid smile.
  StickCP2.Display.fillEllipse(cx, cy, w / 2, h / 2, C_MAIN);
  StickCP2.Display.fillEllipse(cx, cy - 5, w / 2 - 4, h / 2 - 3, C_BG);
}

void solidFrown(int cx, int cy, int w, int h) {
  StickCP2.Display.fillEllipse(cx, cy, w / 2, h / 2, C_MAIN);
  StickCP2.Display.fillEllipse(cx, cy + 6, w / 2 - 4, h / 2 - 3, C_BG);
}

void solidHappyEye(int cx, int cy) {
  // Thick solid curved closed eye like the reference.
  StickCP2.Display.fillEllipse(cx, cy, 14, 8, C_MAIN);
  StickCP2.Display.fillEllipse(cx, cy + 5, 11, 6, C_BG);
}

void solidSadEye(int cx, int cy) {
  StickCP2.Display.fillEllipse(cx, cy, 14, 8, C_MAIN);
  StickCP2.Display.fillEllipse(cx, cy - 5, 11, 6, C_BG);
}

void drawHeart(int x, int y) {
  StickCP2.Display.fillCircle(x - 5, y - 4, 6, C_MAIN);
  StickCP2.Display.fillCircle(x + 5, y - 4, 6, C_MAIN);
  StickCP2.Display.fillTriangle(x - 11, y - 1, x + 11, y - 1, x, y + 13, C_MAIN);
}

void redrawNeutralEyesOnly() {
  // Clear only the eye areas instead of clearing the whole screen.
  // This prevents visible LCD flicker during blinking.
  StickCP2.Display.fillRect(70, 43, 40, 30, C_BG);
  StickCP2.Display.fillRect(130, 43, 40, 30, C_BG);

  if (eyesClosed) {
    solidBar(78, 56, 24, 5);
    solidBar(138, 56, 24, 5);
  } else {
    eyeDot(90, 56);
    eyeDot(150, 56);
  }
}

void drawNeutralFace() {
  if (eyesClosed) {
    solidBar(78, 56, 24, 5);
    solidBar(138, 56, 24, 5);
  } else {
    eyeDot(90, 56);
    eyeDot(150, 56);
  }
  solidOpenMouth(120, 78, 28, 16);
}

void drawHappyFace() {
  solidHappyEye(90, 57);
  solidHappyEye(150, 57);
  solidSmile(120, 82, 34, 18);
}

void drawLoveFace() {
  drawHeart(90, 55);
  drawHeart(150, 55);
  solidOpenMouth(120, 78, 28, 16);
}

void drawSurprisedFace() {
  eyeDot(90, 55);
  eyeDot(150, 55);
  StickCP2.Display.fillCircle(120, 83, 10, C_MAIN);
}

void drawSadFace() {
  solidSadEye(90, 57);
  solidSadEye(150, 57);
  solidFrown(120, 88, 31, 16);
}

void drawAngryFace() {
  // Filled slanted eyes exactly in the visual style of the reference.
  StickCP2.Display.fillTriangle(75, 48, 104, 54, 83, 68, C_MAIN);
  StickCP2.Display.fillTriangle(165, 48, 136, 54, 157, 68, C_MAIN);
  solidFrown(120, 88, 31, 16);
}

void drawDeadFace() {
  // Thick X eyes.
  for (int cx : {90, 150}) {
    StickCP2.Display.fillTriangle(cx - 12, 47, cx - 7, 44, cx + 12, 66, C_MAIN);
    StickCP2.Display.fillTriangle(cx + 7, 44, cx + 12, 47, cx - 12, 66, C_MAIN);
  }
  solidFrown(120, 88, 31, 16);
}

void drawSleepFace() {
  // z Z Z centered above a small solid mouth, no face frame.
  StickCP2.Display.setTextDatum(middle_center);
  StickCP2.Display.setTextColor(C_MAIN, C_BG);

  StickCP2.Display.setTextSize(1);
  StickCP2.Display.drawString("z", 91, 55);
  StickCP2.Display.setTextSize(2);
  StickCP2.Display.drawString("Z", 120, 49);
  StickCP2.Display.setTextSize(3);
  StickCP2.Display.drawString("Z", 154, 42);
  StickCP2.Display.setTextSize(1);

  solidBar(110, 88, 20, 6);
}

void drawRecordingFace() {
  // Compact waveform centered where Loki's eyes normally sit.
  int phase = (millis() / 70) % 12;
  const int left = 64;
  const int bars = 13;
  const int spacing = 9;

  StickCP2.Display.fillRect(45, 34, 150, 58, C_BG);

  for (int i = 0; i < bars; ++i) {
    int x = left + i * spacing;
    int d = abs(((i + phase) % 12) - 6);
    int h = 7 + (6 - min(d, 6)) * 4;
    if (i == 6) h += 6;
    h = min(h, 46);
    StickCP2.Display.fillRoundRect(x, 61 - h / 2, 5, h, 2, C_MAIN);
  }

  solidBar(111, 91, 18, 6);
}

void drawTypingFace() {
  // Fixed message box with three solid dots. No animation.
  const int x = 72;
  const int y = 45;
  const int w = 96;
  const int h = 45;

  StickCP2.Display.fillRoundRect(x, y, w, h, 10, C_MAIN);
  StickCP2.Display.fillTriangle(x + 66, y + h - 2,
                               x + 81, y + h - 2,
                               x + 76, y + h + 10, C_MAIN);

  // Inner black area gives a simple bubble while dots remain solid accent.
  StickCP2.Display.fillRoundRect(x + 4, y + 4, w - 8, h - 8, 8, C_BG);

  StickCP2.Display.fillCircle(101, 67, 5, C_MAIN);
  StickCP2.Display.fillCircle(120, 67, 5, C_MAIN);
  StickCP2.Display.fillCircle(139, 67, 5, C_MAIN);
}

void drawBirthdayCake() {
  // Layered birthday cake. The lower tier is intentionally drawn LAST,
  // so it visually sits in front of the upper tier.

  const int cx = 120;

  // Candles behind the cake.
  for (int x : {100, 120, 140}) {
    StickCP2.Display.fillRoundRect(x - 3, 37, 6, 20, 2, C_MAIN);
    StickCP2.Display.fillCircle(x, 32, 4, C_MAIN);
    StickCP2.Display.fillTriangle(x, 26, x - 4, 33, x + 4, 33, C_MAIN);
  }

  // Upper tier first = visually behind.
  StickCP2.Display.fillRoundRect(cx - 34, 54, 68, 26, 7, C_MAIN);
  StickCP2.Display.fillRoundRect(cx - 34, 54, 68, 7, 6, C_TEXT);

  // Small frosting drops.
  for (int x : {94, 108, 122, 136, 148}) {
    StickCP2.Display.fillCircle(x, 62, 3, C_TEXT);
  }

  // Lower tier LAST = foreground/front layer.
  StickCP2.Display.fillRoundRect(cx - 50, 72, 100, 31, 8, C_MAIN);

  // Front frosting cap.
  StickCP2.Display.fillRoundRect(cx - 50, 72, 100, 9, 7, C_TEXT);

  // A little overlap lip makes the lower layer clearly appear in front.
  StickCP2.Display.fillRoundRect(cx - 42, 68, 84, 9, 5, C_MAIN);

  // Front decorations.
  for (int x : {84, 102, 120, 138, 156}) {
    StickCP2.Display.fillCircle(x, 90, 3, C_BG);
  }

  // Bottom shadow/base line in Loki accent.
  StickCP2.Display.fillRoundRect(cx - 45, 98, 90, 5, 2, C_MAIN);
}

void drawReactionFace(Mood r) {
  switch (r) {
    case LOVE:       drawLoveFace(); break;
    case SURPRISED:  drawSurprisedFace(); break;
    case SAD:
    case WORRIED:    drawSadFace(); break;
    case ANGRY:      drawAngryFace(); break;
    case DEAD:       drawDeadFace(); break;
    case SLEEPY:     drawSleepFace(); break;
    case HAPPY:      drawHappyFace(); break;
    default:         drawNeutralFace(); break;
  }
}

void drawFace() {
  StickCP2.Display.fillScreen(C_BG);
  drawTopBar();

  if (mood == LISTENING) {
    drawRecordingFace();
    centerText("Recording...", 119, C_DIM);
    return;
  }

  if (mood == THINKING) {
    drawTypingFace();
    centerText("Typing...", 112, C_DIM);
    return;
  }

  if (showingReaction && specialAction == ACTION_BIRTHDAY) {
    drawBirthdayCake();
    centerText("Happy Birthday!", 116, C_MAIN);
    return;
  }

  drawReactionFace(showingReaction ? reactionMood : NORMAL);

  if (WiFi.status() == WL_CONNECTED)
    centerText("Hold M5 to talk", 122, C_DIM);
