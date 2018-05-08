
// Variable mit der anzahl Punkten zwischen 0 - 255
static int M = 1000;

// Variable zum tracken der Gewichte
const int tracking = 40;

// C# only zum erstellen von Randomzahlen
static Random rnd = new Random();

// Array mit den Testwerten
static double[] _x = new double[M];

// Array mit den Gewichten
static double[,] w = new double[M, M];

// Lernrate
static double learnrate = 1;

/**************************************************************
main() des Programms
***************************************************************/

int main(){

	// Initialisierung des Test Array + Gewichte
	 for (int i = 0; i < M; i++)
	{
		_x[i] += ((255.0 / M) * i);
		for (int k = 1; k < M; k++)
		{
			w[k, i] = rnd.NextDouble();
			//Console.WriteLine(String.Format("Weight: {0}", w[k, i]));
		}
		
	}
	
	// Zum erstellen eines Files mit den Gewichten vor den Updates
	for (int i = 0; i < tracking; i++)
	{
		for (int k = 1; k < tracking; k++)
		{
			File.AppendAllText("weights.txt",
				String.Format("[{0}][{1}] {2}\n", k, i, Math.Round(w[k, i], 2).ToString()),
				Encoding.UTF8);
		}
	}
	
	// Variante die Ausgeführt werden soll
	lokalerMittelWert();
	
	// Zum erstellen eines Files mit den Gewichten nach den Updates
	for (int i = 0; i < tracking; i++)
	{
		for (int k = 1; k < tracking; k++)
		{
			File.AppendAllText("weights_after.txt",
				String.Format("[{0}][{1}] {2}\n", k, i, Math.Round(w[k, i], 2).ToString()),
				Encoding.UTF8);
		}

	}


}
		
/**************************************************************
Errechnet die 1. Variante, mit abziehen des lokalen Mittelwertes
***************************************************************/

void lokalerMittelWert()
{
	//Array in dem e(n) gespeichert wird
	double[] x_error = new double[M];
	
	//Laufzeitvariable
	int x_count = 0;
	
	// x_count + 1 da x_count > 0 sein muss
	while (x_count + 1 < M)
	{
		//Erstellt ein neues Array mit allen werten bis zur Laufzeitvariable
		double[] x_part_Array = new double[x_count];
		Array.Copy(_x, 0, x_part_Array, 0, x_count);                                  
		double x_middle = (x_count > 0) ? ( x_part_Array.Sum() / x_count) : 0;
		
		// Variable für die errechnete Zahl
		double x_pred = 0.0;
		
		// Variable mit der eigentlichen Zahl
		double x_actual = _x[x_count + 1];

		for (int i = 1; i < x_count; i++)
		{
			x_pred += (w[i, x_count] * (_x[x_count - i] - x_middle));
		}
		x_pred += x_middle;

		//Console.WriteLine(String.Format("X_sum: {0}", x_middle));

		//Console.WriteLine(String.Format("X_pred: {0}", x_pred));
		//Console.WriteLine(String.Format("X_actual: {0}", x_actual));

		x_error[x_count] = x_actual - x_pred;
		
		// Funktion zum berechnen des Quadrates
		double x_square = 0;
		for (int i = 1; i <= x_count; i++)
		{
			x_square += Math.Pow(_x[x_count - i], 2);
		}

		// Funktion zum updaten der Gewichte
		for (int i = 1; i < x_count; i++)
		{
			w[i, x_count + 1] = w[i, x_count] + learnrate * x_error[x_count] * (_x[x_count - i] / x_square);
		}
		
		// Laufzeitvariable hochzählen
		x_count += 1;
	}

	// Berechenen des mittleren Fehlers
	double mittel = x_error.Sum() / x_error.Length;
	
	// Berechenen der varianz des Fehlers
	double varianz = 0.0;
	foreach (double x_e in x_error)
	{
		varianz += Math.Pow(x_e - mittel, 2);
	}
	varianz /= x_error.Length;
	
	// Hängt dem Angegebenen File den Vorgegebenen String an
	File.AppendAllText("ergebnisse.txt",
			String.Format("Quadratische Varianz(x_error): {0}\n Mittelwert(x_error): {1}\n\n", varianz, mittel),
			Encoding.UTF8);
}

/**************************************************************
Errechnet die 2. Variante, mit abziehen des direkten Vorgängers
***************************************************************/
		
void direkterVorgaenger()
{
	//Array in dem e(n) gespeichert wird
	double[] x_error = new double[M];
	
	//Laufzeitvariable
	int x_count = 0;
	
	// x_count + 1 da x_count > 0 sein muss
	while (x_count + 1 < M)
	{
		// Variable für die errechnete Zahl
		double x_pred = 0.0;
		
		// Variable mit der eigentlichen Zahl
		double x_actual = _x[x_count + 1];

		// Funktion fürs berechnen der Vorhersagezahl
		for (int i = 1; i < x_count; i++)
		{
			x_pred += (w[i, x_count] * (_x[x_count - 1] - _x[x_count - i - 1]));
		}
		x_pred += _x[x_count - 1];

		//Console.WriteLine(String.Format("X_pred: {0}", x_pred));

		// Hängt dem Angegebenen File den Vorgegebenen String an
		File.AppendAllText("direkterVorgaenger.txt",
		   String.Format("{0}. X_pred {1}\n", x_count, x_pred),
		   Encoding.UTF8);
		   
		//Console.WriteLine(String.Format("X_actual: {0}", x_actual));
		
		// Hängt dem Angegebenen File den Vorgegebenen String an
		File.AppendAllText("direkterVorgaenger.txt",
			String.Format("{0}. X_actual {1}\n", x_count, x_actual),
			Encoding.UTF8);

		// Berechnung des Fehlers
		x_error[x_count] = x_actual - x_pred;
		
		
		//Console.WriteLine(String.Format("X_error: {0}", x_error));
		
		// Hängt dem Angegebenen File den Vorgegebenen String an
		File.AppendAllText("direkterVorgaenger.txt",
		String.Format("{0}. X_error {1}\n\n", x_count, x_error),
		Encoding.UTF8);
		
		// Funktion zum berechnen des Quadrates
		double x_square = 0;
		for (int i = 1; i < x_count; i++)
		{
			x_square += Math.Pow(_x[x_count - 1] - _x[x_count - i - 1], 2);
		}
		
		//Console.WriteLine(String.Format("X_square: {0}", x_square));
		
		// Hängt dem Angegebenen File den Vorgegebenen String an
		//File.AppendAllText("direkterVorgaenger.txt",
		//    String.Format("{0}. X_square {1}\n", x_count, x_square),
		//    Encoding.UTF8);
		
		// Funktion zum updaten der Gewichte
		for (int i = 1; i < x_count; i++)
		{
			w[i, x_count + 1] = w[i, x_count] + learnrate * x_error[x_count] * ((_x[x_count - 1] - _x[x_count - i - 1]) / x_square);
		}
		
		// Laufzeitvariable hochzählen
		x_count += 1;
	}
	
	// Berechenen des mittleren Fehlers
	double mittel = x_error.Sum() / x_error.Length;
	
	// Berechenen der varianz des Fehlers
	double varianz = 0.0;
	foreach (double x_e in x_error)
	{
		varianz += Math.Pow(x_e - mittel, 2);
	}
	varianz /= x_error.Length;
	
	// Hängt dem Angegebenen File den Vorgegebenen String an
	File.AppendAllText("ergebnisse.txt",
			String.Format("Quadratische Varianz(x_error): {0}\n Mittelwert(x_error): {1}\n\n", varianz, mittel),
			Encoding.UTF8);
}

/**************************************************************
Errechnet die 3. Variante, mit abziehen des differenziellen Vorgängers
***************************************************************/

void differenziellerVorgaenger()
{
    //Array in dem e(n) gespeichert wird
    double[] x_error = new double[M];

    //Laufzeitvariable
    int x_count = 0;

    // x_count + 1 da x_count > 0 sein muss
    while (x_count + 1 < M)
    {
        // Variable für die errechnete Zahl
        double x_pred = 0.0;

        // Variable mit der eigentlichen Zahl
        double x_actual = _x[x_count + 1];

        // Funktion fürs berechnen der Vorhersagezahl
        for (int i = 1; i < x_count; i++)
        {
            x_pred += (w[i, x_count] * (_x[x_count - i] - _x[x_count - i - 1]));
        }
        x_pred += _x[x_count - 1];

        //Console.WriteLine(String.Format("X_pred: {0}", x_pred));

        // Hängt dem Angegebenen File den Vorgegebenen String an
        File.AppendAllText("differenziellerVorgaenger.txt",
           String.Format("{0}. X_pred {1}\n", x_count, x_pred),
           Encoding.UTF8);

        //Console.WriteLine(String.Format("X_actual: {0}", x_actual));

        // Hängt dem Angegebenen File den Vorgegebenen String an
        File.AppendAllText("differenziellerVorgaenger.txt",
            String.Format("{0}. X_actual {1}\n", x_count, x_actual),
            Encoding.UTF8);

        // Berechnung des Fehlers
        x_error[x_count] = x_actual - x_pred;


        //Console.WriteLine(String.Format("X_error: {0}", x_error));

        // Hängt dem Angegebenen File den Vorgegebenen String an
        File.AppendAllText("differenziellerVorgaenger.txt",
        String.Format("{0}. X_error {1}\n\n", x_count, x_error),
        Encoding.UTF8);

        // Funktion zum berechnen des Quadrates
        double x_square = 0;
        for (int i = 1; i < x_count; i++)
        {
            x_square += Math.Pow(_x[x_count - i] - _x[x_count - i - 1], 2);
        }

        //Console.WriteLine(String.Format("X_square: {0}", x_square));

        // Hängt dem Angegebenen File den Vorgegebenen String an
        //File.AppendAllText("direkterVorgaenger.txt",
        //    String.Format("{0}. X_square {1}\n", x_count, x_square),
        //    Encoding.UTF8);

        // Funktion zum updaten der Gewichte
        for (int i = 1; i < x_count; i++)
        {
            w[i, x_count + 1] = w[i, x_count] + learnrate * x_error[x_count] * ((_x[x_count - i] - _x[x_count - i - 1]) / x_square);
        }

        // Laufzeitvariable hochzählen
        x_count += 1;
    }

    // Berechenen des mittleren Fehlers
    double mittel = x_error.Sum() / x_error.Length;

    // Berechenen der varianz des Fehlers
    double varianz = 0.0;
    foreach (double x_e in x_error)
    {
        varianz += Math.Pow(x_e - mittel, 2);
    }
    varianz /= x_error.Length;

    // Hängt dem Angegebenen File den Vorgegebenen String an
    File.AppendAllText("ergebnisse.txt",
            String.Format("Quadratische Varianz(x_error): {0}\n Mittelwert(x_error): {1}\n\n", varianz, mittel),
            Encoding.UTF8);
}