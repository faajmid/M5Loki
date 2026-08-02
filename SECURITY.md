# Security

## API keys

Never commit a real Gemini API key.

M5Loki keeps the local key in:

```text
M5Loki/config.h
```

That file is ignored by Git.

If a key is accidentally exposed, revoke it immediately in Google AI Studio and create a replacement.

## Reporting a security issue

For security-related concerns, contact:

**faajmid@gmail.com**

Please avoid publishing active credentials or sensitive details in a public GitHub issue.
