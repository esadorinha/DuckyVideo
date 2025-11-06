#include "VideoManager.hpp"
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;

// Frame getters + setters
cv::Mat VideoManager::GetCurrentFrame () {
    return currentFrame;
}

cv::Mat VideoManager::GetRedimensionedFrame () {
    return redimentionedFrame;
}

void VideoManager::SetFrame (cv::Mat frame) {
    currentFrame = frame;
}


// Adjust size, reflection and rotation
void VideoManager::MirrorHorizontal () {
    mirrorH = !mirrorH;
}

void VideoManager::MirrorVertical () {
    mirrorV = !mirrorV;
}

void VideoManager::AdjustRotation (int value) {
    rotation += value;
    if (rotation < 0) {rotation += 360;}
    if (rotation >= 360) {rotation -= 360;}
}

bool VideoManager::IsMaxZoomReached () {
    return maxZoom;
}

bool VideoManager::IsZoomInnactive () {
    return zoomOut == 0;
}

void VideoManager::ZoomOut () {
    zoomOut += 1;
}

void VideoManager::ZoomBackIn () {
    zoomOut -= 1;
}


// Adjust color and filters
void VideoManager::AdjustBrightness (int value) {
    brightness = value;
}

void VideoManager::AdjustContrast (float value) {
    contrast = value;
}

void VideoManager::AlterGreyscale () {
    greyScale = !greyScale;
}

void VideoManager::AlterNegativeFilter () {
    negativeFilter = !negativeFilter;
}

void VideoManager::AlterEdgeDetection () {
    edgeDetection = !edgeDetection;
}

void VideoManager::AlterGradientFilter () {
    gradientFilter = !gradientFilter;
}


// Manage Gaussian filter application
void VideoManager::ActivateGaussianFilter (int newKernelSize) {
    if (!gaussianFilter) {gaussianFilter = true;}
    gaussianKernelSize = newKernelSize;
}

void VideoManager::DeactivateGaussianFilter () {
    gaussianFilter = false;
}


// Reset changes to dafault values
void VideoManager::Reset () {
    rotation = 0,
    zoomOut = 0,
    brightness = 0;
    contrast = 1;
    maxZoom = false,
    greyScale = false,
    edgeDetection = false,
    negativeFilter = false,
    gaussianFilter = false,
    gradientFilter = false,
    mirrorH = false,
    mirrorV = false;
}


// Apply current changes
void VideoManager::UpdateFrame () {

    // Mirror
    if (mirrorH) {
        flip(currentFrame, currentFrame, 1);
    }
    if (mirrorV) {
        flip(currentFrame, currentFrame, 0);
    }

    // Apply current brightness and contrast values
    currentFrame.convertTo(currentFrame, -1, contrast, brightness);

    // Apply filters
    if (greyScale) {
        cvtColor(currentFrame, currentFrame, COLOR_BGR2GRAY);
        cvtColor(currentFrame, currentFrame, COLOR_GRAY2BGR);
    }
    if (negativeFilter) {
        currentFrame.convertTo(currentFrame, -1, -1, 255);
    }
    if (gaussianFilter) {
        GaussianBlur(currentFrame, currentFrame, Size(gaussianKernelSize, gaussianKernelSize), 0);
    }
    if (edgeDetection) {
        cvtColor(currentFrame, currentFrame, COLOR_BGR2GRAY);
        Canny(currentFrame, currentFrame, 50, 200);
        cvtColor(currentFrame, currentFrame, COLOR_GRAY2BGR);
    }
    if (gradientFilter) {
        Mat gradX, gradY;
        Sobel(currentFrame, gradX, CV_16S, 1, 0, 3);
        Sobel(currentFrame, gradY, CV_16S, 0, 1, 3);
        convertScaleAbs(gradX, gradX);
        convertScaleAbs(gradY, gradY);
        addWeighted(gradX, 0.5, gradY, 0.5, 0, currentFrame);
    }

    // Adjust rotation and dimensions
    redimentionedFrame = currentFrame;

    maxZoom = false;
    for (int i=0; i<zoomOut;i++) {
        resize(redimentionedFrame, redimentionedFrame, Size(), 0.5, 0.5);
        if (redimentionedFrame.rows/2 == 0 || redimentionedFrame.cols/2 == 0) {
            maxZoom = true;
            break;
        }
    }
    switch (rotation) {
    case 90:
        rotate(redimentionedFrame, redimentionedFrame, ROTATE_90_CLOCKWISE);
        break;
    case 180:
        rotate(redimentionedFrame, redimentionedFrame, ROTATE_180);
        break;
    case 270:
        rotate(redimentionedFrame, redimentionedFrame, ROTATE_90_COUNTERCLOCKWISE);
        break;
    default:
        break;
    }
}
