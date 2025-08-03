#include "../Implementation/MatrixMult.h"

#include <iostream>

template <typename numeric>
void printMat(matrix<numeric>& X){
    for (int i = 0; i < X.m; i++){
        int row = X.rowIdx(i);
        for (int j = 0; j < X.n; j++){
            int idx = row + j;
            if (X[idx] < 10)
                std::cout << ' ';
            std::cout << X[idx] << ' ';
        }
        std::cout << '\n';
    }
}

int main(){
    matrix<float> A(16, 16, 0), B(16, 16, 2);

    for (int i = 0; i < A.n; i++){
        A[i*A.n + i] = 1;
    }

    matrix<float> C = matrixMultiply(A, B, 2, 2);

    std::cout << "Matrix A: \n";
    printMat(A);
    std::cout << "Matrix B: \n";
    printMat(B);
    std::cout << "Matrix C: \n";
    printMat(C);

        

    return 0;
}