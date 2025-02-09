/*
zcauchy.h
Tianli Zhou
Mykyta Makovenko

Fast Erasure Coding for Data Storage: A Comprehensive Study of the Acceleration Techniques
Revisiting the Optimization of CauchyReed-Solomon Coding Matrix for Fault-Tolerant Data Storage
 
Revision 1.1
April, 2021

Tianli Zhou
Department of Electrical & Computer Engineering
Texas A&M University
College Station, TX, 77843
zhoutianli01@tamu.edu
 
Mykyta Makovenko
Department of Industrial & Systems Engineering
Texas A&M University
College Station, TX, 77843
makovenko@tamu.edu

Copyright (c) 2019, Tianli Zhou
Copyright (c) 2021, Mykyta Makovenko
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
#ifndef ZCAUCHY_H
#define ZCAUCHY_H
#include <stdlib.h>
#include <string.h>
#include <map>
extern "C"{
#include "../Jerasure-1.2A/jerasure.h"
#include "../Jerasure-1.2A/cauchy.h"
}
#include <cassert>
#include <vector>
#include <memory>
#include "zcode.h"
using namespace std;

class MatrixFactory;

class ZCauchy : public ZCode
{
public:
    ZCauchy(int tK, int tM, int tW, vector<int> &arr, bool isSmart, bool isNormal, int packetsize, std::shared_ptr<MatrixFactory> matrixFactory);
    ~ZCauchy();
    void encode_single_chunk(char* data, int len, char**& parities);
    void set_erasure(vector<int> arr);
    void decode_single_chunk(char** &data, char** &parities);
    int* bitmatrix;
private:
    int **en_schedule;
    int **de_schedule;
    char** encode_buf;
    int* erasures;
    map<unsigned long long, int**> de_schedules_map;
};

#endif // ZCAUCHY_H
