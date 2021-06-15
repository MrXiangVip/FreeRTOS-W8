#include "board.h"
#include "fsl_himax.h"
#include "fsl_dvpmipi.h"
#include <stdio.h>
#include "fsl_log.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define HIMAX_DelayMs(ms) VIDEO_DelayMs(ms)

/** Himax frame sequence mode selection settings */
static hx_frame_mode_select_setting_t hx_frame_mode_select_settings[] = {
	//  slave    length of tx_cmd={cmd(2), len(1), data with CRC8 (w/o slave_addr_8bit)} (for real I2C TX).
	// mode_id,     cmd_len, {addr  cmd_h cmd_l len   data1 data2, ..., (w/o CRC8)}=cmd (for calculating CRC8).
	{ HX_FRAME_MODE_SELECT_ONLY_NIR,          5, {0x6E, 0x01, 0x02, 0x05, 0x01, 0x69} },
	{ HX_FRAME_MODE_SELECT_ONLY_DOT,          5, {0x6E, 0x01, 0x02, 0x05, 0x02, 0x6A} },
	{ HX_FRAME_MODE_SELECT_ONLY_DEPTH,        5, {0x6E, 0x01, 0x02, 0x05, 0x03, 0x6B} },
	{ HX_FRAME_MODE_SELECT_ALT_NIR_DEPTH,     5, {0x6E, 0x01, 0x02, 0x05, 0x04, 0x6C} },
	{ HX_FRAME_MODE_SELECT_ALT_NIR_DEPTH_DOT, 5, {0x6E, 0x01, 0x02, 0x05, 0x0F, 0x67} },
};

/*******************************************************************************
 * Code
 ******************************************************************************/
static inline uint16_t read_be16(uint8_t *buf)
{
	return (buf[0] << 8) | buf[1];
}

static inline uint16_t read_le16(uint8_t *buf)
{
	return buf[0] | (buf[1] << 8);
}

static inline void write_le16(uint8_t *buf, uint16_t val)
{
	buf[0] = val;
	buf[1] = val >> 8;
}

static inline void write_be16(uint8_t *buf, uint16_t val)
{
	buf[0] = val >> 8;
	buf[1] = val;
}

/*
 * @internal
 * @brief Calculate 8-bit CRC by doing XOR operation on each byte.
 * @param[in] data Pointer pointed to the data buffer.
 * @param[in] len Length of the data buffer to be calculated.
 * @return Result checksum, the 8-bit CRC.
 */
static inline uint8_t hx_crc8(const uint8_t *data, int len)
{
	uint8_t crc8 = 0;

	while (len > 0) {
		crc8 ^= *data;
		data++;
		len--;
	}
	return crc8;
}


/**
 * @internal
 * @brief Get index of Himax frame sequence mode selection setting with matched mode ID.
 * @ingroup ctrl
 *
 * @param[in] mode_id Himax frame sequence mode ID
 * @return On success found, index of member of UVC function setting
 *   with matched Control ID would be returned;
 *   otherwise, return UNKNOWN index.
 */
static uint8_t hx_get_frame_mode_select_setting_idx(int mode_id) {
	uint8_t max = sizeof(hx_frame_mode_select_settings);
	uint8_t i, idx = HX_FRAME_MODE_SELECT_UNKNOWN;
	for (i=0 ; i<max ; i++) {
		if ((int)hx_frame_mode_select_settings[i].mode_id == mode_id) {
			idx = i;
			break;
		}
	}
	return idx;
}

/**
 * @brief Himax I2C burst mode write+read function.
 *
 * @note Stop + Start between write and read command.
 *
 * @param[in]  fd     I2C file descriptor.
 * @param[in]  data_w Pointer to a write command byte array. (with checksum)
 * @param[in]  len_w  Length of data_w in bytes. (with checksum)
 * @param[out] data_r Pointer to a read command byte array. (with checksum)
 * @param[in]  len_r  Length of data_r in bytes. (with checksum)
 * @return 0 on success; otherwise, negative value on failure.
 */
int hx_i2c_burst_write_read(uint8_t *data_w, uint16_t len_w,
                                uint8_t *data_r, uint16_t len_r)
{
    int ret = -1;
    uint8_t crc_r = 0;
    ret = BOARD_3DCamera_I2C_Send(SH430MH_NIR_CAM_I2C_SLAVE_ADDR_7BIT, 0, 0, data_w, len_w);
    if (ret != kStatus_Success)
        return ret;
    HIMAX_DelayMs(5);

	do {
		ret = BOARD_3DCamera_I2C_Receive(SH430MH_NIR_CAM_I2C_SLAVE_ADDR_7BIT, 0, 0, data_r, len_r);
		if (ret != kStatus_Success) {
			LOGE("%s: I2C R %u bytes fail, ret=%d, err=%d(%s), retry...\r\n");
		} else{
			crc_r = hx_crc8(data_r, len_r -1);
			if (crc_r == data_r[len_r -1])
				break;
			else
				LOGE("%s: I2C R checksum fail, retry...\r\n", __func__);
		}
		HIMAX_DelayMs(1);
	} while (1);

	return 0;
}


/**
 * @brief Himax SH430 frame mode selection for different frame type.
 *
 * @param[in] mode_id Himax frame sequence mode ID, plese refer to
 *                    the "cam_ctrl.h" for mode_id enum definition.
 * @return RET_OK (0) on success; otherwise, RET_FAIL (-1).
 */
int hx_cam_ctrl_frame_mode_select(int mode_id)
{
	int32_t ret = 0;
	uint8_t fms_idx = HX_FRAME_MODE_SELECT_UNKNOWN;
	uint8_t status_buf[5];  // 6F 01 01 04 6B (slv cmd_h cmd_l len CRC8)
	uint8_t status_len = 5;
	uint8_t status_crc8 = 0;

	fms_idx = hx_get_frame_mode_select_setting_idx(mode_id);
	if (fms_idx == HX_FRAME_MODE_SELECT_UNKNOWN) {
		LOGE("%s: Failed to get index of frame mode (%u) select settings. \r\n",
				__func__, mode_id);
		return -1;
	}
	LOGD("%s: fms_idx=%u \r\n", __func__, fms_idx);
    uint8_t cmd_len = hx_frame_mode_select_settings[fms_idx].cmd_len; 
	LOGD("%s: fms.cmd_len=%u \r\n", __func__, cmd_len);
 
    //uint8_t crc_w = hx_crc8(hx_frame_mode_select_settings[fms_idx].cmd, cmd_len);

    //hx_frame_mode_select_settings[fms_idx].cmd[cmd_len] = crc_w;

	ret = hx_i2c_burst_write_read(&hx_frame_mode_select_settings[fms_idx].cmd[1],cmd_len,
		                          status_buf, status_len);
    
	if (ret != 0) {
		LOGE("%s: I2C command write/read fail \r\n", __func__);
		return -1;
	}
	return 0;

}

static status_t hx_Reset(void)
{
    GPIO_PinWrite(BOARD_3DCAMERA_IR_WAK_GPIO, BOARD_3DCAMERA_IR_WAK_GPIO_PIN, 1);
	/* Hard reset */
    GPIO_PinWrite(BOARD_3DCAMERA_IR_RST_GPIO, BOARD_3DCAMERA_IR_RST_GPIO_PIN, 1);
    HIMAX_DelayMs(10);
	GPIO_PinWrite(BOARD_3DCAMERA_IR_RST_GPIO, BOARD_3DCAMERA_IR_RST_GPIO_PIN, 0);
	HIMAX_DelayMs(50);
	GPIO_PinWrite(BOARD_3DCAMERA_IR_RST_GPIO, BOARD_3DCAMERA_IR_RST_GPIO_PIN, 1);
	HIMAX_DelayMs(45);
	return kStatus_Success;
}


status_t hx_Init(void)
{
	hx_Reset();
    DVPMIPI_Init();
    hx_cam_ctrl_frame_mode_select(HX_FRAME_MODE_SELECT_ALT_NIR_DEPTH);

    return kStatus_Success;
}

status_t hx_Deinit(void)
{
    GPIO_PinWrite(BOARD_3DCAMERA_IR_WAK_GPIO, BOARD_3DCAMERA_IR_WAK_GPIO_PIN, 0);
	/* Hard reset */
	GPIO_PinWrite(BOARD_3DCAMERA_IR_RST_GPIO, BOARD_3DCAMERA_IR_RST_GPIO_PIN, 0);
    return kStatus_Success;
}

uint8_t hx_cam_ctrl_get_frame_type(const uint8_t *in_data, uint16_t width, uint16_t height)
{
	uint16_t *in_data_raw16 = (uint16_t *)in_data;
	uint8_t watermark = (in_data_raw16[width * height - 1] >> 8) & 0x07;
	int32_t alt_flag = FRAME_TYPE_ANY;

	// [DEBUG]
	LOGD("watermark: %d\r\n", watermark);

	switch (watermark) {
		case 0x01:
			alt_flag = FRAME_TYPE_IR;
			break;
		case 0x02:
			alt_flag = FRAME_TYPE_DEPTH;
			break;
		default:
			alt_flag = FRAME_TYPE_ANY;
			break;
	}

	return alt_flag;
}

