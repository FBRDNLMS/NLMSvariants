//
//  main.c
//  NLMS
//
//  Created by Stefan Fiese on 26.04.18.
//  Copyright © 2018 Stefan Fiese. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define M 1000
#define tracking 40; //Count of weights
//static Stack<double> x = new Stack<double>();
//static Random rnd = new Random();
//static double[] _x = new double[M];
//static double[,] w = new double[M, M];
#define learnrate 1.0

double x[] ={0};
double _x[M] = {0};
double w [M][M]={{0},{0}};

char filename(void);
double sum_array(double x[], int length);
void direkterVorgaenger(void);
void lokalerMittelWert(void);

double r2()
{
    return((rand() % 10000) / 10000.0);
}

int rnd()
{
    double u;
    u = r2();
    return u;
}




int main(int argc, char **argv ) {

    int i = 0;
    for (i = 0; i < M; i++) {
        _x[i] += ((255.0 / M) * i);
        for (int k = 1; k < M; k++)
        {
            w[k][i] = rnd();
            //Console.WriteLine(String.Format("Weight: {0}", w[k, i]));
        }
        
    }
    
    for (i = 0; i < tracking; i++){
        for (int k = 1; k < tracking; k++)
        {
            const char *name = fileName();
            FILE *fp = fopen(*name,"wb+");
            File.AppendAllText("weights.txt",
                               String.Format("[{0}][{1}] {2}\n", k, i, Math.Round(w[k, i], 2).ToString()),
                               Encoding.UTF8);
        }
    }
    
    direkterVorgaenger();
    
    for (i = 0; i < tracking; i++) {
        for (int k = 1; k < tracking; k++) {
            File.AppendAllText("weights_after.txt",
                               String.Format("[{0}][{1}] {2}\n", k, i, Math.Round(w[k, i], 2).ToString()),
                               Encoding.UTF8);
        }
        
    }
    
    
    getchar();
    
}

void lokalerMittelWert()
{
    int i;
    for (i=1; i < M; i++){
   // while (x.Count + 1 < M)
        double x_pred = 0.0;
        double x_middle = (i > 0) ? sum_array(x,i) / i : 0;
        double x_actual = _x[i];
        
        for (int j = 1; j < i; j++)
        {
            x_pred += (w[j, i] * (x[i - j] - x_middle));
        }
        x_pred += x_middle;
        
        //Console.WriteLine(String.Format("X_sum: {0}", x_middle));
        
       printf("X_pred: {%f}", x_pred);
       printf("X_actual: {%f}", x_actual);
        
        double x_error = x_actual - x_pred;
        double x_square = 0;
        
        for (int k = 1; k <= i; k++)
        {
            x_square += pow(x[i - k], 2);
        }
        
        for (int l = 1; l < i; l++)
        {
            w[l, i + 1] = w[l, i] + learnrate * x_error * (x[i - l] / x_square);
        }
    }
}

static void direkterVorgaenger()
{
    double x_error[M] = {0};
    int i;
    for(i = 0; i < M; i++)
    //while (x.Count + 1 < M)
    {
        double x_pred = 0.0;
        //double[] x_array = _x;
        double x_actual = _x[i + 1];
        if (x.Count > 0)
        {
            for (int j = 1; j < i; j++)
            {
                x_pred += (w[j, i] * (_x[i - j] - _x[i - j - 1]));
            }
            x_pred += x[i - 1];
            
            //Console.WriteLine(String.Format("X_sum: {0}", x_middle));
            
            //Console.WriteLine(String.Format("X_pred: {0}", x_pred));
            File.AppendAllText("direkterVorgaenger.txt",
                               String.Format("{0}. X_pred {1}\n",x.Count, x_pred),
                               Encoding.UTF8);
            //Console.WriteLine(String.Format("X_actual: {0}", x_actual));
            File.AppendAllText("direkterVorgaenger.txt",
                               String.Format("{0}. X_actual {1}\n", x.Count, x_actual),
                               Encoding.UTF8);
            
            x_error[x.Count] = x_actual - x_pred;
            
            //Console.WriteLine(String.Format("X_error: {0}", x_error));
            File.AppendAllText("direkterVorgaenger.txt",
                               String.Format("{0}. X_error {1}\n\n", x.Count, x_error),
                               Encoding.UTF8);
            double x_square = 0;
            
            for (int k = 1; k < i; k++)
            {
                x_square += pow(_x[i - k] - _x[i - k - 1], 2);
            }
            //Console.WriteLine(String.Format("X_square: {0}", x_square));
            //File.AppendAllText("direkterVorgaenger.txt",
            //    String.Format("{0}. X_square {1}\n", x.Count, x_square),
            //    Encoding.UTF8);
            //File.AppendAllText("x_array.txt",
            //    String.Format("{0}. X_array {1}\n", x.Count, x_array[x.Count - 1]),
            //    Encoding.UTF8);
            for (int l = 1; l < i; l++)
            {
                w[l, i + 1] = w[l, i] + learnrate * x_error[i] * ((_x[i - l] - x_array[i - l - 1]) / x_square);
            }
        }
    }
    int x_error_array_length = sizeof(error_array_length)/sizeof(error_array_length[0])
    double mittel = sum_array(x_error, x_error_array_length) / x_error_array_length;
    double varianz = 0.0;
    for (i = 0; i <= x_error_array_length; i++)
    //foreach(double x_e in x_error)
    {
        varianz += pow(x_e - mittel, 2);
    }
    varianz /= x_error_array_length;
    File.AppendAllText("ergebnisse.txt",
                       String.Format("Quadratische Varianz(x_error): {0}\n Mittelwert(x_error): {1}\n\n", varianz, mittel),
                       Encoding.UTF8);
}


char fileName(char *fname){
    //filename
    char date[34];
    //char name[13] = "_results.txt";
    time_t now;
    now = time(NULL);
    strftime(date, 20, "%Y-%m-%d_%H_%M_%S", localtime(&now));
    strcpy(date,*fname);
    //return &date[0];
    return date;
}

/*
 
 ===================================
 
 sum_array
 
 
 sum of all elements in x
 within a defined length
 
 ====================================
 
 */

double sum_array(double x[], int length){
    //int length = 0;
    double sum = 0.0;
    //length = sizeof(x)/sizeof(x[0]);
    for (i=0; i< length; i++){
        sum += x[i];
    }
    return sum;
}
