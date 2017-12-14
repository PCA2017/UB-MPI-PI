/*
 * test_send_recieve.cpp
 *
 *  Created on: Dec 13, 2017
 *      Author: pra01
 */

#include <iostream>
#include <new>
#include <sys/time.h>   // gettimeofday
#include <math.h>       // round
#include <cmath>        // pow
#include <stdlib.h>     // strtod
#include <mpi.h>
using namespace std;


int main(int argc, char **argv) {

	// MPI Variables
	int rank, size, error;
	int meine_Zahl = 0;

	int n = 4;
	int * v1;

	// Vektor mit Startwerten
	v1= new (nothrow) int[n];

	v1[0] = 0;
	v1[1] = 10;
	v1[2] = 20;
	v1[3] = 30;

	int a = 0, b=10, c=20, d=30;

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
		MPI_Send(&b, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Send(&c, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		MPI_Send(&d, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
	}

	if (rank != 0) //Slave
	{
		//cout << "Rank: " << rank << endl;

		//MPI_Status s;

		MPI_Recv(&meine_Zahl, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);

		//cout << "Rank: " << rank << endl;

	}

	//Synchronize all processes
	MPI_Barrier(MPI_COMM_WORLD);


	cout << "Rank: " << rank << " Value: " << meine_Zahl << endl;

	error = MPI_Finalize();

	if (error != 0)
		cout << "Error: " << error << endl;

	return 0;
}

