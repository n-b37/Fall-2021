module MyDesign(input clk, input reset_b, input dut_run, output reg dut_busy, 
output reg [11:0] dut_sram_write_address,input[15:0] sram_dut_read_data, 
output reg[11:0] dut_sram_read_address,
output reg[15:0] dut_sram_write_data, output[11:0] dut_wmem_read_address, 
input [15:0]  wmem_dut_read_data, output reg dut_sram_write_enable,output reg process_signal);


assign dut_wmem_read_address= 16'b1;

reg [15:0] size_count_val;


reg select_for_read;
reg last_input;
reg [15:0] reg_read;
reg [1:0]select_for_size_cout;
reg [15:0] reg_size_count;
reg [1:0] shiftreg_count_sel;
reg [2:0] shift_reg;
reg [15:0] shift_reg1;
reg [15:0] shift_reg2;
reg [15:0] shift_reg3;
reg [15:0] count_value;
reg [15:0] weight_reg;
//reg [9:0] value_reg [13:0];
//reg [9:0] value [13:0];
reg [13:0] res;
reg [15:0] res_reg;
reg input_valid; 
reg[3:0] current_state;
reg[3:0] next_state;
reg process_signal_sel;
reg[3:0] count;
reg dimen_value;
reg [1:0] dimension_code;
//reg[3:0] dimen_reg;
//reg [3:0] zero_flag[13:0];
//reg [3:0] one_flag[13:0];

integer i;
integer j;

always@(posedge clk or negedge reset_b)
begin
    if(!reset_b)
	
            weight_reg <= 0;
    
    else if(dut_run == 1)
            weight_reg <= wmem_dut_read_data;
            
    
    else
        weight_reg <= weight_reg;
   
end

always@(*)
begin
    if(select_for_read == 2'b00)
        reg_read = 0;
    if(select_for_read == 2'b01)
        reg_read = dut_sram_read_address + 12'b1;
    if(select_for_read == 2'b11)
        reg_read = dut_sram_read_address;
end

always@(posedge clk )
begin
    dut_sram_read_address <= reg_read ;
end


always@(*)
begin
    if(select_for_size_cout== 2'b00)
        reg_size_count = sram_dut_read_data;
    if(select_for_size_cout == 2'b01)
        reg_size_count = size_count_val - 16'b1;
    if(select_for_size_cout == 2'b11)
        reg_size_count = size_count_val;
    else
        reg_size_count = reg_size_count;
end


always@(posedge clk)
begin
    size_count_val <= reg_size_count;
end


always@(*)
begin
    if(shiftreg_count_sel == 2'b00)
        shift_reg = 2'b11;
    if(shiftreg_count_sel == 2'b01)
        shift_reg = count_value - 2'b01;
    if(shiftreg_count_sel == 2'b11)
        shift_reg = count_value;
    else 
        shift_reg = shift_reg;
end

always@(posedge clk )
begin
    count_value <= shift_reg;
end

always@(posedge clk or negedge reset_b)
begin
    if(!reset_b)
    begin
        shift_reg1 <= 0;
        shift_reg2 <= 0;
        shift_reg3 <= 0;
    end

    else if(input_valid == 1)
    begin
    shift_reg1 <= sram_dut_read_data;
    shift_reg2 <= shift_reg1;
    shift_reg3 <= shift_reg2;
    end

    else
    begin
        shift_reg1 <= shift_reg1;
        shift_reg2 <= shift_reg2;
        shift_reg3 <= shift_reg3;
    end
end

always@(posedge clk or negedge reset_b)
begin
if(!reset_b)
	process_signal <=0;
else if(process_signal_sel==2'b00)
	process_signal <=0;
else
	process_signal <=1;
end






   localparam s0 = 3'b000;
   localparam s1 = 3'b001;
   localparam s2 = 3'b010;
   localparam s3 = 3'b011;
   localparam s4 = 3'b100;
   localparam s5 = 3'b101;
   localparam s6 = 3'b110;
   localparam s7 = 3'b111;
   localparam s8 = 4'b1000;
    
   
always @(posedge clk or negedge reset_b)
   begin
   if (!reset_b)
      current_state <= 4'b0;
   else
     
      current_state <= next_state;
  
   end

always @(*)
   begin
   case (current_state)
   s0:
   begin
	process_signal_sel=0;
	dut_busy=0;
        dimen_value = 0;
        input_valid=1'b0;
        last_input=1;
       select_for_read = 2'b11;
       shiftreg_count_sel = 2'b11;
       select_for_size_cout = 2'b11;
       if(dut_run==1)
       next_state = s1;
	else
	next_state = s0;
	
   end

   s1:
   begin
        last_input=0;
       process_signal_sel=0;
       input_valid=1'b0;
       select_for_read = 2'b00; // 1
       shiftreg_count_sel = 2'b11; //latch
       select_for_size_cout = 2'b11; //latch
       dimen_value = 0;
	next_state = s2;
	dut_busy=1;

   end

   s2:
   begin
    last_input=0;
	process_signal_sel=0;
       input_valid=1'b0;
       select_for_read = 2'b11; // latch
       shiftreg_count_sel = 2'b11; 
       select_for_size_cout = 2'b11; 
       dimen_value = 0;
	next_state = s3;
	dut_busy=1;
   end


   s3:
   begin
    last_input=0;
    dut_busy=1;
	process_signal_sel=0;
       input_valid=1'b0;
       select_for_read = 2'b01; // +1
       shiftreg_count_sel = 2'b00; 
       select_for_size_cout = 2'b00; //get dimensions from i/p
	dimen_value = 0; 
	if(sram_dut_read_data != 16'h00FF)
	next_state = s4;
	else
    begin
	next_state = s0;
    last_input=1;
    end
	
   end

	
    
    s4:
    begin
     last_input=0;
       process_signal_sel=0;
       input_valid =1'b0;
       select_for_read = 2'b01; // +1
       shiftreg_count_sel = 2'b01; 
       select_for_size_cout = 2'b01; // -1
       dimen_value = 1;
	next_state = s5;
	dut_busy=1;
    end

    s5:
    begin
     last_input=0;
	dut_busy=1;
	process_signal_sel=0;
        input_valid = 1'b1;
        select_for_read = 2'b01; //+1
        select_for_size_cout = 2'b01; // 
        shiftreg_count_sel = 2'b01; // 
        dimen_value = 0;

        if(count_value==16'd1)
        begin
            next_state = s6;
        end

        else
            next_state = s5;

	
    end


    s6:
    begin
     last_input=0;
	dut_busy=1;
    dimen_value = 0;
	select_for_read = 2'b01; //+1
        select_for_size_cout = 2'b01; // 
        shiftreg_count_sel = 2'b11; //
	process_signal_sel=1;
        input_valid = 1'b1;
        if(size_count_val!=1)
        begin
           
            next_state = s6;
        end

        else
        begin
            next_state = s7;
	
        end
        
    end


    s7:
	begin

     last_input=0;
	dut_busy=1;
    dimen_value = 0;
	select_for_read = 2'b01; //+1 addr for next 
	
        select_for_size_cout = 2'b11; // latch
        shiftreg_count_sel = 2'b11; // 
	process_signal_sel=1;
        input_valid = 1'b1;
	
	next_state = s3;
	

	end

   s8:

	begin
 last_input=0;
	dut_busy=1;
    dimen_value = 0;
	select_for_read = 2'b01; //+1 addr for next 
        select_for_size_cout = 2'b00; // latch
        shiftreg_count_sel = 2'b00; // 
	process_signal_sel=0;
        input_valid = 1'b0;
	if(sram_dut_read_data != 16'h00FF)
	next_state = s3;
	else
	next_state = s0;
	
	

	end

	

default : 
begin
dimen_value = 0;
process_signal_sel = 0;
dut_busy = 0;
select_for_read = 2'b11;
shiftreg_count_sel = 2'b11;
select_for_size_cout = 2'b11;
next_state = s0;
last_input = 0;
input_valid = 1'b0;

end


endcase
end





always@(*)
begin
if(process_signal == 1)
begin

for(i=0;i<14;i=i+1)
begin
count=0;


if(shift_reg3[i] ~^ weight_reg[0] ==0)
count=count+1;
else 
count = count;

if(shift_reg3[i+1] ~^ weight_reg[1] ==0)
count=count+1;
else
count = count;

if(shift_reg3[i+2] ~^ weight_reg[2] ==0)
count=count+1;
else
count = count;

if(shift_reg2[i] ~^ weight_reg[3] ==0)
count=count+1;
else
count = count;

if(shift_reg2[i+1] ~^ weight_reg[4] ==0)
count=count+1;
else
count = count;

if(shift_reg2[i+2] ~^ weight_reg[5] ==0)
count=count+1;
else
count = count;

if(shift_reg1[i] ~^ weight_reg[6] ==0)
count=count+1;
else
count = count;

if(shift_reg1[i+1] ~^ weight_reg[7] ==0)
count=count+1;
else
count = count;

if(shift_reg1[i+2] ~^ weight_reg[8] ==0)
count=count+1;
else
count = count;






if(count>4)
	res[i] =1'b0;
else
	res[i] = 1'b1;
end



end
else 
begin
count=4'b0;
for(i=0;i<14;i=i+1)
begin
res[i] = 1'b0;
end
end
end


always@(posedge clk )
begin
    if(!reset_b)
    dimension_code <= 0;

    if(dimen_value == 1)
        begin
            if(sram_dut_read_data == 16)
               dimension_code <= 1;
            if(sram_dut_read_data == 12)
               dimension_code <= 2;
            if(sram_dut_read_data == 10)
               dimension_code <= 3;
        end
    else 
           dimension_code <= dimension_code; 
end








always@(*)
begin

if(process_signal == 1)
    begin

    case(dimension_code)
    2'h0: res_reg = 16'b0;
    2'h1: res_reg = {2'b0,res[13:0]};
    2'h2: res_reg= {6'b0,res[9:0]};
    2'h3: res_reg = {8'b0,res[7:0]};
    default: res_reg = 16'b0;
    endcase
    
    end

else
res_reg = 16'b0;


end



always@(posedge clk )
begin

    if(last_input == 1)
    dut_sram_write_address <= -1;

    if(!reset_b)
    dut_sram_write_address <= -1;
    else if (process_signal == 1)
    begin
        dut_sram_write_address <= dut_sram_write_address + 1;
        dut_sram_write_data <= res_reg;
        dut_sram_write_enable <= 1;
    end

    else
    begin
        dut_sram_write_data <= dut_sram_write_data;
        dut_sram_write_enable <= 0;
    end

end







endmodule
