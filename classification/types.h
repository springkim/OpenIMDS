/*
*  types.h
*  openimds
*
*  Created by spring on 2020. 3. 20...
*  Copyright 2020 spring. All rights reserved.
*
*/
#if !defined(OPENIMDS_7E4_3_14_TYPES_H_INCLUDED)
#define OPENIMDS_7E4_3_14_TYPES_H_INCLUDED
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#ifndef __cplusplus
#include<stdbool.h>
#endif
#if defined(_WIN32) || defined(_WIN64)
#include<Windows.h>
#include<direct.h>
#include<Shlwapi.h>
#include<ShlObj.h>
#include<urlmon.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib,"urlmon.lib")
#pragma warning(disable:4996)
#elif defined(__linux__)
#include<unistd.h>
#include<linux/limits.h>
#define MAX_PATH PATH_MAX
#endif


#define _IMDSImage
typedef enum IMDSFormat IMDSFormat;
enum IMDSFormat {
	RRGGBB = 0,
	BBGGRR,
	RGBRGB,
	BGRBGR
};

typedef struct IMDSImage IMDSImage;
struct IMDSImage {
	int w;
	int h;
	int c;
	int n;
	float** image;
	int* label;
	IMDSFormat format;
	bool original;
};

#endif  //OPENIMDS_7E4_3_14_TYPES_H_INCLUDED