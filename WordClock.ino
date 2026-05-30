// ═══════════════════════════════════════════════════════════════
//  WORD CLOCK  –  Watchy watch face  v2
//  Font: DejaVu Sans Mono Bold 9pt (auto-generated WordClockFont.h)
//  Features:
//    • Auto UK BST / GMT  (no manual change ever needed)
//    • Nighttime blackout  22:00 – 07:00
//    • Buzzer / vibrate disabled  (vibrateOClock = false)
//    • gmtOffset = 0  (RTC stores UTC, localHour() handles offset)
//    • Battery icon  (bottom-right corner, white on black)
//    • MINUTE singular / MINUTES plural
// ═══════════════════════════════════════════════════════════════

#include <Watchy.h>
#include "settings.h"
#include "WordClockFont.h"

// ── Deep-sleep persistent state ──────────────────────────────
RTC_DATA_ATTR bool    displaySleeping = false;
RTC_DATA_ATTR uint8_t lastHour        = 99;   // 99 → full refresh on first boot

// ── Nighttime blackout window (local time, 24 h) ─────────────
#define SLEEP_FROM   00    // Midnight  ← only these two lines need changing
#define SLEEP_UNTIL  7    //  7:00 AM

// ── Character grid  (16 cols × 12 rows) ──────────────────────
static const char GRID[12][17] = {
  "THEBTIMERISEHALF",   // row  0
  "QUARTERTWENTYYYX",   // row  1
  "TENSIXTEENTWONEL",   // row  2
  "EIGHTEENFIVERXXX",   // row  3
  "SEVENTEENINETEEN",   // row  4
  "FOURTEENTHIRTEEN",   // row  5
  "TWELVELEVENTHREE",   // row  6
  "MINUTESXPASTOWOR",   // row  7
  "TWONELEVENINESIX",   // row  8
  "SEVENTHREETWELVE",   // row  9
  "FOURXFIVEIGHTEND",   // row 10
  "XXXOCLOCKXAMXPMX",   // row 11  (cols 0-2 = battery zone, suppressed)
};

// ── Word definition {row, startCol, length} ──────────────────
struct WD { uint8_t r, c, l; };

static const WD W_THE      = {0,  0, 3};
static const WD W_TIME     = {0,  4, 4};
static const WD W_IS       = {0,  9, 2};
static const WD W_HALF     = {0, 12, 4};
static const WD W_QUARTER  = {1,  0, 7};
static const WD W_TWENTY   = {1,  7, 6};
static const WD W_TEN_M    = {2,  0, 3};
static const WD W_SIX_M    = {2,  3, 3};
static const WD W_SIXTEEN  = {2,  3, 7};
static const WD W_TWO_M    = {2, 10, 3};
static const WD W_ONE_M    = {2, 12, 3};
static const WD W_EIGHT_M  = {3,  0, 5};
static const WD W_EIGHTEEN = {3,  0, 8};
static const WD W_FIVE_M   = {3,  8, 4};
static const WD W_SEVEN_M  = {4,  0, 5};
static const WD W_SEVENTN  = {4,  0, 9};
static const WD W_NINE_M   = {4,  8, 4};
static const WD W_NINETEEN = {4,  8, 8};
static const WD W_FOUR_M   = {5,  0, 4};
static const WD W_FOURTEEN = {5,  0, 8};
static const WD W_THIRTEEN = {5,  8, 8};
static const WD W_TWELVE_M = {6,  0, 6};
static const WD W_ELEVEN_M = {6,  5, 6};
static const WD W_THREE_M  = {6, 11, 5};
static const WD W_MINUTE   = {7,  0, 6};  // singular – no trailing S
static const WD W_MINUTES  = {7,  0, 7};  // plural
static const WD W_PAST     = {7,  8, 4};
static const WD W_TO       = {7, 11, 2};
static const WD W_AM       = {11,10, 2};
static const WD W_PM       = {11,13, 2};
static const WD W_OCLOCK   = {11, 3, 6};
static const WD HOURS[13]  = {
  {0, 0, 0},       // [0]  unused
  {8,  2, 3},      // [1]  ONE
  {8,  0, 3},      // [2]  TWO
  {9,  5, 5},      // [3]  THREE
  {10, 0, 4},      // [4]  FOUR
  {10, 5, 4},      // [5]  FIVE
  {8, 13, 3},      // [6]  SIX
  {9,  0, 5},      // [7]  SEVEN
  {10, 8, 5},      // [8]  EIGHT
  {8,  9, 4},      // [9]  NINE
  {10,12, 3},      // [10] TEN
  {8,  4, 6},      // [11] ELEVEN
  {9, 10, 6},      // [12] TWELVE
};

// ── Layout constants ─────────────────────────────────────────
// WordClockFont: advance 12 px wide × 16 px yAdvance
// 16 cols × 12 = 192 px → 4 px margin  |  12 rows × 16 = 192 px → 4 px margin
static const uint8_t MARGIN = 4;
static const uint8_t CW     = 12;
static const uint8_t CH     = 16;

// ─────────────────────────────────────────────────────────────
//  lastSundayOf  –  pure maths, no class members needed
// ─────────────────────────────────────────────────────────────
static uint8_t lastSundayOf(uint16_t year, uint8_t month) {
  static const uint8_t dom[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  uint8_t lastDay = dom[month - 1];
  if (month == 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
    lastDay = 29;
  tmElements_t t;
  t.Year = year - 1970; t.Month = month; t.Day = lastDay;
  t.Hour = 12; t.Minute = 0; t.Second = 0;
  return lastDay - (weekday(makeTime(t)) - 1);  // weekday() 1 = Sunday
}

// ─────────────────────────────────────────────────────────────
class WordClock : public Watchy {
public:
  WordClock(const watchySettings& s) : Watchy(s) {}
  void drawWatchFace() override;

private:
  bool    isUKBST();
  uint8_t localHour();
  bool    isSleepTime();
  void    vibrateIfAwake(uint8_t times = 1);
  void    buildActiveMap(bool active[12][16]);
  void    markMinuteWords(bool active[12][16], uint8_t n);
  void    mk(bool active[12][16], const WD& w);
  void    drawGrid(bool active[12][16]);
  void    drawBattery();
};

// ── Auto UK BST / GMT ─────────────────────────────────────────
bool WordClock::isUKBST() {
  uint16_t yr  = currentTime.Year + 1970;
  uint8_t  mon = currentTime.Month;
  uint8_t  day = currentTime.Day;
  uint8_t  hr  = currentTime.Hour;  // UTC

  if (mon < 3 || mon > 10) return false;
  if (mon > 3 && mon < 10) return true;

  uint8_t lastSun = lastSundayOf(yr, mon);
  if (mon == 3) {
    if (day < lastSun) return false;
    if (day > lastSun) return true;
    return (hr >= 1);
  }
  if (day < lastSun) return true;
  if (day > lastSun) return false;
  return (hr < 1);
}

uint8_t WordClock::localHour() {
  return (currentTime.Hour + (isUKBST() ? 1 : 0)) % 24;
}

// ── Nighttime blackout ────────────────────────────────────────
bool WordClock::isSleepTime() {
  uint8_t h = localHour();
  if (SLEEP_FROM > SLEEP_UNTIL)
    return (h >= SLEEP_FROM || h < SLEEP_UNTIL);
  return (h >= SLEEP_FROM && h < SLEEP_UNTIL);
}

void WordClock::vibrateIfAwake(uint8_t times) {
  if (!isSleepTime()) vibMotor(75, times);
}

// ── Cell marker ───────────────────────────────────────────────
void WordClock::mk(bool active[12][16], const WD& w) {
  for (uint8_t i = 0; i < w.l; i++) active[w.r][w.c + i] = true;
}

// ── Build active map ──────────────────────────────────────────
void WordClock::buildActiveMap(bool active[12][16]) {
  const uint8_t h    = localHour();
  const uint8_t m    = currentTime.Minute;
  const bool    isTo = (m > 30);
  const uint8_t nh   = (h + 1) % 24;
  auto to12 = [](uint8_t x) -> uint8_t { return (x % 12 == 0) ? 12 : (x % 12); };

  mk(active, W_THE);
  mk(active, W_TIME);
  mk(active, W_IS);
  mk(active, h < 12 ? W_AM : W_PM);
  mk(active, HOURS[isTo ? to12(nh) : to12(h)]);

  if (m == 0) {
    mk(active, W_OCLOCK);
  } else {
    mk(active, isTo ? W_TO : W_PAST);
    uint8_t n = isTo ? (60 - m) : m;
    markMinuteWords(active, n);
  }
}

void WordClock::markMinuteWords(bool active[12][16], uint8_t n) {
  if      (n == 1)              mk(active, W_MINUTE);
  else if (n != 15 && n != 30) mk(active, W_MINUTES);

  if      (n ==  1) mk(active, W_ONE_M);
  else if (n ==  2) mk(active, W_TWO_M);
  else if (n ==  3) mk(active, W_THREE_M);
  else if (n ==  4) mk(active, W_FOUR_M);
  else if (n ==  5) mk(active, W_FIVE_M);
  else if (n ==  6) mk(active, W_SIX_M);
  else if (n ==  7) mk(active, W_SEVEN_M);
  else if (n ==  8) mk(active, W_EIGHT_M);
  else if (n ==  9) mk(active, W_NINE_M);
  else if (n == 10) mk(active, W_TEN_M);
  else if (n == 11) mk(active, W_ELEVEN_M);
  else if (n == 12) mk(active, W_TWELVE_M);
  else if (n == 13) mk(active, W_THIRTEEN);
  else if (n == 14) mk(active, W_FOURTEEN);
  else if (n == 15) mk(active, W_QUARTER);
  else if (n == 16) mk(active, W_SIXTEEN);
  else if (n == 17) mk(active, W_SEVENTN);
  else if (n == 18) mk(active, W_EIGHTEEN);
  else if (n == 19) mk(active, W_NINETEEN);
  else if (n == 20) mk(active, W_TWENTY);
  else if (n == 30) mk(active, W_HALF);
  else {
    mk(active, W_TWENTY);
    switch (n - 20) {
      case 1: mk(active, W_ONE_M);   break;
      case 2: mk(active, W_TWO_M);   break;
      case 3: mk(active, W_THREE_M); break;
      case 4: mk(active, W_FOUR_M);  break;
      case 5: mk(active, W_FIVE_M);  break;
      case 6: mk(active, W_SIX_M);   break;
      case 7: mk(active, W_SEVEN_M); break;
      case 8: mk(active, W_EIGHT_M); break;
      case 9: mk(active, W_NINE_M);  break;
    }
  }
}

// ── Grid renderer ─────────────────────────────────────────────
//  All cells      → glyph drawn twice (normal + 1 px right) for
//                   thicker strokes on e-paper.
//  Active cells   → solid white, no further processing.
//  Inactive cells → 25 % visible: after bold draw, every pixel where
//                   NOT (dx even AND dy even) is blacked out, leaving
//                   one white pixel per 2×2 block as the dither grain.
//  Battery zone   → cols 0-2 of row 11 suppressed entirely (pure black).
void WordClock::drawGrid(bool active[12][16]) {
  display.setFont(&WordClockFont);
  display.setTextWrap(false);
  display.setTextColor(GxEPD_WHITE);

  for (uint8_t r = 0; r < 12; r++) {
    for (uint8_t c = 0; c < 16; c++) {

      // Battery zone — leave pure black for the icon
      if (r == 11 && c <= 2) continue;

      const int16_t px = MARGIN + c * CW;
      const int16_t py = MARGIN + r * CH;

      // Bold draw: two passes for every letter
      display.setCursor(px,     py + CH); display.print(GRID[r][c]);
      display.setCursor(px + 1, py + CH); display.print(GRID[r][c]);

      if (!active[r][c]) {
        // 25 % dither — black out 75 % of cell pixels
        for (uint8_t dy = 0; dy < CH; dy++)
          for (uint8_t dx = 0; dx < CW; dx++)
            if (!(dx % 2 == 0 && dy % 2 == 0))
              display.drawPixel(px + dx, py + dy, GxEPD_BLACK);
      }
    }
  }
  display.setFont(NULL);
}

// ── Battery icon (bottom-left, within suppressed cols 0-2 zone) ──
//  Body 22×12 px  nub 3×6 px  total 25 px — sits inside the 36 px
//  (3 × 12) cleared zone leaving 11 px clear to the right of the nub.
void WordClock::drawBattery() {
  float v   = getBatteryVoltage();
  int   pct = (int)(100.0f * (v - 3.3f) / (4.2f - 3.3f));
  pct = (pct > 100) ? 100 : (pct < 0) ? 0 : pct;

  const int16_t BX = 4, BY = 183;
  display.drawRect(BX,    BY,     22, 12, GxEPD_WHITE);   // body 22×12
  display.fillRect(BX+22, BY + 3,  3,  6, GxEPD_WHITE);   // nub 3×6
  int fw = (18 * pct) / 100;
  if (fw > 0)
    display.fillRect(BX + 2, BY + 2, fw, 8, GxEPD_WHITE);
}

// ── Main draw ─────────────────────────────────────────────────
void WordClock::drawWatchFace() {
  if (isSleepTime()) {
    if (!displaySleeping) {
      // ── First entry into blackout window ──────────────────
      // Fill buffer black and let the library push it to the
      // panel once — this is the single intentional refresh.
      display.fillScreen(GxEPD_BLACK);
      displaySleeping = true;
    } else {
      // ── Already sleeping ──────────────────────────────────
      // Power down the display controller (~1 µA standby).
      // The Watchy library calls display.display() after we
      // return, but the panel ignores SPI commands while
      // hibernated — no physical refresh occurs.
      // Saves the ~10–20 mA refresh current for every skipped
      // wake across the full 9-hour blackout window.
      display.hibernate();
    }
    return;
  }
  if (displaySleeping) {
    // ── First wake after blackout ──────────────────────────
    // The library re-initialises the display before calling us,
    // so the panel is already awake. Clear the flag and fall
    // through to a normal full redraw, which clears any ghosting
    // left from the black sleep screen.
    displaySleeping = false;
  }

  bool active[12][16];
  memset(active, 0, sizeof(active));
  buildActiveMap(active);

  display.fillScreen(GxEPD_BLACK);
  drawGrid(active);
  drawBattery();

  lastHour = localHour();
}

// ─────────────────────────────────────────────────────────────
WordClock wc(settings);
void setup() { wc.init(); }
void loop()  {}
