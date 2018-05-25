#include "cropandresize.h"

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

CropAndResize::CropAndResize()
{
//    sourceWidth=640;
//    sourceHeight=480;
    pi=3.14159265358979323846;
    scaleX=1;
    scaleY=1;
    offsetX=0;
    offsetY=0;
    relativeWidth=478;
    relativeHeight=478;
    absoluteWidth=478;
    absoluteHeight=478;
    absoluteTargetX=absoluteWidth/2;
    absoluteTargetY=absoluteHeight/2;
    relativeTargetX=relativeWidth/2;
    relativeTargetY=relativeHeight/2;
    relativeTargetScaledX=relativeTargetX*scaleX;
    relativeTargetScaledY=relativeTargetY*scaleY;
    slopeVertical=false;
    raSlope=0;
}

void CropAndResize::CropToRectangle(Mat source, Mat *destination)
{
    cv::Rect roi;
    roi.x = offsetX;
    roi.y = offsetY;
    roi.width = absoluteWidth;
    roi.height = absoluteHeight;
    *destination = source(roi);
}

void CropAndResize::Resize(Mat source, Mat *destination)
{
    cv::Size mSize(relativeWidth,relativeHeight);//the dst image size,e.g.100x100
    cv::resize( source, *destination, mSize);
}

void CropAndResize::ResizeToFit()
{
    if(offsetX<0) //offsetX=absoluteTargetX-absoluteWidth/2;
    {
        scaleX *= relativeTargetX/absoluteTargetX*1.005;
        scaleY = scaleX;
        absoluteWidth=relativeWidth/scaleX;
        absoluteHeight=relativeHeight/scaleY;
        offsetX=absoluteTargetX-absoluteWidth/2;
        offsetY=absoluteTargetY-absoluteHeight/2;
        relativeTargetX=absoluteTargetX-offsetX;
        relativeTargetY=absoluteTargetY-offsetY;
        relativeTargetScaledX=relativeTargetX*scaleX;
        relativeTargetScaledY=relativeTargetY*scaleY;
        relativeStarX=absoluteStarX-offsetX;
        relativeStarY=absoluteStarY-offsetY;
        relativeStarScaledX=relativeStarX*scaleX;
        relativeStarScaledY=relativeStarY*scaleY;
    }
    if(offsetY<0) //offsetX=absoluteTargetX-absoluteWidth/2;
    {
        scaleY *= relativeTargetY/absoluteTargetY*1.005;
        scaleX = scaleY;
        absoluteWidth=relativeWidth/scaleX;
        absoluteHeight=relativeHeight/scaleY;
        offsetX=absoluteTargetX-absoluteWidth/2;
        offsetY=absoluteTargetY-absoluteHeight/2;
        relativeTargetX=absoluteTargetX-offsetX;
        relativeTargetY=absoluteTargetY-offsetY;
        relativeTargetScaledX=relativeTargetX*scaleX;
        relativeTargetScaledY=relativeTargetY*scaleY;
        relativeStarX=absoluteStarX-offsetX;
        relativeStarY=absoluteStarY-offsetY;
        relativeStarScaledX=relativeStarX*scaleX;
        relativeStarScaledY=relativeStarY*scaleY;
    }
    if( (offsetX + absoluteWidth) > sourceWidth )
    {
        scaleX*=relativeTargetX/(sourceWidth-absoluteTargetX)*1.005;
        scaleY = scaleX;
        absoluteWidth=relativeWidth/scaleX;
        absoluteHeight=relativeHeight/scaleY;
        offsetX=absoluteTargetX-absoluteWidth/2;
        offsetY=absoluteTargetY-absoluteHeight/2;
        relativeTargetX=absoluteTargetX-offsetX;
        relativeTargetY=absoluteTargetY-offsetY;
        relativeTargetScaledX=relativeTargetX*scaleX;
        relativeTargetScaledY=relativeTargetY*scaleY;
        relativeStarX=absoluteStarX-offsetX;
        relativeStarY=absoluteStarY-offsetY;
        relativeStarScaledX=relativeStarX*scaleX;
        relativeStarScaledY=relativeStarY*scaleY;
    }
    if( (offsetY + absoluteHeight) > sourceHeight )
    {
        scaleY*=relativeTargetY/(sourceHeight-absoluteTargetY)*1.005;
        scaleX = scaleY;
        absoluteWidth=relativeWidth/scaleX;
        absoluteHeight=relativeHeight/scaleY;
        offsetX=absoluteTargetX-absoluteWidth/2;
        offsetY=absoluteTargetY-absoluteHeight/2;
        relativeTargetX=absoluteTargetX-offsetX;
        relativeTargetY=absoluteTargetY-offsetY;
        relativeTargetScaledX=relativeTargetX*scaleX;
        relativeTargetScaledY=relativeTargetY*scaleY;
        relativeStarX=absoluteStarX-offsetX;
        relativeStarY=absoluteStarY-offsetY;
        relativeStarScaledX=relativeStarX*scaleX;
        relativeStarScaledY=relativeStarY*scaleY;
    }
}

void CropAndResize::ZoomAbsolute(float zoomValue)
{
    scaleX=zoomValue;
    scaleY=zoomValue;
    ZoomCalculations();
}

void CropAndResize::Zoom(float coefficient)
{
    scaleX*=coefficient;
    scaleY*=coefficient;
    ZoomCalculations();
}

void CropAndResize::ZoomCalculations()
{
    offsetX=(absoluteWidth-relativeWidth/scaleX)/2;
    offsetY=(absoluteHeight-relativeHeight/scaleX)/2;
    absoluteWidth=relativeWidth/scaleX;
    absoluteHeight=relativeHeight/scaleY;
    relativeTargetX=absoluteTargetX-offsetX;
    relativeTargetY=absoluteTargetY-offsetY;
    relativeTargetScaledX=relativeTargetX*scaleX;
    relativeTargetScaledY=relativeTargetY*scaleY;
    relativeStarX=absoluteStarX-offsetX;
    relativeStarY=absoluteStarY-offsetY;
    relativeStarScaledX=relativeStarX*scaleX;
    relativeStarScaledY=relativeStarY*scaleY;
}

void CropAndResize::StarCalculations()
{
    relativeStarScaledX=relativeStarX*scaleX;
    relativeStarScaledY=relativeStarY*scaleY;
    absoluteStarX=relativeStarX+offsetX;
    absoluteStarY=relativeStarY+offsetY;
}

void CropAndResize::ComputeRaSlope()
{
    if(relativeTargetX==relativeStarX)
    {
        slopeVertical=true;
        if(relativeStarY > relativeTargetX)
            alpha = pi/2;
        else
            alpha = 3*pi/2;
        return;
    }
    slopeVertical=false;
    raSlope=(relativeStarY-relativeTargetY)/(relativeStarX-relativeTargetX);
    alpha = atan(raSlope);
}

void CropAndResize::ComputeDrift()
{
    double yq;
    if(slopeVertical)
        yq = 0;
    else
        yq = raSlope*(relativeStarX-relativeTargetX);
//    cout << "Xs = " << relativeStarX-relativeTargetX << endl;
//    cout << "Ys = " << relativeStarY-relativeTargetY << endl;
//    cout << "Yq = " << yq << endl;
    double sq = relativeStarY-relativeTargetY-yq;
//    cout << "SQ = " << sq << endl;
    declDrift = sq * cos(alpha);
    double qd = sq * sin(alpha);
//    cout << "QD = " << qd << endl;
    double tq = (relativeStarX-relativeTargetX)/cos(alpha);
//    cout << "TQ = " << tq << endl;
    raDrift = tq+qd;
}

/*
void CropAndResize::ComputeDrift1()
{
    double slopeLinePointX, slopeLinePointY;
    if(relativeTargetX==relativeStarX) // new slope is vertical
    {
        if(slopeVertical)
        {
            raDrift=relativeStarY-relativeTargetY;
            declDrift=0;
            return;
        }
//        else
//        {
//            inverseSlopeOffset=relativeStarY+relativeStarX*raSlope;
//            slopeLinePointX=(relativeStarY-relativeTargetY+(relativeStarX-relativeTargetX)*raSlope)/2/raSlope;
//            slopeLinePointY=slopePointX*raSlope+relativeTargetY;
//        }
    }
    else
    {
        if(slopeVertical)
        {
            declDrift=relativeTargetX-relativeStarX;
            raDrift=relativeStarY-relativeTargetY;
            return;
        }
    }
    slopeLinePointX=(relativeStarY-relativeTargetY+(relativeStarX-relativeTargetX)*raSlope)/2/raSlope;
    slopeLinePointY=slopeLinePointX*raSlope+relativeTargetY;
    slopeLinePointX+=relativeTargetX;
    declDrift = sqrt( ( pow(slopeLinePointX-relativeStarX, 2) + pow(slopeLinePointY-relativeStarY, 2) ) );
    raDrift = sqrt( ( pow(slopeLinePointX-relativeTargetX, 2) + pow(slopeLinePointY-relativeTargetY, 2) ) );
    cout << "SlopeLinePointX=" << slopeLinePointX << endl;
    cout << "SlopeLinePointY=" << slopeLinePointY << endl;
}
*/

void CropAndResize::SnapToClosestStar()
{

}


void CropAndResize::CropAroundSelection(Mat source, Mat *destination, Mat *intermediary, int relativeX, int relativeY, bool changeAbsoluteTarget)
{
    sourceWidth = source.cols;
    sourceHeight = source.rows;
//    if(relativeTargetScaledX != relativeX)
//    {
        if(changeAbsoluteTarget)
        {
            relativeTargetScaledX=relativeX;
            relativeTargetX=relativeTargetScaledX/scaleX;
            absoluteTargetX=relativeTargetX+offsetX;
        }
//    }
//    if(relativeTargetScaledY != relativeY)
//    {
        if(changeAbsoluteTarget)
        {
            relativeTargetScaledY=relativeY;
            relativeTargetY=relativeTargetScaledY/scaleY;
            absoluteTargetY=relativeTargetY+offsetY;
        }
//    }
    absoluteWidth=relativeWidth/scaleX;
    absoluteHeight=relativeHeight/scaleY;
#ifdef DEBUG
    DisplayGeometryData("CropAroundSelection-old offsets");
#endif
// New offset values (center window around target)
    offsetX=absoluteTargetX-absoluteWidth/2;
    offsetY=absoluteTargetY-absoluteHeight/2;
    relativeTargetX=absoluteTargetX-offsetX;
    relativeTargetY=absoluteTargetY-offsetY;
    relativeTargetScaledX=relativeTargetX*scaleX;
    relativeTargetScaledY=relativeTargetY*scaleY;
    relativeStarX=absoluteStarX-offsetX;
    relativeStarY=absoluteStarY-offsetY;
    relativeStarScaledX=relativeStarX*scaleX;
    relativeStarScaledY=relativeStarY*scaleY;
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

void CropAndResize::CropResize(Mat source, Mat *destination, Mat *intermediary)
{
    sourceWidth = source.cols;
    sourceHeight = source.rows;
    CropToRectangle(source, intermediary);
    Resize(*intermediary, destination);
}

#ifdef DEBUG
void CropAndResize::DisplayGeometryData(std::string message)
{
    return;
    cout << endl;
    cout << message << endl;
    cout << "Geometry Data:" << endl;
    cout << "AbsoluteTarget X: " << absoluteTargetX << endl;
    cout << "AbsoluteTarget Y: " << absoluteTargetY << endl;
    cout << "RelativeTarget X: " << relativeTargetX << endl;
    cout << "RelativeTarget Y: " << relativeTargetY << endl;
    cout << "RelativeTargetScaled X: " << relativeTargetScaledX << endl;
    cout << "RelativeTargetScaled Y: " << relativeTargetScaledY << endl;
    cout << "offset X: " << offsetX << endl;
    cout << "offset Y: " << offsetY << endl;
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
