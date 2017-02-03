-- dm9000a_inst.vhd

-- This file was auto-generated as part of a SOPC Builder generate operation.
-- If you edit it your changes will probably be lost.

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity dm9000a_inst is
	port (
		iRST_N     : in    std_logic                     := '0';             --      clock_reset.reset_n
		ENET_DATA  : inout std_logic_vector(15 downto 0) := (others => '0'); --         export_0.export
		ENET_CMD   : out   std_logic;                                        --                 .export
		ENET_RD_N  : out   std_logic;                                        --                 .export
		ENET_WR_N  : out   std_logic;                                        --                 .export
		ENET_CS_N  : out   std_logic;                                        --                 .export
		ENET_RST_N : out   std_logic;                                        --                 .export
		ENET_INT   : in    std_logic                     := '0';             --                 .export
		iCMD       : in    std_logic                     := '0';             --   avalon_slave_0.address
		iRD_N      : in    std_logic                     := '0';             --                 .read_n
		iWR_N      : in    std_logic                     := '0';             --                 .write_n
		iDATA      : in    std_logic_vector(15 downto 0) := (others => '0'); --                 .writedata
		oDATA      : out   std_logic_vector(15 downto 0);                    --                 .readdata
		iCS_N      : in    std_logic                     := '0';             --                 .chipselect_n
		oINT       : out   std_logic                                         -- interrupt_sender.irq
	);
end entity dm9000a_inst;

architecture rtl of dm9000a_inst is
	component dm9000a is
		port (
			iRST_N     : in    std_logic                     := 'X';             -- reset_n
			ENET_DATA  : inout std_logic_vector(15 downto 0) := (others => 'X'); -- export
			ENET_CMD   : out   std_logic;                                        -- export
			ENET_RD_N  : out   std_logic;                                        -- export
			ENET_WR_N  : out   std_logic;                                        -- export
			ENET_CS_N  : out   std_logic;                                        -- export
			ENET_RST_N : out   std_logic;                                        -- export
			ENET_INT   : in    std_logic                     := 'X';             -- export
			iCMD       : in    std_logic                     := 'X';             -- address
			iRD_N      : in    std_logic                     := 'X';             -- read_n
			iWR_N      : in    std_logic                     := 'X';             -- write_n
			iDATA      : in    std_logic_vector(15 downto 0) := (others => 'X'); -- writedata
			oDATA      : out   std_logic_vector(15 downto 0);                    -- readdata
			iCS_N      : in    std_logic                     := 'X';             -- chipselect_n
			oINT       : out   std_logic                                         -- irq
		);
	end component dm9000a;

begin

	dm9000a_inst : component dm9000a
		port map (
			iRST_N     => iRST_N,     --      clock_reset.reset_n
			ENET_DATA  => ENET_DATA,  --         export_0.export
			ENET_CMD   => ENET_CMD,   --                 .export
			ENET_RD_N  => ENET_RD_N,  --                 .export
			ENET_WR_N  => ENET_WR_N,  --                 .export
			ENET_CS_N  => ENET_CS_N,  --                 .export
			ENET_RST_N => ENET_RST_N, --                 .export
			ENET_INT   => ENET_INT,   --                 .export
			iCMD       => iCMD,       --   avalon_slave_0.address
			iRD_N      => iRD_N,      --                 .read_n
			iWR_N      => iWR_N,      --                 .write_n
			iDATA      => iDATA,      --                 .writedata
			oDATA      => oDATA,      --                 .readdata
			iCS_N      => iCS_N,      --                 .chipselect_n
			oINT       => oINT        -- interrupt_sender.irq
		);

end architecture rtl; -- of dm9000a_inst
