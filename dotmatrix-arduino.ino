/** @file
 *  @brief ディスプレイ制御プログラム
 */

#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>
#include "Fonts.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW ///< ハードウェアタイプを定義

// ディスプレイのピン設定
#define MAX_DEVICES 16 ///< デバイスの最大数を定義
#define DATA_PIN 11 ///< MOSI ピンを定義
#define CS_PIN 10   ///< SS ピンを定義
#define CLK_PIN 13  ///< SCK ピンを定義

// MD_Parola インスタンスを作成
MD_Parola p = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES); ///< MD_Parola インスタンスを作成

/** @brief フォントデータを格納する配列 */
uint8_t font[8][17] = {};

/** @brief セットアップ関数
 *  @details ディスプレイとシリアル通信の初期化を行う
 */
void setup() {
  initSerial();
  initSPI();
  initDisplay();
}

/** @brief シリアル通信の初期化
 */
void initSerial() {
  Serial.begin(9600); ///< シリアル通信の開始
}

/** @brief SPI通信の初期化
 */
void initSPI() {
  SPI.begin(); ///< SPI通信の開始
}

/** @brief ディスプレイの初期化
 */
void initDisplay() {
  p.begin(2); ///< ディスプレイの初期化
  
  p.setIntensity(1);

  p.setZone(0, 0, 7); ///< ゾーン0の設定
  p.setZone(1, 8, 15); ///< ゾーン1の設定

  for(int i = 0; i < 8; i++)
    p.setFont(i, dFont); ///< フォントの設定

  for (int i = 0; i < 8; ++i)
    p.addChar('\x1' + i, font[i]); ///< キャラクタの追加
  
  p.displayZoneText(0, "\x7\x5\x3\x1", PA_LEFT, 50, 0, PA_PRINT, PA_NO_EFFECT); ///< ゾーン0のテキスト表示
  p.displayZoneText(1, "\x8\x6\x4\x2", PA_LEFT, 50, 0, PA_PRINT, PA_NO_EFFECT); ///< ゾーン1のテキスト表示
}

/** @brief データ解析関数
 *  @param data 解析するデータ
 *  @param separator データの区切り文字
 *  @details データを解析し、フォントデータを更新する
 */
void parseData(String data, char separator) {
  int startIndex = 0;
  int endIndex = 0;

  int i = 0;

  while ((endIndex = data.indexOf(separator, startIndex)) != -1) {
    String token = data.substring(startIndex, endIndex);
    int number = token.toInt();

    for(int j = 0; j < 8; j++)
      if(i >= j*17 && i < (j+1)*17)
        font[j][i - j*17] = number;
    
    i++;
    startIndex = endIndex + 1;
  }

  // 最後の数値を取得
  String lastToken = data.substring(startIndex);
  int lastNumber = lastToken.toInt();
  font[7][16] = lastNumber;
}

/** @brief メインループ関数
 *  @details シリアル通信からデータを読み取り、ディスプレイに表示する
 */
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