#include <Arduboy2.h>
#include <Sprites.h>
#include <Keyboard.h>
#include <string.h>
#include "modes.h"

Arduboy2 arduboy;

#define KEYBYTE 0xA5

void setup() {
  arduboy.begin();
  // Need to balance between actual draw need and avoiding blinking serial lights
  arduboy.setFrameRate(15);
  Keyboard.begin();
  Serial.begin(19200);
  arduboy.clear();
}

struct buttonmap {
    uint8_t arduboy;
    uint8_t cmd;
};

enum commands {
    CMDIDLE = 0,
    CMDVOLUP,
    CMDVOLDOWN,
    CMDNEXT,
    CMDPREV,
    CMDPLAYPAUSE,
    CMDSOMETHING,
};

enum state {
    STATESTOP = 0,
    STATEPLAY,
    STATEPAUSE,
};

struct status {
    int16_t volume;
    int16_t pos;
    uint8_t state;
    uint8_t songinfo;
};

struct songinfo {
    uint16_t duration;
    char title[21];
    char artist[21];
    char album[21];
};

/* https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardmodifiers/
 * for other keys.
 * Would need special handling for Ctrl + Modifiers. */
const struct buttonmap mappings[] = {
    {A_BUTTON, CMDPLAYPAUSE},
    {B_BUTTON, CMDSOMETHING},
    {LEFT_BUTTON, CMDPREV},
    {UP_BUTTON, CMDVOLUP},
    {RIGHT_BUTTON, CMDNEXT},
    {DOWN_BUTTON, CMDVOLDOWN},
};

const size_t NUMMAPPINGS = sizeof(mappings) / sizeof(mappings[0]);

static struct status stat = {0};
static struct songinfo info = {0};

void send_cmd()
{
    size_t i;
    uint8_t cmd = CMDIDLE;

    arduboy.pollButtons();

    for (i = 0; i < NUMMAPPINGS; i++)
    {
        if (arduboy.justReleased(mappings[i].arduboy))
        {
            cmd = mappings[i].cmd;
            break;
        }
    }
    
    Serial.write(KEYBYTE);
    Serial.write(cmd);
}

void read_stats()
{
    // Re-zero the stats each time so we don't accidentally
    // try to re-obtain songinfo after disconnect
    memset((char *)&stat, 0, sizeof(stat));

    Serial.readBytes((char *)&stat, sizeof(stat));
    if (stat.songinfo)
    {
        Serial.readBytes((char *)&info, sizeof(info));

        // Be paranoid about null termination
        info.title[20] = '\0';
        info.artist[20] = '\0';
        info.album[20] = '\0';
    }
}

void draw_bar(int row, int val, int max)
{
    int percent = 0; 
    if (val >= 0) 
        percent = (int)(((float)val / (float)max) * 100.0);
    // aim for 100 pixels, skewed to the right. Actually 102 with buffer
    // Height is 6 pixels outer, two active
    arduboy.drawRect(20, row * 8 + 1, 102, 6);
    arduboy.drawRect(22, row * 8 + 3, percent, 2);
}

void draw_time(int totalseconds)
{
    int minutes = totalseconds / 60;
    int seconds = totalseconds % 60;
    
    if (totalseconds < 0)
    {
        arduboy.print("###");
    }
    else
    {
        arduboy.print(minutes);
        if (seconds < 10)
            arduboy.print(":0");
        else
            arduboy.print(":");
        arduboy.print(seconds);
    }
}

/* Full display resolution is 128 x 64
 * Font size is 6 x 8 pixels
 * Available display is 20 char x 8 lines */
void draw_display()
{
    arduboy.clear();
    
    /* Rows 1-3 for title/artist/album */
    arduboy.print(info.title);
    arduboy.print('\n');
    arduboy.print(info.artist);
    arduboy.print('\n');
    arduboy.print(info.album);
    arduboy.print('\n');
    
    /* Followed by position bar, play/pause icon and text */
    arduboy.print("\nPos\n    ");
    draw_bar(4, stat.pos, info.duration);
    Sprites::drawOverwrite(4, 5 * 8, modes, stat.state);
    draw_time(stat.pos);
    arduboy.print("/");
    draw_time(info.duration);
    arduboy.print("\n\n");

    /* Then volume bar */
    arduboy.print("Vol");
    draw_bar(7, stat.volume, 100);
}

/* Also reminder: F() macro is to convert string constant from flash */

void loop() {
    if (!(arduboy.nextFrame()))
        return;

    send_cmd();
    read_stats();
    draw_display();

    arduboy.display();
}
