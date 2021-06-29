#ifndef _system_H
#define _system_H


#include "stm32f10x.h"


//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入



#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height))
#define LINEY(x) ((x) * (((sFONT *)LCD_GetFont())->Width))

#define FSMC_ACCESS_MODE_A                        0x00000000U
#define FSMC_ACCESS_MODE_B                        ((uint32_t)FSMC_BTRx_ACCMOD_0)
#define FSMC_ACCESS_MODE_C                        ((uint32_t)FSMC_BTRx_ACCMOD_1)
#define FSMC_ACCESS_MODE_D                        ((uint32_t)(FSMC_BTRx_ACCMOD_0 | FSMC_BTRx_ACCMOD_1))
#define FSMC_BTRx_ACCMOD_0                  (0x1U << FSMC_BTRx_ACCMOD_Pos)     /*!< 0x10000000 */
#define FSMC_BTRx_ACCMOD_Pos                (28U)          
#define FSMC_NORSRAM_TypeDef            FSMC_Bank1_TypeDef
#define FSMC_NORSRAM_EXTENDED_TypeDef   FSMC_Bank1E_TypeDef
#define FSMC_NAND_TypeDef               FSMC_Bank2_3_TypeDef
#define FSMC_PCCARD_TypeDef             FSMC_Bank4_TypeDef

#define FSMC_NORSRAM_DEVICE             FSMC_Bank1
#define FSMC_NORSRAM_EXTENDED_DEVICE    FSMC_Bank1E
#define FSMC_NAND_DEVICE                FSMC_Bank2_3
#define FSMC_PCCARD_DEVICE              FSMC_Bank4


#define FSMC_MEMORY_TYPE_SRAM                    0x00000000U
#define FSMC_MEMORY_TYPE_PSRAM                   ((uint32_t)FSMC_BCRx_MTYP_0)
#define FSMC_MEMORY_TYPE_NOR                     ((uint32_t)FSMC_BCRx_MTYP_1)


#define FSMC_BCRx_MTYP_Pos                  (2U)                               
#define FSMC_BCRx_MTYP_Msk                  (0x3U << FSMC_BCRx_MTYP_Pos)       /*!< 0x0000000C */
#define FSMC_BCRx_MTYP                      FSMC_BCRx_MTYP_Msk                 /*!< MTYP[1:0] bits (Memory type) */
#define FSMC_BCRx_MTYP_0                    (0x1U << FSMC_BCRx_MTYP_Pos)       /*!< 0x00000004 */
#define FSMC_BCRx_MTYP_1                    (0x2U << FSMC_BCRx_MTYP_Pos)       /*!< 0x00000008 */



#define FSMC_NORSRAM_MEM_BUS_WIDTH_8             0x00000000U
#define FSMC_NORSRAM_MEM_BUS_WIDTH_16            ((uint32_t)FSMC_BCRx_MWID_0)
#define FSMC_NORSRAM_MEM_BUS_WIDTH_32            ((uint32_t)FSMC_BCRx_MWID_1)


#define FSMC_BCRx_MWID_Pos                  (4U)                               
#define FSMC_BCRx_MWID_Msk                  (0x3U << FSMC_BCRx_MWID_Pos)       /*!< 0x00000030 */
#define FSMC_BCRx_MWID                      FSMC_BCRx_MWID_Msk                 /*!< MWID[1:0] bits (Memory data bus width) */
#define FSMC_BCRx_MWID_0                    (0x1U << FSMC_BCRx_MWID_Pos)       /*!< 0x00000010 */
#define FSMC_BCRx_MWID_1                    (0x2U << FSMC_BCRx_MWID_Pos)       /*!< 0x00000020 */


#define FSMC_BURST_ACCESS_MODE_DISABLE           0x00000000U
#define FSMC_WAIT_SIGNAL_POLARITY_LOW            0x00000000U
#define FSMC_WAIT_TIMING_BEFORE_WS               0x00000000U

#define FSMC_WRITE_OPERATION_ENABLE              ((uint32_t)FSMC_BCRx_WREN)



#define FSMC_BCRx_FACCEN_Pos                (6U)                               
#define FSMC_BCRx_FACCEN_Msk                (0x1U << FSMC_BCRx_FACCEN_Pos)     /*!< 0x00000040 */
#define FSMC_BCRx_FACCEN                    FSMC_BCRx_FACCEN_Msk               /*!< Flash access enable */
#define FSMC_BCRx_BURSTEN_Pos               (8U)                               
#define FSMC_BCRx_BURSTEN_Msk               (0x1U << FSMC_BCRx_BURSTEN_Pos)    /*!< 0x00000100 */
#define FSMC_BCRx_BURSTEN                   FSMC_BCRx_BURSTEN_Msk              /*!< Burst enable bit */
#define FSMC_BCRx_WAITPOL_Pos               (9U)                               
#define FSMC_BCRx_WAITPOL_Msk               (0x1U << FSMC_BCRx_WAITPOL_Pos)    /*!< 0x00000200 */
#define FSMC_BCRx_WAITPOL                   FSMC_BCRx_WAITPOL_Msk              /*!< Wait signal polarity bit */
#define FSMC_BCRx_WRAPMOD_Pos               (10U)                              
#define FSMC_BCRx_WRAPMOD_Msk               (0x1U << FSMC_BCRx_WRAPMOD_Pos)    /*!< 0x00000400 */
#define FSMC_BCRx_WRAPMOD                   FSMC_BCRx_WRAPMOD_Msk              /*!< Wrapped burst mode support */
#define FSMC_BCRx_WAITCFG_Pos               (11U)                              
#define FSMC_BCRx_WAITCFG_Msk               (0x1U << FSMC_BCRx_WAITCFG_Pos)    /*!< 0x00000800 */
#define FSMC_BCRx_WAITCFG                   FSMC_BCRx_WAITCFG_Msk              /*!< Wait timing configuration */
#define FSMC_BCRx_WREN_Pos                  (12U)                              
#define FSMC_BCRx_WREN_Msk                  (0x1U << FSMC_BCRx_WREN_Pos)       /*!< 0x00001000 */
#define FSMC_BCRx_WREN                      FSMC_BCRx_WREN_Msk                 /*!< Write enable bit */
#define FSMC_BCRx_WAITEN_Pos                (13U)                              
#define FSMC_BCRx_WAITEN_Msk                (0x1U << FSMC_BCRx_WAITEN_Pos)     /*!< 0x00002000 */
#define FSMC_BCRx_WAITEN                    FSMC_BCRx_WAITEN_Msk               /*!< Wait enable bit */
#define FSMC_BCRx_EXTMOD_Pos                (14U)                              
#define FSMC_BCRx_EXTMOD_Msk                (0x1U << FSMC_BCRx_EXTMOD_Pos)     /*!< 0x00004000 */
#define FSMC_BCRx_EXTMOD                    FSMC_BCRx_EXTMOD_Msk               /*!< Extended mode enable */
#define FSMC_BCRx_ASYNCWAIT_Pos             (15U)                              
#define FSMC_BCRx_ASYNCWAIT_Msk             (0x1U << FSMC_BCRx_ASYNCWAIT_Pos)  /*!< 0x00008000 */
#define FSMC_BCRx_ASYNCWAIT                 FSMC_BCRx_ASYNCWAIT_Msk            /*!< Asynchronous wait */
#define FSMC_BCRx_CBURSTRW_Pos              (19U)                              
#define FSMC_BCRx_CBURSTRW_Msk              (0x1U << FSMC_BCRx_CBURSTRW_Pos)   /*!< 0x00080000 */
#define FSMC_BCRx_CBURSTRW                  FSMC_BCRx_CBURSTRW_Msk             /*!< Write burst enable */



#define FSMC_WAIT_SIGNAL_DISABLE                 0x00000000U
#define FSMC_EXTENDED_MODE_DISABLE               0x00000000U
#define FSMC_ASYNCHRONOUS_WAIT_DISABLE           0x00000000U
#define FSMC_WRITE_BURST_DISABLE                 0x00000000U
/** @defgroup FONTS_Exported_Types
  * @{
  */ 

#endif
