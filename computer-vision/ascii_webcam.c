#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include <highgui.h>
#include <cv.h>

int SAVE_TO_FILE, SHOW_TO_WINDOW, CAMERA_NO;
int FLIP, HEIGHT, WIDTH, HC, WC, WINDOW,STAT;
char *fo;

void help() {
	printf("\n\twebcam [-h] [-f] [-s widthxheigth] [-c CAMERA]\n\n");
	printf("\tshow webcam stream in textual format\n\n");
	printf("\t-c\tindex of camera (0 for default camera)\n");
	printf("\t-f\tflip frames\n");
	printf("\t-w\tshow a window with the true video\n");
	printf("\t-h\tshow this help\n");
	printf("\t-s widthxheigth\tspecify dimension of terminal\n");
	printf("\t-t widthxheigth\tspecify dimension of characters\n");
	printf("\t-i\tstatistics about the stream\n\n");

	printf("\tauthor: NICOLA ZAGO\n\temail: zago.nicola@gmail.com\n\n");
}

int main(int argc, char *argv[]) {
	long diff;
	struct timeval t_1, t_2;
	char fps[20];
	char color[] = {' ', 96 ,'-',':',';','r','i','I','K','G','@','B'};
	int fr_no=0;
	
	HEIGHT = 20;
	WIDTH = 80;
	HC=19;
	WC=10;
	CAMERA_NO = 0;
	WINDOW = 0;
	FLIP = 0;
	
	if (argc<2) {
		help();
		return 1;
	}
	
	int c;

	opterr = 0;

	while ((c = getopt (argc, argv, "hifwc:s:t:")) != -1)
		switch (c) {
			case 'i':
				STAT=1;
				break;
			case 'f':
				FLIP=1;
				break;
			case 's':
				if (sscanf(optarg,"%dx%d",&WIDTH, &HEIGHT) != 2) {
					fprintf(stderr, "error reading argument of -s\n");
					exit(1);
				}
				break;
			case 't':
				if (sscanf(optarg,"%dx%d",&WC, &HC) != 2) {
					fprintf(stderr, "error reading argument of -t\n");
					exit(1);
				}
				break;
			case 'h':
				help();
				return 0;
			case 'c':
				CAMERA_NO = atoi(optarg);
				break;
			case 'w':
				WINDOW = 1;
				break;
			case '?':
				if (optopt == 'c' )
					fprintf (stderr, "Option -c requires camera number.\n");
				else if (optopt == 's')
					fprintf (stderr, "Option -s requires widthxheight of terminal.\n");
				else if (optopt == 't')
					fprintf (stderr, "Option -t requires widthxheight of screen characters.\n");
				else
					fprintf (stderr, "Unknown option character -%c.\n", optopt);
				return 1;
			default:
				abort ();
		}
		
	printf("STATISTICS: %d\n", STAT);
	printf("CAMERA_NO: %d\n", CAMERA_NO);
	printf("WINDOW: %d\n", WINDOW);
	printf("TERMINAL: %d x %d\n",WIDTH, HEIGHT);
	printf("CHARACTERS: %d x %d\n",WC, HC);
	printf("FLIP: %d\n", FLIP);
	
	if (WINDOW)
		cvNamedWindow( "Camera", CV_WINDOW_AUTOSIZE);

	CvCapture* capture = cvCaptureFromCAM(CAMERA_NO);
	
	IplImage* frame = cvQueryFrame( capture );		// catturo un frame prima per determinare le CvSize del flusso
	// correggo h e w nuove: h*ch : w*cw = bmp_h : bmp_w
	if (HEIGHT*HC*1.0/WC/WIDTH < frame->height*1.0/frame->width) {	// riduco w
		WIDTH = (int)((HEIGHT*HC*frame->width)*1.0/WC/frame->height);
	}
	else
		HEIGHT = (int)((WIDTH*WC*frame->height)*1.0/HC/frame->width);
	printf("TERMINAL: %d x %d\n",WIDTH, HEIGHT);
	
	IplImage* frame2 = cvCreateImage( cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 3);	// rimpicciolisco
	IplImage* frame3 = cvCreateImage( cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 1);

	char buffer[HEIGHT][WIDTH+1];
	char *temp;
	int t1,t2,t3,t4;
	for (t1=0; t1<HEIGHT; t1++) buffer[t1][WIDTH]=0;

	unsigned char *data;
	int avg;
	//long var;
	int min, max;
	gettimeofday(&t_1, NULL);
	
	int s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11;
	while (1) {
		frame = cvQueryFrame( capture );

		if (FLIP)
			cvFlip(frame, NULL, 0);

		cvResize( frame, frame2, CV_INTER_LINEAR );
		cvCvtColor(frame2, frame3, CV_RGB2GRAY );

		if (WINDOW)
			cvShowImage( "Camera", frame );

		data = (unsigned char *) frame3->imageData;
		avg=max=0; min=255;
		for (t1=0; t1<HEIGHT; t1++) {
			t2=t1*frame3->widthStep;
			for (t3=0; t3<WIDTH; t3++,t2++) {
				avg+=data[t2];
				if (data[t2]>max) max=data[t2];
				if (data[t2]<min) min=data[t2];
			}
		}
		avg/=(WIDTH*HEIGHT);
		
		/*for (t1=0; t1<HEIGHT; t1++) {
			t2=t1*frame3->widthStep;
			for (t3=0; t3<WIDTH; t3++,t2++) {
				var+=(data[t2]-avg)*(data[t2]-avg);
			}
		}
		var=(long)sqrt(var/(WIDTH*HEIGHT));*/

		s1 = (avg + min*5)/6;
		s2 = (avg + (min<<1))/3;
		s3 = (avg + min)>>1;
		s4 = (avg<<1 + min)/3;
		s5 = (avg*5 + min)/6;
		s6=avg;
		s7 = (max + avg*5)/6;
		s8 = (max + (avg<<1))/3;
		s9 = (max + avg)>>1;
		s10 = (max<<1 + avg)/3;
		s11 = (max*5 + avg)/6;
		
		
		for (t1=0; t1<HEIGHT; t1++) {
			t2=t1*frame3->widthStep;
			temp = buffer[t1];
			for (t3=0; t3<WIDTH; t3++,t2++) {
				// qui converti
				t4 = data[t2];
				if (t4<s6) {
					if (t4>s5) temp[t3] = color[5];
					else if (t4>s4) temp[t3] = color[4];
					else if (t4>s3) temp[t3] = color[3];
					else if (t4>s2) temp[t3] = color[2];
					else if (t4>s1) temp[t3] = color[1];
					else temp[t3] = color[0];
				}
				else {
					if (t4<s7) temp[t3] = color[6];
					else if (t4<s8) temp[t3] = color[7];
					else if (t4<s9) temp[t3] = color[8];
					else if (t4<s10) temp[t3] = color[9];
					else if (t4<s11) temp[t3] = color[10];
					else temp[t3] = color[11];				
				}
			}
		}
		
		if (STAT) {
			// change last line to show fr_no: xxxx and fps: xx.xxx
			fr_no++;
			gettimeofday(&t_2, NULL);
			diff = t_2.tv_sec*1000000+t_2.tv_usec-t_1.tv_sec*1000000-t_1.tv_usec;
			sprintf(fps,"fps: %.3f", 1000000.0/diff);
		
			for (t1=0; fps[t1]!=0; t1++)
				buffer[HEIGHT-1][t1] = fps[t1];

			sprintf(fps,"fr_no: %d", fr_no);
		
			for (t2=0; fps[t2]!=0; t2++)
				buffer[HEIGHT-1][20+t2] = fps[t2];
		}
		
		for (t1=0; t1<HEIGHT; t1++)
			printf("%s\n",buffer[t1]);

		gettimeofday(&t_1, NULL);
		
		t1 = cvWaitKey(20);
		if ( (t1 & 255) == 'q' || (t1 & 255) == 'Q' || (t1 & 255) == 27 )
			break;
	}

	cvReleaseCapture(&capture);
	
	if (WINDOW)
		cvDestroyWindow( "Camera" );
				
	return 0;
}
