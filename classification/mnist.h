/*
*  MNIST.h
*  OpenIMDS
*
*  Created by kimbomm on 2018. 01. 28...
*  Copyright 2018 Sogang CVIP. All rights reserved.
*
*	You can run directly on Visual Studio. But you need below option on MinGW64.
*	-lshlwapi -lurlmon
*  Also can run directly on Linux gcc without any options.
*/
#if !defined(MNIST_7E2_1_1C_MNIST_H_INCLUDED)
#define MNIST_7E2_1_1C_MNIST_H_INCLUDED
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
IMDSImage GetRandomMiniBatch(IMDSImage imds, int batch_size) {
	IMDSImage mini_batch;
	mini_batch.w = imds.w;
	mini_batch.h = imds.h;
	mini_batch.c = imds.c;
	mini_batch.n = batch_size;
	mini_batch.image = (float**)malloc(batch_size*sizeof(float*));
	mini_batch.label = (int*)malloc(batch_size*sizeof(int));
	for (int i = 0; i<batch_size; i++) {
		int idx = rand()%imds.n;
		mini_batch.image[i] = imds.image[idx];
		mini_batch.label[i] = imds.label[idx];
	}
	return mini_batch;
}
#endif
static inline void __EndianChange(void *vp, size_t sz) {
	char *begin = (char *)vp;
	char *end = (char *)vp + sz - 1;
	while (begin < end) {
		*begin ^= *end;
		*end ^= *begin;
		*begin ^= *end;
		begin++;
		end--;
	}
}
static inline IMDSImage __ReadMNIST(char* image_file, char* label_file, int padding, float alpha,bool bias) {
	assert(image_file != NULL);
	assert(label_file != NULL);
	assert(padding >= 0);
	IMDSImage imgs;
	int magic_number, num_of_images, rows, cols;
	FILE* fp_image = fopen(image_file, "rb");
	assert(fp_image != NULL);
	fread(&magic_number, sizeof(int), 1, fp_image); __EndianChange(&magic_number, sizeof(int));
	assert(magic_number == 2051);
	fread(&num_of_images, sizeof(int), 1, fp_image); __EndianChange(&num_of_images, sizeof(int));
	fread(&rows, sizeof(int), 1, fp_image); __EndianChange(&rows, sizeof(int));
	fread(&cols, sizeof(int), 1, fp_image); __EndianChange(&cols, sizeof(int));
	FILE* fp_label = fopen(label_file, "rb");
	int num_of_items;
	assert(fp_label != NULL);
	fread(&magic_number, sizeof(int), 1, fp_label); __EndianChange(&magic_number, sizeof(int));
	assert(magic_number == 2049);
	fread(&num_of_items, sizeof(int), 1, fp_label); __EndianChange(&num_of_items, sizeof(int));
	assert(num_of_items == num_of_images);
	imgs.w = cols;
	imgs.h = rows;
	imgs.c = 1;
	imgs.n = num_of_images;
	imgs.image = (float**)calloc(num_of_images, sizeof(float*));
	imgs.label = (int*)calloc(num_of_images, sizeof(int));
	unsigned char* p = (unsigned char*)calloc(rows*cols, sizeof(unsigned char));
	for (int n = 0; n < num_of_images; n++) {
		imgs.image[n] = (float*)calloc((rows + padding * 2)*(cols + padding * 2) + bias, sizeof(float));
		unsigned char label;
		fread(&label, sizeof(unsigned char), 1, fp_label);
		imgs.label[n] = label;
		fread(p, 1, rows*cols, fp_image);
		for (int y = 0; y < rows; y++) {
			for (int x = 0; x < cols; x++) {
				imgs.image[n][(y + padding)*(cols + padding * 2) + (x + padding)] = (float)p[y*cols + x] * alpha;
			}
		}
	}
	free(p);
	fclose(fp_image);
	fclose(fp_label);
	return imgs;
}
static inline char* __DownloadMNIST(char* tmp_path, char* url_img, char* url_lbl, char* name_img, char* name_lbl,int img_size,int lbl_size) {
#if defined(_WIN32) || defined(_WIN64)
	SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, tmp_path);
	strcat(tmp_path,"\\OpenIMDS\\");
	_mkdir(tmp_path);
	size_t len=strlen(tmp_path);
	assert(len != 0);
	char img_file[MAX_PATH + 1] = { 0 };
	char lbl_file[MAX_PATH + 1] = { 0 };
	strcat(strcpy(img_file, tmp_path), name_img);
	strcat(strcpy(lbl_file, tmp_path), name_lbl);
	int img_file_size = 0;
	FILE* fp = fopen(img_file, "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		img_file_size=ftell(fp);
		fclose(fp);
	}
	if (PathFileExistsA(img_file) == FALSE || img_file_size!= img_size) {
		HRESULT r = URLDownloadToFileA(NULL, url_img, img_file, 0, 0);
		if (r != S_OK) {
			fprintf(stderr, "MNIST image Download failure!\n");
			exit(EXIT_FAILURE);
		}
	}
	int lbl_file_size = 0;
	fp = fopen(lbl_file, "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		lbl_file_size = ftell(fp);
		fclose(fp);
	}
	if (PathFileExistsA(lbl_file) == FALSE || lbl_file_size!=lbl_size) {
		HRESULT r = URLDownloadToFileA(NULL, url_lbl, lbl_file, 0, 0);
		if (r != S_OK) {
			fprintf(stderr, "MNIST label Download failure!\n");
			exit(EXIT_FAILURE);
		}
	}
#elif defined(__linux__)
	strcpy(tmp_path, "/tmp/");
	char img_file[MAX_PATH + 1] = { 0 };
	char lbl_file[MAX_PATH + 1] = { 0 };
	strcat(strcpy(img_file, tmp_path), name_img);
	strcat(strcpy(lbl_file, tmp_path), name_lbl);
	int img_file_size = 0;
	FILE* fp = fopen(img_file, "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		img_file_size = ftell(fp);
		fclose(fp);
	}
	if (access(img_file, F_OK) != 0 || img_file_size!=img_size) {
		char cmd[MAX_PATH + 1] = { 0 };
		sprintf(cmd, "wget -O %s \"%s\" >>/tmp/mnist.log 2>&1", img_file, url_img);
		if (system(cmd) != 0) {
			fprintf(stderr, "MNIST image Download failure!\n");
			exit(EXIT_FAILURE);
		}
	}
	int lbl_file_size = 0;
	fp = fopen(lbl_file, "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		lbl_file_size = ftell(fp);
		fclose(fp);
	}
	if (access(lbl_file, F_OK) != 0 || lbl_file_size!=lbl_size) {
		char cmd[MAX_PATH + 1] = { 0 };
		sprintf(cmd, "wget -O %s \"%s\" >>/tmp/mnist.log 2>&1", lbl_file, url_lbl);
		if (system(cmd) != 0) {
			fprintf(stderr, "MNIST label Download failure!\n");
			exit(EXIT_FAILURE);
		}
	}
#endif
	return tmp_path;
}
static inline IMDSImage GetMnistTrainData(int padding _IMDSCPPDV(0), float alpha _IMDSCPPDV(1.0F),bool bias _IMDSCPPDV(true)) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	char* name_img = "openimds_mnist_train_image.bin";
	char* name_lbl = "openimds_mnist_train_label.bin";
	__DownloadMNIST(tmp_path, "https://www.dropbox.com/s/f56cag59pk58lel/openimds_mnist_train_image.bin?dl=1"
					, "https://www.dropbox.com/s/r1rgfhshebdhnms/openimds_mnist_train_label.bin?dl=1"
					, name_img, name_lbl, 47040016,60008);
	char path_img[MAX_PATH + 1] = { 0 };
	char path_lbl[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_img, tmp_path), name_img);
	strcat(strcpy(path_lbl, tmp_path), name_lbl);
	return __ReadMNIST(path_img, path_lbl, padding, alpha,bias);
}
static inline IMDSImage GetMnistValidData(int padding _IMDSCPPDV(0), float alpha _IMDSCPPDV(1.0F), bool bias _IMDSCPPDV(true)) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	char* name_img = "openimds_mnist_valid_image.bin";
	char* name_lbl = "openimds_mnist_valid_label.bin";
	__DownloadMNIST(tmp_path, "https://www.dropbox.com/s/zsf4wicmmru34s9/openimds_mnist_valid_image.bin?dl=1"
					, "https://www.dropbox.com/s/wsj7xetwmwuhxj8/openimds_mnist_valid_label.bin?dl=1"
					, name_img, name_lbl,7840016,10008);
	char path_img[MAX_PATH + 1] = { 0 };
	char path_lbl[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_img, tmp_path), name_img);
	strcat(strcpy(path_lbl, tmp_path), name_lbl);
	return __ReadMNIST(path_img, path_lbl, padding, alpha,bias);
}
#endif  //MNIST_7E2_1_1C_MNIST_HPP_INCLUDED
