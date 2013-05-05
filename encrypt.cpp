#include <cstdio>
#include <vector>
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

void imshowMany(const std::string& _winName, const vector<Mat>& _imgs)
{
    int nImg = (int)_imgs.size();

    Mat dispImg;

    if (nImg != 2) {
        printf("Number of arguments too small....\n");
        return;
    }

    // w - Maximum number of images in a row
    // h - Maximum number of images in a column
    int w = 2, h = 1;
    int width = _imgs[0].cols;
    int height = _imgs[0].rows;

    dispImg.create(Size(width*w, height*h), CV_8UC3);

    for (int i= 0, m=0, n=0; i<nImg; i++, m+=(width)) {
        Mat imgROI = dispImg(Rect(m, n, width, height));
        _imgs[i].copyTo(imgROI);
    }

    namedWindow(_winName);
    imshow(_winName, dispImg);
    waitKey(0);
}

int main ()
{
    Mat _src = imread(filename , 1);
    Mat _dst = _src.clone();
    short x = 247;
    short y = 121;
    short w = 105;
    short h = 48;
    Mat dst = _dst(Rect(x, y, w, h));

    // encryption
	AES aes(key);
	int size = ((3*dst.cols*dst.rows+15)>>4)<<4;
    unsigned char *input = new unsigned char[size], *temp;
    memset(input, 0, size);
    for (int i = 0; i < dst.rows; i++)
        memcpy(input+i*3*dst.cols, dst.data+i*_dst.cols*3, 3*dst.cols);

    for (int i = 0; i < size; i+=16) {
        temp = input+i;
        aes.Cipher(temp);
    }

    medianBlur(dst, dst, kernel);
    medianBlur(dst, dst, kernel);
/*
    dst = _dst(Rect(173, 247, 95, 48));
    medianBlur(dst, dst, 21);
    medianBlur(dst, dst, kernel);

    dst = _dst(Rect(49, 71, 182, 111));
    medianBlur(dst, dst, kernel);
    medianBlur(dst, dst, kernel);

    vector<Mat> img;
    img.push_back(_src);
    img.push_back(_dst);
    imshowMany("haha", img);
*/
    // output blurred image
    imwrite("result.jpg", _dst);
    
    // encrypt offset and size info
    unsigned char info[16] = "";
    memcpy(info, &x, sizeof(x));
    memcpy(info+2, &y, sizeof(y));
    memcpy(info+4, &w, sizeof(w));
    memcpy(info+6, &h, sizeof(h));
    memcpy(info+8, "pixAES", 6);
    aes.Cipher(info);
    
    // append encrypted block of image and encryted info to blurred image
    FILE *f = fopen("result.jpg", "ab");
    fwrite(input, 1, size, f);
    fwrite(info, 1, 16, f);
    fclose(f);

    return 0;
}
