                    #include <stdio.h>
                    #include<math.h>
                    #include <stdlib.h>
                    #include<string.h>
                    #include <sstream>
                    #include "sim_cache.h"
                    #include <iostream>
                    #include <fstream>
                    #include <iomanip>
                    #include <cassert>
                    using namespace std;


                    void Cache ::set_lru(unsigned addr,int val){
                    long int idx = index_address(addr);
                    for(int j=0;j<assoc;j++){
                                        if(cache_array[idx][j].lru < cache_array[idx][val].lru)
                                        { cache_array[idx][j].lru +=1; }
                                        
                                    }cache_array[idx][val].lru=0;
                    }

                    void Cache ::set_lru_vc(int val){

                    for(int j=0;j<vc_num_blocks;j++){
                                        if(victim_array[0][j].lru < victim_array[0][val].lru)
                                        { victim_array[0][j].lru +=1; }
                                        
                                    }victim_array[0][val].lru=0;
                    }



                    unsigned long int Cache::index_address(unsigned addr)
                    {
                        unsigned int index_addr;
                        int s=0;
                        //printf("Index value");
                        s= size/(assoc*block_size);
                        //printf("%d",assoc);
                        //printf("x");
                        long int index_bits = log2(s);
                        //printf("%d",index_bits);
                        //printf("x");
                        int bo=0;
                        bo= log2(block_size);
                        int len=32-bo;
                        int shift_val= len-index_bits;
                        unsigned mask;
                        mask = ((1 << index_bits) - 1) << (bo);
                        index_addr = (addr & mask)>>bo;
                        
                        
                        return index_addr;
                    }

                    unsigned long int Cache:: tag_address(unsigned addr)
                    {
                        //printf("inside tag");
                        int bo= log2(block_size);
                        unsigned t_addr;
                        int sets=0;
                        sets= size/(assoc*block_size);
                        unsigned long int n_tag_bits=32- log2(sets)-log2(block_size);
                        unsigned long int n_index_bits= log2(sets);
                        unsigned long int shift_values= n_index_bits+bo;
                    
                        unsigned int tag_addr= addr>>shift_values;

                        
                        return tag_addr;
                    }


                    int Cache:: evict_highest_lru_vc() //hit -1,miss -0
                    {
                            int highest_lru=0;
                            int highest_lru_assoc=0;
                        for(int k=0;k<vc_num_blocks;k++){
                            if(highest_lru<victim_array[0][k].lru){
                                highest_lru=victim_array[0][k].lru;
                                highest_lru_assoc=k;
                                }}
                                
                            return highest_lru_assoc;


                    }


                    int Cache:: evict_highest_lru(long int addr) //hit -1,miss -0
                    {
                        
                        long int idx = index_address(addr);

                            int highest_lru=0;
                            int highest_lru_assoc=0;
                        for(int k=0;k<assoc;k++){
                            if(highest_lru<cache_array[idx][k].lru){
                                highest_lru=cache_array[idx][k].lru;
                                highest_lru_assoc=k;
                                }}
                                
                            return highest_lru_assoc;


                    }


                    int Cache:: check_for_space(long int addr) //hit -1,miss -0
                    {
                        long int idx = index_address(addr);
                        int counter=0;
                        for(int i=0;i<assoc;i++){
                            if(cache_array[idx][i].valid==1){
                                    counter++;
                                }}
                        if(counter==assoc){
                            return 0;}
                            else 
                            return 1;
                    }

                    void Cache::swapping(victim_block &temp_v,cache_block &temp,long int addr,int vc_hit_index, int evict_lru,int index_c){

                                swaps+=1;                            

                                int bo=log2(block_size);

                                
                                
                                int address_forVC= concat(temp.tag,index_c);
                                
                                int address_forL1= temp_v.tag<<bo;
                                long int BO =log2(block_size);
                                long int reduced_addressforVC = address_forVC >> BO;

                                victim_array[0][vc_hit_index].tag = reduced_addressforVC;

                                long int idx_ = index_address(address_forL1);
                                long int tag_ = tag_address(address_forL1);

                                cache_array[idx_][evict_lru].tag = tag_;


                                int tempp=0;
                                
                                tempp=temp.dirty;
                                temp.dirty=temp_v.dirty;
                                temp_v.dirty=tempp;

                                //set_lru(address_forL1,evict_lru);
                                //set_lru_vc(vc_hit_index);
                        

                    }

                    void Cache::if_vc_exists(long int addr,int* p){

                                    swap_requests+=1;


                                    
                                    int vc_hit_index=0;
                                    long int idx = index_address(addr);
                                    long int tag = tag_address(addr);
                                    


                                    int bo=log2(block_size);
                                    *p=(evict_highest_lru(addr));
                                    cache_block temp=cache_array[idx][*p];
                                    victim_block temp_v=victim_array[0][vc_hit_index];
                                    // printf("HIghest LRU for eviction at L1 %lu \n",p);
                                    for(int i=0;i<vc_num_blocks;i++){

                                        
                                        if(victim_array[0][i].tag==addr>>bo && victim_array[0][i].valid==1 && cache_array[idx][*p].valid==1 ){

                                        // printf("VC hit at vc address %lx and cache address %lx \n",victim_array[0][i].tag,addr>>bo);
                                        vc_hit_index=i;
                                        swapping(victim_array[0][vc_hit_index],cache_array[idx][*p],addr,vc_hit_index,*p,idx);
                                        if(rw=='w'){
                                            // assert(0);
                                            cache_array[idx][*p].dirty=1;
                                        }
                                        vc_hit=1;
                                        break;
                                        }
                                    }

                                    if(vc_hit==1){

                                        set_lru_vc(vc_hit_index);
                                        set_lru(addr,*p); //check for dirty allocaoon to 1 when write
                                        
                                    }

                                    else{
                                        int space= check_for_space_vc();
                                        if(!space){
                                            // printf("No space so evicting from VC\n");
                                        
                                        int p_vc=(evict_highest_lru_vc());
                                        // printf("Index of evicition at vc %lu \n", p_vc);
                                        int idx=index_address(addr);
                                        
                                        int tag=cache_array[idx][*p].tag;

                                        int combined=concat(tag,idx);

                                        if(!victim_array[0][p_vc].dirty){
                                        int idx=index_address(addr);
                                            // printf("victim not dirty \n");
                                        int BO=log2(block_size);

                                        long int tag_L1 = combined >> BO;
                                        

                                        // printf("Write to victim cache \n");
                                        
                                        victim_array[0][p_vc].tag=tag_L1;
                                        
                                        victim_array[0][p_vc].dirty=cache_array[idx][*p].dirty;
                                        victim_array[0][p_vc].valid=1;
                                        

                                        // printf("%lx evicted tag \n", tag_L1);
                                        // printf("%lx new dirty \n", victim_array[0][p_vc].dirty);

                                    //   printf("Readfrom l2 to l1 \n");
                                    //   c->read_operation(addr);
                                        
                                    long int tag_L1_return = tag_address(addr);
                                        long int index_L1_return =index_address(addr);

                                        // printf("Write or read the incoming to L1 \n");
                                        // cache_array[index_L1_return][p].tag=tag_L1_return;
                                        
                                        
                                        cache_array[index_L1_return][*p].dirty=0;
                                        cache_array[index_L1_return][*p].valid=1;
                                        //set_lru(addr,p);
                                        set_lru_vc(p_vc);
                                        }

                                        else{
                                            int p_vc=(evict_highest_lru_vc());

                                            // printf("victim is dirty \n");
                                            WB+=1;
                                            int bo=log2(block_size);
                                            int address=victim_array[0][p_vc].tag << bo;
                                            // printf("Write back to l2");
                                            if(L2_exists)
                                            c->write_operation(address);

                                    

                                        int idx=index_address(addr);
                                        
                                        int tag=cache_array[idx][*p].tag;

                                        int combined=concat(tag,idx);

                                        long int tag_L1 = combined >> bo;

                                        // printf("Write from l1 to victim \n");
                                        
                                        victim_array[0][p_vc].tag=tag_L1;
                                        victim_array[0][p_vc].dirty=cache_array[idx][*p].dirty;
                                        victim_array[0][p_vc].valid=1;
                                        // printf("%lx evicted tag \n", tag_L1);
                                        // printf("%lx new dirty \n", victim_array[0][p_vc].dirty);

                                        long int tag_L1_return = tag_address(addr);
                                        long int index_L1_return =index_address(addr);
                                    /*  printf("Read from l2 to l1");
                                        c->read_operation(addr);


                                        printf("Read or write to l1");

                                        cache_array[index_L1_return][p].tag=tag_L1_return;
                                        */
                                        cache_array[index_L1_return][*p].dirty=0;
                                        if(rw == 'w') {
                                        cache_array[index_L1_return][*p].dirty=1;}
                                        cache_array[index_L1_return][*p].valid=1;
                                        set_lru(addr,*p);
                                        set_lru_vc(p_vc);

                                        }
                                        }
                                    else{

                                        // printf("There is space in VC \n");
                                    for(int i=0;i<vc_num_blocks;i++){
                                    
                                        int bo=log2(block_size);
                                        int addrr=cache_array[idx][*p].tag;
                                        int comb=concat(addrr,idx);
                                        comb=comb>>bo;

                                        // printf("Combined from L1: %lx ", comb);
                                    if(victim_array[0][i].valid==0){
                                        
                                        victim_array[0][i].tag=comb;
                                        victim_array[0][i].dirty=cache_array[idx][*p].dirty;
                                        victim_array[0][i].valid=1;
                                        // c->read_operation(addr);
                                        // printf("%lx new tag \n",comb);
                                        // printf("%lx new dirty \n", victim_array[0][i].dirty);
                                        int idx=index_address(addr);
                                        int tag=tag_address(addr);
                                        set_lru(addr,*p);
                                        set_lru_vc(i);
                                    // cache_array[idx][p].tag= tag;
                                    // cache_array[idx][p].dirty=0;
                                        if(rw=='w')
                                        cache_array[idx][*p].dirty=1;
                                        
                                    
                                        break;
                                    }

                                    }
                                    }

                                    }

                    }




                    int Cache:: check_for_space_vc() //hit -1,miss -0
                    {
                        
                        int counter=0;
                        for(int i=0;i<vc_num_blocks;i++){
                            if(victim_array[0][i].valid==1){
                                    counter++;
                                }}
                        if(counter==vc_num_blocks){
                            return 0;}
                            else 
                            return 1;
                    }

    


                    void Cache:: read_operation(long int addr) /// hit - 1, miss-0
                    {
                        
                        // printf("Inside read\n");
                        
                        // The "this" pointer is the pointer for the class
                        // this->read+=1; is incrementing the variable for that class
                        // When you are inside a function of the class, the this pointer is implied.
                         read+=1; // = this->read+=1;

                        
                        int read_hit=0;
                        int write_hit=0;
                        long int idx = index_address(addr);
                        long int tag = tag_address(addr);
                        // printf("Tag address %lx Index address %lu", tag, idx);
                        
                        for(int i=0;i<assoc;i++){
                            if(tag==cache_array[idx][i].tag && cache_array[idx][i].valid==1)
                                {
                                    read_hit=1;
                                    hit_index=i;
                                    // printf("Hit index %lu", hit_index);
                                    break;
                                }
                        }
                        if(read_hit==1){              
                                    set_lru(addr,hit_index);
                                
                                }
                        
                            
                            else{
                                    


                                // printf("Read miss\n");
                            
                        readmiss+=1;
                        
                                int space=check_for_space(addr);
                                int p=(evict_highest_lru(addr));
                                // printf("Is there space ? %lu", space);
                                if(L2_exists&&c!=NULL || !L2_exists&&c==NULL){

                                if(space){
                                    
                                    // printf(" c not null and space exists\n");
                                    if(L2_exists)
                                    c->read_operation(addr);

                                    for(int i=0;i<assoc;i++){
                                        if(cache_array[idx][i].valid==0)
                                    {
                                        cache_array[idx][i].tag=tag;
                                        cache_array[idx][i].valid=1;
                                        cache_array[idx][i].dirty=0;
                                        set_lru(addr,i);
                                    break;
                                }
                                }}
                                
                                if(!space){
                                    vc_hit=0;
                                    

                                    if(vc_exists){
                                        // printf("VC EXISTS\n");
                                        if_vc_exists(addr,&p);
                                    }  
                                    
                                    else{
                                    

                                    // printf(" c not null and space does not exist");
                                        
                                        

                                        if(cache_array[idx][p].dirty==1){
                                            //
                                            int concated= concat(cache_array[idx][p].tag,idx);
                                            // printf("CONCATED\n");
                                            // printf("%lu \n",concated);
                                            WB+=1;
                                            if(L2_exists){
                                                
                                            c->write_operation(concated);}
                                        }
                                    }  
                                    if(!vc_hit){
                                        // int p=(evict_highest_lru(addr));
                                        if(L2_exists)
                                        c->read_operation(addr);
                                        
                                        // printf("Index of evicition %lu", p);
                                        // printf("Evicted from L1 %lu \n",cache_array[idx][p].tag);
                                        cache_array[idx][p].tag=tag;
                                        // printf("Wrriting to L1 %lu \n",tag);
                                    // if(rw=='r')
                                        cache_array[idx][p].dirty=0;
                                        if(rw=='w')
                                        cache_array[idx][p].dirty=1;
                                        cache_array[idx][p].valid=1;
                                        set_lru(addr,p);
                                    
                                    }


                                    }

                            }



                                
                                
                            if(L2_exists&&c==NULL){
                                    
                                    

                                
                                if(space ){
                                    // printf("Inside L2, c null and space exists\n");
                                

                                    for(int i=0;i<assoc;i++){
                                        if(cache_array[idx][i].valid==0)
                                    {
                                        cache_array[idx][i].tag=tag;
                                        cache_array[idx][i].valid=1;
                                        cache_array[idx][i].dirty=0;
                                        set_lru(addr,i);
                                    break;
                                }
                                }}
                                if(!space){
                                    
                                    // printf("L2 WRITEBACK %lu \n",WB);
                                    // printf(" c not null and space does not exist2");
                                    
                                        
                                        int p=(evict_highest_lru(addr));
                                        int idx=index_address(addr);
                                        if(cache_array[idx][p].dirty==1)
                                        WB+=1;
                                        
                                        // printf("Index of evicition %lu", p);
                                        cache_array[idx][p].tag=tag;
                                        cache_array[idx][p].dirty=0;
                                        cache_array[idx][p].valid=1;
                                        set_lru(addr,p);
                                    
                                    
                                }





                                }


                            }
                        }

                    int Cache:: concat_v(int x,int y)
                    {
                        int c=0;
                        int bo=0;
                        int index_bits = 0;
                        bo= log2(block_size);
                        
                        
                        c|=x<<(index_bits+bo);

                        c|=y<<bo;
                        return c;
                    }


                    int Cache:: concat(int x,int y)
                    {   
                        int c=0;
                        int index_bo=0;
                        int bo=0;
                        
                        int index_bits = log2(size/(assoc*block_size));
                        bo= log2(block_size);
                        
                        c|=x<<(index_bits+bo);


                        c|=y<<bo;
                        return c;
                    }

                    void Cache:: write_operation(long int addr) /// hit - 1, miss-0
                    {
                    
                        write+=1;
                        
                        //printf("in write block");
                        int write_hit=0;
                        long int idx =0;
                        idx = index_address(addr);
                        long int tag = tag_address(addr);
                        // printf("Tag address %lx Index address %lu \n", tag, idx);
                        
                        for(int i=0;i<assoc;i++){
                        
                            if(tag==cache_array[idx][i].tag && cache_array[idx][i].valid ==1)
                                {   
                                    write_hit=1;
                                    cache_array[idx][i].dirty=1;
                                    hit_index =i;
                                    // printf("Write Hit index %lu \n", hit_index);
                                    break;
                                }
                        }   
                        // printf("Write hit value %lu \n" , write_hit);
                            if(write_hit==1)
                            {
                                
                                    // printf("Inside write hit");
                                    set_lru(addr,hit_index);
                                    
                                    cache_array[idx][hit_index].lru=0;

                                    
                                // printf("Write done\n");
                                
                            }
                            else{
                                // printf("Write miss\n");
                                
                                writemiss+=1;
                                int p=evict_highest_lru(addr);


                                int space=check_for_space(addr);
                                // printf("Is there space? \t %lu \n", space );
                                if(L2_exists&&c!=NULL || !L2_exists){
                                if(space){
                                    if(L2_exists)
                                    c->read_operation(addr);
                                    for(int i=0;i<assoc;i++){
                                        if(cache_array[idx][i].valid==0)
                                    {
                                        // printf("Valid is 1, Dirty is 1 \n");
                                        cache_array[idx][i].tag=tag;
                                        cache_array[idx][i].valid=1;
                                        cache_array[idx][i].dirty=1;
                                        set_lru(addr,i);

              
                                    break;
                                } 
                                }
                                } 
                                if(!space){
                                    vc_hit=0;
                                    if(vc_exists){
                                        // printf("VC EXISTS\n");
                                        if_vc_exists(addr,&p);
                                        
                                    }  
                                        
                                        

                                    else{
                                        
                                        // printf("Index of highest LRU %lu", p);
                                        if(cache_array[idx][p].dirty==1){
                                            int concated= concat(cache_array[idx][p].tag,idx);
                                            // printf("CONCATED\n");
                                            // printf("%lu \n",concated);
                                            WB+=1;
                                            if(L2_exists){
                                                
                                            c->write_operation(concated);}
                                        }
                                    }
                                    if(!vc_hit){
                                        if(L2_exists)
                                        c->read_operation(addr);
                                        cache_array[idx][p].tag=tag;
                                        cache_array[idx][p].dirty=1;
                                        cache_array[idx][p].valid=1;

                                        set_lru(addr,p);
                                    
                                }
                                else{cache_array[idx][p].dirty=1;}
                                }
                                }
                                if(L2_exists){
                                if(c==NULL){
                                    // printf("Atleast inside");
                                    int space=check_for_space(addr);
                                    
                                    if(space){
                                    for(int i=0;i<assoc;i++){
                                        if(cache_array[idx][i].valid==0)
                                    {
                                        cache_array[idx][i].tag=tag;
                                        cache_array[idx][i].valid=1;
                                        cache_array[idx][i].dirty=1;
                                        set_lru(addr,i);
         
                                    break;
                                } 
                                }} 
                                if(!space){
                                    
                                        int idx=index_address(addr);
                                        
                                        // printf("Index of highest LRU2 %lu", p);

                                        if(cache_array[idx][p].dirty==1)
                                        WB+=1;
                                        
                                        
                                        //
                                        cache_array[idx][p].tag=tag;
                                        cache_array[idx][p].dirty=1;
                                        cache_array[idx][p].valid=1;

                                        set_lru(addr,p);

                                        
                                }



                                }}
                            }
                    }


                    void Cache::cache_arr()
                    {
                        int sets= size/(block_size*assoc);
                        cache_array = new cache_block* [sets];
                        for(unsigned long int i =0 ; i< sets ; i++)
                            {
                                cache_array[i] = new cache_block[assoc];
                                //printf("%d \n",cache_array[i]);
                            }
                    }

                    void Cache:: victim_arr()
                    {
                        
                        victim_array = new victim_block* [1];
                    
                        victim_array[0] = new victim_block[vc_num_blocks];
                                //printf("%d \n",cache_array[i]);
                            
                    }

                    void Cache :: initialiseLRU(){
                        int sets= size/(block_size*assoc);
                        for (int i=0;i<sets;i++){
                            for (int j=0;j<assoc; j++){
                                    cache_array[i][j].lru=j;
                            }
                        }
                    }

                    void Cache :: initialiseLRU_v(){
                            for (int j=0;j<vc_num_blocks; j++){
                                    victim_array[0][j].lru=j;
                            }

                    }

                    void Cache :: print_v(){

                        for (int i = 0; i < vc_num_blocks-1; i++)     
            
                // Last i elements are already in place  
                                for (int j = 0; j < vc_num_blocks-i-1; j++)
                                    if (victim_array[0][j].lru > victim_array[0][j+1].lru)
                                        swapp_v(victim_array[0][j], victim_array[0][j+1]);

                    
                    
                            
                            printf("\n===== VC contents ===== \n set   0:");
                            for(int b=0;b<vc_num_blocks;b++){  
                                if(victim_array[0][b].dirty==1){
                                    printf("   %lx D ",victim_array[0][b].tag);}
                                else{
                                    printf("   %lx   ",victim_array[0][b].tag);}

                            }
                            printf("\n");
                    }

                void Cache:: swapp (cache_block &num1,cache_block &num2) 
                {
                    cache_block temp;
                    temp=num1;
                    num1=num2;
                    num2=temp;
                }

                void Cache:: swapp_v (victim_block &num1,victim_block &num2) 
                {
                    victim_block temp;
                    temp=num1;
                    num1=num2;
                    num2=temp;
                }

                    void Cache::print(long int addr){
                        long int idx =0;
                        char D;
                        idx = index_address(addr);
                        int sets= size/(block_size*assoc);
                            

                        /*   for(int i=0; i<sets;i++){
                            for(int k = 1; k<assoc; k++)
                                for(int j= k+1; j<assoc; j++)
                                    if( cache_array[i][k].lru > cache_array[i][j].lru)
                                    {
                                        cache_block temp =  cache_array[i][k];
                                        cache_array[i][k] =  cache_array[i][j];
                                        cache_array[i][j] = temp;
                                    }
                                }*/

                        for(int r=0; r<sets;r++){
                    for(int i = 0; i<assoc-1; i++)
                        for(int j= 0; j<assoc-i-1; j++)
                                if( cache_array[r][j].lru > cache_array[r][j+1].lru)
                                {
                                    swapp(cache_array[r][j],cache_array[r][j+1]);
                                
                                }
                            }

                        for (int a=0;a<sets;a++){
                            printf("set\t %d:\t",a);
                            for(int b=0;b<assoc;b++){
                                if(cache_array[a][b].dirty==1)
                                    printf("%lx \t D\t",cache_array[a][b].tag);
                                else
                                    printf("%lx \t  \t",cache_array[a][b].tag);

                            }printf("\n");
                        }

                        

                        

                                        
                                        

                        

                    }

                    int main (int argc, char* argv[])
                    {
                        // printf("i am here");
                        FILE *FP;               // File handler
                        char *trace_file;       // Variable that holds trace file name;
                        cache_params params;    // look at sim_cache.h header file for the the definition of struct cache_params
                        char rw;                // variable holds read/write type read from input file. The array size is 2 because it holds 'r' or 'w' and '\0'. Make sure to adapt in future projects
                        unsigned long int addr; // Variable holds the address read from input file


                        if(argc != 8)           // Checks if correct number of inputs have been given. Throw error and exit if wrong
                        {
                            printf("Error: Expected inputs:7 Given inputs:%d\n", argc-1);
                            exit(EXIT_FAILURE);
                        }
                        
                        // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
                        params.block_size       = strtoul(argv[1], NULL, 10);
                        params.l1_size          = strtoul(argv[2], NULL, 10);
                        params.l1_assoc         = strtoul(argv[3], NULL, 10);
                        params.vc_num_blocks    = strtoul(argv[4], NULL, 10);
                        params.l2_size          = strtoul(argv[5], NULL, 10);
                        params.l2_assoc         = strtoul(argv[6], NULL, 10);
                        trace_file              = argv[7];

                        // Open trace_file in read mode
                        FP = fopen(trace_file, "r");
                        if(FP == NULL)
                        {
                            // Throw error and exit if fopen() failed
                            printf("Error: Unable to open file %s\n", trace_file);
                            exit(EXIT_FAILURE);
                        }
                        // printf("In l1");
                        
                        // Print params
                        printf("  ===== Simulator configuration =====\n"
                                "  BLOCKSIZE:                     %lu\n"
                                "  L1_SIZE:                          %lu\n"
                                "  L1_ASSOC:                         %lu\n"
                                "  VC_NUM_BLOCKS:                    %lu\n"
                                "  L2_SIZE:                          %lu\n"
                                "  L2_ASSOC:                         %lu\n"
                                "  trace_file:                       %s\n\n"
                                " ===== L1 contents =====\n", params.block_size, params.l1_size, params.l1_assoc, params.vc_num_blocks, params.l2_size, params.l2_assoc, trace_file);
                        
                        Cache l1_cache;
                        Cache l2_cache;

                        
                        

                        // l1_cache.c=NULL;
                        //l2_cache.c=NULL;

                        if(params.l2_size){
                            l1_cache.L2_exists=1;
                            l2_cache.L2_exists=1;
                        }
                        else{
                            l1_cache.L2_exists=0;
                            l2_cache.L2_exists=0;
                        }

                        if(l1_cache.L2_exists){
                        l1_cache.c=&l2_cache;
                        l2_cache.c=NULL;}
                        else
                        l1_cache.c=NULL;
                       
                        if(params.vc_num_blocks>0){
                        l1_cache.vc_exists=1;}
                        else
                        l1_cache.vc_exists=0;

                        

                        
                            l2_cache.size =params.l2_size;
                            l2_cache.assoc =params.l2_assoc;
                            l2_cache.block_size =params.block_size;
                            
                        l1_cache.vc_num_blocks=params.vc_num_blocks;
                        l1_cache.victim_arr();
                        l1_cache.initialiseLRU_v();
                        l1_cache.vc_hit=params.vc_hit;

                        l1_cache.size =params.l1_size;
                        
                        l1_cache.assoc =params.l1_assoc;
                        
                        l1_cache.block_size =params.block_size;
                        
                        
                        char str[2];
                        l1_cache.cache_arr();
                        l1_cache.initialiseLRU();
                        if(l1_cache.L2_exists)
                        l2_cache.cache_arr(); 
                        
                        if(l1_cache.L2_exists)
                        l2_cache.initialiseLRU();
                        
                        

                        
                    

                    

                        
                        // printf("%lu",params.vc_num_blocks);

                        while(fscanf(FP, "%s %lx", str, &addr) != EOF)
                        {
                            
                            rw = str[0];
                            // l1_cache.rw = str[0];
                            if (rw == 'r'){
                                // printf("%s %lx\n", "read", addr);           // Print and test if file is read correctly




                                l1_cache.read_operation(addr); 
                                
                                }
                            else if (rw == 'w'){
                                // printf("%s %lx\n", "write", addr);          // Print and test if file is read correctly
                                l1_cache.write_operation(addr);
                                
                                }

                        }

                    l1_cache.print(addr);
                    if(l1_cache.vc_exists)
                    l1_cache.print_v();
                    if(l1_cache.L2_exists){
                    printf("\n ===== L2 contents =====\n");
                    l2_cache.print(addr);}



        cout<<"\n===== Simulation results =====";
        cout<<"\n a. number of L1 reads:     "<<l1_cache.read<<"\n b. number of L1 read misses:     "<<l1_cache.readmiss<<"\n c. number of L1 writes:     "<<l1_cache.write;
        cout<<"\n d. number of L1 write misses:     "<<l1_cache.writemiss<<"\n e. number of swap requests:     "<<l1_cache.swap_requests;
        cout<<"\n f. swap request rate:     "<< fixed <<setprecision(4) <<(float)(l1_cache.swap_requests)/(l1_cache.read + l1_cache.write)<<"\n g. number of swaps:     "<<l1_cache.swaps;
        cout<<"\n h. combined L1+VC miss rate:     "<< fixed <<setprecision(4) <<(float)(l1_cache.readmiss + l1_cache.writemiss - l1_cache.swaps)/(l1_cache.read + l1_cache.write);
        cout<<"\n i. number writebacks from L1/VC:     "<<l1_cache.WB;
        cout<<"\n j. number of L2 reads:     "<<l2_cache.read<<"\n k. number of L2 read misses:     "<<l2_cache.readmiss<<"\n l. number of L2 writes:     "<<l2_cache.write;
        cout<<"\n m. number of L2 write misses:     "<<l2_cache.writemiss;
        if (l1_cache.L2_exists)
            cout<<"\n n. L2 miss rate:     "<< fixed <<setprecision(4) <<(float)(l2_cache.readmiss)/(l2_cache.read);
        else
            cout<<"\n n. L2 miss rate:     0.0000";
        
        cout<<"\n o. number of writebacks from L2:     "<<l2_cache.WB;

        if(l1_cache.L2_exists)
            cout<<"\n p. total memory traffic:     "<<l2_cache.readmiss + l2_cache.writemiss + l2_cache.WB<<"\n";
        else
            cout<<"\n p. total memory traffic:     "<<l1_cache.readmiss + l1_cache.writemiss + l1_cache.WB - l1_cache.swaps<<"\n";

            


                    
                        return 0;
                    }



