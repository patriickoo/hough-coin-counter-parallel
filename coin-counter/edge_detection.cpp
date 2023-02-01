#include <iostream>
#include <opencv2/opencv.hpp>
#include "hcc_lib.h"

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    char *input_filename = "files/placeholder.jpg";
    char *output_filename = "files/edgedetected.jpg";

    if (argc > 1) {
        input_filename = argv[1];
    }

    Mat img = imread(input_filename);
    FileStorage file("files/matrix.txt", cv::FileStorage::WRITE);

/*
    double minVal;
    double maxVal;
    Point minLoc(0,0), maxLoc(0,0);
    minMaxLoc(img, &minVal, &maxVal);

    int low_stretch = 0;
    int up_stretch = 255;

    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            img.data[i * img.cols + j] = (img.data[i * img.cols + j] - minVal) * ((up_stretch - low_stretch) /
                                         (maxVal - minVal)) + low_stretch ;
        }
    }

    imshow("circles", img);
    waitKey();
*/

    resize(img, img, Size(img.cols * RESIZE_CONSTANT, img.rows * RESIZE_CONSTANT), 0, 0, INTER_NEAREST_EXACT);

    // Convert to graycsale
    Mat img_gray;
    cvtColor(img, img_gray, COLOR_BGR2GRAY);
    // Blur the image for better edge detection
    Mat img_blur;
    GaussianBlur(img_gray, img_blur, Size(7, 7), 0);
    //medianBlur(img_gray, img_blur ,7);
    
    // Canny edge detection
    Mat canny;
    Canny(img_blur, canny, 75, 150, 3, false);

    // Sobel edge detection
    // Mat sobel;
    // Sobel(img_blur, sobel, CV_64F, 1, 1, 5);

    file << "M" << canny;
    // file << "M" << sobel;

    imwrite(output_filename, canny);
    // imwrite(output_filename, sobel);

    return 0;
}
