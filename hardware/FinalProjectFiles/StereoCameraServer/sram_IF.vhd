-- Leo Nickerson, Timo Hohn
-- Avalon-MM-tristate bridge to sram interface module
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity sram_IF is
	port(
	ats_tsb_address		:	in	std_logic_vector(18 downto 1);
	-- should be: 	ats_tsb_address		:	in	std_logic_vector(18 downto 1);
	-- because Avalon-MM-tristate has address range 32 downto 1
	ats_tsb_data		:	inout	std_logic_vector(15 downto 0);
	ats_tsb_write_n		:	in	std_logic;
	ats_tsb_byteenable_n	:	in	std_logic_vector(1 downto 0);
	ats_tsb_chipselect_n	:	in	std_logic;
	ats_tsb_outputenable_n	:	in	std_logic;

	coe_sram_address	:	out	std_logic_vector(17 downto 0);
	coe_sram_writeenable_n	:	out	std_logic;
	coe_sram_upperbyte_n	:	out	std_logic;
	coe_sram_lowerbyte_n	:	out	std_logic;
	coe_sram_chipenable_n	:	out	std_logic;
	coe_sram_outputenable_n	:	out	std_logic
	);
end sram_IF;

architecture structure of sram_IF is
begin
	coe_sram_address	<= ats_tsb_address;
	-- should map 18 downto 1 of tsb to 17 downto 0 of sram
	coe_sram_writeenable_n	<= ats_tsb_write_n;
	coe_sram_upperbyte_n	<= ats_tsb_byteenable_n(1);
	coe_sram_lowerbyte_n	<= ats_tsb_byteenable_n(0);
	coe_sram_chipenable_n	<= ats_tsb_chipselect_n;
	coe_sram_outputenable_n	<= ats_tsb_outputenable_n;

end architecture;

