# M5Loki

**Open-source AI pet firmware for M5StickC Plus2.**

M5Loki turns an M5StickC Plus2 into a small voice-driven AI pet with English/Arabic conversations, expressive reactions, on-device Wi-Fi setup, appearance settings, time/battery status, and playful special actions.

<p align="center">
  <img src="docs/images/m5loki-demo-placeholder.svg" alt="M5Loki" width="700">
</p>

> Replace the demo placeholder with a photo or GIF of your own M5Loki build if you fork the project.

## Features

- Voice recording from the M5StickC Plus2 microphone
- Gemini-powered speech understanding and short pet-style replies
- Automatic English / Arabic language detection
- Custom Arabic shaping + RTL rendering
- Matched embedded 16 px English and Arabic reply fonts
- Reactive faces: neutral, happy, love, surprised, sad, angry, sleep, dead
- Recording waveform + fixed typing/message indicator
- Birthday action with cake + buzzer melody
- On-device Wi-Fi scanner and password keyboard
- Appearance menu: color, dark/light theme, brightness
- Time + sharp 4-block battery indicator
- Persistent settings using ESP32 Preferences/NVS
- No phone companion app required

## Hardware

- **M5StickC Plus2**
- USB-C cable
- 2.4 GHz Wi-Fi
- Your own Gemini API key

## Controls

| Action | Control |
|---|---|
| Talk to Loki | Hold **M5** |
| Send recording | Release **M5** |
| Show reaction | Click **M5** after a normal reply |
| Stop birthday melody | Press **M5** |
| Open menu | Top side button |
| Navigate | Top / bottom side buttons |
| Select | M5 |

Normal flow:

```text
Hold M5 → Recording → Release → Typing → Loki reply → Click M5 → Reaction
```

Birthday flow:

```text
"It's my birthday" / "Play birthday music" / "اليوم عيد ميلادي"
                    ↓
             Cake + birthday melody
                    ↓
               Press M5 anytime
                    ↓
                Happy Loki face
```

## Repository structure

```text
M5Loki/
├── M5Loki/
│   ├── M5Loki.ino                 # Arduino sketch entry point
│   ├── config.example.h           # API-key template
│   ├── M5Loki_prototypes.h
│   ├── M5Loki_globals_01.inc ...  # Embedded fonts + global state
│   └── M5Loki_impl_01.inc ...     # Firmware implementation
├── docs/images/
│   ├── google-ai-studio-api-keys.svg
│   ├── google-ai-studio-api-key-details-censored.svg
│   └── m5loki-demo-placeholder.svg
├── .gitignore
├── CONTRIBUTING.md
├── LICENSE
├── README.md
└── SECURITY.md
```

The firmware is split into small include files only to keep the repository manageable. Open **`M5Loki/M5Loki.ino`** in Arduino IDE; it includes the remaining firmware files automatically.

## 1. Arduino setup

Install **Arduino IDE 2.x**, add ESP32 board support, and select the correct M5StickC Plus2 / ESP32 board and port for your setup.

Install these libraries from Arduino Library Manager:

- `M5StickCPlus2`
- `M5Unified`
- `M5GFX`
- `ArduinoJson`

The firmware also uses standard ESP32 libraries for Wi-Fi, HTTPS, Preferences, and base64 handling.

## 2. Get your own Gemini API key

Every M5Loki user should use **their own API key**. No API key is included in this repository.

1. Open **Google AI Studio**: https://aistudio.google.com/
2. Sign in.
3. Open **API Keys** from the left sidebar.
4. Click **Create API key**.
5. Choose or create the Google Cloud project you want to use.
6. Open the new key and click **Copy key**.
7. Keep the key private.

Official guide: https://ai.google.dev/gemini-api/docs/api-key

### API Keys page

<p align="center">
  <img src="docs/images/google-ai-studio-api-keys.svg" alt="Google AI Studio API Keys page guide" width="850">
</p>

### API key details

<p align="center">
  <img src="docs/images/google-ai-studio-api-key-details-censored.svg" alt="Censored Google AI Studio API key example" width="700">
</p>

> The red bar represents a censored key. **Never publish or commit your real API key.**

## 3. Add your key to M5Loki

Inside the `M5Loki` sketch folder, copy:

```text
config.example.h
```

and rename the copy to:

```text
config.h
```

Then edit `config.h`:

```cpp
#pragma once

#define GEMINI_API_KEY "PASTE_YOUR_GEMINI_API_KEY_HERE"
```

Your real line should have this structure:

```cpp
#define GEMINI_API_KEY "your-complete-private-key-goes-here"
```

Do not remove the quotation marks and do not add spaces inside the key.

`config.h` is intentionally listed in `.gitignore`, so each user keeps their own key locally.

### If you accidentally expose a key

Revoke/delete it in Google AI Studio immediately, create a replacement, and update your local `config.h`. Removing a secret only from the newest Git commit is not enough because it may remain in Git history.

## 4. Choose the Gemini model

The firmware currently uses the model that was tested during development:

```cpp
const char* GEMINI_MODEL = "gemini-flash-lite-latest";
```

This is the easiest starting point for M5Loki. Because a `latest` alias can change over time, you can instead pin a currently supported stable Flash-Lite model.

As of July 2026, Google lists:

```cpp
const char* GEMINI_MODEL = "gemini-3.5-flash-lite";
```

as a stable Flash-Lite model with audio input and structured-output support.

Check Google's current model documentation before changing the string:

- https://ai.google.dev/gemini-api/docs/models
- https://ai.google.dev/gemini-api/docs/models/gemini-3.5-flash-lite

| Choice | Use it when |
|---|---|
| `gemini-flash-lite-latest` | You want the tested M5Loki default / latest Flash-Lite alias |
| `gemini-3.5-flash-lite` | You want a pinned stable Flash-Lite model |

M5Loki is designed around quick, short interactions, so a Flash-Lite model is a better fit than a large reasoning model.

## 5. Upload the firmware

Open:

```text
M5Loki/M5Loki.ino
```

Connect the M5StickC Plus2 by USB-C, select the correct board and port, then **Verify** and **Upload**.

Compilation can take longer than a small Arduino sketch because M5Loki embeds custom English and Arabic bitmap fonts. If flashing itself is slow, a higher ESP32 upload speed such as `921600` may help when your USB connection is stable.

## 6. Connect Wi-Fi

Open the top-button menu:

```text
Wi-Fi
Time
Appearance
About
Back
```

Select **Wi-Fi**, choose your network, enter the password using the on-device keyboard, then choose **CONNECT**. Wi-Fi credentials are stored locally in ESP32 Preferences/NVS for automatic reconnection.

## 7. Appearance

Inside **Appearance**:

```text
Color
Theme
Brightness
Back
```

Available accent colors are Cyan, Green, Magenta, Yellow, and Orange. The device also supports dark/light mode and persistent brightness control.

## 8. English and Arabic

Loki automatically follows the spoken language:

```text
English speech → English reply
Arabic speech  → Arabic reply
```

Arabic rendering includes UTF-8 decoding, contextual shaping, RTL visual ordering, and embedded Arabic glyphs. Western digits `0-9` are used in both languages.

## 9. HTTP 429 / quota errors

If you see:

```text
429 RESOURCE_EXHAUSTED
```

check **Usage**, **Rate Limit**, and billing/free-tier status in Google AI Studio. This normally indicates Gemini quota/rate limiting rather than a microphone or M5Stick hardware failure.

## Security note

This is microcontroller firmware, so a key stored in flashed firmware should not be treated like a perfectly protected server-side secret. The local `config.h` workflow is suitable for personal/open-source experimentation. For a commercial product, prefer a backend/proxy architecture that does not distribute a reusable cloud credential to client devices.

## Contributing

Issues, fixes, documentation improvements, UI ideas, and new reactions are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

M5Loki is open source under the **MIT License**. See [LICENSE](LICENSE).

## Author

**Fahad AlAjmi**

- GitHub: [@faajmid](https://github.com/faajmid)
- Instagram: [@faajmid](https://instagram.com/faajmid)
- TikTok: [@faajmid](https://www.tiktok.com/@faajmid)
- Email: [faajmid@gmail.com](mailto:faajmid@gmail.com)
