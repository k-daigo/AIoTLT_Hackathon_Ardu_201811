#include <Arduino.h>
#include <Camera.h>
#include <LCD.h>
#include <opencv.hpp>
#include <DisplayApp.h>
#include "ML.h"

using namespace cv;
using namespace std;

#define USE_DISPLAYAPP

#define IMAGE_HW 64
#define IMAGE_VW 64
#define LOOP_WAITTIME 160 // 16ms for 60 fps of camera spec.

Camera camera(IMAGE_HW, IMAGE_VW);
LCD lcd(IMAGE_HW, IMAGE_VW);

#ifdef USE_DISPLAYAPP
DisplayApp display_app;
#endif

// 1MB of NC_BSS is not used for malloc function.
// It's better to secure buffer as a static memory in this NC_BSS
uint8_t lcd_buf[2 * IMAGE_HW * IMAGE_VW] __attribute((section("NC_BSS"), aligned(32)));
uint8_t bgr_buf[3 * IMAGE_HW * IMAGE_VW] __attribute((section("NC_BSS"), aligned(32)));
uint8_t gray_buf[1 * IMAGE_HW * IMAGE_VW] __attribute((section("NC_BSS"), aligned(32)));
uint8_t diff_buf[1 * IMAGE_HW * IMAGE_VW] __attribute((section("NC_BSS"), aligned(32)));
uint8_t back_buf[1 * IMAGE_HW * IMAGE_VW] __attribute((section("NC_BSS"), aligned(32)));
uint8_t kirei_buf[1 * IMAGE_HW * IMAGE_VW] __attribute((section("NC_BSS"), aligned(32)));

#define NUM_DEMO 5
//LineTypes linetypes = LINE_AA;
string string_text[NUM_DEMO] = {
    "Gadget", "Gray", "GaussianBlur", "absdiff + threshold", "findContours"};
bool g_disp_back = false;

int kirei_val[4096];
int setup_count = 0;

// キレイ部屋の画像
Mat img_kirei(IMAGE_VW, IMAGE_HW, CV_8UC1, kirei_buf);

void image_to_array(Mat img1);
int matching(Mat img2);
int matching2(Mat img2);

void ml_init()
{
    camera.begin();
    lcd.begin(lcd_buf, IMAGE_HW, IMAGE_VW);
    pinMode(PIN_SW0, INPUT);
    pinMode(PIN_SW1, INPUT);
    pinMode(PIN_LED_RED, OUTPUT);
}

int predict()
{
    static unsigned long last_time = millis();
    static unsigned long update_last_time = millis();
    static int x = 0, y = 0, ax = 10, ay = 10;
    while ((millis() - last_time) < LOOP_WAITTIME)
        ;
    unsigned long loop_time = millis() - last_time;
    last_time = millis();

    Scalar red(0, 0, 255), green(0, 255, 0), blue(255, 0, 0);
    Scalar yellow = red + green;
    Scalar sky = green + blue;
    Scalar white = Scalar::all(255);
    Scalar pink = Scalar(154, 51, 255);

    Mat img_raw(IMAGE_VW, IMAGE_HW, CV_8UC2, camera.getImageAdr());
    Mat img_bgr(IMAGE_VW, IMAGE_HW, CV_8UC3, bgr_buf);
    Mat img_gray(IMAGE_VW, IMAGE_HW, CV_8UC1, gray_buf);
    Mat img_diff(IMAGE_VW, IMAGE_HW, CV_8UC1, diff_buf);
    Mat img_back(IMAGE_VW, IMAGE_HW, CV_8UC1, back_buf);

    cvtColor(img_raw, img_bgr, COLOR_YUV2BGR_YUYV); //covert YUV to RGB

    if (millis() - update_last_time > 1000)
    {
        cvtColor(img_raw, img_back, COLOR_YUV2GRAY_YUY2); // capturing background image
        GaussianBlur(img_back, img_back, Size(21, 21), 0);
    }

    // YUVからGRAYへ変換
    cvtColor(img_raw, img_gray, COLOR_YUV2GRAY_YUY2); // convert YUV to GRAY
    GaussianBlur(img_gray, img_gray, Size(21, 21), 0);

    // 差異を取得
    absdiff(img_back, img_gray, img_diff);
    // 2値化
    threshold(img_diff, img_diff, 25, 255, THRESH_BINARY);

    //  if(setup_count <= 5) {
    //    // 差異を取得
    //    absdiff(img_gray, img_gray, img_diff);
    //    // 2値化
    //    threshold(img_diff, img_diff, 25, 255, THRESH_BINARY);
    //    cvtColor(img_diff, img_kirei, COLOR_GRAY2BGR);
    //    image_to_array(img_kirei);
    //  }
    //  else if(setup_count > 5){
    // マッチング

    // 差異を取得
    //    absdiff(img_kirei, img_gray, img_diff);
    absdiff(img_back, img_gray, img_diff);
    // 2値化
    threshold(img_diff, img_diff, 25, 255, THRESH_BINARY);
    cvtColor(img_diff, img_bgr, COLOR_GRAY2BGR);
    int sa = matching(img_bgr);
    //  }
    setup_count++;

    //  if(g_disp_back){
    //    cvtColor(img_back, img_bgr, COLOR_GRAY2BGR);
    //  }

    lcd.BGR2YUV(img_bgr.data, lcd_buf, IMAGE_HW, IMAGE_VW);

#ifdef USE_DISPLAYAPP
    size_t jpegSize = camera.createJpeg(IMAGE_HW, IMAGE_VW, img_bgr.data, Camera::FORMAT_RGB888);
    display_app.SendJpeg(camera.getJpegAdr(), jpegSize);
#endif

    return sa;
}

void image_to_array(Mat img1)
{
    //  Serial.println("*image_to_array***********************************");
    //  Serial.println("*image_to_array***********************************");
    //  Serial.println("*image_to_array***********************************");
    //  Serial.println("*image_to_array***********************************");
    //  delay(1);

    int pos = 0;
    for (int height = 0; height < img1.rows; height++)
    {
        Vec3b *ptr = img1.ptr<Vec3b>(height);
        for (int width = 0; width < img1.cols; width++)
        {
            Vec3b bgr = ptr[width];
            kirei_val[pos] = bgr[0];
            pos++;

            //      // debug
            //      Serial.print(kirei_val[pos], HEX);
            //      Serial.print(",");
            //      delay(1);
        }
        //    Serial.println("");
        //    delay(1);
    }
}

// マッチング
int matching2(Mat img2)
{
    //  Serial.println("** matching **********************************");
    //  Serial.println("** matching **********************************");
    //  Serial.println("** matching **********************************");
    //  Serial.println("** matching **********************************");

    delay(1);
    int pos = 0;
    int sa = 0;
    for (int height = 0; height < img2.rows; height++)
    {
        Vec3b *ptr = img2.ptr<Vec3b>(height);
        for (int width = 0; width < img2.cols; width++)
        {
            Vec3b bgr = ptr[width];

            // debug
            //      Serial.print(bgr[0], HEX);
            //      delay(1);
            //      Serial.print(",");
            //      delay(1);

            if (bgr[0] != kirei_val[pos])
            {
                sa++;
            }
            pos++;
        }
        //    Serial.println("");
        //    delay(1);
    }

    return sa;
}

// マッチング
int matching(Mat img2)
{
    int pos = 0;
    int sa = 0;
    for (int height = 0; height < img2.rows; height++)
    {
        Vec3b *ptr = img2.ptr<Vec3b>(height);
        for (int width = 0; width < img2.cols; width++)
        {
            Vec3b bgr = ptr[width];
            if (bgr[0] == 0xFF)
            {
                sa++;
            }
            pos++;
        }
    }

    return sa;
}
