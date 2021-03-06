/***
Copyright (c) 2014-2015, Niall Emmart.  All rights reserved.
Copyright (c) 2015, NVIDIA CORPORATION.  All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
***/
namespace xmp {
  __device__ __forceinline__ void _add(RegMP r, RegMP a, RegMP b, bool carryIn, bool carryOut) {
    if(r.length()!=a.length() || r.length()!=b.length()) RMP_ERROR("_add() - length mismatch");

    PTXChain chain(r.length(), carryIn, carryOut);
    #pragma unroll
    for(int index=0;index<r.length();index++)
      chain.ADD(r[index], a[index], b[index]);
    chain.end();
  }

  __device__ __forceinline__ void add(RegMP r, RegMP a, RegMP b) {
    if(r.length()!=a.length() || r.length()!=b.length()) RMP_ERROR("add() - length mismatch");

    PTXChain chain(r.length());
    #pragma unroll
    for(int index=0;index<r.length();index++)
      chain.ADD(r[index], a[index], b[index]);
    chain.end();
  }

  __device__ __forceinline__ bool add_cc(RegMP r, RegMP a, RegMP b) {
    uint32_t result, zero=0;

    if(r.length()!=a.length() || r.length()!=b.length()) RMP_ERROR("add_cc() - length mismatch");

    PTXChain chain(r.length()+1);
    #pragma unroll
    for(int index=0;index<r.length();index++)
      chain.ADD(r[index], a[index], b[index]);
    chain.ADD(result, zero, zero);
    chain.end();
    return result;
  }

  __device__ __forceinline__ void _add_ui(RegMP r, RegMP a, uint32_t b, bool carryIn, bool carryOut) {
    uint32_t zero=0;

    if(r.length()!=a.length()) RMP_ERROR("_add_ui() - length mismatch");

    PTXChain chain(r.length(), carryIn, carryOut);
    chain.ADD(r[0], a[0], b);
    #pragma unroll
    for(int index=1;index<r.length();index++)
      chain.ADD(r[index], a[index], zero);
    chain.end();
  }

  __device__ __forceinline__ void add_ui(RegMP r, RegMP a, uint32_t b) {
    uint32_t zero=0;

    if(r.length()!=a.length()) RMP_ERROR("add_ui() - length mismatch");

    asm volatile ("// START OF ADD_UI");
    PTXChain chain(r.length());
    chain.ADD(r[0], a[0], b);
    #pragma unroll
    for(int index=1;index<r.length();index++)
      chain.ADD(r[index], a[index], zero);
    chain.end();
    asm volatile ("// END OF ADD_UI");
  }

  __device__ __forceinline__ void _add_si(RegMP r, RegMP a, int32_t b, bool carryIn, bool carryOut) {
    uint32_t value=(uint32_t)b;

    if(r.length()!=a.length()) RMP_ERROR("_add_si() - length mismatch");

    PTXChain chain(r.length(), carryIn, carryOut);
    chain.ADD(r[0], a[0], value);
    value=(uint32_t)(b>>31);
    #pragma unroll
    for(int index=1;index<r.length();index++)
      chain.ADD(r[index], a[index], value);
    chain.end();
  }

  __device__ __forceinline__ void add_si(RegMP r, RegMP a, int32_t b) {
    PTXChain chain;
    uint32_t value=(uint32_t)b;

    if(r.length()!=a.length()) RMP_ERROR("add_si() - length mismatch");

    chain.start(r.length());
    chain.ADD(r[0], a[0], value);
    value=(uint32_t)(b>>31);
    #pragma unroll
    for(int index=1;index<r.length();index++)
      chain.ADD(r[index], a[index], value);
    chain.end();
  }
}
