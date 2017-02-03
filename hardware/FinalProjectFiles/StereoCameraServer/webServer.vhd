library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity webServer is
        port
        (
					 -- SRAM ports
					 SRAM_ADDR	:	out		std_logic_vector(17 DOWNTO 0);
					 SRAM_DQ		:	inout		std_logic_vector(15 DOWNTO 0);
					 SRAM_WE_N	:	out		std_logic;	
					 SRAM_OE_N	:	out		std_logic;
					 SRAM_UB_N	:	out		std_logic;
					 SRAM_LB_N	:	out		std_logic;
					 SRAM_CE_N	:	out		std_logic;
						
					 -- the_dm9000a_inst
                ENET_CMD: OUT STD_LOGIC;
                ENET_CS_N: OUT STD_LOGIC;
                ENET_DATA: INOUT STD_LOGIC_VECTOR (15 DOWNTO 0);
                ENET_INT: IN STD_LOGIC;
                ENET_RD_N: OUT STD_LOGIC;
                ENET_RST_N: OUT STD_LOGIC;
                ENET_WR_N: OUT STD_LOGIC;
					 ENET_CLK : OUT STD_LOGIC;
			
						 -- LCD ports
                LCD_ON  :  out std_logic;
                LCD_BLON :      out std_logic;
                LCD_EN  :       out std_logic;
                LCD_RS  :       out std_logic;
                LCD_RW  :       out std_logic;
                LCD_DATA        :       inout   std_logic_vector (7 downto 0);
					 
                -- LEDS
                LEDG    : out std_logic_vector(7 downto 0);
                

                -- DRAM ports
                DRAM_CLK, DRAM_CKE : OUT STD_LOGIC;
                DRAM_ADDR : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
                DRAM_BA_1, DRAM_BA_0 : BUFFER STD_LOGIC;
                DRAM_CS_N, DRAM_CAS_N, DRAM_RAS_N, DRAM_WE_N : OUT STD_LOGIC;
                DRAM_DQ : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
                DRAM_UDQM, DRAM_LDQM : BUFFER STD_LOGIC ;
						
						
					 -- 7 Segment PIO
					 HEX0, HEX1 : OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
					 
                -- CLOCK port
                CLOCK_50        :       in  std_logic;
					 
					 -- Switch			
					 SW					:			in std_logic_vector(7 downto 0);
					 
					 
                -- RESET key
                KEY             :       in  std_logic_vector(0 downto 0);
					 
					 --GPIO_0
					 GPIO_0 : INOUT STD_LOGIC_VECTOR (35 DOWNTO 0);
					 
					 -- Flash
					 FL_ADDR : OUT STD_LOGIC_VECTOR (21 DOWNTO 0);
                FL_DQ : INOUT STD_LOGIC_VECTOR (7 DOWNTO 0);
                FL_OE_N : OUT STD_LOGIC;
                FL_CE_N : OUT STD_LOGIC;
                FL_WE_N : OUT STD_LOGIC;
					 FL_RST_N : OUT STD_LOGIC
                );
end webServer;


-- Library Clause(s) (optional)
-- Use Clause(s) (optional)

architecture structure of webServer is

-- A component declaration declares the interface of an entity or
-- a design unit written in another language.  VHDL requires that
-- you declare a component if you do not intend to instantiate
-- an entity directly.  The component need not declare all the
-- generics and ports in the entity.  It may omit generics/ports
-- with default values.

component niosII_system
			port (
              -- 1) global signals:
                 signal clk_0 : IN STD_LOGIC;
                 signal reset_n : IN STD_LOGIC;
					  signal sram_IF_0_tsb_data : INOUT STD_LOGIC_VECTOR (15 DOWNTO 0);

              -- the_altpll_inst
                 signal altpll_inst_c0_out : OUT STD_LOGIC;
                 signal altpll_inst_c1_out : OUT STD_LOGIC;
                 signal altpll_inst_c2_out : OUT STD_LOGIC;
                 signal locked_from_the_altpll_inst : OUT STD_LOGIC;
                 signal phasedone_from_the_altpll_inst : OUT STD_LOGIC;

              -- the_dm9000a_inst
                 signal ENET_CMD_from_the_dm9000a_inst : OUT STD_LOGIC;
                 signal ENET_CS_N_from_the_dm9000a_inst : OUT STD_LOGIC;
                 signal ENET_DATA_to_and_from_the_dm9000a_inst : INOUT STD_LOGIC_VECTOR (15 DOWNTO 0);
                 signal ENET_INT_to_the_dm9000a_inst : IN STD_LOGIC;
                 signal ENET_RD_N_from_the_dm9000a_inst : OUT STD_LOGIC;
                 signal ENET_RST_N_from_the_dm9000a_inst : OUT STD_LOGIC;
                 signal ENET_WR_N_from_the_dm9000a_inst : OUT STD_LOGIC;

              -- the_lcd_display
                 signal LCD_E_from_the_lcd_display : OUT STD_LOGIC;
                 signal LCD_RS_from_the_lcd_display : OUT STD_LOGIC;
                 signal LCD_RW_from_the_lcd_display : OUT STD_LOGIC;
                 signal LCD_data_to_and_from_the_lcd_display : INOUT STD_LOGIC_VECTOR (7 DOWNTO 0);

              -- the_led_pio
                 signal out_port_from_the_led_pio : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);

              -- the_sdram
                 signal zs_addr_from_the_sdram : OUT STD_LOGIC_VECTOR (11 DOWNTO 0);
                 signal zs_ba_from_the_sdram : OUT STD_LOGIC_VECTOR (1 DOWNTO 0);
                 signal zs_cas_n_from_the_sdram : OUT STD_LOGIC;
                 signal zs_cke_from_the_sdram : OUT STD_LOGIC;
                 signal zs_cs_n_from_the_sdram : OUT STD_LOGIC;
                 signal zs_dq_to_and_from_the_sdram : INOUT STD_LOGIC_VECTOR (15 DOWNTO 0);
                 signal zs_dqm_from_the_sdram : OUT STD_LOGIC_VECTOR (1 DOWNTO 0);
                 signal zs_ras_n_from_the_sdram : OUT STD_LOGIC;
                 signal zs_we_n_from_the_sdram : OUT STD_LOGIC;

              -- the_seven_seg_pio
                 signal out_port_from_the_seven_seg_pio : OUT STD_LOGIC_VECTOR (15 DOWNTO 0);

              -- the_switch
                 signal in_port_to_the_switch : IN STD_LOGIC_VECTOR (7 DOWNTO 0);

              -- the_tri_state_bridge_0_avalon_slave
                 signal address_to_the_ext_flash : OUT STD_LOGIC_VECTOR (21 DOWNTO 0);
                 signal data_to_and_from_the_ext_flash : INOUT STD_LOGIC_VECTOR (7 DOWNTO 0);
                 signal read_n_to_the_ext_flash : OUT STD_LOGIC;
                 signal select_n_to_the_ext_flash : OUT STD_LOGIC;
                 signal write_n_to_the_ext_flash : OUT STD_LOGIC;
					  
				  -- UART Signals
                signal txd_from_the_uart_0 :  OUT STD_LOGIC;
                signal txd_from_the_uart_1 :  OUT STD_LOGIC;
                signal rxd_to_the_uart_0 :  IN STD_LOGIC;
                signal rxd_to_the_uart_1 :  IN STD_LOGIC;
					 
				  -- the_sram_IF_0
					 signal coe_sram_address_from_the_sram_IF_0 : OUT STD_LOGIC_VECTOR (17 DOWNTO 0);
					 signal coe_sram_chipenable_n_from_the_sram_IF_0 : OUT STD_LOGIC;
					 signal coe_sram_lowerbyte_n_from_the_sram_IF_0 : OUT STD_LOGIC;
					 signal coe_sram_outputenable_n_from_the_sram_IF_0 : OUT STD_LOGIC;
					 signal coe_sram_upperbyte_n_from_the_sram_IF_0 : OUT STD_LOGIC;
					 signal coe_sram_writeenable_n_from_the_sram_IF_0 : OUT STD_LOGIC
              );
end component;

        signal BA : STD_LOGIC_VECTOR (1 downto 0);
        signal DQM : STD_LOGIC_VECTOR (1 downto 0);

        signal pll_c1:STD_LOGIC;
        signal pll_locked:STD_LOGIC;
        signal pll_phase: STD_LOGIC;
		  --signal pll_c1_n :STD_LOGIC;
		  signal seven_seg_16 : STD_LOGIC_VECTOR(15 downto 0);
		 -- signal green_leds: STD_LOGIC_VECTOR(7 downto 0);

begin
			-- I have no idea why the SOPC-created IP does not turn the LCD on at all.
        -- I've explicitly turned it on with the next loc.      Note that LCD_ON is the
        -- VCC power line not the programmaticly controlled version that is bit banged.
        LCD_ON <= '1';


        DRAM_BA_1 <= BA(1);
        DRAM_BA_0 <= BA(0);
        DRAM_UDQM <= DQM(1);
        DRAM_LDQM <= DQM(0);
			--pll_c1_n <= ~pll_c1;
		--	DRAM_CLK <= pll_c0;
		--	ENET_CLK <= pll_co_n;
		-- Set the pair of seven segment displays as a 16 bit
		-- vector where the 8th and 16th bit are ignored
		-- (this is the convention that the altera examples use, 
		-- usually the 8th and 16th bits would be decimal points)
		HEX0 <= seven_seg_16(6 downto 0);
		HEX1 <= seven_seg_16(14 downto 8);
		--green_leds <= LEDG(7 downto 0);
		-- Hard wire the flash to always be on (not reset)
		FL_RST_N <= '1';
        -- Component Instantiation Statement (optional)
        NIOSII : niosII_system port map ( 
					--ENET
					ENET_CMD_from_the_dm9000a_inst => ENET_CMD,
               ENET_CS_N_from_the_dm9000a_inst => ENET_CS_N ,
               ENET_DATA_to_and_from_the_dm9000a_inst => ENET_DATA,
               ENET_INT_to_the_dm9000a_inst => ENET_INT,
               ENET_RD_N_from_the_dm9000a_inst => ENET_RD_N,
               ENET_RST_N_from_the_dm9000a_inst => ENET_RST_N,
               ENET_WR_N_from_the_dm9000a_inst => ENET_WR_N,
					
					--PLL
					altpll_inst_c0_out => DRAM_CLK,
					altpll_inst_c1_out => pll_c1,
					altpll_inst_c2_out => ENET_CLK,
					locked_from_the_altpll_inst => pll_locked,
					phasedone_from_the_altpll_inst => pll_phase,
			
					-- LCD
               LCD_E_from_the_lcd_display => LCD_EN,
               LCD_RS_from_the_lcd_display => LCD_RS,
               LCD_RW_from_the_lcd_display => LCD_RW,
               LCD_data_to_and_from_the_lcd_display => LCD_DATA,
					
               -- LEDS
               out_port_from_the_led_pio => LEDG,
					
					-- 7SEG
               out_port_from_the_seven_seg_pio => seven_seg_16,
					
               -- DRAM ports
					zs_addr_from_the_sdram => DRAM_ADDR,
					zs_cke_from_the_sdram => DRAM_CKE,
					zs_cs_n_from_the_sdram => DRAM_CS_N,
					zs_dq_to_and_from_the_sdram => DRAM_DQ,
					zs_cas_n_from_the_sdram => DRAM_CAS_N,
					zs_ras_n_from_the_sdram => DRAM_RAS_N,
					zs_we_n_from_the_sdram => DRAM_WE_N,
					zs_ba_from_the_sdram => BA,
					zs_dqm_from_the_sdram => DQM,
				 
					-- CLOCK port
					clk_0 => CLOCK_50,
					 
					-- Switch			
					in_port_to_the_switch => SW(7 downto 0),
					 
					 
               -- RESET key
               reset_n => KEY(0),
					 
					-- Flash
					address_to_the_ext_flash => FL_ADDR,
               data_to_and_from_the_ext_flash => FL_DQ,
               read_n_to_the_ext_flash => FL_OE_N,
               select_n_to_the_ext_flash => FL_CE_N,
               write_n_to_the_ext_flash => FL_WE_N,
					
					-- Uart
					rxd_to_the_uart_0 => GPIO_0(1),
					txd_from_the_uart_0 => GPIO_0(3),
					rxd_to_the_uart_1 => GPIO_0(5),
					txd_from_the_uart_1 => GPIO_0(7),
					
					-- SRAM
					coe_sram_address_from_the_sram_IF_0 => SRAM_ADDR,
					sram_IF_0_tsb_data => SRAM_DQ,
					coe_sram_writeenable_n_from_the_sram_IF_0 => SRAM_WE_N,
					coe_sram_upperbyte_n_from_the_sram_IF_0 => SRAM_UB_N,
					coe_sram_lowerbyte_n_from_the_sram_IF_0 => SRAM_LB_N,
					coe_sram_chipenable_n_from_the_sram_IF_0 => SRAM_CE_N,
					coe_sram_outputenable_n_from_the_sram_IF_0 => SRAM_OE_N
					);
end structure;
		  
