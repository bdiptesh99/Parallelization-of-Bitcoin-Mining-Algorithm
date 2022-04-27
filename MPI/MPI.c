#include <mpi.h> 
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define WORKTAG     1
#define SIZE     16384
#define DIETAG     2
typedef struct block_header {
        unsigned int    version;
        unsigned char   prev_block[32];
        unsigned char   merkle_root[32];
        unsigned int    timestamp;
        unsigned int    bits;
        unsigned int    nonce;
} block_header;
//Timer
double When()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}
//Sha256 to binary
void hex2bin(unsigned char* dest, unsigned char* src)
{
        unsigned char bin;
        int c, pos;
        char buf[3];
 
        pos=0;
        c=0;
        buf[2] = 0;
        while(c < strlen(src))
        {
                
                buf[0] = src[c++];
                buf[1] = src[c++];
                
                dest[pos++] = (unsigned char)strtol(buf, NULL, 16);
        }
       
}
 

 //SHA functions
void byte_swap(unsigned char* data, int len) {
        int c;
        unsigned char tmp[len];
       
        c=0;
        while(c<len)
        {
                tmp[c] = data[len-(c+1)];
                c++;
        }
       
        c=0;
        while(c<len)
        {
                data[c] = tmp[c];
                c++;
        }
}

//Client - Manages IO and issues the request to the server
void client(int ntasks,int  range )
{
	long long unsigned int nonce = 0; //NONCE Variable
	int	 rank;
	double       result=0; 
	MPI_Status     status;

	for (rank = 1; rank < ntasks; ++rank) {
		MPI_Send(&nonce,         
		1,              
		MPI_INT,        
		rank,           
		WORKTAG,        
		MPI_COMM_WORLD);
		nonce += range; //;
	}


 	
	double start = When();
	double timer = When() - start;
	while (timer < 60) {
		MPI_Recv(&result,       
		1,              
		MPI_DOUBLE,     
		MPI_ANY_SOURCE, 
		MPI_ANY_TAG,    
		MPI_COMM_WORLD, 
		&status);       
		MPI_Send(&nonce, 1, MPI_INT, status.MPI_SOURCE,
		WORKTAG, MPI_COMM_WORLD);
		nonce += range; 
		if ( nonce % 800000 == 0){
            timer = (When() - start);
        }
	}

	for (rank = 1; rank < ntasks; ++rank) {
		MPI_Recv(&result, 1, MPI_DOUBLE, MPI_ANY_SOURCE,
		MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	}
	

	for (rank = 1; rank < ntasks; ++rank) {
		MPI_Send(0, 0, MPI_INT, rank, DIETAG, MPI_COMM_WORLD);
	}
    printf("number of hashs per second = %lld\n",(long long int)(nonce / (When() - start)));

	
}

void server(int range)
{
	double result=1;
    SHA256_CTX sha256_pass1, sha256_pass2;
    
    unsigned char hash1[SHA256_DIGEST_LENGTH];
    unsigned char hash2[SHA256_DIGEST_LENGTH];
    int i;
    
    block_header header;
    
    header.version =        2;
    hex2bin(header.prev_block,              "000000000000000117c80378b8da0e33559b5997f2ad55e2f7d18ec1975b9717");
    hex2bin(header.merkle_root,             "871714dcbae6c8193a2bb9b2a69fe1c0440399f38d94b3a0f1b447275a29978a");
    header.timestamp =      1392872245;
    header.bits =           419520339;
    header.nonce =          0;
   
    
    byte_swap(header.prev_block, 32); 
    byte_swap(header.merkle_root, 32); 
   
	//MPI Server program
	MPI_Status          status;
	for (;;) {
		MPI_Recv(&header.nonce, 1, MPI_INT, 0, MPI_ANY_TAG,
		MPI_COMM_WORLD, &status);
	
		if (status.MPI_TAG == DIETAG) {
			return;
		}
		for(i = 0 ; i < range ; i++)
		{
	        
	        
	        SHA256_Init(&sha256_pass1);
	        
	        SHA256_Update(&sha256_pass1, (unsigned char*)&header, sizeof(block_header));
	        
	        SHA256_Final(hash1, &sha256_pass1);
	                   
	        SHA256_Init(&sha256_pass2);
	        SHA256_Update(&sha256_pass2, hash1, SHA256_DIGEST_LENGTH);
	        SHA256_Final(hash2, &sha256_pass2);

            header.nonce ++;
    	}
		result = 1 ;
		MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
}
int main(int argc, char *argv[])
{
	int myrank;
	MPI_Init(&argc, &argv);   
	MPI_Comm_rank(MPI_COMM_WORLD , &myrank);      
	int	ntasks, range;
	MPI_Comm_size(
	MPI_COMM_WORLD, &ntasks);          
	range = SIZE / ntasks;    
	

	if (myrank == 0) {
		client(ntasks,range);
	} else {
		server(range);
	}
	MPI_Finalize();       
}
