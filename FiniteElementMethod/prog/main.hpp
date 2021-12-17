#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <cmath>
#include <sstream>
#include <algorithm>
#include "common/classes/mesh/Point.hpp"
#include "common/classes/contribution_matrix/ContributionMatrix.hpp"
#include "common/classes/system/SystemParameters.hpp"
#include "common/classes/contribution_matrix/RectangleCommon.hpp"
#include "libs/single_include/nlohmann/json.hpp"
using namespace std;

const string FILE_PARAMETERS_NAME = "data/fem_input/initial_conditions/systemParameters.json";
const string FILE_SYSTEM_NAME = "data/fem_input/initial_conditions/systemNum.json";
const string H_CONST = "hConst";
const string H_LINEAR = "hLinear";

//Rectangle
void createLocalMatrixForEveryRectangleElement(RectnangleContributionMatrix *&contributionMatrixParam,
                                               Point **&coordinateMeshParam,
                                               RectnangleRightPart *&rightPartParam,
                                               SystemPatemeters &systemParameters);

void createLocalContributionMatrixForRectangleElement(RectnangleContributionMatrix &localMatrix,
                                                      Point pointI,
                                                      Point pointJ,
                                                      Point pointK,
                                                      Point pointM,
                                                      RectnangleRightPart &localRightPart,
                                                      SystemPatemeters &systemParameters);

void solveWithRectangleFiniteElements(RectnangleContributionMatrix *&contributionMatrix,
                                      RectnangleRightPart *&localRigthParts,
                                      Point **&coordinateMesh,
                                      double **&matrixPressure,
                                      double *&rightPart,
                                      SystemPatemeters &systemParameters);

void createGlobalPressureMatrixForRectangleElement(
    double **&matrixPressure, RectnangleContributionMatrix *&contributionMatrix,
    double *&rightPartParam, RectnangleRightPart *&localRightPartsParam, int n);

void addBorderConditionsForRectnangleElements(double **&matrixResult,
                                              double *&rightPartParam,
                                              int n,
                                              int MATRIX_PRESSURE_SIZE,
                                              int OTHER_BORDER,
                                              int DOWN_BORDER);

void addBorderConditionsForRectnangleElementsToLeftAndRight(double **&matrixResult,
                                       int n,
                                       double h,
                                       int MATRIX_PRESSURE_SIZE,
                                       double TOP_BORDER,
                                       double BOTTOM_BORDER);

//Basic funcs
void initMatrix(double **&matrix, int row, int column);
void displayMatrix(double **matrix, int row, int column);
void displayMesh(Point **coordinateMesh, int n);
void displayAllLocalMatrixes(ContributionMatrix *&ContributionMatrixParam, int n);
void outputPressureMatrix(double **matrixPressure, int MATRIX_PRESSURE_SIZE);
void displayVector(double *mVector, int n);
void initVector(double *&p, int n);
void initMesh(Point **&coordinateMesh, SystemPatemeters &systemParameters);
void initRightPart(RightPart *&localRigthParts, int MATRIX_CONTRIBUTION_SIZE);
void initContributionMatrix(ContributionMatrix *&contributionMatrix, int MATRIX_CONTRIBUTION_SIZE);
void readSystemParameters(SystemPatemeters &systemParameters, string &method);

//Finite elements method funcs
double countArea(Point pointI, Point pointJ, Point pointK);
void createLocalContributionMatrixForHConst(ContributionMatrix localMatrix, Point pointI, Point pointJ, Point pointK);
void createLocalMatrixForEveryElementHConst(ContributionMatrix *&contributionMatrixParam, Point **&coordinateMeshParam, int n);
void addBorderConditionsHConst(double **&matrixResult, int n, int MATRIX_PRESSURE_SIZE, int OTHER_BORDER, int DOWN_BORDER);
void createGlobalPressureMatrixHConst(double **&matrixPressure, ContributionMatrix *&contributionMatrix, int n);
int solveWithHConst(ContributionMatrix *&contributionMatrix,
                    Point **&coordinateMesh,
                    double **&matrixPressure,
                    SystemPatemeters &systemParameters);

int createLocalContributionMatrixForHLinearTop(ContributionMatrix localMatrix,
                                               Point pointI, Point pointJ, Point pointK,
                                               RightPart localRightPart, SystemPatemeters &systemParameters);
int createLocalContributionMatrixForHLinearBottom(ContributionMatrix localMatrix,
                                                  Point pointI, Point pointJ, Point pointK,
                                                  RightPart localRightPart, SystemPatemeters &systemParameters);
void createLocalMatrixForEveryElementHLinear(ContributionMatrix *&contributionMatrixParam,
                                             Point **&coordinateMeshParam,
                                             RightPart *&rightPartParam,
                                             SystemPatemeters &systemParameters);
void createGlobalPressureMatrixHLinear(double **&matrixPressure, ContributionMatrix *&contributionMatrix,
                                       double *&rightPartParam, RightPart *&localRightPartsParam, int n);
void addBorderConditionsHLinear(double **&matrixResult, double *&rightPartParam, int n,
                                int MATRIX_PRESSURE_SIZE, int OTHER_BORDER, int DOWN_BORDER);

/**
 * Right border == left border
 * */
void addBorderConditionsToLeftAndRight(double **&matrixResult,
                                       int n,
                                       double h,
                                       int MATRIX_PRESSURE_SIZE,
                                       double TOP_BORDER,
                                       double BOTTOM_BORDER);
int solveWithHLinearWithDerBC(ContributionMatrix *&contributionMatrix,
                              RightPart *&localRigthParts,
                              Point **&coordinateMesh,
                              double **&matrixPressure,
                              double *&rightPart,
                              SystemPatemeters &systemParameters);

int solveWithHConstBCLR(ContributionMatrix *&contributionMatrix,
                        Point **&coordinateMesh,
                        double **&matrixPressure,
                        SystemPatemeters &systemParameters);

int solveWithHLinear(ContributionMatrix *&contributionMatrix,
                     RightPart *&localRigthParts,
                     Point **&coordinateMesh,
                     double **&matrixPressure,
                     double *&rightPart,
                     SystemPatemeters &systemParameters);

int solveEquation(const int size);

int AllocateMemory(double **&A, double *&B, double *&X1, const int &n);
int ReadData(const string fileNameMatrix, const string fileNameVector, double **&matrixA, double *&vectorB, const int &n);

int WriteVector(string fileNameOutput, double *&vector, const int &n);

bool GaussMethod(double **&A, double *&B, double *&X, const int &size);

bool MatrixIsPrepared(double **&A, double *&B, const int &i, vector<tuple<int, int>> &permutations, const int &size);

void DiagonalizeEquation(double **&A, double *&B, double *&X, const int &size, vector<tuple<int, int>> &permutations);

tuple<int, int> SearchMax(double **&A, const int &currentMinor, const int &size);

void SwapRows(double **&A, double *&B, const int &i1, const int &i2);
void SwapColomns(double **&A, const int &j1, const int &j2, const int &size);

bool isDegenerate(double **&A, const int &i, const int &size);

int FreeMemory(double **&A, double *&B, double *&X1, const int &n);