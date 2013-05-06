#include <cstdio>
#include <opencv2/opencv.hpp>
#include "AES.h"

using namespace std;
using namespace cv;

#define filename "Stickies.jpg"
#define window_name "after"
#define kernel 25

unsigned char key[] = {
    0x2b, 0x7e, 0x15, 0x16,
    0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88,
    0x09, 0xcf, 0x4f, 0x3c
};

Mat _src, _dst, roi;
Rect rect;
Point point;
int drag = 0;

void mouseHandler(int event, int x, int y, int flags, void* param)
{
    /* user press left button */
    if (event == CV_EVENT_LBUTTONDOWN && !drag) {
        point = Point(x, y);
        drag  = 1;
    }

    /* user drag the mouse */
    if (event == CV_EVENT_MOUSEMOVE && drag) {
        _dst = _src.clone();
        rectangle(_dst, point, Point(x, y), CV_RGB(255, 0, 0), 1, 8, 0);
        imshow("img", _dst);
    }

    /* user release left button */
    if (event == CV_EVENT_LBUTTONUP && drag) {
        _dst = _src.clone();

        rect = Rect(point.x, point.y, x - point.x, y - point.y);
        roi = _dst(rect);
        medianBlur(roi, roi, kernel);
        medianBlur(roi, roi, kernel);
        imshow("img", _dst);
        drag = 0;
    }

    /* user click right button: reset all */
    if (event == CV_EVENT_RBUTTONUP) {
        imshow("img", _src);
        rect.width = rect.height = 0;
        drag = 0;
    }
}

int main (int argc, char *argv[])
{
    _src = imread(filename , 1);

    namedWindow("img", 1);

    setMouseCallback("img", mouseHandler, NULL);
    imshow("img", _src);
    waitKey(0);

    destroyWindow("img");

    if (rect.area() > 0) {
        /* output blurred image */
        imwrite("result.jpg", _dst);

        short x, y, w, h;
        x = rect.x;
        y = rect.y;
        w = rect.width;
        h = rect.height;
        cout << x << " " << y << " " << w << " " << h << endl;

        /* init AES with key and set buffer */
        AES aes(key);
        roi = _src(rect);
        int size = ((3*roi.cols*roi.rows+15)>>4)<<4;
        unsigned char *input = new unsigned char[size], *temp;
        memset(input, 0, size);
        for (int i = 0; i < roi.rows; i++)
            memcpy(input+i*3*roi.cols, roi.data+i*_dst.cols*3, 3*roi.cols);

        /* encrypt 128 bits each time until the whole roi is encryted */
        for (int i = 0; i < size; i+=16) {
            temp = input+i;
            aes.Cipher(temp);
        }

        /* encrypt offset and size info */
        unsigned char info[16] = "";
        memcpy(info, &x, sizeof(x));
        memcpy(info+2, &y, sizeof(y));
        memcpy(info+4, &w, sizeof(w));
        memcpy(info+6, &h, sizeof(h));
        memcpy(info+8, "pixAES", 6);
        aes.Cipher(info);

        /* append encrypted block of image and encryted info to blurred image */
        FILE *f = fopen("result.jpg", "ab");
        fwrite(input, 1, size, f);
        fwrite(info, 1, 16, f);
        fclose(f);

        cout << "Your image has been encrypted" << endl;
    }

    return 0;
}
