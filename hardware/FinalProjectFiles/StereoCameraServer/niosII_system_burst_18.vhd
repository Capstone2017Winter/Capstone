--Legal Notice: (C)2013 Altera Corporation. All rights reserved.  Your
--use of Altera Corporation's design tools, logic functions and other
--software and tools, and its AMPP partner logic functions, and any
--output files any of the foregoing (including device programming or
--simulation files), and any associated documentation or information are
--expressly subject to the terms and conditions of the Altera Program
--License Subscription Agreement or other applicable license agreement,
--including, without limitation, that your use is for the sole purpose
--of programming logic devices manufactured by Altera and sold by Altera
--or its authorized distributors.  Please refer to the applicable
--agreement for further details.


-- turn off superfluous VHDL processor warnings 
-- altera message_level Level1 
-- altera message_off 10034 10035 10036 10037 10230 10240 10030 

library altera;
use altera.altera_europa_support_lib.all;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

--
--Burst adapter parameters:
--adapter is mastered by: cpu/data_master
--adapter masters: uart_1/s1
--asp_debug: 0
--byteaddr_width: 5
--ceil_data_width: 16
--data_width: 16
--dbs_shift: 1
--dbs_upstream_burstcount_width: 5
--downstream_addr_shift: 1
--downstream_burstcount_width: 1
--downstream_max_burstcount: 1
--downstream_pipeline: 1
--dynamic_slave: 0
--master_always_burst_max_burst: 0
--master_burst_on_burst_boundaries_only: 1
--master_data_width: 32
--master_interleave: 0
--master_linewrap_bursts: 0
--nativeaddr_width: 4
--slave_always_burst_max_burst: 0
--slave_burst_on_burst_boundaries_only: 0
--slave_interleave: 0
--slave_linewrap_bursts: 0
--upstream_burstcount: upstream_burstcount
--upstream_burstcount_width: 4
--upstream_max_burstcount: 8
--zero_address_width: 0


entity niosII_system_burst_18 is 
        port (
              -- inputs:
                 signal clk : IN STD_LOGIC;
                 signal downstream_readdata : IN STD_LOGIC_VECTOR (15 DOWNTO 0);
                 signal downstream_readdatavalid : IN STD_LOGIC;
                 signal downstream_waitrequest : IN STD_LOGIC;
                 signal reset_n : IN STD_LOGIC;
                 signal upstream_address : IN STD_LOGIC_VECTOR (4 DOWNTO 0);
                 signal upstream_burstcount : IN STD_LOGIC_VECTOR (3 DOWNTO 0);
                 signal upstream_byteenable : IN STD_LOGIC_VECTOR (1 DOWNTO 0);
                 signal upstream_debugaccess : IN STD_LOGIC;
                 signal upstream_nativeaddress : IN STD_LOGIC_VECTOR (3 DOWNTO 0);
                 signal upstream_read : IN STD_LOGIC;
                 signal upstream_write : IN STD_LOGIC;
                 signal upstream_writedata : IN STD_LOGIC_VECTOR (15 DOWNTO 0);

              -- outputs:
                 signal reg_downstream_address : OUT STD_LOGIC_VECTOR (3 DOWNTO 0);
                 signal reg_downstream_arbitrationshare : OUT STD_LOGIC_VECTOR (4 DOWNTO 0);
                 signal reg_downstream_burstcount : OUT STD_LOGIC;
                 signal reg_downstream_byteenable : OUT STD_LOGIC_VECTOR (1 DOWNTO 0);
                 signal reg_downstream_debugaccess : OUT STD_LOGIC;
                 signal reg_downstream_nativeaddress : OUT STD_LOGIC_VECTOR (3 DOWNTO 0);
                 signal reg_downstream_read : OUT STD_LOGIC;
                 signal reg_downstream_write : OUT STD_LOGIC;
                 signal reg_downstream_writedata : OUT STD_LOGIC_VECTOR (15 DOWNTO 0);
                 signal upstream_readdata : OUT STD_LOGIC_VECTOR (15 DOWNTO 0);
                 signal upstream_readdatavalid : OUT STD_LOGIC;
                 signal upstream_waitrequest : OUT STD_LOGIC
              );
end entity niosII_system_burst_18;


architecture europa of niosII_system_burst_18 is
                signal current_upstream_address :  STD_LOGIC_VECTOR (4 DOWNTO 0);
                signal downstream_address :  STD_LOGIC_VECTOR (3 DOWNTO 0);
                signal downstream_arbitrationshare :  STD_LOGIC_VECTOR (4 DOWNTO 0);
                signal downstream_burstcount :  STD_LOGIC;
                signal downstream_byteenable :  STD_LOGIC_VECTOR (1 DOWNTO 0);
                signal downstream_debugaccess :  STD_LOGIC;
                signal downstream_nativeaddress :  STD_LOGIC_VECTOR (3 DOWNTO 0);
                signal downstream_read :  STD_LOGIC;
                signal downstream_write :  STD_LOGIC;
                signal downstream_writedata :  STD_LOGIC_VECTOR (15 DOWNTO 0);
                signal internal_upstream_waitrequest :  STD_LOGIC;
                signal registered_upstream_address :  STD_LOGIC_VECTOR (4 DOWNTO 0);
                signal sync_nativeaddress :  STD_LOGIC;
                signal transactions_remaining :  STD_LOGIC_VECTOR (2 DOWNTO 0);

begin

  sync_nativeaddress <= or_reduce(upstream_nativeaddress);
  --downstream, which is an e_avalon_master
  --upstream, which is an e_avalon_slave
  process (clk, reset_n)
  begin
    if reset_n = '0' then
      registered_upstream_address <= std_logic_vector'("00000");
    elsif clk'event and clk = '1' then
      if std_logic'(NOT or_reduce(transactions_remaining)) = '1' then 
        registered_upstream_address <= upstream_address;
      end if;
    end if;

  end process;

  current_upstream_address <= A_WE_StdLogicVector((std_logic'(NOT or_reduce(transactions_remaining)) = '1'), upstream_address, registered_upstream_address);
  process (clk, reset_n)
  begin
    if reset_n = '0' then
      transactions_remaining <= std_logic_vector'("000");
    elsif clk'event and clk = '1' then
      transactions_remaining <= A_EXT (A_WE_StdLogicVector((std_logic'(((upstream_read AND NOT internal_upstream_waitrequest))) = '1'), (((std_logic_vector'("00000000000000000000000000000") & (upstream_burstcount)) - std_logic_vector'("000000000000000000000000000000001"))), (std_logic_vector'("00000000000000000000000000000") & (A_WE_StdLogicVector((std_logic'((((downstream_read AND NOT downstream_waitrequest) AND (or_reduce(transactions_remaining))))) = '1'), (((std_logic_vector'("0") & (transactions_remaining)) - (std_logic_vector'("000") & (A_TOSTDLOGICVECTOR(downstream_burstcount))))), (std_logic_vector'("0") & (transactions_remaining)))))), 3);
    end if;

  end process;

  downstream_burstcount <= std_logic'('1');
  downstream_arbitrationshare <= std_logic_vector'("0") & (upstream_burstcount);
  downstream_address <= current_upstream_address (3 DOWNTO 0);
  downstream_nativeaddress <= upstream_nativeaddress;
  downstream_read <= upstream_read OR (or_reduce(transactions_remaining));
  process (clk, reset_n)
  begin
    if reset_n = '0' then
      upstream_readdatavalid <= std_logic'('0');
    elsif clk'event and clk = '1' then
      upstream_readdatavalid <= downstream_readdatavalid;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      upstream_readdata <= std_logic_vector'("0000000000000000");
    elsif clk'event and clk = '1' then
      upstream_readdata <= downstream_readdata;
    end if;

  end process;

  downstream_write <= upstream_write AND NOT(downstream_read);
  downstream_byteenable <= upstream_byteenable;
  downstream_writedata <= upstream_writedata;
  internal_upstream_waitrequest <= downstream_waitrequest OR (or_reduce(transactions_remaining));
  downstream_debugaccess <= upstream_debugaccess;
  process (clk, reset_n)
  begin
    if reset_n = '0' then
      reg_downstream_address <= std_logic_vector'("0000");
    elsif clk'event and clk = '1' then
      if std_logic'(NOT downstream_waitrequest) = '1' then 
        reg_downstream_address <= downstream_address;
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      reg_downstream_arbitrationshare <= std_logic_vector'("00000");
    elsif clk'event and clk = '1' then
      if std_logic'(NOT downstream_waitrequest) = '1' then 
        reg_downstream_arbitrationshare <= downstream_arbitrationshare;
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      reg_downstream_burstcount <= std_logic'('0');
    elsif clk'event and clk = '1' then
      if std_logic'(NOT downstream_waitrequest) = '1' then 
        reg_downstream_burstcount <= downstream_burstcount;
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      reg_downstream_byteenable <= std_logic_vector'("00");
    elsif clk'event and clk = '1' then
      if std_logic'(NOT downstream_waitrequest) = '1' then 
        reg_downstream_byteenable <= downstream_byteenable;
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      reg_downstream_debugaccess <= std_logic'('0');
    elsif clk'event and clk = '1' then
      if std_logic'(NOT downstream_waitrequest) = '1' then 
        reg_downstream_debugaccess <= downstream_debugaccess;
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      reg_downstream_nativeaddress <= std_logic_vector'("0000");
    elsif clk'event and clk = '1' then
      if std_logic'(NOT downstream_waitrequest) = '1' then 
        reg_downstream_nativeaddress <= downstream_nativeaddress;
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      reg_downstream_read <= std_logic'('0');
    elsif clk'event and clk = '1' then
      if std_logic'(NOT downstream_waitrequest) = '1' then 
        reg_downstream_read <= downstream_read;
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      reg_downstream_write <= std_logic'('0');
    elsif clk'event and clk = '1' then
      if std_logic'(NOT downstream_waitrequest) = '1' then 
        reg_downstream_write <= downstream_write;
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      reg_downstream_writedata <= std_logic_vector'("0000000000000000");
    elsif clk'event and clk = '1' then
      if std_logic'(NOT downstream_waitrequest) = '1' then 
        reg_downstream_writedata <= downstream_writedata;
      end if;
    end if;

  end process;

  --vhdl renameroo for output signals
  upstream_waitrequest <= internal_upstream_waitrequest;

end europa;

