        #include <stdio.h>
        #include <stdlib.h>
        #include <string.h>
        #include "sim_proc.h"
        #include <vector>
        #include <iostream>
        #include <math.h>
        #include <fstream>
        #include <iomanip>
        #include <sstream>
        using namespace std;
        /*  argc holds the number of command line arguments
            argv[] holds the commands themselves

            Example:-
            sim 256 32 4 gcc_trace.txt
            argc = 5
            argv[0] = "sim"
            argv[1] = "256"
            argv[2] = "32"
            ... and so on
        */
        int op_type, dest, src1, src2;  // Variables are read from trace file
         long int pc,width,rmt_size,rob_size,iq_size,c_count=0; // Variable holds the pc read from input file
        int ctr;
        int regsEmpty;
        //FILE *FP;
        




        vector <instrr> DE;
        vector <instrr> RN;
        vector <instrr> RR;
        vector <instrr> DI;
        vector <instrr> IQ;
        vector <instrr> WB;
        vector <instrr> RT;
        vector <instrr> execute_list;
        vector <robcell> ROB;
        vector <rmtcell> RMT;

        int head;
        int tail;
        int a=0;
        int er=0;

        void fetch(FILE* FP){

       
            
            instrr instr;
        /*if (feof(FP) || !DE.empty() )
		return;*/
            
        //  if(FP!=NULL && DE.empty()){
        while(DE.size()<width ){
           // for( int i=0;i<width;i++){
              // cout<<"end:"<<e<<"\n";
                
           

        a= fscanf(FP, "%lx %d %d %d %d", &pc, &op_type, &dest, &src1, &src2);
        
       // printf("%lx %d %d %d %d\n", pc, op_type, dest, src1, src2); //Print to check if inputs have been read correctly
            if(a == EOF){
              er=1;
              return;
              //cout<<er;
              }
            if(!er){
            instr.pc = ctr;
            instr.op_type = op_type;
            instr.dest = dest;
            instr.src1_actual = src1;
            instr.src2_actual = src2;
            instr.src1 = src1;
            instr.src2 = src2;
            instr.s1_is_from_ROB = 0;
            instr.s2_is_from_ROB = 0;
            
            instr.de_cyc=0;
            instr.fe_cyc=0;
            instr.rn_cyc=0;
            instr.rr_cyc=0;
            instr.di_cyc=0;
            instr.is_cyc=0;
            instr.wb_cyc=0;
            instr.rt_cyc=0;
            
            instr.de_s=0;
            instr.fe_s=0;
            instr.rn_strt=0;
            instr.rr_s=0;
            instr.dis=0;
            instr.is_s=0;
            instr.wb_s=0;
            instr.rt_s=0;

            

            instr.s1_readybit_in_iq = 0;
            instr.s2_readybit_in_iq = 0;

        

            instr.fe_s = c_count;
			instr.fe_cyc = 1;
			instr.de_s = c_count + 1;

            
                if(instr.op_type == 0)
                instr.cycle_count = 1;

                else if(instr.op_type == 1)
                instr.cycle_count = 2;

                else if(instr.op_type == 2)
                instr.cycle_count = 5;
            
       // cout<<DE.size();
        

                    
        DE.push_back(instr);
        //cout<<instr.pc<<"\n";
        ctr++;
        

            }
            else
            break;
        }
          /*  cout<<"FETCH\n";
                     for ( int i=0; i<DE.size(); i++){
                         cout<<DE.size();
                            cout<<"src1:"<<DE[i].src1<<"src2:"<<DE[i].src2<<"dest"<<DE[i].dest<<DE[i].s1_is_from_ROB<<DE[i].s2_is_from_ROB<<"\n";}

            */

        }


 void set_ROB(int head, int p, int q, int r){

     
			ROB[head].dest_reg =p;
            ROB[head].pc = q;
            ROB[head].ready_bit =r;

 }

 

 void set_RMT(int c, int p, int q){
     
					RMT[c].valid = p;
					RMT[c].tag = q;
 }

       void decode(){
                    int flag=0;
                if(!DE.empty() && RN.empty() )
                flag=1;
                int s=DE.size();
                
                if(flag){
                    int itr=0;
                        while(itr<s){

                            DE[itr].rn_strt = c_count +1;
                            DE[itr].de_cyc = DE[itr].rn_strt - DE[itr].de_s;

                            RN.push_back(DE[itr]);

                            itr++;
                            }
                        // RN=DE;
                    
                    
                    
                  /*  cout<<"DECODE\n";
                     for ( int i=0; i<DE.size(); i++){
                            cout<<"src1:"<<DE[i].src1<<"src2:"<<DE[i].src2<<"dest"<<DE[i].dest<<DE[i].s1_is_from_ROB<<DE[i].s2_is_from_ROB<<"\n";}
*/
                    DE.clear();
                  /*  cout<< RN[0].pc << "\n";
                    cout<< RN[1].pc << "\n";
                    cout<< RN[2].pc<< "\n";*/
                    
                    
                }
                
        }

         int calc_free_space(){
            
             int size1=0;
            if(head==tail)
            {
                if(!ROB[head].pc )
                size1=rob_size;
                else
                size1=0;
                
            }

            if(head<tail)
            size1 =  rob_size -  tail +  head;
            else if( tail< head)
            size1 =  head -  tail;
            
         
                return size1;
        }
void calc_RN(int t)
{
            RN[t].rr_s = c_count + 1;
			RN[t].rn_cyc = RN[t].rr_s - RN[t].rn_strt;

}

void set_dest(int tl, int t){
                            
                            // setting the destination in ROB
                            set_ROB(tl,RN[t].pc,RN[t].dest,0);
                            


                            // update RMT with the destination reg
                            while(RN[t].dest != -1){

                                set_RMT(RN[t].dest,tl,1);

                               
                                break;
                            }
                            // rename the dest 
                            RN[t].dest = tl;
}


void set_destination(int TL,int t)
{
                            // setting the destination in ROB

                            set_ROB(TL,RN[t].dest, RN[t].pc, 0);


                            // update RMT with the destination reg
                            while(RN[t].dest != -1){
                                set_RMT(RN[t].dest, 1,TL);
                                break;

                            }
                            // rename the dest 
                            RN[t].dest = TL;
}


 void set_src_dest(){

      int size=RN.size();
     int t=0;
     while(t < size)
                        {
                         //   cout<<"HERE";
                            
                           
                            // searching for the source regs in RMT
                            // if found, rename
                            while(RN[t].src1 != -1 && RMT[RN[t].src1].valid)
                            {
                               // cout<<"inftn"<<RN[i].src1;

                                RN[t].src1 = RMT[RN[t].src1].tag;
                                RN[t].s1_is_from_ROB =1;
                                
                                break;
                            }
                            

                            while(RN[t].src2 != -1 && RMT[RN[t].src2].valid)
                            {

                                RN[t].src2 = RMT[RN[t].src2].tag;
                                RN[t].s2_is_from_ROB =1;
                                break;
                            }
                            
                            set_destination(tail,t);
                            

                             
                            

                            // increment tail
                            if(tail != rob_size -1)
                                tail++;
                            else
                                tail = (rob_size%tail)-1;

                            calc_RN(t);
                           
					
         

                            t++;
                        }

                        

 }


        void rename(){

            while(!RN.empty() && RR.empty())
           {
                //cout<<"dwwkd";
             int size=RN.size();
             int size1=calc_free_space();


        //cout<<"size:"<<size<<"size1:"<<size1<<"\n";
            
           

            
                    //cout<<"HERE";
                    //cout<<ROB[0].dest_reg;
            if(size1 >= RN.size())
                set_src_dest();
            else
                return;
                        
                      /*  for ( int i=0; i<RN.size(); i++){
                            RR.push_back(RN[i]);}
*/
                  /*  for ( int i=0; i<RN.size(); i++){
                            cout<<"src1:"<<RN[i].src1<<"src2:"<<RN[i].src2<<"dest"<<RN[i].dest<<DE[i].s2_is_from_ROB<<"\n";;
                            printf("%d%d\n",RR[i].s1_is_from_ROB, RR[i].s2_is_from_ROB);} */
                        RR=RN;
                        RN.clear();
            
         /*   cout<<"RENAME\n";
            cout<< RR[0].pc<< "\n";
            cout<< RR[1].pc << "\n";
            cout<< RR[2].pc<< "\n";
            cout<<"done\n";*/
            
            break;
            

           }
        }

        void readreg()
        {
            while(!RR.empty() && DI.empty())
            {
                int i=0;
            while(i < RR.size())
                        {

                         if(!RR[i].s1_is_from_ROB)    // does not exist in ROB
                            RR[i].s1_readybit_in_iq=1;

                        
                        else{  // exists in ROB and ready bit is 1
                            if(RR[i].s1_is_from_ROB==1 && ROB[RR[i].src1].ready_bit==1){
                                RR[i].s1_readybit_in_iq=1;

                            }
                        }


                        if(!RR[i].s2_is_from_ROB)    // does not exist in ROB
                        RR[i].s2_readybit_in_iq=1;

                        
                        else{  // exists in ROB and ready bit is 1
                            if(RR[i].s2_is_from_ROB==1 && ROB[RR[i].src2].ready_bit==1){
                                RR[i].s2_readybit_in_iq=1;

                            }
                        }
                                RR[i].dis = c_count + 1;
				                RR[i].rr_cyc = RR[i].dis - RR[i].rr_s;

                            i++;
                        }
                        
                       /* for ( int i=0; i<RR.size(); i++)
                            DI.push_back(RR[i]);*/
                    
                    DI=RR;
                    RR.clear();
                    break;

           /* cout<<"read reg\n";
            cout<< DI[0].pc << "\n";
            cout<< DI[1].pc << "\n";
            cout<< DI[2].pc<< "\n"; */

                }
            

           
        }


        void dispatch()
        {
            int flag=0;
            if(DI.empty())
                return;
           
        
             int sz=0;
          //  cout<<"iq-size:\n"<<iq_size;
          //  cout<<"iq.size:\n"<<IQ.size();
            sz=iq_size-IQ.size();
           // cout<<"sz:\n"<<sz;
   
          //  cout<<"sz:"<<sz;
            if(sz<DI.size()){
                flag=1;
                
            }
          //  cout<<flag<<"sndj";
            if(!flag){
               // cout<<"sxaHeree";
               int k=0;

                while(k<DI.size()){
                            
                            DI[k].is_s = c_count + 1;
				            DI[k].di_cyc = DI[k].is_s - DI[k].dis;
                            IQ.push_back(DI[k]);

                            k++;

                }
                
               /* for ( int i=0; i<DI.size(); i++){
                            cout<<"src1:"<<DI[i].src1<<"src2:"<<DI[i].src2<<"dest"<<DI[i].dest<<"\n";}*/
                DI.clear();

               // cout<<"IQ";
              /*  for ( int i=0; i<IQ.size(); i++){
                            cout<<"src1:"<<IQ[i].src1<<"ready_bit"<<"src2:"<<IQ[i].src2<<"dest"<<IQ[i].dest<<"\n";
                            cout<<"s1 ready bit:"<<IQ[i].s1_readybit_in_iq<<"s2 ready bit:\n"<<IQ[i].s2_readybit_in_iq;}
                
            cout<<"m=neher";*/
            }
        
       
}


void calc_iqcycle(int w){
    
					IQ[w].ex_s = c_count + 1;
					IQ[w].is_cyc = IQ[w].ex_s - IQ[w].is_s;
                    execute_list.push_back(IQ[w]);
}

void issue()
{
    
	while (IQ.size() > 0)
	{
		int s = 0;

		int flag=0;
		do
	    {
            flag=0;
            int w=0;
			while(w < IQ.size())
			{
				if (IQ[w].s1_readybit_in_iq==1 )
				{
                    if(IQ[w].s2_readybit_in_iq==1)
                    {
                        calc_iqcycle(w);
					
					IQ.erase(IQ.begin() + w);
					s++;
					flag=10;
					break;
                    }
				}
                w++;
			}
            if(flag==0)
            {
                return;
            }
		}while(s<width);
		break;
	}
	
}

void set_ready_bits(int src1, int src2, int seq, int j,int y){
                        if (src1 == execute_list[y].dest)
                            switch(seq)
                            {
                                case 1:
                                IQ[j].s1_readybit_in_iq = 1;
                                break;

                                case 2:
                                DI[j].s1_readybit_in_iq = 1;
                                break;

                                case 3:
                                RR[j].s1_readybit_in_iq = 1;
                                break;


                            }
							

						if (src2 == execute_list[y].dest)
							switch(seq)
                            {
                                case 1:
                                IQ[j].s2_readybit_in_iq = 1;
                                break;
                                
                                case 2:
                                DI[j].s2_readybit_in_iq = 1;
                                break;

                                case 3:
                                RR[j].s2_readybit_in_iq = 1;
                                break;

                            }
}


void wakeupcalls(int y){
    int j=0;
    int seq=0;

    while(j < IQ.size())										
					{
                        seq=1;
                        set_ready_bits(IQ[j].src1,IQ[j].src2, seq, j,y);
						

                            j++;
					}
        int k=0;
					while(k < DI.size())									
					{
                        seq=2;
                        set_ready_bits(DI[k].src1,DI[k].src2, seq, k,y);
						
                            k++;
					}
        int l=0;
					while(l < RR.size())									
					{
                        seq=3;
						set_ready_bits(RR[l].src1,RR[l].src2, seq, l,y);

                            l++;
					}
}

void calc_ex_cyc(int i){

    
					execute_list[i].wb_s = c_count + 1;
					execute_list[i].ex_cyc = execute_list[i].wb_s - execute_list[i].ex_s;

					WB.push_back(execute_list[i]);

}
    void Execute()
    {
        int flag1=0;

        if(execute_list.size() == 0)
            flag1=1;

	if(!flag1)
	{
        
        for(int k=0;k<execute_list.size();k++){
		execute_list[k].cycle_count--;}



        
		int flag = 1;
		
		do
		{
			flag = 0;

			for ( int i = 0; i < execute_list.size(); i++)
			{
				while (execute_list[i].cycle_count == 0)
				{
                    calc_ex_cyc(i);
                   // cout<<"IN HERE"<<WB.size();

                   wakeupcalls(i);

					

					execute_list.erase(execute_list.begin() + i);
					flag=10;
					break;
				}
			}
             if(flag==0)
            {
                return;
            }
		}while (flag != 0);

		
	}
}
                void write_b(int k){

                    ROB[WB[k].dest].ready_bit = 1;
                    WB[k].rt_s = c_count + 1;
			        WB[k].wb_cyc = WB[k].rt_s - WB[k].wb_s; 

                    
                }

       void Writeback(){
         //  cout<<"wbsize"<<WB.size();
         int flag=0;
         int s=WB.size();
            if(s)
            {
                flag=1;
            }
            if(flag){
                int k=0;
                while(k< s){

                    write_b(k);

                    k++;

                    
			
                    }

                 for( int h=0; h<s;h++)
                    RT.push_back(WB[h]);

                WB.clear();
            }

           
           /* 
            for ( int i=0; i<WB.size(); i++){
                    cout<<"src1:"<<WB[i].src1<<"src2:"<<WB[i].src2<<"dest"<<WB[i].dest<<"\n";}
             */           
                     
        }



        void print_values(int i){

            RT[i].rt_cyc = (c_count + 1) - RT[i].rt_s;

					cout << RT [i].pc << " fu{" << RT [i].op_type << "} src{" << RT [i].src1_actual << "," << RT [i].src2_actual << "} ";
					printf("dst{%d",ROB[head].dest_reg);
                    cout << "} FE{" << RT [i].fe_s << "," << RT [i].fe_cyc << "} ";
					cout << "DE{" << RT [i].de_s << "," << RT [i].de_cyc << "} RN{" << RT [i].rn_strt << "," << RT [i].rn_cyc << "} ";
					cout << "RR{" << RT [i].rr_s << "," << RT [i].rr_cyc << "} DI{" << RT [i].dis << "," << RT [i].di_cyc << "} ";
					cout << "IS{" << RT [i].is_s << "," << RT [i].is_cyc << "} EX{" << RT [i].ex_s << "," << RT [i].ex_cyc << "} ";
					cout << "WB{" << RT [i].wb_s << "," << RT [i].wb_cyc << "} RT{" << RT [i].rt_s << "," << RT [i].rt_cyc << "} " << endl;

					RT.erase(RT.begin() + i);
        }
 

    void Retire()
    
    
    {
        
           

    for(int k=0;k<width;k++)

    {

        

        if (calc_free_space()==rob_size)
		
				return;
		

		
            int n=0;
            while(n < RR.size())
			{
				if (RR[n].src1 == head && ROB[head].ready_bit==1)
					RR[n].s1_readybit_in_iq = 1;

				if (RR[n].src2 == head && ROB[head].ready_bit==1)
					RR[n].s2_readybit_in_iq = 1;

                    n++;
                
			}


            int i=0;
		    while (i < RT.size())
			{
				if (RT[i].pc == ROB[head].pc && ROB[head].ready_bit==1)
				{

                    print_values(i);
                            
					break;
				}
                i++;

			}

             int c=0;
			while(c < rmt_size)
			{
				while (head==RMT[c].tag && ROB[head].ready_bit==1)
				{

                    set_RMT(c,0,0);
                    break;
				}
                c++;
			}


while(ROB[head].ready_bit==1)
        {
            set_ROB(head,0,0,0);

            int end=0;
            if(head== rob_size-1 )
             end =1;



			if (end)
            head = (rob_size%head)-1;
				
			else
            head++;
				
        break;
			
        }
	}
		
 
                  
}



        int flag=0;

        bool Advance_Cycle(FILE* FP)
        {
            c_count++;

            if(er && regsEmpty)
                
             return false;
            else
             return true;
        }

        int main (int argc, char* argv[])
        {
            FILE *FP;               // File handler
            char *trace_file;       // Variable that holds trace file name;
            proc_params params;       // look at sim_bp.h header file for the the definition of struct proc_params
            
            if (argc != 5)
            {
                printf("Error: Wrong number of inputs:%d\n", argc-1);
                exit(EXIT_FAILURE);
            }
            
            params.rob_size     = strtoul(argv[1], NULL, 10);
            params.iq_size      = strtoul(argv[2], NULL, 10);
            params.width        = strtoul(argv[3], NULL, 10);
            trace_file          = argv[4];
         /*   printf("rob_size:%lu "
                    "iq_size:%lu "
                    "width:%lu "
                    "tracefile:%s\n", params.rob_size, params.iq_size, params.width, trace_file);*/
            // Open trace_file in read mode
            iq_size=params.iq_size ;
            width=params.width;
            rob_size = params.rob_size;
            FP = fopen(trace_file, "r");

            rmt_size=67;

            c_count =0;
            pc=0;
            regsEmpty=0;
            



            if(FP == NULL)
            {
                // Throw error and exit if fopen() failed
                printf("Error: Unable to open file %s\n", trace_file);
                exit(EXIT_FAILURE);
            }

             /*   instrr instr;
                instr.s1_readybit_in_iq = 0;
                instr.s2_readybit_in_iq = 0;
                instr.s1_is_from_ROB =0;
                instr.s2_is_from_ROB =0;
                
*/
                
            head=tail=3;
            ROBcell rb;
            rb.dest_reg=0;
            rb.pc=0;
            rb.ready_bit=0;
            for( int i=0; i<rob_size;i++){
                ROB.push_back(rb);
            } 

               RMTcell rm;
               rm.tag = -1;
               rm.valid = 0;

             for (int i = 0; i < 67; i++)
	            {
	        	RMT.push_back(rm);
                }

            do
           {
              

              Retire();
              Writeback();
              Execute();
              issue();
              dispatch();
              readreg();
              rename();
              decode();
              fetch(FP);
             
              /* fetch(FP);
              decode();
              rename();
              readreg();
              dispatch();
              issue();
              Execute();
              Writeback();
              Retire();*/
              
           
             //Execute();
             int check_regs=0;

             if(DE.empty() && RN.empty() && RR.empty() && DI.empty())
             check_regs =1;

             int check_wts=0;

             if(IQ.size() == 0 && WB.size() == 0 && execute_list.size() == 0 )
             check_wts =1;

              
                
                if ( check_regs && check_wts )
			    if (calc_free_space()==rob_size)
					regsEmpty = 1;

		
                
                 
                
                
                
                

            }
            while (Advance_Cycle(FP));
            fclose(FP);
            
            
            cout << "# === Simulator Command =========" << endl;
	cout << "# ";
	for (int i = 0; i < argc; i++)
		cout << argv[i] << " ";
	cout << endl << "# === Processor Configuration ===" << endl;
	cout << "# ROB_SIZE = " << rob_size << endl;
	cout << "# IQ_SIZE  = " << iq_size << endl;
	cout << "# WIDTH    = " << width << endl;
	cout << "# === Simulation Results ========" << endl;
	cout << "# Dynamic Instruction Count    = " << ctr << endl;
	cout << "# Cycles                       = " << c_count << endl;
	cout << "# Instructions Per Cycle (IPC)  = " << fixed << setprecision(2) << ((float)ctr / (float)c_count) << endl;

                
        }
                
            //  printf("%lx %d %d %d %d\n", pc, op_type, dest, src1, src2); //Print to check if inputs have been read correctly
            //  if(DE.empty())
            //  flag =1;
                
           
        //}
