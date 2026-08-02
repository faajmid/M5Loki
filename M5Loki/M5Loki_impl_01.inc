void activity() { lastActivity = millis(); }

void centerText(const String& s, int y, uint16_t color) {
  StickCP2.Display.setTextDatum(middle_center);
  StickCP2.Display.setTextColor(color, C_BG);
  StickCP2.Display.setTextSize(1);
  StickCP2.Display.drawString(s, 120, y);
}

String shorten(const String& s, int n) {
  if ((int)s.length() <= n) return s;
  return s.substring(0, n - 3) + "...";
}


// --------------------- Arabic / RTL rendering ---------------------

struct ArForm {
  uint16_t base, iso, fin, ini, med;
};

static const ArForm AR_FORMS[] PROGMEM = {
  {0x0621,0xFE80,0,0,0},
  {0x0622,0xFE81,0xFE82,0,0},
  {0x0623,0xFE83,0xFE84,0,0},
  {0x0624,0xFE85,0xFE86,0,0},
  {0x0625,0xFE87,0xFE88,0,0},
  {0x0626,0xFE89,0xFE8A,0xFE8B,0xFE8C},
  {0x0627,0xFE8D,0xFE8E,0,0},
  {0x0628,0xFE8F,0xFE90,0xFE91,0xFE92},
  {0x0629,0xFE93,0xFE94,0,0},
  {0x062A,0xFE95,0xFE96,0xFE97,0xFE98},
  {0x062B,0xFE99,0xFE9A,0xFE9B,0xFE9C},
  {0x062C,0xFE9D,0xFE9E,0xFE9F,0xFEA0},
  {0x062D,0xFEA1,0xFEA2,0xFEA3,0xFEA4},
  {0x062E,0xFEA5,0xFEA6,0xFEA7,0xFEA8},
  {0x062F,0xFEA9,0xFEAA,0,0},
  {0x0630,0xFEAB,0xFEAC,0,0},
  {0x0631,0xFEAD,0xFEAE,0,0},
  {0x0632,0xFEAF,0xFEB0,0,0},
  {0x0633,0xFEB1,0xFEB2,0xFEB3,0xFEB4},
  {0x0634,0xFEB5,0xFEB6,0xFEB7,0xFEB8},
  {0x0635,0xFEB9,0xFEBA,0xFEBB,0xFEBC},
  {0x0636,0xFEBD,0xFEBE,0xFEBF,0xFEC0},
  {0x0637,0xFEC1,0xFEC2,0xFEC3,0xFEC4},
  {0x0638,0xFEC5,0xFEC6,0xFEC7,0xFEC8},
  {0x0639,0xFEC9,0xFECA,0xFECB,0xFECC},
  {0x063A,0xFECD,0xFECE,0xFECF,0xFED0},
  {0x0641,0xFED1,0xFED2,0xFED3,0xFED4},
  {0x0642,0xFED5,0xFED6,0xFED7,0xFED8},
  {0x0643,0xFED9,0xFEDA,0xFEDB,0xFEDC},
  {0x0644,0xFEDD,0xFEDE,0xFEDF,0xFEE0},
  {0x0645,0xFEE1,0xFEE2,0xFEE3,0xFEE4},
  {0x0646,0xFEE5,0xFEE6,0xFEE7,0xFEE8},
  {0x0647,0xFEE9,0xFEEA,0xFEEB,0xFEEC},
  {0x0648,0xFEED,0xFEEE,0,0},
  {0x0649,0xFEEF,0xFEF0,0,0},
  {0x064A,0xFEF1,0xFEF2,0xFEF3,0xFEF4},
};
constexpr int AR_FORM_COUNT = sizeof(AR_FORMS) / sizeof(AR_FORMS[0]);

bool containsArabic(const String& s) {
  const uint8_t* p = (const uint8_t*)s.c_str();
  while (*p) {
    uint32_t cp = 0;
    if (*p < 0x80) { cp = *p++; }
    else if ((*p & 0xE0) == 0xC0) { cp = (*p++ & 0x1F) << 6; if (*p) cp |= (*p++ & 0x3F); }
    else if ((*p & 0xF0) == 0xE0) { cp = (*p++ & 0x0F) << 12; if (*p) cp |= (*p++ & 0x3F) << 6; if (*p) cp |= (*p++ & 0x3F); }
    else { ++p; continue; }
    if ((cp >= 0x0600 && cp <= 0x06FF) || (cp >= 0xFE70 && cp <= 0xFEFF)) return true;
  }
  return false;
}

int utf8ToCodepoints(const String& s, uint16_t* out, int maxOut) {
  const uint8_t* p = (const uint8_t*)s.c_str();
  int n = 0;
  while (*p && n < maxOut) {
    uint32_t cp = 0;
    if (*p < 0x80) cp = *p++;
    else if ((*p & 0xE0) == 0xC0) {
      cp = (*p++ & 0x1F) << 6;
      if (*p) cp |= (*p++ & 0x3F);
    } else if ((*p & 0xF0) == 0xE0) {
      cp = (*p++ & 0x0F) << 12;
      if (*p) cp |= (*p++ & 0x3F) << 6;
      if (*p) cp |= (*p++ & 0x3F);
    } else {
      ++p;
      continue;
    }

    // Drop Arabic vowel marks/harakat to keep this tiny renderer clean.
    if (cp >= 0x064B && cp <= 0x065F) continue;
    if (cp <= 0xFFFF) out[n++] = (uint16_t)cp;
  }
  return n;
}

bool getArForm(uint16_t base, ArForm& f) {
  for (int i = 0; i < AR_FORM_COUNT; ++i) {
    memcpy_P(&f, &AR_FORMS[i], sizeof(ArForm));
    if (f.base == base) return true;
  }
  return false;
}

bool joinsPrev(uint16_t cp) {
  ArForm f;
  return getArForm(cp, f) && f.fin;
}

bool joinsNext(uint16_t cp) {
  ArForm f;
  return getArForm(cp, f) && f.ini;
}

int shapeArabic(const uint16_t* in, int n, uint16_t* out) {
  for (int i = 0; i < n; ++i) {
    ArForm f;
    if (!getArForm(in[i], f)) {
      out[i] = in[i];
      continue;
    }

    bool prev = i > 0 && joinsNext(in[i - 1]) && joinsPrev(in[i]);
    bool next = i + 1 < n && joinsNext(in[i]) && joinsPrev(in[i + 1]);

    if (prev && next && f.med) out[i] = f.med;
    else if (prev && f.fin) out[i] = f.fin;
    else if (next && f.ini) out[i] = f.ini;
    else out[i] = f.iso;
  }
  return n;
}

bool isLTRChar(uint16_t cp) {
  return (cp >= '0' && cp <= '9') ||
         (cp >= 'A' && cp <= 'Z') ||
         (cp >= 'a' && cp <= 'z') ||
         cp == '.' || cp == '/' || cp == '-' || cp == '_' || cp == '%';
}

// Convert logical Arabic order to visual left-to-right order.
// Western number/English runs stay internally left-to-right.
int makeVisualRTL(const uint16_t* shaped, int n, uint16_t* visual) {
  int outN = 0;
  for (int i = n - 1; i >= 0;) {
    if (isLTRChar(shaped[i])) {
      int end = i;
      int start = i;
      while (start > 0 && isLTRChar(shaped[start - 1])) --start;
      for (int k = start; k <= end; ++k) visual[outN++] = shaped[k];
      i = start - 1;
    } else {
      visual[outN++] = shaped[i--];
    }
  }
  return outN;
}

bool getArabicGlyph(uint16_t cp, ArGlyphMeta& g) {
  for (int i = 0; i < AR_GLYPH_COUNT; ++i) {
    memcpy_P(&g, &AR_GLYPHS[i], sizeof(ArGlyphMeta));
    if (g.cp == cp) return true;
  }
  return false;
}

int asciiAdvance(uint16_t cp) {
  if (cp == ' ') return 4;
  return 6; // M5 default font at text size 1
}

int visualWidth(const uint16_t* v, int n) {
  int w = 0;
  for (int i = 0; i < n; ++i) {
    if (v[i] < 128) w += asciiAdvance(v[i]);
    else {
      ArGlyphMeta g;
      if (getArabicGlyph(v[i], g)) w += g.adv;
      else w += 5;
    }
  }
  return w;
}

void drawArabicGlyph(uint16_t cp, int x, int baseline, uint16_t color) {
  ArGlyphMeta g;
  if (!getArabicGlyph(cp, g)) return;

  int bitCount = g.w * g.h;
  for (int bit = 0; bit < bitCount; ++bit) {
    uint8_t b = pgm_read_byte(&AR_BITMAP[g.off + bit / 8]);
    if (b & (0x80 >> (bit & 7))) {
      int px = bit % g.w;
      int py = bit / g.w;
      StickCP2.Display.drawPixel(x + g.xo + px, baseline + g.yo + py, color);
    }
  }
}

int arabicLineWidth(const String& s) {
  uint16_t logical[192], shaped[192], visual[192];
  int n = utf8ToCodepoints(s, logical, 192);
  shapeArabic(logical, n, shaped);
  int vn = makeVisualRTL(shaped, n, visual);
  return visualWidth(visual, vn);
}

void drawArabicLine(const String& s, int rightX, int baseline, uint16_t color) {
  uint16_t logical[192], shaped[192], visual[192];
  int n = utf8ToCodepoints(s, logical, 192);
  shapeArabic(logical, n, shaped);
  int vn = makeVisualRTL(shaped, n, visual);

  int x = rightX - visualWidth(visual, vn);
  StickCP2.Display.setTextSize(1);
  StickCP2.Display.setTextColor(color, C_BG);
  StickCP2.Display.setTextDatum(top_left);

  int i = 0;
  while (i < vn) {
    if (visual[i] < 128) {
      String run;
      while (i < vn && visual[i] < 128) {
        run += (char)visual[i++];
      }
      StickCP2.Display.drawString(run, x, baseline - 10);
      x += run.length() * 6;
    } else {
      ArGlyphMeta g;
      if (getArabicGlyph(visual[i], g)) {
        drawArabicGlyph(visual[i], x, baseline, color);
        x += g.adv;
      } else {
        x += 5;
      }
      ++i;
    }
  }
}

void drawArabicWrapped(const String& text, int rightX, int y, int width, int maxLines, uint16_t color) {
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
      // Very long unbroken token: draw it as-is.
