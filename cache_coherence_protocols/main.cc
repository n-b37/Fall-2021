/*******************************************************
                          main.cc
********************************************************/
#include <cstdlib>
#include <assert.h>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
using namespace std;

#include "cache.h"

int main(int argc, char *argv[])
{
	
	ifstream fin;
	FILE * pFile;

	

	if(argv[1] == NULL){
		 printf("input format: ");
		 printf("./smp_cache <cache_size> <assoc> <block_size> <num_processors> <protocol> <trace_file> \n");
		 exit(0);
        }

	/*****uncomment the next five ls*****/
	int cache_size = atoi(argv[1]);
	int cache_assoc= atoi(argv[2]);
	int blk_size   = atoi(argv[3]);
	int num_processors = atoi(argv[4]);/*1, 2, 4, 8*/
	int protocol   = atoi(argv[5]);	 /*0:MSI, 1:MESI, 2:Dragon*/
	char *fname =  (char *)malloc(20);
 	fname = argv[6];
	 char rw;



	 cout<<"===== 506 Personal Information ====="<<endl;
    cout<<"FirstName Nandana LastName Balachandran"<<endl;
    cout<<"UnityID nbalach"<<endl;
    cout<<"ECE492 Students? NO"<<endl;

	//****************************************************//
	//**printf("===== Simulator configuration =====\n");**//
	//*******print out simulator configuration here*******//
	//****************************************************//
	cout<<"===== 506 SMP Simulator configuration ====="<<endl;
    cout<<"L1_SIZE: "<<cache_size<<endl;
    cout<<"L1_ASSOC: "<<cache_assoc<<endl;
    cout<<"L1_BLOCKSIZE: "<<blk_size<<endl;
    cout<<"NUMBER OF procESSORS: "<<num_processors<<endl;
	

	
    switch (protocol)
    {
    case 0:
        cout<<"COHERENCE PROTOCOL: MSI"<<endl;
        break;
    case 1:
        cout<<"COHERENCE PROTOCOL: MESI"<<endl;
        break;
    case 2:
        cout<<"COHERENCE PROTOCOL: Dragon"<<endl;
        break;
    }
    cout<<"TRACE FILE: "<<argv[6]<<endl;

/*
typedef struct cache_block{
    int cache_size;
    int cache_assoc;
    int blk_size;
} cache_block;


cache_block* cb = new cache_block[num_processes];
*/

Cache** cachesArray = new Cache*[num_processors];
/*= new Cache(cache_size,cache_assoc,blk_size);
*/
//cout<<cachesArray;
for(int i=0;i<num_processors;i++){
	cachesArray[i]= new Cache(cache_size,cache_assoc,blk_size);
	// cp[i] = Cache(cache_size,cache_assoc,blk_size);
	
}
	//****************************************************//
	//**printf("===== Simulator configuration =====\n");**//
	//*******print out simulator configuration here*******//
	//****************************************************//

 
	//*********************************************//
        //*****create an array of caches here**********//
	//*********************************************//	

	pFile = fopen (fname,"r");
	if(pFile == 0)
	{   
		printf("Trace file problem\n");
		exit(0);
	}
	///******************************************************************//
	//**read trace file,l by l,each(processor#,operation,addr)**//
	//*****propagate each request down through memory hierarchy**********//
	//*****by calling cachesArray[processor#]->Access(...)***************//
	///******************************************************************//
	
	fin.open(fname, ios::in); //fname
	string l;

	while (getline(fin,l))
	{
		ulong hex_a;
		int procc1;

        procc1 = l[0] - '0';
        rw = l[2];
        string addr = l.substr(4);

        if (procc1<0 || procc1>8)
            break;
            
        istringstream buffer (addr);
        buffer >> hex >> hex_a;
	//cout<<"here";
        cachesArray[procc1]->Access(hex_a, rw, protocol, procc1,num_processors, cachesArray);
	
	}
	
	fclose(pFile);

	//********************************//
	//print out all caches' statistics //
	//********************************//
	for(int i=0; i<num_processors; i++)
    {
        cachesArray[i]->printStats(i, protocol);
    }
}
