#include "../include/matrixoperations.h"
#include <opencv2/opencv.hpp>
#include <thrust/extrema.h>
#include <cufft.h>
#include <cuComplex.h>
#include <math.h>

#define N 10

namespace matrix_operations {

__global__ void multiply(int n, double *x, double *y)
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  for (int i = index; i < n; i += stride)
    y[i] = x[i] * y[i];
}

std::vector<double>
multiplyVecKernel( const std::vector<double>& first,
                   const std::vector<double>& second )
{
    double *x = nullptr, *y = nullptr;
    int arraySize = first.size();

    cudaMallocManaged(&x, arraySize*sizeof(double));
    cudaMallocManaged(&y, arraySize*sizeof(double));
    cudaDeviceSynchronize();

    cudaMemcpy(x, &first.front(),
               arraySize*sizeof(double),
               cudaMemcpyHostToDevice
    );
    cudaMemcpy(y, &second.front(),
               arraySize*sizeof(double),
               cudaMemcpyHostToDevice
    );
    cudaDeviceSynchronize();

    int blockSize = 256;
    int numBlocks = (N + blockSize - 1) / blockSize;
    multiply<<<numBlocks, blockSize>>>(arraySize, x, y);
    cudaDeviceSynchronize();
    std::vector<double> retVect(arraySize);
    cudaMemcpy(&retVect.front(), y,
               arraySize*sizeof(double),
               cudaMemcpyDeviceToHost
    );
    cudaDeviceSynchronize();
    // Free memory
    cudaFree(x);
    cudaFree(y);
    cudaDeviceSynchronize();

    return retVect;
}

std::vector<double> calcLinspaceTimes (
        int vectSize, double startT, double endT )
{
    std::vector<double> hostEvenTimes(vectSize);
    thrust::device_vector<double> gpuEvenTimes = hostEvenTimes;
//        std::cout << endT << " " << startT << std::endl;
    double timeGap = (endT - startT) / (vectSize - 1);
    assert(timeGap>0);

    thrust::sequence(gpuEvenTimes.begin(), gpuEvenTimes.end(),
                     hostEvenTimes.front(), timeGap);
    thrust::transform(thrust::device,
                      gpuEvenTimes.begin(), gpuEvenTimes.end(),
                      thrust::make_constant_iterator(startT),
                      gpuEvenTimes.begin(),
                      thrust::plus<double>()
    );

    gpuEvenTimes.back() = endT;
    thrust::copy(gpuEvenTimes.begin(),
                 gpuEvenTimes.end(),
                 hostEvenTimes.begin()
    );
    cudaDeviceSynchronize();
    return (hostEvenTimes);
}

std::vector<double> normalize(const std::vector<double>& vect)
{
    thrust::device_vector<float> dVect = vect;
    double norm = (thrust::reduce(dVect.begin(), dVect.end(), 0))
                            / vect.size();

    using namespace thrust::placeholders;
    thrust::transform(dVect.begin(), dVect.end(),
                      dVect.begin(), _1 /= norm);

    std::vector<double> retVect;
    thrust::copy(dVect.begin(), dVect.end(), retVect.begin());

    cudaDeviceSynchronize();
    return retVect;
}

std::vector<double>
multiplyVec(const std::vector<double>& first,
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

unsigned maxIndex(const std::vector<double>& vect)
{
    thrust::device_vector<double> d_vec = vect;
    thrust::device_vector<double>::iterator iter =
      thrust::max_element(d_vec.begin(), d_vec.end());
    unsigned position = iter - d_vec.begin();
//    float max_val = *iter;
    return position;
}

void gpuCalcFFT(const std::vector<double>& vectMeans,
                std::vector<double>& absVect)
{
//    std::cout << "daw: " << vectMeans.size();
    int n = vectMeans.size();
    double* in = new double[n];
    std::copy(vectMeans.begin(), vectMeans.end(), in);
    double* out = new double[2*n];
    // Allocate the buffer
    cufftDoubleReal *d_in;
    cufftDoubleComplex *d_out;
    unsigned int out_mem_size = sizeof(cufftDoubleComplex)*n;
    unsigned int in_mem_size = sizeof(cufftDoubleReal)*n;
    cudaMalloc((void **)&d_in, in_mem_size);
    cudaMalloc((void **)&d_out, out_mem_size);
    cudaDeviceSynchronize();
    cufftHandle plan;
    cufftResult res = cufftPlan1d(&plan, n, CUFFT_D2Z, 1);
    if (res != CUFFT_SUCCESS) {
        return;
    }
    cudaMemcpy(d_in, in, in_mem_size, cudaMemcpyHostToDevice);
    res = cufftExecD2Z(plan, d_in, d_out);
    if (res != CUFFT_SUCCESS) {
        return;
    }

    cudaMemcpy(out, d_out, out_mem_size, cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    std::vector<double> real, im;
    for(int i =1; i < 2*(n/2+1); i+=2) {
        real.push_back(out[i-1]);
        im.push_back(out[i]);
    }
//    obliczenie abs z tego
    for (int i =0; i< real.size(); ++i)
    {
        absVect.push_back(std::hypot(real.at(i), im.at(i)));
    }
}

}
