#include "displaygeometry.h"

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

DisplayGeometry::DisplayGeometry()
{
    scaleX=1;
    scaleY=1;
    offsetX=0;
    offsetY=0;
    relativeWidth=478;
    relativeHeight=478;
    absoluteWidth=478;
    absoluteHeight=478;
    sourceTargetX=absoluteWidth/2;
    sourceTargetY=absoluteHeight/2;
    absoluteTargetX=absoluteWidth/2;
    absoluteTargetY=absoluteHeight/2;
    relativeTargetX=absoluteTargetX*scaleX;
    relativeTargetY=absoluteTargetY*scaleY;
}

void DisplayGeometry::CropToRectangle(Mat source, Mat *destination)
{ // crop the source image to a rectangle that will be later scaled to the image window control rectangle
    cv::Rect roi;
    if(offsetX>=0)
    {
        roi.x = offsetX;
        roi.width = absoluteWidth;
    }
    else
    {
        roi.x = 0;
        roi.width=sourceWidth;
    }
    if(offsetY>=0)
    {
        roi.y = offsetY;
        roi.height = absoluteHeight;
    }
    else
    {
        roi.y = 0;
        roi.height = sourceHeight;

    }
    *destination = source(roi);
}

void DisplayGeometry::Resize(Mat source, Mat *destination)
{ // resize the image to the dimensions of the image window control
    cv::Size mSize(relativeWidth,relativeHeight);
    if(offsetY<0)
        mSize.height=sourceHeight*scaleY;
    else
        if(offsetX<0)
            mSize.width=sourceWidth*scaleX;
    cv::resize( source, *destination, mSize);
}

void DisplayGeometry::ResizeToFit()
{
    if(offsetX<0) //offsetX=sourceTargetX-absoluteWidth/2;
    {
        scaleX *= absoluteTargetX/sourceTargetX*1.005;
        scaleY = scaleX;
        RecalculateTarget();
        RecalculateStar();
    }
    if(offsetY<0) //offsetX=sourceTargetX-absoluteWidth/2;
    {
        scaleY *= absoluteTargetY/sourceTargetY*1.005;
        scaleX = scaleY;
        RecalculateTarget();
        RecalculateStar();
    }
    if( (offsetX + absoluteWidth) > sourceWidth )
    {
        scaleX*=absoluteTargetX/(sourceWidth-sourceTargetX)*1.005;
        scaleY = scaleX;
        RecalculateTarget();
        RecalculateStar();
    }
    if( (offsetY + absoluteHeight) > sourceHeight )
    {
        scaleY*=absoluteTargetY/(sourceHeight-sourceTargetY)*1.005;
        scaleX = scaleY;
        RecalculateTarget();
        RecalculateStar();
    }
    absoluteWidth=relativeWidth/scaleX;
    absoluteHeight=relativeHeight/scaleY;
    offsetX=sourceTargetX-absoluteWidth/2;
    offsetY=sourceTargetY-absoluteHeight/2;

}

void DisplayGeometry::init(double sourceImageWidth, double sourceImageHeight) // calculate image geometry starting from known source dimensions
{
    sourceWidth=sourceImageWidth;
    sourceHeight=sourceImageHeight;
    sourceTargetX=sourceWidth/2;
    sourceTargetY=sourceHeight/2;

    if(sourceWidth==sourceHeight)
    {
        scaleX=relativeWidth/sourceWidth;
        scaleY=scaleX;
        offsetX=0;
        offsetY=0;
        absoluteWidth=sourceWidth;
        absoluteHeight=sourceHeight;
    }
    if(sourceWidth > sourceHeight)
    {
        scaleX=relativeWidth/sourceWidth;
        scaleY=scaleX;
        absoluteWidth=relativeWidth/scaleX;
        absoluteHeight=relativeHeight/scaleY;
        offsetX=0;
        #ifdef IMAGELABELSVERTIACALALIGNMENTMIDDLE
            offsetY=(sourceHeight-absoluteHeight)/2;
        #elif
            offsetY=0;
        #endif
    }
    if(sourceWidth < sourceHeight)
    {
        scaleY=relativeHeight/sourceHeight;
        scaleY=scaleX;
        absoluteWidth=relativeWidth/scaleX;
        absoluteHeight=relativeHeight/scaleY;
        offsetY=0;
        offsetX=(sourceWidth-absoluteWidth)/2;
    }
    RecalculateTarget();
}

void DisplayGeometry::ZoomAbsolute(float zoomValue)
{
    scaleX=zoomValue;
    scaleY=zoomValue;
    ZoomCalculations();
}

void DisplayGeometry::Zoom(float coefficient)
{
    scaleX*=coefficient;
    scaleY*=coefficient;
    ZoomCalculations();
}

void DisplayGeometry::ZoomCalculations()
{
    offsetX+=(absoluteWidth-relativeWidth/scaleX)/2; // half of the absolute width difference after zooming
    offsetY+=(absoluteHeight-relativeHeight/scaleX)/2; // half of the absolute height difference after zooming
    absoluteWidth=relativeWidth/scaleX;
    absoluteHeight=relativeHeight/scaleY;
    RecalculateTarget();
    RecalculateStar();
}

void DisplayGeometry::RecalculateTarget()
{
    absoluteTargetX=sourceTargetX-offsetX;
    absoluteTargetY=sourceTargetY-offsetY;
    relativeTargetX=absoluteTargetX*scaleX;
    relativeTargetY=absoluteTargetY*scaleY;
}

void DisplayGeometry::RecalculateStar()
{
    absoluteStarX=sourceStarX-offsetX;
    absoluteStarY=sourceStarY-offsetY;
    relativeStarX=absoluteStarX*scaleX;
    relativeStarY=absoluteStarY*scaleY;}

void DisplayGeometry::RecalculateImage()
{

}

void DisplayGeometry::RecalculateStarFromAbsolutePosition()
{
    relativeStarX=absoluteStarX*scaleX;
    relativeStarY=absoluteStarY*scaleY;
    sourceStarX=absoluteStarX+offsetX;
    sourceStarY=absoluteStarY+offsetY;
}

void DisplayGeometry::CropAroundSelection(Mat source, Mat *destination, Mat *intermediary, int relativeX, int relativeY, bool changesourceTarget)
{
    sourceWidth = source.cols;
    sourceHeight = source.rows;
    if(changesourceTarget)
    {
        relativeTargetX=relativeX;
        absoluteTargetX=relativeTargetX/scaleX;
        sourceTargetX=absoluteTargetX+offsetX;
        relativeTargetY=relativeY;
        absoluteTargetY=relativeTargetY/scaleY;
        sourceTargetY=absoluteTargetY+offsetY;
    }
#ifdef DEBUG
    DisplayGeometryData("CropAroundSelection-old offsets");
#endif
// New offset values (center window around target)
    offsetX=sourceTargetX-absoluteWidth/2;
    offsetY=sourceTargetY-absoluteHeight/2;
    RecalculateTarget();
    RecalculateStar();
#ifdef DEBUG
    DisplayGeometryData("CropAroundSelection-new offsets");
#endif
    ResizeToFit();
#ifdef DEBUG
    DisplayGeometryData("CropAroundSelection-resized");
#endif
    CropToRectangle(source, intermediary);
    Resize(*intermediary, destination);
}

void DisplayGeometry::CropResize(Mat source, Mat *destination, Mat *intermediary)
{
    sourceWidth = source.cols;
    sourceHeight = source.rows;
    CropToRectangle(source, intermediary); // intermediary file has the same scale factor as the source
                                           // file, it is just cropped to a rectangle
    Resize(*intermediary, destination); // resized rectangle to be shown in the image window
}

#ifdef DEBUG
void DisplayGeometry::DisplayGeometryData(std::string message)
{
//    return;
    cout << endl;
    cout << message << endl;
    cout << "Geometry Data:" << endl;
    cout << "SourceTarget X: " << sourceTargetX << endl;
    cout << "SourceTarget Y: " << sourceTargetY << endl;
    cout << "AbsoluteTarget X: " << absoluteTargetX << endl;
    cout << "AbsoluteTarget Y: " << absoluteTargetY << endl;
    cout << "RelativeTarget X: " << relativeTargetX << endl;
    cout << "RelativeTarget Y: " << relativeTargetY << endl;
    cout << "Offset X: " << offsetX << endl;
    cout << "Offset Y: " << offsetY << endl;
    cout << "Scale X: " << scaleX << endl;
    cout << "Scale Y: " << scaleY << endl;
    cout << "Source Width: " << sourceWidth << endl;
    cout << "Source Height: " << sourceHeight << endl;
    cout << "Absolute Width: " << absoluteWidth << endl;
    cout << "Absolute Height: " << absoluteHeight << endl;
    cout << "Relative Width: " << relativeWidth << endl;
    cout << "Relative Height: " << relativeHeight << endl;
    cout << "Calculated X space: " << offsetX+absoluteWidth << endl;
    cout << "Calculated Y space: " << offsetY+absoluteHeight << endl;

}
#endif
