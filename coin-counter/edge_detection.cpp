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
        input_filename = argv[1];  // uses the first given argument as file path for execution
    }

    Mat img = imread(input_filename);
    FileStorage file("files/matrix.txt", cv::FileStorage::WRITE);

    /* scale the image down by factor RESIZE_CONSTANT declared in the lib */
    resize(img, img, Size(img.cols * RESIZE_CONSTANT, img.rows * RESIZE_CONSTANT), 0, 0, INTER_NEAREST_EXACT);

    Mat img_gray;
    cvtColor(img, img_gray, COLOR_BGR2GRAY);  // converts to grayscale
    Mat img_blur;
    GaussianBlur(img_gray, img_blur, Size(7, 7), 0);  // blurs the image, to avoid anomalies

    Mat canny;
    Canny(img_blur, canny, 75, 150, 3, false);  // applying Cany edge detection

    file << "M" << canny;  // print edge detected matrix on txt file

    imwrite(output_filename, canny);  // produce edge detected image

    return 0;
}
