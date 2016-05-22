#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <getopt.h>

#define ACCURANCY 0.1

void houghLine(cv::Mat original, int limit);
cv::Mat create_phase(cv::Mat src, cv::Mat bin, int RMax);
void usage();


int main(int argc, char* argv[]) {

    int ch;
    int iflag = 0, 
        lflag = 0;
    int limit;
    char* filename;
    char * endptr;

    while ( (ch = getopt(argc, argv, "i:l:")) != -1) {
        switch (ch) {
        case 'i':
            iflag = 1;
            filename = optarg;
            break;
        case 'l':
            lflag = 1;
            limit = atoi(optarg);
            break;
        default:
            usage();
            return 0;
        }
    }

    // required arguments
    if(!lflag || !iflag){
        usage();
        return 0;
    }

    cv::Mat original = cv::imread(filename);
    std::cout << "Original image: " << filename << std::endl;

    houghLine(original, limit);

    cv::waitKey();
    return 0;
}


void houghLine(cv::Mat original, int limit) {

    cv::Mat src, rgb, bin;
    src = original.clone();

    rgb = cv::Mat(src.size(), IPL_DEPTH_8U, 3);

    cv::cvtColor(src, rgb, cv::COLOR_BGR2GRAY);

    bin = cv::Mat(src.size(), IPL_DEPTH_8U, 1);
    // find contour with Canny algorithm
    cv::Canny(src, bin, 50, 200);

    // max distance from coords origin
    int RMax = cvRound( sqrt( (double)(src.cols * src.cols + src.rows * src.rows) ) );
    cv::Mat phase = create_phase(src, bin, RMax);

    std::vector<float> thetas;
    std::vector<int> rs;
    
    for(int f = 0; f < 180; f++){
        short* ptrP = (short*) (phase.data + f * phase.step);
        // by all distancies
        for(int r = 0; r < RMax; r+=1) {
            if(ptrP[r] >= limit) {
                thetas.push_back(f);
                rs.push_back(r);
            }
        }
    }
    
    // draw all found lines
    for(int y = 0; y < rgb.rows; y++) {
        uchar* ptr = (uchar*) (rgb.data + y * rgb.step);
        for(int x = 0; x < rgb.cols; x++){

            for(int i = 0; i < thetas.size(); i++) {
                float Theta = thetas[i] * CV_PI / 180.0;

                if ( cvRound(((y) * sin(Theta) + (x) * cos(Theta))) == rs[i]) {
                    ptr[x] = 0;
                    ptr[x + 1] = 255;
                    ptr[x + 2] = 0;
                }
            }
        }
    }
    cv::namedWindow( "PHASE", cv::WINDOW_NORMAL);
    cv::imshow( "PHASE", phase );

    cv::namedWindow( "CONTOUR", cv::WINDOW_NORMAL);
    cv::imshow( "CONTOUR", bin );
    
    cv::namedWindow( "FOUND LINES", cv::WINDOW_NORMAL);
    cv::imshow( "FOUND LINES", rgb );
    cv::waitKey(0);
}


cv::Mat create_phase(cv::Mat src, cv::Mat bin, int RMax){
    
    int x = 0, y = 0, r = 0, f = 0;
    cv::Mat phase = cv::Mat(cv::Size(RMax, 180), IPL_DEPTH_16U, 1);

    for(y = 0; y < bin.rows; y++){
        uchar* ptr = (uchar*) (bin.data + y * bin.step);
        for(x = 0; x < bin.cols; x++){
            // is it contour?
            if(ptr[x] > 0) {
                for(int f = 0; f < 180; f++) {
                    short* ptrP = (short*) (phase.data + f * phase.step);
                    // by all distancies
                    for(int r = 0; r < RMax; r+=1) {
                        // grads to rads
                        float theta = f * CV_PI / 180.0; 

                        // math is exact but life is full of accidents
                        if ( abs(( (y)*sin(theta) + (x)*cos(theta)) - r) < ACCURANCY ) {
                            // point in Hough space
                            ptrP[r]++;
                        }
                    }
                }
            }
        }
    }

    return phase;
}

void usage(){
    std::cout << "Usage: [options]\n\n"
        "Options:\n"
        "\t-i  - image path\n"
        "\t-l  - limit\n" << std::endl;
}