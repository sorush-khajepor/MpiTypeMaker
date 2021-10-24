# MpiTypeMaker


To create an MPI type for a struct or tuple, we need to find block lenghts, displacemnts and data types of each member. This is usually hard-coded case by case and takes valuable time. Here with the aid of *C++20*, variadic functions, and type-traits, I created a simple header library to automate this task. 

This is also explained in [my blog post](https://iamsorush.com/posts/mpi-automate-custom-type/).

## Compiler

GCC 11 with flag `std=c++20`. 

## Code

The code with examples are on GitHub [Here](https://github.com/sorush-khajepor/MpiTypeMaker). To use it in your project only header `MpiTypeMaker.h` is needed. 

## Struct

If you have a struct like:

```cpp
struct Particle
{
	float Index;
	char Name[3];
	std::array<double, 2> Location;
};
```

And you want to send and receive it via MPI. You just add the header,`MpiTypeMaker.h`, and call `CreateCustomMpiType()` with first an object of the struct and then all its members:

```cpp
#include "MpiTypeMaker.h"

Particle p;
auto particleMpiType = CreateCustomMpiType(p, p.Location, p.Index, p.Name);
```

That's it. Now you can send a `particle` like:

```cpp
MPI_Send(&particle, 1, particleMpiType, 0, 0, MPI_COMM_WORLD);
```

## Tuple

For tuple the prcess is even easier, you don't need to name the members. For example, for this tuple:

```cpp
int id = 50;
array<char, 4> name{'J', 'a', 'c', 'k'};
array<double,2> data{7.1, 8.1};

auto person = make_tuple(id, name, data);
```

Just run this command:

```cpp
auto personMpiType = CreateTupleMpiType(person);
```

Now send it like:

```cpp
MPI_Send(&person, 1, personMpiType, 0, 0, MPI_COMM_WORLD);
```

## Run examples

Clone the project from GitHub:

```bash
git clone https://github.com/sorush-khajepor/MpiTypeMaker.git
```

In a terminal go into downloaded folder and run

```sh
mkdir build
cd build
cmake ..
make
```

The source files of examples are in `somePath/MpiTypeMaker/examples/`. After taking above step they are compiled in `build` folder. In a terminal opened there, run examples like:

```sh
mpirun -np 2 particles_struct 
```

