#include <mpi.h>
#include <iostream>
#include <vector>
#include <tuple>
#include "MpiTypeMaker.h"

using namespace std;
using namespace MpiTypeMaker;


int main()
{
	MPI_Init(NULL, NULL);
	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int id = 50;
	array<char, 4> name{'J', 'a', 'c', 'k'};
	array<double,2> data{7.1, 8.1};

	auto send = make_tuple(id, name, data);
	auto particleMpiType = CreateTupleMpiType(send);

	decltype(send) recv;

	MPI_Request req;
	MPI_Status stat;

	if (rank == 0)
	{
		MPI_Recv(&recv, 1, particleMpiType, 1, 0, MPI_COMM_WORLD, &stat);
	}
	if (rank == 1)
	{
		MPI_Send(&send, 1, particleMpiType, 0, 0, MPI_COMM_WORLD);
	}

	if (rank == 0)
	{
		// print received data
		cout << "id=" << get<0>(recv) << "\nname=";
		for (auto &&i : get<1>(recv))
			cout << i;
		cout <<"\ndata="<< get<2>(recv)[0] <<","<< get<2>(recv)[1] << "\n";
	}

	MPI_Finalize();
}
