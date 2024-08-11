#ifndef IMIO_H
#define IMIO_H

#include "bmpreader.h"
#include "bmpwriter.h"
#include "image.h"
#include "mat.h"

Mat imread(const string& path);
void imwrite(const string& path, Mat& mat);

#endif // IMIO_H
