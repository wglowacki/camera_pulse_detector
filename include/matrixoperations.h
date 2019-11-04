#ifndef MATRIXOPERATIONS_H
#define MATRIXOPERATIONS_H

#include <vector>

#include <thrust/reduce.h>
#include <thrust/functional.h>
#include <thrust/transform.h>
#include <thrust/iterator/constant_iterator.h>
#include <thrust/sequence.h>

namespace matrix_operations
{
    std::vector<double> calcLinspaceTimes(int vectSize, double startT, double endT);
    std::vector<double> normalize(const std::vector<double>& vect);
    std::vector<double> multiplyVec(const std::vector<double>& first,
                                    const std::vector<double>& second);
    std::vector<double> multiplyVecKernel(const std::vector<double>& first,
                                    const std::vector<double>& second);
};

#endif // MATRIXOPERATIONS_H
