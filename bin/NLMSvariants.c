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
#define RESULTS 2
#define DIRECT_PREDECESSOR 3
#define GRAPH 4

double x[] = {0};
double _x[M] = {0};
double w [M][M]={{0},{0}};

/* *file handling* */
char * mkFileName( char* buffer, size_t max_len, int suffixId );
char *fileSuffix( int id );
void myLogger( FILE* fp, int myVar); 

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
        for (int k = 1; k < M; k++){
            w[k][i] = rndm(); // Init weights
        }
    }

    mkFileName( fileName, sizeof(fileName), PURE_WEIGHTS); 
    // save plain test_array before math magic happens
    FILE *fp0 = fopen(fileName,"w");
    for (i = 0; i < tracking; i++){
      for ( int k = 1; k < tracking; k++ ){
        fprintf(fp0, "[%d][%d] %lf\n", k, i, w[k][i]); 
      }
    }
    fclose(fp0);


    // math magic
    directPredecessor(); // TODO: needs some love!
    //localMean();
   

    // save test_array after math magic happened
   // memset( fileName, '\0', sizeof(fileName) );
    mkFileName( fileName, sizeof(fileName), USED_WEIGHTS); 
    FILE *fp1 = fopen(fileName,"w");
    for (i = 0; i < tracking; i++) {
        for (int k = 1; k < tracking; k++) {
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

	double xSquared = 0.0;

	for ( i = 1; i < xCount; i++ ){ //get x squared
          xSquared =+ pow(_x[xCount-i],2);
        }
	
	for ( i - 1; i < xCount; i++ ){ //update weights
	  w[i][xCount+1] = w[i][xCount] + learnrate * xError[xCount] * (_x[xCount - i] / xSquared);
	}
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
    FILE *fp2 = fopen(fileName, "w+"); 
    fprintf(fp2, "quadr. Varianz(x_error): {%f}\nMittelwert:(x_error): {%f}\n\n", deviation, mean);
    fclose(fp2);

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

    for ( xCount = 1; xCount < M+1; xCount++ ){
      xActual = _x[xCount+1]; 
      double xPredicted = 0.0;
      for ( i = 1; i < xCount; i++ ){
        xPredicted += ( w[i][xCount] * ( _x[xCount - i] - _x[xCount - i - 1] ));
     }

      xPredicted += _x[xCount-1]; 
      xError[xCount] = xActual - xPredicted;

      fprintf(fp3, "{%d}.\txPredicted{%f}\txActual{%f}\txError{%f}\n", xCount, xPredicted, xActual, xError[xCount]);
      

      double xSquared = 0.0;
      //get x squared
      for ( i = 1; i < xCount; i++ ){
		xSquared += pow( _x[xCount - i] - _x[xCount - i - 1], 2); // substract direct predecessor
      }	

      for ( i = 1; i < xCount; i++){ 
        w[i][xCount+1] = w[i][xCount] + learnrate * xError[xCount] * ( ( _x[xCount - i - 1] ) / xSquared ); //TODO: double val out of bounds
      }
    }
    int xErrorLength = sizeof(xError) / sizeof(xError[0]);  
    double mean = sum_array(xError, xErrorLength) / xErrorLength;
    double deviation = 0.0;

    for ( i = 0; i < xErrorLength -1; i++ ){
      deviation += pow( xError[i] - mean, 2);
    }

    
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
	char * suffix[] = {"_weights_pure.txt", "_weights_used.txt", "direct_predecessor.txt", "ergebnisse.txt", "_graph.svg"};
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


 Pipes to stdout or files by using a filepointer for logging purposes

 ==========================================================================
 */

void myLogger ( FILE* fp, int myVar ){
	
	fprintf( fp, "Logging: %d\n", myVar);
	
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

 parser

 Parses files, used for template.svg mainly

 ==========================================================================
*/

char * parser( char buffer) {
	    FILE *fp = fopen ( "template.svg", "wr" );
    char *line = NULL;
    char *ptr;
    size_t len = 0;
    ssize_t read;
    char string[] = "HECK!!!";    

    if( fp == NULL ) {
        exit(EXIT_FAILURE);
    }
   
    while ( ( read = getline(&line, &len, fp) ) != -1) {
       // printf("Retrieved line of length %zu :\n", read);
       //puts(line);
   	if( strstr(line, "<path d=\"M0 0 ") != NULL ) { 	    
		ptr = line;
		printf("%s", *ptr);
		
		fprintf(fp, "%s\n", string); 

	}	
    }

   free(line);
    exit(EXIT_SUCCESS);
}

