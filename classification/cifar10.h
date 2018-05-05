/*
*  cifar10.h
*  OpenIMDS
*
*  Created by kimbomm on 2018. 1. 30...
*  Copyright 2018 Sogang CVIP. All rights reserved.
*
*/
#if !defined(OPENIMDS_7E2_1_1E_CIFAR10_H_INCLUDED)
#define OPENIMDS_7E2_1_1E_CIFAR10_H_INCLUDED
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
#ifndef _IMDSCPPDV	//IMDS c++ default value
#ifdef __cplusplus
#define _IMDSCPPDV(VALUE)	=(VALUE)		
#else
#define _IMDSCPPDV(VALUE)
#endif
#endif
#if !defined(_IMDSImage)
#define _IMDSImage
typedef struct IMDSImage IMDSImage;
struct IMDSImage {
	int w;
	int h;
	int c;
	int n;
	float** image;
	int* label;
};
static inline void ReleaseIMDSImage(IMDSImage* imgs) {
	if (imgs->image) {
		for (int i = 0; i < imgs->n; i++) {
			free(imgs->image[i]);
		}
		free(imgs->image);
		imgs->image = NULL;
	}
	if (imgs->label) {
		free(imgs->label);
		imgs->label = NULL;
	}
	imgs->n = imgs->w = imgs->h = imgs->c = 0;
}
#endif
static inline IMDSImage __ReadCIFAR10(char* image_file, int padding, float alpha,bool bias) {
	assert(image_file != NULL);
	assert(padding >= 0);
	FILE* fp = fopen(image_file, "rb");
	assert(fp != NULL);
	IMDSImage imgs;
	const int W = 32;
	const int H = 32;
	imgs.w = W + padding*2;
	imgs.h = H + padding*2;
	imgs.c = 3;
	imgs.n = 10000;
	imgs.label = (int*)calloc(imgs.n, sizeof(int));
	imgs.image = (float**)calloc(imgs.n, sizeof(float*));
	char row[3073];
	for (int i = 0; i < 10000; i++) {
		fread(row, 1, 3073, fp);
		imgs.label[i] = (int)row[0];
		imgs.image[i] = (float*)calloc((W + padding * 2)*(H + padding * 2)*3 + bias, sizeof(float));
		for (int y = 0; y < H; y++) {
			for (int x = 0; x < W; x++) {
				imgs.image[i][(y + padding)*imgs.w * 3 + (x + padding) * 3 + 0] = row[y*W + x + 2 * W*H + 1] * alpha;
				imgs.image[i][(y + padding)*imgs.w * 3 + (x + padding) * 3 + 1] = row[y*W + x + 1 * W*H + 1] * alpha;
				imgs.image[i][(y + padding)*imgs.w * 3 + (x + padding) * 3 + 2] = row[y*W + x + 0 * W*H + 1] * alpha;
			}
		}
	}
	return imgs;
}
static inline char* __DownloadCifar10(char* tmp_path, char* url_img, char* name_img,int file_size) {
#if defined(_WIN32) || defined(_WIN64)
	SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, tmp_path);
	strcat(tmp_path, "\\OpenIMDS\\");
	_mkdir(tmp_path);
	size_t len = strlen(tmp_path);
	assert(len != 0);
	char img_file[MAX_PATH + 1] = { 0 };
	strcat(strcpy(img_file, tmp_path), name_img);
	int size = 0;
	FILE* fp = fopen(img_file,"rb");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fclose(fp);
	}
	if (PathFileExistsA(img_file) == FALSE || size!=file_size) {
		HRESULT r = URLDownloadToFileA(NULL, url_img, img_file, 0, 0);
		if (r != S_OK) {
			fprintf(stderr, "CIFAR10 image Download failure!\n");
			exit(EXIT_FAILURE);
		}
	}
#elif defined(__linux__)
	strcpy(tmp_path, "/tmp/");
	char img_file[MAX_PATH + 1] = { 0 };
	strcat(strcpy(img_file, tmp_path), name_img);
	int size = 0;
	FILE* fp = fopen(img_file, "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fclose(fp);
	}
	if (access(img_file, F_OK) != 0 || size!=file_size) {
		char cmd[MAX_PATH + 1] = { 0 };
		sprintf(cmd, "wget -O %s \"%s\" >>/tmp/mnist.log 2>&1", img_file, url_img);
		if (system(cmd) != 0) {
			fprintf(stderr, "CIFAR10 image Download failure!\n");
			exit(EXIT_FAILURE);
		}
	}
#endif
	return tmp_path;
}
static inline IMDSImage GetCifar10TrainData(int padding _IMDSCPPDV(0), float alpha _IMDSCPPDV(1.0F), bool bias _IMDSCPPDV(true)) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	char name_img[MAX_PATH + 1] = { 0 };
	IMDSImage train;
	char* url[5] = {
		"https://www.dropbox.com/s/1u0hpkbbddn3hc1/openimds_cifar10_train_1.bin?dl=1"
		,"https://www.dropbox.com/s/mxk1zozw42cqk2a/openimds_cifar10_train_2.bin?dl=1"
		,"https://www.dropbox.com/s/cxg81q891j78wa9/openimds_cifar10_train_3.bin?dl=1"
		,"https://www.dropbox.com/s/tcnds6l7rba5tn7/openimds_cifar10_train_4.bin?dl=1"
		,"https://www.dropbox.com/s/u9500s411uocjgg/openimds_cifar10_train_5.bin?dl=1"
	};
	train.c = 3;
	train.w = 32 + padding * 2;
	train.h = 32 + padding * 2;
	train.n = 50000;
	train.image = (float**)calloc(train.n, sizeof(float*));
	train.label = (int*)calloc(train.n, sizeof(int));
	for (int i = 1; i <= 5; i++) {
		sprintf(name_img, "openimds_cifar10_train_%d.bin",i);
		__DownloadCifar10(tmp_path, url[i-1], name_img,30730000);
		char path_img[MAX_PATH + 1] = { 0 };
		strcat(strcpy(path_img, tmp_path), name_img);
		IMDSImage temp= __ReadCIFAR10(path_img, padding, alpha,bias);
		memcpy(train.image + (i - 1) * 10000, temp.image, temp.n * sizeof(float*));
		memcpy(train.label + (i - 1) * 10000, temp.label, temp.n * sizeof(int));
		free(temp.image);
		free(temp.label);
	}
	return train;
}
static inline IMDSImage GetCifar10ValidData(int padding _IMDSCPPDV(0), float alpha _IMDSCPPDV(1.0F), bool bias _IMDSCPPDV(true)) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	char* name_img = "openimds_cifar10_test.bin";
	char* url = "https://www.dropbox.com/s/uifxmb6hi0rmvr3/openimds_cifar10_test.bin?dl=1";
	__DownloadCifar10(tmp_path, url,name_img,30730000);
	char path_img[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_img, tmp_path), name_img);
	return __ReadCIFAR10(path_img, padding, alpha,bias);	
}
static inline char* GetCifar10Class(int index) {
	switch (index) {
		case 0:return "airplane";
		case 1:return "automobile";
		case 2:return "bird";
		case 3:return "cat";
		case 4:return "deer";
		case 5:return "dog";
		case 6:return "frog";
		case 7:return "horse";
		case 8:return "ship";
		case 9: return "truck";
		default: return "NULL";
	}
}
#endif  //OPENIMDS_7E2_1_1E_CIFAR10_H_INCLUDED