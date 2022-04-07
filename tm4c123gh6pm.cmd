/******************************************************************************
 *
 * Default Linker Command file for the Texas Instruments TM4C123GH6PM
 *
 * This is derived from revision 15071 of the TivaWare Library.
 *
 *****************************************************************************/

--retain=g_pfnVectors

MEMORY
{
	PAGE 0:
	    FLASH (RX) : origin = 0x00000000, length = 0x00040000
	    SRAM (RWX) : origin = 0x20000000, length = 0x00008000
	    UART1 (RW) : origin = 0x4000C000, length = 0x00001000
}

/* The following command line options are set as part of the CCS project.    */
/* If you are building using the command line, or for some reason want to    */
/* define them here, you can uncomment and modify these lines as needed.     */
/* If you are using CCS for building, it is probably better to make any such */
/* modifications in your CCS project and leave this file alone.              */
/*                                                                           */
/* --heap_size=0                                                             */
/* --stack_size=256                                                          */
/* --library=rtsv7M4_T_le_eabi.lib                                           */

/* Section allocation in memory */

SECTIONS
{
    .intvecs:   > 0x00000000
    .text   :   > FLASH PAGE 0
    .const  :   > FLASH PAGE 0
    .cinit  :   > FLASH PAGE 0
    .pinit  :   > FLASH PAGE 0
    .init_array : > FLASH PAGE 0

    .vtable :   > 0x20000000
    .data   :   > SRAM PAGE 0
    .bss    :   > SRAM PAGE 0
    .sysmem :   > SRAM PAGE 0
    .stack  :   > SRAM PAGE 0


    .uart   :   > UART1 PAGE 0
}

__STACK_TOP = __stack + 512;
