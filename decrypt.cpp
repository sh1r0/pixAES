#include <cstdio>
#include <cstring>
#include <opencv2/opencv.hpp>
#include "AES.h"
#include "md5.h"
#include "pwd.h"

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    const char *IMG = (argc < 2) ? "result.jpg" : argv[1];
    FILE *f = fopen(IMG, "rb");
    if (!f) {
        perror("Error");
        return -1;
    }

    cout << "Please enter your password for decryption!!" << endl << "password: ";
    string pwd = getPassword();
    MD5 md5(pwd);
    unsigned char *key = md5.getDigest();

    AES aes(key);

    // get encrypted info
    unsigned char info[16] = "";
    fseek(f, -16, SEEK_END);
    fread(info, 1, 16, f);

    // verify key and info
    aes.InvCipher(info);
    if (memcmp(info+8, "pixAES", 6)) {
        fprintf(stderr, "Incorrect password or there is no secret in the image\n");
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
    Mat _src = imread(IMG, 1);
    Mat dst = _src(Rect(x, y, w, h));
    for (int i = 0; i < dst.rows; i++)
        memcpy(dst.data+i*_src.cols*3, input+i*3*dst.cols, 3*dst.cols);
    imwrite("recover.jpg", _src);

    cout << "Success!!" << endl;

    return 0;
}
