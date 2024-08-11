#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "imio.h"
#include "utils.h"
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Gaussian Kernel 5x5
    Gaussian_Kernel.resize(5, std::vector<double> (5));
    Gaussian_Kernel[0][0] =  (double)1/273; Gaussian_Kernel[0][1] = (double)4/273; Gaussian_Kernel[0][2] =   (double)7/273; Gaussian_Kernel[0][3] = (double)4/273; Gaussian_Kernel[0][4] =  (double)1/273;
    Gaussian_Kernel[1][0] =  (double)4/273; Gaussian_Kernel[1][1] = (double)16/273; Gaussian_Kernel[1][2] =  (double)26/273; Gaussian_Kernel[1][3] = (double)16/273; Gaussian_Kernel[1][4] =   (double)4/273;
    Gaussian_Kernel[2][0] =  (double)7/273; Gaussian_Kernel[2][1] = (double)26/273; Gaussian_Kernel[2][2] =  (double)41/273; Gaussian_Kernel[2][3] = (double)26/273; Gaussian_Kernel[2][4] =   (double)7/273;
    Gaussian_Kernel[3][0] =  (double)4/273; Gaussian_Kernel[3][1] = (double)16/273; Gaussian_Kernel[3][2] =  (double)26/273; Gaussian_Kernel[3][3] = (double)16/273; Gaussian_Kernel[3][4] =   (double)4/273;
    Gaussian_Kernel[4][0] =  (double)1/273; Gaussian_Kernel[4][1] =(double)4/273; Gaussian_Kernel[4][2] =  (double)7/273; Gaussian_Kernel[4][3] = (double)4/273; Gaussian_Kernel[4][4] =   (double)1/273;

    //Sobel Horizontal Kernel
    Gx_Kernel.resize(3, std::vector<double> (3));
    Gx_Kernel[0][0] =  -1; Gx_Kernel[0][1] = 0; Gx_Kernel[0][2] =  1;
    Gx_Kernel[1][0] =  -2; Gx_Kernel[1][1] = 0; Gx_Kernel[1][2] =  2;
    Gx_Kernel[2][0] =  -1; Gx_Kernel[2][1] = 0; Gx_Kernel[2][2] =  1;

    //Sobel Vertical Kernel
    Gy_Kernel.resize(3, std::vector<double> (3));
    Gy_Kernel[0][0] =   1; Gy_Kernel[0][1] =  2; Gy_Kernel[0][2] =  1;
    Gy_Kernel[1][0] =   0; Gy_Kernel[1][1] =  0; Gy_Kernel[1][2] =  0;
    Gy_Kernel[2][0] =  -1; Gy_Kernel[2][1] = -2; Gy_Kernel[2][2] = -1;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionOpen_triggered()
{
    file_name = QFileDialog::getOpenFileName(NULL, QObject::tr("Open Text File[Image]"));
    if(file_name.isEmpty()){
        this->message_box.setText("Select an image");
        this->message_box.exec();
        return;
    }

    this->image = imread(file_name.toStdString());
    this->height=image.getHeight();
    this->width=image.getWidth();
    apply_mean_filter(this->image);
    this->image_intensity = this->image;
    bmp_to_grayscale(this->image_intensity);

    QPixmap image(file_name);
    //ui->label->setPixmap(image.scaled(width, height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    ui->label->setPixmap(file_name);
    ui->label->setScaledContents(true);

    QPixmap intensity("../processed_images/intensity.bmp");
    //ui->label_2->setPixmap(intensity.scaled(width, height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    ui->label_2->setPixmap(intensity);
    ui->label_2->setScaledContents(true);

    this->message_box.setText("Image "+file_name+" has been loaded.");
    this->message_box.exec();
}


void MainWindow::on_actionSave_triggered()
{
    QString file_name = QInputDialog::getText(0,"Save Image","Enter the image name");
    imwrite("../processed_images/" +file_name.toStdString(),this->image_intensity);
    this->message_box.setText("Image has been saved as "+file_name);
    this->message_box.exec();
}


void MainWindow::on_actionExit_triggered()
{
    QMessageBox::StandardButton question;
    question=QMessageBox::question(this, "Quit", "Are you sure ?",QMessageBox::Yes|QMessageBox::No);
    if (question == QMessageBox::Yes){
        QApplication::quit();
    }
    else{
    }
}


void MainWindow::on_actionIntensity_Histogram_triggered()
{
    calculate_intensity_hist(this->image_intensity);
    QChartView *view = new QChartView;
    QChart *chart = new QChart;
    view->setChart(chart);

    QLineSeries *series = new QLineSeries;
    series->setColor(QColor("Black"));
    series->setName("Black");
    for(int j=0; j <256; ++j){
        *series << QPoint(j, this->image_intensity.histogram[j]);
    }

    chart->addSeries(series);

    chart->createDefaultAxes();
    chart->axisX()->setGridLineVisible(false);
    chart->axisY()->setGridLineVisible(false);

    QLabel *label = new QLabel;
    QPixmap image("../processed_images/intensity.bmp");
    label->setPixmap(image.scaled(300, 300,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    QWidget *window=new QWidget;
    window->setWindowTitle("Intensity Histogram");
    QHBoxLayout *lay = new QHBoxLayout;
    lay->addWidget(label);
    lay->addWidget(view);
    window->setLayout(lay);
    window->show();

}


void MainWindow::on_actionColor_Histogram_triggered()
{
    calculate_colored_hist(this->image);
    QChartView *view = new QChartView;
    QChart *chart = new QChart;
    view->setChart(chart);

    QString color[3]={"Red", "Green", "Blue"};
    for(int i=0; i<3; i++){
        QLineSeries *series = new QLineSeries;
        series->setColor(QColor(color[i]));
        series->setName(color[i]);
        for(int j=0; j <256; ++j){
            if(i==0)*series << QPoint(j, this->image.red_histogram[j]);
            else if(i==1)*series << QPoint(j, this->image.green_histogram[j]);
            else *series << QPoint(j, this->image.blue_histogram[j]);
        }
        chart->addSeries(series);
    }

    chart->createDefaultAxes();
    chart->axisX()->setGridLineVisible(false);
    chart->axisY()->setGridLineVisible(false);

    QLabel *label = new QLabel;
    QPixmap image(file_name);
    label->setPixmap(image.scaled(300, 300,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    QWidget *window=new QWidget;
    window->setWindowTitle("Color Histogram");
    QHBoxLayout *lay = new QHBoxLayout;
    lay->addWidget(label);
    lay->addWidget(view);
    window->setLayout(lay);
    window->show();
}

//Image Displaying Function
void::MainWindow::display_Image(string filename, string title){
    QLabel *label = new QLabel;
    QPixmap n(QString::fromUtf8((filename).c_str()));
    label->setPixmap(n.scaled(500, 500,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    QWidget *window=new QWidget;
    QHBoxLayout *lay = new QHBoxLayout;
    lay->addWidget(label);
    window->setWindowTitle(QString::fromUtf8(title.c_str()));
    window->setLayout(lay);
    window->show();
}


void MainWindow::on_actionOtsu_Threshold_triggered()
{
    this->image_binary = this->image_intensity;
    otsu_thresholding(this->image_binary);

    imwrite("../processed_images/otsu_thresholding.bmp", image_binary);
    display_Image("../processed_images/otsu_thresholding.bmp", "Image Segmentation: Otsu Thresholding");
}


void MainWindow::on_actionK_Means_Binary_Threshold_triggered()
{
    this->image_binary = this->image_intensity;
    k_means_thresholding(this->image_binary);

    imwrite("../processed_images/k_means_binary_thresholding.bmp", image_binary);
    display_Image("../processed_images/k_means_binary_thresholding.bmp", "Image Segmentation: K-Means Binary Thresholding");
}

void MainWindow::on_actionDilation_triggered()
{
    dilation(image_binary);

    imwrite("../processed_images/dilation.bmp", image_binary);
    display_Image("../processed_images/dilation.bmp", "Morphology: Dilation");
}


void MainWindow::on_actionErosion_triggered()
{
    erosion(image_binary);

    imwrite("../processed_images/erosion.bmp", image_binary);
    display_Image("../processed_images/erosion.bmp", "Morphology: Erosion");
}


void MainWindow::on_actionOpening_triggered()
{
    erosion(image_binary);
    dilation(image_binary);

    imwrite("../processed_images/opening.bmp", image_binary);
    display_Image("../processed_images/opening.bmp", "Morphology: Opening");
}


void MainWindow::on_actionClosing_triggered()
{
    dilation(image_binary);
    erosion(image_binary);

    imwrite("../processed_images/closing.bmp", image_binary);
    display_Image("../processed_images/closing.bmp", "Morphology: Closing");
}


void MainWindow::on_actionSmoothening_triggered()
{
    smoothening_gaussian_filter();
    imwrite("../processed_images/smoothed_image.bmp", smoothed_image);
    display_Image("../processed_images/smoothed_image.bmp", "Edge Detection: Smoothening");
}


void MainWindow::on_actionGx_triggered()
{
    Mat gradient_x = calc_horizontal_gradient();
    imwrite("../processed_images/gx.bmp", gradient_x);
    display_Image("../processed_images/Gx.bmp", "Sobel Filter: Horizontal Gradient");
}


void MainWindow::on_actionGy_triggered()
{
    Mat gradient_y = calc_vertical_gradient();
    imwrite("../processed_images/gy.bmp", gradient_y);
    display_Image("../processed_images/Gy.bmp", "Sobel Filter: Vertical Gradient");
}


void MainWindow::on_actionGradiant_triggered()
{
    calc_gradient_n_direction();
    imwrite("../processed_images/gradient.bmp", gradient);
    display_Image("../processed_images/gradient.bmp", "Sobel Filter: Gradient");
}


void MainWindow::on_actionOrientation_triggered()
{
    calc_gradient_n_direction();
    imwrite("../processed_images/direction.bmp", direction);
    display_Image("../processed_images/direction.bmp", "Sobel Filter: Direction");
}


void MainWindow::on_actionSobel_Edge_Detector_triggered()
{
    calc_gradient_n_direction();
    image_edge_dect = gradient;
    //otsu_thresholding(image_edge_dect);
    imwrite("../processed_images/sobel_detector.bmp", image_edge_dect);
    display_Image("../processed_images/sobel_detector.bmp", "Sobel Edge Detector: Sobel Edge Image");
}


void MainWindow::on_actionCanny_Edge_Detector_triggered()
{
    canny_edge_dectector();
    imwrite("../processed_images/canny.bmp", image_edge_dect);
    display_Image("../processed_images/canny.bmp", "Canny Edge Detector: Canny Edge Image");
}


void MainWindow::on_actionLine_Detector_triggered()
{
    line_detect = image_edge_dect;
    hough_line_detector(line_detect);
    imwrite("../processed_images/hough_line_detector_binary.bmp", line_detect);
    display_Image("../processed_images/hough_line_detector_binary.bmp", "Hough Transform: Hough Line Detector");

    imwrite("../processed_images/hough_line_detector.bmp", image_intensity);
    display_Image("../processed_images/hough_line_detector.bmp", "Hough Transform: Hough Line Detector");
}


void MainWindow::on_actionCircle_Detector_triggered()
{
    hough_circle_detector(image_edge_dect);
    imwrite("../processed_images/hough_circle_detector_binary.bmp", image_edge_dect);
    display_Image("../processed_images/hough_circle_detector_binary.bmp", "Hough Transform: Hough Circle Detector");

    imwrite("../processed_images/hough_circle_detector.bmp", image_intensity);
    display_Image("../processed_images/hough_circle_detector.bmp", "Hough Transform: Hough Circle Detector");
}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this,"About","BIL3012 - Image Processing Term Project \nLecturer: Prof. Murat EKİNCİ");
}


void MainWindow::on_actionContributes_triggered()
{
    QMessageBox::about(this,"Contributors","Osman Can AKSOY");
}

