#ifndef CROPANDRESIZE_H
#define CROPANDRESIZE_H

#include <string>

#include "globalsettings.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class CropAndResize
{
public:
    CropAndResize();
    double sourceWidth;
    double sourceHeight;
    float scaleX, scaleY;
    double relativeTargetX, relativeTargetY; // mouse selection in image widget
    double relativeTargetScaledX, relativeTargetScaledY; // mouse selection in image widget
    double absoluteTargetX, absoluteTargetY; // relative to source
    double relativeStarX, relativeStarY; //
    double relativeStarScaledX, relativeStarScaledY; //
    double absoluteStarX, absoluteStarY; //
    double raDrift, declDrift;
    double raDriftScaled, declDriftScaled;
    double pi;
    double raSlope;
    double alpha;
    bool slopeVertical;
    int verticalDirection;
    int offsetX, offsetY; //relative to source
    int relativeWidth, relativeHeight;
    int absoluteWidth, absoluteHeight;
    void CropToRectangle(cv::Mat source, cv::Mat *destination); //crop to rectangle area
    void Resize(cv::Mat source, cv::Mat *destination);
    void ResizeToFit();
    void ZoomAbsolute(float zoomValue);
    void Zoom(float coefficient);
    void ZoomCalculations();
    void StarCalculations();
    void ComputeRaSlope();
    void ComputeDrift();
//    void ComputeDrift1();
    void SnapToClosestStar();
    void CropAroundSelection(cv::Mat source, cv::Mat *destination, cv::Mat *intermediary, int relativeX, int relativeY, bool changeAbsoluteTarget);
    void CropResize(cv::Mat source, cv::Mat *destination, cv::Mat *intermediary);
#ifdef DEBUG
    void DisplayGeometryData(std::string message);
#endif
};

#endif // CROPANDRESIZE_H
