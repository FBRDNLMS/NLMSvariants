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

#define M 100
#define tracking 40 //Count of weights
#define learnrate 1.0
#define PURE_WEIGHTS 0
#define USED_WEIGHTS 1
#define RESULTS 3
#define DIRECT_PREDECESSOR 2
#define LOCAL_MEAN 4

double x[] = {0};
double _x[M] = {0};
double w [M][M]={{0},{0}};


/* graph building */
typedef struct {	
	double xVal[7];
	double yVal[7];
}point_t;

point_t points[M]; // [0]=xActual, [1]=xPredicted from directPredecessor, [2]=xPredicted from localMean

/* *file handling* */
char * mkFileName( char* buffer, size_t max_len, int suffixId );
char *fileSuffix( int id );
void myLogger( FILE* fp, point_t points[]); 
size_t getline( char **lineptr, size_t *n, FILE *stream ); //redundant under POSIX supporting OS
void mkSvgGraph( point_t points[]);

/* *rand seed* */
double r2( void );
double rndm( void );

/* *math* */
double sum_array( double x[], int length );
void directPredecessor( void );
void localMean( void );



int main(int argc, char **argv ) {
    char fileName[50];	
    int i;

    srand( (unsigned int) time(NULL) );
    
    for (i = 0; i < M; i++) {
        _x[i] += ((255.0 / M) * i); // Init test values
        for (int k = 0; k < M; k++){
            w[k][i] = rndm(); // Init weights
        }
    }
    	
    mkFileName( fileName, sizeof(fileName), PURE_WEIGHTS); 
    // save plain test_array before math magic happens
    FILE *fp0 = fopen(fileName,"w");
    for (i = 0; i <= tracking; i++){
      for ( int k = 0; k < tracking; k++ ){
        fprintf(fp0, "[%d][%d] %lf\n", k, i, w[k][i]); 
      }
    }
    fclose(fp0);


    // math magic
    localMean(); 
    directPredecessor(); // TODO: used_weights.txt has gone missing! 

    // save test_array after math magic happened
   // memset( fileName, '\0', sizeof(fileName) );
    mkFileName( fileName, sizeof(fileName), USED_WEIGHTS); 
    FILE *fp1 = fopen(fileName,"w");
    for (i = 0; i < tracking; i++) {
        for (int k = 0; k < tracking; k++) {
            fprintf(fp1, "[%d][%d] %lf\n", k,i, w[k][i]);
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

void localMean( void ) {
    char fileName[50];
    double xError[M]; // includes e(n)
    memset(xError, 0, M);// initialize xError-array with Zero
    int xCount = 0; // runtime var	
    int i;
    mkFileName(fileName, sizeof(fileName), LOCAL_MEAN );			
    FILE* fp4 = fopen(fileName, "w");	
    fprintf(fp4, "\n\n\n\n*********************LocalMean*********************\n");

    for (xCount = 1; xCount < M; xCount++){ 
        
	//double xPartArray[xCount]; //includes all values at the size of runtime var

	double xMean = ( xCount > 0 ) ? ( sum_array(_x, xCount) / xCount ) : 0;// xCount can not be zero 
        
	double xPredicted = 0.0;
	double xActual = _x[xCount + 1];

	for ( i = 1; i < xCount; i++ ){ //get predicted value
	  xPredicted += ( w[i][xCount] * ( _x[xCount - i] - xMean )) ;
	}
	
	xPredicted += xMean;
	xError [xCount] = xActual - xPredicted;
	points[xCount].xVal[2] = xCount;
	points[xCount].yVal[2] = xPredicted;
	double xSquared = 0.0;

	for ( i = 1; i < xCount; i++ ){ //get x squared
          xSquared =+ pow(_x[xCount-i],2);
        }
	
	for ( i - 1; i < xCount; i++ ){ //update weights
	  w[i][xCount+1] = w[i][xCount] + learnrate * xError[xCount] * (_x[xCount - i] / xSquared);
	}
    
    fprintf(fp4, "{%d}.\txPredicted{%f}\txActual{%f}\txError{%f}\n", xCount, xPredicted, xActual, xError[xCount]);
    } 
    int xErrorLength = sizeof(xError) / sizeof(xError[0]);
    double mean = sum_array(xError, xErrorLength) / M;
    double deviation = 0.0;
   
    // Mean square
    for( i = 0; i < M - 1; i++ ){
      deviation += pow( xError[i], 2 );
    }
    deviation /= xErrorLength;

   
    // write in file
    mkFileName( fileName, sizeof(fileName), RESULTS );
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

void directPredecessor( void ) {
    char fileName[50];	 
    double xError [M];
    int xCount = 0, i;
        double xActual;
    
    // File handling
    mkFileName( fileName, sizeof(fileName), DIRECT_PREDECESSOR);
    FILE *fp3 = fopen(fileName, "w");	
    fprintf(fp3, "\n\n\n\n*********************DirectPredecessor*********************\n");

    for ( xCount = 1; xCount < M+1; xCount++ ){
      xActual = _x[xCount+1]; 
      double xPredicted = 0.0;
      
      for ( i = 1; i < xCount; i++ ){
        xPredicted += ( w[i][xCount] * ( _x[xCount - i] - _x[xCount - i - 1] ));
      }
      xPredicted += _x[xCount-1]; 
      xError[xCount] = xActual - xPredicted;

      fprintf(fp3, "{%d}.\txPredicted{%f}\txActual{%f}\txError{%f}\n", xCount, xPredicted, xActual, xError[xCount]);
      points[xCount].xVal[0] = xCount;
      points[xCount].yVal[0] = xActual;
      points[xCount].xVal[1] = xCount;
      points[xCount].yVal[1] = xPredicted;

      double xSquared = 0.0; 
      
      for ( i = 1; i < xCount; i++ ){
		xSquared += pow( _x[xCount - i] - _x[xCount - i - 1], 2); // substract direct predecessor
      }	
      for ( i = 1; i < xCount; i++){ 
        	w[i][xCount+1] = w[i][xCount] + learnrate * xError[xCount] * ( ( _x[xCount - i] - _x[xCount - i - 1] ) / xSquared ); //TODO: double val out of bounds
      }
    }

    int xErrorLength = sizeof(xError) / sizeof(xError[0]);  
    double mean = sum_array(xError, xErrorLength) / xErrorLength;
    double deviation = 0.0;

    for ( i = 0; i < xErrorLength -1; i++ ){
      		deviation += pow( xError[i] - mean, 2);
    }

    
    mkSvgGraph( points);
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

char *mkFileName( char* buffer, size_t max_len, int suffixId) {
	const char * format_str = "%Y-%m-%d_%H_%M_%S";
	size_t date_len;
	const char * suffix = fileSuffix( suffixId );
	time_t now = time(NULL);

	strftime( buffer, max_len, format_str, localtime(&now) );
	date_len = strlen(buffer);
	strncat( buffer, suffix, max_len - date_len );
}



/*
 =========================================================================

 fileSuffix

 Contains and returns every suffix for all existing filenames 

 ==========================================================================
*/

char * fileSuffix( int id ) {
	char * suffix[] = {"_weights_pure.txt", "_weights_used.txt", "_direct_predecessor.txt", "_ergebnisse.txt", "_localMean.txt"};
	return suffix[id];
}


/*
 ==========================================================================

 svgGraph	

 	
 ==========================================================================
*/
/*
void Graph ( ) {
	char fileName[50];
	mkFileName(fileName, sizeof(fileName), GRAPH);
	FILE* fp4 = fopen(fileName, "w");
	pfrintf
*/	


/*
 ==========================================================================

 myLogger


 Logs on filepointer, used for svg graphing 

 ==========================================================================
 */

void myLogger ( FILE* fp, point_t points[] ){
	int i;
	for( i = 0; i <= M; i++ ){
		fprintf( fp, "L %f %f\n", points[i].xVal[0], points[i].yVal[0]);
	}
	fprintf(fp, "\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.8px\"/>\n<path d=\"M0 0\n");
	for( i = 0; i <  M-1; i++ ) {
		fprintf( fp, "L %f %f\n", points[i].xVal[1], points[i].yVal[1]);
	}	
	fprintf(fp, "\" fill=\"none\" stroke=\"green\" stroke-width=\"0.8px\"/>\n<path d=\"M0 0\n");
	for( i = 0; i <= M; i++ ) {
		fprintf(fp, "L %f %f\n", points[i].xVal[2], points[i].yVal[2]);
	}
}	



/*
 =========================================================================
 
 sum_array
 
 
 Sum of all elements in x within a defined length
 
 =========================================================================
 */

double sum_array( double x[], int length ){
    int i = 0;
    double sum = 0.0;

    for( i=0; i< length; i++ ) {
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

double r2( void ) {
    return((rand() % 10000) / 10000.0);
}



/*
 ==========================================================================

 rndm

 fills a double variable with random value and returns it

 ==========================================================================
*/

double rndm( void ) {
    double rndmval= r2();
    return rndmval;
}



/*
 ==========================================================================

 getline

 This code is public domain -- Will Hartung 4/9/09
 Microsoft Windows is not POSIX conform and does not support getline. 
 What the Heck? 

 =========================================================================
*/

size_t getline(char **lineptr, size_t *n, FILE *stream) {
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    int c;

    if (lineptr == NULL) {
        return -1;
    }
    if (stream == NULL) {
        return -1;
    }
    if (n == NULL) {
        return -1;
    }
    bufptr = *lineptr;
    size = *n;

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }
    if (bufptr == NULL) {
        bufptr = malloc(128);
        if (bufptr == NULL) {
            return -1;
        }
        size = 128;
    }
    p = bufptr;
    while(c != EOF) {
        if ((p - bufptr) > (size - 1)) {
            size = size + 128;
            bufptr = realloc(bufptr, size);
            if (bufptr == NULL) {
                return -1;
            }
        }
        *p++ = c;
        if (c == '\n') {
            break;
        }
        c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}



/*
 ==========================================================================

 mkSvgGraph

 parses template.svg and writes results in said template

 ==========================================================================
*/

void mkSvgGraph( point_t points[] ) {
    FILE *input = fopen ( "template.svg", "r" );
    FILE *target = fopen ( "output.svg", "w" );
    char *line = NULL;
   // char *ptr;
    size_t len = 0;
    ssize_t read;
    char values[64];
    char firstGraph[15] = {"<path d=\"M0 0"};   /* There is a space behind  the last zero, 
    					      but windows does not recognize it in strtsr() ?!
					      so no output will return*/

    if( input == NULL ) {
        exit(EXIT_FAILURE);
    }
   
    while ( ( read = getline(&line, &len, input) ) != -1) {
       //printf("Retrieved line of length %zu :\n", read);
       //puts(line); // debug purpose 
	fprintf(target, "%s",line);
   	if( strstr(line, firstGraph) != NULL ) { 	    
	//fprintf(target,"HECK!!!\n");
	myLogger( target, points );
	}	
    }

   free(line);
  // free(target);
  // free(input);
   //exit(EXIT_SUCCESS);
}

