#include <stdio.h>
#include <stdlib.h>

int toInt(char *n, int start, int length) {
	int ris=0,a=start+length-1;
	
	for ( ; a>=start; a--) {
		ris = (ris<<8) | (n[a]&255);
	}
	
	return ris;
}

int main(int argc, char *argv[]) {
	int w, h;
	int cw=10, ch=19;
	int col[15][4] = {{255,0,0,'1'},{0,255,0,'2'},{255,255,0,'3'},{0,0,255,'4'},
				{255,0,255,'5'},{0,255,255,'6'},{127,0,0,'1'},{0,127,0,'2'},{127,127,0,'3'},{0,0,127,'4'},
				{127,0,127,'5'},{0,127,127,'6'},{255,255,255,0},{128,128,128,0},{0,0,0,0}};
	
	if (argc < 4 || argc > 6) {
		fprintf(stderr, "Syntax:\n");
		fprintf(stderr, "%s target_w target_h [char_w=10] [char_h=19]:,\n",argv[0]);
		fprintf(stderr, " - target_w: width of the resized image\n");
		fprintf(stderr, " - target_h: heigth of the resized image\n");
		fprintf(stderr, " - char_w: width of a character in the terminal\n");
		fprintf(stderr, " - char_h: heigth of a character in the terminal\n");
		fprintf(stderr, "e.g.: %s test.bmp 100 40 10 19\n",argv[0]);
		return 1;
	}
	
	FILE *fi = fopen(argv[1],"rb");
	if (fi <= 0) {
		fprintf(stderr, "Error opening file.\n");
		return 1;
	}
	
	w = atoi(argv[2]);
	if (w <= 0) {
		fprintf(stderr, "Invalid width.\n");
		return 1;
	}

	h = atoi(argv[3]);
	if (h <= 0) {
		fprintf(stderr, "Invalid heigth.\n");
		return 1;
	}
	
	if (argc>=5) {
		cw = atoi(argv[4]);
		if (cw <= 0) {
			fprintf(stderr, "Invalid character width.\n");
			return 1;
		}
	}

	if (argc>=6) {
		ch = atoi(argv[5]);
		if (ch <= 0) {
			fprintf(stderr, "Invalid character heigth.\n");
			return 1;
		}
	}
	
	fprintf(stderr, "w: %d, h: %d\n",w,h);
	fprintf(stderr, "cw: %d, ch: %d\n",cw,ch);
	
	fseek(fi, 0L, SEEK_END);
	int sz = ftell(fi);
	fseek(fi, 0L, SEEK_SET);
	
	char *bmp = malloc(sizeof(char)*sz);
	
	fread(bmp, sizeof(char), sz, fi);
	
	fclose(fi);
	
	if (bmp[0] != 'B' || bmp[1] != 'M') {
		fprintf(stderr, "Image is not a BMP\n");
		return 1;
	}
	
	int bmp_w=toInt(bmp,18,4);
	int bmp_h=toInt(bmp,22,4);
	fprintf(stderr, "Original BMP size: %dx%d (%d byte)\n",bmp_w,bmp_h,sz);
	
	// compute new h/w: h*ch : w*cw = bmp_h : bmp_w
	if (h*ch*1.0/cw/w < bmp_h*1.0/bmp_w) {	// reduce w
		w = (int)((h*ch*bmp_w)*1.0/cw/bmp_h);
	}
	else
		h = (int)((w*cw*bmp_h)*1.0/ch/bmp_w);
		
	fprintf(stderr, "output size (in characters): %dx%d\n",w,h);
	// resize original bmp in a new one sized h*w
	
	int row_length = (((bmp_w*3+3)/4)*4);
	float rapp_h = (float)(bmp_h*1.0 / h);
	float rapp_w = (float)(bmp_w*1.0 / w);
	//printf("%d %.3f %.3f\n",row_length,rapp_h,rapp_w);
	int a,b,c, i,j, e,f, tot, cr,cg,cb;
	printf("#!/bin/bash\n");
	for (a=h-1; a>=0; a--) {
		printf("echo \"");
		for (b=0; b<w; b++) {
			e = (int)((a+1)*rapp_h);
			f = (int)((b+1)*rapp_w);
			cr = cg=cb=tot=0;
			for (i=(int)(a*rapp_h); i<e; i++)
				for (j=(int)(b*rapp_w); j<f; j++) {
					c = 54+i*row_length+j*3;
					cr += bmp[c+2]&255;
					cg += bmp[c+1]&255;
					cb += bmp[c]&255;
					tot++;
				}
			cr/=tot;
			cg/=tot;
			cb/=tot;
			
			//printf("(%d %d %d)",cr,cg,cb);
			
			e=1000000;
			f=14;
			for (i=0; i<15; i++) {
				j = (cr-col[i][0])*(cr-col[i][0]) + (cg-col[i][1])*(cg-col[i][1]) + (cb-col[i][2])*(cb-col[i][2]);
				if (j<e) {
					e=j;
					f=i;
				}
			}
			
			if (f<6)
				printf("$(tput setaf %c)#",'1'+f);
			else if (f<12)
				printf("$(tput setaf %c)/",'1'+f-6);
			else if (f==12)
				printf("$(tput sgr0)#");
			else if (f==13)
				printf("$(tput sgr0)/");
			else
				printf(" ");
		}
		printf("\"\n");
	}
	printf("echo \"$(tput sgr0)\n\"");
	
	return 0;
}
