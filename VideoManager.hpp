#ifndef VIDEOMANAGER_HPP
#define VIDEOMANAGER_HPP

#include <opencv2/opencv.hpp>

class VideoManager {
    private:
        cv::Mat currentFrame,
                redimentionedFrame;
        int rotation = 0,
            zoomOut = 0,
            gaussianKernelSize = 3,
            brightness = 0;
        float contrast = 1;
        bool maxZoom = false,
            mirrorH = false,
            mirrorV = false,
            greyScale = false,
            edgeDetection = false,
            negativeFilter = false,
            gradientFilter = false,
            gaussianFilter = false;

    public:
        // Frame getters + setters
        cv::Mat GetCurrentFrame();
        cv::Mat GetRedimensionedFrame();
        void SetFrame(cv::Mat frame);

        // Adjust size, reflection and rotation
        void MirrorHorizontal();
        void MirrorVertical();
        void AdjustRotation(int value);
        bool IsZoomInnactive();
        bool IsMaxZoomReached();
        void ZoomOut();
        void ZoomBackIn();

        // Adjust color and filters
        void AdjustBrightness(int value);
        void AdjustContrast(float value);
        void AlterGreyscale();
        void AlterNegativeFilter();
        void AlterEdgeDetection();
        void AlterGradientFilter();

        // Manage Gaussian filter application
        void ActivateGaussianFilter(int newKernelSize);
        void DeactivateGaussianFilter();

        // Reset changes to dafault values
        void Reset();

        // Apply current changes
        void UpdateFrame();
};

#endif