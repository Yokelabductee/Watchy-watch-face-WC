#pragma once
#include <Watchy.h>

// ╔══════════════════════════════════════════════════════════════╗
//  WATCHY WORD CLOCK — settings
//
//  ── WiFi credentials ──────────────────────────────────────────
//  Do NOT put WiFi credentials here. Watchy uses WiFiManager:
//
//  1. Upload this sketch.
//  2. On the watch: Back+Up buttons → "Setup WiFi" in the menu.
//  3. On your phone/PC: connect to the "Watchy AP" WiFi network.
//  4. A browser page opens at 192.168.4.1 → enter your SSID
//     and password → Save.
//  5. Watchy connects, syncs time from the NTP server below,
//     and returns to the watch face.
//  You only need to do this once — credentials are stored on
//  the watch and survive firmware updates.
//
//  ── Quotes answer ─────────────────────────────────────────────
//  In C++, string values always stay inside double quotes.
//  Example:  .ntpServer = "time.google.com"
//  If your WiFi name is  MyHome  → type  "MyHome"  with quotes.
// ╚══════════════════════════════════════════════════════════════╝

const watchySettings settings = {
    // ── Weather (unused — leave blank) ────────────────────────
    .cityID                = "",
    .lat                   = "",
    .lon                   = "",
    .weatherAPIKey         = "",
    .weatherURL            = "",
    .weatherUnit           = "",
    .weatherLang           = "",
    .weatherUpdateInterval = 0,

    // ── NTP ───────────────────────────────────────────────────
    // time.google.com is reliable and fast worldwide.
    // gmtOffset = 0 → RTC stores UTC; localHour() in
    // WordClock.ino automatically applies BST (+1) or GMT (+0).
    .ntpServer             = "time.google.com",
    .gmtOffset             = 0,

    // ── Vibration ─────────────────────────────────────────────
    .vibrateOClock         = false,   // buzzer disabled
};
