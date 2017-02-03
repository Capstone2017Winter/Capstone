#
# dm9000a_driver_ucosii.tcl
#

# Create a new driver
create_driver dm9000a_driver

# Associate it with some hardware known as "dm9000a"
set_sw_property hw_class_name dm9000a

# The version of this driver is "7.2"
set_sw_property version 7.2

# This driver may be incompatible with versions of hardware less
# than specified below. Updates to hardware and device drivers
# rendering the driver incompatible with older versions of
# hardware are noted with this property assignment.
#
# Multiple-Version compatibility was introduced in version 7.1;
# prior versions are therefore excluded.
set_sw_property min_compatible_hw_version 7.1

# Initialize the driver in alt_sys_init()
set_sw_property auto_initialize true

# Location in generated BSP that above sources will be copied into
set_sw_property bsp_subdirectory drivers

# This driver supports the uC/OS-II (OS) type
add_sw_property supported_bsp_type HAL
add_sw_property supported_bsp_type UCOSII

#
# Source file listings...
#

# Include files
add_sw_property include_source UCOSII/inc/dm9000a.h
add_sw_property include_source UCOSII/inc/basic_io.h
add_sw_property include_source UCOSII/inc/dm9000a_regs.h

# C/C++ source files - INICHE
add_sw_property c_source UCOSII/src/dm9000a.c
add_sw_property include_directory UCOSII/inc

# End of file
