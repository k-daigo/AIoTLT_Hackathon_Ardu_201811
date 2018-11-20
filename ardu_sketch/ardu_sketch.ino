/**
* @brief メイン
* @author k-daigo
* @date 2018/11/17
* @details -
*/

#include "Const.h"
#include "ML.h"
#include "Rireki.h"
#include "Sound.h"

#define PROC_INTERVAL 1000

// ボイス区分
int voice_kbn = VOICE_KBN_NONE;
int prev_voice_kbn = VOICE_KBN_GOOD;

void setup()
{
  Serial.begin(9600);

  ml_init();
}

void loop()
{
  // 一定時間Sleep
  delay(PROC_INTERVAL);

  Serial.print("voice_kbn=");
  delay(1);
  Serial.println(voice_kbn);
  delay(1);

  // 人がいるか判定（動体検出）
  boolean hito_iru = true;
  voice_kbn = VOICE_KBN_GOOD;
  if (hito_iru)
  {
    if (voice_kbn == VOICE_KBN_ALERT)
    {
      // ボイス発話（アラート）
      output_sound(VOICE_KBN_ALERT);
      return;
    }
    else
    {
      // ボイス発話（褒める）
      output_sound(VOICE_KBN_GOOD);
      return;
    }
  }

  // 人がいない処理
  // 画像撮影

  // 推論実行
  int result_predict = predict();

  Serial.print("result_predict=");
  delay(1);
  Serial.println(result_predict);
  delay(1);

  // 履歴保存
  save_rireki(result_predict);
  send_to_server(result_predict);

  // 汚部屋度判定
  int now_voice_kbn = judge_obeya(result_predict);

  Serial.print("now_voice_kbn=");
  delay(1);
  Serial.println(now_voice_kbn);
  delay(1);

  // 変動なしの為のNONE区分
  voice_kbn = VOICE_KBN_NONE;

  // 前回がキレイで今回汚部屋になったらアラート
  if (prev_voice_kbn != VOICE_KBN_ALERT)
  {
    if (now_voice_kbn == VOICE_KBN_ALERT)
    {
      voice_kbn = VOICE_KBN_ALERT;
    }
  }
  // 前回が汚部屋で今回キレイになったら褒める
  else
  {
    if (now_voice_kbn == VOICE_KBN_GOOD)
    {
      voice_kbn = VOICE_KBN_GOOD;
    }
  }

  // 前回の汚部屋度として保持
  prev_voice_kbn = now_voice_kbn;
}
