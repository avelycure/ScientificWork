#include "SecondOrderTriangleFE.hpp"

void solveWithSecondOrderTriangleFE(TriangleContributionMatrixSecondOrder *&contributionMatrix,
                                    TriangleRightPartSecondOrder *&localRigthParts,
                                    Point **&coordinateMesh,
                                    double **&matrixPressure,
                                    double *&rightPart,
                                    SystemParameters &systemParameters)
{
    const int MATRIX_PRESSURE_SIZE = systemParameters.n * systemParameters.n;
    const int MATRIX_CONTRIBUTION_SIZE = (systemParameters.n - 1) * (systemParameters.n - 1) / 2;

    initMatrix(matrixPressure, MATRIX_PRESSURE_SIZE, MATRIX_PRESSURE_SIZE);
    for (int i = 0; i < MATRIX_PRESSURE_SIZE; i++)
        for (int j = 0; j < MATRIX_PRESSURE_SIZE; j++)
            matrixPressure[i][j] = 0.0;

    initMesh(coordinateMesh, systemParameters);

    contributionMatrix = new TriangleContributionMatrixSecondOrder[MATRIX_CONTRIBUTION_SIZE];

    localRigthParts = new TriangleRightPartSecondOrder[MATRIX_CONTRIBUTION_SIZE];

    initVector(rightPart, MATRIX_PRESSURE_SIZE);

    createLocalMatrixForEveryElementQuadraticTriangles(contributionMatrix,
                                                       coordinateMesh,
                                                       localRigthParts,
                                                       systemParameters);

    createGlobalPressureMatrixQuadraticTriangles(matrixPressure,
                                                 contributionMatrix,
                                                 rightPart,
                                                 localRigthParts,
                                                 systemParameters.n);

    addBorderConditionsOnPressureValues(matrixPressure,
                                        rightPart,
                                        systemParameters.n,
                                        MATRIX_PRESSURE_SIZE,
                                        systemParameters.LOW_BORDER,
                                        systemParameters.HIGH_BORDER);

    outputPressureMatrix(matrixPressure, MATRIX_PRESSURE_SIZE);
}

void createLocalMatrixForEveryElementQuadraticTriangles(TriangleContributionMatrixSecondOrder *&contributionMatrixParam,
                                                        Point **&coordinateMeshParam,
                                                        TriangleRightPartSecondOrder *&rightPartParam,
                                                        SystemParameters &systemParameters)
{
    int n = systemParameters.n;
    int finiteElementNumber = -1;

    for (int i = 0; i < n - 2; i += 2)
        for (int j = 0; j < n - 2; j += 2)
        {
            finiteElementNumber++;
            createLocalContributionMatrixForQuardaticTriangle(contributionMatrixParam[finiteElementNumber],
                                                              coordinateMeshParam[i][j],
                                                              coordinateMeshParam[i + 1][j],
                                                              coordinateMeshParam[i + 2][j],
                                                              coordinateMeshParam[i + 1][j + 1],
                                                              coordinateMeshParam[i][j + 2],
                                                              coordinateMeshParam[i][j + 1],
                                                              rightPartParam[finiteElementNumber],
                                                              systemParameters);
            finiteElementNumber++;
            createLocalContributionMatrixForQuardaticTriangle(contributionMatrixParam[finiteElementNumber],
                                                              coordinateMeshParam[i + 2][j + 2],
                                                              coordinateMeshParam[i + 1][j + 2],
                                                              coordinateMeshParam[i][j + 2],
                                                              coordinateMeshParam[i + 1][j + 1],
                                                              coordinateMeshParam[i + 2][j],
                                                              coordinateMeshParam[i + 2][j + 1],
                                                              rightPartParam[finiteElementNumber],
                                                              systemParameters);
        }
}

void createLocalContributionMatrixForQuardaticTriangle(TriangleContributionMatrixSecondOrder localMatrix,
                                                       Point pointI,
                                                       Point pointJ,
                                                       Point pointK,
                                                       Point pointL,
                                                       Point pointM,
                                                       Point pointN,
                                                       TriangleRightPartSecondOrder localRightPart,
                                                       SystemParameters &systemParameters)
{
    double hMin = systemParameters.hMin;
    double k = systemParameters.k;

    double k1 = (pointM.getX() - pointK.getX()) / (pointM.getY() - pointK.getY());
    double k2 = pointK.getX() - pointK.getY() * ((pointM.getX() - pointK.getX()) / (pointM.getY() - pointK.getY()));

    double A1 = pow(hMin, 3.0);
    double A2 = 3.0 * pow(hMin, 2.0) * k;
    double A3 = 3.0 * hMin * pow(k, 2.0);
    double A4 = pow(k, 3.0);

    double s1 = (1.0 / 1.0) * (pointM.getY() - pointI.getY());
    double s2 = (1.0 / 2.0) * (pow(pointM.getY(), 2.0) - pow(pointI.getY(), 2.0));
    double s3 = (1.0 / 3.0) * (pow(pointM.getY(), 3.0) - pow(pointI.getY(), 3.0));
    double s4 = (1.0 / 4.0) * (pow(pointM.getY(), 4.0) - pow(pointI.getY(), 4.0));
    double s5 = (1.0 / 5.0) * (pow(pointM.getY(), 5.0) - pow(pointI.getY(), 5.0));
    double s6 = (1.0 / 6.0) * (pow(pointM.getY(), 6.0) - pow(pointI.getY(), 6.0));
    double s7 = (1.0 / 7.0) * (pow(pointM.getY(), 7.0) - pow(pointI.getY(), 7.0));
    double s8 = (1.0 / 8.0) * (pow(pointM.getY(), 8.0) - pow(pointI.getY(), 8.0));

    double zi = pointI.getX();

    double c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, c18, c19;

    setCoefficients(c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, c18, c19,
                    s1, s2, s3, s4, s5, s6, s7,
                    k1, k2,
                    A1, A2, A3, A4,
                    zi);

    double R6 = 6.0 * systemParameters.mu * systemParameters.L * systemParameters.U * k /
                (systemParameters.Hn * systemParameters.Hn * systemParameters.pMin);

    double *a = new double[6];
    double *b = new double[6];
    double *c = new double[6];
    double *d = new double[6];
    double *e = new double[6];
    double *f = new double[6];

    setFormFunctionsCoefficients(a, b, c, d, e, f, pointI, pointJ, pointK, pointM, pointN);

    double valR1, valR2, valR3, valR4, valR5;
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 6; j++)
        {
            valR1 = c[j] * (c[i] * c1 + f[i] * c3 + e[i] * c13);
            valR2 = e[j] * (b[i] * c5 + d[i] * c8 + f[i] * c9 + c[i] * c14 + e[i] * c18);
            valR3 = f[j] * (c[i] * c4 + f[i] * c7 + e[i] * c11);
            valR4 = b[j] * (b[i] * c2 + e[i] * c6 + d[i] * c15);
            valR5 = d[j] * (e[i] * c10 + b[i] * c16 + d[i] * c19);
            localMatrix.setElement(i, j, valR1 + valR2 + valR3 + valR4 + valR5);
        }

    for (int i = 0; i < 6; i++)
        localRightPart.setElement(i, -R6 * (a[i] * k1 * s2 + c[i] * k2 * s2 + b[i] * k1 * k2 * s2 + 0.5 * e[i] * k2 * k2 * s2 +
                                            d[i] * k1 * k2 * k2 * s2 + c[i] * k1 * s3 + 0.5 * b[i] * k1 * k1 * s3 +
                                            f[i] * k2 * s3 + e[i] * k1 * k2 * s3 + d[i] * k1 * k1 * k2 * s3 +
                                            f[i] * k1 * s4 + 0.5 * e[i] * k1 * k1 * s4 + d[i] * k1 * k1 * k1 * s4 / 3.0 +
                                            a[i] * s1 * (k2 - zi) - c[i] * s2 * zi - f[i] * s3 * zi - 0.5 * e[i] * s2 * zi * zi +
                                            0.5 * b[i] * s1 * (k2 * k2 - zi * zi) + d[i] * s1 * (k2 * k2 * k2 - zi * zi * zi) / 3.0));

    delete[] a;
    delete[] b;
    delete[] c;
    delete[] d;
    delete[] e;
    delete[] f;
}

void createGlobalPressureMatrixQuadraticTriangles(double **&matrixPressure,
                                                  TriangleContributionMatrixSecondOrder *&contributionMatrix,
                                                  double *&rightPartParam,
                                                  TriangleRightPartSecondOrder *&localRightPartsParam,
                                                  int n)
{
    int *globalNodeNumbersIJK = new int[6];
    int finiteElementNumber = 0;

    for (int i = 0; i < n - 2; i += 2)
        for (int j = 0; j < n - 2; j += 2)
        {
            //for top triangle
            /*i*/ globalNodeNumbersIJK[0] = i * n + j;
            /*j*/ globalNodeNumbersIJK[1] = globalNodeNumbersIJK[0] + n;
            /*k*/ globalNodeNumbersIJK[2] = globalNodeNumbersIJK[1] + n;
            /*l*/ globalNodeNumbersIJK[3] = globalNodeNumbersIJK[1] + 1;
            /*m*/ globalNodeNumbersIJK[4] = globalNodeNumbersIJK[0] + 2;
            /*n*/ globalNodeNumbersIJK[5] = globalNodeNumbersIJK[0] + 1;

            for (int i1 = 0; i1 < 6; i1++)
                for (int i2 = 0; i2 < 6; i2++)
                    matrixPressure[globalNodeNumbersIJK[i1]][globalNodeNumbersIJK[i2]] +=
                        contributionMatrix[finiteElementNumber].matrix[i1][i2];

            for (int i1 = 0; i1 < 6; i1++)
                rightPartParam[globalNodeNumbersIJK[i1]] += localRightPartsParam[finiteElementNumber].getElement(i1);

            finiteElementNumber++;

            //for bottom triangle
            /*i*/ globalNodeNumbersIJK[0] = globalNodeNumbersIJK[2] + 2;
            /*j*/ globalNodeNumbersIJK[1] = globalNodeNumbersIJK[0] - n;
            /*k*/ globalNodeNumbersIJK[2] = globalNodeNumbersIJK[1] - n;
            /*l*/ globalNodeNumbersIJK[3] = globalNodeNumbersIJK[1] - 1;
            /*m*/ globalNodeNumbersIJK[4] = globalNodeNumbersIJK[0] - 2;
            /*n*/ globalNodeNumbersIJK[5] = globalNodeNumbersIJK[0] - 1;

            for (int i1 = 0; i1 < 6; i1++)
                for (int i2 = 0; i2 < 6; i2++)
                    matrixPressure[globalNodeNumbersIJK[i1]][globalNodeNumbersIJK[i2]] +=
                        contributionMatrix[finiteElementNumber].matrix[i1][i2];

            for (int i1 = 0; i1 < 6; i1++)
                rightPartParam[globalNodeNumbersIJK[i1]] += localRightPartsParam[finiteElementNumber].getElement(i1);

            finiteElementNumber++;
        }
}