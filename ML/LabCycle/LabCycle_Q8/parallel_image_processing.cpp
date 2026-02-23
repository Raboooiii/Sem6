#include <iostream>
#include <opencv2/opencv.hpp>
#include <omp.h>

using namespace cv;
using namespace std;

int main() {

    // Load Image
    Mat image = imread("input.jpg");

    if (image.empty()) {
        cout << "Error: Could not load image!" << endl;
        return -1;
    }

    cout << "Image Loaded Successfully!" << endl;

    Mat blur_serial = image.clone();
    Mat blur_parallel = image.clone();
    Mat sharpen_parallel = image.clone();
    Mat gray, edges;

    // -----------------------------------
    // BLUR FILTER (3x3 Average Kernel)
    // -----------------------------------

    float blur_kernel[3][3] = {
        {1/9.0, 1/9.0, 1/9.0},
        {1/9.0, 1/9.0, 1/9.0},
        {1/9.0, 1/9.0, 1/9.0}
    };

    // SERIAL BLUR
    double start = omp_get_wtime();

    for (int i = 1; i < image.rows - 1; i++) {
        for (int j = 1; j < image.cols - 1; j++) {
            for (int c = 0; c < 3; c++) {
                float sum = 0;
                for (int k = -1; k <= 1; k++) {
                    for (int l = -1; l <= 1; l++) {
                        sum += image.at<Vec3b>(i + k, j + l)[c] * blur_kernel[k + 1][l + 1];
                    }
                }
                blur_serial.at<Vec3b>(i, j)[c] = saturate_cast<uchar>(sum);
            }
        }
    }

    double serial_time = omp_get_wtime() - start;
    cout << "Serial Blur Time: " << serial_time << " seconds" << endl;

    // PARALLEL BLUR
    start = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 1; i < image.rows - 1; i++) {
        for (int j = 1; j < image.cols - 1; j++) {
            for (int c = 0; c < 3; c++) {
                float sum = 0;
                for (int k = -1; k <= 1; k++) {
                    for (int l = -1; l <= 1; l++) {
                        sum += image.at<Vec3b>(i + k, j + l)[c] * blur_kernel[k + 1][l + 1];
                    }
                }
                blur_parallel.at<Vec3b>(i, j)[c] = saturate_cast<uchar>(sum);
            }
        }
    }

    double parallel_time = omp_get_wtime() - start;
    cout << "Parallel Blur Time: " << parallel_time << " seconds" << endl;
    cout << "Speedup: " << serial_time / parallel_time << endl;

    // -----------------------------------
    // SHARPEN FILTER
    // -----------------------------------

    float sharpen_kernel[3][3] = {
        {0,-1,0},
        {-1,5,-1},
        {0,-1,0}
    };

    #pragma omp parallel for
    for (int i = 1; i < image.rows - 1; i++) {
        for (int j = 1; j < image.cols - 1; j++) {
            for (int c = 0; c < 3; c++) {
                float sum = 0;
                for (int k = -1; k <= 1; k++) {
                    for (int l = -1; l <= 1; l++) {
                        sum += image.at<Vec3b>(i + k, j + l)[c] * sharpen_kernel[k + 1][l + 1];
                    }
                }
                sharpen_parallel.at<Vec3b>(i, j)[c] = saturate_cast<uchar>(sum);
            }
        }
    }

    cout << "Sharpening completed." << endl;

    // -----------------------------------
    // EDGE DETECTION (SOBEL)
    // -----------------------------------

    cvtColor(image, gray, COLOR_BGR2GRAY);
    edges = gray.clone();

    int sobel_x[3][3] = {
        {-1,0,1},
        {-2,0,2},
        {-1,0,1}
    };

    #pragma omp parallel for
    for (int i = 1; i < gray.rows - 1; i++) {
        for (int j = 1; j < gray.cols - 1; j++) {

            int gx = 0;

            for (int k = -1; k <= 1; k++) {
                for (int l = -1; l <= 1; l++) {
                    gx += gray.at<uchar>(i + k, j + l) * sobel_x[k + 1][l + 1];
                }
            }

            edges.at<uchar>(i, j) = saturate_cast<uchar>(abs(gx));
        }
    }

    cout << "Edge Detection completed." << endl;

    // -----------------------------------
    // SAVE RESULTS
    // -----------------------------------

    imwrite("blur_serial.jpg", blur_serial);
    imwrite("blur_parallel.jpg", blur_parallel);
    imwrite("sharpen.jpg", sharpen_parallel);
    imwrite("edges.jpg", edges);

    cout << "All images saved successfully!" << endl;

    return 0;
}