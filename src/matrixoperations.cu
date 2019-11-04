#include "../include/matrixoperations.h"
#include <opencv2/opencv.hpp>

#define N 10

namespace matrix_operations {

__global__ void multiply(int n, double *x, double *y)
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  for (int i = index; i < n; i += stride)
    y[i] = x[i] * y[i];
}

std::vector<double> multiplyVecKernel(const std::vector<double>& first,
                                const std::vector<double>& second)
{
    double *x, *y;
    // Allocate Unified Memory – accessible from CPU or GPU
    int arraySize = first.size();

    cudaMallocManaged(&x, arraySize*sizeof(double));
    cudaMallocManaged(&y, arraySize*sizeof(double));
    cudaDeviceSynchronize();

    cudaMemcpy(x, &first.front(), arraySize*sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(y, &second.front(),arraySize*sizeof(double), cudaMemcpyHostToDevice);
    cudaDeviceSynchronize();

    int blockSize = 256;
    int numBlocks = (N + blockSize - 1) / blockSize;
    multiply<<<numBlocks, blockSize>>>(arraySize, x, y);

    // Wait for GPU to finish before accessing on host
    cudaDeviceSynchronize();
    std::vector<double> retVect(arraySize);
    cudaMemcpy(&retVect.front(), y, arraySize*sizeof(double), cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    // Free memory
    cudaFree(x);
    cudaFree(y);
    cudaDeviceSynchronize();

    return retVect;
}

    std::vector<double> calcLinspaceTimes(int vectSize, double startT, double endT)
    {
        std::vector<double> hostEvenTimes(vectSize);
        thrust::device_vector<double> gpuEvenTimes = hostEvenTimes;
//        std::cout << endT << " " << startT << std::endl;
        double timeGap = (endT - startT) / (vectSize - 1);
        assert(timeGap>0);
        thrust::sequence(gpuEvenTimes.begin(), gpuEvenTimes.end(), hostEvenTimes.front(), timeGap);

        thrust::transform(thrust::device, gpuEvenTimes.begin(), gpuEvenTimes.end(),
                          thrust::make_constant_iterator(startT),
                          gpuEvenTimes.begin(),
                          thrust::plus<double>());

        gpuEvenTimes.back() = endT;
        thrust::copy(gpuEvenTimes.begin(), gpuEvenTimes.end(), hostEvenTimes.begin());
//        std::for_each(hostEvenTimes.begin(), hostEvenTimes.end(), [](double var) {
//           std::cout << var << std::endl;
//        });
        cudaDeviceSynchronize();
        return (hostEvenTimes);
    }

    std::vector<double> normalize(const std::vector<double>& vect)
    {
        thrust::device_vector<float> dVect = vect;
        double norm = (thrust::reduce(dVect.begin(), dVect.end(), 0)) / vect.size();

        using namespace thrust::placeholders;
        thrust::transform(dVect.begin(), dVect.end(), dVect.begin(), _1 /= norm);

        std::vector<double> retVect;
        thrust::copy(dVect.begin(), dVect.end(), retVect.begin());

        cudaDeviceSynchronize();
        return retVect;
    }

    std::vector<double> multiplyVec(const std::vector<double>& first,
                                    const std::vector<double>& second)
    {
        thrust::device_vector<double> V1 = first;
        thrust::device_vector<double> V2 = second;
        thrust::device_vector<float> V3(first.size());

        thrust::transform(V1.begin(), V1.end(), V2.begin(), V3.begin(),
                          thrust::multiplies<double>());

        std::vector<double> retVect;
        thrust::copy(V3.begin(), V3.end(), retVect.begin());
        return retVect;
    }



}
