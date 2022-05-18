
#define Sensitivity 0.02
#define buzzer_pin 4

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI#include <SPI.h>

#endif
#ifdef U8X8_HAVE_HW_I2C#include <Wire.h>

#endif

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);

#include "GameManager.h"

//X,Y Values of joystick between 1 and 0
double cursorX = 0;
double cursorY = 0;

//X,Y values of a move, Source and Destination
int xS = 0;
int yS = 0;
int xD = 0;
int yD = 0;
int SelectedCellIndex = -1; //Index of the source cell

int buttonState = 0; // current state of the button
int lastButtonState = 0; // previous state of the button

bool gameStarted = false;
int screen_status = 0; // Current screen status
GameManager gameManager = NULL;

//lcd screen function
void display_Game_Started() {
  u8g2.clearBuffer();
  u8g2.drawStr(3, 40, "Game Started!");
  u8g2.sendBuffer();
}
void display_Winner(int player) {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 27, ("Player " + String(player) + " Won").c_str());
  u8g2.sendBuffer();
}
void display_PvP_Highlight() {
  u8g2.clearBuffer(); // clear the internal memory
  u8g2.setFont(u8g2_font_helvR14_tr); // choose a suitable font
  u8g2.drawStr(45, 27, "PvP");
  u8g2.drawRFrame(42, 11, 42, 20, 7); // write something to the internal memory
  u8g2.drawStr(42, 50, "CPU");
  u8g2.sendBuffer(); // transfer internal memory to the display
}
void display_CPU_Highlight() {
  u8g2.clearBuffer(); // clear the internal memory
  u8g2.setFont(u8g2_font_helvR14_tr); // choose a suitable font
  u8g2.drawStr(45, 27, "PvP"); // write something to the internal memory
  u8g2.drawStr(42, 50, "CPU");
  u8g2.drawRFrame(40, 33, 45, 20, 7);
  u8g2.sendBuffer(); // transfer internal memory to the display
}
void display_Difficulty(int diff) {
  u8g2.clearBuffer(); // clear the internal memory
  u8g2.setFont(u8g2_font_helvR14_tr); // choose a suitable font
  u8g2.drawStr(10, 36, "1");
  u8g2.drawStr(59, 36, "2");
  u8g2.drawStr(110, 36, "3");
  if (diff == 0)
    u8g2.drawRFrame(7, 20, 18, 20, 7);
  else if (diff == 1)
    u8g2.drawRFrame(55, 20, 18, 20, 7);
  else if (diff == 2)
    u8g2.drawRFrame(106, 20, 18, 20, 7);
  u8g2.sendBuffer(); // transfer internal memory to the display
}

int handle_joystick() {
  //Getting joystick input
  int xValue = analogRead(A0) + 100;
  int yValue = analogRead(A1) + 100;
  buttonState = !digitalRead(7);
  //moving the cursor
  cursorX += map(xValue, 0, 1000, -1, 1) * Sensitivity;
  cursorY += map(yValue, 0, 1000, -1, 1) * Sensitivity;
  //Cursor out of bounds
  if ((int) cursorX == -1) {
    cursorX = 7;
  }
  if ((int) cursorX == 8) {
    cursorX = -1;
  }
  if ((int) cursorY == -1) {
    cursorY = 7;
  }
  if ((int) cursorY == 8) {
    cursorY = -1;
  }

  return (int) cursorX * 8 + (int) cursorY;
}

void handle_menu(int cursorIndex) {
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      if (screen_status == 1) {
        display_Difficulty(0);
        screen_status = 2;
      } else if (screen_status == 0) {
        gameManager = GameManager(0);
        gameManager.printBoard();
        display_Game_Started();
        gameStarted = true;
      } else {
        gameManager = GameManager(screen_status - 1);
        gameManager.printBoard();
        display_Game_Started();
        gameStarted = true;
      }
    }
  }
  if (screen_status <= 1) {
    if (!(cursorIndex / 8 % 2) && screen_status != 0) {
      display_PvP_Highlight();
      screen_status = 0;
    } else if (cursorIndex / 8 % 2 && screen_status != 1) {
      display_CPU_Highlight();
      screen_status = 1;
    }
  } else {
    if (cursorIndex % 8 % 3 == 0 && screen_status != 2) {
      display_Difficulty(0);
      screen_status = 2;
    } else if (cursorIndex % 8 % 3 == 1 && screen_status != 3) {
      display_Difficulty(1);
      screen_status = 3;
    } else if (cursorIndex % 8 % 3 == 2 && screen_status != 4) {
      display_Difficulty(2);
      screen_status = 4;
    }
  }

}
void handle_game(int cursorIndex) {
  for (int i = 0; i < 64; i++)
    gameManager.leds[i].maximizeBrightness(Brightness);
  gameManager.leds[cursorIndex].maximizeBrightness(Brightness * 10);
  if (SelectedCellIndex != -1)
    gameManager.leds[SelectedCellIndex].maximizeBrightness(Brightness * 10);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      if (gameManager.player1 -> isOccupied(1ULL << cursorIndex) || gameManager.player2 -> isOccupied(1ULL << cursorIndex)) {
        tone(buzzer_pin, 1000);
        delay(50);
        noTone(buzzer_pin);
        xS = cursorX;
        yS = cursorY;
        SelectedCellIndex = cursorIndex;
      } else if (SelectedCellIndex != -1) {
        xD = cursorX;
        yD = cursorY;
        int STATUS = gameManager.gameLoop(xS, yS, xD, yD);
        SelectedCellIndex = -1;
        if (STATUS) {
          gameManager = NULL;
          gameStarted = false;
          FastLED.clear();
          display_Winner(STATUS);
          screen_status = 0;
        }
      }
    }
  }
}

void setup() {

  Serial.begin(9600);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(7, INPUT);
  digitalWrite(7, HIGH);

  u8g2.begin();
  display_PvP_Highlight();
  FastLED.addLeds < NEOPIXEL, 6 > (gameManager.leds, BOARD_SIZE * BOARD_SIZE);
  FastLED.setBrightness(Brightness);

}

void loop() {
  int cursorIndex = handle_joystick();
  if (gameStarted)
    handle_game(cursorIndex);
  else
    handle_menu(cursorIndex);
  lastButtonState = buttonState;
  FastLED.show();
}
