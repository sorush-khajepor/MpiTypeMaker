#include <mpi.h>
#include <iostream>
#include <vector>
#include "MpiTypeMaker.h"

using namespace std;
using namespace MpiTypeMaker;

struct Particle
{
	float Index;
	char Name[3];
	std::array<double, 2> Location;
};

auto printParticle(const Particle &p)
{
	cout << "Index=" << p.Index << "\n";
	cout << "Name = ";
	for (auto &&c : p.Name)
		cout << c;
	cout << "\n";
	cout << "Location = ";
	for (auto &&v : p.Location)
		cout << v << " ";
	cout << "\n---\n";
}

void initSampleParticles(vector<Particle> &particles, int particlesCount)
{
	for (size_t i = 0; i < particlesCount; i++)
	{
		Particle p{.Index = i, .Name{'P', '0', 'A' + i}, .Location{i * 10 + 0.5, i * 10 + 0.5}};
		particles.push_back(p);
	}
}

int main()
{
	MPI_Init(NULL, NULL);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Status stat;
	Particle p_send{.Index = 10, .Name{'P', '0', 'A'}, .Location{1.5, 2.5}};
	Particle p_recv;

	auto particleMpiType = CreateCustomMpiType(p_send, p_send.Location, p_send.Index, p_send.Name);

	if (rank == 0)
		MPI_Recv(&p_recv, 1, particleMpiType, 1, 0, MPI_COMM_WORLD, &stat);

	if (rank == 1)
		MPI_Send(&p_send, 1, particleMpiType, 0, 0, MPI_COMM_WORLD);

	if (rank == 0)
		printParticle(p_recv);

	MPI_Finalize();
}
