#include "capture.h"
#include "sln_shell.h"

#if 1 //CAPTURE_ENABLE
#include "fsl_log.h"

// loadbin /home/zds/capture.clip 0x81000000 0xF99C00
// savebin /home/felixpan/capture.clip 0x81000000 0xDF9800
#define SDRAM_START 0x80000000
// 16 MB
#define CAPTURE_OFFSET (16 * 1024 * 1024)
// 0x81000000
#define RESERVE_ADDR (SDRAM_START + CAPTURE_OFFSET)
// 14 MB
#define RESERVE_SIZE  0xe00000//0x1000000

// Save to BGR clip format
#define DUMP_FRAME_SIZE (SNAPSHOT_WIDTH * SNAPSHOT_HEIGHT * 3)
#define DUMP_MAX_FRAMES (RESERVE_SIZE / DUMP_FRAME_SIZE)  //530

static int gCaptureIndex = 0;

int capture_init()
{
   gCaptureIndex = 0;
   return 0;
}

int capture_frame(unsigned char* pFrame)
{
    if (gCaptureIndex < DUMP_MAX_FRAMES) {
        unsigned char* pStart = (unsigned char*)RESERVE_ADDR + DUMP_FRAME_SIZE * gCaptureIndex;

        for (int i = 0 ; i < SNAPSHOT_HEIGHT; i++) {
            for (int j = 0; j < SNAPSHOT_WIDTH; j++) {
                pStart[(i * SNAPSHOT_WIDTH + j) * 3] = pFrame[(i * SNAPSHOT_WIDTH + j) * 3];
                pStart[(i * SNAPSHOT_WIDTH + j) * 3 + 1] = pFrame[(i * SNAPSHOT_WIDTH + j) * 3 + 1];
                pStart[(i * SNAPSHOT_WIDTH + j) * 3 + 2] = pFrame[(i * SNAPSHOT_WIDTH + j) * 3 + 2];
            }
        }

        UsbShell_Printf("[C:%d]\r\n", gCaptureIndex);
        gCaptureIndex++;
    }

    return 0;
}

#else

int capture_init()
{
	return 0;
}

int capture_frame(unsigned char* pFrame)
{
    return 0;
}

#endif

#if CAPTURE_REGISTRATION_ENABLE
#include "fsl_log.h"

// savebin /home/zds/capture.clip 0x80A00000 0xEF1000
// savebin /home/zds/feature.clip 0x81F00000 0x19000

#define SDRAM_START 0x80000000

// 10 MB
#define CAPTURE_OFFSET (10* 1024 * 1024)
// 21M
#define RESERVE_SIZE  0x1500000

// 0x81000000
#define RESERVE_ADDR (SDRAM_START + CAPTURE_OFFSET)

// Save to BGR clip format
#define DUMP_FRAME_SIZE (APP_FOCUS_WIDTH * APP_FOCUS_HEIGHT * 3)
#define DUMP_MAX_FRAMES (RESERVE_SIZE / DUMP_FRAME_SIZE)
#define DUMP_RECOG_FRAME_OFFSET 20

static int gCaptureIndex = 0;  // recognition dump index
static int gCaptureRecIndex = DUMP_RECOG_FRAME_OFFSET; // registration dump index

//
// BGRA88 TO BGR888
//
int capture_rframe(unsigned char* pFrame, int rec)
{
    int index = 0;

    if (rec) {
        if (gCaptureRecIndex >= DUMP_MAX_FRAMES) {
            gCaptureRecIndex = DUMP_RECOG_FRAME_OFFSET;
            index = gCaptureRecIndex;
        } else {
            index = gCaptureRecIndex;
            gCaptureRecIndex++;
        }
    } else {
        if (gCaptureIndex >= DUMP_RECOG_FRAME_OFFSET) {
            gCaptureIndex = 0;
            index = gCaptureIndex;
        } else {
            index = gCaptureIndex;
            gCaptureIndex++;
        }
    }

    unsigned char* pStart = (unsigned char*)RESERVE_ADDR + DUMP_FRAME_SIZE * index;

    for (int i = 0 ; i < APP_FOCUS_HEIGHT; i++) {
        for (int j = 0; j < APP_FOCUS_WIDTH; j++) {
            pStart[(i * APP_FOCUS_WIDTH + j) * 3 + 2] = pFrame[(i * APP_FOCUS_WIDTH + j) * 3];
            pStart[(i * APP_FOCUS_WIDTH + j) * 3 + 1] = pFrame[(i * APP_FOCUS_WIDTH + j) * 3 + 1];
            pStart[(i * APP_FOCUS_WIDTH + j) * 3] = pFrame[(i * APP_FOCUS_WIDTH + j) * 3 + 2];
        }
    }

    LOGD("[C:%d/%d]\r\n", index, DUMP_MAX_FRAMES);

    return 0;
}


#define FEATUREDATA_MAX_COUNT 100
#define FEATUREDATA_NAME_MAX_LEN 8
#define FEATUREDATA_FEATURE_SIZE 128

/********************************************************
 * item        |  address   |  size/sector |
 * FeatureMap  | 0x61800000 |      1       |
 * FeatureItem | 0x61840000 |      1       |
 * Snapshot    | 0x61880000 |      .       |
 */

typedef struct {
    char name[FEATUREDATA_NAME_MAX_LEN];
    float feature[FEATUREDATA_FEATURE_SIZE];
} FeatureItem;//1kB

// 31 MBytes
#define CAPTURE_FEATURE_OFFSET (31* 1024 * 1024)

// 0x81000000
#define RESERVE_FEATURE_ADDR (SDRAM_START + CAPTURE_FEATURE_OFFSET)
#define RESERVE_FEATURE_SIZE  0x19000
#define DUMP_FEATURE_SIZE (FEATUREDATA_FEATURE_SIZE * 4 + FEATUREDATA_NAME_MAX_LEN)

static int gCaptureFIndex = 0;  // recognition dump index
static int gCaptureFRecIndex = DUMP_RECOG_FRAME_OFFSET; // registration dump index

int capture_rfeature(std::string name, std::vector<float> feature, int rec)
{
    //LOGD("[F]>>\r\n");
    int index = 0;

    if (rec) {
        if (gCaptureFRecIndex >= DUMP_MAX_FRAMES) {
            gCaptureFRecIndex = DUMP_RECOG_FRAME_OFFSET;
            index = gCaptureFRecIndex;
        } else {
            index = gCaptureFRecIndex;
            gCaptureFRecIndex++;
        }
    } else {
        if (gCaptureFIndex >= DUMP_RECOG_FRAME_OFFSET) {
            gCaptureFIndex = 0;
            index = gCaptureFIndex;
        } else {
            index = gCaptureFIndex;
            gCaptureFIndex++;
        }
    }

    unsigned char* pName = (unsigned char*)RESERVE_FEATURE_ADDR + DUMP_FEATURE_SIZE * index;
    float* pFeature = (float*)(pName + FEATUREDATA_NAME_MAX_LEN);

    // name
    int len = name.length();
    const char* psName = name.c_str();

    if (len > FEATUREDATA_NAME_MAX_LEN) {
        len = FEATUREDATA_NAME_MAX_LEN;
    }

    for (int i = 0 ; i < len; i++) {
        pName[i] = psName[i];
    }

    for (int i = 0 ; i < FEATUREDATA_FEATURE_SIZE; i++) {
        pFeature[i] = feature[i];
    }

    LOGD("[F:%d/%d %c: %x]\r\n", index, DUMP_MAX_FRAMES, pName[0], *((unsigned int*)&pFeature[0]));

    return 0;
}

#else
int capture_rframe(unsigned char* pFrame, int rec)
{
    return 0;
}

int capture_rfeature(std::string name, std::vector<float> feature, int rec)
{
    return 0;
}
#endif
