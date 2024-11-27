#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <iostream>
#include <cmath>   // For std::abs
#include <numeric> // For std::accumulate
#include <algorithm> // For std::count_if
#include <cstdio>

static const int _512by512_IMG_SIZE = 262144;
static const int BMP_COLOUR_TABLE_SIZE = 1024;
static const int BMP_HEADER_SIZE = 54;
static const int MAX_COLOUR = 255;
static const int MIN_COLOUR = 0;
static const int COLOUR_WHITE = MAX_COLOUR;
static const int COLOUR_BLACK = MIN_COLOUR;
static const int N_GRAYLEVELS = 255;



class ImageProcessing {
    public:
        ImageProcessing();
        ImageProcessing(
            const char *_inImageName,
            const char *_outImageName,
            int *_height,
            int *_width,
            int *_bitDepth,
            unsigned char *_header,
            unsigned char *_colourTable,
            unsigned char *_inBuffer,
            unsigned char *_outBuffer);

        virtual ~ImageProcessing();

        void readImage();
        void writeImage();
        void copyImageData(unsigned char *_sourceBuffer, unsigned char *_destinationBuffer, int bufferSize);

        int countPixelsAboveThreshold(unsigned char *data, int size, float threshold);
        float calculateGroupMean(unsigned char *data, int size, float threshold, bool aboveThreshold, int groupCount);
        void binarizeImageData(unsigned char *_inputImageData, unsigned char *_outImageData, int imageSize, int threshold);
        void iterativelyBinarizeImageData(unsigned char *_inputImageData, unsigned char *_outImageData, int imageSize);

        void increaseBrightness(unsigned char *_inputImageData, unsigned char *_outImageData, int imageSize, int brightness);
        void decreaseBrightness(unsigned char *_inputImageData, unsigned char *_outImageData, int imageSize, int brightness);

        void computeImageHistogram(unsigned char *_imageData, int imageRows, int imageColumns, float hist[]);
        void computeImageHistogramFile(unsigned char * _imageData, int imageRows, int imageColumns, float hist[],const char *histFile);
        void equalizeImageHistogram(unsigned char *_inputImageData, unsigned char *_outImageData, int imageRows, int imageColumns);

        void inverseGreyLevel(unsigned char *_inputImageData, unsigned char *_outImageData, int imageWidth, int imageHeight);
        void rotateImage(unsigned char *_inputImageData, unsigned char *_outputImageData, int *width, int *height);
        void scaleImage(unsigned char *_inputImageData, unsigned char *_outputImageData, int originalWidth, int originalHeight, int newWidth, int newHeight);

        void meanBlurFilter(unsigned char *_inputImageData, unsigned char *_outputImageData, int imageWidth, int imageHeight);

        void detectLines(unsigned char *_inputImageData, unsigned char *_outputImageData, int imageWidth, int imageHeight, int maskType);

    protected:

    private:
        const char *inImageName;
        const char *outImageName;
        int *height;
        int *width;
        int *bitDepth;
        unsigned char *header;
        unsigned char *colourTable;
        unsigned char *inBuffer;
        unsigned char *outBuffer;
};

#endif
