module DM9000A_IF(	//	HOST Side
					// Signals fro Avalon-MM slave port "s1"
					avs_s1_writedata_iDATA,
					avs_s1_readdata_oDATA,
					avs_s1_address_iCMD,
					avs_s1_read_n_iRD_N,
					avs_s1_write_n_iWR_N,
					avs_s1_chipselect_n_iCS_N,
					avs_s1_reset_n_iRST_N,
					avs_s1_clk_iCLK,
					avs_s1_irq_oINT,
					
					//	export signal to DM9000A Chip
					avs_s1_export_ENET_DATA,
					avs_s1_export_ENET_CMD,
					avs_s1_export_ENET_RD_N,
					avs_s1_export_ENET_WR_N,
					avs_s1_export_ENET_CS_N,
					avs_s1_export_ENET_RST_N,
					avs_s1_export_ENET_INT,
					avs_s1_export_ENET_CLK
					);
//	HOST Side
input	[15:0]	avs_s1_writedata_iDATA;
input			avs_s1_address_iCMD;
input			avs_s1_read_n_iRD_N;
input			avs_s1_write_n_iWR_N;
input			avs_s1_chipselect_n_iCS_N;
input			avs_s1_reset_n_iRST_N;
input			avs_s1_clk_iCLK;
output	[15:0]	avs_s1_readdata_oDATA;
output			avs_s1_irq_oINT;
//	DM9000A Side
inout	[15:0]	avs_s1_export_ENET_DATA;
output			avs_s1_export_ENET_CMD;
output			avs_s1_export_ENET_RD_N;
output			avs_s1_export_ENET_WR_N;
output			avs_s1_export_ENET_CS_N;
output			avs_s1_export_ENET_RST_N;
output			avs_s1_export_ENET_CLK;
input			avs_s1_export_ENET_INT;


assign	avs_s1_export_ENET_DATA 	=	avs_s1_write_n_iWR_N	? 16'hzzzz : avs_s1_writedata_iDATA;
assign	avs_s1_readdata_oDATA		=	avs_s1_read_n_iRD_N ? 16'hzzzz : avs_s1_export_ENET_DATA;

assign	avs_s1_export_ENET_RST_N	=	avs_s1_reset_n_iRST_N;
assign	avs_s1_irq_oINT		        =	avs_s1_export_ENET_INT;
assign	avs_s1_export_ENET_CMD 	    =	avs_s1_address_iCMD;
assign	avs_s1_export_ENET_CS_N 	=	avs_s1_chipselect_n_iCS_N;
assign	avs_s1_export_ENET_RD_N 	=	avs_s1_read_n_iRD_N;
assign	avs_s1_export_ENET_WR_N 	=	avs_s1_write_n_iWR_N;
assign	avs_s1_export_ENET_CLK 		=	avs_s1_clk_iCLK;


endmodule



