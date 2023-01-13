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

#ifdef IS_PAPI
    #include <papi.h>
#endif

#define ERROR(cond,text) if ((cond)) fprintf(stderr, text)

void mm(double **m1, double **m2, double **m3, int a, int b, int c) {
    for (int i=0; i<a; i++) {
        for (int j=0; j<c; j++) {
            for (int k=0; k<b; k++) {
                m3[i][j] += m1[i][k]*m2[k][j];
            }
        }
    }
}

void mm_trasp(double **m1, double **m2, double **m3, int a, int b, int c) {
    double T[c][b];
    for (int i=0; i<c; i++)
        for (int j=0; j<b; j++)
            T[i][j] = m2[j][i];
    
    for (int i=0; i<a; i++) {
        for (int j=0; j<c; j++) {
            for (int k=0; k<b; k++) {
                m3[i][j] += m1[i][k]*m2[j][k];
            }
        }
    }
}

void print_matrix(double **m, int h, int w) {
    int i,j;

    for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {
            printf("%.3f ", m[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int a=0;
int b=0;
int c=0;
int prove = 100;
int is_inv = 1;
int is_file=0;
int mode=0;
char *file;

void read_arg(int argc, char *argv[]) {
    
    int sw;
    while ((sw = getopt (argc, argv, "a:b:c:f:n:ihm:")) != -1)
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
        case 'n':
            prove = atoi(optarg);
            break;
        case 'm':
            mode = atoi(optarg);
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
            fprintf(stderr, "\t-n N     number of cycles [default=100]\n");
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

    double **m1;    // a x b
    double **m2;    // b x c
    double **m3;    // a x c
    
    m1 = (double **) malloc(a*sizeof(double *));
    m2 = (double **) malloc(b*sizeof(double *));
    m3 = (double **) malloc(a*sizeof(double *));
    
    for (int i=0; i<a; i++) {
        m1[i] = (double *) malloc(b*sizeof(double));
        m3[i] = (double *) malloc(c*sizeof(double));
    }
    for (int i=0; i<b; i++) {
        m2[i] = (double *) malloc(c*sizeof(double));
    }
    
    if (fi) {
        for (int i=0; i<a; i++) {
            int t = fread(m1[i], sizeof(double), b, fi);
            ERROR( t!=b , "can't read m1\n" );
        }
        for (int i=0; i<b; i++) {
            int t = fread(m2[i], sizeof(double), c, fi);
            ERROR( t!=c , "can't read m2\n" );
        }
    }
    else {
        for (int i=0; i<a; i++) {
            for (int j=0; j<b; j++)
                m1[i][j] = 1.0;
        }
        for (int i=0; i<b; i++) {
            for (int j=0; j<c; j++)
                m2[i][j] = 1.0;
        }
    }
    
    for (int i=0; i<a; i++) {
        memset(m3[i], 0, c*sizeof(double));
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

    if (mode==1) {
        fprintf(stderr,"2idxtras:");
        for (t=0; t<prove; t++)
            mm_trasp(m1,m2,m3,a,b,c);
    }
    else {
        fprintf(stderr,"2idxnorm:");
        for (t=0; t<prove; t++)
            mm(m1,m2,m3,a,b,c);
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

#ifdef DEBUG
    print_matrix(m1,a,b);
    print_matrix(m2,b,c);
    print_matrix(m3,a,c);
#endif

}
