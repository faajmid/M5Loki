# M5Loki

**Open-source AI pet firmware for M5StickC Plus2.**

M5Loki turns an M5StickC Plus2 into a small voice-driven AI pet with English/Arabic conversations, expressive reactions, on-device Wi-Fi setup, appearance settings, time/battery status, and playful special actions.

<p align="center">
  <img src="docs/images/m5loki-demo-placeholder.svg" alt="M5Loki" width="700">
</p>

## Features

- Voice recording from the M5StickC Plus2 microphone
- Gemini-powered speech understanding and short pet-style replies
- Automatic English / Arabic language detection
- Arabic shaping + RTL rendering
- Matched 16 px English and Arabic reply fonts
- Reactive faces: neutral, happy, love, surprised, sad, angry, sleep, dead
- Recording waveform + fixed message/typing indicator
- Birthday cake + buzzer melody
- Press M5 during birthday music to stop immediately and return to the Happy face
- On-device Wi-Fi scanner and password keyboard
- Appearance menu with Color, Theme, and Brightness
- Dynamic Dark/Light mode, including the status bar and battery area
- Time + sharp 4-block battery indicator
- Settings stored using ESP32 Preferences/NVS
- No phone companion app required

## Hardware

- **M5StickC Plus2**
- USB-C cable
- 2.4 GHz Wi-Fi
- Your own Gemini API key

## Quick start

1. Download or clone the whole repository.
2. Open `M5Loki/M5Loki.ino` in Arduino IDE.
3. Paste **your own** Gemini API key near the top of `M5Loki.ino`.
4. Install the required libraries.
5. Choose your M5StickC Plus2 / ESP32 board and COM port.
6. Verify and Upload.

> The firmware is organized into internal `.inc` files to keep the project readable. **Normal users only edit `M5Loki.ino`.** Do not open or combine the `.inc` files manually.

## Arduino setup

Install **Arduino IDE 2.x** and these libraries:

- M5StickCPlus2
- M5Unified
- M5GFX
- ArduinoJson

The project also uses standard ESP32/Arduino components such as Wi-Fi, HTTPClient, Preferences, mbedTLS/base64, and ESP32 heap utilities.

> M5Loki contains embedded English and Arabic bitmap fonts, so compilation can take longer than a small Arduino sketch. This is normal.

## Gemini API key — every user must use their own key

M5Loki does **not** include a working API key. Each user must create and paste their own key before uploading the firmware.

### 1. Create your key

Go to Google AI Studio:

https://aistudio.google.com/

Then:

1. Sign in.
2. Open **API Keys**.
3. Click **Create API key**.
4. Select or create a Google Cloud project.
5. Open the new key.
6. Click **Copy key**.
7. Keep the key private.

Official Google guide:

https://ai.google.dev/gemini-api/docs/api-key

<p align="center">
  <img src="docs/images/google-ai-studio-api-keys.svg" alt="Google AI Studio API Keys guide" width="850">
</p>

<p align="center">
  <img src="docs/images/google-ai-studio-api-key-details-censored.svg" alt="Google AI Studio key details guide" width="700">
</p>

> Never publish a real API key. The example/screenshot is intentionally censored.

### 2. Paste your key in M5Loki.ino

Near the top of `M5Loki/M5Loki.ino`, find:

```cpp
// ===================== USER CONFIG ======================
#define GEMINI_API_KEY "PASTE_YOUR_GEMINI_API_KEY_HERE"
// ========================================================
```

Replace only the placeholder text between the quotation marks:

```cpp
#define GEMINI_API_KEY "YOUR_COMPLETE_GEMINI_API_KEY"
```

Keep the quotation marks and do not add spaces inside the key.

If you fork or publish your modified firmware, remove your real key first and restore the placeholder.

## Gemini model

The firmware uses the lightweight tested default:

```cpp
const char* GEMINI_MODEL = "gemini-flash-lite-latest";
```

Flash-Lite is a good fit for M5Loki because the goal is fast, short pet-style voice interaction rather than long reasoning responses.

Model names, quotas, and availability can change, so check Google's current documentation before changing the model:

https://ai.google.dev/gemini-api/docs/models

## Controls

| Action | Control |
|---|---|
| Talk | Hold **M5** |
| Send recording | Release **M5** |
| Show conversation reaction | Click **M5** |
| Stop birthday song | Press **M5** while music is playing |
| Open menu | Top side button |
| Navigate | Top / bottom side buttons |
| Select | M5 |

## Interaction flow

```text
Hold M5
  ↓
Recording face
  ↓
Release
  ↓
Message/typing indicator
  ↓
Loki reply
  ↓
Click M5
  ↓
Conversation reaction
```

Birthday example:

```text
"It's my birthday"
"Play birthday music"
"اليوم عيد ميلادي"
        ↓
Cake + birthday melody
        ↓
Press M5 anytime
        ↓
Music stops + Happy face
```

## Menu

```text
Wi-Fi
Time
Appearance
About
Back
```

Inside **Appearance**:

```text
Color
Theme
Brightness
Back
```

Available accent colors:

- Cyan
- Green
- Magenta
- Yellow
- Orange

Dark/Light mode changes the whole interface, including the top status bar and battery background.

## Wi-Fi

Wi-Fi setup is completely on-device:

1. Open **Menu → Wi-Fi**.
2. Select your network.
3. Enter the password using the on-device keyboard.
4. Select **CONNECT**.

Saved Wi-Fi credentials are stored in ESP32 Preferences/NVS.

## Language support

M5Loki automatically follows the spoken language:

```text
English speech → English reply
Arabic speech  → Arabic reply
```

Arabic rendering includes contextual shaping and RTL display. Western digits `0-9` are used in both languages.

## About screen

The firmware About page shows:

```text
M5LOKI
AI Pet Firmware
for M5StickC Plus2

by Fahad AlAjmi
GitHub: @faajmid

v1.0
```

## HTTP 429 / RESOURCE_EXHAUSTED

If Gemini returns:

```text
429 RESOURCE_EXHAUSTED
```

check your Google AI Studio **Usage**, **Rate Limit**, and billing/free-tier quota. This is generally an API quota/rate-limit issue rather than an M5StickC Plus2 microphone failure.

## Repository structure

```text
M5Loki/
├── M5Loki/
│   ├── M5Loki.ino              # Open/edit this file
│   ├── config.h                 # Internal compatibility header; do not edit
│   ├── M5Loki_prototypes.h     # Internal declarations
│   ├── M5Loki_globals_*.inc    # Internal state + embedded fonts
│   └── M5Loki_impl_*.inc       # Internal firmware implementation
├── docs/
│   └── images/                  # Setup illustrations / project media
├── .gitignore
├── CONTRIBUTING.md
├── LICENSE
├── README.md
└── SECURITY.md
```

### Why is the firmware split into `.inc` files?

The full source is large because it contains embedded 16 px English and Arabic bitmap fonts. Splitting the implementation keeps GitHub easier to browse and maintain.

For a normal user, it still behaves like one Arduino project:

```text
Open M5Loki.ino
→ paste your API key
→ compile
→ upload
```

Arduino resolves the included files automatically because they live in the same sketch folder.

## Security note

An API key compiled into microcontroller firmware should be treated as a device credential, not as a perfectly secret server-side credential. This setup is appropriate for personal/open-source experimentation. For a commercial product, use a backend/proxy architecture so reusable cloud credentials are not distributed in firmware.

## Contributing

Bug fixes, documentation improvements, UI refinements, additional semantic reactions, and embedded-system optimizations are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

M5Loki is open source under the **MIT License**. See [LICENSE](LICENSE).

## Author

**Fahad AlAjmi**

- GitHub: [@faajmid](https://github.com/faajmid)
- Instagram: [@faajmid](https://instagram.com/faajmid)
- TikTok: [@faajmid](https://www.tiktok.com/@faajmid)
- Email: [faajmid@gmail.com](mailto:faajmid@gmail.com)
