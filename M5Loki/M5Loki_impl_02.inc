      current = remaining;
      consumed = remaining.length();
    }

    drawArabicLine(current, rightX, y + line * 19 + 14, color);
    ++line;

    if (consumed >= (int)remaining.length()) break;
    remaining = remaining.substring(consumed);
    remaining.trim();
  }
}



void drawArabicWrappedCentered(const String& text, int centerX, int y, int width, int maxLines, uint16_t color) {
  String remaining = text;
  remaining.trim();
  int line = 0;

  while (remaining.length() && line < maxLines) {
    String current;
    int consumed = 0;
    int pos = 0;

    while (pos < (int)remaining.length()) {
      int nextSpace = remaining.indexOf(' ', pos);
      if (nextSpace < 0) nextSpace = remaining.length();

      String word = remaining.substring(pos, nextSpace);
      String candidate = current.length() ? current + " " + word : word;

      if (current.length() && arabicLineWidth(candidate) > width) break;

      current = candidate;
      consumed = nextSpace;
      pos = nextSpace + 1;
      if (nextSpace == (int)remaining.length()) break;
    }

    if (!current.length()) {
      current = remaining;
      consumed = remaining.length();
    }

    int lineW = arabicLineWidth(current);
    int rightX = centerX + lineW / 2;
    drawArabicLine(current, rightX, y + line * 20 + 15, color);
    ++line;

    if (consumed >= (int)remaining.length()) break;
    remaining = remaining.substring(consumed);
    remaining.trim();
  }
}


bool getEnglishGlyph(uint8_t cp, EnGlyphMeta& g) {
  if (cp < 32 || cp > 126) return false;
  int idx = cp - 32;
  memcpy_P(&g, &EN_GLYPHS[idx], sizeof(EnGlyphMeta));
  return true;
}

int englishTextWidth16(const String& s) {
  int w = 0;
  for (size_t i = 0; i < s.length(); ++i) {
    uint8_t cp = (uint8_t)s[i];
    EnGlyphMeta g;
    if (getEnglishGlyph(cp, g)) w += g.adv;
    else w += 6;
  }
  return w;
}

void drawEnglishGlyph16(uint8_t cp, int x, int baseline, uint16_t color) {
  EnGlyphMeta g;
  if (!getEnglishGlyph(cp, g)) return;

  int bitCount = g.w * g.h;
  for (int bit = 0; bit < bitCount; ++bit) {
    uint8_t b = pgm_read_byte(&EN_BITMAP[g.off + bit / 8]);
    if (b & (0x80 >> (bit & 7))) {
      int px = bit % g.w;
      int py = bit / g.w;
      StickCP2.Display.drawPixel(x + g.xo + px, baseline + g.yo + py, color);
    }
  }
}

void drawEnglishLine16(const String& s, int centerX, int baseline, uint16_t color) {
  int w = englishTextWidth16(s);
  int x = centerX - w / 2;

  for (size_t i = 0; i < s.length(); ++i) {
    uint8_t cp = (uint8_t)s[i];
    EnGlyphMeta g;
    if (getEnglishGlyph(cp, g)) {
      drawEnglishGlyph16(cp, x, baseline, color);
      x += g.adv;
    } else {
      x += 6;
    }
  }
}

void drawEnglishWrappedCentered(const String& text, int centerX, int y, int width, int maxLines, uint16_t color) {
  String remaining = text;
  remaining.trim();
  int line = 0;

  while (remaining.length() && line < maxLines) {
    String current;
    int consumed = 0;
    int pos = 0;

    while (pos < (int)remaining.length()) {
      int nextSpace = remaining.indexOf(' ', pos);
      if (nextSpace < 0) nextSpace = remaining.length();

      String word = remaining.substring(pos, nextSpace);
      String candidate = current.length() ? current + " " + word : word;

      if (current.length() && englishTextWidth16(candidate) > width) break;

      current = candidate;
      consumed = nextSpace;
      pos = nextSpace + 1;
      if (nextSpace == (int)remaining.length()) break;
    }

    if (!current.length()) {
      current = remaining;
      consumed = remaining.length();
    }

    drawEnglishLine16(current, centerX, y + line * 20 + 14, color);
    ++line;

    if (consumed >= (int)remaining.length()) break;
    remaining = remaining.substring(consumed);
    remaining.trim();
  }
}

void wrapped(const String& text, int x, int y, int width, int maxLines, uint16_t color) {
  StickCP2.Display.setTextDatum(top_left);
  StickCP2.Display.setTextColor(color, C_BG);
  StickCP2.Display.setTextSize(1);

  String left = text;
  int chars = max(8, width / 6);

  for (int line = 0; line < maxLines && left.length(); ++line) {
    int cut = min(chars, (int)left.length());

    if (cut < (int)left.length()) {
      int space = left.lastIndexOf(' ', cut);
      if (space > 0) cut = space;
    }

    String part = left.substring(0, cut);
    part.trim();
    StickCP2.Display.drawString(part, x, y + line * 11);

    left = left.substring(cut);
    left.trim();
  }
}

Mood parseMood(String s) {
  s.toLowerCase();
  s.trim();

  if (s == "love") return LOVE;
  if (s == "surprised") return SURPRISED;
  if (s == "sad") return SAD;
  if (s == "angry") return ANGRY;
  if (s == "dead") return DEAD;
  if (s == "sleep" || s == "sleepy") return SLEEPY;

  // Map old/extra labels into the simpler face set.
  if (s == "excited") return HAPPY;
  if (s == "curious") return SURPRISED;
  if (s == "worried") return SAD;
  if (s == "thinking" || s == "typing") return THINKING;
  if (s == "normal" || s == "neutral") return NORMAL;

  return HAPPY;
}

String moodName() {
  switch (mood) {
    case LOVE: return "love";
    case EXCITED: return "excited";
    case CURIOUS: return "curious";
    case SURPRISED: return "surprised";
    case THINKING: return "thinking";
    case SAD: return "sad";
    case WORRIED: return "worried";
    case ANGRY: return "angry";
    case DEAD: return "dead";
    case SLEEPY: return "sleepy";
    case LISTENING: return "listening";
    case NORMAL: return "normal";
    default: return "happy";
  }
}



// --------------------- Theme / clock ---------------------

void applyTheme() {
  C_MAIN = LOKI_COLORS[colorIndex];
  if (lightMode) {
    C_BG = C_TEXT;
    C_TEXT = TFT_BLACK;
    C_DIM = 0x7BEF;
  } else {
    C_BG = TFT_BLACK;
    C_TEXT = TFT_WHITE;
    C_DIM = 0x8410;
  }
}

void saveSettings() {
  prefs.begin("loki-ui", false);
  prefs.putInt("color", colorIndex);
  prefs.putBool("light", lightMode);
  prefs.putInt("hour", setHour);
  prefs.putInt("minute", setMinute);
  prefs.putInt("brightness", brightnessLevel);
  prefs.end();
}

void loadSettings() {
  prefs.begin("loki-ui", true);
  colorIndex = constrain(prefs.getInt("color", 0), 0, 4);
  lightMode = prefs.getBool("light", false);
  setHour = constrain(prefs.getInt("hour", 12), 0, 23);
  setMinute = constrain(prefs.getInt("minute", 0), 0, 59);
  brightnessLevel = constrain(prefs.getInt("brightness", 180), 40, 255);
  prefs.end();
  clockBaseMinutes = setHour * 60 + setMinute;
  clockBaseMillis = millis();
  applyTheme();
  StickCP2.Display.setBrightness(brightnessLevel);
}

void getClock(int &hour, int &minute) {
  uint32_t elapsedMin = (millis() - clockBaseMillis) / 60000UL;
  int total = (clockBaseMinutes + elapsedMin) % (24 * 60);
  hour = total / 60;
  minute = total % 60;
}

String clockText() {
  int h, m;
  getClock(h, m);
  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", h, m);
  return String(buf);
}

void drawTopBar() {
  StickCP2.Display.fillRect(0, 0, 240, 24, TFT_BLACK);

  String t = clockText();

  StickCP2.Display.setTextSize(2);
  StickCP2.Display.setTextDatum(middle_left);
  StickCP2.Display.setTextColor(C_MAIN, TFT_BLACK);
  StickCP2.Display.drawString(t, 4, 12);

  StickCP2.Display.setTextDatum(middle_center);
  StickCP2.Display.drawString("LOKI", 120, 12);

  StickCP2.Display.setTextSize(1);
  drawBattery();
}

// --------------------- Battery ---------------------

int batteryPercent() {
  // M5Unified/M5StickCPlus2 exposes the battery voltage through Power.
  int mv = StickCP2.Power.getBatteryVoltage();
  if (mv <= 0) return 100;

  // Practical LiPo approximation: 3.30 V = empty, 4.20 V = full.
  int pct = map(mv, 3300, 4200, 0, 100);
  return constrain(pct, 0, 100);
}

void drawBattery() {
  int pct = batteryPercent();

