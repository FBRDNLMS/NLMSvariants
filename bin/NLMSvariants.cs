using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace NMLS_Graphisch
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        static int NumberOfSamples = 1000;
        const int tracking = 40;
        static Stack<double> x = new Stack<double>();
        static Random rnd = new Random();
        static double[] _x = new double[NumberOfSamples];
        static double[,] w = new double[NumberOfSamples, NumberOfSamples];
        static double learnrate = 0.2;
        static double[] pixel_array;
        static int windowSize = 5;

        private void button1_Click(object sender, EventArgs e)
        {

            NumberOfSamples = Int32.Parse(comboBox2.SelectedItem.ToString());
            chart1.ChartAreas[0].AxisX.Maximum = NumberOfSamples;
            chart1.ChartAreas[0].AxisY.Maximum = 300;
            chart1.ChartAreas[0].AxisY.Minimum = -5;

            if (checkBox1.Checked)
            {
                for (int i = 0; i < tracking; i++)
                {
                    for (int k = 0; k < windowSize; k++)
                    {
                        File.AppendAllText("weights.txt",
                            String.Format("[{0}][{1}] {2}\n", k, i, Math.Round(w[k, i], 2).ToString()),
                            Encoding.UTF8);
                    }
                }
            }

            Series[] series = new Series[2];
            switch (comboBox1.SelectedItem.ToString())
            {

                case "lokaler Mittelwert":
                    series = lokalerMittelWert();
                    break;

                case "direkter Vorgänger":
                    series = direkterVorgaenger();
                    break;

                case "differenzieller Vorgänger":
                    series = diffVorgaenger();
                    break;

                default:
                    return;

            }

            foreach (Series s in series)
            {
                if (chart1.Series.IndexOf(s.Name) < 0)
                {
                    chart1.Series.Add(s);
                }
                else
                {
                    chart1.Series.RemoveAt(chart1.Series.IndexOf(s.Name));
                    chart1.Series.Add(s);
                }
            }

            if (checkBox1.Checked)
            {
                for (int i = 0; i < tracking; i++)
                {
                    for (int k = 0; k < windowSize; k++)
                    {
                        File.AppendAllText("weights_after.txt",
                            String.Format("[{0}][{1}] {2}\n", k, i, Math.Round(w[k, i], 2).ToString()),
                            Encoding.UTF8);
                    }

                }
            }


        }

        Series[] lokalerMittelWert()
        {
            int x_count = 0;
            double[] x_error = new double[NumberOfSamples];
            x_error[0] = 0;

            //Graphischer Stuff
            Series lokal_M_error = new Series("Lokaler Mittelwert Error");
            Series lokal_M_predic = new Series("Lokaler Mittelwert Prediction");
            lokal_M_error.ChartType = SeriesChartType.Spline;
            lokal_M_predic.ChartType = SeriesChartType.Spline;

            while (x_count + 1 < NumberOfSamples)
            {
                double[] x_part_Array = new double[x_count];

                int _sourceIndex = (x_count > windowSize) ? x_count - windowSize : 0;
                int _arrayLength = (x_count > windowSize) ? windowSize + 1 : (x_count > 0) ? x_count : 0;

                Array.Copy(_x, _sourceIndex, x_part_Array, 0, _arrayLength);                               
                double x_middle = (x_count > 0) ? ( x_part_Array.Sum() / _arrayLength) : 0;
                double x_pred = 0.0;
                double[] x_array = _x;
                double x_actual = _x[x_count + 1];

                for (int i = 1; i < _arrayLength; i++)
                {
                    x_pred += (w[i, x_count] * (x_array[x_count - i] - x_middle));
                }
                x_pred += x_middle;

                // Output Stuff
                if (checkBox1.Checked)
                {
                    File.AppendAllText("lokalerMittelwert.txt",
                   String.Format("{0}. X_pred {1}\n", x_count, x_pred),
                   Encoding.UTF8);

                    File.AppendAllText("lokalerMittelwert.txt",
                        String.Format("{0}. X_actual {1}\n", x_count, x_actual),
                        Encoding.UTF8);
                }

                x_error[x_count] = x_actual - x_pred;
                double x_square = 0;

                //Output Stuff
                if (checkBox1.Checked)
                {
                    File.AppendAllText("lokalerMittelwert.txt",
                    String.Format("{0}. X_error {1}\n\n", x_count, x_error[x_count]),
                    Encoding.UTF8);
                }

                for (int i = 1; i < _arrayLength; i++)
                {
                    x_square += Math.Pow(x_array[x_count - i] - x_middle, 2);
                }

                for (int i = 1; i < _arrayLength; i++)
                {
                    w[i, x_count + 1] = w[i, x_count] + learnrate * x_error[x_count] * ((x_array[x_count - i] - x_middle) / x_square);
                }

                // Graphischer Stuff
                lokal_M_error.Points.AddXY(x_count, x_error[x_count]);
                lokal_M_predic.Points.AddXY(x_count, x_pred);

                x_count += 1;
            }
            double mittel = x_error.Where(d => !double.IsNaN(d)).Sum() / x_error.Length;
            double varianz = 0.0;
            foreach (double x_e in x_error)
            {
                if(!double.IsNaN(x_e))
                    varianz += Math.Pow(x_e - mittel, 2);
            }
            varianz /= x_error.Length;
            if (checkBox1.Checked)
            {
                File.AppendAllText("ergebnisse.txt",
                        String.Format("Quadratische Varianz(x_error): {0}\n Mittelwert(x_error): {1}\n\n", varianz, mittel),
                        Encoding.UTF8);
            }
            return new Series[] { lokal_M_predic, lokal_M_error };
        }

        Series[] direkterVorgaenger()
        {
            double[] x_error = new double[NumberOfSamples];
            x_error[0] = 0;
            int x_count = 0;

            // Graphischer Stuff
            Series direkterVorgaenger_error = new Series("Direkter Vorgänger Error");
            Series direkterVorgaenger_predic = new Series("Direkter Vorgänger Prediction");
            direkterVorgaenger_error.ChartType = SeriesChartType.Spline;
            direkterVorgaenger_predic.ChartType = SeriesChartType.Spline;

            while (x_count + 1 < NumberOfSamples)
            {
                double x_pred = 0.0;
                double[] x_array = _x;
                double x_actual = _x[x_count + 1];

                if (x_count > 0)
                {
                    int _arrayLength = (x_count > windowSize) ? windowSize + 1 : x_count;

                    for (int i = 1; i < _arrayLength; i++)
                    {
                        x_pred += (w[i, x_count] * (x_array[x_count - 1] - x_array[x_count - i - 1]));
                    }
                    x_pred += x_array[x_count - 1];

                    // Output Stuff
                    if (checkBox1.Checked)
                    {
                        File.AppendAllText("direkterVorgaenger.txt",
                       String.Format("{0}. X_pred {1}\n", x_count, x_pred),
                       Encoding.UTF8);

                        File.AppendAllText("direkterVorgaenger.txt",
                            String.Format("{0}. X_actual {1}\n", x_count, x_actual),
                            Encoding.UTF8);
                    }


                    x_error[x_count] = x_actual - x_pred;

                    // Output Stuff
                    if (checkBox1.Checked)
                    {
                        File.AppendAllText("direkterVorgaenger.txt",
                        String.Format("{0}. X_error {1}\n\n", x_count, x_error[x_count]),
                        Encoding.UTF8);
                    }

                    double x_square = 0;
                    for (int i = 1; i < _arrayLength; i++)
                    {
                        x_square += Math.Pow(x_array[x_count - 1] - x_array[x_count - i - 1], 2);
                    }

                    for (int i = 1; i < _arrayLength; i++)
                    {
                        w[i, x_count + 1] = w[i, x_count] + learnrate * x_error[x_count] * ((x_array[x_count - 1] - x_array[x_count - i - 1]) / x_square);
                    }
                }

                //Graphischer Stuff
                direkterVorgaenger_error.Points.AddXY(x_count, x_error[x_count]);
                direkterVorgaenger_predic.Points.AddXY(x_count, x_pred);

                x_count += 1;
            }
            double mittel = x_error.Where(d => !double.IsNaN(d)).Sum() / x_error.Length;
            double varianz = 0.0;
            foreach (double x_e in x_error)
            {
                if (!double.IsNaN(x_e))
                    varianz += Math.Pow(x_e - mittel, 2);
            }
            varianz /= x_error.Length;
            if (checkBox1.Checked)
            {
                File.AppendAllText("ergebnisse.txt",
                        String.Format("Quadratische Varianz(x_error): {0}\n Mittelwert(x_error): {1}\n\n", varianz, mittel),
                        Encoding.UTF8);
            }

            return new Series[] { direkterVorgaenger_error, direkterVorgaenger_predic };
        }

        Series[] diffVorgaenger()
        {
            double[] x_error = new double[NumberOfSamples];
            x_error[0] = 0;
            int x_count = 1;

            //Graphischer Stuff
            Series diffVorgaenger_error = new Series("Differenzieller Vorgänger Error");
            Series diffVorgaenger_predic = new Series("Differenzieller Vorgänger Prediction");
            diffVorgaenger_error.ChartType = SeriesChartType.Spline;
            diffVorgaenger_predic.ChartType = SeriesChartType.Spline;

            while (x_count + 1 < NumberOfSamples)
            {
                double x_pred = 0.0;
                double[] x_array = _x;
                double x_actual = _x[x_count + 1];
                if (x_count > 0)
                {
                    int _arrayLength = (x_count > windowSize) ? windowSize + 1 : x_count;

                    for (int i = 1; i < _arrayLength; i++)
                    {
                        x_pred += (w[i, x_count] * (x_array[x_count - i] - x_array[x_count - i - 1]));
                    }
                    x_pred += x_array[x_count - 1];

                    // Output Stuff
                    if (checkBox1.Checked)
                    {
                        File.AppendAllText("differenziellerVorgaenger.txt",
                       String.Format("{0}. X_pred {1}\n", x_count, x_pred),
                       Encoding.UTF8);

                        File.AppendAllText("differenziellerVorgaenger.txt",
                            String.Format("{0}. X_actual {1}\n", x_count, x_actual),
                            Encoding.UTF8);
                    }


                    x_error[x_count] = x_actual - x_pred;

                    // Output Stuff
                    if (checkBox1.Checked)
                    {
                        File.AppendAllText("differenziellerVorgaenger.txt",
                        String.Format("{0}. X_error {1}\n\n", x_count, x_error[x_count]),
                        Encoding.UTF8);
                    }


                    double x_square = 0;
                    for (int i = 1; i < _arrayLength; i++)
                    {
                        x_square += Math.Pow(x_array[x_count - i] - x_array[x_count - i - 1], 2);
                    }

                    for (int i = 1; i < _arrayLength; i++)
                    {
                        w[i, x_count + 1] = w[i, x_count] + learnrate * x_error[x_count] * ((x_array[x_count - i] - x_array[x_count - i - 1]) / x_square);
                    }

                }

                //Graphischer Stuff
                diffVorgaenger_error.Points.AddXY(x_count, x_error[x_count]);
                diffVorgaenger_predic.Points.AddXY(x_count, x_pred);

                x_count += 1;
            }
            double mittel = x_error.Where(d => !double.IsNaN(d)).Sum() / x_error.Length;
            double varianz = 0.0;
            foreach (double x_e in x_error)
            {
                if (!double.IsNaN(x_e))
                    varianz += Math.Pow(x_e - mittel, 2);
            }
            varianz /= x_error.Length;

            if (checkBox1.Checked)
            {
                File.AppendAllText("ergebnisse.txt",
                        String.Format("Quadratische Varianz(x_error): {0}\n Mittelwert(x_error): {1}\n\n", varianz, mittel),
                        Encoding.UTF8);
            }

            return new Series[] { diffVorgaenger_error, diffVorgaenger_predic };
        }

        // Inizialisierung von Arrays
        private void Form1_Load(object sender, EventArgs e)
        {
            comboBox1.SelectedIndex = 0;
            comboBox2.SelectedIndex = 0;
            chart1.Series.Clear();
            Series x_actual = new Series("Actual x Value");
            x_actual.ChartType = SeriesChartType.Spline;
            for (int i = 0; i < NumberOfSamples; i++)
            {
                _x[i] += ((255.0 / NumberOfSamples) * i);
                for (int k = 0; k < windowSize; k++)
                {
                    w[k, i] = rnd.NextDouble();
                    //Console.WriteLine(String.Format("Weight[{0}, {1}]: {2}",k,i, w[k, i]));
                }
                x_actual.Points.AddXY(i, _x[i]);
            }
            chart1.Series.Add(x_actual);
        }

        // Graphen Clearen
        private void button2_Click(object sender, EventArgs e)
        {
            chart1.Series.Clear();
            Series x_actual = new Series("Actual x Value");
            x_actual.ChartType = SeriesChartType.Spline;
            for (int i = 0; i < NumberOfSamples; i++)
            {
                x_actual.Points.AddXY(i, _x[i]);
            }
            chart1.Series.Add(x_actual);
        }

        // Bild Laden
        private void button3_Click(object sender, EventArgs e)
        {

            OpenFileDialog openFileDialog = new OpenFileDialog();

            if(openFileDialog.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    Bitmap img = new Bitmap(openFileDialog.FileName);
                    pixel_array = new double[img.Width * img.Height];

                    for (int i = 1; i < img.Width; i++)
                    {
                        for (int j = 1; j < img.Height; j++)
                        {

                            Color pixel = img.GetPixel(i, j);
                            pixel_array[j*i] = pixel.R;

                        }
                    }
                    NumberOfSamples = (img.Width * img.Height) / 2;
                    comboBox2.Items.Add(NumberOfSamples);
                    _x = pixel_array;
                    w = new double[NumberOfSamples, NumberOfSamples];
                    for (int i = 0; i < NumberOfSamples; i++)
                    {
                        for (int k = 1; k < NumberOfSamples; k++)
                        {
                            w[k, i] = rnd.NextDouble();
                        }
                    }
                }
                catch(Exception exep)
                {
                    MessageBox.Show("Konnte Bild nicht laden.");
                    MessageBox.Show(String.Format("{0}", exep.ToString()));
                }
            }
        }
    }

}

