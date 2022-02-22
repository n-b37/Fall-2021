#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifdef OPENMP_HARNESS
#include <omp.h>
#endif

#ifdef MPI_HARNESS
#include <mpi.h>
#endif

#ifdef HYBRID_HARNESS
#include <omp.h>
#include <mpi.h>
#endif

#include "sort.h"
#include "graph.h"

struct Graph *countSortEdgesBySource (struct Graph *graph)
{
 // printf("entering first loop");
    int i,j,k,l,m,n,q,r;
     int P;
    P= numThreads;
	    int RADIX= 8;
    int nbits = 4;
    int base=0;
 
    struct Edge *sorted_edges_array = newEdgeArray(graph->num_edges);
   int *vertex_count = (int *)malloc(((256)*P) * sizeof(int));
  
#pragma omp parallel shared(vertex_count)
{
int tid;
 // printf("entering parallel loop");
tid=omp_get_thread_num();
int key;

 int pos;
int i,j,k,l,m,n,q,r;
    int offset_start, offset_end;
    
        if(tid==0)
    {
    P= numThreads;
    int *vertex_count = (int *)malloc((256)* (P) * sizeof(int)); 
    }
    
    
for(i=0;i<256;++i){
vertex_count[tid*256+i]=0;
}


    offset_start = tid * (graph->num_edges/P);
    if(tid== (P - 1))
    {
    offset_end = offset_start + (graph->num_edges / P) + (graph->num_edges % P);
     }
     else
     {
     offset_end = offset_start + (graph->num_edges/P);
     }
      //  printf("for thread %d end is %d and start is %d \n",tid,offset_end,offset_start);
      for(k=offset_start;k<offset_end;++k)
    {
        key = (((graph->sorted_edges_array[k].src) >> (RADIX * nbits)) & 0xFF);
        vertex_count[tid*256+key]++ ;
    }
#pragma omp barrier
 if(tid==0){
for(q=0;q<256;q++){
for(j=0;j<P; j++ ){
 base=base+vertex_count[j*256+q];
        vertex_count[j*256+q]=base;
}
}}
 #pragma omp barrier
 pos=0;
     key=0;
 for(m=offset_end-1; m>= offset_start; --m ){
     key = (((graph->sorted_edges_array[m].src) >> (RADIX * nbits)) & 0xFF);
    pos = vertex_count[tid*256+key] - 1;
       sorted_edges_array[pos] = graph->sorted_edges_array[m];
     vertex_count[tid*256+key]--;
		
		}
    }
 free(vertex_count);
    free(graph->sorted_edges_array);

    graph->sorted_edges_array = sorted_edges_array;
    printEdgeArray(graph->sorted_edges_array,graph->num_edges);
    return graph;

}

#ifdef OPENMP_HARNESS
struct Graph *radixSortEdgesBySourceOpenMP (struct Graph *graph)
{


  int itr;
     int P;
    P= numThreads;
	    int RADIX= 8;
    int base=0;
 
 
  for(itr=0;itr<4;itr++){
 
    struct Edge *sorted_edges_array = newEdgeArray(graph->num_edges);
   int *vertex_count = (int *)malloc(((256)*P) * sizeof(int));
  
#pragma omp parallel shared(vertex_count)
{
int tid;
tid=omp_get_thread_num();
int key;

 int pos;
int i,j,k,l,m,n,q,r;
    int offset_start, offset_end;
    
    #pragma omp barrier

     if(tid==0)
    {
    //P= numThreads;
    int *vertex_count = (int *)malloc((256)* (P) * sizeof(int)); 
    }
    
for(i=0;i<256;++i){
vertex_count[tid*256+i]=0;
}


    offset_start = tid * (graph->num_edges/P);
    if(tid== (P - 1))
    {
    offset_end = offset_start + (graph->num_edges / P) + (graph->num_edges % P);
     }
     else
     {
     offset_end = offset_start + (graph->num_edges/P);
     }
      for(k=offset_start;k<offset_end;++k)
    {
        key = (((graph->sorted_edges_array[k].src) >> (RADIX * itr)) & 0xFF);
        vertex_count[tid*256+key]++ ;
    }
#pragma omp barrier
 if(tid==0){
 base=0;
for(q=0;q<256;q++){
for(j=0;j<P; j++ ){
 base=base+vertex_count[j*256+q];
        vertex_count[j*256+q]=base;
}
}}
 #pragma omp barrier
 pos=0;
     key=0;
 for(m=offset_end-1; m>= offset_start; --m ){
     key = (((graph->sorted_edges_array[m].src) >> (RADIX * itr)) & 0xFF);
    pos = vertex_count[tid*256+key] - 1;
       sorted_edges_array[pos] = graph->sorted_edges_array[m];
     vertex_count[tid*256+key]--;
		
		}
    }
    free(vertex_count);
    free(graph->sorted_edges_array);

    graph->sorted_edges_array = sorted_edges_array;
    }
  

    return graph;
  
}
#endif





#ifdef MPI_HARNESS
struct Graph *radixSortEdgesBySourceMPI (struct Graph *graph)
{

  int itr;
  int i,j,k,l,m,n,q,r;
     int num_processes;
     int pid;
     int rank=0;
     int RADIX= 8;
    //int nbits = 4;
    int base=0;
    int root_process=0;
   
   
   
 
    struct Edge *sorted_edges_array = newEdgeArray(graph->num_edges);
   
   
   
   //size_per_process= (graph->num_edges) / num_processes;
    
    int p_size[num_processes];
   
    size_per_process= (graph->num_edges) / num_processes;
   
    int t=0;
    
    
    if(((graph->num_edges) % num_processes)==0)
    {
      for(t=0;t<num_processes;t++)
      {
        p_size[t]=size_per_process;
      }
    }
    
    else{
    
      if(t==num_processes)
      {
       p_size[t]=((graph->num_edges) % num_processes);
      }
      else
       p_size[t]=size_per_process;
     
     }
    
      struct Edge *sorted_edges_array_per_process = newEdgeArray(p_size[pid]);
     
     int *vertex_count = (int *)malloc((256) * sizeof(int));
     
      int *vertex_count_processes = (int *)malloc(((256)*num_processes) * sizeof(int));
     
       
for(i=0;i<256;++i){
  vertex_count[i]=0;
}
     
     
     
     
    MPI_Scatter(&sorted_edges_array[size_per_process*pid],p_size[pid],MPI_UINT64_T,&edge_arr_per_process, p_size[p_id],MPI_UINT64_T, 0, MPI_COMM_WORLD);
    

int key;

int pos;
int i,j,k,l,m,n,q,r;
  
  
  
  





  for(i = 0; i < p_size[pid]; ++i)
    {
        key = graph->edge_arr_per_process[i].src>> (RADIX * itr)) & 0xFF);
        vertex_count[key]++;
    }
     
   /*  
      for(k=offset_start;k<offset_end;++k)
    {
        key = (((graph->sorted_edges_array[k].src) >> (RADIX * itr)) & 0xFF);
        vertex_count[tid*256+key]++ ;
    }*/
  /*  
    for(i = 1; i < 256; ++i)
    {
        vertex_count[edge_arr_per_process[i]] += vertex_count[edge_arr_per_process[i-1]];
    }
    */

MPI_Gather(&vertex_count, 256 , MPI_INT, &vertex_count_processes[pid*256], 256 , MPI_INT, 0, MPI_COMM_WORLD);

     
  /*    for(k=offset_start;k<offset_end;++k)
    {
        key = (((graph->sorted_edges_array[k].src) >> (RADIX * itr)) & 0xFF);
        vertex_count[tid*256+key]++ ;
    }*/

   
 if(pid==0){
 base=0;
for(q=0;q<256;q++){
for(j=0;j<P; j++ ){
 base=base+vertex_count[j*256+q];
        vertex_count[j*256+q]=base;
}
}}



     pos=0;
     key=0;
 for(m=graph->num_edges -1; m>= 0; --m ){
     key = (((graph->sorted_edges_array[m].src) >> (RADIX * itr)) & 0xFF);
    pos = vertex_count[(pid)*256+key] - 1;
       sorted_edges_array[pos] = graph->sorted_edges_array[m];
     vertex_count[pid*256+key]--;
		
		}
    
    free(vertex_count);
    free(graph->sorted_edges_array);

    graph->sorted_edges_array = sorted_edges_array;
    
  
    return graph;
}
#endif







#ifdef HYBRID_HARNESS
struct Graph *radixSortEdgesBySourceHybrid (struct Graph *graph)
{

    printf("*** START Radix Sort Edges By Source Hybrid*** \n");
    return graph;
}
#endif