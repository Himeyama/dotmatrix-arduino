#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>
#include "Fonts.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// ディスプレイのピン設定
#define MAX_DEVICES 16
#define DATA_PIN 11 // MOSI ピン
#define CS_PIN 10   // SS ピン
#define CLK_PIN 13  // SCK ピン

// MD_Parola インスタンスを作成
MD_Parola p = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

uint8_t font[8][17] = {
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {} 
};

void setup() {
  Serial.begin(9600);

  SPI.begin();

  p.begin(2);
  p.setZone(0, 0, 7);
  p.setZone(1, 8, 15);

  for(int i = 0; i < 8; i++)
    p.setFont(i, dFont);

  for (int i = 0; i < 8; ++i)
    p.addChar('\x1' + i, font[i]);
  
  p.displayZoneText(0, "\x7\x5\x3\x1", PA_LEFT, 50, 0, PA_PRINT, PA_NO_EFFECT);
  p.displayZoneText(1, "\x8\x6\x4\x2", PA_LEFT, 50, 0, PA_PRINT, PA_NO_EFFECT);
}

void parseData(String data, char separator) {
  int startIndex = 0;
  int endIndex = 0;

  int i = 0;

  while ((endIndex = data.indexOf(separator, startIndex)) != -1) {
    String token = data.substring(startIndex, endIndex);
    int number = token.toInt();

    if(i < 17) // 0-16
      font[0][i] = number;
    else if(i < 34) // 17-34
      font[1][i - 17] = number;
    else if(i < 51) // 32-47
      font[2][i - 34] = number;
    else if(i < 68) //48-79
      font[3][i - 51] = number;
    else if(i < 85)
      font[4][i - 68] = number;
    else if(i < 102)
      font[5][i - 85] = number;
    else if(i < 119)
      font[6][i - 102] = number;
    else if(i < 136)
      font[7][i - 119] = number;
    
    i++;
    startIndex = endIndex + 1;
  }

  // 最後の数値を取得
  String lastToken = data.substring(startIndex);
  int lastNumber = lastToken.toInt();
  font[7][16] = lastNumber;
}

int i = 0;

void loop() {
  if(Serial.available()){
    String txt = Serial.readString();
    parseData(txt, ',');
  }

  if(p.displayAnimate()){
    for (int i = 0; i < 8; ++i)
      p.addChar('\x1' + i, font[i]);

    p.displayZoneText(0, "\x1\x3\x5\x7", PA_LEFT, 50, 0, PA_PRINT, PA_NO_EFFECT);
    p.displayZoneText(1, "\x2\x4\x6\x8", PA_LEFT, 50, 0, PA_PRINT, PA_NO_EFFECT);
  }
}
