# elock_oobe
 NXP dual camera smart elock application




#1 : 分区说明
BOARD_FLASH:                                 0x60000000  
fatfs:                  #define FATFS_FLASH_REGION_OFFSET       (0x900000)     // -->2M
FACEREC_FS_ITEM_ADDR:   #define FACEREC_FS_MAP_ADDR             (0xB00000U)    // -->2M
//拓展的用户数据  
USER_EXTEND_FS_ADDR:    #define USER_EXTEND_FS_ADDR             (0xD00000U)    // -->2M          
config file:            #define CONFIG_FS_ADDR                  (0xF00000U)    // 

Total: 16M
 ______________________________________________________________________
|                       |                     |         |    |    |    |
|           A           |           B         |  FATFS  |FACE|USER|CFG |
|_______________________|_____________________|_________|____|____|____|

修改记录:  
xshx add 20220519