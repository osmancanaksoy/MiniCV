#ifndef MATRIX_H
#define MATRIX_H
#include <iostream>

using namespace std;

class Matrix {

public:

    long double** matrix;
    uint32_t row;
    uint32_t column;

    Matrix();
    Matrix(uint32_t row, uint32_t column);
    Matrix(uint32_t row, uint32_t column, uint32_t value);
    ~Matrix();

    void remove();
    void resize(uint32_t row, uint32_t column);
    void resize(uint32_t row, uint32_t column, uint32_t value);
    void copyTo(Matrix& B);

    Matrix inverse();
    Matrix transpose();

    void addRow(const Matrix& row);

    Matrix operator *(const Matrix& B);
    Matrix operator *(long double S);
    Matrix operator +(const Matrix& B);
    Matrix operator -(const Matrix& B);

    void operator =(const Matrix& B);
    void operator -=(const Matrix& B);
    void operator +=(const Matrix& B);
    void operator *=(const Matrix& B);
    void operator *=(long double S);

    long double& operator ()(uint32_t row, uint32_t column);
    const long double& operator ()(uint32_t row, uint32_t column)const;

    friend ostream& operator <<(ostream& os, const Matrix&);
    friend istream& operator >>(istream& is, Matrix& matrix);

    friend long double calculateMahalanobisDistance(Matrix& CovarianceMatrix, Matrix& Mean, Matrix& Point);
};

#endif // MATRIX_H
