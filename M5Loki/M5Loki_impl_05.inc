  StickCP2.Display.setTextDatum(middle_left);
  StickCP2.Display.setTextColor(C_TEXT, C_BG);
  StickCP2.Display.drawString(shorten(masked + "_", 34), 11, 43);

  for (int r = 0; r < 4; ++r) {
    int len = strlen(rows[r]);
    int cellW = 17;
    int startX = (240 - len * cellW) / 2;
    int y = 56 + r * 14;

    for (int c = 0; c < len; ++c) {
      int x = startX + c * cellW;
      bool selected = (r == keyRow && c == keyCol);

      if (selected) {
        StickCP2.Display.fillRect(x, y, 15, 12, C_MAIN);
        StickCP2.Display.setTextColor(TFT_BLACK, C_MAIN);
      } else {
        StickCP2.Display.drawRect(x, y, 15, 12, C_DIM);
        StickCP2.Display.setTextColor(C_TEXT, C_BG);
      }

      StickCP2.Display.setTextDatum(middle_center);
      StickCP2.Display.drawString(String(rows[r][c]), x + 7, y + 6);
    }
  }

  bool connectSelected = (keyRow == CONNECT_ROW);
  const int bx = 74;
  const int by = 114;
  const int bw = 92;
  const int bh = 16;

  if (connectSelected) {
    StickCP2.Display.fillRect(bx, by, bw, bh, C_MAIN);
    StickCP2.Display.setTextColor(TFT_BLACK, C_MAIN);
  } else {
    StickCP2.Display.drawRect(bx, by, bw, bh, C_MAIN);
    StickCP2.Display.setTextColor(C_MAIN, C_BG);
  }

  StickCP2.Display.setTextDatum(middle_center);
  StickCP2.Display.drawString("CONNECT", 120, by + bh / 2);
}

// --------------------- Wi-Fi connection ---------------------

bool connectWiFi(const String& ssid, const String& password, bool saveIt) {
  screen = CONNECTING;
  activity();
  StickCP2.Display.fillScreen(C_BG);
  centerText("Connecting to", 47);
  centerText(shorten(ssid, 28), 67, C_TEXT);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(false, false);
  delay(100);
  WiFi.begin(ssid.c_str(), password.c_str());

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    StickCP2.update();
    delay(150);
  }

  if (WiFi.status() == WL_CONNECTED) {
    chosenSSID = ssid;
    wifiPassword = password;

    if (saveIt) {
      prefs.begin("m5loki", false);
      prefs.putString("ssid", ssid);
      prefs.putString("pass", password);
      prefs.end();
    }

    mood = HAPPY;
    replyText = "Wi-Fi connected!";
    screen = FACE;
    activity();
    drawFace();
    return true;
  }

  errorText = "Could not connect to Wi-Fi";
  screen = ERROR_SCREEN;
  mood = SAD;
  return false;
}

void trySavedWiFi() {
  prefs.begin("m5loki", true);
  String ssid = prefs.getString("ssid", "");
  String pass = prefs.getString("pass", "");
  prefs.end();

  if (ssid.length()) connectWiFi(ssid, pass, false);
}

// --------------------- WAV/base64 ---------------------

void le16(uint8_t* p, uint16_t v) {
  p[0] = v; p[1] = v >> 8;
}

void le32(uint8_t* p, uint32_t v) {
  p[0] = v; p[1] = v >> 8; p[2] = v >> 16; p[3] = v >> 24;
}

uint8_t* makeWav(size_t& wavSize) {
  size_t pcmBytes = audioCount * 2;
  wavSize = 44 + pcmBytes;

  uint8_t* wav = (uint8_t*)heap_caps_malloc(wavSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!wav) wav = (uint8_t*)malloc(wavSize);
  if (!wav) return nullptr;

  memcpy(wav, "RIFF", 4);
  le32(wav + 4, wavSize - 8);
  memcpy(wav + 8, "WAVEfmt ", 8);
  le32(wav + 16, 16);
  le16(wav + 20, 1);
  le16(wav + 22, 1);
  le32(wav + 24, SAMPLE_RATE);
  le32(wav + 28, SAMPLE_RATE * 2);
  le16(wav + 32, 2);
  le16(wav + 34, 16);
  memcpy(wav + 36, "data", 4);
  le32(wav + 40, pcmBytes);
  memcpy(wav + 44, audio, pcmBytes);
  return wav;
}

String toBase64(const uint8_t* data, size_t len) {
  size_t needed = 0;
  mbedtls_base64_encode(nullptr, 0, &needed, data, len);

  char* out = (char*)heap_caps_malloc(needed + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!out) out = (char*)malloc(needed + 1);
  if (!out) return "";

  size_t written = 0;
  int result = mbedtls_base64_encode(
    (unsigned char*)out, needed, &written, data, len
  );

  if (result != 0) {
    free(out);
    return "";
  }

  out[written] = 0;
  String encoded(out);
  free(out);
  return encoded;
}

// --------------------- Gemini ---------------------

bool askGemini() {
  if (WiFi.status() != WL_CONNECTED) {
    errorText = "Connect Wi-Fi first";
    screen = ERROR_SCREEN;
    return false;
  }

  if (String(GEMINI_API_KEY).startsWith("PASTE_")) {
    errorText = "Paste your Gemini API key";
    screen = ERROR_SCREEN;
    return false;
  }

  mood = THINKING;
  screen = FACE;
  drawFace();

  size_t wavSize = 0;
  uint8_t* wav = makeWav(wavSize);

  if (!wav) {
    errorText = "Not enough WAV memory";
    screen = ERROR_SCREEN;
    return false;
  }

  String b64 = toBase64(wav, wavSize);
  free(wav);

  if (!b64.length()) {
    errorText = "Base64 conversion failed";
    screen = ERROR_SCREEN;
    return false;
  }

  String instruction =
    "You are Loki, a tiny cute robotic pet. Listen to the user's audio and understand the MEANING. "
    "Detect English or Arabic and reply naturally in the SAME language using no more than 18 words. "
    "Arabic replies must use Arabic script, write Loki as لوكي, avoid English words, and Western digits 0-9 are fine. "
    "Choose one visible reaction: neutral, happy, love, surprised, sad, angry, sleep, dead. "
    "Choose one action: none or birthday. "
    "Use action=birthday whenever the user says it is their birthday, talks about celebrating their birthday, "
    "asks for birthday music/song, or expresses the same birthday meaning in Arabic such as عيد ميلادي. "
    "For birthday action, reply may be very short because the device will show a cake and play music instead of displaying text. "
    "Choose reactions from meaning and tone, not keyword matching. "
    "Affection -> love; good/funny -> happy; shocking -> surprised; hurt/disappointment -> sad; "
    "hostility -> angry; tired/sleep request -> sleep; dead only for playful dead/broken/shutdown statements. "
    "Return only JSON.";

  DynamicJsonDocument doc(4096);
  JsonArray contents = doc["contents"].to<JsonArray>();
  JsonArray parts = contents.add<JsonObject>()["parts"].to<JsonArray>();
  parts.add<JsonObject>()["text"] = instruction;

  JsonObject inlineData = parts.add<JsonObject>()["inline_data"].to<JsonObject>();
  inlineData["mime_type"] = "audio/wav";
  inlineData["data"] = "__AUDIO__";

  JsonObject config = doc["generationConfig"].to<JsonObject>();
  config["temperature"] = 0.8;
  config["maxOutputTokens"] = 160;
  config["responseMimeType"] = "application/json";

  JsonObject schema = config["responseSchema"].to<JsonObject>();
  schema["type"] = "OBJECT";
  JsonObject props = schema["properties"].to<JsonObject>();
  props["transcript"]["type"] = "STRING";
  props["reply"]["type"] = "STRING";
  props["emotion"]["type"] = "STRING";
  props["action"]["type"] = "STRING";

  JsonArray values = props["emotion"]["enum"].to<JsonArray>();
  const char* allowed[] = {
    "neutral","happy","love","surprised","sad","angry","sleep","dead"
  };
  for (auto v : allowed) values.add(v);

  JsonArray actionValues = props["action"]["enum"].to<JsonArray>();
  actionValues.add("none");
  actionValues.add("birthday");

  JsonArray required = schema["required"].to<JsonArray>();
  required.add("transcript");
  required.add("reply");
  required.add("emotion");
  required.add("action");

  String body;
  body.reserve(b64.length() + 5000);
  serializeJson(doc, body);
  body.replace("__AUDIO__", b64);
  b64 = "";

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  String url = "https://generativelanguage.googleapis.com/v1beta/models/" +
               String(GEMINI_MODEL) + ":generateContent?key=" + String(GEMINI_API_KEY);

  if (!http.begin(client, url)) {
    errorText = "Could not start HTTPS";
    screen = ERROR_SCREEN;
    return false;
  }

  http.addHeader("Content-Type", "application/json");
  http.setTimeout(30000);
