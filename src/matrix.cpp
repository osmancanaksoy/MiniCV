#include "matrix.h"
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

Matrix::Matrix() {

    row = 3;
    column = 3;

    matrix = new long double* [row];

    for (int i = 0; i < row; i++) {

        matrix[i] = new long double[column];

        for (int j = 0; j < column; j++) {

            matrix[i][j] = 0;
        }
    }
}

Matrix::Matrix(uint32_t row, uint32_t column) {

    this->row = row;
    this->column = column;

    matrix = new long double* [row];

    for (int i = 0; i < row; i++) {

        matrix[i] = new long double[column];

        for (int j = 0; j < column; j++)
            matrix[i][j] = 0;
    }
}

Matrix::Matrix(uint32_t row, uint32_t column, uint32_t value) {

    this->row = row;
    this->column = column;

    matrix = new long double* [row];

    for (int i = 0; i < row; i++) {

        matrix[i] = new long double[column];

        for (int j = 0; j < column; j++)
            matrix[i][j] = value;
    }
}

Matrix::~Matrix() {

//    for (int i = 0; i < row; i++)
//        delete[] matrix[i];

//    delete[] matrix;
}

void Matrix::remove() {

    for (int i = 0; i < row; i++)
        delete[] matrix[i];

    delete[] matrix;
}

void Matrix::resize(uint32_t row, uint32_t column) {

    remove();

    this->row = row;
    this->column = column;

    matrix = new long double* [row];

    for (int i = 0; i < row; i++) {

        matrix[i] = new long double[column];

        for (int j = 0; j < column; j++)
            matrix[i][j] = 0;
    }
}

void Matrix::resize(uint32_t row, uint32_t column, uint32_t value) {

    remove();

    this->row = row;
    this->column = column;

    matrix = new long double* [row];

    for (int i = 0; i < row; i++) {

        matrix[i] = new long double[column];

        for (int j = 0; j < column; j++)
            matrix[i][j] = (long double)value;
    }
}

void Matrix::copyTo(Matrix& B) {

    if (row != B.row || column != B.column) {

        //cout<<"Kopyalama icin gecersiz boyutlar!.. (Deep Copy)"<<endl;

        B.resize(row, column);

    }

    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            B(i, j) = matrix[i][j];

    //cout<<"Kopyalandi..."<<endl;
}

Matrix Matrix::inverse() {

    int i, j, k, n;

    long double ratio, a;

    n = row;

    Matrix A(n, 2 * n);
    Matrix B(row, column);

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            A(i, j) = matrix[i][j];

    for (i = 0; i < n; i++) {
        for (j = n; j < 2 * n; j++) {

            if (i == (j - n))
                A(i, j) = 1.0;

            else
                A(i, j) = 0.0;
        }
    }

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {

            if (i != j) {

                ratio = A(j, i) / A(i, i);

                for (k = 0; k < 2 * n; k++) {

                    A(j, k) -= ratio * A(i, k);
                }
            }
        }
    }

    for (i = 0; i < n; i++) {

        a = A(i, i);

        for (j = 0; j < 2 * n; j++) {

            A(i, j) /= a;
        }
    }

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            B(i, j) = A(i, j + n);

    return B;
}

Matrix Matrix::transpose() {

    Matrix B(column, row);

    for (int i = 0; i < column; i++)
        for (int j = 0; j < row; j++)
            B(i, j) = matrix[j][i];

    return B;
}

void Matrix::addRow(const Matrix& B) {

    if (matrix[0][0] == 0 && matrix[0][1] == 0) {

        matrix[0][0] = B(0, 0);
        matrix[0][1] = B(0, 1);

        return;
    }

    Matrix temp(row, column);

    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            temp(i, j) = matrix[i][j];

    resize(row + 1, column);

    bool state = false;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {

            if (i == row - 1) {

                for (int k = 0; k < column; k++)
                    matrix[row - 1][k] = B(0, k);
                state = true;
                break;


            }

            matrix[i][j] = temp(i, j);
        }

        if (state)
            break;


    }
}

Matrix Matrix::operator *(const Matrix& B) {

    if (column != B.row) {

        cout << "Carpma icin gecersiz boyutlar!.." << endl;

        exit(EXIT_FAILURE);
    }

    Matrix C(row, B.column);

    long double sum;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < B.column; j++) {

            sum = 0;

            for (int k = 0; k < column; k++) {

                sum += matrix[i][k] * B(k, j);
            }

            C(i, j) = sum;
        }
    }


    return C;
}

Matrix Matrix::operator *(long double S) {

    Matrix A(row, column);

    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            A(i, j) = matrix[i][j] * S;

    return A;
}

Matrix Matrix::operator +(const Matrix& B) {

    if (row != B.row || column != B.column) {

        cout << "Toplama icin gecersiz boyutlar!.." << endl;

        exit(EXIT_FAILURE);
    }

    Matrix C(row, column);

    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            C(i, j) = matrix[i][j] + B(i, j);


    return C;
}

Matrix Matrix::operator -(const Matrix& B) {

    if (row != B.row || column != B.column) {

        cout << "Cikarma icin gecersiz boyutlar!.." << endl;

        exit(EXIT_FAILURE);
    }

    Matrix C(row, column);

    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            C(i, j) = matrix[i][j] - B(i, j);

    return C;
}

void Matrix::operator =(const Matrix& B) {

    if (row != B.row || column != B.column) {

        //cout<<"Kopyalama icin gecersiz boyutlar!.."<<endl;

        resize(B.row, B.column);
    }

    for (int i = 0; i < B.row; i++)
        for (int j = 0; j < B.column; j++)
            matrix[i][j] = B(i, j);

    //cout<<"Kopyalandi..."<<endl;
}

void Matrix::operator -=(const Matrix& B) {

    if (row != B.row || column != B.column) {

        cout << "-= icin gecersiz boyutlar!.." << endl;

        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            matrix[i][j] -= B(i, j);

}

void Matrix::operator +=(const Matrix& B) {

    if (row != B.row || column != B.column) {

        cout << "+= icin gecersiz boyutlar!.." << endl;

        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            matrix[i][j] += B(i, j);

}

void Matrix::operator *=(Matrix const& B) {

    if (column != B.row) {

        cout << "*= icin gecersiz boyutlar!.." << endl;

        exit(EXIT_FAILURE);
    }

    Matrix C(row, B.column);

    long double sum;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < B.column; j++) {

            sum = 0;

            for (int k = 0; k < column; k++) {

                sum += matrix[i][k] * B(k, j);
            }

            C(i, j) = sum;
        }
    }

    resize(row, B.column);

    C.copyTo(*this);
}

void Matrix::operator *=(long double S) {

    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            matrix[i][j] *= S;

}



long double& Matrix::operator ()(uint32_t row, uint32_t column) {

    return matrix[row][column];
}

const long double& Matrix::operator ()(uint32_t row, uint32_t column)const {

    return matrix[row][column];
}


ostream& operator<<(ostream& os, const Matrix& matrix) {

    for (int i = 0; i < matrix.row; i++) {
        for (int j = 0; j < matrix.column; j++) {

            os << matrix(i, j) << " ";
        }
        cout << endl;
    }

    return os;
}

istream& operator >>(istream& is, Matrix& matrix) {

    cout << "Matrix" << " " << matrix.row << "x" << matrix.column << endl;

    for (int i = 0; i < matrix.row; i++) {
        for (int j = 0; j < matrix.column; j++) {

            cout << "matrix[" << i << "][" << j << "]=";

            is >> matrix(i, j);
        }
    }

    return is;
}

long double calculateMahalanobisDistance(Matrix& CovarianceMatrix, Matrix& Mean, Matrix& Point) {

    Matrix E, EI, M, P, PT;


    E = CovarianceMatrix;
    EI = E.inverse();
    M = Mean;
    P = Point - Mean;
    PT = P.transpose();


    long double distance = ((P * EI) * PT)(0, 0);

    long double a = sqrt(fabs(distance));

    return a;
}





