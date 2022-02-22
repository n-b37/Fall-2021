      /*******************************************************
                               cache.cc
      ********************************************************/

      #include <stdlib.h>
      #include <assert.h>
      #include <iostream>
      #include <stdio.h>
      #include "cache.h"
      #include <iomanip>
      #include <iostream>
      #include <fstream>
      using namespace std;
      
  

   bool Cache::cE (ulong addr, int processor, int num_proccessor, Cache * cachesArray[])
   {
      int cpy = 0;
      int i=0;
      while(i<num_proccessor)
      {
         if (i!=processor)
         {
               if((cachesArray[i]->findLine(addr))!=NULL)
               {
                  cpy++;
                  break;
               }
         }
         i++;
      }

      if (cpy == 0)
         return false;
      else
         return true;

   }



      Cache::Cache(int s,int a,int b )
      {
         ulong i, j;
         reads = readMisses = writes = 0; 
         writeMisses = writeBacks= currentCycle = 0;
         mem_t = interventions = invalidations = flushes =  num_busrdx = c2c = 0;
         
         size       = (ulong)(s);
         lineSize   = (ulong)(b);
         assoc      = (ulong)(a);   
         sets       = (ulong)((s/b)/a);
         numLines   = (ulong)(s/b);
         log2Sets   = (ulong)(log2(sets));   
         log2Blk    = (ulong)(log2(b));   
      
         //*******************//
         //initialize your counters here//
         //*******************//
      
         tagMask =0;
         for(i=0;i<log2Sets;i++)
         {
            tagMask <<= 1;
            tagMask |= 1;
         }
         cache= NULL;
         /**create a two dimentional cache, sized as cache[sets][assoc]**/ 
         cache = new cacheLine*[sets];
         for(i=0; i<sets; i++)
         {
            cache[i]=NULL;
            cache[i] = new cacheLine[assoc];
            for(j=0; j<assoc; j++) 
            {
            cache[i][j].invalidate();
            }
         }      
         
      }

      /**you might add other parameters to Access()
      since this function is an entry point 
      to the memory hierarchy (i.e. caches)**/
      void Cache::Access(ulong addr,uchar op, int protocol, int processor,int num_processors, Cache* cachesArray[])
      {
         currentCycle++;/*per cache global counter to maintain LRU order 
               among cache ways, updated on every cache access*/
         //int num_processors = 4;
         if(op == 'w') writes++;
         else          reads++;
         
         cacheLine * line = findLine(addr);
      if(line == NULL)/*miss*/
         {
            //writeMisses++;
            //writemiss msi
            cacheLine *nline = fillLine(addr);
            if(op == 'w')
         {     
            if(protocol == 0)
            {
               writeMisses++;
               num_busrdx++;
               //mem_t++;
               int a = 0;
               
               while(a < num_processors){
                  //cout<< "addsdsd";
                  if( a != processor){
                     
                     cacheLine *check_line = cachesArray[a]->findLine(addr);
                     
                     if(check_line){
                        
                        switch (check_line->getFlags())
                        {


                           case MODIFIED:

                              //cout<<"inside modified \n";
                           // mem_t++;
                              cachesArray[a]->writeBack(addr);
                              check_line -> setFlags(INVALID);
                              cachesArray[a]->invalidations++;
                              cachesArray[a]->flushes++;
                              check_line->invalidate();
                           break;

                           case SHARED:
                           //cout<<"inside dhsred \n";
                           //  mem_t++;
                              check_line -> setFlags(INVALID);
                              cachesArray[a]->invalidations++;
                              cachesArray[a]->findLine(addr)->invalidate();
                           break;

                        }
                     }
                  }
                  a++;
               }

               nline -> setFlags(MODIFIED);

            }
      // write miss MESI
        else if(protocol == 1)
         {
            
               writeMisses++;
               num_busrdx++;
               if( cE (addr, processor, num_processors, cachesArray) ) 
            {
               c2c++;  
            }
               int i=0;
      while(i<num_processors)
      {
         if (i!=processor)
         {
               cacheLine *check_for_line = cachesArray[i]->findLine(addr);
               if(check_for_line!=NULL)
               {
                  ulong flag = check_for_line->getFlags();
                  switch(flag)
                  {
                     case MODIFIED:

                        cachesArray[i]->writeBack(addr);
                     check_for_line->setFlags(INVALID);
                     cachesArray[i]->invalidations++;
                     cachesArray[i]->flushes++;
                     check_for_line->invalidate();

                     break;

                     case SHARED:
                  
                     check_for_line->setFlags(INVALID);
                     cachesArray[i]->invalidations++;
                     check_for_line->invalidate();
                     // cachesArray[i]->flushes++;
                     break;

                     case EXCLUSIVE:
                  
                     check_for_line->setFlags(INVALID);
                     cachesArray[i]->invalidations++;
                     check_for_line->invalidate();
                     // cachesArray[i]->flushes++;
                     break;
               }
         }
        
      } i++;
   }
               nline -> setFlags(MODIFIED);   
            // mem_t++;
                  //   check for other copies
                  /*   int copies, b;
                     copies =0;
                     b=0;
                     while(b < num_processors)
                     {
                        if(b != processor){
                           cacheLine *check_line = cachesArray[b]->findLine(addr);
                           if(check_line != NULL){
                           copies=1; break;}
                        }
                        b++;
                     }*/
      // while other copy exists
         
         
            
            } 
      
      
     else if(protocol == 2){


                     writeMisses++;


            if(cE (addr, processor, num_processors, cachesArray))
            {
               nline->setFlags(SHARED_MOD);
               int d=0;
               while ( d < num_processors)
               {
                  if ( d != processor)
                  {
                     cacheLine *check_line = cachesArray[d]->findLine(addr);
                     if(check_line){
                        ulong flag1 = check_line->getFlags();

                        if(flag1==EXCLUSIVE){

                              check_line -> setFlags(SHARED);
                              cachesArray[d]->interventions++;
                        }

                         else if(flag1==MODIFIED){

                              check_line -> setFlags (SHARED_MOD);
                              cachesArray[d]->interventions++;
                              cachesArray[d]->flushes++;
                              // cachesArray[d]->findLine(addr)->invalidate();
                           }

                        else if (flag1==SHARED_MOD){
                             // check_line->setFlags(SHARED);
                               cachesArray[d]->flushes++;
                           }

                        }
                     }
                  

                  d++;
            }

              int i=0;

      while(i<num_processors)
      {
         if (i!=processor)
         {
               cacheLine *check_for_line = cachesArray[i]->findLine(addr);
               if(check_for_line!=NULL)
                  if (check_for_line->getFlags() == SHARED_MOD)
                     check_for_line->setFlags(SHARED);
         }
         i++;
      }

            } 

            else{
               nline -> setFlags (MODIFIED);
               
               int s=0;
               while ( s < num_processors)
               {
                  if ( s != processor)
                  {
                     cacheLine *check_line = cachesArray[s]->findLine(addr);
                     if(check_line){
                        ulong flag1 = check_line->getFlags();

                        if(flag1==EXCLUSIVE){

                              check_line -> setFlags(SHARED);
                              cachesArray[s]->interventions++;
                          }

                        if(flag1==MODIFIED){

                              check_line -> setFlags (SHARED_MOD);
                              cachesArray[s]->interventions++;
                              cachesArray[s]->flushes++;
                         }

                         if(flag1==SHARED_MOD)
                              cachesArray[s]->flushes++;
                           

                        
                     }
                  }

                  s++;
               }
            }


      } // close p2
         }  //close w 







         else  //read miss msi
         {  
            
         readMisses++;
            if( protocol == 0){
               //readMisses++;

               int a = 0;
               
               while(a < num_processors){
                  
                  if( a != processor){
                     
                     cacheLine *check_line = cachesArray[a]->findLine(addr);
                     
                     if(check_line != NULL){
                        ulong flag = check_line->getFlags();
                        //mem_t++;
                        //cout<< flag << "\n";
                        switch (flag)
                        {


                           case MODIFIED:

                              //cout<<"inside modified \n";
                              //mem_t++;
                              cachesArray[a]->writeBack(addr);
                              check_line -> setFlags(SHARED);
                              cachesArray[a]->interventions++;
                              cachesArray[a]->flushes++;
                           break;

                           case SHARED:
                           //cout<<"inside dhsred \n";
                              check_line -> setFlags(SHARED);
                           break;

                           case INVALID:
                           //cout<<"inside exc \n";
                              check_line -> setFlags(INVALID);
                           break;


                        }
                     }
                  }
                  a++;
               }

               nline -> setFlags(SHARED);


         
            }
         
      // read miss mesi
        else if(protocol == 1){
            
                  //cout<<"HEREHE";
                     int cpy, r;
                     cpy =0;
                     r=0;
                     while(r < num_processors)
                     {
                        if(r != processor){
                           cacheLine *check_line = cachesArray[r]->findLine(addr);
                           if(check_line!=NULL){
                           cpy=1; break;}
                        }
                        r++;
                     }
   

   if(cpy == 1)     
               { 
                     
                     

   nline->setFlags(SHARED);
   c2c++;
    }

   else
   nline->setFlags(EXCLUSIVE);
                     
     int i=0;
      while(i<num_processors)
      {
         if (i!=processor)
         {
               cacheLine *check_for_line = cachesArray[i]->findLine(addr);
               if(check_for_line!=NULL)
               {
                  ulong flag = check_for_line->getFlags();
                  switch(flag)
                  {
                     case(MODIFIED):
                     check_for_line->setFlags(SHARED);
                           cachesArray[i]->interventions++;
                           cachesArray[i]->writeBack(addr);
                               cachesArray[i]->flushes++;
                     break;

                     case(EXCLUSIVE):
                        check_for_line->setFlags(SHARED);
                           cachesArray[i]->interventions++;
                     break;


                    
               }}
         }
         i++;
      }
   
   }
      
      
            else if(protocol == 2)
            {

               int flag, r;
                     flag =0;
                     r=0;
                     while(r < num_processors)
                     {
                        if(r != processor){
                           cacheLine *check_line = cachesArray[r]->findLine(addr);
                           if(check_line){
                           flag=1; break;}
                        }
                        r++;
                     }
                     
               if(flag == 1){
                  nline->setFlags(SHARED);
               }
               else{
                  nline->setFlags(EXCLUSIVE);
               }

               int t=0;

               while(t<num_processors){

                  if(t!=processor)
                  {

                     cacheLine *check_line = cachesArray[t]->findLine(addr);
                     if(check_line)
                     {

                        ulong flag = check_line->getFlags();



                        if (flag == EXCLUSIVE)
                        {
                              check_line->setFlags(SHARED);
                              cachesArray[t]->interventions++;
                        }
                        else if (flag == MODIFIED)
                        {
                              check_line->setFlags(SHARED_MOD);
                              cachesArray[t]->interventions++;
                              cachesArray[t]->flushes++;
                        }
                        else if (flag == SHARED_MOD)
                              cachesArray[t]->flushes++;
                     }

                  }

               t++;
               }

      }

      
               
      }   // close miss 
         }
         
         else
         {
            /**since it's a hit, update LRU and update dirty flag**/
            updateLRU(line);
            
            if(op == 'w')
            {
                  ulong flag1 = line->getFlags();

                  if(protocol ==0)
                  {
                     switch(flag1)
                     {
                        
                        case SHARED:

                              line->setFlags(MODIFIED);
                              mem_t++;
                              num_busrdx++;
                           
                              
                              int a = 0;
               while(a < num_processors){
                  
                  if( processor != a){

                     cacheLine *check_line = cachesArray[a]->findLine(addr);

                     if(check_line!=NULL){
                        ulong flag = check_line->getFlags();

                        switch (flag)
                        {
                           case MODIFIED:
                              cachesArray[a]->writeBack(addr);
                              check_line -> setFlags(INVALID);
                              cachesArray[a]->invalidations++;
                              cachesArray[a]->flushes++;
                              check_line->invalidate();
                              //mem_t ++;
                           break;

                           case SHARED:
                              check_line -> setFlags(INVALID);
                              cachesArray[a]->invalidations++;
                              check_line->invalidate();
                              
                           break;

                           case INVALID:
                              check_line -> setFlags(INVALID);
                           break;


                        }
                     }
                  }
                  a++;
               }



                        break;
                     }

                  }

                  else if(protocol == 1){
                     
                  ulong flag1 = line->getFlags();
                 
                        if(flag1==SHARED)
                        {
    
                        line->setFlags(MODIFIED);

                        int i=0;
      while(i<num_processors)
      {
         if (i!=processor)
         {
               cacheLine *check_for_line = cachesArray[i]->findLine(addr);
               if(check_for_line!=NULL)
               {
                  ulong flag = check_for_line->getFlags();
                  if (flag == SHARED)
                  {
                     check_for_line->setFlags(INVALID);
                     cachesArray[i]->invalidations++;
                     check_for_line->invalidate();
                    // cachesArray[i]->flushes++;
                  }
               }
         }
         i++;
      }
                        }

                       else if(flag1==EXCLUSIVE){
                           line->setFlags(MODIFIED);
                        }

                        /*int g=0;
                        while(g<num_processors)
         {
               if (g!=processor)
            {
                  cacheLine *check_for_line = cachesArray[g]->findLine(addr);
                  if(check_for_line!=NULL)
                  {
                     ulong flag = check_for_line->getFlags();
                     if (flag == SHARED)
                     {
                        check_for_line->setFlags(INVALID);
                        cachesArray[g]->invalidations++;
                     }
                     
                  }
            }
            g++;
         }*/
         
                  
                        
                     }
                     
                     
                  

                  else if(protocol == 2){
                     ulong flag1 = line->getFlags();
                     if(flag1 == SHARED)
                    {
                        int i=0;

      while(i<num_processors)
      {
         if (i!=processor)
         {
               cacheLine *check_for_line = cachesArray[i]->findLine(addr);
               if(check_for_line!=NULL)
                  if (check_for_line->getFlags() == SHARED_MOD)
                     check_for_line->setFlags(SHARED);
         }
         i++;
      }
                        if (cE(addr, processor, num_processors, cachesArray))
                            line->setFlags(SHARED_MOD);
                        else
                            line->setFlags(MODIFIED);
                    }
                    else if (flag1 == SHARED_MOD)
                    {
                        int i=0;

      while(i<num_processors)
      {
         if (i!=processor)
         {
               cacheLine *check_for_line = cachesArray[i]->findLine(addr);
               if(check_for_line!=NULL)
                  if (check_for_line->getFlags() == SHARED_MOD)
                     check_for_line->setFlags(SHARED);
         }
         i++;
      }
                        if (!cE(addr, processor, num_processors, cachesArray))
                            line->setFlags(MODIFIED);
                    }
                    else if (flag1 == EXCLUSIVE)
                        line->setFlags(MODIFIED);

                  }     
            }
         }
      }

      /*look up line*/
      cacheLine * Cache::findLine(ulong addr)
      {
         ulong i, j, tag, pos;
         
         pos = assoc;
         tag = calcTag(addr);
         i   = calcIndex(addr);
      
         for(j=0; j<assoc; j++)
         if(cache[i][j].isValid())
               if(cache[i][j].getTag() == tag)
            {
               pos = j; break; 
            }
         if(pos == assoc)
         return NULL;
         else
         return &(cache[i][pos]); 
      }

      /*upgrade LRU line to be MRU line*/
      void Cache::updateLRU(cacheLine *line)
      {
      line->setSeq(currentCycle);  
      }

      /*return an invalid line as LRU, if any, otherwise return LRU line*/
      cacheLine * Cache::getLRU(ulong addr)
      {
         ulong i, j, victim, min;

         victim = assoc;
         min    = currentCycle;
         i      = calcIndex(addr);
         
         for(j=0;j<assoc;j++)
         {
            if(cache[i][j].isValid() == 0) return &(cache[i][j]);     
         }   
         for(j=0;j<assoc;j++)
         {
         if(cache[i][j].getSeq() <= min) { victim = j; min = cache[i][j].getSeq();}
         } 
         assert(victim != assoc);
         
         return &(cache[i][victim]);
      }

      /*find a victim, move it to MRU position*/
      cacheLine *Cache::findLineToReplace(ulong addr)
      {
         cacheLine * victim = getLRU(addr);
         updateLRU(victim);
      
         return (victim);
      }

      /*allocate a new line*/
      cacheLine *Cache::fillLine(ulong addr)
      { 
         ulong tag;
      
         cacheLine *victim = findLineToReplace(addr);
         assert(victim != 0);
         if(victim->getFlags() == MODIFIED || victim->getFlags() == SHARED_MOD) 
            writeBack(addr);

            
         tag = calcTag(addr);   
         victim->setTag(tag);
         victim->setFlags(INVALID);    
         /**note that this cache line has been already 
            upgraded to MRU in the previous function (findLineToReplace)**/

         return victim;
      }

      void Cache::printStats(int i, int protocol)
      {
         cout<<"============ Simulation results (Cache "<<i<<") ============\n";
         cout<<" 01. number of reads:      "<<reads<<"\n 02. number of read misses:      "<<readMisses;
         cout<<"\n 03. number of writes:      "<<writes<<"\n 04. number of write misses:      "<<writeMisses;
         cout<<"\n 05. total miss rate:      "<<fixed <<setprecision(2) <<(float)((readMisses+writeMisses)*100)/(reads+writes)<<"%";
         cout<<"\n 06. number of writebacks:      "<<writeBacks;
         cout<<"\n 07. number of cache-to-cache transfers:     "<<c2c;
         if (protocol == 0)
            cout<<"\n 08. number of memory transactions:       "<<readMisses+writeMisses+writeBacks+mem_t;
         else
            cout<<"\n 08. number of memory transactions:       "<<readMisses+writeMisses+writeBacks-c2c;
         cout<<"\n 09. number of interventions:      "<<interventions;
         cout<<"\n 10. number of invalidations:      "<<invalidations;
         cout<<"\n 11. number of flushes:      "<<flushes;
         cout<<"\n 12. number of BusRdX:      "<<num_busrdx<<endl;

         /****print out the rest of statistics here.****/
         /****follow the ouput file format**************/
      }
