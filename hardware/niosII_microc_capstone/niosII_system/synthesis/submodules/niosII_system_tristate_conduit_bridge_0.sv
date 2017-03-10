// (C) 2001-2013 Altera Corporation. All rights reserved.
// Your use of Altera Corporation's design tools, logic functions and other 
// software and tools, and its AMPP partner logic functions, and any output 
// files any of the foregoing (including device programming or simulation 
// files), and any associated documentation or information are expressly subject 
// to the terms and conditions of the Altera Program License Subscription 
// Agreement, Altera MegaCore Function License Agreement, or other applicable 
// license agreement, including, without limitation, that your use is for the 
// sole purpose of programming logic devices manufactured by Altera and sold by 
// Altera or its authorized distributors.  Please refer to the applicable 
// agreement for further details.


// $Id: //acds/rel/12.1sp1/ip/merlin/altera_tristate_conduit_bridge/altera_tristate_conduit_bridge.sv.terp#1 $
// $Revision: #1 $
// $Date: 2012/10/10 $
// $Author: swbranch $

//Defined Terp Parameters


			    

`timescale 1 ns / 1 ns
  				      
module niosII_system_tristate_conduit_bridge_0 (
     input  logic clk
    ,input  logic reset
    ,input  logic request
    ,output logic grant
    ,input  logic[ 0 :0 ] tcs_generic_tristate_controller_0_tcm_read_n_out
    ,output  wire [ 0 :0 ] generic_tristate_controller_0_tcm_read_n_out
    ,output logic[ 7 :0 ] tcs_generic_tristate_controller_0_tcm_data_in
    ,input  logic[ 7 :0 ] tcs_generic_tristate_controller_0_tcm_data_out
    ,input  logic tcs_generic_tristate_controller_0_tcm_data_outen
    ,inout  wire [ 7 :0 ]  generic_tristate_controller_0_tcm_data_out
    ,input  logic[ 0 :0 ] tcs_generic_tristate_controller_0_tcm_chipselect_n_out
    ,output  wire [ 0 :0 ] generic_tristate_controller_0_tcm_chipselect_n_out
    ,input  logic[ 0 :0 ] tcs_generic_tristate_controller_0_tcm_write_n_out
    ,output  wire [ 0 :0 ] generic_tristate_controller_0_tcm_write_n_out
    ,input  logic[ 21 :0 ] tcs_generic_tristate_controller_0_tcm_address_out
    ,output  wire [ 21 :0 ] generic_tristate_controller_0_tcm_address_out
		     
   );
   reg grant_reg;
   assign grant = grant_reg;
   
   always@(posedge clk) begin
      if(reset)
	grant_reg <= 0;
      else
	grant_reg <= request;      
   end
   


 // ** Output Pin generic_tristate_controller_0_tcm_read_n_out 
 
    reg                       generic_tristate_controller_0_tcm_read_n_outen_reg;     
  
    always@(posedge clk) begin
	 if( reset ) begin
	   generic_tristate_controller_0_tcm_read_n_outen_reg <= 'b0;
	 end
	 else begin
	   generic_tristate_controller_0_tcm_read_n_outen_reg <= 'b1;
	 end
     end		     
   
 
    reg [ 0 : 0 ] generic_tristate_controller_0_tcm_read_n_out_reg;   

     always@(posedge clk) begin
	 generic_tristate_controller_0_tcm_read_n_out_reg   <= tcs_generic_tristate_controller_0_tcm_read_n_out[ 0 : 0 ];
      end
          
 
    assign 	generic_tristate_controller_0_tcm_read_n_out[ 0 : 0 ] = generic_tristate_controller_0_tcm_read_n_outen_reg ? generic_tristate_controller_0_tcm_read_n_out_reg : 'z ;
        


 // ** Bidirectional Pin generic_tristate_controller_0_tcm_data_out 
   
    reg                       generic_tristate_controller_0_tcm_data_outen_reg;
  
    always@(posedge clk) begin
	 generic_tristate_controller_0_tcm_data_outen_reg <= tcs_generic_tristate_controller_0_tcm_data_outen;
     end
  
  
    reg [ 7 : 0 ] generic_tristate_controller_0_tcm_data_out_reg;   

     always@(posedge clk) begin
	 generic_tristate_controller_0_tcm_data_out_reg   <= tcs_generic_tristate_controller_0_tcm_data_out[ 7 : 0 ];
      end
         
  
    assign 	generic_tristate_controller_0_tcm_data_out[ 7 : 0 ] = generic_tristate_controller_0_tcm_data_outen_reg ? generic_tristate_controller_0_tcm_data_out_reg : 'z ;
       
  
    reg [ 7 : 0 ] 	generic_tristate_controller_0_tcm_data_in_reg;
								    
    always@(posedge clk) begin
	 generic_tristate_controller_0_tcm_data_in_reg <= generic_tristate_controller_0_tcm_data_out[ 7 : 0 ];
    end
    
  
    assign      tcs_generic_tristate_controller_0_tcm_data_in[ 7 : 0 ] = generic_tristate_controller_0_tcm_data_in_reg[ 7 : 0 ];
        


 // ** Output Pin generic_tristate_controller_0_tcm_chipselect_n_out 
 
    reg                       generic_tristate_controller_0_tcm_chipselect_n_outen_reg;     
  
    always@(posedge clk) begin
	 if( reset ) begin
	   generic_tristate_controller_0_tcm_chipselect_n_outen_reg <= 'b0;
	 end
	 else begin
	   generic_tristate_controller_0_tcm_chipselect_n_outen_reg <= 'b1;
	 end
     end		     
   
 
    reg [ 0 : 0 ] generic_tristate_controller_0_tcm_chipselect_n_out_reg;   

     always@(posedge clk) begin
	 generic_tristate_controller_0_tcm_chipselect_n_out_reg   <= tcs_generic_tristate_controller_0_tcm_chipselect_n_out[ 0 : 0 ];
      end
          
 
    assign 	generic_tristate_controller_0_tcm_chipselect_n_out[ 0 : 0 ] = generic_tristate_controller_0_tcm_chipselect_n_outen_reg ? generic_tristate_controller_0_tcm_chipselect_n_out_reg : 'z ;
        


 // ** Output Pin generic_tristate_controller_0_tcm_write_n_out 
 
    reg                       generic_tristate_controller_0_tcm_write_n_outen_reg;     
  
    always@(posedge clk) begin
	 if( reset ) begin
	   generic_tristate_controller_0_tcm_write_n_outen_reg <= 'b0;
	 end
	 else begin
	   generic_tristate_controller_0_tcm_write_n_outen_reg <= 'b1;
	 end
     end		     
   
 
    reg [ 0 : 0 ] generic_tristate_controller_0_tcm_write_n_out_reg;   

     always@(posedge clk) begin
	 generic_tristate_controller_0_tcm_write_n_out_reg   <= tcs_generic_tristate_controller_0_tcm_write_n_out[ 0 : 0 ];
      end
          
 
    assign 	generic_tristate_controller_0_tcm_write_n_out[ 0 : 0 ] = generic_tristate_controller_0_tcm_write_n_outen_reg ? generic_tristate_controller_0_tcm_write_n_out_reg : 'z ;
        


 // ** Output Pin generic_tristate_controller_0_tcm_address_out 
 
    reg                       generic_tristate_controller_0_tcm_address_outen_reg;     
  
    always@(posedge clk) begin
	 if( reset ) begin
	   generic_tristate_controller_0_tcm_address_outen_reg <= 'b0;
	 end
	 else begin
	   generic_tristate_controller_0_tcm_address_outen_reg <= 'b1;
	 end
     end		     
   
 
    reg [ 21 : 0 ] generic_tristate_controller_0_tcm_address_out_reg;   

     always@(posedge clk) begin
	 generic_tristate_controller_0_tcm_address_out_reg   <= tcs_generic_tristate_controller_0_tcm_address_out[ 21 : 0 ];
      end
          
 
    assign 	generic_tristate_controller_0_tcm_address_out[ 21 : 0 ] = generic_tristate_controller_0_tcm_address_outen_reg ? generic_tristate_controller_0_tcm_address_out_reg : 'z ;
        

endmodule


