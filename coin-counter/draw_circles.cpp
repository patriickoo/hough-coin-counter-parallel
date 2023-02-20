#include <opencv2/opencv.hpp>
#include <iostream>
#include "hcc_lib.h"

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    char *input_filename = "files/placeholder.jpg";

    if (argc > 1) {
        input_filename = argv[1];  // this should be the same as the input of edge detection
    }

    Mat img = imread(input_filename);

    FILE *f;
    f = fopen("files/circle_coordinates.csv", "r");
    int x, y, radius;

    fscanf(f, "%*s %*s %*s");  // skip the title row

    while (fscanf(f, "%d %d %d", &x, &y, &radius) == 3) {  // continue as long as there are rows in the file

        printf("drawing x(%d) y(%d) rad(%d)\n", x, y, radius);
        Point p = Point(x / RESIZE_CONSTANT, y / RESIZE_CONSTANT);
        circle(img, p, 1, Scalar(0, 255, 0), 12, LINE_AA);  // draw the centre of the circle
        circle(img, p, radius / RESIZE_CONSTANT, Scalar(255, 0, 0), 2, LINE_AA);  // draw the circumnference

    }

    imshow("circles", img);  // show the image with circles drawn as popup
    waitKey();  // wait for any key press to exit

    return 0;
}
