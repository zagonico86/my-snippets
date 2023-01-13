#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include <emmintrin.h>

#ifdef IS_PAPI
    #include <papi.h>
#endif

#define ERROR(cond,text) if ((cond)) fprintf(stderr, text)

#define SM 8

void print_matrix(float *m, int h, int w) {
    int i,j;

    for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {
            printf("%.3f ", m[i*w+j]);
        }
        printf("\n");
    }
    printf("\n");
}
/*
void mm_1(float *m1, float *m2, float *m3, int a, int b, int c) {
    for (int i=0; i<a; i++) {
        for (int j=0; j<c; j++) {
            for (int k=0; k<b; k++) {
                m3[i*c+j] += m1[i*b+k]*m2[k*c+j];
            }
        }
    }
}

void mm_1_transp(float *m1, float *m2, float *m3, int a, int b, int c) {
    float T[c*b];
    for (int i=0; i<b; i++)     // m2 b x c, T c x b
        for (int j=0; j<c; j++)
            T[j*b+i] = m2[i*c+j];
    
    for (int i=0; i<a; i++) {
        for (int j=0; j<c; j++) {
            for (int k=0; k<b; k++) {
                m3[i*c+j] += m1[i*b+k]*T[j*b+k];
            }
        }
    }
}

void mm_1_1idx(float *m1, float *m2, float *m3, int a, int b, int c) {
    float *ma, *mb, *mc;
    
    ma = m1;
    for (int i=0; i<a; i++) {
        for (int j=0; j<c; j++) {
            mb = m2+j;
            int t = i*c+j;
            for (int k=0; k<b; k++, mb+=c) {
                m3[t] += ma[k] * *mb;
            }
        }
        ma+=b;
    }
}

void mm_1_transp_1idx(float *m1, float *m2, float *m3, int a, int b, int c) {
    float T[c*b];
    float *ma, *mb;
    int t1,t2;
    
    for (int i=0; i<b; i++) {    // m2 b x c, T c x b
        t1=i*c;
        t2=i;
        for (int j=0; j<c; j++, t2+=b , t1++)
            T[t2] = m2[t1];
    }
    
    for (int i=0; i<a; i++) {
        for (int j=0; j<c; j++) {
            t1 = i*c+j;
            ma = m1+(i*b);
            mb = T+(j*b);
            for (int k=0; k<b; k++) {
                m3[t1] += ma[k]* mb[k];
            }
        }
    }
}

void mm_1_transp_1idx_unroll2(float *m1, float *m2, float *m3, int a, int b, int c) {
    float T[c*b];
    float *ma, *mb;
    int t1,t2;
    
    for (int i=0; i<b; i++) {    // m2 b x c, T c x b
        t1=i*c;
        t2=i;
        for (int j=0; j<c; j++, t2+=b , t1++)
            T[t2] = m2[t1];
    }
    
    int b_2=(b>>1)<<1;
    
    for (int i=0; i<a; i++) {
        for (int j=0; j<c; j++) {
            t1 = i*c+j;
            ma = m1+(i*b);
            mb = T+(j*b);
            int k;
            for (k=0; k<b_2; k+=2) {
                m3[t1] += ma[k]* mb[k];
                m3[t1] += ma[k+1]* mb[k+1];
            }
            for ( ; k<b; k++) {
                m3[t1] += ma[k]* mb[k];
            }
        }
    }
}

void mm_1_transp_1idx_unroll4(float *m1, float *m2, float *m3, int a, int b, int c) {
    float T[c*b];
    float *ma, *mb;
    int t1,t2;
    
    for (int i=0; i<b; i++) {    // m2 b x c, T c x b
        t1=i*c;
        t2=i;
        for (int j=0; j<c; j++, t2+=b , t1++)
            T[t2] = m2[t1];
    }
    
    int b_2=(b>>2)<<2;
    
    int a1,a2,a3,a4;
    for (int i=0; i<a; i++) {
        for (int j=0; j<c; j++) {
            t1 = i*c+j;
            ma = m1+(i*b);
            mb = T+(j*b);
            int k;
            for (k=0; k<b_2; k+=4) {
                a1 = ma[k]* mb[k];
                a2 = ma[k+1]* mb[k+1];
                a3 = ma[k+2]* mb[k+2];
                a4 = ma[k+3]* mb[k+3];
                a1+=a2;
                a3+=a4;
                a1+=a3;
                m3[t1] += a1;
            }
            for ( ; k<b; k++) {
                m3[t1] += ma[k]* mb[k];
            }
        }
    }
}

void mm_1_transp_1idx_unroll8(float *m1, float *m2, float *m3, int a, int b, int c) {
    float T[c*b];
    float *ma, *mb;
    int t1,t2;
    
    for (int i=0; i<b; i++) {    // m2 b x c, T c x b
        t1=i*c;
        t2=i;
        for (int j=0; j<c; j++, t2+=b , t1++)
            T[t2] = m2[t1];
    }
    
    int b_2 = (b>>3)<<3;
    
    int a1,a2,a3,a4,a5,a6,a7,a8;
    for (int i=0; i<a; i++) {
        for (int j=0; j<c; j++) {
            t1 = i*c+j;
            ma = m1+(i*b);
            mb = T+(j*b);
            int k;
            for (k=0; k<b_2; k+=4) {
                a1 = ma[k]* mb[k];
                a2 = ma[k+1]* mb[k+1];
                a3 = ma[k+2]* mb[k+2];
                a4 = ma[k+3]* mb[k+3];
                a5 = ma[k+4]* mb[k+4];
                a6 = ma[k+5]* mb[k+5];
                a7 = ma[k+6]* mb[k+6];
                a8 = ma[k+7]* mb[k+7];
                a1+=a2;
                a3+=a4;
                a5+=a6;
                a7+=a8;
                a1+=a3;
                a5+=a7;
                a1+=a5;
                m3[t1] += a1;
            }
            for ( ; k<b; k++) {
                m3[t1] += ma[k]* mb[k];
            }
        }
    }
}*/

void mm_1_transp_1idx_sse2(float *m1, float *m2, float *m3, int a, int b, int c) {
    float T[c*b];
    __m128d *ma, *mb;
    int t1,t2;
    
    for (int i=0; i<b; i++) {    // m2 b x c, T c x b
        t1=i*c;
        t2=i;
        for (int j=0; j<c; j++, t2+=b , t1++)
            T[t2] = m2[t1];
    }
    
    int b_2 = b>>1;
    
    __m128d parz;
    float temp[2];
    for (int i=0; i<a; i++) {
        ma = (__m128d *) (m1+(i*b));
        for (int j=0; j<c; j++) {
            mb = (__m128d *) (T+(j*b));
            parz = _mm_set1_pd(0.0);
            for (int k=0; k<b_2; k++) {
                parz = _mm_add_pd( parz, _mm_mul_pd(ma[k], mb[k]) );
            }
            _mm_store_pd( temp, parz);
            *m3++ = temp[0]+temp[1];
        }
    }
}

#ifndef CHUNK
    #define CHUNK 8
#endif

void mm_1_transp_1idx_sse2_cache(float *m1, float *m2, float *m3, int a, int b, int c) {
    float *T;
    posix_memalign((void **) &T, 16, c*b*sizeof(float));
    __m128d *ma, *mb, mt;
    float *mc;
    int t1,t2;
    
    float *src, *dst;
    for (int i=0; i<b; i+=CHUNK) {
        for (int j=0; j<c; j+=CHUNK) {
            src = m2 + i*c + j;
            dst = T + j*b + i;
            for (int i1=0; i1<CHUNK; i1++, src+=c, dst++) {
                for (int i2=0; i2<CHUNK; i2++) {
                    *(dst+i2*b) = src[i2];
                }
            }
        }
    }
    
    for (int i=0; i<b; i++) {    // m2 b x c, T c x b
        t1=i*c;
        t2=i;
        for (int j=0; j<c; j++, t2+=b , t1++)
            T[t2] = m2[t1];
    }
    
    int b_2 = b>>1;
    
    __m128d parz;
    float temp[2];
    
//   ___b_____    __c__         __c__
//  |         |  |     |       |     |
// a|         | b|     |      a|     |
//  |         |  |     |  ==   |     |
//  |_________|  |     |       |_____|
//               |_____|

//   ___b_____    ___b_____         __c__
//  |         |  |         |       |     |
// a|         | c|         |      a|     |
//  |         |  |_________|  ==   |     |
//  |_________|                    |_____|
//               

    for (int j=0; j<b_2; j+=CHUNK) {
        for (int i=0; i<a; i+=CHUNK) {
            for (int k=0; k<c; k+=CHUNK) {
                ma = (__m128d *) (m1 + i*b + j*2);
                for (int i1=0; i1<CHUNK; i1++, ma+=b_2) {
                    mb = (__m128d *) (m2 + k*b + j*2);
                    mc = m3 + (i+i1)*c + k;
                    for (int i2=0; i2<CHUNK; i2++, mb+=b_2, mc++) {
                        mt = _mm_set1_pd(0.0);
                        for (int i3=0; i3<CHUNK; i3++) {
                            mt = _mm_add_pd(mt, _mm_mul_pd(ma[i3], mb[i3]));
                        }
                        _mm_store_pd( temp, mt);
                        *mc += temp[0] + temp[1];
                    }
                }
            }
        }
    }
    
    free(T);
}

void mm_1_kij_sse2_cache(float *m1, float *m2, float *m3, int a, int b, int c) {
    __m128d ma, *mb, mt;
    float *mc, *m1chunk;

    int b_2 = b>>1;
    
    __m128d parz;
    float temp[2];
    
//   ___b_____    __c__         __c__
//  |         |  |     |       |     |
// a|         | b|     |      a|     |
//  |         |  |     |  ==   |     |
//  |_________|  |     |       |_____|
//               |_____|

  /*  __m128d ma, *mb, *mc = (__m128d *) m3;
    int t1,t2;

    int c_2 = c>>1;
    for (int i=0; i<a; i++) {
        mb = (__m128d *) m2;
        for (int k=0; k<b; k++, mb+=c_2) {
            ma = _mm_load1_pd(m1++);    //k*a+i
            for (int j=0; j<c_2; j++) {
                mc[j] = _mm_add_pd(mc[j], _mm_mul_pd(ma, mb[j]));
            }
        }
        mc+=c_2;
    }*/
    
    int c_2 = c>>1;
    
    for (int j=0; j<b; j+=CHUNK) {
        for (int i=0; i<a; i+=CHUNK) {
            for (int k=0; k<c_2; k+=CHUNK) {
                m1chunk = m1 + i*b + j;
                for (int i1=0; i1<CHUNK; i1++, m1chunk+=b-CHUNK) {
                    mb = (__m128d *) (m2 + k*c + j*2);
                    mc = m3 + (i+i1)*c + k;
                    for (int i2=0; i2<CHUNK; i2++, mb+=b_2, mc++) {
                        ma = _mm_load1_pd(m1chunk++);
                        for (int i3=0; i3<CHUNK; i3++) {
                            mt = _mm_add_pd(mt, _mm_mul_pd(ma[i3], mb[i3]));
                        }
                        _mm_store_pd( temp, mt);
                        *mc += temp[0] + temp[1];
                    }
                }
            }
        }
    }
}

/*void potente(float **mul1, float **mul2, float **res, int N) {
    int i, i2, j, j2, k, k2;
    float *restrict rres;
    float *restrict rmul1;
    float *restrict rmul2;
    for (i = 0; i < N; i += SM)
      for (j = 0; j < N; j += SM)
        for (k = 0; k < N; k += SM)
          for (i2 = 0, rres = &res[i][j], rmul1 = &mul1[i][k]; i2 < SM;
               ++i2, rres += N, rmul1 += N)
            {
              _mm_prefetch (&rmul1[8], _MM_HINT_NTA);
              for (k2 = 0, rmul2 = &mul2[k][j]; k2 < SM; ++k2, rmul2 += N)
                {
                  __m128d m1d = _mm_load_sd (&rmul1[k2]);
                  m1d = _mm_unpacklo_pd (m1d, m1d);
                  for (j2 = 0; j2 < SM; j2 += 2)
                    {
                      __m128d m2 = _mm_load_pd (&rmul2[j2]);
                      __m128d r2 = _mm_load_pd (&rres[j2]);
                      _mm_store_pd (&rres[j2],
                                    _mm_add_pd (_mm_mul_pd (m2, m1d), r2));
                    }
                }
            }
}*/
//   ___b_____    __c__         __c__
//  |         |  |     |       |     |
// a|         | b|     |      a|     |
//  |         |  |     |  ==   |     |
//  |_________|  |     |       |_____|
//               |_____|

void mm_1_kij(float *m1, float *m2, float *m3, int a, int b, int c) {
    float *mb;
    
    float ma;
    for (int i=0; i<a; i++) {
        mb = m2;
        for (int k=0; k<b; k++) {
            ma = *m1++;
            for (int j=0; j<c; j++) {
                m3[j] += ma * mb[j];
            }
            mb += c;
        }
        m3+=c;
    }
}
/*
void mm_1_kij_bis(float *m1, float *m2, float *m3, int a, int b, int c) {
    float *mb;
    
    float ma;
    for (int i=0; i<a; i++) {
        mb = m2;
        for (int k=0; k<b; k++) {
            for (int j=0; j<c; j++) {
                m3[j] += *m1 * mb[j];
            }
            mb += c;
            m1++;
        }
        m3+=c;
    }
}

void mm_1_kij_unroll2(float *m1, float *m2, float *m3, int a, int b, int c) {
    float *mb;
    
    int r1,r2;
    float ma;
    for (int i=0; i<a; i++) {
        mb = m2;
        for (int k=0; k<b; k++) {
            ma = *m1++;
            for (int j=0; j<c; j+=2) {
                m3[j] = ma * mb[j];
                m3[j+1] = ma * mb[j+1];
            }
            mb += c;
        }
        m3+=c;
    }
}

void mm_1_kij_unroll4(float *m1, float *m2, float *m3, int a, int b, int c) {
    float *mb;

    float ma;
    for (int i=0; i<a; i++) {
        mb = m2;
        for (int k=0; k<b; k++) {
            ma = *m1++;
            for (int j=0; j<c; j+=4) {
                m3[j] = ma * mb[j];
                m3[j+1] = ma * mb[j+1];
                m3[j+2] = ma * mb[j+2];
                m3[j+3] = ma * mb[j+3];
            }
            mb += c;
        }
        m3+=c;
    }
}

void mm_1_kij_unroll8(float *m1, float *m2, float *m3, int a, int b, int c) {
    float *mb;

    float ma;
    for (int i=0; i<a; i++) {
        mb = m2;
        for (int k=0; k<b; k++) {
            ma = *m1++;
            for (int j=0; j<c; j+=8) {
                m3[j] = ma * mb[j];
                m3[j+1] = ma * mb[j+1];
                m3[j+2] = ma * mb[j+2];
                m3[j+3] = ma * mb[j+3];
                m3[j+4] = ma * mb[j+4];
                m3[j+5] = ma * mb[j+5];
                m3[j+6] = ma * mb[j+6];
                m3[j+7] = ma * mb[j+7];
            }
            mb += c;
        }
        m3+=c;
    }
}*/
//   ___b_____    __c__         __c__
//  |         |  |     |       |     |
// a|   A     | b|  B  |      a|  C  |
//  |         |  |     |  ==   |     |
//  |_________|  |     |       |_____|
//               |_____|

void mm_1_kij_sse2(float *m1, float *m2, float *m3, int a, int b, int c) {
    __m128d ma, *mb, *mc = (__m128d *) m3;
    int t1,t2;

    int c_2 = c>>1;
    for (int i=0; i<a; i++) {
        mb = (__m128d *) m2;
        for (int k=0; k<b; k++, mb+=c_2) {
            ma = _mm_load1_pd(m1++);    //k*a+i
            for (int j=0; j<c_2; j++) {
                mc[j] = _mm_add_pd(mc[j], _mm_mul_pd(ma, mb[j]));
            }
        }
        mc+=c_2;
    }
}

void mm_1_kij_sse2_unroll2(float *m1, float *m2, float *m3, int a, int b, int c) {
    __m128d ma, *mb, *mc = (__m128d *) m3;
    int t1,t2;

    int c_2 = (c>>2)<<1;
    for (int i=0; i<a; i++) {
        mb = (__m128d *) m2;
        for (int k=0; k<b; k++, mb+=c_2) {
            ma = _mm_load1_pd(m1++);    //k*a+i
            for (int j=0; j<c_2; j+=2) {
                mc[j] = _mm_add_pd(mc[j], _mm_mul_pd(ma, mb[j]));
                mc[j+1] = _mm_add_pd(mc[j+1], _mm_mul_pd(ma, mb[j+1]));
            }
        }
        mc+=c_2;
    }
}

void mm_1_kij_sse2_unroll4(float *m1, float *m2, float *m3, int a, int b, int c) {
    __m128d ma, *mb, *mc = (__m128d *) m3;
    int t1,t2;

    int c_2 = (c>>3)<<2;
    for (int i=0; i<a; i++) {
        mb = (__m128d *) m2;
        for (int k=0; k<b; k++, mb+=c_2) {
            ma = _mm_load1_pd(m1++);    //k*a+i
            for (int j=0; j<c_2; j+=4) {
                mc[j] = _mm_add_pd(mc[j], _mm_mul_pd(ma, mb[j]));
                mc[j+1] = _mm_add_pd(mc[j+1], _mm_mul_pd(ma, mb[j+1]));
                mc[j+2] = _mm_add_pd(mc[j+2], _mm_mul_pd(ma, mb[j+2]));
                mc[j+3] = _mm_add_pd(mc[j+3], _mm_mul_pd(ma, mb[j+3]));
            }
        }
        mc+=c_2;
    }
}

void mm_1_kij_sse2_unroll8(float *m1, float *m2, float *m3, int a, int b, int c) {
    __m128d ma, *mb, *mc = (__m128d *) m3;
    int t1,t2;

    int c_2 = (c>>4)<<3;
    for (int i=0; i<a; i++) {
        mb = (__m128d *) m2;
        for (int k=0; k<b; k++, mb+=c_2) {
            ma = _mm_load1_pd(m1++);    //k*a+i
            for (int j=0; j<c_2; j+=8) {
                mc[j] = _mm_add_pd(mc[j], _mm_mul_pd(ma, mb[j]));
                mc[j+1] = _mm_add_pd(mc[j+1], _mm_mul_pd(ma, mb[j+1]));
                mc[j+2] = _mm_add_pd(mc[j+2], _mm_mul_pd(ma, mb[j+2]));
                mc[j+3] = _mm_add_pd(mc[j+3], _mm_mul_pd(ma, mb[j+3]));
                mc[j+4] = _mm_add_pd(mc[j+4], _mm_mul_pd(ma, mb[j+4]));
                mc[j+5] = _mm_add_pd(mc[j+5], _mm_mul_pd(ma, mb[j+5]));
                mc[j+6] = _mm_add_pd(mc[j+6], _mm_mul_pd(ma, mb[j+6]));
                mc[j+7] = _mm_add_pd(mc[j+7], _mm_mul_pd(ma, mb[j+7]));
            }
        }
        mc+=c_2;
    }
}

#define LATO 80

void mm_1_transp_1idx_sse2_unroll2(float *m1, float *m2, float *m3, int a, int b, int c) {
    float T[c*b];
    __m128d *ma, *mb;
    int t1,t2;
    
    for (int i=0; i<b; i++) {    // m2 b x c, T c x b
        t1=i*c;
        t2=i;
        for (int j=0; j<c; j++, t2+=b , t1++)
            T[t2] = m2[t1];
    }
    
    int b_2 = b>>1;
    
    __m128d parz, r1, r2;
    float temp[2];
    for (int i=0; i<a; i++) {
        ma = (__m128d *) (m1+(i*b));
        for (int j=0; j<c; j++) {
            mb = (__m128d *) (T+(j*b));
            parz = _mm_set1_pd(0.0);
            for (int k=0; k<b_2; k+=2) {
                r1 = _mm_mul_pd(ma[k], mb[k]);
                r2 = _mm_mul_pd(ma[k+1], mb[k+1]);
                r1 = _mm_add_pd(r1, r2);
                parz = _mm_add_pd( parz, r1);
            }
            _mm_store_pd( temp, parz);
            *m3++ = temp[0]+temp[1];
        }
    }
}


void mm_1_transp_1idx_sse2_unroll4(float *m1, float *m2, float *m3, int a, int b, int c) {
    float T[c*b];
    __m128d *ma, *mb;
    int t1,t2;
    
    for (int i=0; i<b; i++) {    // m2 b x c, T c x b
        t1=i*c;
        t2=i;
        for (int j=0; j<c; j++, t2+=b , t1++)
            T[t2] = m2[t1];
    }
    
    int b_2 = b>>1;
    
    __m128d parz, r1, r2, r3, r4;
    float temp[2];
    for (int i=0; i<a; i++) {
        ma = (__m128d *) (m1+(i*b));
        for (int j=0; j<c; j++) {
            mb = (__m128d *) (T+(j*b));
            parz = _mm_set1_pd(0.0);
            for (int k=0; k<b_2; k+=4) {
                r1 = _mm_mul_pd(ma[k], mb[k]);
                r2 = _mm_mul_pd(ma[k+1], mb[k+1]);
                r3 = _mm_mul_pd(ma[k+2], mb[k+2]);
                r4 = _mm_mul_pd(ma[k+3], mb[k+3]);
                r1 = _mm_add_pd(r1, r2);
                r3 = _mm_add_pd(r3, r4);
                r1 = _mm_add_pd(r1, r3);
                parz = _mm_add_pd( parz, r1);
            }
            _mm_store_pd( temp, parz);
            *m3++ = temp[0]+temp[1];
        }
    }
}

void mm_1_transp_1idx_sse2_unroll8(float *m1, float *m2, float *m3, int a, int b, int c) {
    float *T;
    posix_memalign((void **) &T, 16, c*b*sizeof(float));
    __m128d *ma, *mb;
    int t1,t2;
    
    for (int i=0; i<b; i++) {    // m2 b x c, T c x b
        t1=i*c;
        t2=i;
        for (int j=0; j<c; j++, t2+=b , t1++)
            T[t2] = m2[t1];
    }
    
    int b_2 = b>>1;
    
    __m128d parz, r1, r2, r3, r4, r5, r6, r7, r8;
    float temp[2];
    for (int i=0; i<a; i++) {
        ma = (__m128d *) (m1+(i*b));
        for (int j=0; j<c; j++) {
            mb = (__m128d *) (T+(j*b));
            parz = _mm_set1_pd(0.0);
            for (int k=0; k<b_2; k+=8) {
                r1 = _mm_mul_pd(ma[k], mb[k]);
                r2 = _mm_mul_pd(ma[k+1], mb[k+1]);
                r3 = _mm_mul_pd(ma[k+2], mb[k+2]);
                r4 = _mm_mul_pd(ma[k+3], mb[k+3]);
                r5 = _mm_mul_pd(ma[k+4], mb[k+4]);
                r6 = _mm_mul_pd(ma[k+5], mb[k+5]);
                r7 = _mm_mul_pd(ma[k+6], mb[k+6]);
                r8 = _mm_mul_pd(ma[k+7], mb[k+7]);
                r1 = _mm_add_pd(r1, r2);
                r3 = _mm_add_pd(r3, r4);
                r5 = _mm_add_pd(r5, r6);
                r7 = _mm_add_pd(r7, r8);
                r1 = _mm_add_pd(r1, r3);
                r5 = _mm_add_pd(r5, r7);
                r1 = _mm_add_pd(r1, r5);
                parz = _mm_add_pd( parz, r1);
            }
            _mm_store_pd( temp, parz);
            *m3++ = temp[0]+temp[1];
        }
    }

    free(T);
}


int a=0;
int b=0;
int c=0;
int prove = 100;
int is_inv = 1;
int is_file=0;
int mode=0;
int debug=0;
char *file;

void read_arg(int argc, char *argv[]) {
    
    int sw;
    while ((sw = getopt (argc, argv, "a:b:c:f:n:ihm:d")) != -1)
    switch (sw) {
        case 'a':
            a = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 'c':
            c = atoi(optarg);
            break;
        case 'd':
            debug=1;
            break;
        case 'm':
            mode = atoi(optarg);
            break;
        case 'n':
            prove = atoi(optarg);
            break;
        case 'f':
            is_file = 1;
            file = optarg;
            break;
        case 'i':
            is_inv=0;
            break;
        case '?':
            if (optopt == 'a' || optopt == 'b' || optopt == 'c' || optopt == 'n')
               fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        case 'h':
            fprintf(stderr,"%s [-s lim_small_fft] [-b lim_big_fft] [-p] [-n n_cycles] [-i] [-h]\n\n",argv[0]);
            fprintf(stderr, "\t-a N     size a [default=100]\n");
            fprintf(stderr, "\t-b N     size b [default=100]\n");
            fprintf(stderr, "\t-c N     size c [default=100]\n");
            fprintf(stderr, "\t-d       debug [default=no]\n");
            fprintf(stderr, "\t-n N     number of cycles [default=100]\n");
            fprintf(stderr, "\t-m N     mode [0-n, default=0]\n");
            fprintf(stderr, "\t-i       divide by four the number of cycles every doubling of sizes [default=no]\n");
            return;
        default:
            abort ();
    }
    
    if (!a && !b && !c && !is_file) {
        fprintf(stderr, "errore: specifica le dimensioni o un file\n");
        exit(1);
    }
    if (a && !b && !c) {
        b=c=a;
    }
}

int main(int argc, char *argv[]) {
    read_arg(argc, argv);

    FILE *fi = NULL;
    if (is_file) {
        fi = fopen(file, "r");
        int t = fread(&a, sizeof(int), 1, fi);
        ERROR( t!=1 , "can't read a\n" );
        t = fread(&b, sizeof(int), 1, fi);
        ERROR( t!=1 , "can't read b\n" );
        t = fread(&c, sizeof(int), 1, fi);
        ERROR( t!=1 , "can't read c\n" );
    }

    float *m1;
    float *m2;
    float *m3;
    posix_memalign((void **) &m1, 16, a*b*sizeof(float));
    posix_memalign((void **) &m2, 16, b*c*sizeof(float));
    posix_memalign((void **) &m3, 16, a*c*sizeof(float));

    if (fi) {
        int t = fread(m1, sizeof(float), a*b, fi);
        ERROR( t!=a*b , "can't read m1\n" );
        t = fread(m2, sizeof(float), c*b, fi);
        ERROR( t!=c*b , "can't read m2\n" );
    }
    else {
        int t, lim=a*b;
        for (t=0; t<lim; t++) m1[t]=t;
        //lim=b*c;
        //for (t=0; t<lim; t++) m2[t]=1;
        for (t=0; t<b; t++) m2[t*b+t]=1;
        //for (t=0; t<lim; t++) m2[t]=rand();
    }
    
    memset(m3, 0, a*c*sizeof(float));
    
    float **mm1 = (float **) malloc(a*sizeof(float *));
    float **mm2 = (float **) malloc(a*sizeof(float *));
    float **mm3 = (float **) malloc(a*sizeof(float *));
    
    for (int t=0; t<a; t++) {
        posix_memalign((void **) &(mm1[t]), 64, a*sizeof(float));
        for (int t2=0; t2<a; t2++) mm1[t][t2]=t*a + t2;
        posix_memalign((void **) &(mm2[t]), 64, a*sizeof(float));
        mm2[t][t]=1;
        posix_memalign((void **) &(mm3[t]), 64, a*sizeof(float));
        memset(mm3[t], 0, a*sizeof(float));
    }
            
#ifdef IS_PAPI
    float real_time, proc_time,mflops;
    long long flpops;
    float ireal_time, iproc_time, imflops;
    long long iflpops;
    int retval;

    if((retval=PAPI_flops(&ireal_time,&iproc_time,&iflpops,&imflops)) < PAPI_OK) {
        printf("Could not initialise PAPI_flops \n");
        printf("Your platform may not support floating point operation event.\n"); 
        printf("retval: %d\n", retval);
        exit(1);
    }
#else
    struct timeval t1,t2;
    gettimeofday( &t1, NULL);
#endif

    int t;
    
    switch (mode) {
 /*       case 0:
            fprintf(stderr,"normal: ");
            for (t=0; t<prove; t++)
                mm_1(m1,m2,m3,a,b,c);
            break;
        case 1:
            fprintf(stderr,"transpose: ");
            for (t=0; t<prove; t++)
                mm_1_transp(m1,m2,m3,a,b,c);
            break;
        case 2:
            fprintf(stderr,"idx: ");
            for (t=0; t<prove; t++)
                mm_1_1idx(m1,m2,m3,a,b,c);
            break;
        case 3:
            fprintf(stderr,"idx+transp: ");
            for (t=0; t<prove; t++)
                mm_1_transp_1idx(m1,m2,m3,a,b,c);
            break;
        case 4:
            fprintf(stderr,"idx+trans+unroll2: ");
            for (t=0; t<prove; t++)
                mm_1_transp_1idx_unroll2(m1,m2,m3,a,b,c);
            break;
        case 5:
            fprintf(stderr,"idx+trans+unroll4: ");
            for (t=0; t<prove; t++)
                mm_1_transp_1idx_unroll4(m1,m2,m3,a,b,c);
            break;
        case 6:
            fprintf(stderr,"idx+trans+unroll8: ");
            for (t=0; t<prove; t++)
                mm_1_transp_1idx_unroll8(m1,m2,m3,a,b,c);
            break;*/
        case 7:
            fprintf(stderr,"idx+trans+sse2: ");
            for (t=0; t<prove; t++)
                mm_1_transp_1idx_sse2(m1,m2,m3,a,b,c);
            break;
        case 8:
            fprintf(stderr,"idx+trans+sse2+unroll2: ");
            for (t=0; t<prove; t++)
                mm_1_transp_1idx_sse2_unroll2(m1,m2,m3,a,b,c);
            break;
        case 9:
            fprintf(stderr,"idx+trans+sse2+unroll4: ");
            for (t=0; t<prove; t++)
                mm_1_transp_1idx_sse2_unroll4(m1,m2,m3,a,b,c);
            break;
        case 10:
            fprintf(stderr,"idx+trans+sse2+unroll8: ");
            for (t=0; t<prove; t++)
                mm_1_transp_1idx_sse2_unroll8(m1,m2,m3,a,b,c);
            break;
        case 11:
            fprintf(stderr,"idx+kij: ");
            for (t=0; t<prove; t++)
                mm_1_kij(m1,m2,m3,a,b,c);
            break;
    /*    case 12:
            fprintf(stderr,"idx+kij+sse2_unr2: ");
            for (t=0; t<prove; t++)
                mm_1_kij_sse2_unroll2(m1,m2,m3,a,b,c);
            break;
        case 13:
            fprintf(stderr,"idx+kij+sse2_unr4: ");
            for (t=0; t<prove; t++)
                mm_1_kij_sse2_unroll4(m1,m2,m3,a,b,c);
            break;
        case 14:
            fprintf(stderr,"idx+kij+sse2_unr8: ");
            for (t=0; t<prove; t++)
                mm_1_kij_sse2_unroll8(m1,m2,m3,a,b,c);
            break;*/
        case 15:
            fprintf(stderr,"idx+kij+sse2: ");
            for (t=0; t<prove; t++)
                mm_1_kij_sse2(m1,m2,m3,a,b,c);
            break;
        case 16:
            fprintf(stderr,"idx+kij+sse2+unroll8: ");
            for (t=0; t<prove; t++)
                mm_1_kij_sse2_unroll8(m1,m2,m3,a,b,c);
            break;
        case 17:
            fprintf(stderr,"potente: ");
            for (t=0; t<prove; t++)
                mm_1_transp_1idx_sse2_cache(m1,m2,m3,a,b,c);
            break;
        default:
            break;
    }


#ifdef IS_PAPI
    if((retval=PAPI_flops( &real_time, &proc_time, &flpops, &mflops))<PAPI_OK) {
        printf("retval: %d\n", retval);
        exit(1);
    }
    
    //fprintf(stderr, "Size: %d-%d-%d Cycles: %d Real_time: %f Proc_time: %f  Total_flpops: %lld MFLOPS: %f\n",a,b,c, prove, real_time, proc_time,flpops,mflops);
    fprintf(stderr, " %d-%d-%d[%d] - %f %f %lld %f\n",a,b,c, prove, real_time, proc_time,flpops,mflops);
#else
    gettimeofday( &t2, NULL);
    long t3 = t2.tv_sec*1000000 + t2.tv_usec - t1.tv_sec*1000000 - t1.tv_usec;
    
    fprintf(stderr, "Size: %d-%d-%d Cycles: %d Real_time: %f Proc_time: %f\n",a,b,c,prove,t3,clock()/CLOCKS_PER_SEC);
#endif

    if (debug) {
        print_matrix(m1,a,b);
        print_matrix(m2,b,c);
        print_matrix(m3,a,c);
    }

}
