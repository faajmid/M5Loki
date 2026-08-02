# M5Loki

**Open-source AI pet firmware for M5StickC Plus2.**

M5Loki turns an M5StickC Plus2 into a small voice-driven AI pet with bilingual English/Arabic conversations, expressive reactions, on-device Wi-Fi setup, appearance settings, battery/time status, and playful special actions.

<p align="center">
  <img src="docs/images/m5loki-demo-placeholder.png" alt="M5Loki demo placeholder" width="700">
</p>

> Replace the placeholder above with your own photo or GIF before publishing the repository.

## Highlights

- Voice recording directly from the M5StickC Plus2 microphone
- Gemini-powered understanding and short replies
- Automatic English / Arabic language detection
- Custom Arabic shaping and RTL rendering
- Matched embedded English and Arabic reply fonts
- Reactive Loki faces: neutral, happy, love, surprised, sad, angry, sleep, dead
- Recording waveform and fixed typing/message indicator
- Birthday special action with cake + buzzer melody
- On-device Wi-Fi scanner and password keyboard
- Appearance menu with color, theme, and brightness
- Time + 4-block battery indicator
- Settings saved in ESP32 Preferences / NVS
- No phone companion app required

## Hardware

- **M5StickC Plus2**
- USB-C cable
- 2.4 GHz Wi-Fi connection
- Gemini API key

## Interaction

| Action | Control |
|---|---|
| Talk to Loki | Hold **M5** |
| Send recording | Release **M5** |
| Show reaction | Click **M5** after a normal reply |
| Stop birthday melody | Press **M5** |
| Open menu | Top side button |
| Move through menus | Top / bottom side buttons |
| Select | M5 |

Normal conversation flow:

```text
Hold M5
  ↓
Recording waveform
  ↓
Release
  ↓
Typing indicator
  ↓
Loki reply
  ↓
Click M5
  ↓
Reaction face
```

Birthday flow:

```text
"It's my birthday"
"Play birthday music"
"اليوم عيد ميلادي"
        ↓
Birthday cake + melody
        ↓
Press M5 anytime
        ↓
Happy Loki face
```

## Repository structure

```text
M5Loki/
├── M5Loki/
│   ├── M5Loki.ino
│   └── config.example.h
├── docs/
│   └── images/
│       ├── google-ai-studio-api-keys.png
│       ├── google-ai-studio-api-key-details-censored.png
│       └── m5loki-demo-placeholder.png
├── .gitignore
├── CONTRIBUTING.md
├── LICENSE
├── README.md
└── SECURITY.md
```

## 1. Arduino setup

Install the **Arduino IDE 2.x**.

Add ESP32 board support and select the correct ESP32/M5StickC Plus2 board configuration for your installation.

Install these libraries from Arduino Library Manager:

- **M5StickCPlus2**
- **M5Unified**
- **M5GFX**
- **ArduinoJson**

The firmware also uses standard ESP32/Arduino components such as Wi-Fi, HTTP client, Preferences, and mbedTLS/base64 support.

## 2. Create a Gemini API key

M5Loki uses the Gemini API through **Google AI Studio**.

1. Go to **Google AI Studio**: https://aistudio.google.com/
2. Sign in.
3. In the left sidebar, open **API Keys**.
4. Click **Create API key** at the top-right if you do not already have one.
5. Choose/create the Google Cloud project you want to use.
6. Open the created key.
7. Click **Copy key**.
8. Keep that value private.

Google's official API-key guide:

- https://ai.google.dev/gemini-api/docs/api-key

### API Keys page

<p align="center">
  <img src="docs/images/google-ai-studio-api-keys.png" alt="Google AI Studio API Keys page" width="850">
</p>

### Key details

<p align="center">
  <img src="docs/images/google-ai-studio-api-key-details-censored.png" alt="Google AI Studio API key details with key censored" width="700">
</p>

> The API key in the screenshot is covered on purpose. **Never upload or share a real API key.**

### Put the key in M5Loki

Create:

```text
M5Loki/config.h
```

and write:

```cpp
#pragma once

#define GEMINI_API_KEY "PASTE_YOUR_GEMINI_API_KEY_HERE"
```

The line should have this structure:

```cpp
#define GEMINI_API_KEY "your-complete-key-goes-between-these-quotes"
```

Do not add spaces inside the key, do not remove the quotation marks, and do not publish `config.h`.

## 3. Configure your API key safely

The repository intentionally does **not** include `config.h`.

Copy:

```text
config.example.h
```

and rename the copy to:

```text
config.h
```

Then replace:

```cpp
#define GEMINI_API_KEY "PASTE_YOUR_GEMINI_API_KEY_HERE"
```

with your own key.

`config.h` is ignored by `.gitignore`, which helps prevent accidental API-key commits.

### Never publish your API key

If you accidentally push a real key to GitHub:

1. Revoke/delete the exposed key in Google AI Studio.
2. Create a new key.
3. Update your local `config.h`.
4. Do not rely only on deleting the key from the latest Git commit; secrets can remain in repository history.

## 4. Open and upload M5Loki

Open:

```text
M5Loki/M5Loki.ino
```

in Arduino IDE.

Connect the M5StickC Plus2 via USB-C, choose the correct board and port, then **Verify** and **Upload**.

The sketch contains embedded Arabic and English bitmap fonts, so compilation can take longer than a small Arduino sketch. This is normal.

If upload itself is slow, a higher ESP32 upload speed such as **921600** may help when your USB connection is stable.

## 5. Connect Wi-Fi

Open Loki's menu with the top side button:

```text
Wi-Fi
Time
Appearance
About
Back
```

Select **Wi-Fi**, choose your network, type the password using the on-device keyboard, then select **CONNECT**.

Credentials are stored in ESP32 Preferences/NVS for automatic reconnection.

## 6. Appearance

Inside:

```text
Appearance
```

you can configure:

```text
Color
Theme
Brightness
Back
```

Included accent colors:

- Cyan
- Green
- Magenta
- Yellow
- Orange

The status bar shows:

```text
TIME            LOKI            BATTERY
```

with a sharp 4-block battery indicator.

## 7. Language support

M5Loki dynamically follows the spoken language.

```text
English speech → English reply
Arabic speech  → Arabic reply
```

Arabic rendering uses:

- UTF-8 decoding
- Arabic contextual shaping
- RTL visual ordering
- Embedded Arabic glyphs

Western digits `0-9` are used in both languages.

## 8. Choose the Gemini model

M5Loki currently defaults to:

```cpp
const char* GEMINI_MODEL = "gemini-flash-lite-latest";
```

This is the model alias tested with this firmware and is the recommended starting point for M5Loki.

### Option A — tested M5Loki default

```cpp
const char* GEMINI_MODEL = "gemini-flash-lite-latest";
```

`gemini-flash-lite-latest` is the alias tested with this project. Because `latest` aliases can move to newer releases, if it becomes unavailable or behaves differently, switch to a pinned stable Flash-Lite model.

### Option B — pin a stable Flash-Lite model

Google currently lists **Gemini 3.5 Flash-Lite** as a stable, low-latency model with audio input and structured output support:

```cpp
const char* GEMINI_MODEL = "gemini-3.5-flash-lite";
```

Before changing the model, check the official Gemini model page because model names, quotas, and deprecation dates can change.

Official model documentation:

- https://ai.google.dev/gemini-api/docs/models
- https://ai.google.dev/gemini-api/docs/models/gemini-3.5-flash-lite

### Which model should I use?

| Model choice | Best for |
|---|---|
| `gemini-flash-lite-latest` | Tested default for M5Loki; easiest starting point |
| `gemini-3.5-flash-lite` | A pinned stable Flash-Lite model |
| Full Flash models | More capability, but usually unnecessary for Loki's short pet-style interactions |

M5Loki does **not** need a large reasoning model. The goal is a short, responsive pet-like conversation, so Flash-Lite is the natural fit.

## 9. Quota / HTTP 429

If Loki works once and then shows:

```text
429 RESOURCE_EXHAUSTED
```

your Gemini project has reached a quota or rate limit.

Check **Usage**, **Rate Limit**, and your billing/free-tier status in Google AI Studio. Waiting for the quota window to reset or using a project with sufficient quota may be required.

The error is not necessarily a microphone or M5Stick hardware failure.

## 10. Customization

Useful places to customize in `M5Loki.ino` include:

- Loki's Gemini personality/system instruction
- Reaction behavior
- Accent colors
- Menu items
- Birthday melody
- UI geometry
- Brightness limits
- Reply length
- Face shapes

The reply fonts are embedded bitmap fonts. Changing their pixel size requires regenerating the font data rather than simply calling `setTextSize()`.

## API-key security note

This project runs on a microcontroller, so any API key stored in flashed firmware should be treated as a device credential rather than a perfectly secret server-side credential.

For personal/open-source experimentation, the local `config.h` workflow is simple and practical. For a commercial or widely distributed product, use a backend/proxy or another architecture that does not distribute a reusable cloud API credential in client firmware.

## Roadmap ideas

- Optional on-device API-key entry
- Conversation history
- Additional semantic actions
- More sound themes
- OTA firmware updates
- Optional local/offline AI integrations

## Contributing

Issues, fixes, documentation improvements, UI ideas, and new reactions are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

M5Loki is released under the **MIT License**. See [LICENSE](LICENSE).

You may use, modify, study, and redistribute the project under the terms of that license.

## Author

**Fahad AlAjmi**

- GitHub: [@faajmid](https://github.com/faajmid)
- Instagram: [@faajmid](https://instagram.com/faajmid)
- TikTok: [@faajmid](https://www.tiktok.com/@faajmid)
- Email: [faajmid@gmail.com](mailto:faajmid@gmail.com)

---

If you build your own Loki, consider sharing a photo or improvement with the project.
