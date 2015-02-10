;/*----------------------------------------------------------------------------
; *      RL-ARM - RTX
; *----------------------------------------------------------------------------
; *      Name:    SWI_TABLE.S
; *      Purpose: Pre-defined SWI Table
; *      Rev.:    V4.13
; *----------------------------------------------------------------------------
; *      This code is part of the RealView Run-Time Library.
; *      Copyright (c) 2004-2010 KEIL - An ARM Company. All rights reserved.
; *---------------------------------------------------------------------------*/


                AREA    SWI_TABLE, CODE, READONLY

                EXPORT  SWI_Count

SWI_Cnt         EQU    (SWI_End-SWI_Table)/4
SWI_Count       DCD     SWI_Cnt

                IMPORT  __SWI_0
                IMPORT  __SWI_1
                IMPORT  __SWI_2
                IMPORT  __SWI_3
                IMPORT  __SWI_4
                IMPORT  __SWI_5
                IMPORT  __SWI_6
                IMPORT  __SWI_7

; Import user SWI functions here.
;               IMPORT  __SWI_8


                EXPORT  SWI_Table
SWI_Table
                DCD     __SWI_0                 ; SWI 0 used by RTX
                DCD     __SWI_1                 ; SWI 1 used by RTX
                DCD     __SWI_2                 ; SWI 2 used by RTX
                DCD     __SWI_3                 ; SWI 3 used by RTX
                DCD     __SWI_4                 ; SWI 4 used by RTX
                DCD     __SWI_5                 ; SWI 5 used by RTX
                DCD     __SWI_6                 ; SWI 6 used by RTX
                DCD     __SWI_7                 ; SWI 7 used by RTX

; Insert user SWI functions here. SWI 0..7 are used by RTX Kernel.
;               DCD     __SWI_8                 ; user SWI function

SWI_End

                END

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
