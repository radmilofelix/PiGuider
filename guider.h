#ifndef GUIDER_H
#define GUIDER_H

#include <QWidget>


#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
//#include <ctime>
#include <QElapsedTimer>

#include "pincontrolpi.h"
#include "displaygeometry.h"
#include "loadsaveandsettings.h"



namespace Ui {
class Guider;
}

class Guider : public QWidget
{
    Q_OBJECT

public:
    explicit Guider(QWidget *parent = 0);
    ~Guider();
    bool enabled;
    bool guideStarSelected;
    bool targetSelected;
    cv::VideoCapture cap;
    bool refreshEnabled; // status fot the refresh button
    bool interfaceWindowOpen;
    int ascensionGuiding, declinationGuiding;
    int calibrationStatus;
    int triggerGuide; // arcseconds - if drift is under this value guiding will not be triggered
    cv::Mat myImage, srcImage, processImage, im;
    DisplayGeometry dgeometry;
    QPoint targetPosition;
    std::vector<cv::KeyPoint> keypoints;
    size_t starsDetected;
    float centerCoefficient;
    PinControlPi pincontrol;
    LoadSaveAndSettings lsas;
    double pi;
    double leftStarX, leftStarY, resolutionComputeX, resolutionComputeY;
    double arcsecPerPixel;
    double normalTrackingSpeed, acceleratedTrackingSpeed, deceleratedTrackingSpeed; // arcsec/second
    double raDrift, declDrift, raDriftArcsec, declDriftArcsec;
//    double raDriftScaled, declDriftScaled;
    double raSlope;
    double alpha;
    bool slopeVertical;
    double ascTimeToGuide, declTimeToGuide;
    QElapsedTimer ascensionGuideTimer, declinationGuideTimer;
    QPixmap changingButtonsPixmap;
    QSize buttonSize;
    void Flush(cv::VideoCapture& camera);
    void LoadFromQrc(QString qrc, int flag);
    void DisplayData();
    void RefreshData();
    void LoadUnchanged();
    void NewCapture();
    void SetReticle(cv::Mat *image, int x, int y, cv::Scalar colour);
    void SetSlopeLine(cv::Mat *image, int x, int y, bool slopeVertical, double slope,cv::Scalar colour);
    void GammaCorrection(const cv::Mat &img, const double gamma_, cv::Mat *result);
    void ImageBlur(int blurType, int maxKernelLength);
    void BasicLinearTransform(const cv::Mat &img, const double alpha_, const int beta_, cv::Mat *result);
    void StarDetector();
//    void FindStarCenter(cv::Point2f startPoint, int kpSize);
    void FindClosestStarToTarget();
    void FindAndTrackStar();
    void SnapToNearestStar();
    void ComputeRaSlope();
    void ComputeDrift();
    void Calibration();
    void DoGuide();
    void StopGuiders(bool ascension, bool declination);

    void SetEnableButtonImage(bool on);
    void SetRefreshButtonImage(bool on);
    void SetSlowButtonImage(bool on);
    void SetNormalButtonImage(bool on);
    void SetFastButtonImage(bool on);
    void SetSlowButtonImage_2(bool on);
    void SetFastButtonImage_2(bool on);
    void SetCalibrateButtonImage(bool on);

#ifdef CAPTUREGUIDING
    void CaptureImagesToFiles();
    int captureIndex;
    bool captureFlag;
#endif
#ifdef CAPTUREFROMFILE
    int fileIndex;
    int numberOfFiles;
    char filenameFull[4096];
#endif


private slots:
    void on_closeButton_clicked();
    void on_enableButton_clicked();
    void on_refreshButton_clicked();
    void on_plusButton_clicked();
    void on_minusButton_clicked();
    void on_targetButton_clicked();

    void Mouse_current_pos();
    void Mouse_pressed();
    void Mouse_left();

//    void on_resetButton_clicked();
    void on_testButton_clicked();
    void on_horizontalZoomSlider_valueChanged(int value);
    void on_horizontalGammaSlider_valueChanged();
    void on_horizontalGammaSlider_sliderReleased();
//    void on_raSlopeButton_clicked();
//    void on_snapButton_clicked();

    void on_saveButton_clicked();
    void on_slowButton_clicked();
    void on_normalButton_clicked();
    void on_fastButton_clicked();
    void on_fastButton_2_clicked();
    void on_slowButton_2_clicked();
    void on_calibrateButton_clicked();


private:
    Ui::Guider *ui;
};

#endif // GUIDER_H
