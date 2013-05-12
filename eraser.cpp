#include <cstdio>
#include <opencv2/opencv.hpp>
#define R 5

using namespace cv;

Mat img0, img1;

/* return i in [a, b] */
int bound(short i,short a,short b)
{
    return min(max(i,min(a,b)),max(a,b));
}

void mouseHandler(int event, int x, int y, int flags, void* param)
{
    x = bound(x, 0, img0.cols-1);
    y = bound(y, 0, img0.rows-1);

    /* user drag the mouse */
    if (event == CV_EVENT_MOUSEMOVE) {
        Rect box = Rect(max(0, x-R), max(0, y-R), min(R, x)+min(R, img0.cols-1-x)+1, min(R, y)+min(R, img0.rows-1-y)+1);
        Mat roi = img1(box);
        if (flags == CV_EVENT_FLAG_RBUTTON)
            img0(box).copyTo(roi);
        else if (flags == CV_EVENT_FLAG_LBUTTON)
            medianBlur(roi, roi, 51);
        imshow("img", img1);
    }

    if (event == CV_EVENT_LBUTTONDBLCLK) {
        img1 = img0.clone();
        imshow("img", img1);
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
        img0 = imread("lena.jpg", 1);
    else
        img0 = imread(argv[1], 1);

    namedWindow("img", 1);

    setMouseCallback("img", mouseHandler, NULL);
    img1 = img0.clone();
    imshow("img", img1);
    waitKey(0);

    destroyWindow("img");

    return 0;
}
