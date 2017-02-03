library ieee;
use ieee.std_logic_1164.all;


entity dm9000a is 
  port(
					signal iCMD,iRD_N,iWR_N,
					       iCS_N,iRST_N: in std_logic;

          signal iDATA: in std_logic_vector(15 downto 0);
          signal oDATA: out std_logic_vector(15 downto 0);

					signal oINT: out std_logic;

          --  DM9000A Side
          signal ENET_DATA: inout std_logic_vector(15 downto 0);

          signal ENET_CMD,
					       ENET_RD_N,ENET_WR_N,
					       ENET_CS_N,ENET_RST_N: out std_logic;

          signal ENET_INT: in std_logic
					       
					);
end dm9000a;

architecture behavior of dm9000a is
begin

	ENET_DATA  <=	iDATA when iWR_N='0' else	(others => 'Z');
	oDATA      <=	ENET_DATA	;

	ENET_CMD   <=	iCMD;
	ENET_RD_N  <=	iRD_N;
	ENET_WR_N	 <=	iWR_N;
	ENET_CS_N	 <=	iCS_N;
	ENET_RST_N <=	iRST_N;
	oINT       <=	ENET_INT;

end behavior;

