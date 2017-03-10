-- Nancy Minderman
-- nancy.minderman@ualberta.ca
-- This file makes extensive use of Altera template structures.
-- This file is the top-level file for lab 1 winter 2014 for version 12.1sp1 on Windows 7


-- A library clause declares a name as a library.  It 
-- does not create the library; it simply forward declares 
-- it. 
library ieee;

-- Commonly imported packages:

	-- STD_LOGIC and STD_LOGIC_VECTOR types, and relevant functions
	use ieee.std_logic_1164.all;

	-- SIGNED and UNSIGNED types, and relevant functions
	use ieee.numeric_std.all;

	-- Basic sequential functions and concurrent procedures
	use ieee.VITAL_Primitives.all;
	
	use work.DE2_CONSTANTS.all;
	
	entity niosII_microc_capstone is
	
	port
	(
		-- Input ports and 50 MHz Clock
		KEY		: in  std_logic_vector (0 downto 0);
		SW			: in 	std_logic_vector (0 downto 0);
		CLOCK_50	: in  std_logic;
		
		-- LCD on board
		LCD_BLON	: out std_logic;
		LCD_ON	: out std_logic;
		LCD_DATA	: inout DE2_LCD_DATA_BUS;
		LCD_RS	: out std_logic;
		LCD_EN	: out std_logic;
		LCD_RW	: out std_logic;
		
		-- SDRAM on board
		--DRAM_ADDR	:	out 	std_logic_vector (11 downto 0);
		DRAM_ADDR	:	out	DE2_SDRAM_ADDR_BUS;
		DRAM_BA_0	: 	out	std_logic;
		DRAM_BA_1	:	out	std_logic;
		DRAM_CAS_N	:	out	std_logic;
		DRAM_CKE		:	out	std_logic;
		DRAM_CLK		:	out	std_logic;
		DRAM_CS_N	:	out	std_logic;
		--DRAM_DQ		:	inout std_logic_vector (15 downto 0);
		DRAM_DQ		:	inout 	DE2_SDRAM_DATA_BUS;
		DRAM_LDQM	: 	out	std_logic;
		DRAM_UDQM	: 	out	std_logic;
		DRAM_RAS_N	: 	out	std_logic;
		DRAM_WE_N	: 	out 	std_logic;

		-- SRAM on board
		
		SRAM_ADDR	:	out	DE2_SRAM_ADDR_BUS;
		SRAM_DQ		:	inout DE2_SRAM_DATA_BUS;
		SRAM_WE_N	:	out	std_logic;
		SRAM_OE_N	:	out	std_logic;
		SRAM_UB_N	:	out 	std_logic;
		SRAM_LB_N	:	out	std_logic;
		SRAM_CE_N	:	out	std_logic;
		
		--Ethernet
		ENET_CLK		:	out 	std_logic;
		ENET_CMD		:	out 	std_logic;
		ENET_CS_N	:	out 	std_logic;
		ENET_INT		:	in 	std_logic;
		ENET_RD_N	:	out 	std_logic;
		ENET_WR_N	:	out 	std_logic;
		ENET_RST_N	:	out	std_logic;
		ENET_DATA	:	inout	std_logic_vector(15 downto 0);
		
		--VGA
		VGA_B			:	out	std_LOGIC_VECTOR(9 downto 0);
		VGA_G			:	out	std_LOGIC_VECTOR(9 downto 0);
		VGA_R			:	out	std_LOGIC_VECTOR(9 downto 0);
		VGA_CLK		:	out	std_LOGIC;
		VGA_HS		:	out	std_LOGIC;
		VGA_VS		:	out	std_LOGIC;
		VGA_BLANK	:	out	std_LOGIC;
		VGA_SYNC		:	out	std_LOGIC;
		
		-- FLASH
		
		FL_ADDR     :  out   std_logic_vector (21 downto 0);
		FL_CE_N     :  out   std_logic_vector (0 downto 0);
		FL_OE_N     :  out   std_logic_vector (0 downto 0);
		FL_DQ     :  inout   std_logic_vector (7 downto 0);
		FL_RST_N    :  out   std_logic_vector (0 downto 0);
		FL_WE_N     :  out   std_logic_vector (0 downto 0);
		
		--SD Card
		SD_CMD		:	inout	std_LOGIC;
		SD_DAT		:	inout	std_LOGIC;
		SD_DAT3		:	inout	std_LOGIC;
		SD_CLK		:	out	std_LOGIC
	);
end niosII_microc_capstone;


architecture structure of niosII_microc_capstone is

	-- Declarations (optional)
	
	 component niosII_system is
        port (
            clk_clk                                 : in    std_logic                     := 'X';             -- clk
            reset_reset_n                           : in    std_logic                     := 'X';             -- reset_n
				sdram_0_wire_addr                       : out   DE2_SDRAM_ADDR_BUS;                    			  -- addr
            sdram_0_wire_ba                         : out   std_logic_vector(1 downto 0);                     -- ba
            sdram_0_wire_cas_n                      : out   std_logic;                                        -- cas_n
            sdram_0_wire_cke                        : out   std_logic;                                        -- cke
            sdram_0_wire_cs_n                       : out   std_logic;                                        -- cs_n
            sdram_0_wire_dq                         : inout DE2_SDRAM_DATA_BUS := (others => 'X'); 			  -- dq
            sdram_0_wire_dqm                        : out   std_logic_vector(1 downto 0);                     -- dqm
            sdram_0_wire_ras_n                      : out   std_logic;                                        -- ras_n
            sdram_0_wire_we_n                       : out   std_logic;                                        -- we_n
				up_clocks_0_sdram_clk_clk               : out   std_logic;                                        -- clk
            sram_0_external_interface_DQ            : inout DE2_SRAM_DATA_BUS := (others => 'X'); 				  -- DQ
            sram_0_external_interface_ADDR          : out   DE2_SRAM_ADDR_BUS;                    				  -- ADDR
            sram_0_external_interface_LB_N          : out   std_logic;                                        -- LB_N
            sram_0_external_interface_UB_N          : out   std_logic;                                        -- UB_N
            sram_0_external_interface_CE_N          : out   std_logic;                                        -- CE_N
            sram_0_external_interface_OE_N          : out   std_logic;                                        -- OE_N
            sram_0_external_interface_WE_N          : out   std_logic;                                        -- WE_N
            character_lcd_0_external_interface_DATA : inout DE2_LCD_DATA_BUS  := (others => 'X'); 				  -- DATA
            character_lcd_0_external_interface_ON   : out   std_logic;                                        -- ON
            character_lcd_0_external_interface_BLON : out   std_logic;                                        -- BLON
            character_lcd_0_external_interface_EN   : out   std_logic;                                        -- EN
            character_lcd_0_external_interface_RS   : out   std_logic;                                        -- RS
            character_lcd_0_external_interface_RW   : out   std_logic;                                        -- RW
				dm9000a_if_0_s1_export_DATA             : inout std_logic_vector(15 downto 0) := (others => 'X'); -- DATA
            dm9000a_if_0_s1_export_CMD              : out   std_logic;                                        -- CMD
            dm9000a_if_0_s1_export_RD_N             : out   std_logic;                                        -- RD_N
            dm9000a_if_0_s1_export_WR_N             : out   std_logic;                                        -- WR_N
            dm9000a_if_0_s1_export_CS_N             : out   std_logic;                                        -- CS_N
            dm9000a_if_0_s1_export_RST_N            : out   std_logic;                                        -- RST_N
            dm9000a_if_0_s1_export_INT              : in    std_logic                     := 'X';             -- INT
            dm9000a_if_0_s1_export_CLK              : out   std_logic;                                        -- CLK
				video_vga_controller_0_external_interface_CLK               : out   std_logic;                                        -- CLK
            video_vga_controller_0_external_interface_HS                : out   std_logic;                                        -- HS
            video_vga_controller_0_external_interface_VS                : out   std_logic;                                        -- VS
            video_vga_controller_0_external_interface_BLANK             : out   std_logic;                                        -- BLANK
            video_vga_controller_0_external_interface_SYNC              : out   std_logic;                                        -- SYNC
            video_vga_controller_0_external_interface_R                 : out   std_logic_vector(9 downto 0);                     -- R
            video_vga_controller_0_external_interface_G                 : out   std_logic_vector(9 downto 0);                     -- G
            video_vga_controller_0_external_interface_B                 : out   std_logic_vector(9 downto 0);                     -- B
            altera_up_sd_card_avalon_interface_0_conduit_end_b_SD_cmd   : inout std_logic                     := 'X';             -- b_SD_cmd
            altera_up_sd_card_avalon_interface_0_conduit_end_b_SD_dat   : inout std_logic                     := 'X';             -- b_SD_dat
            altera_up_sd_card_avalon_interface_0_conduit_end_b_SD_dat3  : inout std_logic                     := 'X';             -- b_SD_dat3
            altera_up_sd_card_avalon_interface_0_conduit_end_o_SD_clock : out   std_logic;                                         -- o_SD_clock
				tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_address_out : out std_LOGIC_VECTOR(21 downto 0);
				tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_data_out : inout std_LOGIC_VECTOR(7 downto 0);
				tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_write_n_out : out std_LOGIC;
				tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_chipselect_n_out : out std_LOGIC;
				tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_read_n_out : out std_LOGIC		
        );
    end component niosII_system;

--	These signals are for matching the provided IP core to
-- The specific SDRAM chip in our system	 
	 signal BA	: std_logic_vector (1 downto 0);
	 signal DQM	:	std_logic_vector (1 downto 0);
	 signal CLOCK_25 : std_logic;
	 

begin
	DRAM_BA_1 <= BA(1);
	DRAM_BA_0 <= BA(0);
	
	DRAM_UDQM <= DQM(1);
	DRAM_LDQM <= DQM(0);
	FL_RST_N(0)  <= '1';

	--need to create proccess that will generate a 25 mhz clock for ethernet
	process(CLOCK_50)
	begin
	  if(rising_edge(CLOCK_50)) then
		 CLOCK_25   <= not CLOCK_25;
	  end if;
	end process;

	ENET_CLK <= CLOCK_25;
	
	-- Component Instantiation Statement (optional)
	
	  u0 : component niosII_system
        port map (
            clk_clk                                 => CLOCK_50,                                
            reset_reset_n                           => KEY(0),
				sdram_0_wire_addr                       => DRAM_ADDR,                      
            sdram_0_wire_ba                         => BA,                        
            sdram_0_wire_cas_n                      => DRAM_CAS_N,                      
            sdram_0_wire_cke                        => DRAM_CKE,                       
            sdram_0_wire_cs_n                       => DRAM_CS_N,                      
            sdram_0_wire_dq                         => DRAM_DQ,                         
            sdram_0_wire_dqm                        => DQM,                        
            sdram_0_wire_ras_n                      => DRAM_RAS_N,                     
            sdram_0_wire_we_n                       => DRAM_WE_N,                       
            up_clocks_0_sdram_clk_clk               => DRAM_CLK,
            sram_0_external_interface_DQ            => SRAM_DQ,           
            sram_0_external_interface_ADDR          => SRAM_ADDR,          
            sram_0_external_interface_LB_N          => SRAM_LB_N,         
            sram_0_external_interface_UB_N          => SRAM_UB_N,          
            sram_0_external_interface_CE_N          => SRAM_CE_N,         
            sram_0_external_interface_OE_N          => SRAM_OE_N,         
            sram_0_external_interface_WE_N          => SRAM_WE_N,          
            character_lcd_0_external_interface_DATA => LCD_DATA, 
            character_lcd_0_external_interface_ON   => LCD_ON,   
            character_lcd_0_external_interface_BLON => LCD_BLON, 
            character_lcd_0_external_interface_EN   => LCD_EN,   
            character_lcd_0_external_interface_RS   => LCD_RS,   
            character_lcd_0_external_interface_RW   => LCD_RW,						
				dm9000a_if_0_s1_export_DATA             => ENET_DATA,             						  --             					 dm9000a_if_0_s1_export.DATA
            dm9000a_if_0_s1_export_CMD              => ENET_CMD,              						  --                                   				  .CMD
            dm9000a_if_0_s1_export_RD_N             => ENET_RD_N,             						  --                                   				  .RD_N
            dm9000a_if_0_s1_export_WR_N             => ENET_WR_N,             					     --                                   				  .WR_N
            dm9000a_if_0_s1_export_CS_N             => ENET_CS_N,             						  --                                   				  .CS_N
            dm9000a_if_0_s1_export_RST_N            => ENET_RST_N,            						  --                                   				  .RST_N
            dm9000a_if_0_s1_export_INT              => ENET_INT,               						  --                                   				  .INT      
				video_vga_controller_0_external_interface_CLK               => VGA_CLK,               --        video_vga_controller_0_external_interface.CLK
            video_vga_controller_0_external_interface_HS                => VGA_HS,                --                                                 .HS
            video_vga_controller_0_external_interface_VS                => VGA_VS,                --                                                 .VS
            video_vga_controller_0_external_interface_BLANK             => VGA_BLANK,             --                                                 .BLANK
            video_vga_controller_0_external_interface_SYNC              => VGA_SYNC,              --                                                 .SYNC
            video_vga_controller_0_external_interface_R                 => VGA_R,                 --                                                 .R
            video_vga_controller_0_external_interface_G                 => VGA_G,                 --                                                 .G
            video_vga_controller_0_external_interface_B                 => VGA_B,                 --                                                 .B
            altera_up_sd_card_avalon_interface_0_conduit_end_b_SD_cmd   => SD_CMD,   				  -- altera_up_sd_card_avalon_interface_0_conduit_end.b_SD_cmd
            altera_up_sd_card_avalon_interface_0_conduit_end_b_SD_dat   => SD_DAT,   				  --                                                 .b_SD_dat
            altera_up_sd_card_avalon_interface_0_conduit_end_b_SD_dat3  => SD_DAT3,  				  --                                                 .b_SD_dat3
            altera_up_sd_card_avalon_interface_0_conduit_end_o_SD_clock => SD_CLK, 					  --                                                 .o_SD_clock
				tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_address_out => FL_ADDR,
				tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_data_out => FL_DQ,
				tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_write_n_out => FL_WE_N(0),
				tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_chipselect_n_out => FL_CE_N(0),
				tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_read_n_out => FL_OE_N(0)
		  );

end structure;


library ieee;

-- Commonly imported packages:

	-- STD_LOGIC and STD_LOGIC_VECTOR types, and relevant functions
	use ieee.std_logic_1164.all;

package DE2_CONSTANTS is
	
	type DE2_SDRAM_ADDR_BUS is array(11 downto 0) of std_logic;
	type DE2_SDRAM_DATA_BUS is array(15 downto 0) of std_logic;
	
	type DE2_LCD_DATA_BUS	is array(7 downto 0) of std_logic;
	
	type DE2_LED_GREEN		is array(7 downto 0)  of std_logic;
	
	type DE2_SRAM_ADDR_BUS	is array(17 downto 0) of std_logic;
	type DE2_SRAM_DATA_BUS  is array(15 downto 0) of std_logic;
	
end DE2_CONSTANTS;



