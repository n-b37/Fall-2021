#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim_bp.h"
#include <math.h> 

void counter_operation(int *prediction_table, char outcome, long int index)
{
	if(outcome == 't')
	{
		if(prediction_table[index]==3)
		{
			prediction_table[index]=3;
		}
		else
		{
			prediction_table[index]++;
		}
	}
	else if(outcome == 'n')
	{
		if(prediction_table[index]==0)
		{
			prediction_table[index]=0;
		}
		else
		{
			prediction_table[index]--;
		}
	}
}

char prediction(int *prediction_table, long int index)
{
	char prediction_result;
	if(prediction_table[index] == 2 || prediction_table[index] == 3)
	{
		prediction_result = 't';
	}
	else
	{
		prediction_result = 'n';
	}
	return prediction_result;
}

int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file
    params.K = 0;
	params.M1 = 0;
	params.M2 = 0;
	params.N = 0;

	int m_pred=0;
	int n_pred=0;
    int index_addr_gs=0;

    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
    
    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
    
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }
    
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }

    int *counter_bm = (int *)malloc(sizeof(int) * (pow(2,params.M2)));
    int *counter_gs = (int *)malloc(sizeof(int) * (pow(2,params.M1)));
    int *counter_hybrid = (int *)malloc(sizeof(int) * (pow(2,params.K)));


        for(int i=0;i<pow (2,params.M2) ;i++){
            counter_bm[i]=2;
        }
        for(int i=0;i<pow (2,params.M1) ;i++){
            counter_gs[i]=2;
        }
        
        for(int i=0;i<pow (2,params.K) ;i++){
            counter_hybrid[i]=1;
        }
    
    unsigned long int bhr=0b0;
    unsigned long int bhr_a=0b0;
    char t_n_b;
    char t_n_gs;
    char t_n;
        
    
    char str[2];
    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
        n_pred+=1;
        outcome = str[0];
      /*  if (outcome == 't')
            printf("%lx %s\n", addr, "t");           // Print and test if file is read correctly
        else if (outcome == 'n')
            printf("%lx %s\n", addr, "n");          // Print and test if file is read correctly
        */
       //bimodal
       //int *counter_bm = (int *)malloc(sizeof(int) * (pow(2,params.M2)));
        // char t_n;
        unsigned long int index_addr;
        unsigned long int index_addr_gs;
        unsigned long int mask;
        addr=addr>>2;
        mask = ((1 << params.M2) - 1);
        index_addr = (addr & mask);


     if(argc==4){
         index_addr = (addr & mask);
         // printf("inside here");
        if(counter_bm[index_addr]==2 || counter_bm[index_addr]== 3)
        
            t_n='t';
        else
            t_n='n';

        if(outcome == 't'){
            if(counter_bm[index_addr]==3)
                counter_bm[index_addr]=3;
            else
            counter_bm[index_addr]+=1;

        }

            
        else if (outcome == 'n'){
            if(counter_bm[index_addr]==0)
                counter_bm[index_addr]=0;
            else
            counter_bm[index_addr]-=1;}

        if(t_n != outcome)
        	{
        		m_pred++;
			}
        }
    //GSHARE
    
    unsigned mask_gs;
    unsigned long int addr_after_xor;
    mask_gs = ((1 << params.M1) - 1);
    index_addr_gs = (addr & mask_gs);
    bhr_a=bhr<<(params.M1-params.N);
    addr_after_xor=bhr_a ^ index_addr_gs;
  
    if(argc == 5)
	{
        
     if(counter_gs[addr_after_xor]==2 || counter_gs[addr_after_xor]== 3)
            t_n='t';
        else
            t_n='n';
    if(outcome == 't'){
        if(counter_gs[addr_after_xor]==3)
            counter_gs[addr_after_xor]=3;
        else
            counter_gs[addr_after_xor]+=1;

        bhr=bhr>>1;
		bhr = bhr |( 1<<(params.N - 1));

        }

            
        else if (outcome == 'n')
        {
            if(counter_gs[addr_after_xor]==0)
                counter_gs[addr_after_xor]=0;
            else
            counter_gs[addr_after_xor]-=1;

            bhr = bhr >> 1; 
        }

            if(t_n != outcome)
        	m_pred+=1;
    }

    //hybrid
    unsigned long int idx;
    idx= addr & ((1<<params.K)-1);

    if(argc == 7)
	{

        if(counter_gs[addr_after_xor]==2 || counter_gs[addr_after_xor]== 3)
            t_n_gs='t';
        else
            t_n_gs='n';

        if(counter_bm[index_addr]==2 || counter_bm[index_addr]== 3)
            t_n_b='t';
        else
            t_n_b='n';
       
        if(counter_hybrid[idx]==2 || counter_hybrid[idx]==3)
        {
             
        t_n=t_n_gs;

        if(outcome == 't'){

        if(counter_gs[addr_after_xor]==3)
            counter_gs[addr_after_xor]=3;
        else
            counter_gs[addr_after_xor]+=1;

        

        }

            
        else if (outcome == 'n')
        {
            if(counter_gs[addr_after_xor]==0)
                counter_gs[addr_after_xor]=0;
            else
            counter_gs[addr_after_xor]-=1;

           
        }

    }

    



        else{
            
            t_n=t_n_b;

            if(outcome == 't')
        {
            if(counter_bm[index_addr]==3)
                counter_bm[index_addr]=3;
            else
            counter_bm[index_addr]+=1;

        }

            
        else if (outcome == 'n'){
            if(counter_bm[index_addr]==0)
                counter_bm[index_addr]=0;
            else
            counter_bm[index_addr]-=1;}
        }


    if(outcome == 't'){
        bhr=bhr>>1;
		bhr = bhr |( 1<<(params.N - 1));}

    if (outcome == 'n')
        {
            bhr = bhr >> 1; 
        }



        if(t_n != outcome)
        	{
        		m_pred++;
		}
        if((outcome != t_n_b)&&(outcome == t_n_gs)){
				if(counter_hybrid[idx]==3)
				{
					counter_hybrid[idx]=3;
				}
				else
				{
					counter_hybrid[idx]++;
				}
			}
			else if((outcome != t_n_gs)&&(outcome == t_n_b))
			{
				if(counter_hybrid[idx]==0)
				{
					counter_hybrid[idx]=0;
				}
				else
				{
					counter_hybrid[idx]--;
				}
			}
		
    }



    }

float mispred_rate = (float)m_pred/n_pred * 100;
    printf("OUTPUT\n");
    printf(" number of predictions:\t   %d\n",n_pred);
    printf(" number of mispredictions: %d\n",m_pred);
    printf(" misprediction rate:\t   %0.2f%s\n",mispred_rate,"%");

    if(argc==7)
    {
    	 printf("FINAL CHOOSER CONTENTS\n");
	     for(int l=0; l<pow(2,params.K); l++)
	     {	
	    	printf(" %d\t",l);
	    	printf("%d\n",counter_hybrid[l]);
	    
		 }
	}
   
	
	if(argc==5 || argc==7)
	{
		 printf("FINAL GSHARE CONTENTS\n");
	    for(int k=0; k<pow(2,params.M1); k++)
	    {	
	    	printf(" %d\t",k);
	    	printf("%d\n",counter_gs[k]);
		}
	}
   
	
	if(argc==4||argc==7)
	{
		printf("FINAL BIMODAL CONTENTS\n");
	    for(int m=0; m<pow(2,params.M2); m++)
	    {	
	    	printf(" %d\t",m);
	    	printf("%d\n",counter_bm[m]);
	    
		}
	}
	

       

    return 0;
}
