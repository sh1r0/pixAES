#include <cstdio>
#include <cstring>
#include <opencv2/opencv.hpp>
#include "AES.h"

using namespace std;
using namespace cv;

#define filename "Stickies.jpg"

unsigned char key[] = {
    0x2b, 0x7e, 0x15, 0x16,
    0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88,
    0x09, 0xcf, 0x4f, 0x3c
};

int main()
{
    AES aes(key);
    FILE *f = fopen("result.jpg", "rb");
    if (!f) {
        perror("The encryted file dose not exist!!");
        return -1;
    }

    // get encrypted info
    unsigned char info[16] = "";
    fseek(f, -16, SEEK_END);
    fread(info, 1, 16, f);

    // verify key and info
    aes.InvCipher(info);
    if (memcmp(info+8, "pixAES", 6)) {
        fprintf(stderr, "The file is not encrypted or the key is wrong\n");
        return -1;
    }

    short x, y, w, h;
    memcpy(&x, info, sizeof(x));
    memcpy(&y, info+2, sizeof(y));
    memcpy(&w, info+4, sizeof(w));
    memcpy(&h, info+6, sizeof(h));

    int size = ((3*w*h+15)>>4)<<4;
    unsigned char *input = new unsigned char[size], *temp;
    fseek(f, -16-size, SEEK_END);
    fread(input, 1, size, f);
    fclose(f);

    // decryption
	for (int i = 0; i < size; i+=16) {
        temp = input+i;
        aes.InvCipher(temp);
    }

    // recovery
    Mat _src = imread("result.jpg", 1);
    Mat dst = _src(Rect(x, y, w, h));
    for (int i = 0; i < dst.rows; i++)
        memcpy(dst.data+i*_src.cols*3, input+i*3*dst.cols, 3*dst.cols);
    imwrite("recover.jpg", _src);

    return 0;
}
