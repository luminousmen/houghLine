#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>

#define ACCURANCY 0.1

void houghLine(IplImage* original, int limit);
IplImage* create_phase(IplImage* src, IplImage* bin, int &RMax);

int main(int argc, char* argv[]) {

    IplImage *original=0;

    char* filename = argv[1];
    int limit = atoi(argv[2]);
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
    assert(original != 0);

    IplImage *src = 0, *rgb = 0;
    IplImage *bin = 0;
    IplImage *phase = 0;

    src = cvCloneImage(original);

    rgb = cvCreateImage(cvGetSize(src), 8, 3);
    cvConvertImage(src, rgb, CV_GRAY2BGR);

    bin = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    cvCanny(src, bin, 50, 200);

    cvNamedWindow( "bin", 1 );
    cvShowImage( "bin", bin );

    int RMax;
    phase = create_phase(src, bin, RMax);

    unsigned int MaxPhaseValue = 0;

    std::vector<float> thetas;
    std::vector<int> rs;
    
    for(int f = 0; f < 180; f++){ //перебираем все возможные углы наклона
        short* ptrP = (short*) (phase->imageData + f * phase->widthStep);
        for(int r = 0; r < RMax; r++) { // перебираем все возможные расстояния от начала координат
            if(ptrP[r] > limit) {
                MaxPhaseValue = ptrP[r];

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

    RMax = cvRound( sqrt( (double)(src->width * src->width + src->height * src->height) ) );
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

    cvNamedWindow( "phase", 1 );
    cvShowImage( "phase", phase );
    return phase;
}