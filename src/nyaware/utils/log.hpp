#pragma once

#include <cstdio>

#define LOG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define LOGI(fmt, ...) printf("i | " fmt "\n", ##__VA_ARGS__)
#define LOGE(fmt, ...) printf("! | " fmt "\n", ##__VA_ARGS__)
#define LOGW(fmt, ...) printf("* | " fmt "\n", ##__VA_ARGS__)
#define LOGQ(fmt, ...) printf("? | " fmt "\n", ##__VA_ARGS__)
#ifdef _DEBUG
#define LOGD(fmt, ...) printf("D | " fmt "\n", ##__VA_ARGS__)
#else
#define LOGD()
#endif