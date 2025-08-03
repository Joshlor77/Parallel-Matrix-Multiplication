#include "ThreadPool.h"

#include <memory>
#include <vector>
#include <exception>
#include <future>

#include <iostream>

//m x n matrix. m rows, n columns. Non-sparse
template <typename numeric>
struct matrix {
    std::vector<numeric> mat;
    int m, n;

    matrix() = default;
    matrix(int m_, int n_, int fillValue = 0):
        mat(std::vector<numeric>(m_ * n_, fillValue)), m(m_), n(n_)
    {}

    numeric& operator[](int i){
        return mat[i];
    }
    int inline rowIdx(int row){ //Row part of the linear indexing for a Non-sparse 2d array
        return row * n;
    }
};

struct IncompatibleMatrixDimensions {};

template <typename numeric>
matrix<numeric> matrixMultiply(matrix<numeric>& A, matrix<numeric>& B, int chunkSizeX=32, int chunkSizeY=32){
    if (A.n != B.m)
        throw IncompatibleMatrixDimensions();
    matrix<numeric> C(A.m, B.n, 0);
    ThreadPool pool;

    auto multiplyChunk = [&A, &B, &C](int startX, int endX, int startY, int endY){
        for (int rowC = startY; rowC < endY; rowC++){
            int rowCIdx = C.rowIdx(rowC);
            for (int colC = startX; colC < endX; colC++){
                int entryCIdx = rowCIdx + colC;
                int rowAIdx = A.rowIdx(rowC);
                int colBIdx = colC;
                for (int i = 0; i < A.n; i++){
                    C[entryCIdx] += A[rowAIdx + i] * B[B.rowIdx(colC) + colBIdx];
                }
            }
        }
    };

    int chunkX = (C.m - 1) / chunkSizeX + 1;
    int chunkY = (C.n - 1) / chunkSizeY + 1;
    std::vector<std::future<void>> futures(chunkX * chunkY);

    for(int i = 0; i < chunkX; i++){
        int x = i * chunkSizeX;
        int futX = i * chunkX;
        for (int j = 0; j < chunkY; j++){
            int y = j *chunkSizeY;
            futures[futX + j] = pool.submit(std::bind(multiplyChunk, x, x + chunkSizeX, y, y + chunkSizeY));
        }
    }
    for (auto& fut : futures){
        fut.get();
    }

    return C;
};
