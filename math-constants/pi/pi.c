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

    int64_t *pi = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t *fract5 = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t *fract239 = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t *fract5div = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t *fract239div = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t div_2i_1;
    fract5[0] =  160000000;
    fract239[0] = 40000000;
	pi[0] = 0;
	for (int i=1; i<fract_length; i++) {
		pi[i] = fract5[i] = fract239[i] = 0;
	}
    int fract5_start=0;
    int fract239_start=0;
    int64_t i=1;

    // 1/5 and 1/239
    fract5[0]/=5;
    int64_t rem = 0;
    int64_t rem2 = 0;
    for (int j=0; j<fract_length; j++) {
        int64_t n = rem*BASE + fract239[j];
        fract239[j] = n/239;
        rem = n%239;
    }
    // pi = 1/5 - 1/239
    rem = 0;
    for (int j=0; j<fract_length; j++) {
        pi[j] += fract5[j] - fract239[j];
        if (pi[j]<0) {
            pi[j]+=BASE;
            pi[j-1]--;
        }
    }
        
    while (fract239_start<fract_length-1 || fract239[fract_length-1]>100)
    {
        div_2i_1 = (2*i+1);
        rem = 0;
        rem2 = 0;
        // fract239 /= 239*239
		// fract239div = fract239/(2i+1)
        for (int j=fract239_start; j<fract_length; j++) {
            int64_t n = rem*BASE + fract239[j];
            rem = n%57121;
            fract239[j] = n/57121;
			
            int64_t n2 = rem2*BASE + fract239[j];
            fract239div[j] = n2/div_2i_1;
            rem2 = n2%div_2i_1;
        }
        rem = 0;
        rem2 = 0;
        // fract5 /= 5*5
		// fract5div = fract5/(2i+1)
        for (int j=fract5_start; j<fract_length; j++) {
            int64_t n = rem*BASE + fract5[j];
            fract5[j] = n/25;
			rem = n%25;
			
            int64_t n2 = rem2*BASE + fract5[j];
            fract5div[j] = n2/div_2i_1;
            rem2 = n2%div_2i_1;
        }

        // pi = pi - fract5div + fract239div  => pi -= (fract5div - fract239div)
        if ((i&1)>0) {
            rem = 0;
            for (int j=fract_length-1; j>=fract5_start; j--) {
                pi[j] += fract239div[j] - fract5div[j];
                if (pi[j]<0) {
                    pi[j]+=BASE;
                    pi[j-1]--;
                }
                else if (pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                }
            }

            int j = fract5_start-1;
            while (j>0 && pi[j]>=BASE) {
                pi[j-1]+=pi[j]/BASE;
                pi[j]%=BASE;
                j--;
            }
        }
        // pi += fract5div - fract239div
        else {
            rem = 0;
            for (int j=fract_length-1; j>=fract5_start; j--) {
                pi[j] += fract5div[j] - fract239div[j];
                if (pi[j]<0) {
                    pi[j]+=BASE;
                    pi[j-1]--;
                }
                else if (pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                }
            }

            int j = fract5_start-1;
            while (j>0 && pi[j]>=BASE) {
                pi[j-1]+=pi[j]/BASE;
                pi[j]%=BASE;
                j--;
            }
        }

        if (fract239[fract239_start]==0) fract239_start++;
        if (fract5[fract5_start]==0) fract5_start++;

        i++;
    }
    
    while (fract5_start<fract_length-1 || fract5[fract_length-1]>100)
    {
        div_2i_1 = (2*i+1);
        rem = 0;
        rem2 = 0;
        // fract5 /= 5*5
        for (int j=fract5_start; j<fract_length; j++) {
            int64_t n = rem*BASE + fract5[j];
            fract5[j] = n/25;
			rem = n%25;
			
            int64_t n2 = rem2*BASE + fract5[j];
            fract5div[j] = n2/div_2i_1;
            rem2 = n2%div_2i_1;
        }

        // pi = pi - fract5div
        if ((i&1)>0) {
            rem = 0;
            for (int j=fract_length-1; j>=fract5_start; j--) {
                pi[j] -= fract5div[j];
                if (pi[j]<0) {
                    pi[j]+=BASE;
                    pi[j-1]--;
                }
            }

            int j = fract5_start-1;
            while (j>0 && pi[j]>=BASE) {
                pi[j-1]+=pi[j]/BASE;
                pi[j]%=BASE;
                j--;
            }
        }
        // pi += fract5div
        else {
            rem = 0;
            for (int j=fract_length-1; j>=fract5_start; j--) {
                pi[j] += fract5div[j];
                if (pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                }
            }

            int j = fract5_start-1;
            while (j>0 && pi[j]>=BASE) {
                pi[j-1]+=pi[j]/BASE;
                pi[j]%=BASE;
                j--;
            }
        }
        if (fract5[fract5_start]==0) fract5_start++;

        i++;
    }

    free(fract5);
    free(fract239);
    free(fract5div);
    free(fract239div);

    return pi;
}

// pi = 176 arctan 1/57 + 28 arctan 1/239 -  48 arctan 1/682 + 96 arctan 1/12943
int64_t  *binomialCust2(int digits) {
    int64_t  BASE = 1000000000;

    int fract_length = digits / 9;
    while (fract_length%4!=0)fract_length++;
    arrayLength = fract_length;

    int64_t *pi = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t *fract57 = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t *fract239 = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t *fract682 = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t *fract12943 = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t *fract57div = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t *fract239div = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t *fract682div = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t *fract12943div = (int64_t  *)malloc(sizeof(int64_t )*fract_length);
    int64_t div_2i_1;
    fract57[0] =   176000000;
    fract239[0] =   28000000;
	fract682[0] =   48000000;
	fract12943[0] = 96000000;
	pi[0] = 0;
	for (int i=1; i<fract_length; i++) {
		pi[i] = fract57[i] = fract239[i] = fract682[i] = fract12943[i] = 0;
	}
    int fract57_start=0;
    int fract239_start=0;
    int fract682_start=0;
    int fract12943_start=0;
    int64_t i=1;

    // 1/57
    int64_t rem = 0;
    int64_t rem2 = 0;
    for (int j=0; j<fract_length; j++) {
        int64_t n = rem*BASE + fract57[j];
        fract57[j] = n/57;
        rem = n%57;
    }
	// 1/239
    rem = 0;
    rem2 = 0;
    for (int j=0; j<fract_length; j++) {
        int64_t n = rem*BASE + fract239[j];
        fract239[j] = n/239;
        rem = n%239;
    }
	// 1/682
    rem = 0;
    rem2 = 0;
    for (int j=0; j<fract_length; j++) {
        int64_t n = rem*BASE + fract682[j];
        fract682[j] = n/682;
        rem = n%682;
    }
	// 1/12943
    rem = 0;
    rem2 = 0;
    for (int j=0; j<fract_length; j++) {
        int64_t n = rem*BASE + fract12943[j];
        fract12943[j] = n/12943;
        rem = n%12943;
    }
    // pi = 1/57 + 1/239 - 1/682 + 1/12943
    rem = 0;
    for (int j=fract_length-1; j>=0; j--) {
        pi[j] += fract57[j] + fract239[j] - fract682[j] + fract12943[j];
        if (pi[j]<0) {
			pi[j]+=BASE;
			pi[j-1]--;
		}
		else if (pi[j]>=BASE) {
			pi[j-1]+=pi[j]/BASE;
			pi[j]%=BASE;
		}
    }
	
    while (fract12943_start<fract_length-1 || fract12943[fract_length-1]>100)
    {
        div_2i_1 = (2*i+1);
        rem = 0;
        rem2 = 0;
        // fract12943 /= 12943*12943
		// fract12943div = fract12943/(2i+1)
        for (int j=fract12943_start; j<fract_length; j++) {
            int64_t n = rem*BASE + fract12943[j];
            rem = n%167521249;
            fract12943[j] = n/167521249;
			
            int64_t n2 = rem2*BASE + fract12943[j];
            fract12943div[j] = n2/div_2i_1;
            rem2 = n2%div_2i_1;
        }
        rem = 0;
        rem2 = 0;
        // fract682 /= 682*682
		// fract682div = fract682/(2i+1)
        for (int j=fract682_start; j<fract_length; j++) {
            int64_t n = rem*BASE + fract682[j];
            rem = n%465124;
            fract682[j] = n/465124;
			
            int64_t n2 = rem2*BASE + fract682[j];
            fract682div[j] = n2/div_2i_1;
            rem2 = n2%div_2i_1;
        }
        rem = 0;
        rem2 = 0;
        // fract239 /= 239*239
		// fract239div = fract239/(2i+1)
        for (int j=fract239_start; j<fract_length; j++) {
            int64_t n = rem*BASE + fract239[j];
            rem = n%57121;
            fract239[j] = n/57121;
			
            int64_t n2 = rem2*BASE + fract239[j];
            fract239div[j] = n2/div_2i_1;
            rem2 = n2%div_2i_1;
        }
        rem = 0;
        rem2 = 0;
        // fract57 /= 57*57
		// fract57div = fract57/(2i+1)
        for (int j=fract57_start; j<fract_length; j++) {
            int64_t n = rem*BASE + fract57[j];
            fract57[j] = n/3249;
			rem = n%3249;
			
            int64_t n2 = rem2*BASE + fract57[j];
            fract57div[j] = n2/div_2i_1;
            rem2 = n2%div_2i_1;
        }

        if ((i&1)>0) {
            rem = 0;
            for (int j=fract_length-1; j>=fract57_start; j--) {
                pi[j] += fract682div[j] - fract57div[j] - fract239div[j] - fract12943div[j];
                if (pi[j]<0) {
                    pi[j-1]+=pi[j]/BASE - 1;
                    pi[j]=pi[j]%BASE + BASE;	// necessary because negative
                }
                else if (pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                }
            }

            int j = fract57_start-1;
            while (j>0 && pi[j]>=BASE) {
                pi[j-1]+=pi[j]/BASE;
                pi[j]%=BASE;
                j--;
            }
        }
        else {
            rem = 0;
            for (int j=fract_length-1; j>=fract57_start; j--) {
                pi[j] += fract57div[j] + fract239div[j] - fract682div[j] + fract12943div[j];
                if (pi[j]<0) {
                    pi[j-1]+=pi[j]/BASE - 1;
                    pi[j]=pi[j]%BASE + BASE;	// necessary because negative
                }
                else if (pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                }
            }

            int j = fract57_start-1;
            while (j>0 && pi[j]>=BASE) {
                pi[j-1]+=pi[j]/BASE;
                pi[j]%=BASE;
                j--;
            }
        }

        if (fract12943[fract12943_start]==0) fract12943_start++;
        if (fract682[fract682_start]==0) fract682_start++;
        if (fract239[fract239_start]==0) fract239_start++;
        if (fract57[fract57_start]==0) fract57_start++;

        i++;
    }
    
	        
    while (fract682_start<fract_length-1 || fract682[fract_length-1]>100)
    {
        div_2i_1 = (2*i+1);
        rem = 0;
        rem2 = 0;
        // fract682 /= 682*682
		// fract682div = fract682/(2i+1)
        for (int j=fract682_start; j<fract_length; j++) {
            int64_t n = rem*BASE + fract682[j];
            rem = n%465124;
            fract682[j] = n/465124;
			
            int64_t n2 = rem2*BASE + fract682[j];
            fract682div[j] = n2/div_2i_1;
            rem2 = n2%div_2i_1;
        }
        rem = 0;
        rem2 = 0;
        // fract239 /= 239*239
		// fract239div = fract239/(2i+1)
        for (int j=fract239_start; j<fract_length; j++) {
            int64_t n = rem*BASE + fract239[j];
            rem = n%57121;
            fract239[j] = n/57121;
			
            int64_t n2 = rem2*BASE + fract239[j];
            fract239div[j] = n2/div_2i_1;
            rem2 = n2%div_2i_1;
        }
        rem = 0;
        rem2 = 0;
        // fract57 /= 57*57
		// fract57div = fract57/(2i+1)
        for (int j=fract57_start; j<fract_length; j++) {
            int64_t n = rem*BASE + fract57[j];
            fract57[j] = n/3249;
			rem = n%3249;
			
            int64_t n2 = rem2*BASE + fract57[j];
            fract57div[j] = n2/div_2i_1;
            rem2 = n2%div_2i_1;
        }

        if ((i&1)>0) {
            rem = 0;
            for (int j=fract_length-1; j>=fract57_start; j--) {
                pi[j] += fract682div[j] - fract57div[j] - fract239div[j];
                if (pi[j]<0) {
                    pi[j-1]+=pi[j]/BASE - 1;
                    pi[j]=pi[j]%BASE + BASE;	// necessary because negative
                }
                else if (pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                }
            }

            int j = fract57_start-1;
            while (j>0 && pi[j]>=BASE) {
                pi[j-1]+=pi[j]/BASE;
                pi[j]%=BASE;
                j--;
            }
        }
        else {
            rem = 0;
            for (int j=fract_length-1; j>=fract57_start; j--) {
                pi[j] += fract57div[j] + fract239div[j] - fract682div[j];
                if (pi[j]<0) {
                    pi[j-1]+=pi[j]/BASE - 1;
                    pi[j]=pi[j]%BASE + BASE;	// necessary because negative
                }
                else if (pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                }
            }

            int j = fract57_start-1;
            while (j>0 && pi[j]>=BASE) {
                pi[j-1]+=pi[j]/BASE;
                pi[j]%=BASE;
                j--;
            }
        }

        if (fract682[fract682_start]==0) fract682_start++;
        if (fract239[fract239_start]==0) fract239_start++;
        if (fract57[fract57_start]==0) fract57_start++;

        i++;
    }
	        
    while (fract239_start<fract_length-1 || fract239[fract_length-1]>100)
    {
        div_2i_1 = (2*i+1);
        rem = 0;
        rem2 = 0;
        // fract239 /= 239*239
		// fract239div = fract239/(2i+1)
        for (int j=fract239_start; j<fract_length; j++) {
            int64_t n = rem*BASE + fract239[j];
            rem = n%57121;
            fract239[j] = n/57121;
			
            int64_t n2 = rem2*BASE + fract239[j];
            fract239div[j] = n2/div_2i_1;
            rem2 = n2%div_2i_1;
        }
        rem = 0;
        rem2 = 0;
        // fract57 /= 57*57
		// fract57div = fract57/(2i+1)
        for (int j=fract57_start; j<fract_length; j++) {
            int64_t n = rem*BASE + fract57[j];
            fract57[j] = n/3249;
			rem = n%3249;
			
            int64_t n2 = rem2*BASE + fract57[j];
            fract57div[j] = n2/div_2i_1;
            rem2 = n2%div_2i_1;
        }

        if ((i&1)>0) {
            rem = 0;
            for (int j=fract_length-1; j>=fract57_start; j--) {
                pi[j] -= fract57div[j] + fract239div[j];
                if (pi[j]<0) {
                    pi[j-1]+=pi[j]/BASE - 1;
                    pi[j]=pi[j]%BASE + BASE;	// necessary because negative
                }
                else if (pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                }
            }

            int j = fract57_start-1;
            while (j>0 && pi[j]>=BASE) {
                pi[j-1]+=pi[j]/BASE;
                pi[j]%=BASE;
                j--;
            }
        }
        else {
            rem = 0;
            for (int j=fract_length-1; j>=fract57_start; j--) {
                pi[j] += fract57div[j] + fract239div[j];
                if (pi[j]<0) {
                    pi[j-1]+=pi[j]/BASE - 1;
                    pi[j]=pi[j]%BASE + BASE;	// necessary because negative
                }
                else if (pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                }
            }

            int j = fract57_start-1;
            while (j>0 && pi[j]>=BASE) {
                pi[j-1]+=pi[j]/BASE;
                pi[j]%=BASE;
                j--;
            }
        }

        if (fract239[fract239_start]==0) fract239_start++;
        if (fract57[fract57_start]==0) fract57_start++;

        i++;
    }
	
    while (fract57_start<fract_length-1 || fract57[fract_length-1]>100)
    {
        div_2i_1 = (2*i+1);
        rem = 0;
        rem2 = 0;
        // fract57 /= 5*5
        for (int j=fract57_start; j<fract_length; j++) {
            int64_t n = rem*BASE + fract57[j];
            fract57[j] = n/3249;
			rem = n%3249;
			
            int64_t n2 = rem2*BASE + fract57[j];
            fract57div[j] = n2/div_2i_1;
            rem2 = n2%div_2i_1;
        }

        // pi = pi - fract57div
        if ((i&1)>0) {
            rem = 0;for (int j=fract_length-1; j>=fract57_start; j--) {
                pi[j] -= fract57div[j];
                if (pi[j]<0) {
                    pi[j-1]+=pi[j]/BASE - 1;
                    pi[j]=pi[j]%BASE + BASE;	// necessary because negative
                }
            }
            int j = fract57_start-1;
            while (j>0 && pi[j]>=BASE) {
                pi[j-1]+=pi[j]/BASE;
                pi[j]%=BASE;
                j--;
            }
        }
        // pi += fract57div
        else {
            rem = 0;for (int j=fract_length-1; j>=fract57_start; j--) {
                pi[j] += fract57div[j];
                if (pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                }
            }
            int j = fract57_start-1;
            while (j>0 && pi[j]>=BASE) {
                pi[j-1]+=pi[j]/BASE;
                pi[j]%=BASE;
                j--;
            }
        }
        if (fract57[fract57_start]==0) fract57_start++;

        i++;
    }

    free(fract57);
    free(fract239);
    free(fract682);
    free(fract12943);
    free(fract57div);
    free(fract239div);
    free(fract682div);
    free(fract12943div);

    return pi;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <poly = 1|2> <digits = n>\n", argv[0]);
        return 1;  // Return an error code
    }
    
    int poly = atoi(argv[1]);
    int digits = atoi(argv[2]);
    
    clock_t start_time = clock();

    int64_t  *res;
	if (poly==1) {
		res = binomialCust(digits);
	}
	else {
		res = binomialCust2(digits);
	}
   
    clock_t end_time = clock();

    double elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Execution Time: %f seconds\n", elapsed_time);

    FILE *fi = fopen("pi.txt", "w");
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
