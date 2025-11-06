#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QSlider>
#include "VideoManager.hpp"

#include <opencv2/opencv.hpp>
using namespace cv;

#define CAMERA 0        // Open the default camera
#define EXIT_KEY 27     // Stop capturing by pressing ESC
#define FPS 30

#define COMMANDS_HEIGHT 585
#define COMMANDS_WIDTH 210
#define BTN_HEIGHT 20
#define BTN_WIDTH 200
#define SLIDER_HEIGHT 20
#define SLIDER_WIDTH 170
#define SLIDER_NUM_WIDTH 28
#define SLIDER_TITLE_HEIGHT 15
#define SPACE 5

#define BTN_ABOVE       BTN_HEIGHT+SPACE
#define SLIDER_ABOVE    SLIDER_TITLE_HEIGHT+SLIDER_HEIGHT+SPACE*2


int main(int argc, char** argv)
{
    // 0. BASIC CONFIGURATION FOR VIDEO CAPTURING

    int camera = CAMERA;
    VideoCapture cap;
    if(!cap.open(camera)) // Check VideoCapture documentation;
        return 0;
    VideoWriter video;
    VideoManager videoManager;
    bool recording = false;


    // 1. COMMAND WINDOW SECTION 1

    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Options");
    window.setFixedSize(COMMANDS_WIDTH, COMMANDS_HEIGHT);

    int currentHeight = SPACE*2;

    // 1.1 Mirror horizontally button
    QPushButton *btnMirrorH = new QPushButton("Mirror horizontally", &window);
    btnMirrorH->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    QObject::connect(btnMirrorH, &QPushButton::clicked, [&videoManager]() {
        videoManager.MirrorHorizontal();
    });
    currentHeight += BTN_ABOVE;

    // 1.2 Mirror vertically button
    QPushButton *btnMirrorV = new QPushButton("Mirror vertically", &window);
    btnMirrorV->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    QObject::connect(btnMirrorV, &QPushButton::clicked, [&videoManager]() {
        videoManager.MirrorVertical();
    });
    currentHeight += BTN_ABOVE;

    // 1.3 Clockwise rotation button
    QPushButton *btnRotateClock = new QPushButton("Rotate clockwise", &window);
    btnRotateClock->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    QObject::connect(btnRotateClock, &QPushButton::clicked, [&videoManager]() {
        videoManager.AdjustRotation(90);
    });
    currentHeight += BTN_ABOVE;

    // 1.4 Counter clockwise rotation button
    QPushButton *btnRotateCounter = new QPushButton("Rotate counterclock", &window);
    btnRotateCounter->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    QObject::connect(btnRotateCounter, &QPushButton::clicked, [&videoManager]() {
        videoManager.AdjustRotation(-90);
    });
    currentHeight += BTN_ABOVE;

    // 1.5 Zoom out (and back in) button
    QPushButton *btnZoomOut = new QPushButton("Zoom out", &window);
    QPushButton *btnZoomIn = new QPushButton("Zoom back in", &window);
    btnZoomOut->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    QObject::connect(btnZoomOut, &QPushButton::clicked, [&videoManager, btnZoomIn]() {
        if (!videoManager.IsMaxZoomReached()) {
            videoManager.ZoomOut();
            if (!btnZoomIn->isEnabled()) {btnZoomIn->setEnabled(true);}
        }
    });
    currentHeight += BTN_ABOVE;

    // 1.6 Zoom in button
    // (Declared right above because it's referenced by last button's function)
    btnZoomIn->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    btnZoomIn->setEnabled(false);
    QObject::connect(btnZoomIn, &QPushButton::clicked, [&videoManager, btnZoomIn]() {
        videoManager.ZoomBackIn();
        if (videoManager.IsZoomInnactive()) {btnZoomIn->setEnabled(false);}
    });
    currentHeight += BTN_ABOVE;
    currentHeight += SPACE;

    // 1.7 Separation line
    QFrame *line1 = new QFrame(&window);
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken); 
    line1->setGeometry(SPACE, currentHeight, BTN_WIDTH, 8);
    currentHeight += SPACE*3;


    // 2. COMMAND WINDOW SECTION 2

    // 2.1 Brightness slider
    // Description:
    QLabel *title1 = new QLabel("Brightness:", &window);
    title1->setGeometry(SPACE, currentHeight, BTN_WIDTH, SLIDER_TITLE_HEIGHT);
    title1->setAlignment(Qt::AlignCenter);
    currentHeight += SLIDER_TITLE_HEIGHT;
    // Slider:
    QSlider *sliderBright = new QSlider(Qt::Horizontal, &window);
    sliderBright->setRange(-255, 255);
    sliderBright->setValue(0);
    sliderBright->setGeometry(SPACE, currentHeight, SLIDER_WIDTH, SLIDER_HEIGHT);
    QObject::connect(sliderBright, &QSlider::sliderReleased, [&videoManager, sliderBright]() {
        videoManager.AdjustBrightness(sliderBright->value());
    });
    // Label to show slider value:
    QLabel *num1 = new QLabel(QString::number(sliderBright->value()), &window);
    num1->setGeometry(SLIDER_WIDTH+SPACE, currentHeight-2, SLIDER_NUM_WIDTH, SLIDER_HEIGHT);
    num1->setAlignment(Qt::AlignCenter);
    QObject::connect(sliderBright, &QSlider::valueChanged, [num1](int value) {
        num1->setText(QString::number(value));
    });
    currentHeight += SLIDER_HEIGHT;

    // 2.2 Contrast slider
    // Description:
    QLabel *title2 = new QLabel("Contrast:", &window);
    title2->setGeometry(SPACE, currentHeight, BTN_WIDTH, SLIDER_TITLE_HEIGHT);
    title2->setAlignment(Qt::AlignCenter);
    currentHeight += SLIDER_TITLE_HEIGHT;
    // Slider:
    QSlider *sliderCont = new QSlider(Qt::Horizontal, &window);
    sliderCont->setRange(0, 255);
    sliderCont->setValue(100);
    sliderCont->setGeometry(SPACE, currentHeight, SLIDER_WIDTH, SLIDER_HEIGHT);
    // Dividing slider into frequency sections:
    auto mapConstrastValue = [](int sliderValue) -> float {
        if (sliderValue <= 200) {
            // Maps 0-100 to 0-2
            return sliderValue / 100.0f;
        } else {
            // Maps 100-255 to 2-255
            return 2.0f + ((sliderValue - 200) / 55.0f) * 254.0f;
        }
    };
    QObject::connect(sliderCont, &QSlider::sliderReleased, [&videoManager, sliderCont, mapConstrastValue]() {
        float contrastValue = mapConstrastValue(sliderCont->value());
        videoManager.AdjustContrast(contrastValue);
    });
    // Label to show slider value:
    QLabel *num2 = new QLabel(QString::number(mapConstrastValue(sliderCont->value()), 'f', 2), &window);
    num2->setGeometry(SLIDER_WIDTH+SPACE, currentHeight-2, SLIDER_NUM_WIDTH, SLIDER_HEIGHT);
    num2->setAlignment(Qt::AlignCenter);
    QObject::connect(sliderCont, &QSlider::valueChanged, [num2, mapConstrastValue](float value) {
        if (value <= 200) {
            num2->setText(QString::number(mapConstrastValue(value), 'f', 2));
        } else {
            num2->setText(QString::number(mapConstrastValue(value), 'f', 0));
        }
    });
    currentHeight += SLIDER_HEIGHT;

    // 2.3 Greyscale button
    QPushButton *btnGrey = new QPushButton("Greyscale", &window);
    btnGrey->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    QObject::connect(btnGrey, &QPushButton::clicked, [&videoManager, btnGrey]() {
        videoManager.AlterGreyscale();
        btnGrey->setText(btnGrey->text() == "Greyscale" ? "Colored" : "Greyscale");
    });
    currentHeight += BTN_ABOVE;

    // 2.4 Negative button
    QPushButton *btnNegative = new QPushButton("Negative", &window);
    btnNegative->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    QObject::connect(btnNegative, &QPushButton::clicked, [&videoManager]() {
        videoManager.AlterNegativeFilter();
    });
    currentHeight += BTN_ABOVE;

    // 2.5 Edge detection button
    QPushButton *btnEdges = new QPushButton("Edge detection", &window);
    btnEdges->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    QObject::connect(btnEdges, &QPushButton::clicked, [&videoManager, btnEdges]() {
        videoManager.AlterEdgeDetection();
        btnEdges->setText(btnEdges->text() == "Edge detection" ? "No edge detection" : "Edge detection");
    });
    currentHeight += BTN_ABOVE;

    // 2.6 Gradient button
    QPushButton *btnGrad = new QPushButton("Gradient", &window);
    btnGrad->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    QObject::connect(btnGrad, &QPushButton::clicked, [&videoManager, btnGrad]() {
        videoManager.AlterGradientFilter();
        btnGrad->setText(btnGrad->text() == "Gradient" ? "Turn off gradient" : "Gradient");
    });
    currentHeight += BTN_ABOVE;
    currentHeight += SPACE;

    // 2.7 Separation line
    QFrame *line2 = new QFrame(&window);
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken); 
    line2->setGeometry(SPACE, currentHeight, BTN_WIDTH, 8);
    currentHeight += SPACE*3;


    // 3. COMMAND WINDOW SECTION 3

    // 3.1 Gaussian filter buttons
    QPushButton *btnGaussian = new QPushButton("Apply Gaussian filter", &window);
    QPushButton *btnDeGaussian = new QPushButton("Deactivate Gaussian filter", &window);
    btnGaussian->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    currentHeight += BTN_ABOVE;
    currentHeight += SPACE;

    // 3.2 Gaussian kernel size slider: 
    // Slider description:
    QLabel *title3 = new QLabel("Gaussian kernel size:", &window);
    title3->setGeometry(SPACE, currentHeight, BTN_WIDTH, SLIDER_TITLE_HEIGHT);
    title3->setAlignment(Qt::AlignCenter);
    currentHeight += SLIDER_TITLE_HEIGHT;
    // Slider:
    QSlider *sliderSize = new QSlider(Qt::Horizontal, &window);
    sliderSize->setRange(1, 7);
    sliderSize->setValue(1);
    sliderSize->setGeometry(SPACE, currentHeight, SLIDER_WIDTH, SLIDER_HEIGHT);
    auto mapKernelSizeValue = [](int sliderValue) -> int {
        return sliderValue*2 + 1;
    };
    // Label to show slider value:
    QLabel *num3 = new QLabel(QString::number(sliderSize->value()), &window);
    num3->setGeometry(SLIDER_WIDTH+SPACE, currentHeight-2, SLIDER_NUM_WIDTH, SLIDER_HEIGHT);
    num3->setAlignment(Qt::AlignCenter);
    num3->setText(QString::number(3, 'f', 0));
    QObject::connect(sliderSize, &QSlider::valueChanged, [num3, mapKernelSizeValue](int value) {
        num3->setText(QString::number(mapKernelSizeValue(value), 'f', 0));
    });
    currentHeight += SLIDER_HEIGHT;
    currentHeight += SPACE;

    // 3.3 Connect button and slider
    QObject::connect(btnGaussian, &QPushButton::clicked, [&videoManager, btnGaussian, btnDeGaussian, sliderSize, mapKernelSizeValue]() {
        float newSize = mapKernelSizeValue(sliderSize->value());
        videoManager.ActivateGaussianFilter(newSize);
        btnDeGaussian->setEnabled(true);
    });

    // 3.4 Gaussian deactivation button
    btnDeGaussian->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    btnDeGaussian->setEnabled(false);
    QObject::connect(btnDeGaussian, &QPushButton::clicked, [&videoManager, btnDeGaussian]() {
        videoManager.DeactivateGaussianFilter();
        btnDeGaussian->setEnabled(false);
    });
    currentHeight += BTN_ABOVE;
    currentHeight += SPACE;

    // 3.5 Separation line
    QFrame *line3 = new QFrame(&window);
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Sunken); 
    line3->setGeometry(SPACE, currentHeight, BTN_WIDTH, 8);
    currentHeight += SPACE*3;


    // 4. COMMANDS FOR RECORDING AND RESETING

    QPushButton *btnRec = new QPushButton("Start recording", &window);
    QPushButton *btnStop = new QPushButton("Stop recording", &window);

    // 4.1 Start recording button
    btnRec->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    QObject::connect(btnRec, &QPushButton::clicked, [&recording, btnRec, btnStop]() {
        if (!recording) {
            recording = true;
            btnRec->setText("Recording now!");
            btnStop->setEnabled(true);
        }
    });
    currentHeight += BTN_ABOVE;

    // 4.2 Stop recording button
    btnStop->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    btnStop->setEnabled(false);
    QObject::connect(btnStop, &QPushButton::clicked, [&recording, btnRec, btnStop]() {
        recording = false;
        btnStop->setEnabled(false);
        btnRec->setText("Restart recording");
    });
    currentHeight += BTN_ABOVE;
    currentHeight += SPACE;

    // 4.3 Separation line
    QFrame *line4 = new QFrame(&window);
    line4->setFrameShape(QFrame::HLine);
    line4->setFrameShadow(QFrame::Sunken); 
    line4->setGeometry(SPACE, currentHeight, BTN_WIDTH, 8);
    currentHeight += SPACE*3;

    // 4.4 Reset button
    QPushButton *btnReset = new QPushButton("Reset", &window);
    btnReset->setGeometry(SPACE, currentHeight, BTN_WIDTH, BTN_HEIGHT);
    QObject::connect(btnReset, &QPushButton::clicked, [&videoManager, btnZoomIn, btnDeGaussian, btnGrey, btnEdges, btnGrad]() {
        videoManager.Reset();
        btnZoomIn->setEnabled(false);
        btnDeGaussian->setEnabled(false);
        btnGrey->setText("Greyscale");
        btnEdges->setText("Edge detection");
        btnGrad->setText("Gradient");
    });


    // 5. LAUNCH WINDOW
    window.show();


    // 6. LOOP FOR IMAGE CAPTURING

    bool recordBuffer = false;
    int frameWidth, frameHeight;
    Mat frame, newFrame;
    for(;;) {
        // Save last captured frame:
        cap >> frame;   
        if(frame.empty()) break;        // End video stream if nothing was captured
        imshow("Original", frame);      // Show frame
        videoManager.SetFrame(frame);   // Send frame to VideoManager

        // Edit frame
        QCoreApplication::processEvents();  // Allow command window events to be processed
        videoManager.UpdateFrame();         // Update frame accordingly

        // Record frame if required
        newFrame = videoManager.GetCurrentFrame();
        if (recording) {
            // Configure new video
            if (!recordBuffer) {
                frameWidth = newFrame.cols;
                frameHeight = newFrame.rows;
                video.open("DuckyVideo.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), FPS, Size(frameWidth, frameHeight));
                if (!video.isOpened()) {
                    std::cout << "Failed to open video file" << std::endl;
                    return -1;
                }
            }
            // Register frame
            video.write(newFrame); 
        } else {
            // Save recording in progress
            if (recordBuffer) {
                video.release();
            }
        }

        // Save recording status for future iteration
        recordBuffer = recording;

        // Show new frame (with corrected dimensions)
        newFrame = videoManager.GetRedimensionedFrame();
        imshow("Edited", newFrame);     

        // Stop capturing if exit key is pressed
        if(waitKey(1) == EXIT_KEY) break; 
    }

    cap.release(); // Release the VideoCapture object
    return 0;
}
