#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>



double *load_data (FILE *stream, size_t *n)
{
  size_t size = 0;
  size_t loaded_elements;
  double *data;
  float *data_temp;
  const uint32_t mask = 1 <<31;
  //Wczytaj pierwsze 32 bity
  uint32_t *header = (uint32_t*) malloc (sizeof (uint32_t));
  size_t result = fread (header, sizeof (uint32_t), 1, stream);
  
  if(!result)
  {
    fputs("Nie udało się odczytać nagłówka tablicy", stderr);
    exit (1);
  }

  //Sprawdź liczbę elementów w pliku binarnym
  size = header[0]&(~mask);
  size = 100;
  data = (double*) malloc (sizeof (double)*size);

  //Sprawdź czy pierwszy bit jest równy 0 czy 1
  if(header[0]&mask)
  {
    loaded_elements = fread (data, sizeof (double), size, stream);
  }
  else
  {
    data_temp = (float*)malloc(sizeof(float)*size);
    loaded_elements = fread (data_temp, sizeof (float), size, stream);
    for (int i =0; i<loaded_elements; i++)
      data[i]=data_temp[i];
    free(data_temp);
  }

  if (loaded_elements < size)
  {
    fputs ("Nie udało się odczytać całej tablicy!", stderr);
    exit(1);
  }
  *n = size;
  return data;
}

void mean_std (const double data[], size_t n, double *m, double *s)
{
  double mean = 0;
  for (int i = 0; i < n; i++)
    mean += data [i];
  mean = mean / n;

  double sigma = 0;
  for (int i = 0; i < n; i++)
    sigma += (data [i] - mean) * (data [i] - mean);
  *m = mean;
  *s = sqrt (sigma / (n - 1));
}

void sort (double data[], size_t n, double *max, double *min)
{
  int tmp;
  int change = 0;
  for (int i = 0; i < n-1; i++)
  {
    change = 0;
    for (int j = 0; j < n-1-i; j++)
    {
      if (data[j+1] < data[j])
      {  // odwracamy kolejność dwóch elementów
      tmp = data[j];
      data[j] = data[j+1];
      data[j+1] = tmp;
      change = 1;
      }
    }
    if(!change) break;
  }
*min = data [0];
*max = data[n-1];
}

double median (const double data[], size_t n)
{
  if (n % 2)
    return data[(n-1)/2];
  else
    return (data[n/2]+data[n/2-1])/2;
}

int main(void)
{
  size_t n;
  double mean, std, max, min, med;
  double *data = load_data (stdin, &n);
  mean_std (data, n, &mean, &std);
  sort(data, n, &max, &min);
  med = median(data, n);
  printf ("n=%lu, mean=%e, std=%e, max=%e, min=%e, median=%e\n", n, mean, std, max, min, med);
  free(data);
  return 0;
}