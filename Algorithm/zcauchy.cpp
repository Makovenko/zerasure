/*
zcauchy.cpp
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
#include "zcauchy.h"
#include <sys/time.h>
#include "../Search/zelement.h"
#include "zmatrixfactory.hpp"

ZCauchy::ZCauchy(int tK, int tM, int tW, vector<int> &arr, bool isSmart, bool isNormal, int m_packetsize, std::shared_ptr<MatrixFactory> matrixFactory): ZCode(tK,1,1,m_packetsize)
{
    K = tK;
    M = tM;
    W = tW;
    struct timeval t0,t1;
    gettimeofday(&t0,NULL);
    shared_ptr<int> matrix = matrixFactory->produce(K, M, W, arr);
    if(isNormal)
        cauchy_improve_coding_matrix(K,M,W,matrix.get());
    bitmatrix = jerasure_matrix_to_bitmatrix(K,M,W,matrix.get());
    if(!isSmart)
        en_schedule = jerasure_dumb_bitmatrix_to_schedule(K,M,W,bitmatrix);
    else
        en_schedule = jerasure_smart_bitmatrix_to_schedule(K,M,W,bitmatrix);
    gettimeofday(&t1,NULL);
    init_time = diff_us(t0,t1);

    int i = 0;
    int n_cpy = 0,  n_xor = 0;
    while(1)
    {
        if(en_schedule[i][0] == -1)break;
//        printf("%d,%d,%d,%d,%d\n", en_schedule[i][0],en_schedule[i][1],en_schedule[i][2],en_schedule[i][3],en_schedule[i][4]);
        if(en_schedule[i][4] == 0)
            n_cpy ++;
        else
            n_xor ++;
        i++;
    }
    printf(" !!! Cauchy Schedule Len %d, memcpy %d, xor %d, cost %d, init time %d us\n",
           i, n_cpy, n_xor, n_cpy*ZElement::cpy_weight + ZElement::xor_weight*n_xor,init_time);
    printf("Generator Matrix:\n");
    jerasure_print_matrix(matrix.get(),M,K,W);
    printf("Bitmatrix :\n");
    jerasure_print_bitmatrix(bitmatrix,M*W,K*W,W);
    encode_buf = malloc2d(K, W*packetsize);
    erasures = (int*) malloc(sizeof(int) * (K+M));
    blocksize =  packetsize * K * W;
}

ZCauchy::~ZCauchy()
{
    free(bitmatrix);
    free2d(encode_buf, K);
    if(erasures != NULL)
        free(erasures);
}

void ZCauchy::encode_single_chunk(char* data, int len, char**& parities)
{
    int i,j;
    assert(data != NULL);
    assert(parities != NULL);
    assert(len ==  packetsize * K * W);
    for(i = 0;i<K;i++)
    {
        memcpy(encode_buf[i], data + i*packetsize * W, packetsize * W);
    }
    jerasure_schedule_encode(K,M,W,en_schedule,encode_buf,parities, W*packetsize, packetsize);
}

void ZCauchy::set_erasure(vector<int> arr)
{
    unsigned long long pattern = 0LL;
    int i;
    for(i = 0;i<arr.size();i++)
    {
        erasures[i] = arr[i];
        pattern |= 1LL << arr[i];
    }
    erasures[i] = -1;
//    for(i = 0;i<arr.size()+1;i++)
//        printf("%d ", erasures[i]);
//    printf("\n");

    if(de_schedules_map.find(pattern) != de_schedules_map.end())
    {
        printf("found decode schedule for pattern %llu\n",pattern);
        de_schedule = de_schedules_map[pattern];
    }
    else
    {
        printf("new decode schedule for pattern %llu\n",pattern);
        de_schedule = jerasure_generate_decoding_schedule(K,M,W,bitmatrix,erasures,1);
        assert(de_schedule != NULL);
        de_schedules_map[pattern] = de_schedule;
    }
}

void ZCauchy::decode_single_chunk(char **&data, char **&parities)
{
    char **ptrs;
    ptrs = set_up_ptrs_for_scheduled_decoding(K, M, erasures, data, parities);
    jerasure_do_scheduled_operations(ptrs, de_schedule, packetsize);
    free(ptrs);
}
