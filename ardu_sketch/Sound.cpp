/*
 * Sound.cpp
 * 2018/11/17
 * AIoTLT Hackason ObeyaTeam
 * Masaru Hashimoto
*/

#include "Sound.h"
#include "mbed.h"
#include "const.h"
#include "SdUsbConnect.h"
#include "EasyPlayback.h"
#include "EasyDec_WavCnv2ch.h"

// ファイル名長さ
#define FILE_NAME_LEN          (64)
// SDカードマウント名
#define MOUNT_NAME             "storage"

static EasyPlayback AudioPlayer;

  
void output_sound(int VOICE_KBN) {
    DIR  * d;
    struct dirent * p;

    // ファイルパス
    char file_path[sizeof("/"MOUNT_NAME"/") + FILE_NAME_LEN];
	// SDカード接続
    SdUsbConnect storage(MOUNT_NAME);

    // デコード
    AudioPlayer.add_decoder<EasyDec_WavCnv2ch>(".wav");
    AudioPlayer.add_decoder<EasyDec_WavCnv2ch>(".WAV");

	// 音量設定(0.0～1.0)
    AudioPlayer.outputVolume(0.4);
    
    // SDカード接続まち
    storage.wait_connect();
    // file search
    d = opendir("/"MOUNT_NAME"/");
        size_t len = strlen(p->d_name);
        if (len < FILE_NAME_LEN) {        
          // 状態で分岐する
          switch(VOICE_KBN) {
            // VOICE_KBN_NONE
            case VOICE_KBN_NONE:
              // make file path
              sprintf(file_path, "/%s/%s", MOUNT_NAME, "none_voice.wav");
              break;
            
            // ホメる
            case VOICE_KBN_GOOD:
              // make file path
              sprintf(file_path, "/%s/%s", MOUNT_NAME, "good_voice.wav");
              break;
            
            // アラート
            case VOICE_KBN_ALERT:
              // make file path
              sprintf(file_path, "/%s/%s", MOUNT_NAME, "alert_voice.wav");
              break;
            
            // 想定外数値�
            default:
              // make file path
              sprintf(file_path, "/%s/%s", MOUNT_NAME, "default_voice.wav");
              break;      
          }
          // デバッグ用
          // printf("%s\r\n", file_path);

          // 音声を再生する
          AudioPlayer.play(file_path);
        }
    // ディレクトリクローズ
    closedir(d);
}
