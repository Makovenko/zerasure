/*
zmatrixfactory.h
Mykyta Makovenko

Fast Erasure Coding for Data Storage: A Comprehensive Study of the Acceleration Techniques
 
Revisiting the Optimization of CauchyReed-Solomon Coding Matrix for Fault-TolerantData Storage

Revision 1.0
April, 2021

Mykyta Makovenko
Department of Industrial & Systems Engineering
Texas A&M University
College Station, TX, 77843
makovenko@tamu.edu

Copyright (c) 2019, Tianli Zhou
          (c) 2021, Mykyta Makovenko
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

- Neither the name of the Texas A&M University nor the names of its
  contributors may be used to endorse or promote products derived
  from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef ZMATRIXFACTORY_H
#define ZMATRIXFACTORY_H
extern "C"{
#include "../Jerasure-1.2A/jerasure.h"
#include "../Jerasure-1.2A/cauchy.h"
}
#include <vector>
#include <memory>

class MatrixFactory {
public:
  MatrixFactory() {}
  virtual ~MatrixFactory() {}
  virtual std::unique_ptr<int> produce(int K, int M, int W, std::vector<int>& data) = 0;
};

class CauchyMatrixFactory: public MatrixFactory {
public:
  CauchyMatrixFactory() {}
  virtual ~CauchyMatrixFactory() {}
  std::unique_ptr<int> produce(int K, int M, int W, std::vector<int>& data) override final {
    return std::unique_ptr<int>(cauchy_xy_coding_matrix(K,M,W,data.data()+K,data.data()));
  }
};

class ExtendedCauchyMatrixFactory: public MatrixFactory {
public:
  ExtendedCauchyMatrixFactory() {}
  virtual ~ExtendedCauchyMatrixFactory() {}
  std::unique_ptr<int> produce(int K, int M, int W, std::vector<int>& data) override final {
    return std::unique_ptr<int>(ecauchy_xy_coding_matrix(K,M,W,data.data()+K,data.data(),data.data()+K+M+K, data.data()+K+M));
  }
};

#endif // ZMATRIXFACTORY_H
