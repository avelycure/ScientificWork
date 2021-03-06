#pragma once
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>

void solveEquation(const int size);

void allocateMemory(double **&A,
                    double *&B,
                    double *&X,
                    const int &n);

void freeMemory(double **&A,
                double *&B,
                double *&X,
                const int &n);

bool readData(const std::string fileNameMatrix,
              const std::string fileNameVector,
              double **&matrixA,
              double *&vectorB,
              const int &n);

int WriteVector(std::string fileNameOutput,
                double *&vector,
                const int &n);

bool solveWithGauss(double **&A,
                    double *&B,
                    double *&X,
                    const int &size);

bool matrixIsPrepared(double **&A,
                      double *&B,
                      const int &i,
                      const int &size);

void diagonalizeEquation(double **&A,
                         double *&B,
                         double *&X,
                         const int &size);

int searchMaxInColumn(double **&A,
                      const int &currentMinor,
                      const int &size);

void swapRows(double **&A,
              double *&B,
              const int &i1,
              const int &i2);

bool isDegenerate(double **&A,
                  const int &i,
                  const int &size);
