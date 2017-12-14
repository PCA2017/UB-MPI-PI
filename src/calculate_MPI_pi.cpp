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
#include <stdlib.h>     // strtod
#include <mpi.h>
using namespace std;

int main(int argc, char **argv) {

	// Zeitmessung
	struct timeval t_start, t_stop;
	long seconds, useconds;
	double duration;

	// MPI Variables
	int rank, size, error;
	double pi = 0.0, sum = 0.0, partial_sum = 0.0;
	int seg_start, seg_end; // Start und Ende eines Segments den ein Slave berechnet


	// Everybody gets the params
	int i, n, num_tasks;

	n = int(strtod(argv[1], NULL));          // Anzahl der Elemente
	num_tasks = int(strtod(argv[2], NULL));  // Anzahl der Segmente


	// Get startet with MPI
	error = MPI_Init(&argc, &argv);

	//Get process ID
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//Get processes Number
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//Synchronize all processes
	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0) //Master
	{
		// Der Master berechnet die Zu berechnenden Wertebereich für jeden Slave

		// Vektor mit Startwerten
		int * vs;
		// Ergebnisvektor mit Endwerten
		int * ve;

		vs= new (nothrow) int[num_tasks];
		ve= new (nothrow) int[num_tasks];

		// Calculate the Starts and Ends of the segments the different Tasks calculate
		// Bisschen unübersichtlich mit den Indices da der Master nichts zum berechnen kriegt.
		int start = 0;
		int end;
		for(i = 0; i <= (num_tasks-2); i++)
		{
			end = round((double(n) / double(num_tasks-1)) * double(i + 1));
			vs[i+1] = start;
			ve[i+1] = end;
			start = end + 1;
		}

		// Werte an Slaves senden
		for(i = 1; i <= (num_tasks-1); i++)
		{
			MPI_Send(&vs[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&ve[i], 1, MPI_INT, i, 1, MPI_COMM_WORLD);
		}
	}

	// Slaves empfangen die gesendeten Werte
	if(rank != 0)
	{
		MPI_Recv(&seg_start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);
		MPI_Recv(&seg_end, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, NULL);
	}

	//Synchronize all processes
	MPI_Barrier(MPI_COMM_WORLD);

	if(rank == 0)
	{
		// Zeit stoppen
		gettimeofday(&t_start, NULL);
	}

	//Each Slave will caculate a part of the sum
	if(rank != 0) // Slaves
	{
		for (i = seg_start; i <= seg_end; i++)
			partial_sum += (1.0/(2*i + 1)) * pow ((-1), i);

		cout << "Rank: " << rank << " Partial Sum " << partial_sum << " Startindex: " <<  seg_start << " Endindex: " << seg_end << endl;
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

		cout << "Knotenanzahl: " << size << " Pi=" << pi << " Dauer: " <<  duration << " Sekunden" << endl;
	}

	error = MPI_Finalize();

	if (error != 0)
		cout << "Error: " << error << endl;

	return 0;
}
