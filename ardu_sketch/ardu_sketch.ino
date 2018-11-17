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
    if (voice_kbn == VOICE_KBN_ALTER)
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

  // 履歴保存
  SaveRireki();

  // 汚部屋度判定
  if (prev_voice_kbn != VOICE_KBN_ALTER)
  {
    // 汚部屋閾値以上
    if (true)
    {
      voice_kbn = VOICE_KBN_ALTER;

      // インスタ投稿
      // NOP
    }
  }
  else
  {
    // 汚部屋閾値未満
    voice_kbn = VOICE_KBN_GOOD;
  }
}
