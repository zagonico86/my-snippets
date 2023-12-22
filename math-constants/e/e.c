#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

int arrayLength;

int64_t  *binomialCust(int digits) {
    int64_t  BASE = 1000000000;

    int fract_length = digits / 9;
    while (fract_length%4!=0)fract_length++;
	arrayLength = fract_length;

    int64_t  *e = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t  *fract = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    e[0] = fract[0] = BASE/10;
    int fract_start=0;
    int i=1;

    while (fract_start<fract_length-1 || fract[fract_length-1]>100)
    {
        int64_t  rem = 0;
        // fract /= i
        for (int j=fract_start; j<fract_length; j++) {
            int64_t  n = rem*BASE + fract[j];
            fract[j] = n/i;
            rem = n%i;
        }
        // e += fract
        for (int j=fract_start; j<fract_length; j++) {
            e[j] += fract[j];
            int k=j;
            while (e[k]>=BASE) {
                e[k-1] += e[k]/BASE;
                e[k]%=BASE;
				k--;
            }
        }

        if (fract[fract_start]==0) fract_start++;

        i++;
    }
	free(fract);

    return e;
}

int64_t  *binomialCust1(int digits) {
    int64_t  BASE = 1000000000;

    int fract_length = digits / 9;
    while (fract_length%4!=0)fract_length++;
	arrayLength = fract_length;

    int64_t  *e = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t  *fract = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    e[0] = fract[0] = BASE/10;
    int fract_start=0;
    int i=1;

    while (fract_start<fract_length-1 || fract[fract_length-1]>100)
    {
        int64_t  rem = 0;
        // fract /= i
        for (int j=fract_start; j<fract_length; j++) {
            int64_t  n = rem*BASE + fract[j];
            fract[j] = n/i;
            rem = n%i;
            e[j] += fract[j];
        }

        if (fract[fract_start]==0) fract_start++;

        i++;
    }
	
	for (int j=fract_length-1; j>=0; j--) {
		if (e[j]>=BASE) {
			e[j-1] += e[j]/BASE;
			e[j]%=BASE;
		}
	}
	
	free(fract);

    return e;
}

int64_t  *binomialCustM(int digits) {
    int64_t  BASE = 1000000000;

    int fract_length = digits / 9;
    while (fract_length%4!=0)fract_length++;
	arrayLength = fract_length;

    int64_t  *e = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t  *fract = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    e[0] = fract[0] = BASE/10;
    int fract_start=0;
    int i=1;

    while (fract_start<fract_length-1 || fract[fract_length-1]>100)
    {
        int64_t  rem = 0;
        
        for (int j=fract_start; j<fract_length; j+=4) {
			// fract /= i
			// e += fract
            int64_t  n = rem*BASE + fract[j];
            fract[j] = n/i;
            rem = n%i;
			e[j] += fract[j];
			
			n = rem*BASE + fract[j+1];
            fract[j+1] = n/i;
            rem = n%i;
			e[j+1] += fract[j+1];
			
			n = rem*BASE + fract[j+2];
            fract[j+2] = n/i;
            rem = n%i;
			e[j+2] += fract[j+2];
			
			n = rem*BASE + fract[j+3];
            fract[j+3] = n/i;
            rem = n%i;
			e[j+3] += fract[j+3];
        }

        if (fract[fract_start]==0 && fract[fract_start+1]==0 && fract[fract_start+2]==0 && fract[fract_start+3]==0) fract_start+=4;

        i++;
    }
	
	
	for (int j=fract_length-1; j>=0; j-=4) {
		if (e[j]>=BASE) {
			e[j-1] += e[j]/BASE;
			e[j]%=BASE;
		}
		if (e[j-1]>=BASE) {
			e[j-2] += e[j-1]/BASE;
			e[j-1]%=BASE;
		}
		if (e[j-2]>=BASE) {
			e[j-3] += e[j-2]/BASE;
			e[j-2]%=BASE;
		}
		if (e[j-3]>=BASE) {
			e[j-4] += e[j-3]/BASE;
			e[j-3]%=BASE;
		}
	}
	free(fract);

    return e;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
        printf("Usage: %s <digits>\n", argv[0]);
        return 1;  // Return an error code
    }
	
	int digits = atoi(argv[1]);
	
    clock_t start_time = clock();

    int64_t  *res = binomialCustM(digits);
   
    clock_t end_time = clock();

    double elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Execution Time: %f seconds\n", elapsed_time);

	FILE *fi = fopen("output.txt", "w");
    if (fi == NULL) {
        perror("Error opening file");
        return 1;
    }
	for (int i=0; i<arrayLength; i++) {
		fprintf(fi, "%09d", res[i]);
	}
	
    fclose(fi);

    return 0;
}
