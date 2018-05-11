//
//
//  NLMSvariants.c
//
//  Created by FBRDNLMS on 26.04.18.
//
//

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <float.h> // DBL_MAX

#define NUMBER_OF_SAMPLES 1000
#define WINDOWSIZE 5
#define tracking 40 //Count of weights
#define learnrate 0.8
#define PURE_WEIGHTS 0
#define USED_WEIGHTS 1
#define RESULTS 3
#define DIRECT_PREDECESSOR 2
#define LOCAL_MEAN 4
#define TEST_VALUES 5
#define DIFFERENTIAL_PREDECESSOR 6
#define RGB_COLOR 255
#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

//double x[] = { 0.0 };
double xSamples[NUMBER_OF_SAMPLES] = { 0.0 };
double w[WINDOWSIZE][NUMBER_OF_SAMPLES] = { { 0.0 },{ 0.0 } };

/* *svg graph building* */
typedef struct {
	double xVal[7];
	double yVal[7];
}point_t;

point_t points[NUMBER_OF_SAMPLES]; // [0] = xActual, [1]=xpredicted from localMean, [2]=xpredicted from directPredecessor, [3] = xpredicted from differentialpredecessor, [4] = xError from localMean, [5] xError from directPredecessor, [6] xError from differentialPredecessor

/* *ppm read, copy, write* */
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
void colorSamples( FILE* fp ); // stores color channel values in xSamples[]

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
void differentialPredecessor( void );
double *popNAN(double *xError, int xErrorLength); //return new array without NAN values
double windowXMean( int _arraylength, int xCount );


int main(int argc, char **argv) {
	char fileName[50];
	int i, k, xLength;
	int *colorChannel;
	imagePixel_t *image;


	image = rdPPM("cow.ppm");
	mkFileName(fileName, sizeof(fileName), TEST_VALUES);
	FILE* fp5 = fopen(fileName, "w");
	xLength = ppmColorChannel(fp5, image);
	printf("%d\n", xLength);

	FILE* fp6 = fopen(fileName, "r");
	colorSamples ( fp6 );

	srand((unsigned int)time(NULL));

	for (i = 0; i < NUMBER_OF_SAMPLES; i++) {
//		_x[i] += ((255.0 / M) * i); // Init test values
		for (int k = 0; k < WINDOWSIZE; k++) {
			w[k][i]= rndm(); // Init weights
		}
	}

	mkFileName(fileName, sizeof(fileName), PURE_WEIGHTS);
	// save plain test_array before math magic happens
	FILE *fp0 = fopen(fileName, "w");
	for (i = 0; i <= tracking; i++) {
		for ( k = 0; k < WINDOWSIZE; k++) {
			fprintf(fp0, "[%d][%d] %lf\n", k, i, w[k][i]);
		}
	}
	fclose(fp0);


	// math magic
	localMean();
	//directPredecessor();
	//differentialPredecessor();
	 // save test_array after math magic happened
	 // memset( fileName, '\0', sizeof(fileName) );
	mkFileName(fileName, sizeof(fileName), USED_WEIGHTS);
	FILE *fp1 = fopen(fileName, "w");
	for (i = 0; i < tracking; i++) {
		for (int k = 0; k < WINDOWSIZE; k++) {
			fprintf(fp1, "[%d][%d] %lf\n", k, i, w[k][i]);
		}

	}
	fclose(fp1);

	// getchar();
	printf("DONE!");

}


/*
 ======================================================================================================

    localMean

    Variant (1/3), substract local mean.

 ======================================================================================================
*/

void localMean(void) {
	char fileName[50];
	double xError[NUMBER_OF_SAMPLES]; // includes e(n)
	memset(xError, 0.0, NUMBER_OF_SAMPLES);// initialize xError-array with Zero
	int xCount = 0, i; // runtime var;
	mkFileName(fileName, sizeof(fileName), LOCAL_MEAN);
	FILE* fp4 = fopen(fileName, "w");
	fprintf(fp4, "\n=====================================LocalMean=====================================\n");

    double xMean = xSamples[0];
	double weightedSum = 0.0;
	double filterOutput = 0.0;
    double xSquared = 0.0;
    double xPredicted = 0.0;
    double xActual = 0.0;


	for (xCount = 1; xCount < NUMBER_OF_SAMPLES; xCount++) { // first value will not get predicted
        double xPartArray[xCount]; //includes all values at the size of runtime var
		//int _sourceIndex = (xCount > WINDOWSIZE) ? xCount - WINDOWSIZE : xCount;
		int _arrayLength = (xCount > WINDOWSIZE) ? WINDOWSIZE + 1 : xCount;
		//printf("xCount:%d, length:%d\n", xCount, _arrayLength);
        xMean = ( xCount > 0 ) ? windowXMean(_arrayLength, xCount) : 0;
        // printf("WINDOWSIZE:%f\n", windowXMean(_arrayLength, xCount));
		xPredicted = 0.0;
		xActual = xSamples[xCount + 1];
	//	weightedSum += _x[ xCount-1 ] * w[xCount][0];

		for (i = 1; i < _arrayLength ; i++) { //get predicted value
		 	xPredicted += (w[i][xCount] * ( xSamples[xCount - i] - xMean));

		}
		xPredicted += xMean;
		xError[xCount] = xActual - xPredicted;
		printf("Pred: %f\t\tActual:%f\n", xPredicted,xActual);
		points[xCount].xVal[1] = xCount;
		points[xCount].yVal[1] = xPredicted;
		points[xCount].xVal[4] = xCount;
		points[xCount].yVal[4] = xError[xCount];

        xSquared = 0.0;
		for (i = 1; i < _arrayLength; i++) { //get xSquared
			//xSquared += pow(xSamples[xCount - i], 2);
			xSquared += pow(xSamples[xCount - i] - xMean, 2);
			printf("xSquared:%f\n", xSquared);
		}
		if(xSquared == 0.0){ // returns Pred: -1.#IND00
            xSquared = 1.0;
		}
        //printf("%f\n", xSquared);
		for (i = 1; i < _arrayLength; i++) { //update weights
			w[i][xCount + 1] = w[i][xCount] + learnrate * xError[xCount] * ( (xSamples[xCount - i] - xMean) / xSquared);
		}

		fprintf(fp4, "{%d}.\txPredicted{%f}\txActual{%f}\txError{%f}\n", xCount, xPredicted, xActual, xError[xCount]);

	}
	int xErrorLength = sizeof(xError) / sizeof(xError[0]);
	printf("vor:%d", xErrorLength);
	popNAN(xError, xErrorLength);
	printf("nach:%d", xErrorLength);
	xErrorLength = sizeof(xError) / sizeof(xError[0]);
	double mean = sum_array(xError, xErrorLength) / xErrorLength;
	double deviation = 0.0;

	// Mean square
	for (i = 0; i < xErrorLength - 1; i++) {
		deviation += pow(xError[i] - mean, 2);
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
  ======================================================================================================

    directPredecessor

    Variant (2/3),
    substract direct predecessor

  ======================================================================================================
*/

void directPredecessor(void) {
	char fileName[512];
	double xError[2048];
	int xCount = 0, i;
	double xActual;
    int xPredicted = 0.0;
	// File handling
	mkFileName(fileName, sizeof(fileName), DIRECT_PREDECESSOR);
	FILE *fp3 = fopen(fileName, "w");
	fprintf(fp3, "\n=====================================DirectPredecessor=====================================\n");

	for (xCount = 1; xCount < NUMBER_OF_SAMPLES + 1; xCount++) {
        double xPartArray[xCount]; //includes all values at the size of runtime var
		//int _sourceIndex = (xCount > WINDOWSIZE) ? xCount - WINDOWSIZE : xCount;
		int _arrayLength = (xCount > WINDOWSIZE) ? WINDOWSIZE + 1 : xCount;
		printf("xCount:%d, length:%d\n", xCount, _arrayLength);
        double xMean = ( xCount > 0 ) ? windowXMean(_arrayLength, xCount) : 0;
        printf("%f\n", windowXMean(_arrayLength, xCount));
		xPredicted = 0.0;
		xActual = xSamples[xCount + 1];

		for (i = 1; i < _arrayLength; i++) {
			xPredicted += (w[i][xCount] * (xSamples[xCount - 1] - xSamples[xCount - i - 1]));
		}
		xPredicted += xSamples[xCount - 1];
		xError[xCount] = xActual - xPredicted;

		fprintf(fp3, "{%d}.\txPredicted{%f}\txActual{%f}\txError{%f}\n", xCount, xPredicted, xActual, xError[xCount]);
		points[xCount].xVal[2] = xCount;
		points[xCount].yVal[2] = xPredicted;
		points[xCount].xVal[5] = xCount;
		points[xCount].yVal[5] = xError[xCount];

		double xSquared = 0.0;

		for (i = 1; i < _arrayLength; i++) {
			xSquared += pow(xSamples[xCount - 1] - xSamples[xCount - i - 1], 2); // substract direct predecessor
		}
		for (i = 1; i < _arrayLength; i++) {
			w[i][xCount + 1] = w[i][xCount] + learnrate * xError[xCount] * ((xSamples[xCount - 1] - xSamples[xCount - i - 1]) / xSquared);
		}
	}

	int xErrorLength = sizeof(xError) / sizeof(xError[0]);
    printf("vor:%d", xErrorLength);
	popNAN(xError, xErrorLength);
	printf("nach:%d", xErrorLength);
	xErrorLength = sizeof(xError) / sizeof(xError[0]);
	double mean = sum_array(xError, xErrorLength) / xErrorLength;
	double deviation = 0.0;

	for (i = 0; i < xErrorLength - 1; i++) {
		deviation += pow(xError[i] - mean, 2);
	}
	deviation /= xErrorLength;

	mkSvgGraph(points);
	fprintf(fp3, "{%d}.\tLeast Mean Squared{%f}\tMean{%f}\n\n", xCount, deviation, mean);
	fclose(fp3);
}


/*
 ======================================================================================================

    differentialPredecessor

    variant (3/3),
    differenital predecessor.

 ======================================================================================================
 */
void differentialPredecessor ( void ) {

	char fileName[512];
	double xError[2048];
	int xCount = 0, i;
	double xActual;

	// File handling
	mkFileName(fileName, sizeof(fileName), DIFFERENTIAL_PREDECESSOR);
	FILE *fp6 = fopen(fileName, "w");
	fprintf(fp6, "\n=====================================DifferentialPredecessor=====================================\n");

	for (xCount = 1; xCount < NUMBER_OF_SAMPLES + 1; xCount++) {
		xActual = xSamples[xCount + 1];
		double xPredicted = 0.0;

		for (i = 1; i < xCount; i++) {
			xPredicted += (w[i][xCount] * (xSamples[xCount - i] - xSamples[xCount - i - 1]));
		}
		xPredicted += xSamples[xCount - 1];
		xError[xCount] = xActual - xPredicted;

		fprintf(fp6, "{%d}.\txPredicted{%f}\txActual{%f}\txError{%f}\n", xCount, xPredicted, xActual, xError[xCount]);
		points[xCount].xVal[3] = xCount;
		points[xCount].yVal[3] = xPredicted;
		points[xCount].xVal[6] = xCount;
		points[xCount].yVal[6] = xError[xCount];
		double xSquared = 0.0;

		for (i = 1; i < xCount; i++) {
			xSquared += pow(xSamples[xCount - i] - xSamples[xCount - i - 1], 2); // substract direct predecessor
		}
		for (i = 1; i < xCount; i++) {
			w[i][xCount + 1] = w[i][xCount] + learnrate * xError[xCount] * ((xSamples[xCount - i] - xSamples[xCount - i - 1]) / xSquared);
		}
	}
	int xErrorLength = sizeof(xError) / sizeof(xError[0]);
	double mean = sum_array(xError, xErrorLength) / xErrorLength;
	double deviation = 0.0;

	for (i = 0; i < xErrorLength - 1; i++) {
		deviation += pow(xError[i] - mean, 2);
	}
	deviation /= xErrorLength;

	mkSvgGraph(points);
	fprintf(fp6, "{%d}.\tLeast Mean Squared{%f}\tMean{%f}\n\n", xCount, deviation, mean);
	fclose(fp6);


}


/*
 ======================================================================================================

    mkFileName

    Writes the current date plus the suffix with index suffixId
    into the given buffer. If the total length is longer than max_len,
    only max_len characters will be written.

 ======================================================================================================

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
 ======================================================================================================

    fileSuffix

    Contains and returns every suffix for all existing filenames

 ======================================================================================================
*/

char * fileSuffix(int id) {
	char * suffix[] = { "_weights_pure.txt", "_weights_used.txt", "_direct_predecessor.txt", "_ergebnisse.txt", "_localMean.txt","_testvalues.txt", "_differential_predecessor.txt" };
	return suffix[id];
}


/*
 ======================================================================================================

    myLogger

    Logs x,y points to svg graph

 ======================================================================================================
*/
void bufferLogger(char *buffer, point_t points[]) {
	int i;
	char _buffer[512] = "";

	for (i = 0; i < NUMBER_OF_SAMPLES - 1; i++) { // xActual
		sprintf(_buffer, "L %f %f\n", points[i].xVal[0], points[i].yVal[0]);
		strcat(buffer, _buffer);
	}
	strcat(buffer, "\" fill=\"none\" stroke=\"black\" stroke-width=\"0.4px\"/>\n<path d=\"M0 0\n");
	for (i = 0; i < NUMBER_OF_SAMPLES - 1; i++) { // xPrediceted from localMean
		sprintf(_buffer, "L %f %f\n", points[i].xVal[1], points[i].yVal[1]);
		strcat(buffer, _buffer);
	}
	strcat(buffer, "\" fill=\"none\" stroke=\"green\" stroke-width=\"0.4px\"/>\n<path d=\"M0 0\n");
	for (i = 0; i <= NUMBER_OF_SAMPLES - 1; i++) { //xPreddicted from directPredecessor
		sprintf(_buffer, "L %f %f\n", points[i].xVal[2], points[i].yVal[2]);
		strcat(buffer, _buffer);
	}
	strcat(buffer, "\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.4px\"/>\n<path d=\"M0 0\n");
	for (i = 0; i < NUMBER_OF_SAMPLES - 1; i++ ){ //xPredicted from diff Pred
		sprintf(_buffer, "L %f %f\n", points[i].xVal[3], points[i].xVal[3]);
		strcat(buffer, _buffer);
	}

}


/*
 ======================================================================================================

    sum_array

    Sum of all elements in x within a defined length

 ======================================================================================================
*/

double sum_array(double x[], int xlength) {
	int i = 0;
	double sum = 0.0;

    if (xlength !=0 ){
        for (i = 0; i < xlength; i++) {
            sum += x[i];
        }
    }
	return sum;
}


/*
 ======================================================================================================

    popNanLength

    returns length of new array without NAN values

 ======================================================================================================
*/

double *popNAN( double *xError,int xErrorLength ) {
    int i, counter;
    double noNAN [xErrorLength];

    for ( i = 0; i < xErrorLength; i++) {
        if ( !isnan(xError[i]) ) {
            noNAN[i] = xError[i];
            counter++;
        }
    }
    realloc(noNAN, counter * sizeof(double));
    int noNANLength = sizeof(noNAN)/ sizeof(noNAN[0]);
    memcpy(xError, noNAN, noNANLength);
    return xError;

}
/*
 ======================================================================================================

    r2

    returns a random double value between 0 and 1

 ======================================================================================================
*/

double r2(void) {
	return((rand() % 10000) / 10000.0);
}


/*
 ======================================================================================================

    rndm

    fills a double variable with random value and returns it

 ======================================================================================================
*/

double rndm(void) {
	double rndmval = r2();
	return rndmval;
}


/*
 ======================================================================================================

    mkSvgGraph

    parses template.svg and writes results in said template

 ======================================================================================================
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
 ======================================================================================================

    rdPPM

    reads data from file of type PPM, stores colorchannels in a struct in the
    size of given picture

 ======================================================================================================
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
 ======================================================================================================

    mkPpmFile

    gets output from the result of rdPpmFile and writes a new PPM file. Best Case is a
    carbon copy of the source image. Build for debugging

 ======================================================================================================
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
 ======================================================================================================

    ppmColorChannel

    gets one of the rgb color channels and writes them to a file

 ======================================================================================================
*/

int ppmColorChannel(FILE* fp, imagePixel_t *image) {
//	int length = 1000; // (image->x * image->y) / 3;
	int i = 0;

	if (image) {
		for ( i = 0; i < NUMBER_OF_SAMPLES - 1; i++ ){
		fprintf(fp,"%d\n", image->data[i].green);
		}
	}
	fclose(fp);
	return NUMBER_OF_SAMPLES;
}


/*
 ======================================================================================================

    colorSamples

    reads colorChannel values from file and stores them in xSamples as well as points datatype for
    creating the SVG graph

 ======================================================================================================
*/
void colorSamples( FILE* fp ) {
	int i = 0;
	int d, out;
	double f;
	char  buffer[NUMBER_OF_SAMPLES];

	while ( !feof(fp) ) {
		if ( fgets(buffer, NUMBER_OF_SAMPLES, fp) != NULL ) {
		sscanf(buffer,"%lf", &xSamples[i]);
		//printf("%lf\n", xSamples[i] );
		points[i].yVal[0] = xSamples[i];
		points[i].xVal[0] = i;
		++i;
		}
	}
	fclose(fp);
}

double windowXMean (int _arraylength, int xCount) {
    int count;
    double sum = 0.0;
    double *ptr;
   // printf("*window\t\t*base\t\txMean\n\n");
    for ( ptr = &xSamples[xCount - _arraylength]; ptr != &xSamples[xCount]; ptr++) { //set ptr to beginning of window
		//window = xCount - _arraylength
		//base = window - _arraylength;
		//sum = 0.0;
		//for( count = 0; count < _arraylength; count++){
			sum += *ptr;
		//	printf("%f\n", *base);

		//}
    }
		//printf("\n%lf\t%lf\t%lf\n", *ptr, *ptr2, (sum/(double)WINDOW));
    return sum/(double)_arraylength;
}
