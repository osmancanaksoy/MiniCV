#include "object.h"
#include "imio.h"
#include <fstream>
#include <QtMath>

using namespace std;

Object::Object() {}

Object::Object(int label_id, long int** matrix, Point p1,
               Point p2, Point p3, Point p4) {

    this->label_id = label_id;
    this->matrix = matrix;
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
    this->p4 = p4;

    height = p2.get_x_point() - p1.get_x_point() + 1;
    width = p4.get_y_point() - p3.get_y_point() + 1;

    invariants.resize(1, 7);
}

Object::~Object() {}

long double Object::m(long double p, long double q) {
    long double sum = 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            sum += pow((long double)i, (long double)p) * pow((long double)j, (long double)q) * (long double)matrix[p1.get_x_point() + i][p3.get_y_point() + j];
        }
    }

    return sum;
}

long double Object::mu(long double p, long double q) {

    long double sum = 0;


    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            if (matrix[p1.get_x_point() + i][p3.get_y_point() + j])
                sum += pow((long double)i - centroid_row, (long double)p) * pow((long double)j - centroid_col, (long double)q);
        }
    }

    return sum;
}

long double Object::nu(long double p, long double q) {

    long double k = 0;

    if (p + q >= 2)
        k = (long double)(p + q) / 2.0 + 1.0;

    long double nu = mu(p, q) / pow(mu(0, 0), k);

    return nu;
}

void Object::calculate_centroid() {

    centroid_row = m(1, 0) / m(0, 0);
    centroid_col = m(0, 1) / m(0, 0);
}

void Object::calculate_invariants() {

    calculate_centroid();

    invariants(0, 0) = nu(2, 0) + nu(0, 2);
    invariants(0, 1) = pow(nu(2, 0) - nu(0, 2), 2.0) + 4.0 * pow(nu(1, 1), 2.0);
    invariants(0, 2) = pow(nu(3, 0) - 3.0 * nu(1, 2), 2.0) + pow(3.0 * nu(2, 1) - nu(0, 3), 2.0);
    invariants(0, 3) = pow(nu(3, 0) + nu(1, 2), 2.0) + pow(nu(2, 1) + nu(0, 3), 2.0);
    invariants(0, 4) = (nu(3, 0) - 3.0 * nu(1, 2)) * (nu(3, 0) + nu(1, 2)) *
                           (pow(nu(3, 0) + nu(1, 2), 2.0) - 3.0 * pow(nu(2, 1) + nu(0, 3), 2.0)) +
                       (3.0 * nu(2, 1) - nu(0, 3)) * (nu(2, 1) + nu(0, 3)) *
                           (3.0 * (pow(nu(3, 0) + nu(1, 2), 2.0) - pow(nu(2, 1) + nu(0, 3), 2.0)));

    invariants(0, 5) = (nu(2, 0) - nu(0, 2)) * (pow(nu(3, 0) + nu(1, 2), 2.0) -
                                                pow(nu(2, 1) + nu(0, 3), 2.0)) + 4.0 * nu(1, 1) * (nu(3, 0) + nu(1, 2)) *
                             (nu(2, 1) + nu(0, 3));

    invariants(0, 6) = (3.0 * nu(2, 1) - nu(0, 3)) * (nu(3, 0) + nu(1, 2)) * (pow(nu(3, 0) + nu(1, 2), 2.0) -
                                                                              3.0 * pow(nu(2, 1) + nu(0, 3), 2.0)) - (nu(3, 0) - 3.0 * nu(1, 2)) * (nu(2, 1) + nu(0, 3)) *
                             (3.0 * pow(nu(3, 0) + nu(1, 2), 2.0) - pow(nu(2, 1) + nu(0, 3), 2.0));
}

long double Object::calculate_invariants_avg() {
    long double avg = 0;
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 7; j++) {
            avg += invariants(i, j);
        }
    }
    avg / 7;
    return avg;
}

void Object::draw_bounding_box(Mat& mat) {

    //dikey kenarlar
    for (int j = 0; j < 2; j++)
        for (int i = 0; i < height; i++) {

            mat.pixels[p1.get_x_point() + i][p3.get_y_point() + (width - 1) * j].B = 0;
            mat.pixels[p1.get_x_point() + i][p3.get_y_point() + (width - 1) * j].G = 255;
            mat.pixels[p1.get_x_point() + i][p3.get_y_point() + (width - 1) * j].R = 0;
        }

    //yatay kenarlar
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < width; j++) {

            mat.pixels[p1.get_x_point() + (height - 1) * i][p3.get_y_point() + j].B = 0;
            mat.pixels[p1.get_x_point() + (height - 1) * i][p3.get_y_point() + j].G = 255;
            mat.pixels[p1.get_x_point() + (height - 1) * i][p3.get_y_point() + j].R = 0;
        }
}

void Object::cut_object(Mat mat) {

    int height = p2.get_x_point() - p1.get_x_point() + 1;
    int width = p4.get_y_point() - p3.get_y_point() + 1;
    if (width < 0 || height < 0) {
        width = 0;
        height = 0;
    }
    Mat temp(height, width);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            temp(i, j) = mat.pixels[p1.get_x_point() + i][p3.get_y_point() + j];
        }
    }
    imwrite("object" + to_string(label_id) + ".bmp", temp);
}

void::Object::save_invariants() {

    ofstream moments("objects\\object" + to_string(label_id) + ".txt", ios::out);

    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 7; j++) {

            moments << invariants(i, j) << " ";
        }
        moments << endl;
    }

    moments.close();
}
