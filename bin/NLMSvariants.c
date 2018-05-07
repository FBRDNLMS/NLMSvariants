//
//  
//  NLMSvariants.c
//
//  Created by FBRDNLMS on 26.04.18.
//  Copyright © 2018 FBRDNLMS. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <float.h> // DBL_MAX

#define M 1000
#define tracking 40 //Count of weights
#define learnrate 1.0
#define PURE_WEIGHTS 0
#define USED_WEIGHTS 1
#define RESULTS 3
#define DIRECT_PREDECESSOR 2
#define LOCAL_MEAN 4
#define TEST_VALUES 5
#define RGB_COLOR 255
#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

double x[] = { 0 };
double _x[M] = { 0 };
double w[M][M] = { { 0 },{ 0 } };

/* *svg graph building* */
typedef struct {
	double xVal[7];
	double yVal[7];
}point_t;

point_t points[M]; // [0]=xActual, [1]=xPredicted from directPredecessor, [2]=xPredicted from localMean

/* *ppm reader/writer* */
typedef struct {
	unsigned char red, green, blue;
}colorChannel_t;

typedef struct {
	int x, y;
	colorChannel_t *data;
}imagePixel_t;

static imagePixel_t * rdPPM(char *fileName); // read PPM file format
void mkPpmFile(char *fileName, imagePixel_t *image); // writes PPM file
int ppmColorChannel(FILE* fp, imagePixel_t *image); // writes colorChannel from PPM file to log file
void ppmTo_X( FILE* fp ); // stores color channel values in _x[]

/* *file handling* */
char * mkFileName(char* buffer, size_t max_len, int suffixId);
char *fileSuffix(int id);
void myLogger(FILE* fp, point_t points[]);
void mkSvgGraph(point_t points[]);

/* *rand seed* */
double r2(void);
double rndm(void);

/* *math* */
double sum_array(double x[], int length);
void directPredecessor(void);
void localMean(void);



int main(int argc, char **argv) {
	char fileName[50];
	int i, xLength;
	int *colorChannel;	
	imagePixel_t *image;
	

	image = rdPPM("beaches.ppm");
	mkFileName(fileName, sizeof(fileName), TEST_VALUES);
	FILE* fp5 = fopen(fileName, "w");
	xLength = ppmColorChannel(fp5, image);
	printf("%d\n", xLength);	
	
	FILE* fp6 = fopen(fileName, "r");
	ppmTo_X ( fp6 );

	srand((unsigned int)time(NULL));

	for (i = 0; i < M; i++) {
//		_x[i] += ((255.0 / M) * i); // Init test values
		for (int k = 0; k < M; k++) {
			w[k][i] = rndm(); // Init weights
		}
	}

	mkFileName(fileName, sizeof(fileName), PURE_WEIGHTS);
	// save plain test_array before math magic happens
	FILE *fp0 = fopen(fileName, "w");
	for (i = 0; i <= tracking; i++) {
		for (int k = 0; k < tracking; k++) {
			fprintf(fp0, "[%d][%d] %lf\n", k, i, w[k][i]);
		}
	}
	fclose(fp0);


	// math magic
	localMean();
	directPredecessor(); // TODO: used_weights.txt has gone missing! 

	 // save test_array after math magic happened
	 // memset( fileName, '\0', sizeof(fileName) );
	mkFileName(fileName, sizeof(fileName), USED_WEIGHTS);
	FILE *fp1 = fopen(fileName, "w");
	for (i = 0; i < tracking; i++) {
		for (int k = 0; k < tracking; k++) {
			fprintf(fp1, "[%d][%d] %lf\n", k, i, w[k][i]);
		}

	}
	fclose(fp1);

	// getchar();
	printf("DONE!");

}


/*
=======================================================================================

localMean


Variant (1/3), substract local mean.

=======================================================================================
*/

void localMean(void) {
	char fileName[50];
	double xError[M]; // includes e(n)
	memset(xError, 0, M);// initialize xError-array with Zero
	int xCount = 0; // runtime var	
	int i;
	mkFileName(fileName, sizeof(fileName), LOCAL_MEAN);
	FILE* fp4 = fopen(fileName, "w");
	fprintf(fp4, "\n\n\n\n*********************LocalMean*********************\n");

	for (xCount = 1; xCount < M; xCount++) {

		//double xPartArray[xCount]; //includes all values at the size of runtime var

		double xMean = (xCount > 0) ? (sum_array(_x, xCount) / xCount) : 0;// xCount can not be zero 

		double xPredicted = 0.0;
		double xActual = _x[xCount + 1];

		for (i = 1; i < xCount; i++) { //get predicted value
			xPredicted += (w[i][xCount] * (_x[xCount - i] - xMean));
		}

		xPredicted += xMean;
		xError[xCount] = xActual - xPredicted;
		points[xCount].xVal[2] = xCount;
		points[xCount].yVal[2] = xPredicted;
		double xSquared = 0.0;

		for (i = 1; i < xCount; i++) { //get x squared
			xSquared = +pow(_x[xCount - i], 2);
		}

		for (i - 1; i < xCount; i++) { //update weights
			w[i][xCount + 1] = w[i][xCount] + learnrate * xError[xCount] * (_x[xCount - i] / xSquared);
		}

		fprintf(fp4, "{%d}.\txPredicted{%f}\txActual{%f}\txError{%f}\n", xCount, xPredicted, xActual, xError[xCount]);
	}
	int xErrorLength = sizeof(xError) / sizeof(xError[0]);
	double mean = sum_array(xError, xErrorLength) / M;
	double deviation = 0.0;

	// Mean square
	for (i = 0; i < M - 1; i++) {
		deviation += pow(xError[i], 2);
	}
	deviation /= xErrorLength;


	// write in file
	mkFileName(fileName, sizeof(fileName), RESULTS);
	FILE *fp2 = fopen(fileName, "w");
	fprintf(fp2, "quadr. Varianz(x_error): {%f}\nMittelwert:(x_error): {%f}\n\n", deviation, mean);
	fclose(fp2);
	fclose(fp4);
}


/*
===================================

directPredecessor


Variant (2/3),
substract direct predecessor

===================================
*/

void directPredecessor(void) {
	char fileName[512];
	double xError[2048];
	int xCount = 0, i;
	double xActual;

	// File handling
	mkFileName(fileName, sizeof(fileName), DIRECT_PREDECESSOR);
	FILE *fp3 = fopen(fileName, "w");
	fprintf(fp3, "\n\n\n\n*********************DirectPredecessor*********************\n");

	for (xCount = 1; xCount < M + 1; xCount++) {
		xActual = _x[xCount + 1];
		double xPredicted = 0.0;

		for (i = 1; i < xCount; i++) {
			xPredicted += (w[i][xCount] * (_x[xCount - i] - _x[xCount - i - 1]));
		}
		xPredicted += _x[xCount - 1];
		xError[xCount] = xActual - xPredicted;

		fprintf(fp3, "{%d}.\txPredicted{%f}\txActual{%f}\txError{%f}\n", xCount, xPredicted, xActual, xError[xCount]);
		points[xCount].xVal[0] = xCount;
		points[xCount].yVal[0] = xActual;
		points[xCount].xVal[1] = xCount;
		points[xCount].yVal[1] = xPredicted;

		double xSquared = 0.0;

		for (i = 1; i < xCount; i++) {
			xSquared += pow(_x[xCount - i] - _x[xCount - i - 1], 2); // substract direct predecessor
		}
		for (i = 1; i < xCount; i++) {
			w[i][xCount + 1] = w[i][xCount] + learnrate * xError[xCount] * ((_x[xCount - i] - _x[xCount - i - 1]) / xSquared); //TODO: double val out of bounds
		}
	}

	int xErrorLength = sizeof(xError) / sizeof(xError[0]);
	double mean = sum_array(xError, xErrorLength) / xErrorLength;
	double deviation = 0.0;

	for (i = 0; i < xErrorLength - 1; i++) {
		deviation += pow(xError[i] - mean, 2);
	}


	mkSvgGraph(points);
	fprintf(fp3, "{%d}.\tLeast Mean Squared{%f}\tMean{%f}\n\n", xCount, deviation, mean);
	fclose(fp3);
}




/*
=========================================================================

mkFileName


Writes the current date plus the suffix with index suffixId
into the given buffer. If[M ?K the total length is longer than max_len,
only max_len characters will be written.

=========================================================================
*/

char *mkFileName(char* buffer, size_t max_len, int suffixId) {
	const char * format_str = "%Y-%m-%d_%H_%M_%S";
	size_t date_len;
	const char * suffix = fileSuffix(suffixId);
	time_t now = time(NULL);

	strftime(buffer, max_len, format_str, localtime(&now));
	date_len = strlen(buffer);
	strncat(buffer, suffix, max_len - date_len);
	return buffer;
}



/*
=========================================================================

fileSuffix

Contains and returns every suffix for all existing filenames

==========================================================================
*/

char * fileSuffix(int id) {
	char * suffix[] = { "_weights_pure.txt", "_weights_used.txt", "_direct_predecessor.txt", "_ergebnisse.txt", "_localMean.txt","_testvalues.txt" };
	return suffix[id];
}


/*
==========================================================================

myLogger


Logs on filepointer, used for svg graphing

==========================================================================
*/
/*
void myLogger(FILE* fp, point_t points[]) {
	int i;
	for (i = 0; i <= M; i++) { // xActual
		fprintf(fp, "L %f %f\n", points[i].xVal[0], points[i].yVal[0]);
	}
	fprintf(fp, "\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.4px\"/>\n<path d=\"M0 0\n");
	for (i = 0; i < M - 1; i++) { // xPred from directPredecessor
		fprintf(fp, "L %f %f\n", points[i].xVal[1], points[i].yVal[1]);
	}
	fprintf(fp, "\" fill=\"none\" stroke=\"green\" stroke-width=\"0.4px\"/>\n<path d=\"M0 0\n");
	for (i = 0; i <= M; i++) { //xPred from lastMean
		fprintf(fp, "L %f %f\n", points[i].xVal[2], points[i].yVal[2]);
	}
}
*/
void bufferLogger(char *buffer, point_t points[]) {
	int i;
	char _buffer[512] = "";

	for (i = 0; i <= M; i++) { // xActual
		sprintf(_buffer, "L %f %f\n", points[i].xVal[0], points[i].yVal[0]);
		strcat(buffer, _buffer);
	}
	strcat(buffer, "\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.4px\"/>\n<path d=\"M0 0\n");
	for (i = 0; i < M - 1; i++) { // xPred from directPredecessor
		sprintf(_buffer, "L %f %f\n", points[i].xVal[1], points[i].yVal[1]);
		strcat(buffer, _buffer);
	}
	strcat(buffer, "\" fill=\"none\" stroke=\"green\" stroke-width=\"0.4px\"/>\n<path d=\"M0 0\n");
	for (i = 0; i <= M; i++) { //xPred from lastMean
		sprintf(_buffer, "L %f %f\n", points[i].xVal[2], points[i].yVal[2]);
		strcat(buffer, _buffer);
	}
}


/*
=========================================================================

sum_array


Sum of all elements in x within a defined length

=========================================================================
*/

double sum_array(double x[], int length) {
	int i = 0;
	double sum = 0.0;

	for (i = 0; i< length; i++) {
		sum += x[i];
	}
	return sum;
}


/*
==========================================================================

r2

returns a random double value between 0 and 1

==========================================================================
*/

double r2(void) {
	return((rand() % 10000) / 10000.0);
}



/*
==========================================================================

rndm

fills a double variable with random value and returns it

==========================================================================
*/

double rndm(void) {
	double rndmval = r2();
	return rndmval;
}



/*
==========================================================================

mkSvgGraph

parses template.svg and writes results in said template

==========================================================================
*/

void mkSvgGraph(point_t points[]) {
	FILE *input = fopen("template.svg", "r");
	FILE *target = fopen("output.svg", "w");
	char line[512];
	char firstGraph[15] = { "<path d=\"M0 0" };  

	if (input == NULL) {
		exit(EXIT_FAILURE);
	}
	
	char buffer[131072] = "";
	
	memset(buffer, '\0', sizeof(buffer));
	while(!feof(input)) {
		fgets(line, 512, input);
		strncat(buffer, line,strlen(line));
	//	printf("%s\n", line);
		if (strstr(line, firstGraph) != NULL) {
			bufferLogger(buffer, points);
		}

	}
	fprintf(target, buffer);
	//puts(buffer);
}



/*
===========================================================================

rdPPM

reads data from file of type PPM, stores colorchannels in a struct in the
size of given picture

==========================================================================
*/

static imagePixel_t *rdPPM(char *fileName) {
	char buffer[16];
	imagePixel_t *image;
	int c, rgbColor;

	FILE *fp = fopen(fileName, "rb");
	if (!fp) {
		exit(EXIT_FAILURE);
	}
	if (!fgets(buffer, sizeof(buffer), fp)) {
		perror(fileName);
		exit(EXIT_FAILURE);
	}
	if (buffer[0] != 'P' || buffer[1] != '6') {
		fprintf(stderr, "No PPM file format\n");
		exit(EXIT_FAILURE);
	}
	image = (imagePixel_t *)malloc(sizeof(imagePixel_t));
	if (!image) {
		fprintf(stderr, "malloc() failed");
	}
	c = getc(fp);
	while (c == '#') {
		while (getc(fp) != '\n');
		c = getc(fp);
	}
	ungetc(c, fp);
	if (fscanf(fp, "%d %d", &image->x, &image->y) != 2) {
		fprintf(stderr, "Invalid image size in %s\n", fileName);
		exit(EXIT_FAILURE);
	}
	if (fscanf(fp, "%d", &rgbColor) != 1) {
		fprintf(stderr, "Invalid rgb component in %s\n", fileName);
	}
	if (rgbColor != RGB_COLOR) {
		fprintf(stderr, "Invalid image color range in %s\n", fileName);
		exit(EXIT_FAILURE);
	}
	while (fgetc(fp) != '\n');
	image->data = (colorChannel_t *)malloc(image->x * image->y * sizeof(imagePixel_t));
	if (!image) {
		fprintf(stderr, "malloc() failed");
		exit(EXIT_FAILURE);
	}
	if (fread(image->data, 3 * image->x, image->y, fp) != image->y) {
		fprintf(stderr, "Loading image failed");
		exit(EXIT_FAILURE);
	}
	fclose(fp);
	return image;
}



/*
=======================================================================================

mkPpmFile

gets output from the result of rdPpmFile and writes a new mkPpmFile. Best Case is a
carbon copy of the source image

=======================================================================================
*/

void mkPpmFile(char *fileName, imagePixel_t *image) {
	FILE* fp = fopen(fileName, "wb");
	if (!fp) {
		fprintf(stderr, "Opening file failed.");
		exit(EXIT_FAILURE);
	}
	fprintf(fp, "P6\n");
	fprintf(fp, "%d %d\n", image->x, image->y);
	fprintf(fp, "%d\n", RGB_COLOR);
	fwrite(image->data, 3 * image->x, image->y, fp);
	fclose(fp);
}

/*
======================================================================================

ppmColorChannel

gets one of the rgb color channels and returns the array

======================================================================================
*/

int ppmColorChannel(FILE* fp, imagePixel_t *image) {
	int length = 1000; // (image->x * image->y) / 3;
	int i = 0; 
	
	if (image) {
		for ( i = 0; i <= length; i++ ){ 
		fprintf(fp,"%d\n", image->data[i].green);
		}
	}	
	fclose(fp);
	return length;	
}

void ppmTo_X( FILE* fp ) {
	int i = 0;
	int d, out;
	double f;
	int length = 1000;	
	char  buffer[length];

	while ( !feof(fp) ) {
		if ( fgets(buffer, length, fp) != NULL ) {	
		sscanf(buffer,"%lf", &_x[i]);
		printf("%lf\n", _x[i] );
		++i;
		}
	}	
	fclose(fp);
}
