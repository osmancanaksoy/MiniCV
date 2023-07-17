#ifndef UTILS_H
#define UTILS_H
#include "mainwindow.h"
#include "mat.h"
#include "pixel.h"
#include "imio.h"
#include "matrix.h"
#include "object.h"
#include "shape.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
#include "float.h"
#include <vector>
#include <algorithm>
//#include "matplotlibcpp.h"

#define M_PI 3.14159265358979323846

//Convert to Gray-Level Image
void bmp_to_grayscale(Mat& mat) {
    for (int i = 0; i < mat.getHeight(); i++) {
        for (int j = 0; j < mat.getWidth(); j++) {

            //int gray_value = 0.30 * mat(i, j).R + 0.59 * mat(i, j).G + 0.11 * mat(i, j).B;
            uint8_t gray_value = (0.3 * mat(i,j).R) + (0.59 * mat(i, j).G) + (0.11 * mat(i, j).B);

            mat(i, j).R = gray_value;
            mat(i, j).G = gray_value;
            mat(i, j).B = gray_value;
        }
    }
    imwrite("../processed_images/intensity.bmp", mat);
}

//Calculating Gray-Level Image Histogram
void calculate_intensity_hist(Mat& mat) {

    mat.histogram = new long int[256];
    for (int i = 0; i < 256; i++)
        mat.histogram[i] = 0;

    for (int i = 0; i < mat.getHeight(); i++) {
        for (int j = 0; j < mat.getWidth(); j++) {
            mat.histogram[mat(i, j).B]++;
        }
    }
}

//Calculating Colored-Image Histogram
void calculate_colored_hist(Mat& mat) {

    mat.red_histogram = new long int[256];
    mat.green_histogram = new long int[256];
    mat.blue_histogram = new long int[256];

    for (int i = 0; i < 256; i++) {
        mat.red_histogram[i] = 0;
        mat.green_histogram[i] = 0;
        mat.blue_histogram[i] = 0;
    }

    for (int i = 0; i < mat.getHeight(); i++) {
        for (int j = 0; j < mat.getWidth(); j++) {
            mat.red_histogram[mat(i, j).R]++;
            mat.green_histogram[mat(i, j).G]++;
            mat.blue_histogram[mat(i, j).B]++;
        }
    }
}

//Inverting Background
void invert_binary_image_background(Mat& mat) {
    Pixel background_color = mat(0, 0);
    if (background_color.R == 0) return;

    for (int i = 0; i < mat.getHeight(); i++) {
        for (int j = 0; j < mat.getWidth(); j++) {
            if (mat(i, j).R == 255) {
                mat(i, j).R = 0;
                mat(i, j).G = 0;
                mat(i, j).B = 0;
            }
            else {
                mat(i, j).R = 255;
                mat(i, j).G = 255;
                mat(i, j).B = 255;
            }
        }
    }
}

void threshold(Mat& mat, int T) {

    for (int i = 0; i < mat.getHeight(); i++) {
        for (int j = 0; j < mat.getWidth(); j++) {
            if (mat(i, j).R < T) {
                mat(i, j).R = 0;
                mat(i, j).G = 0;
                mat(i, j).B = 0;
            }
            else {
                mat(i, j).R = 255;
                mat(i, j).G = 255;
                mat(i, j).B = 255;
            }
        }
    }
    invert_binary_image_background(mat);
}

//Otsu Thresholding
void otsu_thresholding(Mat& mat) {
    calculate_intensity_hist(mat);
    int T = 0; double max = 0;
    for (int i = 0; i < 256; i++) {
        double weight_b = 0, mean_b = 0, weight_f = 0, mean_f = 0;
        for (int j = 0; j < i; j++) {
            weight_b += mat.histogram[j];
            mean_b += (mat.histogram[j] * j);
        }
        for (int j = i; j < 256; j++) {
            weight_f += mat.histogram[j];
            mean_f += (mat.histogram[j] * j);
        }
        mean_b /= weight_b;
        weight_b /= (mat.getHeight() * mat.getWidth());
        mean_f /= weight_f;
        weight_f /= (mat.getHeight() * mat.getWidth());

        double variance = (weight_b * weight_f) * pow((mean_b - mean_f), 2);
        if (variance > max) {
            max = variance;
            T = i;
        }
    }

    threshold(mat, T);
}


//K-Means Method for Thresholding-Binary Image
void k_means_thresholding(Mat& mat)
{
    srand(time(NULL));
    const int k = 2;
    int past_mean_centroid[k];  //Past K1 and K2 centroid mean
    int present_mean_centroid[k];   //New K1 and K2 centroid mean

    std::vector<int> centroid[k];   //Gray level values in K1 and K2
    int old_Error[k], new_Error[k]; //For changes in old centroid and new centroid

    calculate_intensity_hist(mat);
    for (int i = 0; i < k; i++) {
        present_mean_centroid[i] = rand() % 255;
        new_Error[i] = 0;
    }

    do {
        for (int i = 0; i < k; i++) {
            past_mean_centroid[i] = present_mean_centroid[i];
            old_Error[i] = new_Error[i];
            centroid[i].clear();
        }

        //Classifying gray level values in K1 and K2 depending on the nearest centroid
        for (int i = 0; i < 256; i++) {
            int nearest = 0; double distance = 0;
            for (int j = 0; j < k; j++) {
                double d = abs(present_mean_centroid[j] - i);
                if (d > distance) {
                    distance = d; nearest = j;
                }
            }
            centroid[nearest].push_back(i);
        }

        //Calculating mean of each centroids
        for (int i = 0; i < k; i++) {
            double mean = 0, hist = 0;
            for (int j = 0; j < (int)centroid[i].size(); j++) {
                mean += (centroid[i][j] * mat.histogram[centroid[i][j]]); hist += mat.histogram[centroid[i][j]];
            }
            present_mean_centroid[i] = mean / hist;
            new_Error[i] = abs(present_mean_centroid[i] - past_mean_centroid[i]);
        }
    } while (!(new_Error[0] == old_Error[0] && new_Error[1] == old_Error[1])); //Terminate while there are no changes in cluster centres


    for (int i = 0; i < mat.getHeight(); i++) {
        for (int j = 0; j < mat.getWidth(); j++) {
            Pixel color = mat(i, j);

            int group;
            for (int c = 0; c < k; c++) {
                std::vector<int>::iterator it=std::find(centroid[c].begin(), centroid[c].end(), (int)color.R);
                if (it != centroid[c].end()) {
                    group = c;
                    break;
                }
            }
            if (group == 0) {
                mat(i, j).R = 0;
                mat(i, j).G = 0;
                mat(i, j).B = 0;
            }
            else {
                mat(i, j).R = 255;
                mat(i, j).G = 255;
                mat(i, j).B = 255;
            }
        }
    }

    invert_binary_image_background(mat);
}

//Check if centroids are similar
bool is_centroids_same(Pixel *C1, Pixel *C2, int N){
    for(int i=0; i<N; i++){
        if(abs((int)C1[i].R-(int)C2[i].R)<=0.1){
            return false;
        }
        else if(abs((int)C1[i].B-(int)C2[i].B)<=0.1){
            return false;
        }
        else if(abs((int)C1[i].G-(int)C2[i].G)<=0.1){
            return false;
        }
        else{}
    }
    return true;
}


struct Neighbor{
    vector<Point> neighbor;
};

// K-Means Method for Thresholding-Colored Image
void MainWindow::on_actionK_Means_Threshold_triggered()
{
    QString nstring = QInputDialog::getText(0,"Input number of clusters","Input");
    int k= nstring.toInt();
    srand(time(NULL));

    //Initializing k clusters
    Pixel past_mean_centroid[k];
    Pixel present_mean_centroid[k];

    //For grouping
    Neighbor centroid[k];

    Pixel random_colors[k];

    calculate_colored_hist(image);
    for(int i=0; i<k; i++){
        int x = (rand() % width);
        int y = (rand() % height);
        present_mean_centroid[i]=image(y, x);
    }

    for(int i=0; i<k; i++){
        random_colors[i].R = (rand() % 255);
        random_colors[i].G = (rand() % 255);
        random_colors[i].B = (rand() % 255);
    }

    int count=0;
    do{
        for(int i=0; i<k; i++) {
            past_mean_centroid[i]=present_mean_centroid[i];
            centroid[i].neighbor.clear();
        }

        for(int i=0; i<height; i++){
            for(int j=0; j<width; j++){
                int nearest=0; double distance=0;
                Pixel color=image(i, j);
                for(int c=0; c<k; c++){
                    double d=sqrt((pow(present_mean_centroid[c].R-color.R, 2)+
                                     pow(present_mean_centroid[c].G-color.G, 2)+
                                     pow(present_mean_centroid[c].B-color.B, 2)));
                    if(d>distance){
                        distance=d; nearest=c;
                    }
                }
                Point P={j, i};
                centroid[nearest].neighbor.push_back(P);
            }
        }

        //Average mean of each clusters
        for(int i=0; i<k; i++){
            double mean_red=0, mean_blue=0, mean_green=0;
            if (count==0) std::cout<<centroid[i].neighbor.size()<<std::endl;
            for(int j=0; j<(int)centroid[i].neighbor.size(); j++){
                int x=centroid[i].neighbor[j].x;
                int y=centroid[i].neighbor[j].y;
                Pixel color = image(y, x);
                mean_red+=color.R; mean_green+=color.G; mean_blue+=color.B;
            }
            Pixel new_centroid={static_cast<unsigned char>(mean_red/k),
                                  static_cast<unsigned char>(mean_green/k),
                                  static_cast<unsigned char>(mean_blue/k)};
            present_mean_centroid[i]=new_centroid;
        }
        std::cout<<"iteration "<<count<<std::endl;
        count++;
    }while(!is_centroids_same(present_mean_centroid, past_mean_centroid, k));

    this->image_binary=this->image;
    for(int i=0; i<k; i++){
        std::cout<<centroid[i].neighbor.size()<<std::endl;
        for(int j=0; j<(int)centroid[i].neighbor.size(); j++){
            int x=centroid[i].neighbor[j].x;
            int y=centroid[i].neighbor[j].y;
            image_binary(y, x).R = random_colors[i].R;
            image_binary(y, x).G = random_colors[i].G;
            image_binary(y, x).B = random_colors[i].B;
        }
    }

    imwrite("../processed_images/k_means_clustering.bmp", image_binary);
    display_Image("../processed_images/k_means_clustering.bmp", "Image Segmentation: K-Means Clustering");
}

//Dilation
void dilation(Mat& mat) {

    uint16_t B;

    int i;
    int j;
    int k;
    int l;

    Pixel** temp = new Pixel * [mat.getHeight()];

    for (int i = 0; i < mat.getHeight(); i++) {

        temp[i] = new Pixel[mat.getWidth()];

        for (int j = 0; j < mat.getWidth(); j++) {

            temp[i][j].B = mat(i,j).B;
            temp[i][j].G = mat(i, j).G;
            temp[i][j].R = mat(i, j).R;
        }
    }

    for (i = 0; i < mat.getHeight(); i++)
        for (j = 0; j < mat.getWidth(); j++) {

            mat(i, j).B = 0;
            mat(i, j).G = 0;
            mat(i, j).R = 0;
        }

    for (i = 0; i < mat.getHeight() - 2; i++) {
        for (j = 0; j < mat.getWidth(); j++) {

            B = 0;

            for (k = 0; k < 3; k++) {
                for (l = 0; l < 3; l++) {

                    B += temp[i + k][j + l].B;
                }
            }

            if (B == 9 * 255) {

                mat(i + 1, j + 1).B = 255;
                mat(i + 1, j + 1).G = 255;
                mat(i + 1, j + 1).R = 255;
            }
        }
    }
}

//Erosion
void erosion(Mat& mat) {

    uint16_t B;

    int i;
    int j;
    int k;
    int l;

    Pixel** temp = new Pixel * [mat.getHeight()];

    for (int i = 0; i < mat.getHeight(); i++) {

        temp[i] = new Pixel[mat.getWidth()];

        for (int j = 0; j < mat.getWidth(); j++) {

            temp[i][j].B = mat(i,j).B;
            temp[i][j].G = mat(i, j).G;
            temp[i][j].R = mat(i, j).R;
        }
    }

    for (i = 0; i < mat.getHeight(); i++)
        for (j = 0; j < mat.getWidth(); j++) {

            mat(i, j).B = 0;
            mat(i, j).G = 0;
            mat(i, j).R = 0;
        }

    for (i = 0; i < mat.getHeight() - 2; i++) {
        for (j = 0; j < mat.getWidth(); j++) {

            B = 0;

            for (k = 0; k < 3; k++) {
                for (l = 0; l < 3; l++) {

                    B += temp[i + k][j + l].B;
                }
            }

            if (B > 254) {

                mat(i + 1, j + 1).B = 255;
                mat(i + 1, j + 1).G = 255;
                mat(i + 1, j + 1).R = 255;

            }
        }
    }
}

//Mean Filter
void apply_mean_filter(Mat& mat) {

    float R;
    float G;
    float B;
    for (int i = 0; i < mat.getHeight() - 2; i++) {
        for (int j = 0; j < mat.getWidth(); j++) {
            R = 0;
            G = 0;
            B = 0;
            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 3; l++) {
                    R += mat(i + k, j + l).R;
                    G += mat(i + k, j + l).G;
                    B += mat(i + k, j + l).B;

                }
            }
            R /= 9.0;
            G /= 9.0;
            B /= 9.0;

            mat(i + 1, j + 1).R = R;
            mat(i + 1, j + 1).G = G;
            mat(i + 1, j + 1).B = B;
        }
    }

}


//Connected Component Labeling
long int kimebagli(long int* baglistesi, long int deger) {
    while (deger != *(baglistesi + deger)) {
        deger = *(baglistesi + deger);
    }
    return deger;
}

void sirala(int dizi[]) {
    int temp = 0, i = 0;
    while (i < 3) {
        if (dizi[i] > dizi[i + 1]) {
            temp = dizi[i];
            dizi[i] = dizi[i + 1];
            dizi[i + 1] = temp;
            i = 0;
        }
        else { i++; }
    }

}
void MainWindow::on_actionConnected_Component_Labeling_triggered()
{
    Mat mat = this->image_binary;
    Mat ccl = this->image_binary;

    long int* list;
    list = new long int[((mat.getWidth() * mat.getHeight()) / 4)];

    label_matrix = new long int* [mat.getHeight()];
    for (int newd = 0; newd < mat.getHeight(); newd++) {
        label_matrix[newd] = new long int[mat.getWidth()];
    }

    list[0] = 0;
    list[1] = 0;
    int label_count = 2;
    int p = 0;
    int* etiket_sayisi = &p;

    for (int i = 0; i < mat.getHeight(); i++) {
        for (int j = 0; j < mat.getWidth(); j++) {
            mat.pixels[i][j].R > 0 ? label_matrix[i][j] = 1 : label_matrix[i][j] = 0;
        }
    }

    int komsu[4];


    for (int i = 0; i < (image_binary.getHeight() - 1); i++) {
        for (int j = 0; j < (image_binary.getWidth() - 1); j++) {
            if ((int)label_matrix[i][j] != 0) {
                komsu[0] = (int)label_matrix[i][j - 1];
                komsu[1] = (int)label_matrix[i - 1][j - 1];
                komsu[2] = (int)label_matrix[i - 1][j];
                komsu[3] = (int)label_matrix[i - 1][j + 1];

                if (komsu[0] == 0 && komsu[1] == 0 && komsu[2] == 0 && komsu[3] == 0) {

                    label_matrix[i][j] = label_count;
                    list[label_count] = label_count;
                    label_count++;
                }
                else {
                    sirala(komsu);

                    if (komsu[0] != 0 && komsu[1] != komsu[0]) { list[komsu[1]] = kimebagli(list, komsu[0]); }
                    if (komsu[1] != 0 && komsu[2] != komsu[1]) { list[komsu[2]] = kimebagli(list, komsu[1]); }
                    if (komsu[2] != 0 && komsu[3] != komsu[2]) { list[komsu[3]] = kimebagli(list, komsu[2]); }

                    for (int ii = 0; ii < 4; ii++) { if (komsu[ii] > 0) { label_matrix[i][j] = komsu[ii]; break; } }

                }
            }
        }
    }
    long int* yenibag;
    yenibag = new long int[label_count];
    yenibag[0] = 0;
    yenibag[1] = 1;
    for (int bbsay = 2; bbsay < label_count; bbsay++) {
        if (bbsay == kimebagli(list, bbsay)) {
            (*etiket_sayisi)++;//yeni nesne bulundu
            //eski etiketin bu nesne numarasını temsil ettiği saklanıyor
            yenibag[bbsay] = *etiket_sayisi;
        }
    }

    for (int i = 0; i < mat.getHeight() - 1; i++) {
        for (int j = 0; j < mat.getWidth() - 1; j++) {

            uint8_t newlabel = (uint8_t)yenibag[kimebagli(list, label_matrix[i][j])];

            ccl.pixels[i][j].R = (newlabel & 0x0000ff);
            ccl.pixels[i][j].G = ((newlabel & 0x00ff00) >> 8);
            ccl.pixels[i][j].B = ((newlabel & 0xff0000) >> 16);
        }
    }

    for (int i = 0; i < mat.getHeight(); i++) {
        for (int j = 0; j < mat.getWidth(); j++) {
            ccl.pixels[i][j].R = ccl.pixels[i][j].R * (37);
            ccl.pixels[i][j].G = ccl.pixels[i][j].R * (21);
            ccl.pixels[i][j].B = ccl.pixels[i][j].R * (11);
        }
    }

    imwrite("../processed_images/ccl.bmp", ccl);
    display_Image("../processed_images/ccl.bmp", "Image Recognation: Connected Component Labeling");


    Point* p1 = new Point[p];
    Point* p2 = new Point[p];
    Point* p3 = new Point[p];
    Point* p4 = new Point[p];

    bool durum = false;
    for (int etiket = 1; etiket < p + 1; etiket++) {

        for (int i = 0; i < mat.getHeight(); i++) {
            for (int j = 0; j < mat.getWidth(); j++) {

                if (!durum && yenibag[kimebagli(list, label_matrix[i][j])] == etiket) {

                    p1[etiket - 1].set_x_point(i);
                    p1[etiket - 1].set_y_point(j);
                    //x1[etiket - 1] = i;
                    //y1[etiket - 1] = j;

                    durum = true;
                }

                else if (durum && yenibag[kimebagli(list, label_matrix[i][j])] == etiket) {

                    p2[etiket - 1].set_x_point(i);
                    p2[etiket - 1].set_y_point(j);
                    //x2[etiket - 1] = i;
                    //y2[etiket - 1] = j;

                }
            }
        }
        durum = false;
        for (int j = 0; j < mat.getWidth(); j++) {
            for (int i = mat.getHeight() - 1; i >= 0; i--) {

                if (!durum && yenibag[kimebagli(list, label_matrix[i][j])] == etiket) {

                    p3[etiket - 1].set_x_point(i);
                    p3[etiket - 1].set_y_point(j);
                    //x3[etiket - 1] = i;
                    //y3[etiket - 1] = j;

                    durum = true;
                }

                else if (durum && yenibag[kimebagli(list, label_matrix[i][j])] == etiket) {

                    p4[etiket - 1].set_x_point(i);
                    p4[etiket - 1].set_y_point(j);
                    //x4[etiket - 1] = i;
                    //y4[etiket - 1] = j;

                }
            }
        }
        durum = false;
    }



    for (int i = 0; i < mat.getHeight(); i++) {
        for (int j = 0; j < mat.getWidth(); j++) {

            if (yenibag[kimebagli(list, label_matrix[i][j])] != 0)
                label_matrix[i][j] = 1;
        }
    }

    for (int i = 0; i < p; i++) {
        objects.push_back(Object(i+1, label_matrix, p1[i], p2[i], p3[i], p4[i]));
        object_count++;
    }

    delete [] p1;
    delete [] p2;
    delete [] p3;
    delete [] p4;

}

void MainWindow::on_actionDraw_Bounded_Box_triggered()
{
    for(int i = 0; i < object_count; i++) {
        objects[i].draw_bounding_box(image_binary);
    }
    imwrite("../processed_images/bounded_box.bmp", image_binary);
    display_Image("../processed_images/bounded_box.bmp", "Image Recognation: Draw Bounded Box");
}

void MainWindow::on_actionDraw_Object_Edge_triggered()
{
    for(int i = 0; i < object_count; i++) {
        objects[i].cut_object(image_binary);
    }

    this->message_box.setText(QString::number(object_count) + " items were bounded...");
    this->message_box.exec();
}

void MainWindow::on_actionFeature_Extraction_triggered()
{
    QTextEdit *label = new QTextEdit;
    for(int i = 0; i < object_count; i++) {
        objects[i].calculate_invariants();
        cout << objects[i].invariants;
    }

    for(int i = 0; i < object_count; i++) {
        for (int k = 0; k < 1; k++) {
            for (int l = 0; l < 7; l++) {
                label->append(QString::number(objects[i].label_id) + " : "+QString::number(objects[i].invariants(k,l), 'f', 5));
            }
        }
    }

    label->setReadOnly(true);


    QWidget *window=new QWidget;
    QHBoxLayout *lay = new QHBoxLayout;
    lay->addWidget(label);
    window->setWindowTitle(QString::fromUtf8("Calculated Invariants"));
    window->setLayout(lay);
    window->show();

    this->message_box.setText(QString::number(object_count) + " items calculated invariants...");
    this->message_box.exec();
}

void MainWindow::on_actionTrain_triggered()
{
    long double average_array[7] = { 0 };
    for (int a = 0; a < object_count; a++) {
        for (int i = 0; i < 1; i++) {
            for (int j = 0; j < 7; j++) {
                average_array[j] += objects[a].invariants(i, j);
            }
        }
    }
    for (int i = 0; i < 7; i++) {
        average_array[i] /= object_count;
    }

    QString file_name = QInputDialog::getText(0,"Data Set","Enter the data set name");
    ofstream database("../data/" +file_name.toStdString(), ios::out);

    for (int i = 0; i < 7; i++) {
        database << average_array[i] << " ";
    }
    database << endl;

    database.close();
}

long double euclidean_distance(Matrix& matrix_one, Matrix& matrix_two) {

    long double distance = 0;


    for (int i = 0; i < 1; i++) {

        for (int j = 0; j < 7; j++) {

            distance += pow(matrix_one(i, j) - matrix_two(i, j), 2.0);
        }
    }

    long double a = sqrt(distance);

    return a;

}

void MainWindow::on_actionTest_triggered()
{
    //string dosya_adlari[4] = { "dikdortgen.txt", "ucgen.txt", "daire.txt", "kalp.txt" };
    //Shape shape[4] = { Shape("dikdortgen"), Shape("ucgen"), Shape("daire"), Shape("kalp") };

    string dosya_adlari[3] = { "fistik.txt", "coco_pops.txt", "cubuk.txt" };
    Shape shape[3] = { Shape("fistik"), Shape("coco_pops"), Shape("cubuk") };
    for (int a = 0; a < 3; a++) {
        ifstream file("../data/" + dosya_adlari[a], ios::in);

        for (int i = 0; i < 1; i++) {
            for (int j = 0; j < 7; j++) {
                file >> shape[a].invariant(i, j);
            }
        }
        file.close();
    }

    float distance = FLT_MAX;
    float distance_temp;

    int index;
    for (int i = 0; i < object_count; i++) {
        index = 0;
        distance = FLT_MAX;

        for (int j = 0; j < 3; j++) {

            distance_temp = euclidean_distance(shape[j].invariant, objects[i].invariants);


            if (distance_temp < distance) {

                distance = distance_temp;
                index = j;
            }

        }

        shape[index].detected_count++;

    }

    QTextEdit *label = new QTextEdit;
    for (int i = 0; i < 3; i++) {
        label->append(shape[i].name + " : "+QString::number(shape[i].detected_count));
    }
    label->setReadOnly(true);


    QWidget *window=new QWidget;
    QHBoxLayout *lay = new QHBoxLayout;
    lay->addWidget(label);
    window->setWindowTitle(QString::fromUtf8("Detected Objects"));
    window->setLayout(lay);
    window->show();
}

//Gaussian Filter
void MainWindow::smoothening_gaussian_filter(){
    this->smoothed_image=this->image_intensity;
    this->smoothed_image=convolute(Gaussian_Kernel, false);
}

//Gx- Sobel Filter
Mat MainWindow::calc_horizontal_gradient(){
    smoothening_gaussian_filter();
    return convolute(Gx_Kernel, false);
}

//Gy- Sobel Filter
Mat MainWindow::calc_vertical_gradient(){
    smoothening_gaussian_filter();
    return convolute(Gy_Kernel, false);
}

//Calculating Gradient and Orientation- Sobel Filter
void MainWindow::calc_gradient_n_direction(){
    smoothening_gaussian_filter();

    this->gradient=this->smoothed_image;
    this->direction=this->smoothed_image;

    double Gx, Gy;

    double G;
    double temp_angle;

    magnitude.resize(height, width, 0);
    angle.resize(height, width, 0);
    edge_directions.resize(height, width, 0);

    color.resize(height, width, 0);

    for (int i = 0; i < height - 2; i++) {
        for (int j = 0; j < width - 2; j++) {

            Gx = 0;
            Gy = 0;

            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 3; l++) {

                    Gx += smoothed_image(i + k,j + l).B * Gx_Kernel[k][l];
                    Gy += smoothed_image(i + k,j + l).B * Gy_Kernel[k][l];
                }
            }

            G = sqrt(pow(Gx, 2) + pow(Gy, 2));
            G=G>255? 255: G;
            Pixel Gradient(G, G, G);
            gradient(i, j) = Gradient;
            magnitude(i, j) = G;

            temp_angle = atan2(Gy, Gx) / M_PI * 180.0;
            Pixel Theta (temp_angle, temp_angle, temp_angle);
            direction(i,j) = Theta;
            angle(i, j) = temp_angle;
        }
    }
}

//Non-Maximum Suppression
void MainWindow::non_maxima_suppression(){
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {

            if (((angle(i, j) > -22.5) && (angle(i, j) < 22.5)) || ((angle(i, j) > -157.5) && (angle(i, j) < 157.5))) {

                edge_directions(i, j) = 0;
            }

            else if (((angle(i, j) > 22.5) && (angle(i, j) < 67.5)) || ((angle(i, j) > -157.5) && (angle(i, j) < -112.5))) {

                edge_directions(i, j) = 45;
            }



            else if (((angle(i, j) > 67.5) && (angle(i, j) < 112.5)) || ((angle(i, j) > -112.5) && (angle(i, j) < -67.5))) {

                edge_directions(i, j) = 90;
            }


            else if (((angle(i, j) > 112.5) && (angle(i, j) < 157.5)) || ((angle(i, j) > -67.5) && (angle(i, j) < -22.5))) {

                edge_directions(i, j) = 135;
            }
            else {
                edge_directions(i, j) = 0;
            }
        }
    }

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {

            if (edge_directions(i, j) == 0) {

                if (magnitude(i, j) < magnitude(i - 1, j) || magnitude(i, j) < magnitude(i + 1, j)) {
                    color(i, j) = 0;
                }
                else {
                    color(i, j) = magnitude(i, j);
                }
            }

            else if (edge_directions(i, j) == 45) {

                if (magnitude(i, j) < magnitude(i - 1, j - 1) || magnitude(i, j) < magnitude(i + 1, j + 1)) {
                    color(i, j) = 0;
                }
                else {
                    color(i, j) = magnitude(i, j);
                }
            }

            else if (edge_directions(i, j) == 90) {

                if (magnitude(i, j) < magnitude(i, j - 1) || magnitude(i, j) < magnitude(i, j + 1)) {
                    color(i, j) = 0;
                }
                else {
                    color(i, j) = magnitude(i, j);
                }
            }

            else if (edge_directions(i, j) == 135) {

                if (magnitude(i, j) < magnitude(i - 1, j + 1) || magnitude(i, j) < magnitude(i + 1, j - 1)) {
                    color(i, j) = 0;
                }
                else {
                    color(i, j) = magnitude(i, j);
                }
            }
        }
    }

    double max = -1;
    for (int i = 1; i < height; i++) {
        for (int j = 1; j < width; j++) {

            if (magnitude(i, j) > max)
                max = magnitude(i, j);

        }
    }
    cout << max << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image_edge_dect.pixels[i][j].B = color(i, j) < 0 ? 0 : (color(i, j) > 255 ? 255 : color(i, j));
            image_edge_dect.pixels[i][j].G = color(i, j) < 0 ? 0 : (color(i, j) > 255 ? 255 : color(i, j));
            image_edge_dect.pixels[i][j].R = color(i, j) < 0 ? 0 : (color(i, j) > 255 ? 255 : color(i, j));
        }
    }
}

//Hysterisis Thresholding
void MainWindow::hysteresis_thresholding(int T_low, int T_high){

    Matrix son(height, width);


    int low = T_low;
    int high = T_high;

    for (int i = 1; i < height - 2; i++) {
        for (int j = 1; j < width - 2; j++) {

            if (color(i, j) > high) {
                son(i, j) = 255;
            }
            if (color(i, j) < low) {
                son(i, j) = 0;
            }
            else {

                if (edge_directions(i, j) == 0) {

                    if (color(i, j + 1) == 255 || color(i, j - 1) == 255) {
                        son(i, j) = 255;
                    }

                }

                else if (edge_directions(i, j) == 45) {

                    if (color(i - 1, j + 1) == 255 || color(i + 1, j - 1) == 255) {
                        son(i, j) = 255;
                    }
                }

                else if (edge_directions(i, j) == 90) {

                    if (color(i - 1, j) == 255 || color(i + 1, j) == 255) {
                        son(i, j) = 255;
                    }
                }

                else if (edge_directions(i, j) == 135) {

                    if (color(i - 1, j - 1) == 255 || color(i + 1, j + 1) == 255) {
                        son(i, j) = 255;
                    }
                }
            }
        }
    }

    double max = -1;

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            if (color(i, j) > max)
                max = color(i, j);
        }
    }


    Mat yeni(height, width);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image_edge_dect.pixels[i][j].B = son(i, j) < 0 ? 0 : (son(i, j) > 255 ? 255 : son(i, j));
            image_edge_dect.pixels[i][j].G = image_edge_dect.pixels[i][j].B;
            image_edge_dect.pixels[i][j].R = image_edge_dect.pixels[i][j].B;
        }
    }
}


//Canny Edge Detection
void MainWindow::canny_edge_dectector(){
    calc_gradient_n_direction();
    image_edge_dect=gradient;

    //Non-Maxima Suppression
    non_maxima_suppression();
    imwrite("../processed_images/non_maxima.bmp", image_edge_dect);
    display_Image("../processed_images/non_maxima.bmp", "Canny Edge Detector: Non-Maxima Supression");

    //Hysterisis Thresholding
    QString nstring = QInputDialog::getText(0,"Input Low Threshold","Input");
    int T_low=nstring.toInt();
    nstring.clear();
    nstring = QInputDialog::getText(0,"Input High Threshold","Input");
    int T_high=nstring.toInt();
    hysteresis_thresholding(T_low, T_high);
}

Mat MainWindow::convolute(std::vector<std::vector<double>> Kernel, bool normal){
    Mat output=normal?this->image: this->smoothed_image;

    int padding_size=(int)Kernel.size()/2;
    //Gradient X=Image Matrix*KernelX and Gradient Y=Image Matrix*Kernel
    for (int i=padding_size; i<this->height-padding_size; i++){
        for (int j=padding_size; j<this->width-padding_size; j++){
            double Convolute_red=0, Convolute_blue=0, Convolute_green=0;
            for (int k=0; k<Kernel.size(); k++){
                for (int l=0; l<Kernel.size(); l++){
                    if (normal){    //For Colored Filter e.g mean Filter
                        Convolute_red+= Kernel[k][l] * image(i+k-padding_size, j+l-padding_size).R;
                        Convolute_blue+= Kernel[k][l] * image(i+k-padding_size, j+l-padding_size).B;
                        Convolute_green+= Kernel[k][l] * image(i+k-padding_size, j+l-padding_size).G;
                    }
                    else{   //For Intensity Filters e.g gaussian and sobel filter
                        Convolute_red+= Kernel[k][l] * image(i+k-padding_size, j+l-padding_size).R;
                        Convolute_blue=Convolute_green=Convolute_red;
                    }
                }
            }
            Pixel convolute={Convolute_red, Convolute_green, Convolute_blue};

            output(i, j) = convolute;
        }
    }
    return output;
}

void draw_circle(Mat& mat, int center_x, int center_y, int radius, Pixel color) {

    for (double theta = 0; theta <= 2 * M_PI; theta += 0.01) {
        int x = round(radius * cos(theta) + center_x);
        int y = round(radius * sin(theta) + center_y);
        //if (x >= 0 && x < mat.getWidth() && y >= 0 && y < mat.getHeight()) {
        //    mat.pixels[x][y] = color;
        //}
        if (x < 0 || x > mat.getWidth() - 1 || y < 0 || y > mat.getHeight() - 1)
            continue;
        mat.pixels[y][x] = color;
    }
}

void draw_line(Mat& mat, int x0, int y0, int x1, int y1) {

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;

    int err = dx - dy;

    int x = x0;
    int y = y0;

    while (x != x1 || y != y1) {

        if ((x >= 0 && x < mat.getWidth()) && (y >= 0 && y < mat.getHeight())) {
            mat(y, x).R = 0;
            mat(y, x).G = 255;
            mat(y, x).B = 0;
        }

        int e2 = 2 * err;
        if (e2 >= -dy) {
            err -= dy;
            x += sx;
        }

        if (e2 <= dx) {
            err += dx;
            y += sy;
        }
    }

}

void MainWindow::hough_line_detector(Mat& mat) {

    int max_rho = round(sqrt(pow(mat.getHeight(), 2.0) + pow(mat.getWidth(), 2.0)));

    int max_Q = 90;

    int acc_rows = (max_rho * 2) + 1;
    int acc_cols = (max_Q * 2) + 1;

    Matrix acc(acc_rows, acc_cols, 0);

    int max_value = 0;

    for (int i = 0; i < mat.getHeight(); i++) {
        for (int j = 0; j < mat.getWidth(); j++) {
            if (mat(i, j).R == 255) {
                for (int Q = -90; Q <= 90; Q++) {

                    double radian = (Q * M_PI) / 180;
                    double rho = round(j * cos(radian)) + round(i * sin(radian));

                    acc(max_rho + rho, Q + max_Q) = acc(max_rho + rho, Q + max_Q) + 1;

                    if (acc(max_rho + rho, Q + max_Q) > max_value)
                        max_value = acc(max_rho + rho, Q + max_Q);
                }
            }
        }
    }
    //cout << max_value;
    Mat temp(acc_rows, acc_cols);
    for (int i = 0; i < temp.getHeight(); i++) {
        for (int j = 0; j < temp.getWidth(); j++) {
            temp(i, j).R = round((acc(i, j) / (double)max_value) * 255);
            temp(i, j).G = round((acc(i, j) / (double)max_value) * 51);
            temp(i, j).B = round((acc(i, j) / (double)max_value) * 255);
        }
    }

    for (int i = 0; i < temp.getHeight(); i++) {
        for (int j = 0; j < temp.getWidth(); j++) {
            if ((max_value - acc(i, j)) < 30) {
                draw_circle(temp, j, i, 5, Pixel(0, 255, 0));
            }
        }
    }

    imwrite("../processed_images/hough_line_space.bmp", temp);
    display_Image("../processed_images/hough_line_space.bmp", "Hough Transform: Hough Line Space");

    for (int i = 0; i < acc_rows; i++) {
        for (int j = 0; j < acc_cols; j++) {

            if (acc(i, j) > 130) {
                double Q = ((j - max_Q) * M_PI) / 180.0;
                double rho = i - max_rho;

                int x0;
                int y0;

                int x1;
                int y1;

                if ((j - max_Q >= -90 && j - max_Q <= -45) ||
                    (j - max_Q >= 45 && j - max_Q <= 90)) {

                    // y = (r - x cos(t)) / sin(t)
                    x0 = 0;
                    y0 = round((rho - x0 * cos(Q)) / sin(Q));
                    x1 = mat.getWidth() - 1;
                    y1 = round((rho - x1 * cos(Q)) / sin(Q));
                }

                else {

                    // x = (r - y sin(t)) / cos(t);
                    y0 = 0;
                    x0 = round((rho - y0 * sin(Q)) / cos(Q));
                    y1 = mat.getHeight() - 1;
                    x1 = round((rho - y1 * sin(Q)) / cos(Q));
                }

                draw_line(mat, x0, y0, x1, y1);
                draw_line(image_intensity, x0, y0, x1, y1);
            }
        }
    }
}

void MainWindow::hough_circle_detector(Mat& mat) {


    QString nstring = QInputDialog::getText(0,"Input minimum radius","Input");
    int min=nstring.toInt();
    nstring.clear();
    nstring = QInputDialog::getText(0,"Input maximum radius","Input");
    int max=nstring.toInt();


    int*** acc = new int**[max];

    for (int i = 0; i < max; i++) {
        acc[i] = new int*[mat.getHeight()];

        for (int j = 0; j < mat.getHeight(); j++) {
            acc[i][j] = new int [mat.getWidth()];
        }
    }

    for (int k = 0; k < max; k++) {
        for (int i = 0; i < mat.getHeight(); i++) {
            for (int j = 0; j < mat.getWidth(); j++) {
                acc[k][i][j] = 0;
            }
        }
    }


    int max_value = 0;
    int value = 0;
    for (double r = min;r < max; r++) {
        for (int i = 0; i < mat.getHeight(); i++) {

                for (int j = 0; j < mat.getWidth(); j++) {
                if (mat(i,j).B == 255) {
                    for (double theta = 0; theta < 360; theta++) {

                        double a = (double)i - r * cos(theta * M_PI / 180.0);
                        double b = (double)j - r * sin(theta * M_PI / 180.0);

                        if (a < mat.getHeight() && a>0 && b < mat.getWidth() && b>0)
                            value = ++acc[(int)r][(int)a][(int)b];

                        if (value > max_value)
                            max_value = value;

                    }
                }
                }


        }
    }
    cout << max_value;

    Mat temp(mat.getHeight(), mat.getWidth());
    for (int r = min;r < max; r++) {
        for (int i = 0; i < mat.getHeight(); i++) {
                for (int j = 0; j < mat.getWidth(); j++) {

                temp.pixels[i][j].B = acc[r][i][j] < 0 ? 0 : (acc[r][i][j] > 255 ? 255 : acc[r][i][j]);
                temp.pixels[i][j].G = acc[r][i][j] < 0 ? 0 : (acc[r][i][j] > 255 ? 255 : acc[r][i][j]);
                temp.pixels[i][j].R = acc[r][i][j] < 0 ? 0 : (acc[r][i][j] > 255 ? 255 : acc[r][i][j]);
                }
        }
    }

    imwrite("../processed_images/hough_circle_space.bmp", temp);
    display_Image("../processed_images/hough_circle_space.bmp", "Hough Transform: Hough Circle Space");


    for (int r = min; r < max; r++) {
        for (int i = 0; i < mat.getHeight(); i++) {
            for (int j = 0;j < mat.getWidth(); j++) {
                    if (acc[r][i][j] > 140) {
                    draw_circle(mat,j,i,r, Pixel(0,255,0));
                    draw_circle(image_intensity,j,i,r, Pixel(0,255,0));
                    }
            }
        }
    }
}
#endif // UTILS_H

