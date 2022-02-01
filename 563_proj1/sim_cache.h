#ifndef SIM_CACHE_H
#define SIM_CACHE_H

typedef struct cache_params{
    unsigned long int block_size;
    unsigned long int l1_size;
    unsigned long int l1_assoc;
    unsigned long int vc_num_blocks;
    unsigned long int l2_size;
    unsigned long int l2_assoc;
    unsigned long int vc_hit;
}cache_params;

typedef struct cache_block{
    long int valid;
    long int dirty;
    long int tag;
    long int lru;};

typedef struct victim_block{
    long int valid;
    long int dirty;
    long int tag;
    long int lru;};
#endif



class Cache{
    public:
        Cache *c;
        long int size, block_size, assoc;
        cache_block **cache_array;
        victim_block **victim_array;
        int vc_num_blocks;
        int hit_index;
        int sets;
        int rw;
        cache_block temp;
        victim_block temp_v;
        int vc_exists;
        int L2_exists;
        int vc_hit;
        int L1_read = 0;
                    int readmiss = 0;
                    int write = 0;
                    int writemiss = 0;
                    int WB = 0;
                    int swap_requests = 0;
                    int swaps = 0;
                    int read = 0;

void print_v();
void initialiseLRU();
void initialiseLRU_v();
int concat_v(int,int);
void victim_arr();
void if_vc_exists(long int,int*);
void swapping(victim_block&,cache_block&,long int,int,int,int );

unsigned long int  index_address( unsigned);

unsigned long int  tag_address(unsigned);
//void swap(struct*,struct*);

bool  hit_or_miss(long int ); //hit -1,miss -0

int  evict_highest_lru(long int) ;//hit -1,miss -0

int check_for_space(long int); //hit -1,miss -0
void swapp(cache_block&,cache_block&);
void swapp_v(victim_block&,victim_block&);
void dummy(long int);
void  read_operation(long int ); /// hit - 1, miss-0


void  write_operation(long int ); /// hit - 1, miss-0

void cache_arr();
void print(long int);

int concat(int,int);

void set_lru(unsigned, int);
void set_lru_vc(int);
int evict_highest_lru_vc() ;
int check_for_space_vc();
// Put additional data structures here as per your requirement

};
