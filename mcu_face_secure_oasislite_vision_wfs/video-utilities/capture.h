#ifndef _CAPTURE_H_
#define _CAPTURE_H_

#include "commondef.h"

#include <vector>
#include <string>
int capture_init();
int capture_frame(unsigned char* pFrame);
int capture_rframe(unsigned char* pFrame, int rec);
int capture_rfeature(std::string name, std::vector<float> feature, int rec);

#endif /*_CAPTURE_H_*/
