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

#define PROC_INTERVAL (30 * 60 * 1000)

// ボイス区分
int voice_kbn = VOICE_KBN_GOOD;
int prev_voice_kbn = VOICE_KBN_GOOD;

void setup()
{
}

void loop()
{
  // 一定時間Sleep
  delay(PROC_INTERVAL);

  // 人がいるか判定（動体検出）
  boolean hito_iru = false;
  if (hito_iru)
  {
    if (voice_kbn == VOICE_KBN_ALERT)
    {
      // ボイス発話（アラート）
    }
    else
    {
      // ボイス発話（褒める）
    }
    return;
  }

  // 人がいない処理
  // 画像撮影

  // 推論実行
  double result_predict = predict();

  // 履歴保存
  save_rireki(result_predict);
  send_to_server(result_predict);

  // 汚部屋度判定
  int now_voice_kbn = judge_obeya(result_predict);

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
