#ifndef DISPLAYGEOMETRY_H
#define DISPLAYGEOMETRY_H

#include <string>

#include "globalsettings.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class DisplayGeometry
{
public:
    DisplayGeometry();
    double sourceWidth;
    double sourceHeight;
    float scaleX, scaleY;
    double absoluteTargetX, absoluteTargetY; // mouse selection in image widget
    double relativeTargetX, relativeTargetY; // mouse selection in image widget
    double sourceTargetX, sourceTargetY; // relative to source
    double absoluteStarX, absoluteStarY; //
    double relativeStarX, relativeStarY; //
    double sourceStarX, sourceStarY; //
    int offsetX, offsetY; //relative to source
    int relativeWidth, relativeHeight;
    int absoluteWidth, absoluteHeight;
    void CropToRectangle(cv::Mat source, cv::Mat *destination); //crop to rectangle area
    void Resize(cv::Mat source, cv::Mat *destination);
    void ResizeToFit();
    void init(double sourceImageWidth, double sourceImageHeight);
    void ZoomAbsolute(float zoomValue);
    void Zoom(float coefficient);
    void ZoomCalculations();
    void RecalculateTarget();
    void RecalculateStar();
    void RecalculateImage();
    void RecalculateStarFromAbsolutePosition();
    void CropAroundSelection(cv::Mat source, cv::Mat *destination, cv::Mat *intermediary, int relativeX, int relativeY, bool changesourceTarget);
    void CropResize(cv::Mat source, cv::Mat *destination, cv::Mat *intermediary);
#ifdef DEBUG
    void DisplayGeometryData(std::string message);
#endif
};

#endif // DISPLAYGEOMETRY_H
