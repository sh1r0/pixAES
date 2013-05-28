#include <cstdio>
#include <opencv2/opencv.hpp>

#define R 10

using namespace cv;

Mat _src, _dst;
Mat mask;

/* return i in [a, b] */
int bound(short i,short a,short b)
{
    return min(max(i,min(a,b)),max(a,b));
}

void mouseHandler(int event, int x, int y, int flags, void* param)
{
    x = bound(x, 0, _src.cols-1);
    y = bound(y, 0, _src.rows-1);

    /* user drag the mouse */
    if (event == CV_EVENT_MOUSEMOVE || event == CV_EVENT_LBUTTONDOWN) {
        Rect box = Rect(max(0, x-R), max(0, y-R), min(R, x)+min(R, _src.cols-1-x)+1, min(R, y)+min(R, _src.rows-1-y)+1);
        Mat roi = _dst(box);
        if (flags == CV_EVENT_FLAG_RBUTTON)
            _src(box).copyTo(roi, mask);
        else if (flags == CV_EVENT_FLAG_LBUTTON) {
            Mat temp;
            medianBlur(roi, temp, 51);
            temp.copyTo(roi, mask);
        }
        imshow("img", _dst);
    }

    if (event == CV_EVENT_LBUTTONDBLCLK) {
        _dst = _src.clone();
        imshow("img", _dst);
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
        _src = imread("lena.jpg", 1);
    else
        _src = imread(argv[1], 1);

    mask = Mat::zeros(Size(1+R<<1, 1+R<<1), CV_8UC1);
    circle(mask, Point(R,R), R, Scalar(255), CV_FILLED, 8, 0);

    namedWindow("img", 1);

    setMouseCallback("img", mouseHandler, NULL);
    _dst = _src.clone();
    imshow("img", _dst);
    waitKey(0);

    destroyWindow("img");

    return 0;
}
