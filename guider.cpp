#include "guider.h"
#include "ui_guider.h"

#include <iostream>
#include <string>
#include <chrono>
#include <math.h>


//#include <opencv2/opencv.hpp>
//#include <opencv2/core.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/imgproc/imgproc.hpp>


#include <QMouseEvent>
#include "imagelabel.h"


using namespace cv;
using namespace std;

Guider::Guider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Guider)
{
    ui->setupUi(this);
    connect(ui->guiderImageLabel, SIGNAL(Mouse_Pos()), this, SLOT(Mouse_current_pos()));
    connect(ui->guiderImageLabel, SIGNAL(Mouse_Pressed()), this, SLOT(Mouse_pressed()));
    connect(ui->guiderImageLabel, SIGNAL(Mouse_Left()), this, SLOT(Mouse_left()));
//    lsas.DisplayData();
    buttonSize.setHeight(50);
    buttonSize.setWidth(50);
    enabled=false;
    guideStarSelected=false;
    targetSelected=false;
    pi=3.14159265358979323846;
    targetPosition.setX(lsas.setRelativeWidth.value/2);
    targetPosition.setY(lsas.setRelativeHeight.value/2);
    triggerGuide=lsas.setTriggerGuide.value;
    starsDetected=0;
    centerCoefficient=1;
    pincontrol.AscensionRelease();
    pincontrol.DeclinationRelease();
    refreshEnabled=false;
    calibrationStatus=lsas.paramCalibrationStatus.value;
    interfaceWindowOpen=false;
    ascensionGuiding=0;
    declinationGuiding=0;
    if(lsas.paramSlopeVertical.value)
        slopeVertical=true;
    else
        slopeVertical=false;
    raSlope=lsas.paramRaSlope.value;
    arcsecPerPixel=lsas.paramArcsecPerPixel.value;
    normalTrackingSpeed=lsas.setNormalTrackingSpeed.value; // arcsec per second (Earth angular rotation speed)
    acceleratedTrackingSpeed=normalTrackingSpeed*(1+lsas.setGuidingSpeed.value); // = 22.5 arcsec/sec (measured on SW-SA)
    deceleratedTrackingSpeed=normalTrackingSpeed*(1-lsas.setGuidingSpeed.value); // = 7.5 arcsec/sec (measured on SW-SA)
    raDrift=0;
    declDrift=0;
    raDriftArcsec=0;
    declDriftArcsec=0;
    ui->horizontalGammaSlider->setRange(0, 2000);
    ui->horizontalGammaSlider->setValue(1000);
    ui->horizontalGammaSlider->setVisible(true);
    SetSlowButtonImage_2(false);
    SetFastButtonImage_2(false);
    SetNormalButtonImage(true);

#ifdef CAPTUREGUIDING
    captureIndex=0;
    captureFlag=false;
#endif

#ifdef CAPTUREFROMFILE
    fileIndex=0;
#else
//    cap.open(0);// 0 = first camera, 1 = second camera...
    cap.open("/dev/video0");// 0 = first camera, 1 = second camera...
#endif
    NewCapture();
    ui->horizontalZoomSlider->setRange(10, 1000);
    ui->horizontalZoomSlider->setValue((int)dgeometry.scaleX);
    ui->horizontalZoomSlider->setVisible(true);
    DisplayData();
}

Guider::~Guider()
{
    delete ui;
}

void Guider::on_closeButton_clicked()
{
    interfaceWindowOpen=false;
    close();
}

void Guider::on_enableButton_clicked()
{
    if(enabled)
    {
        enabled=false;
        on_normalButton_clicked();
        SetEnableButtonImage(false);
    }
    else
    {
        enabled=true;
        refreshEnabled=false;
        SetRefreshButtonImage(false);
        SetEnableButtonImage(true);
    }
    this->repaint();
}

void Guider::on_refreshButton_clicked()
{
    if(refreshEnabled)
    {
        refreshEnabled=false;
        SetRefreshButtonImage(false);
    }
    else
    {
        refreshEnabled=true;
        enabled=false;
        SetRefreshButtonImage(true);
        SetEnableButtonImage(false);

        lsas.ReadParams();
        starsDetected=0;
        targetSelected=false;
        guideStarSelected=false;
        dgeometry.scaleX=0.746875;
        dgeometry.scaleY=0.746878;
        dgeometry.offsetX=0;
        dgeometry.offsetY=-80;
        dgeometry.relativeWidth=478;
        dgeometry.relativeHeight=478;
//        dgeometry.absoluteWidth=478;
//        dgeometry.absoluteHeight=478;
        dgeometry.absoluteWidth=640;
        dgeometry.absoluteHeight=480;
        dgeometry.sourceTargetX=dgeometry.absoluteWidth/2;
        dgeometry.sourceTargetY=dgeometry.absoluteHeight/2;
        dgeometry.absoluteTargetX=dgeometry.relativeWidth/2;
        dgeometry.absoluteTargetY=dgeometry.relativeHeight/2;
        dgeometry.relativeTargetX=dgeometry.absoluteTargetX*dgeometry.scaleX;
        dgeometry.relativeTargetY=dgeometry.absoluteTargetY*dgeometry.scaleY;
        raDrift=0;
//        raDriftScaled=0;
        raDriftArcsec=0;
        declDrift=0;
//        declDriftScaled=0;
        declDriftArcsec=0;

        ui->horizontalGammaSlider->setValue(1000);
        ui->horizontalZoomSlider->setValue((int)dgeometry.scaleX);
        DisplayData();
        NewCapture();
        this->repaint();
    }
}

void Guider::Flush(cv::VideoCapture& camera)
{ // flush the camera capture buffer to minimize the delay between 2 captured frames

/*
// Old implementation
    int framesWithDelayCount = 0;
    while (framesWithDelayCount <= 1)
    {
        auto begin = chrono::high_resolution_clock::now();
        camera.grab();
        auto end = chrono::high_resolution_clock::now();
        auto dur = end - begin;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        if(ms > 0)
        {
            framesWithDelayCount++;
        }
    }
*/
    int delay = 0;
    QElapsedTimer timer;
    int framesWithDelayCount = 0;
    while (framesWithDelayCount <= 2)
    {
        timer.start();
        camera.grab();
        delay = timer.elapsed();
        if(delay > 1)
        {
            framesWithDelayCount++;
        }
    }
}

void Guider::LoadFromQrc(QString qrc, int flag)
{
    QFile file(qrc);
    if(file.open(QIODevice::ReadOnly))
    {
        qint64 sz = file.size();
        std::vector<uchar> buf(sz);
        file.read((char*)buf.data(), sz);
        srcImage = imdecode(buf, flag);
    }
}

void Guider::DisplayData()
{
    QString valX;
    QString valY;
    QString rezMessage;

    valX=QString::number(dgeometry.scaleX);
    valY=QString::number(dgeometry.scaleY);
    rezMessage=valX+", ";
    rezMessage=rezMessage+valY;
    ui->labelScaleValue->setText(rezMessage);

    valX=QString::number((int)dgeometry.sourceTargetX);
    valY=QString::number((int)dgeometry.sourceTargetY);
    rezMessage=valX+", ";
    rezMessage=rezMessage+valY;
    ui->labelTargetValue->setText(rezMessage);

    valX=QString::number((int)dgeometry.offsetX);
    valY=QString::number((int)dgeometry.offsetY);
    rezMessage=valX+", ";
    rezMessage=rezMessage+valY;
    ui->labelOffsetValue->setText(rezMessage);

    rezMessage=QString::number((double)ui->horizontalGammaSlider->value()/1000);
    ui->labelGammaValue->setText(rezMessage);

    rezMessage=QString::number(starsDetected);
    ui->labelStarsDetected->setText(rezMessage);

    rezMessage=QString::number((double)raDrift);
    ui->labelRaDriftValue->setText(rezMessage);

    rezMessage=QString::number((double)declDrift);
    ui->labelDeclDriftValue->setText(rezMessage);

    rezMessage=QString::number((double)raDriftArcsec);
    ui->labelRaDriftValueArcsec->setText(rezMessage);

    rezMessage=QString::number((double)declDriftArcsec);
    ui->labelDeclDriftValueArcsec->setText(rezMessage);

    if(slopeVertical)
        ui->labelSlopeValue->setText("V");
    else
    {
        rezMessage=QString::number((double)raSlope);
        ui->labelSlopeValue->setText(rezMessage);
    }
}

void Guider::RefreshData() // called by main interface if enabled=true or refreshEnabled=true
{
#ifdef CAPTUREFROMFILE
    if(fileIndex < (numberOfFiles-1))
//    if(fileIndex < (60))
        fileIndex++;
    else
        fileIndex=0;
    cout << "File Index: " << fileIndex << endl;
#endif
    NewCapture();
    if(enabled)
    {
        FindAndTrackStar();
        Calibration();
        DoGuide();
    }
#ifdef CAPTUREGUIDING
    if(captureFlag)
        CaptureImagesToFiles(); // capture guide scope images for simulation purposes
#endif
}

void Guider::LoadUnchanged()
{
    int newx,newy;
    if(srcImage.cols > srcImage.rows)
    {
        newx=ui->guiderImageLabel->width();
        newy=srcImage.rows*newx/srcImage.cols;
        dgeometry.scaleX=(double)newx/(double)srcImage.cols;
        dgeometry.scaleY=dgeometry.scaleX;
#ifdef IMAGELABELSVERTIACALALIGNMENTMIDDLE
        dgeometry.offsetY=(newy - ui->guiderImageLabel->height())/2/dgeometry.scaleY;
#endif
    }
    else
    {
        newy=ui->guiderImageLabel->height();
        newx=srcImage.cols*newy/srcImage.rows;
        dgeometry.scaleY=(double)newy/(double)srcImage.rows;
        dgeometry.scaleX=dgeometry.scaleY;
    }
    dgeometry.absoluteTargetX=dgeometry.sourceTargetX-dgeometry.offsetX;
    dgeometry.absoluteTargetY=dgeometry.sourceTargetY-dgeometry.offsetY;
    dgeometry.relativeTargetX=dgeometry.absoluteTargetX*dgeometry.scaleX;
#ifdef IMAGELABELSVERTIACALALIGNMENTMIDDLE
    // see vertical alignment of the image label controls in piguider.ui and guider.ui
    dgeometry.relativeTargetY=dgeometry.sourceTargetY*dgeometry.scaleY;
#else
    dgeometry.relativeTargetY=dgeometry.absoluteTargetY*dgeometry.scaleY;
#endif
    cv::Size mSize(newx,newy);//the dst image size,e.g.100x100
    cv::resize( srcImage, myImage, mSize);
    processImage=myImage;
}

void Guider::NewCapture()
{
#ifndef CAPTUREFROMFILE
    if(!cap.isOpened())  // check if we succeeded
    {
        cout <<  "Could not open USB camera for capture" << endl ;
        ui->labelMessages->setText("Could not open USB camera for capture");
        ui->labelMessages->adjustSize();
        LoadFromQrc("://media/NightSky.png",IMREAD_COLOR);
//        srcImage = imread("media/NightSky.png");
        return;
    }
    Flush(cap);
    cap >> srcImage;
    cap >> srcImage;
    if( srcImage.empty() )                      // Check for invalid input
    {
        ui->labelMessages->setText("Could not open camera capture");
        ui->labelMessages->adjustSize();
        LoadFromQrc("://media/NightSky.png",IMREAD_COLOR);
//        srcImage = imread("media/NightSky.png");
        return;
    }
#else

//#include "capture-00-Miscellaneous.h" // miscellaneous sky photos
//#include "capture-01-Arcturus-CNE-CWC3.h" // Arcturus Canyon-CNE-CWC3-webcam - 1332 files - 20 min simulation
//#include "capture-02-Arcturus-CNE-HWC1.h" // Arcturus Canyon-CNE-HWC1-webcam - 123 files
//#include "capture-03-Arcturus-Trust-16429.h" // Arcturus Trust-16429-webcam - 41 files
//#include "capture-04-Jupiter-CNE-HWC1.h" // Jupiter Canyon-CNE-HWC1-webcam - 27 files
//#include "capture-05-Jupiter-Trust-16429.h" // Jupiter Trust-16429-webcam - 42 files
//#include "capture-06-Polaris-CNE-CWC3.h" // Polaris Canyon-CNE-CWC3-webcam - 52 files
//#include "capture-07-Polaris-Trust-16429.h" // Polaris Trust-16429-webcam - 24 files
//#include "capture-08-Alkaid-CNE-CWC3.h" // Alkaid - Canyon-CNE-CWC3-webcam-calibration - 512 files
//#include "capture-09-Alkaid-CNE-CWC3.h" // Alkaid - Canyon-CNE-CWC3-webcam-calibration - 404 files
#include "capture-10-Arcturus-CNE-CWC3.h" // Arcturus - Canyon-CNE-CWC3-webcam-RA & Declination drift (with clouds) - 2040 files
//#include "capture-11-Vega-CNE-CWC3.h" // Vega - Canyon-CNE-CWC3-webcam-RA & Declination drift (with clouds) - 1335 files
    if(srcImage.empty())
    {
        ui->labelMessages->setText("Could not open or find the image");
        ui->labelMessages->adjustSize();
        LoadFromQrc("://media/NightSky.png",IMREAD_COLOR);
        return;
    }
#endif

    if(dgeometry.scaleX<=1)
    {
        LoadUnchanged();
    }
    else
    {
        if(dgeometry.offsetY < 0)
            LoadUnchanged();
        else
        {
            dgeometry.CropResize(srcImage, &myImage, &processImage);
        }
    }
    if(interfaceWindowOpen)
        DisplayData();

#ifndef CAPTUREFROMFILE
//        cvtColor(srcImage, srcImage, CV_BGR2RGB);
#endif

    imwrite("/run/shm/GuiderSourceImage.jpg", srcImage);
    GammaCorrection(myImage, (double)ui->horizontalGammaSlider->value()/1000, &myImage);
    if(targetSelected)
    {
        SetReticle(&myImage, dgeometry.relativeTargetX, dgeometry.relativeTargetY, Scalar(0,255,0));
        if(!refreshEnabled)
            SetSlopeLine(&myImage, dgeometry.relativeTargetX, dgeometry.relativeTargetY, slopeVertical, raSlope, Scalar(255,0,0));
    }
    else
    {
        SetReticle(&myImage, dgeometry.relativeTargetX, dgeometry.relativeTargetY, Scalar(0,0,255));
    }
    if(starsDetected==0 || refreshEnabled)
    {
        imwrite("/run/shm/GuiderWorkingImage.jpg", myImage);
        cvtColor(myImage, myImage, CV_BGR2RGB);
        if(interfaceWindowOpen)
            ui->guiderImageLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888)));
    }
    if(interfaceWindowOpen)
        this->repaint();
}

void Guider::SetReticle(Mat *image, int x, int y, Scalar colour)
{
    int reticleDimension=40;
    int reticleGap=reticleDimension/4;

    int startx1=x-reticleDimension;
    int endx1=x-reticleGap;
    int starty1=y;
    int endy1=y;

    int startx2=x;
    int endx2=x;
    int starty2=y-reticleDimension;
    int endy2=y-reticleGap;


    int startx3=x+reticleGap;
    int endx3=x+reticleDimension;
    int starty3=y;
    int endy3=y;

    int startx4=x;
    int endx4=x;
    int starty4=y+reticleGap;
    int endy4=y+reticleDimension;

    line(*image, Point(startx1,starty1), Point(endx1,endy1), colour,2,8,0);
    line(*image, Point(startx2,starty2), Point(endx2,endy2), colour,2,8,0);
    line(*image, Point(startx3,starty3), Point(endx3,endy3), colour,2,8,0);
    line(*image, Point(startx4,starty4), Point(endx4,endy4), colour,2,8,0);
}

void Guider::SetSlopeLine(Mat *image, int x, int y, bool slopeVertical,double slope, Scalar colour)
{
    int imageWidth=image->cols;
    int imageHeight=image->rows;
    if(slopeVertical)
    {
        line(*image, Point(x,0), Point(x,imageHeight), colour,2,8,0);
        return;
    }
    int pointLeftX, pointLeftY, pointRightX, pointRightY;
    double slopeVerticalOffset=(double)y-(double)(slope*x);
    double slopeHorizontalOffset=(-slopeVerticalOffset)/slope;
    double widthOffset=(double)(slope*imageWidth)+slopeVerticalOffset;
    double heightOffset=((double)imageHeight-slopeVerticalOffset)/slope;
    if((slopeVerticalOffset >= 0) && (slopeVerticalOffset <= imageHeight))
    {
        pointLeftX=0;
        pointLeftY=slopeVerticalOffset;
        if((slopeHorizontalOffset >=0) && (slopeHorizontalOffset <=imageWidth))
        {
          pointRightX=slopeHorizontalOffset;
          pointRightY=0;
        }
        else
        {
            if(widthOffset > imageHeight )
            {
                pointRightX=heightOffset;
                pointRightY=imageHeight;
            }
            else
            {
                pointRightX=imageWidth;
                pointRightY=widthOffset;
            }

        }
    }
    else
    {
        if((slopeHorizontalOffset >=0) && (slopeHorizontalOffset <= imageWidth))
        {
            pointLeftX=slopeHorizontalOffset;
            pointLeftY=0;
            if((widthOffset > imageHeight) || (widthOffset < 0) )
            {
                pointRightX=heightOffset;
                pointRightY=imageHeight;
            }
            else
            {
                pointRightX=imageWidth;
                pointRightY=widthOffset;
            }
        }
        else
        {
            pointLeftX=heightOffset;
            pointLeftY=imageHeight;
            pointRightX=imageWidth;
            pointRightY=widthOffset;
        }
    }
    line(*image, Point(pointLeftX,pointLeftY), Point(pointRightX,pointRightY), colour,2,8,0);
}

void Guider::GammaCorrection(const Mat &img, const double gamma_, Mat *result)
{
    CV_Assert(gamma_ >= 0);
    //![changing-contrast-brightness-gamma-correction]
//    Mat img_gamma_corrected;
    Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    for( int i = 0; i < 256; ++i)
        p[i] = saturate_cast<uchar>(pow(i / 255.0, gamma_) * 255.0);
    *result = img.clone();
    LUT(img, lookUpTable, *result);
    //![changing-contrast-brightness-gamma-correction]
}

void Guider::ImageBlur(int blurType, int maxKernelLength)
{
    for ( int i = 1; i < maxKernelLength; i = i + 2 )
    {
        switch(blurType)
        {
        case 0:
            medianBlur ( im, im, i );
            break;
        case 1:
            blur( im, im, Size( i, i ), Point(-1,-1) );
            break;
        case 2:
            GaussianBlur( im, im, Size( i, i ), 0, 0 );
            break;
        default:
            medianBlur ( im, im, i );
        }
    }
}

void Guider::BasicLinearTransform(const Mat &img, const double alpha_, const int beta_, Mat *result)
{
    img.convertTo(*result, -1, alpha_, beta_);
    QString btValue=QString::number(alpha_);
    QString rzMessage="Alpha=: " + btValue;
    btValue=QString::number(beta_);
    rzMessage=rzMessage+" | Beta=: " + btValue;
    ui->labelMessages->setText(rzMessage);
}

void Guider::StarDetector()
{

///////////////////////////// Detection methods /////////////////////////////////////////////
#ifndef BRISKDETECTION
//#define BRISKDETECTION
#endif

#ifndef AKAZEDETECTION
//#define AKAZEDETECTION
#endif

#ifndef ORBDETECTION
//#define ORBDETECTION
#endif

#ifndef KAZEDETECTION
//#define KAZEDETECTION
#endif

#ifndef BLOBDETECTION
#define BLOBDETECTION
#endif
/////////////////////////////////////////////////////////////////////////////////////////////
/*
    int maxPixelValue=0;
    int minPixelValue=255;
    for(int j=0;j<im.rows;j++)
    {
        for (int i=0;i<im.cols;i++)
        {
            if(maxPixelValue < (int)im.at<uchar>(i,j))
               maxPixelValue = (int)im.at<uchar>(i,j);
            if(minPixelValue > (int)im.at<uchar>(i,j))
               minPixelValue = (int)im.at<uchar>(i,j);
        }
    }
    cout << "minPixelValue: " << minPixelValue << endl;
    cout << "maxPixelValue: " << maxPixelValue << endl;
*/

#ifdef BLOBDETECTION
        // Setup SimpleBlobDetector parameters.
    SimpleBlobDetector::Params params;

    // Change thresholds
//    params.minThreshold = 25;
//    params.thresholdStep = 10;
//    params.maxThreshold = 255;
    params.minThreshold = lsas.paramsMinThreshold.value;
    params.thresholdStep = lsas.paramsThresholdStep.value;
    params.maxThreshold = lsas.paramsMaxThreshold.value;

    // Filter by Area.
    params.filterByArea = true;
    params.minArea = lsas.paramsMinArea.value;
//    params.minArea = 1; // scope f=240mm, sensor width=2.5mm
//    params.minArea = 1; // scope f=300mm (4197), sensor width=22.8mm
//    params.minArea = 2; // scope f=135mm (0399), sensor width=22.8mm
//    params.minArea = 1; // scope f=27mm (0432), sensor width=22.8mm
      params.maxArea = lsas.paramsMaxArea.value;

    // filter my min distance
//    params.minDistBetweenBlobs=5;
    params.minDistBetweenBlobs=params.minArea/2;

    // Filter by Circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.7;

    // Filter by Convexity
    params.filterByConvexity = true;
    params.minConvexity = 0.87;

    // Filter by Inertia
    params.filterByInertia = true;
    params.minInertiaRatio = 0.8;

    // Filter by colour
    params.filterByColor=true;
    params.blobColor=255;

    // Set up detector with params
    Ptr<SimpleBlobDetector> keypointDetector = SimpleBlobDetector::create(params);
#endif

#ifdef BRISKDETECTION
//    int threshold = maxPixelValue*.7; // default 30
    int threshold = 20; // default 30
    int octaves = 0; // default 3
    float patternScale = 1.0f; // default 1.0f
    Ptr<BRISK> keypointDetector = BRISK::create(threshold, octaves, patternScale);
    centerCoefficient=0.08;
#endif

#ifdef ORBDETECTION
//    Ptr<ORB> keypointDetector = ORB::create();
    int nfeatures=500;
    float scaleFactor=1.7f; // default 1.2f
    int nlevels=8;
    int edgeThreshold=20; // default31
    int firstLevel=0;
    int WTA_K=2;
    int scoreType=ORB::HARRIS_SCORE;
    int patchSize=20; // default 31
    int fastThreshold=20;
    Ptr<ORB> keypointDetector = ORB::create(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize, fastThreshold);
    centerCoefficient=0.045;
#endif


#ifdef KAZEDETECTION
    bool extended=false;
    bool upright=true;
    float threshold = 0.001f; // default 0.001f
    int octaves = 4; // default 4
    int sublevels = 4; // default 4
    int diffusivity = 1;
    Ptr<KAZE> keypointDetector = KAZE::create(extended, upright, threshold, octaves, sublevels, diffusivity);
    centerCoefficient=0.25;
#endif

#ifdef AKAZEDETECTION
    int descriptor_type=AKAZE::DESCRIPTOR_KAZE;
    int descriptor_size=0;
    int descriptor_channels=1;
    float threshold=0.0005f; // defalt 0.001f
    int octaves=4; // default 4
    int sublevels=4; // default 4
    int diffusivity=0;
    Ptr<AKAZE> keypointDetector = AKAZE::create(descriptor_type, descriptor_size, descriptor_channels, threshold, octaves, sublevels, diffusivity);
    centerCoefficient=0.25;
#endif
    // Detect keypoints
    keypointDetector->detect( im, keypoints);
}

/*
void Guider::FindStarCenter(Point2f startPoint, int kpSize)
{
    int maxPixelValue=0;
    int maxPositionX, maxPositionY;
    for(int j=startPoint.y; j<(startPoint.y+kpSize); j++)
    {
        for (int i=startPoint.x; i<(startPoint.x+kpSize); i++)
        {
            if(maxPixelValue < (int)im.at<uchar>(j,i))
            {
               maxPixelValue = (int)im.at<uchar>(j,i);
               maxPositionX=i;
               maxPositionY=j;
            }
        }
    }
    dgeometry.absoluteStarX = maxPositionX;
    dgeometry.absoluteStarY = maxPositionY;
}
*/
void Guider::FindClosestStarToTarget()
{
    if(starsDetected==0)
        return;
    size_t numberOfKeypoints=keypoints.size();
    Point2f point, point1;
    double distance;
    double minDistance=60000;
    int searchIndex=-1;
    for( int i=0; i<(int)numberOfKeypoints; i++ )
    {
        point = keypoints[i].pt;
        distance = sqrt( ( pow((double)point.x-dgeometry.absoluteTargetX, 2) + pow((double)point.y-dgeometry.absoluteTargetY, 2) ) );
        if(distance < minDistance)
        {
            searchIndex=i;
            minDistance=distance;
        }
    }
//    int keypointSize=keypoints[searchIndex].size;
    point = keypoints.at(searchIndex).pt;

//    FindStarCenter(point, keypointSize);
    dgeometry.absoluteStarX = point.x;
    dgeometry.absoluteStarY = point.y;

//    dgeometry.absoluteStarX = point.x+keypointSize*centerCoefficient;
//    dgeometry.absoluteStarY = point.y+keypointSize*centerCoefficient;

    dgeometry.RecalculateStarFromAbsolutePosition();
}

void Guider::FindAndTrackStar()
{
    if(!targetSelected)
        return;
    im=processImage; // search will be done in the intermediary file (rectangle having the same scale as the original)
    cvtColor(im, im, CV_BGR2GRAY);
    GammaCorrection(im, (double)ui->horizontalGammaSlider->value()/1000, &im);
//#ifdef CAPTUREFROMFILE
//    cvtColor(im, im, CV_BGR2GRAY);
//#endif
//    ImageBlur(2,3);

//    int contrast=8;
//    int brightness=-2;
//    BasicLinearTransform(im, contrast, brightness, &im);

    imwrite("/run/shm/StarDetect.jpg", im);
    //    imshow("original",im);
    //    waitKey();

    StarDetector();
        //the total no of keypoints detected are:
//    int oldStarsDetected=starsDetected;
    starsDetected=keypoints.size();
    FindClosestStarToTarget();
    if(!refreshEnabled)
        ComputeDrift();
    if(interfaceWindowOpen)
        DisplayData();
    if(starsDetected)
    {
        if(guideStarSelected)
            SetReticle(&myImage, dgeometry.relativeStarX, dgeometry.relativeStarY, Scalar(0,0,255));
        imwrite("/run/shm/GuiderWorkingImage.jpg", myImage);
        if(interfaceWindowOpen)
        {
            cvtColor(myImage, myImage, CV_BGR2RGB);
            ui->guiderImageLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888))); // colour
            this->repaint();
        }
    }
}

void Guider::SnapToNearestStar()
{
    dgeometry.absoluteTargetX=dgeometry.absoluteStarX;
    dgeometry.relativeTargetX=dgeometry.absoluteTargetX*dgeometry.scaleX;
    dgeometry.absoluteTargetY=dgeometry.absoluteStarY;
    dgeometry.relativeTargetY=dgeometry.absoluteTargetY*dgeometry.scaleY;
    dgeometry.CropAroundSelection(srcImage, &myImage, &processImage, dgeometry.relativeTargetX, dgeometry.relativeTargetY, true);
    GammaCorrection(myImage, (double)ui->horizontalGammaSlider->value()/1000, &myImage);
    SetReticle(&myImage, dgeometry.relativeTargetX, dgeometry.relativeTargetY, Scalar(0,255,0));
    if(interfaceWindowOpen)
    {
        ui->guiderImageLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888)));
        DisplayData();
        this->repaint();
    }
    guideStarSelected=true;
}

void Guider::ComputeRaSlope()
{
    if(dgeometry.absoluteTargetX==dgeometry.absoluteStarX)
    {
        slopeVertical=true;
        if(dgeometry.absoluteStarY > dgeometry.absoluteTargetY)
            alpha = pi/2;
        else
            alpha = 3*pi/2;
        return;
    }
    slopeVertical=false;
    raSlope=(dgeometry.absoluteStarY-dgeometry.absoluteTargetY)/(dgeometry.absoluteStarX-dgeometry.absoluteTargetX);
    alpha = atan(raSlope);}

void Guider::ComputeDrift()
{
    if(!starsDetected)
    {
        raDrift=0;
        declDrift=0;
    }
    else
    {
        double yq;
        if(slopeVertical)
        {
            raDrift=dgeometry.absoluteStarY-dgeometry.absoluteTargetY;
            declDrift=-(dgeometry.absoluteStarX-dgeometry.absoluteTargetX)*sin(alpha);
        }
        else
        {
            yq = raSlope*(dgeometry.absoluteStarX-dgeometry.absoluteTargetX);
            double sq = dgeometry.absoluteStarY-dgeometry.absoluteTargetY-yq;
            declDrift = sq * cos(alpha);
            double qd = sq * sin(alpha);
            double tq = (dgeometry.absoluteStarX-dgeometry.absoluteTargetX)/cos(alpha);
            raDrift = tq+qd;
        }
    }
    raDriftArcsec=raDrift*arcsecPerPixel;
    declDriftArcsec=declDrift*arcsecPerPixel;
}

void Guider::Calibration()
{
    if(!calibrationStatus)
        return;
    double moveFactor;
    switch(calibrationStatus)
    {
    case 1:
        moveFactor=0.2;
        if( (dgeometry.absoluteStarX >  dgeometry.absoluteTargetX*(1+moveFactor)) || \
            (dgeometry.absoluteStarX <  dgeometry.absoluteTargetX*(1-moveFactor)) || \
            (dgeometry.absoluteStarY >  dgeometry.absoluteTargetY*(1+moveFactor)) || \
            (dgeometry.absoluteStarY <  dgeometry.absoluteTargetY*(1-moveFactor)))
        { // guider star has moved
            ComputeRaSlope();
            calibrationStatus=2;
            ui->labelMessages->setText("C A L I B R A T I O N");
            ui->labelMessages->adjustSize();
        }
        break;
    case 2:
        moveFactor=0.6;
#ifdef DEBUG
        moveFactor*=0.8;
#endif
        if( (dgeometry.absoluteStarX >  dgeometry.absoluteTargetX*(1+moveFactor)) || \
            (dgeometry.absoluteStarX <  dgeometry.absoluteTargetX*(1-moveFactor)) || \
            (dgeometry.absoluteStarY >  dgeometry.absoluteTargetY*(1+moveFactor)) || \
            (dgeometry.absoluteStarY <  dgeometry.absoluteTargetY*(1-moveFactor)))
        { // guider star is close to edge
            leftStarX=dgeometry.sourceStarX;
            leftStarY=dgeometry.sourceStarY;
//                    leftStarX=dgeometry.absoluteStarX;
//                    leftStarY=dgeometry.absoluteStarY;
            ComputeRaSlope();
            ui->labelMessages->setText("C A L I B R A T I O N\nTurn off tracking!");
            ui->labelMessages->adjustSize();
            on_normalButton_clicked();
            calibrationStatus=3;
        }
        break;
    case 3:
        moveFactor=0.2;
        if( (dgeometry.absoluteStarX <  dgeometry.absoluteTargetX*(1+moveFactor)) && \
            (dgeometry.absoluteStarX >  dgeometry.absoluteTargetX*(1-moveFactor)) && \
            (dgeometry.absoluteStarY <  dgeometry.absoluteTargetY*(1+moveFactor)) && \
            (dgeometry.absoluteStarY >  dgeometry.absoluteTargetY*(1-moveFactor)))
            // guide star is close to target position
        {
            ui->labelMessages->setText("C A L I B R A T I O N");
            ui->labelMessages->adjustSize();
            calibrationStatus=4;
            resolutionComputeX=dgeometry.sourceStarX;
            resolutionComputeY=dgeometry.sourceStarY;
            ascensionGuideTimer.start();
        }
        break;
    case 4:
        moveFactor=0.6;
#ifdef DEBUG
        moveFactor*=0.9;
#endif
        if( (dgeometry.absoluteStarX >  dgeometry.absoluteTargetX*(1+moveFactor)) || \
            (dgeometry.absoluteStarX <  dgeometry.absoluteTargetX*(1-moveFactor)) || \
            (dgeometry.absoluteStarY >  dgeometry.absoluteTargetY*(1+moveFactor)) || \
            (dgeometry.absoluteStarY <  dgeometry.absoluteTargetY*(1-moveFactor)))
        { // guider star is close to edge
            double saveTargetX=dgeometry.sourceTargetX;
            double saveTargetY=dgeometry.sourceTargetY;
            dgeometry.sourceTargetX=leftStarX;
            dgeometry.sourceTargetY=leftStarY;
            dgeometry.RecalculateTarget();
            ComputeRaSlope();
            dgeometry.sourceTargetX=saveTargetX;
            dgeometry.sourceTargetY=saveTargetY;
            dgeometry.RecalculateTarget();
            double resolutionDistance=sqrt((resolutionComputeX-dgeometry.sourceStarX) * (resolutionComputeX-dgeometry.sourceStarX) \
                        + (resolutionComputeY-dgeometry.sourceStarY) * (resolutionComputeY-dgeometry.sourceStarY));
            arcsecPerPixel=ascensionGuideTimer.elapsed()*normalTrackingSpeed/resolutionDistance/1000;
            lsas.paramArcsecPerPixel.value=arcsecPerPixel;
            lsas.paramRaSlope.value=raSlope;
            lsas.paramSlopeVertical.value=slopeVertical;
            lsas.paramCalibrationStatus.value=0;
            lsas.paramAlpha.value=alpha;
            lsas.SaveParams();
            QString rezMessage="C A L I B R A T I O N\nResolution: ";
            rezMessage+=QString::number((double)arcsecPerPixel);
            rezMessage+=" arcsec/pixel\nTurn on tracking!\nPress calibration buton when done.";
            ui->labelMessages->setText(rezMessage);
            ui->labelMessages->adjustSize();
            calibrationStatus=5;
        }
        break;
    case 6:
        moveFactor=0.1;
        if( (dgeometry.absoluteStarX <  dgeometry.absoluteTargetX*(1+moveFactor)) && \
            (dgeometry.absoluteStarX >  dgeometry.absoluteTargetX*(1-moveFactor)) && \
            (dgeometry.absoluteStarY <  dgeometry.absoluteTargetY*(1+moveFactor)) && \
            (dgeometry.absoluteStarY >  dgeometry.absoluteTargetY*(1-moveFactor)))
            // guide star is close to target position
        {
            ui->labelMessages->setText("C A L I B R A T I O N\nEnd of calibration.\nPress calibration buton to exit.");
            ui->labelMessages->adjustSize();
            on_normalButton_clicked();
            calibrationStatus=7;
        }
        break;
    }
}

void Guider::DoGuide()
{
    double reductionCoefficient=0.8;
    if(calibrationStatus || !targetSelected)
        return;
    if(raDriftArcsec > triggerGuide)
    {
        ascTimeToGuide=raDriftArcsec/acceleratedTrackingSpeed*reductionCoefficient;
//        on_normalButton_clicked();
        StopGuiders(true,false);
        if(starsDetected)
            on_fastButton_clicked();
        ascensionGuideTimer.start();
    }
    if(raDriftArcsec < -triggerGuide)
    {
        ascTimeToGuide=-raDriftArcsec/acceleratedTrackingSpeed*reductionCoefficient;
//        on_normalButton_clicked();
        StopGuiders(true,false);
        if(starsDetected)
            on_slowButton_clicked();
        ascensionGuideTimer.start();
    }
    if(ascensionGuideTimer.elapsed()/1000 > ascTimeToGuide)
        StopGuiders(true,false);
    if(lsas.iSDeclinationTracked.value)
    {
        if(declDriftArcsec > triggerGuide)
        {
            declTimeToGuide=declDriftArcsec/acceleratedTrackingSpeed*reductionCoefficient;
            StopGuiders(false,true);
            if(starsDetected)
                on_fastButton_2_clicked();
            declinationGuideTimer.start();
        }
        if(declDriftArcsec < -triggerGuide)
        {
            declTimeToGuide=-declDriftArcsec/acceleratedTrackingSpeed*reductionCoefficient;
            StopGuiders(false,true);
            if(starsDetected)
                on_slowButton_2_clicked();
            declinationGuideTimer.start();
        }
        if(declinationGuideTimer.elapsed()/1000 > declTimeToGuide)
            StopGuiders(false,true);
    }
}

void Guider::StopGuiders(bool ascension, bool declination)
{
    if(lsas.iSDeclinationTracked.value)
    {
        if(ascension)
        {
            if(declination)
            {
                on_normalButton_clicked();
            }
            else
            {
                ascensionGuiding=0;
//                declinationGuiding=false;
                pincontrol.AscensionRelease();
//                pincontrol.DeclinationRelease();
                if(interfaceWindowOpen)
                {
                    SetSlowButtonImage(false);
//                    SetNormalButtonImage(true);
                    SetFastButtonImage(false);
//                    SetSlowButtonImage_2(false);
//                    SetFastButtonImage_2(false);
                }
            }
        }
        else
        {
            if(declination)
            {
//                ascensionGuiding=0;
                declinationGuiding=0;
//                pincontrol.AscensionRelease();
                pincontrol.DeclinationRelease();
                if(interfaceWindowOpen)
                {
//                    SetSlowButtonImage(false);
//                    SetNormalButtonImage(true);
//                    SetFastButtonImage(false);
                    SetSlowButtonImage_2(false);
                    SetFastButtonImage_2(false);
                }
            }
//            else
//            {
//            }
        }
    }
    else
        on_normalButton_clicked();
}

void Guider::SetEnableButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/tools-512x512/shutdown-on.png");
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/shutdown.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->enableButton->setIcon(buttonIcon);
    ui->enableButton->setIconSize(buttonSize);
    ui->enableButton->setFixedSize(buttonSize);
}

void Guider::SetRefreshButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/tools-512x512/search-on.png");
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/search.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->refreshButton->setIcon(buttonIcon);
    ui->refreshButton->setIconSize(buttonSize);
    ui->refreshButton->setFixedSize(buttonSize);
}

void Guider::SetSlowButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/tools-512x512/gearMinus-on.png");
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/gearMinus.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->slowButton->setIcon(buttonIcon);
    ui->slowButton->setIconSize(buttonSize);
    ui->slowButton->setFixedSize(buttonSize);
}

void Guider::SetNormalButtonImage(bool on)
{
    QIcon buttonIcon;
    if(lsas.iSDeclinationTracked.value)
    {
        if(ascensionGuiding)
        {
            if(declinationGuiding)
                changingButtonsPixmap.load("://media/icons/tools-512x512/gearBlue.png");
            else
                changingButtonsPixmap.load("://media/icons/tools-512x512/gearGrey.png");
        }
        else
        {
            if(declinationGuiding)
                changingButtonsPixmap.load("://media/icons/tools-512x512/gearGrey.png");
            else
                changingButtonsPixmap.load("://media/icons/tools-512x512/gearNeutral-on.png");
        }
    }
    else
    {
        if(on)
            changingButtonsPixmap.load("://media/icons/tools-512x512/gearNeutral-on.png");
        else
            changingButtonsPixmap.load("://media/icons/tools-512x512/gearNeutral.png");
    }
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->normalButton->setIcon(buttonIcon);
    ui->normalButton->setIconSize(buttonSize);
    ui->normalButton->setFixedSize(buttonSize);
}

void Guider::SetFastButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/tools-512x512/gearPlus-on.png");
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/gearPlus.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->fastButton->setIcon(buttonIcon);
    ui->fastButton->setIconSize(buttonSize);
    ui->fastButton->setFixedSize(buttonSize);
}

void Guider::SetSlowButtonImage_2(bool on)
{
    QIcon buttonIcon;
    if(lsas.iSDeclinationTracked.value)
    {
        if(on)
            changingButtonsPixmap.load("://media/icons/tools-512x512/gearMinus-on.png");
        else
            changingButtonsPixmap.load("://media/icons/tools-512x512/gearMinus.png");
    }
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/gearMinusGrey.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->slowButton_2->setIcon(buttonIcon);
    ui->slowButton_2->setIconSize(buttonSize);
    ui->slowButton_2->setFixedSize(buttonSize);
}

void Guider::SetFastButtonImage_2(bool on)
{
    QIcon buttonIcon;
    if(lsas.iSDeclinationTracked.value)
    {
        if(on)
            changingButtonsPixmap.load("://media/icons/tools-512x512/gearPlus-on.png");
        else
            changingButtonsPixmap.load("://media/icons/tools-512x512/gearPlus.png");
    }
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/gearPlusGrey.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->fastButton_2->setIcon(buttonIcon);
    ui->fastButton_2->setIconSize(buttonSize);
    ui->fastButton_2->setFixedSize(buttonSize);
}




void Guider::SetCalibrateButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/tools-512x512/calibration-on.png");
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/calibration.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->calibrateButton->setIcon(buttonIcon);
    ui->calibrateButton->setIconSize(buttonSize);
    ui->calibrateButton->setFixedSize(buttonSize);
}

#ifdef CAPTUREGUIDING
void Guider::CaptureImagesToFiles()
{
    if(!lsas.IsUSBFolder())
    {
        ui->labelMessages->setText("Capture media absent!\n(USB flash mem)\nCan not capture.");
        ui->labelMessages->adjustSize();
        return;
    }
    char fileName[500];
    FILE* dataFile;
    if(!targetSelected || refreshEnabled)
    {
        ui->labelMessages->setText("Target star is not selected\nor capture is in refresh mode!\nData recording disabled.");
        ui->labelMessages->adjustSize();
    }
    else
    {
        strcpy(fileName, lsas.usbFlashMemoryPath.value);
        strcat(fileName, "GuiderData.csv");
        bool fileExists=false;
        if(lsas.IsFile(fileName))
            fileExists=true;
        if( !(dataFile=fopen(fileName, "a+")))
        {
            cout << "Unable to open data file " << fileName << endl;
            return;
        }
        QString buff;
        if(!fileExists)
        {
            buff= "epochTime,relativeTime,raDrift, declDrift, x, y\n";
            fwrite(buff.toLatin1(), buff.length(), 1, dataFile);
        }
        time_t currTime=time(0);
        buff = QString::number((time_t)currTime) + ",0,";
        buff += QString::number((double)raDriftArcsec) + ",";
        buff += QString::number((double)declDriftArcsec) + ",";
        buff += QString::number((double)dgeometry.absoluteStarX) + ",";
        buff += QString::number((double)dgeometry.absoluteStarY);
        fwrite(buff.toLatin1(), buff.length(), 1, dataFile);
        buff="\n";
        fwrite(buff.toLatin1(), buff.length(), 1, dataFile);
        fclose(dataFile);
    }
    if(!lsas.captureJustGuiderData.value)
    {
        if(captureIndex%2)
        {
            lsas.SetChronologicFileName();
            char chronoFile[100];
            strcpy(chronoFile, lsas.chronologicFilename.toLatin1());
            strcpy(fileName, lsas.usbFlashMemoryPath.value);
            strcat(fileName, lsas.GuiderFolder.value);
            strcat(fileName, lsas.chronologicFilename.toLatin1());
            strcat(fileName,".jpg");
            imwrite(fileName, srcImage);
        }
        captureIndex++;
    }
}
#endif

void Guider::on_testButton_clicked()
{
    FindAndTrackStar();
}

void Guider::on_plusButton_clicked()
{
//    ui->horizontalSlider->setValue((int)(dgeometry.scaleX*10*1.1));
    dgeometry.Zoom(1.1);
    dgeometry.CropAroundSelection(srcImage, &myImage, &processImage, dgeometry.relativeTargetX, dgeometry.relativeTargetY, false);
    GammaCorrection(myImage, (double)ui->horizontalGammaSlider->value()/1000, &myImage);
    imwrite("/run/shm/GuiderWorkingImage.jpg", myImage);
    cvtColor(myImage, myImage, CV_BGR2RGB);
    ui->guiderImageLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888)));
    DisplayData();
    ui->horizontalZoomSlider->setValue((int)dgeometry.scaleX*10);
    this->repaint();
}

void Guider::on_minusButton_clicked()
{
    dgeometry.Zoom(0.9);
    dgeometry.CropAroundSelection(srcImage, &myImage, &processImage, dgeometry.relativeTargetX, dgeometry.relativeTargetY, false);
    GammaCorrection(myImage, (double)ui->horizontalGammaSlider->value()/1000, &myImage);
    imwrite("/run/shm/GuiderWorkingImage.jpg", myImage);
    cvtColor(myImage, myImage, CV_BGR2RGB);
    ui->guiderImageLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888)));
    DisplayData();
    ui->horizontalZoomSlider->setValue((int)dgeometry.scaleX*10);
    this->repaint();
}

void Guider::on_targetButton_clicked()
{
#ifdef DEBUG
    dgeometry.DisplayGeometryData("target button clicked");
#endif
    targetSelected=true;
//    cout << "Target X: " << targetPosition.x() << endl;
//    cout << "Target Y: " << targetPosition.y() << endl;
    dgeometry.CropAroundSelection(srcImage, &myImage, &processImage, targetPosition.x(), targetPosition.y(), true);
    GammaCorrection(myImage, (double)ui->horizontalGammaSlider->value()/1000, &myImage);
    DisplayData();
    FindAndTrackStar();
    if(starsDetected>0)
    {
        SnapToNearestStar();
        ui->labelMessages->setText("Target locked on guide star.");
        ui->labelMessages->adjustSize();
    }
    else
    {
        ui->labelMessages->setText("Could not find any guide star!");
        ui->labelMessages->adjustSize();
        SetReticle(&myImage, dgeometry.relativeTargetX, dgeometry.relativeTargetY, Scalar(0,255,0));
        imwrite("/run/shm/GuiderWorkingImage.jpg", myImage);
        cvtColor(myImage, myImage, CV_BGR2RGB);
        ui->guiderImageLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888))); // colour
    }
    this->repaint();
}


void Guider::Mouse_current_pos()
{
//#ifdef DEBUG
//    cout << "X: " << ui->guiderImageLabel->x << endl;
//    cout << "Y: " << ui->guiderImageLabel->y << endl;
//#endif
}

void Guider::Mouse_pressed()
{
    targetPosition.setX(ui->guiderImageLabel->x);
    targetPosition.setY(ui->guiderImageLabel->y);
//#ifdef DEBUG
//    cout << "targetX: " << ui->guiderImageLabel->x << endl;
//    cout << "targetY: " << ui->guiderImageLabel->y << endl;
//#endif
}

void Guider::Mouse_left()
{

}


void Guider::on_horizontalZoomSlider_valueChanged(int value)
{
    dgeometry.ZoomAbsolute((float)(value/10));
//    dgeometry.CropResize(srcImage, &myImage, &processImage);
    dgeometry.CropAroundSelection(srcImage, &myImage, &processImage, dgeometry.relativeTargetX, dgeometry.relativeTargetY, false);
    GammaCorrection(myImage, (double)ui->horizontalGammaSlider->value()/1000, &myImage);
    imwrite("/run/shm/GuiderWorkingImage.jpg", myImage);
    cvtColor(myImage, myImage, CV_BGR2RGB);
    ui->guiderImageLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888)));
    DisplayData();
    this->repaint();
}

void Guider::on_horizontalGammaSlider_valueChanged()
{
    DisplayData();
}

void Guider::on_horizontalGammaSlider_sliderReleased()
{
    NewCapture();
}

void Guider::on_saveButton_clicked()
{
#ifdef CAPTUREGUIDING
    if(captureFlag)
    {
        captureFlag=false;
        pincontrol.AscensionRelease();
        ui->labelMessages->setText("File saving toggled OFF.");
    }
    else
    {
        captureFlag=true;
//        pincontrol.AscensionRelease(); // record normal guidespeed
        pincontrol.AscensionPlus(); // record accelerated guide speed
//        pincontrol.AscensionMinus(); // record decelerated guide speed
        ui->labelMessages->setText("File saving toggled ON.");
    }
    ui->labelMessages->adjustSize();
#endif
}

void Guider::on_slowButton_clicked()
{
    ascensionGuiding=1;
    pincontrol.AscensionMinus();
    if(interfaceWindowOpen)
    {
        SetSlowButtonImage(true);
        SetNormalButtonImage(false);
        SetFastButtonImage(false);
    }
}

void Guider::on_normalButton_clicked()
{
    if(lsas.iSDeclinationTracked.value)
    {
        declinationGuiding=0;
        pincontrol.DeclinationRelease();
        if(interfaceWindowOpen)
        {
            SetSlowButtonImage_2(false);
            SetFastButtonImage_2(false);
        }
    }
    ascensionGuiding=0;
    pincontrol.AscensionRelease();
    if(interfaceWindowOpen)
    {
        SetSlowButtonImage(false);
        SetNormalButtonImage(true);
        SetFastButtonImage(false);
    }
}

void Guider::on_fastButton_clicked()
{
    ascensionGuiding=-1;
    pincontrol.AscensionPlus();
    if(interfaceWindowOpen)
    {
        SetSlowButtonImage(false);
        SetNormalButtonImage(false);
        SetFastButtonImage(true);
    }
}

void Guider::on_fastButton_2_clicked()
{
    if(!lsas.iSDeclinationTracked.value)
        return;
    declinationGuiding=-1;
    pincontrol.DeclinationPlus();
    if(interfaceWindowOpen)
    {
        SetSlowButtonImage_2(false);
        SetNormalButtonImage(false);
        SetFastButtonImage_2(true);
    }
}

void Guider::on_slowButton_2_clicked()
{
    if(!lsas.iSDeclinationTracked.value)
        return;
    declinationGuiding=1;
    pincontrol.DeclinationMinus();
    if(interfaceWindowOpen)
    {
        SetSlowButtonImage_2(true);
        SetNormalButtonImage(false);
        SetFastButtonImage_2(false);
    }
}

void Guider::on_calibrateButton_clicked()
{
    if(!targetSelected)
    {
        ui->labelMessages->setText("GUIDE STAR NOT SELECTED!\nCan not start/continue calibration!");
        ui->labelMessages->adjustSize();
        return;
    }
    switch(calibrationStatus)
    {
    case 0:
        calibrationStatus=1;
        ui->labelMessages->setText("C A L I B R A T I O N\nTurn off or on tracking when prompted\nto do so...");
        ui->labelMessages->adjustSize();
        enabled=true;
        refreshEnabled=false;
        on_fastButton_clicked();
        SetEnableButtonImage(true);
        SetRefreshButtonImage(false);
        SetCalibrateButtonImage(true);
        break;
    case 5:
        calibrationStatus=6;
        SetCalibrateButtonImage(true);
        ui->labelMessages->setText("C A L I B R A T I O N\nReturning guide star to center.");
        ui->labelMessages->adjustSize();
        on_fastButton_clicked();
        break;
    case 7:
        calibrationStatus=0;
        SetCalibrateButtonImage(false);
        ui->labelMessages->setText("Message Box");
        ui->labelMessages->adjustSize();
        break;
    default:
        calibrationStatus=0;
        SetCalibrateButtonImage(false);
        ui->labelMessages->setText("Message Box");
        ui->labelMessages->adjustSize();
        on_normalButton_clicked();
    }
}

