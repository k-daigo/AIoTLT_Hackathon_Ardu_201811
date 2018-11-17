/**
* @brief 履歴系
* @author k-daigo
* @date 2018/11/17
* @details -
*/

#include "Const.h"
#include "Rireki.h"

#define OBEYA_THRESHOLD 50.0

/**
* @brief 履歴の保存を行う
*/
void save_rireki(double result_predict)
{
    // NOP
    return;
}

/**
* @brief サーバにデータを送信する
*/
void send_to_server(double result_predict)
{
    // NOP
    return;
}

/**
* @brief 汚部屋判定
*/
int judge_obeya(double result_predict)
{
    if (result_predict > OBEYA_THRESHOLD)
    {
        return VOICE_KBN_ALERT;
    }

    return VOICE_KBN_GOOD;
}
