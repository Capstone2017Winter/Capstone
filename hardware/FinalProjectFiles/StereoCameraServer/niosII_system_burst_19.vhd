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
--adapter is mastered by: cpu/instruction_master
--adapter masters: sram_IF_0/tsb
--asp_debug: 0
--byteaddr_width: 20
--ceil_data_width: 16
--data_width: 16
--dbs_shift: 1
--dbs_upstream_burstcount_width: 5
--downstream_addr_shift: 1
--downstream_burstcount_width: 1
--downstream_max_burstcount: 1
--downstream_pipeline: 1
--dynamic_slave: 1
--master_always_burst_max_burst: 1
--master_burst_on_burst_boundaries_only: 0
--master_data_width: 32
--master_interleave: 0
--master_linewrap_bursts: 1
--nativeaddr_width: 19
--slave_always_burst_max_burst: 0
--slave_burst_on_burst_boundaries_only: 0
--slave_interleave: 0
--slave_linewrap_bursts: 0
--upstream_burstcount: 4'h8
--upstream_burstcount_width: 4
--upstream_max_burstcount: 8
--zero_address_width: 0


entity niosII_system_burst_19 is 
        port (
              -- inputs:
                 signal clk : IN STD_LOGIC;
                 signal downstream_readdata : IN STD_LOGIC_VECTOR (15 DOWNTO 0);
                 signal downstream_readdatavalid : IN STD_LOGIC;
                 signal downstream_waitrequest : IN STD_LOGIC;
                 signal reset_n : IN STD_LOGIC;
                 signal upstream_address : IN STD_LOGIC_VECTOR (19 DOWNTO 0);
                 signal upstream_byteenable : IN STD_LOGIC_VECTOR (1 DOWNTO 0);
                 signal upstream_debugaccess : IN STD_LOGIC;
                 signal upstream_nativeaddress : IN STD_LOGIC_VECTOR (18 DOWNTO 0);
                 signal upstream_read : IN STD_LOGIC;
                 signal upstream_write : IN STD_LOGIC;
                 signal upstream_writedata : IN STD_LOGIC_VECTOR (15 DOWNTO 0);

              -- outputs:
                 signal reg_downstream_address : OUT STD_LOGIC_VECTOR (18 DOWNTO 0);
                 signal reg_downstream_arbitrationshare : OUT STD_LOGIC_VECTOR (4 DOWNTO 0);
                 signal reg_downstream_burstcount : OUT STD_LOGIC;
                 signal reg_downstream_byteenable : OUT STD_LOGIC_VECTOR (1 DOWNTO 0);
                 signal reg_downstream_debugaccess : OUT STD_LOGIC;
                 signal reg_downstream_nativeaddress : OUT STD_LOGIC_VECTOR (18 DOWNTO 0);
                 signal reg_downstream_read : OUT STD_LOGIC;
                 signal reg_downstream_write : OUT STD_LOGIC;
                 signal reg_downstream_writedata : OUT STD_LOGIC_VECTOR (15 DOWNTO 0);
                 signal upstream_readdata : OUT STD_LOGIC_VECTOR (15 DOWNTO 0);
                 signal upstream_readdatavalid : OUT STD_LOGIC;
                 signal upstream_waitrequest : OUT STD_LOGIC
              );
end entity niosII_system_burst_19;


architecture europa of niosII_system_burst_19 is
                signal address_offset :  STD_LOGIC_VECTOR (3 DOWNTO 0);
                signal atomic_counter :  STD_LOGIC;
                signal burst_offset :  STD_LOGIC_VECTOR (3 DOWNTO 0);
                signal current_upstream_address :  STD_LOGIC_VECTOR (19 DOWNTO 0);
                signal current_upstream_burstcount :  STD_LOGIC_VECTOR (3 DOWNTO 0);
                signal current_upstream_read :  STD_LOGIC;
                signal current_upstream_write :  STD_LOGIC;
                signal data_counter :  STD_LOGIC_VECTOR (4 DOWNTO 0);
                signal dbs_adjusted_upstream_burstcount :  STD_LOGIC_VECTOR (4 DOWNTO 0);
                signal downstream_address :  STD_LOGIC_VECTOR (18 DOWNTO 0);
                signal downstream_address_base :  STD_LOGIC_VECTOR (19 DOWNTO 0);
                signal downstream_arbitrationshare :  STD_LOGIC_VECTOR (4 DOWNTO 0);
                signal downstream_burstcount :  STD_LOGIC;
                signal downstream_burstdone :  STD_LOGIC;
                signal downstream_byteenable :  STD_LOGIC_VECTOR (1 DOWNTO 0);
                signal downstream_debugaccess :  STD_LOGIC;
                signal downstream_nativeaddress :  STD_LOGIC_VECTOR (18 DOWNTO 0);
                signal downstream_read :  STD_LOGIC;
                signal downstream_write :  STD_LOGIC;
                signal downstream_write_reg :  STD_LOGIC;
                signal downstream_writedata :  STD_LOGIC_VECTOR (15 DOWNTO 0);
                signal enable_state_change :  STD_LOGIC;
                signal fifo_empty :  STD_LOGIC;
                signal internal_upstream_waitrequest :  STD_LOGIC;
                signal max_burst_size :  STD_LOGIC;
                signal p1_atomic_counter :  STD_LOGIC;
                signal p1_fifo_empty :  STD_LOGIC;
                signal p1_state_busy :  STD_LOGIC;
                signal p1_state_idle :  STD_LOGIC;
                signal pending_register_enable :  STD_LOGIC;
                signal pending_upstream_read :  STD_LOGIC;
                signal pending_upstream_read_reg :  STD_LOGIC;
                signal pending_upstream_write :  STD_LOGIC;
                signal pending_upstream_write_reg :  STD_LOGIC;
                signal read_address_offset :  STD_LOGIC_VECTOR (3 DOWNTO 0);
                signal read_update_count :  STD_LOGIC;
                signal read_write_dbs_adjusted_upstream_burstcount :  STD_LOGIC_VECTOR (4 DOWNTO 0);
                signal registered_read_write_dbs_adjusted_upstream_burstcount :  STD_LOGIC_VECTOR (4 DOWNTO 0);
                signal registered_upstream_address :  STD_LOGIC_VECTOR (19 DOWNTO 0);
                signal registered_upstream_burstcount :  STD_LOGIC_VECTOR (3 DOWNTO 0);
                signal registered_upstream_byteenable :  STD_LOGIC_VECTOR (1 DOWNTO 0);
                signal registered_upstream_nativeaddress :  STD_LOGIC_VECTOR (18 DOWNTO 0);
                signal registered_upstream_read :  STD_LOGIC;
                signal registered_upstream_write :  STD_LOGIC;
                signal state_busy :  STD_LOGIC;
                signal state_idle :  STD_LOGIC;
                signal sync_nativeaddress :  STD_LOGIC;
                signal transactions_remaining :  STD_LOGIC_VECTOR (4 DOWNTO 0);
                signal transactions_remaining_reg :  STD_LOGIC_VECTOR (4 DOWNTO 0);
                signal update_count :  STD_LOGIC;
                signal upstream_burstdone :  STD_LOGIC;
                signal upstream_read_run :  STD_LOGIC;
                signal upstream_write_run :  STD_LOGIC;
                signal write_address_offset :  STD_LOGIC_VECTOR (3 DOWNTO 0);
                signal write_update_count :  STD_LOGIC;

begin

  sync_nativeaddress <= or_reduce(upstream_nativeaddress);
  --downstream, which is an e_avalon_master
  --upstream, which is an e_avalon_slave
  upstream_burstdone <= A_WE_StdLogic((std_logic'(current_upstream_read) = '1'), ((to_std_logic(((transactions_remaining = (std_logic_vector'("0000") & (A_TOSTDLOGICVECTOR(downstream_burstcount)))))) AND downstream_read) AND NOT downstream_waitrequest), ((to_std_logic((((std_logic_vector'("0000000000000000000000000000") & (transactions_remaining)) = (((std_logic_vector'("00000000000000000000000000000000") & (A_TOSTDLOGICVECTOR(atomic_counter))) + std_logic_vector'("000000000000000000000000000000001")))))) AND downstream_write) AND NOT downstream_waitrequest));
  p1_atomic_counter <= Vector_To_Std_Logic(((std_logic_vector'("00000000000000000000000000000000") & (A_TOSTDLOGICVECTOR(atomic_counter))) + (std_logic_vector'("0") & ((A_WE_StdLogicVector((std_logic'(downstream_read) = '1'), (std_logic_vector'("0000000000000000000000000000000") & (A_TOSTDLOGICVECTOR(downstream_burstcount))), std_logic_vector'("00000000000000000000000000000001")))))));
  downstream_burstdone <= (((downstream_read OR downstream_write)) AND NOT downstream_waitrequest) AND to_std_logic(((std_logic'(p1_atomic_counter) = std_logic'(downstream_burstcount))));
  dbs_adjusted_upstream_burstcount <= A_WE_StdLogicVector((std_logic'(pending_register_enable) = '1'), read_write_dbs_adjusted_upstream_burstcount, registered_read_write_dbs_adjusted_upstream_burstcount);
  read_write_dbs_adjusted_upstream_burstcount <= Std_Logic_Vector'(std_logic_vector'("1000") & A_ToStdLogicVector(std_logic'('0')));
  process (clk, reset_n)
  begin
    if reset_n = '0' then
      registered_read_write_dbs_adjusted_upstream_burstcount <= std_logic_vector'("00000");
    elsif clk'event and clk = '1' then
      if std_logic'(pending_register_enable) = '1' then 
        registered_read_write_dbs_adjusted_upstream_burstcount <= read_write_dbs_adjusted_upstream_burstcount;
      end if;
    end if;

  end process;

  p1_state_idle <= ((state_idle AND NOT upstream_read) AND NOT upstream_write) OR ((((state_busy AND to_std_logic((((std_logic_vector'("000000000000000000000000000") & (data_counter)) = std_logic_vector'("00000000000000000000000000000000"))))) AND p1_fifo_empty) AND NOT pending_upstream_read) AND NOT pending_upstream_write);
  p1_state_busy <= (state_idle AND ((upstream_read OR upstream_write))) OR (state_busy AND ((((to_std_logic(NOT (((std_logic_vector'("000000000000000000000000000") & (data_counter)) = std_logic_vector'("00000000000000000000000000000000")))) OR NOT p1_fifo_empty) OR pending_upstream_read) OR pending_upstream_write)));
  enable_state_change <= NOT ((downstream_read OR downstream_write)) OR NOT downstream_waitrequest;
  process (clk, reset_n)
  begin
    if reset_n = '0' then
      pending_upstream_read_reg <= std_logic'('0');
    elsif clk'event and clk = '1' then
      if std_logic'((upstream_read AND state_idle)) = '1' then 
        pending_upstream_read_reg <= Vector_To_Std_Logic(-SIGNED(std_logic_vector'("00000000000000000000000000000001")));
      elsif std_logic'(upstream_burstdone) = '1' then 
        pending_upstream_read_reg <= std_logic'('0');
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      pending_upstream_write_reg <= std_logic'('0');
    elsif clk'event and clk = '1' then
      if std_logic'(upstream_burstdone) = '1' then 
        pending_upstream_write_reg <= std_logic'('0');
      elsif std_logic'((upstream_write AND ((state_idle OR NOT internal_upstream_waitrequest)))) = '1' then 
        pending_upstream_write_reg <= Vector_To_Std_Logic(-SIGNED(std_logic_vector'("00000000000000000000000000000001")));
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      state_idle <= std_logic'('1');
    elsif clk'event and clk = '1' then
      if std_logic'(enable_state_change) = '1' then 
        state_idle <= p1_state_idle;
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      state_busy <= std_logic'('0');
    elsif clk'event and clk = '1' then
      if std_logic'(enable_state_change) = '1' then 
        state_busy <= p1_state_busy;
      end if;
    end if;

  end process;

  pending_upstream_read <= pending_upstream_read_reg;
  pending_upstream_write <= pending_upstream_write_reg AND NOT upstream_burstdone;
  pending_register_enable <= state_idle OR ((((upstream_read OR upstream_write)) AND NOT internal_upstream_waitrequest));
  process (clk, reset_n)
  begin
    if reset_n = '0' then
      registered_upstream_read <= std_logic'('0');
    elsif clk'event and clk = '1' then
      if std_logic'(pending_register_enable) = '1' then 
        registered_upstream_read <= upstream_read;
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      registered_upstream_write <= std_logic'('0');
    elsif clk'event and clk = '1' then
      if std_logic'(pending_register_enable) = '1' then 
        registered_upstream_write <= upstream_write;
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      registered_upstream_burstcount <= std_logic_vector'("0000");
    elsif clk'event and clk = '1' then
      if std_logic'(pending_register_enable) = '1' then 
        registered_upstream_burstcount <= std_logic_vector'("1000");
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      registered_upstream_address <= std_logic_vector'("00000000000000000000");
    elsif clk'event and clk = '1' then
      if std_logic'(pending_register_enable) = '1' then 
        registered_upstream_address <= upstream_address;
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      registered_upstream_nativeaddress <= std_logic_vector'("0000000000000000000");
    elsif clk'event and clk = '1' then
      if std_logic'(pending_register_enable) = '1' then 
        registered_upstream_nativeaddress <= upstream_nativeaddress;
      end if;
    end if;

  end process;

  current_upstream_read <= registered_upstream_read AND NOT(downstream_write);
  current_upstream_write <= registered_upstream_write;
  current_upstream_address <= registered_upstream_address;
  current_upstream_burstcount <= A_WE_StdLogicVector((std_logic'(pending_register_enable) = '1'), std_logic_vector'("1000"), registered_upstream_burstcount);
  process (clk, reset_n)
  begin
    if reset_n = '0' then
      atomic_counter <= std_logic'('0');
    elsif clk'event and clk = '1' then
      if std_logic'((((downstream_read OR downstream_write)) AND NOT downstream_waitrequest)) = '1' then 
        atomic_counter <= Vector_To_Std_Logic(A_WE_StdLogicVector((std_logic'(downstream_burstdone) = '1'), std_logic_vector'("00000000000000000000000000000000"), (std_logic_vector'("0000000000000000000000000000000") & (A_TOSTDLOGICVECTOR(p1_atomic_counter)))));
      end if;
    end if;

  end process;

  read_update_count <= current_upstream_read AND NOT downstream_waitrequest;
  write_update_count <= (current_upstream_write AND downstream_write) AND downstream_burstdone;
  update_count <= read_update_count OR write_update_count;
  transactions_remaining <= A_WE_StdLogicVector((std_logic'(((state_idle AND ((upstream_read OR upstream_write))))) = '1'), dbs_adjusted_upstream_burstcount, transactions_remaining_reg);
  process (clk, reset_n)
  begin
    if reset_n = '0' then
      transactions_remaining_reg <= std_logic_vector'("00000");
    elsif clk'event and clk = '1' then
      transactions_remaining_reg <= A_EXT (A_WE_StdLogicVector((std_logic'(((state_idle AND ((upstream_read OR upstream_write))))) = '1'), (std_logic_vector'("0") & (dbs_adjusted_upstream_burstcount)), A_WE_StdLogicVector((std_logic'(update_count) = '1'), ((std_logic_vector'("0") & (transactions_remaining_reg)) - (std_logic_vector'("00000") & (A_TOSTDLOGICVECTOR(downstream_burstcount)))), (std_logic_vector'("0") & (transactions_remaining_reg)))), 5);
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      data_counter <= std_logic_vector'("00000");
    elsif clk'event and clk = '1' then
      data_counter <= A_EXT (A_WE_StdLogicVector((std_logic'(((state_idle AND upstream_read) AND NOT internal_upstream_waitrequest)) = '1'), (std_logic_vector'("0000000000000000000000000000") & (dbs_adjusted_upstream_burstcount)), A_WE_StdLogicVector((std_logic'(downstream_readdatavalid) = '1'), ((std_logic_vector'("0000000000000000000000000000") & (data_counter)) - std_logic_vector'("000000000000000000000000000000001")), (std_logic_vector'("0000000000000000000000000000") & (data_counter)))), 5);
    end if;

  end process;

  max_burst_size <= std_logic'('1');
  downstream_burstcount <= Vector_To_Std_Logic(A_WE_StdLogicVector(((transactions_remaining>(std_logic_vector'("0000") & (A_TOSTDLOGICVECTOR(max_burst_size))))), (std_logic_vector'("0000") & (A_TOSTDLOGICVECTOR(max_burst_size))), transactions_remaining));
  downstream_arbitrationshare <= A_WE_StdLogicVector((std_logic'(current_upstream_read) = '1'), (dbs_adjusted_upstream_burstcount), dbs_adjusted_upstream_burstcount);
  process (clk, reset_n)
  begin
    if reset_n = '0' then
      write_address_offset <= std_logic_vector'("0000");
    elsif clk'event and clk = '1' then
      write_address_offset <= A_EXT (A_WE_StdLogicVector((std_logic'((state_idle AND upstream_write)) = '1'), std_logic_vector'("00000000000000000000000000000000"), (std_logic_vector'("000000000000000000000000000") & (A_WE_StdLogicVector((std_logic'((((downstream_write AND NOT downstream_waitrequest) AND downstream_burstdone))) = '1'), ((std_logic_vector'("0") & (write_address_offset)) + (std_logic_vector'("0000") & (A_TOSTDLOGICVECTOR(downstream_burstcount)))), (std_logic_vector'("0") & (write_address_offset)))))), 4);
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      read_address_offset <= std_logic_vector'("0000");
    elsif clk'event and clk = '1' then
      read_address_offset <= A_EXT (A_WE_StdLogicVector((std_logic'((state_idle AND upstream_read)) = '1'), std_logic_vector'("00000000000000000000000000000000"), (std_logic_vector'("000000000000000000000000000") & (A_WE_StdLogicVector((std_logic'(((downstream_read AND NOT downstream_waitrequest))) = '1'), ((std_logic_vector'("0") & (read_address_offset)) + (std_logic_vector'("0000") & (A_TOSTDLOGICVECTOR(downstream_burstcount)))), (std_logic_vector'("0") & (read_address_offset)))))), 4);
    end if;

  end process;

  downstream_nativeaddress <= A_SRL(registered_upstream_nativeaddress,std_logic_vector'("00000000000000000000000000000010"));
  address_offset <= A_WE_StdLogicVector((std_logic'(current_upstream_read) = '1'), read_address_offset, write_address_offset);
  downstream_address_base <= Std_Logic_Vector'(current_upstream_address(19 DOWNTO 2) & std_logic_vector'("00"));
  burst_offset <= A_EXT (((std_logic_vector'("0") & (downstream_address_base(4 DOWNTO 1))) + (std_logic_vector'("0") & (address_offset))), 4);
  downstream_address <= A_EXT (Std_Logic_Vector'(downstream_address_base(19 DOWNTO 5) & burst_offset & A_ToStdLogicVector(std_logic'('0'))), 19);
  process (clk, reset_n)
  begin
    if reset_n = '0' then
      downstream_read <= std_logic'('0');
    elsif clk'event and clk = '1' then
      if std_logic'((NOT downstream_read OR NOT downstream_waitrequest)) = '1' then 
        downstream_read <= Vector_To_Std_Logic(A_WE_StdLogicVector((std_logic'((state_idle AND upstream_read)) = '1'), std_logic_vector'("00000000000000000000000000000001"), A_WE_StdLogicVector(((transactions_remaining = (std_logic_vector'("0000") & (A_TOSTDLOGICVECTOR(downstream_burstcount))))), std_logic_vector'("00000000000000000000000000000000"), (std_logic_vector'("0000000000000000000000000000000") & (A_TOSTDLOGICVECTOR(downstream_read))))));
      end if;
    end if;

  end process;

  upstream_readdatavalid <= downstream_readdatavalid;
  upstream_readdata <= downstream_readdata;
  fifo_empty <= std_logic'('1');
  p1_fifo_empty <= std_logic'('1');
  process (clk, reset_n)
  begin
    if reset_n = '0' then
      downstream_write_reg <= std_logic'('0');
    elsif clk'event and clk = '1' then
      if std_logic'((NOT downstream_write_reg OR NOT downstream_waitrequest)) = '1' then 
        downstream_write_reg <= Vector_To_Std_Logic(A_WE_StdLogicVector((std_logic'((state_idle AND upstream_write)) = '1'), std_logic_vector'("00000000000000000000000000000001"), A_WE_StdLogicVector((std_logic'(((to_std_logic(((transactions_remaining = (std_logic_vector'("0000") & (A_TOSTDLOGICVECTOR(downstream_burstcount)))))) AND downstream_burstdone))) = '1'), std_logic_vector'("00000000000000000000000000000000"), (std_logic_vector'("0000000000000000000000000000000") & (A_TOSTDLOGICVECTOR(downstream_write_reg))))));
      end if;
    end if;

  end process;

  process (clk, reset_n)
  begin
    if reset_n = '0' then
      registered_upstream_byteenable <= std_logic_vector'("11");
    elsif clk'event and clk = '1' then
      if std_logic'(pending_register_enable) = '1' then 
        registered_upstream_byteenable <= upstream_byteenable;
      end if;
    end if;

  end process;

  downstream_write <= (downstream_write_reg AND upstream_write) AND NOT(downstream_read);
  downstream_byteenable <= A_WE_StdLogicVector((std_logic'(downstream_write_reg) = '1'), upstream_byteenable, registered_upstream_byteenable);
  downstream_writedata <= upstream_writedata;
  upstream_read_run <= state_idle AND upstream_read;
  upstream_write_run <= ((state_busy AND upstream_write) AND NOT downstream_waitrequest) AND NOT(downstream_read);
  internal_upstream_waitrequest <= Vector_To_Std_Logic(A_WE_StdLogicVector((std_logic'(((upstream_read OR current_upstream_read))) = '1'), (std_logic_vector'("0000000000000000000000000000000") & (A_TOSTDLOGICVECTOR(NOT upstream_read_run))), A_WE_StdLogicVector((std_logic'(current_upstream_write) = '1'), (std_logic_vector'("0000000000000000000000000000000") & (A_TOSTDLOGICVECTOR(NOT upstream_write_run))), std_logic_vector'("00000000000000000000000000000001"))));
  downstream_debugaccess <= upstream_debugaccess;
  process (clk, reset_n)
  begin
    if reset_n = '0' then
      reg_downstream_address <= std_logic_vector'("0000000000000000000");
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
      reg_downstream_nativeaddress <= std_logic_vector'("0000000000000000000");
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

