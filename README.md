# Parallelization-of-Bitcoin-Mining-Algorithm
Bitcoin Mining Algorithm with Parallel Computing
This is a IT301 Course Project

## Abstract
The project involves implementing bitcoin mining
using parallel computing using OpenMP, MPI and CUDA and
comparing the three methods. Bitcoin mining involves finding
the NONCE value which is the target hash using trial and
error(brute force). This is a very heavy computational work
as the number of possibilities are 2<sup>256</sup> which is huge. Hence,
parallelizing this process will help in achieving the NONCE faster
in bitcoin mining. The project was done in C++. The output of
each library(OpenMP, MPI and CUDA) is the number of hashes
per second which represents the number of hashes the algorithm
generated per second. Since this is a trial and error algorithm, the
higher hashes per second, the faster the target hash is achieved.
For the sake of simplicity, the NONCE is any hash divisible by
800000. To calculate the hash, SHA256 algorithm is used, which
is the best hashing algorithm known and is extensively used in
blockchains.


## Compiling commands

### MPI
```mpicc -o mpi MPI.c -lssl -lcrypto && ./mpi
mpiexec -n 2 ./mpi
