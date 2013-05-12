#include <cstdio>
#include <vector>
#include <opencv2/opencv.hpp>
#include "AES.h"

using namespace std;
using namespace cv;

#define IMG "Stickies.jpg"
#define window_name "after"
#define kernel 51
#define METHOD 2

unsigned char KEY[] = {
    0x2b, 0x7e, 0x15, 0x16,
    0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88,
    0x09, 0xcf, 0x4f, 0x3c
};

Mat _src, _dst, imgROI;
Rect box;
Point point;
int drag;
vector<Point> points;
const Point *pts;
int npts;

/* return i in [a, b] */
int bound(short i,short a,short b)
{
    return min(max(i,min(a,b)),max(a,b));
}

void mouseHandler(int event, int x, int y, int flags, void* param)
{
    x = bound(x, 0, _src.cols-1);
    y = bound(y, 0, _src.rows-1);

    /* user press left button */
    if (event == CV_EVENT_LBUTTONDOWN && !drag) {
        #if METHOD == 1
        point = Point(x, y);
        #else
        points.clear();
        points.push_back(Point(x, y));
        #endif
        drag  = 1;
    }

    /* user drag the mouse */
    if (event == CV_EVENT_MOUSEMOVE && drag) {
        _dst = _src.clone();

        #if METHOD == 1
        rectangle(_dst, point, Point(x, y), CV_RGB(255, 0, 0), 1, 8, 0);
        #else
        pts = (const Point*)Mat(points).data;
        npts = points.size();
        points.push_back(Point(x, y));
        polylines(_dst, &pts, &npts, 1, false, Scalar(0,0,255), 2, 8, 0);
        #endif

        imshow("img", _dst);
    }

    /* user release left button */
    if (event == CV_EVENT_LBUTTONUP && drag) {
        _dst = _src.clone();

        #if METHOD == 1
        Point temp(point);
        point.x = min(x, temp.x);
        x = max(x, temp.x);
        point.y = min(y, temp.y);
        y = max(y, temp.y);
        box = Rect(point.x, point.y, x - point.x, y - point.y);
        imgROI = _dst(box);
        medianBlur(imgROI, imgROI, kernel);
        medianBlur(imgROI, imgROI, kernel);
        #else
        box = boundingRect(points);
        Mat mask = Mat::ones(_src.size(), CV_8UC1);
        vector< vector<Point> > contour;
        contour.push_back(points);
        fillPoly(mask, contour, Scalar(0));
        medianBlur(_src(box), _dst(box), 51);
        _src(box).copyTo(_dst(box), mask(box));
        #endif

        imshow("img", _dst);
        drag = 0;
    }

    /* user click right button: reset all */
    if (event == CV_EVENT_RBUTTONUP) {
        imshow("img", _src);
        box.width = box.height = 0;
        drag = 0;
    }
}

int main (int argc, char *argv[])
{
    if (argc < 2)
        _src = imread(IMG, 1);
    else
        _src = imread(argv[1], 1);

    unsigned char *key;
    if (argc < 3)
        key = KEY;

    namedWindow("img", 1);

    setMouseCallback("img", mouseHandler, NULL);
    imshow("img", _src);
    char c = waitKey(0);

    destroyWindow("img");

    if (c == 's' && box.area() > 0) {
        /* output blurred image */
        imwrite("result.jpg", _dst);

        short x, y, w, h;
        x = box.x;
        y = box.y;
        w = box.width;
        h = box.height;
        // cout << x << " " << y << " " << w << " " << h << endl;

        /* init AES with key and set buffer */
        AES aes(key);
        imgROI = _src(box);
        int size = ((3*imgROI.cols*imgROI.rows+15)>>4)<<4;
        unsigned char *input = new unsigned char[size], *temp;
        memset(input, 0, size);
        for (int i = 0; i < imgROI.rows; i++)
            memcpy(input+i*3*imgROI.cols, imgROI.data+i*_dst.cols*3, 3*imgROI.cols);

        /* encrypt 128 bits each time until the whole imgROI is encryted */
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
