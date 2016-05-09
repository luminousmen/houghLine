#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>

void houghLine(IplImage* original, float accuracy=0.1) {
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

    int RMax = cvRound( sqrt( (double)(src->width * src->width + src->height * src->height) ) );
    phase = cvCreateImage(cvSize(RMax, 180), IPL_DEPTH_16U, 1);
    cvZero(phase);

    int x = 0, y = 0, r = 0, f = 0;
    float theta = 0;

    for(y = 0; y < bin->height; y++){
        uchar* ptr = (uchar*) (bin->imageData + y * bin->widthStep);
        for(x = 0; x < bin->width; x++){
            if(ptr[x] > 0) { // это пиксель контура?
                for(f = 0; f < 180; f++) { //перебираем все возможные углы наклона
                    short* ptrP = (short*) (phase->imageData + f * phase->widthStep);
                    for(r = 0; r < RMax; r++){ // перебираем все возможные расстояния от начала координат
                        theta = f * CV_PI / 180.0; // переводим градусы в радианы

                        if ( abs(( (y)*sin(theta) + (x)*cos(theta)) - r) < accuracy ) {
                            ptrP[r]++;
                        }
                    }
                }
            }
        }
    }

    cvNamedWindow( "phase", 1 );
    cvShowImage( "phase", phase );

    IplImage* phaseImage = cvCreateImage( cvSize(phase->width * 3, phase->height * 3), IPL_DEPTH_16U, 1);
    cvResize(phase, phaseImage);

    cvNamedWindow( "phaseImage", 1 );
    cvShowImage( "phaseImage", phaseImage);

    unsigned int MaxPhaseValue = 0;
    unsigned int limit = 300;

    std::vector<float> thetas;
    std::vector<int> rs;
    
    for(f = 0; f < 180; f++){ //перебираем все возможные углы наклона
        short* ptrP = (short*) (phase->imageData + f * phase->widthStep);
        for(r = 0; r < RMax; r++) { // перебираем все возможные расстояния от начала координат
            if(ptrP[r] > limit) {
                MaxPhaseValue = ptrP[r];

                std::cout << Theta << std::endl;
                std::cout << R << std::endl;

                thetas.push_back(f);
                rs.push_back(r);
            }
        }
    }
    
    for(y = 0; y < rgb->height; y++) {
        uchar* ptr = (uchar*) (rgb->imageData + y * rgb->widthStep);
        for(x = 0; x < rgb->width; x++){

            for(int i = 0; i < thetas.size(); i++) {
                Theta = thetas[i] * CV_PI / 180.0;

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
    cvReleaseImage(&phaseImage);
}


int main(int argc, char* argv[]) {

        IplImage *original=0;

        char* filename = argv[1];
        original = cvLoadImage(filename, 0);

        printf("[i] image: %s\n", filename);
        assert( original != 0 );

        cvNamedWindow( "original", 1 );
        cvShowImage( "original", original );

        houghLine(original);

        cvWaitKey(0);

        cvReleaseImage(&original);
        cvDestroyAllWindows();
        return 0;
}