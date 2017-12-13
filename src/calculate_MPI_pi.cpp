//============================================================================
// Name        : calculate_MPI_pi.cpp
// Author      : pra01
// Description : Calculate Pi MPI-Style.
//============================================================================

#include <iostream>
#include <new>
#include <sys/time.h>   // gettimeofday
#include <math.h>       // round
#include <cmath>        // pow
#include <mpi.h>
using namespace std;

//int main(int argc, char **argv) {
int main (int argc, char* argv[]) {

	// Zeitmessung
	struct timeval t_start, t_stop;   // Zeitmessung mit gettimeofday
	long seconds, useconds;
	double duration;

	// MPI Variables
	int rank, size, error;
	double pi = 0.0, sum = 0.0, partial_sum = 0.0;


	// Everybody calculates the beginning and end of every segment. -----------------------
	int i, n, num_tasks;
	int * vs;
	int * ve;

	//n = atoi(argv[1]);          // Anzahl der Elemente
	//num_tasks = atoi(argv[2]);  // Anzahl der Segmente

	n = 10000;          // Anzahl der Elemente
	num_tasks = 64;  // Anzahl der Segmente

	// Vektor mit Startwerten
	vs= new (nothrow) int[num_tasks];
	// Ergebnisvektor mit Endwerten
	ve= new (nothrow) int[num_tasks];

	// Calculate the Starts and Ends of the segments the different Tasks calculate
	int start = 0;
	int end;
	for(i = 1; i < num_tasks; i++)
	{
		end = round((double(n) / double(num_tasks)) * double(i + 1));
		vs[i] = start;
		ve[i] = end;
		start = end + 1;
	}
	// -------------------------------------------------------------------------------------


	// Get startet with MPI
	error = MPI_Init(&argc, &argv);

	//Get process ID
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//Get processes Number
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//Synchronize all processes and get the begin time
	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0) //Master
	{
		// Zeit stoppen
		gettimeofday(&t_start, NULL);
	}

	//Each Slave will caculate a part of the sum
	if(rank != 0) // Slaves
	{
		for (i = vs[rank]; i <= ve[rank]; i++)
		{
		partial_sum += (1.0/(2*i + 1)) * pow ((-1), i);
//		if ((n%2) == 0)
//			partial_sum += double(1.0/(2*i + 1));
//		else
//			partial_sum -= double(1.0/(2*i + 1));
		}
		//cout << "Rank: " << rank << " Partial Sum " << partial_sum << " Startindex: " <<  vs[rank] << " Endindex: " << ve[rank] << endl;
	}

	//Sum up all results
	MPI_Reduce(&partial_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	//Synchronize all processes and get the end time
	MPI_Barrier(MPI_COMM_WORLD);

	//Caculate and print PI
	if (rank == 0) //Master
	{
		pi = 4 * sum; // sum = atan(1)

		// Zeit stoppen
		gettimeofday(&t_stop, NULL);
		// Dauer berechnen
		seconds  = t_stop.tv_sec  - t_start.tv_sec;
		useconds = t_stop.tv_usec - t_start.tv_usec;
		duration = seconds + useconds/1000000.0;  // Dauer in Sekunden

		cout << "Knotenanzahl: " << size << " Pi= " << pi << " Dauer: " <<  duration << " Sekunden" << endl;
	}

	error = MPI_Finalize();

	return 0;
}
