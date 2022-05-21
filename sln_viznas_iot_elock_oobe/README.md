# elock_oobe
 NXP dual camera smart elock application




#1 : 分区说明
BOARD_FLASH:                                 0x60000000  
fatfs:                 #define FATFS_FLASH_REGION_OFFSET       0xB20000//0x60B20000 -> 0x60D20000
FACEREC_FS_ITEM_ADDR:   #define FACEREC_FS_MAP_ADDR  (0xD20000U)  
//拓展的用户数据  
USER_EXTEND_FS_ADDR:    #define  USER_EXTEND_FS_ADDR    (0xE20000U)  
config文件的位置：  \#define CONFIG_FS_ADDR               (0xF20000U)//

Total: 16M
 ______________________________________________________________________
|                       |                     |         |    |    |    |
|           A           |           B         |  FATFS  |FACE|USER|CFG |
|_______________________|_____________________|_________|____|____|____|

修改记录:  
xshx add 20220519