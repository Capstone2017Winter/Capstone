-- sram_IF_0.vhd

-- This file was auto-generated as part of a SOPC Builder generate operation.
-- If you edit it your changes will probably be lost.

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity sram_IF_0 is
	port (
		ats_tsb_address         : in    std_logic_vector(17 downto 0) := (others => '0'); --           tsb.address
		ats_tsb_data            : inout std_logic_vector(15 downto 0) := (others => '0'); --              .data
		ats_tsb_write_n         : in    std_logic                     := '0';             --              .write_n
		ats_tsb_byteenable_n    : in    std_logic_vector(1 downto 0)  := (others => '0'); --              .byteenable_n
		ats_tsb_chipselect_n    : in    std_logic                     := '0';             --              .chipselect_n
		ats_tsb_outputenable_n  : in    std_logic                     := '0';             --              .outputenable_n
		coe_sram_address        : out   std_logic_vector(17 downto 0);                    -- conduit_end_0.export
		coe_sram_writeenable_n  : out   std_logic;                                        --              .export
		coe_sram_upperbyte_n    : out   std_logic;                                        --              .export
		coe_sram_lowerbyte_n    : out   std_logic;                                        --              .export
		coe_sram_chipenable_n   : out   std_logic;                                        --              .export
		coe_sram_outputenable_n : out   std_logic                                         --              .export
	);
end entity sram_IF_0;

architecture rtl of sram_IF_0 is
	component sram_IF is
		port (
			ats_tsb_address         : in    std_logic_vector(17 downto 0) := (others => 'X'); -- address
			ats_tsb_data            : inout std_logic_vector(15 downto 0) := (others => 'X'); -- data
			ats_tsb_write_n         : in    std_logic                     := 'X';             -- write_n
			ats_tsb_byteenable_n    : in    std_logic_vector(1 downto 0)  := (others => 'X'); -- byteenable_n
			ats_tsb_chipselect_n    : in    std_logic                     := 'X';             -- chipselect_n
			ats_tsb_outputenable_n  : in    std_logic                     := 'X';             -- outputenable_n
			coe_sram_address        : out   std_logic_vector(17 downto 0);                    -- export
			coe_sram_writeenable_n  : out   std_logic;                                        -- export
			coe_sram_upperbyte_n    : out   std_logic;                                        -- export
			coe_sram_lowerbyte_n    : out   std_logic;                                        -- export
			coe_sram_chipenable_n   : out   std_logic;                                        -- export
			coe_sram_outputenable_n : out   std_logic                                         -- export
		);
	end component sram_IF;

begin

	sram_if_0 : component sram_IF
		port map (
			ats_tsb_address         => ats_tsb_address,         --           tsb.address
			ats_tsb_data            => ats_tsb_data,            --              .data
			ats_tsb_write_n         => ats_tsb_write_n,         --              .write_n
			ats_tsb_byteenable_n    => ats_tsb_byteenable_n,    --              .byteenable_n
			ats_tsb_chipselect_n    => ats_tsb_chipselect_n,    --              .chipselect_n
			ats_tsb_outputenable_n  => ats_tsb_outputenable_n,  --              .outputenable_n
			coe_sram_address        => coe_sram_address,        -- conduit_end_0.export
			coe_sram_writeenable_n  => coe_sram_writeenable_n,  --              .export
			coe_sram_upperbyte_n    => coe_sram_upperbyte_n,    --              .export
			coe_sram_lowerbyte_n    => coe_sram_lowerbyte_n,    --              .export
			coe_sram_chipenable_n   => coe_sram_chipenable_n,   --              .export
			coe_sram_outputenable_n => coe_sram_outputenable_n  --              .export
		);

end architecture rtl; -- of sram_IF_0
