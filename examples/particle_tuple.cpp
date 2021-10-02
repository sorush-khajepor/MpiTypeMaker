#include <mpi.h>
#include <iostream>
#include <vector>

#include "MpiTypeMaker.h"

using namespace std;
using namespace MpiTypeMaker;

using Particle = std::tuple<int,double,float>;

auto printParticle(const Particle &p)
{
	cout << "Index=" << get<0>(p) << "\n";
	cout << "Name = "<<get<1>(p)<<"\n";
	//for (auto &&c : get<1>(p))
//		cout << c;
//	cout << "\n";
	cout << "Location = "<<get<2>(p);
//	for (auto &&v : get<2>(p))
	//	cout << v << " ";
	cout << "\n---\n";
}

void initSampleParticles(vector<Particle> &particles, int particlesCount)
{
	for (size_t i = 0; i < particlesCount; i++)
	{
		Particle p;
		get<0>(p) = i;
		get<1>(p)=i; //get<1>(p)[1]= '0'; get<1>(p)[2]= 'A'+i;;
		get<2>(p)=i*10+0.5;
		particles.push_back(p);
	}
}

int main()
{
	MPI_Init(NULL, NULL);

	Particle p;

	//auto particleMpiType = CreateCustomMpiType(p, get<0>(p), get<1>(p), get<2>(p));
	auto particleMpiType=CreateTupleMpiType(p);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Request req;
	MPI_Status stat;

	auto particlesCount = 3;
	vector<Particle> recv(particlesCount);

	if (rank == 0)
	{
		MPI_Recv(recv.data(), recv.size(), particleMpiType, 1, 0, MPI_COMM_WORLD, &stat);
	}
	if (rank == 1)
	{
		vector<Particle> send;
		initSampleParticles(send, particlesCount);
		MPI_Send(send.data(), send.size(), particleMpiType, 0, 0, MPI_COMM_WORLD);
	}

	if (rank == 0)
	{
		for (auto &p : recv)
			printParticle(p);
	}

	MPI_Finalize();
}
