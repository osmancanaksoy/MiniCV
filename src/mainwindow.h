#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QtCharts>

#include "mat.h"
#include "object.h"
#include "shape.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionExit_triggered();

    void on_actionIntensity_Histogram_triggered();

    void on_actionColor_Histogram_triggered();

    void on_actionOtsu_Threshold_triggered();

    void on_actionK_Means_Binary_Threshold_triggered();

    void on_actionK_Means_Threshold_triggered();

    void on_actionDilation_triggered();

    void on_actionErosion_triggered();

    void on_actionOpening_triggered();

    void on_actionClosing_triggered();

    void on_actionConnected_Component_Labeling_triggered();

    void on_actionDraw_Bounded_Box_triggered();

    void on_actionDraw_Object_Edge_triggered();

    void on_actionFeature_Extraction_triggered();

    void on_actionTrain_triggered();

    void on_actionTest_triggered();

    void on_actionSmoothening_triggered();

    void on_actionGx_triggered();

    void on_actionGy_triggered();

    void on_actionGradiant_triggered();

    void on_actionOrientation_triggered();

    void on_actionSobel_Edge_Detector_triggered();

    void on_actionCanny_Edge_Detector_triggered();

    void on_actionLine_Detector_triggered();

    void on_actionCircle_Detector_triggered();

    void on_actionAbout_triggered();

    void on_actionContributes_triggered();

private:
    Ui::MainWindow *ui;

    QString file_name;
    Mat image;
    int height;
    int width;
    Mat image_intensity;
    Mat image_binary;
    Mat bounded_image_binary;

    long int** label_matrix;
    std::vector<Object> objects;
    int object_count = 0;
    Shape* shapes;

    Mat smoothed_image;
    Mat Gx;
    Mat Gy;
    Mat gradient;
    Mat direction;
    Matrix magnitude;
    Matrix angle;
    Matrix edge_directions;
    Matrix color;
    Mat image_edge_dect;
    Mat line_detect;
    Mat circle_detect;


    QMessageBox message_box;
    QString image_path;

    //Smoothening
    void smoothening_gaussian_filter();
    std::vector<std::vector<double>> Gaussian_Kernel;

    //Sobel Filter
    Mat calc_horizontal_gradient();
    Mat calc_vertical_gradient();
    void calc_gradient_n_direction();
    std::vector<std::vector<double>> Gx_Kernel;
    std::vector<std::vector<double>> Gy_Kernel;

    //Canny Edge Detector
    void canny_edge_dectector();
    void non_maxima_suppression();
    void hysteresis_thresholding(int T_low, int T_high);

    //Hough Transform
    void hough_line_detector(Mat& mat);
    void hough_circle_detector(Mat& mat);

    void display_Image(string filename, string title);
    Mat convolute(std::vector<std::vector<double>> Kernel, bool normal);
};
#endif // MAINWINDOW_H
