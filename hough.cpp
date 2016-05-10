#include <cv.h>
#include <highgui.h>

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <getopt.h>

#define ACCURANCY 0.1

void houghLine(IplImage* original, int limit);
IplImage* create_phase(IplImage* src, IplImage* bin, int &RMax);
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

    IplImage *original = 0;
    original = cvLoadImage(filename, 0);

    std::cout << "Original image: " << filename << std::endl;
    assert( original != 0 );

    cvNamedWindow( "original", 1 );
    cvShowImage( "original", original );

    houghLine(original, limit);

    cvWaitKey(0);

    cvReleaseImage(&original);
    cvDestroyAllWindows();
    return 0;
}


void houghLine(IplImage* original, int limit) {

    IplImage *src = 0, *rgb = 0, *bin = 0, phase = 0;

    src = cvCloneImage(original);

    rgb = cvCreateImage(cvGetSize(src), 8, 3);
    cvConvertImage(src, rgb, CV_GRAY2BGR);

    bin = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    cvCanny(src, bin, 50, 200);

    cvNamedWindow( "bin", 1 );
    cvShowImage( "bin", bin );

    int RMax = cvRound( sqrt( (double)(src->width * src->width + src->height * src->height) ) );
    phase = create_phase(src, bin, RMax);

    std::vector<float> thetas;
    std::vector<int> rs;
    
    for(int f = 0; f < 180; f++){
        short* ptrP = (short*) (phase->imageData + f * phase->widthStep);
        for(int r = 0; r < RMax; r++) {
            if(ptrP[r] >= limit) {
                thetas.push_back(f);
                rs.push_back(r);
            }
        }
    }
    
    for(int y = 0; y < rgb->height; y++) {
        uchar* ptr = (uchar*) (rgb->imageData + y * rgb->widthStep);
        for(int x = 0; x < rgb->width; x++){

            for(int i = 0; i < thetas.size(); i++) {
                float Theta = thetas[i] * CV_PI / 180.0;

                if ( cvRound(((y) * sin(Theta) + (x) * cos(Theta))) == rs[i]) {
                    ptr[3 * x] = 0;
                    ptr[3 * x + 1] = 0;
                    ptr[3 * x + 2] = 255; 
                }
            }
        }
    }

    cvNamedWindow( "line", 1 );
    cvShowImage( "line", rgb );

    cvReleaseImage(&src);
    cvReleaseImage(&rgb);
    cvReleaseImage(&bin);
    cvReleaseImage(&phase);
}


IplImage* create_phase(IplImage* src, IplImage* bin, int &RMax){
    
    IplImage *phase = 0;

    phase = cvCreateImage(cvSize(RMax, 180), IPL_DEPTH_16U, 1);
    cvZero(phase);

    int x = 0, y = 0, r = 0, f = 0;
    float theta = 0;

    for(y = 0; y < bin->height; y++){
        uchar* ptr = (uchar*) (bin->imageData + y * bin->widthStep);
        for(x = 0; x < bin->width; x++){
            if(ptr[x] > 0) {
                for(int f = 0; f < 180; f++) {
                    short* ptrP = (short*) (phase->imageData + f * phase->widthStep);
                    for(int r = 0; r < RMax; r++) {
                        theta = f * CV_PI / 180.0; 

                        if ( abs(( (y)*sin(theta) + (x)*cos(theta)) - r) < ACCURANCY ) {
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