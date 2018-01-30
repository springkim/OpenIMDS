/*
*  MNIST.h
*  MNIST
*
*  Created by kimbomm on 2018. 01. 28...
*  Copyright 2018 Sogang Univ. All rights reserved.
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
#if defined(_WIN32) || defined(_WIN64)
#include<Windows.h>
#include<Shlwapi.h>
#include<urlmon.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib,"urlmon.lib")
#pragma warning(disable:4996)
#elif defined(__linux__)
#include<unistd.h>
#include<linux/limits.h>
#define MAX_PATH PATH_MAX
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
static inline IMDSImage __ReadMNIST(char* image_file, char* label_file, int padding, float alpha) {
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
		imgs.image[n] = (float*)calloc((rows + padding * 2)*(cols + padding * 2), sizeof(float));
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
static inline char* __DownloadMNIST(char* tmp_path, char* url_img, char* url_lbl, char* name_img, char* name_lbl) {
#if defined(_WIN32) || defined(_WIN64)
	int len = GetTempPathA(MAX_PATH, tmp_path);
	assert(len != 0);
	char img_file[MAX_PATH + 1] = { 0 };
	char lbl_file[MAX_PATH + 1] = { 0 };
	strcat(strcpy(img_file, tmp_path), name_img);
	strcat(strcpy(lbl_file, tmp_path), name_lbl);
	if (PathFileExistsA(img_file) == FALSE) {
		HRESULT r = URLDownloadToFileA(NULL, url_img, img_file, 0, 0);
		if (r != S_OK) {
			fprintf(stderr, "MNIST image Download failure!\n");
			exit(EXIT_FAILURE);
		}
	}
	if (PathFileExistsA(lbl_file) == FALSE) {
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
	if (access(img_file, F_OK) != 0) {
		char cmd[MAX_PATH + 1] = { 0 };
		sprintf(cmd, "wget -O %s \"%s\" >>/tmp/mnist.log 2>&1", img_file, url_img);
		if (system(cmd) != 0) {
			fprintf(stderr, "MNIST image Download failure!\n");
			exit(EXIT_FAILURE);
		}
	}
	if (access(lbl_file, F_OK) != 0) {
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
static inline IMDSImage GetMnistTrainData(int padding, float alpha) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	char* name_img = "train-images.idx3-ubyte";
	char* name_lbl = "train-labels.idx1-ubyte";
	__DownloadMNIST(tmp_path, "https://www.dropbox.com/s/f56cag59pk58lel/train-images.idx3-ubyte?dl=1"
					, "https://www.dropbox.com/s/r1rgfhshebdhnms/train-labels.idx1-ubyte?dl=1"
					, name_img, name_lbl);
	char path_img[MAX_PATH + 1] = { 0 };
	char path_lbl[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_img, tmp_path), name_img);
	strcat(strcpy(path_lbl, tmp_path), name_lbl);
	return __ReadMNIST(path_img, path_lbl, padding, alpha);
}
static inline IMDSImage GetMnistValidData(int padding, float alpha) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	char* name_img = "t10k-images.idx3-ubyte";
	char* name_lbl = "t10k-labels.idx1-ubyte";
	__DownloadMNIST(tmp_path, "https://www.dropbox.com/s/zsf4wicmmru34s9/t10k-images.idx3-ubyte?dl=1"
					, "https://www.dropbox.com/s/wsj7xetwmwuhxj8/t10k-labels.idx1-ubyte?dl=1"
					, name_img, name_lbl);
	char path_img[MAX_PATH + 1] = { 0 };
	char path_lbl[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_img, tmp_path), name_img);
	strcat(strcpy(path_lbl, tmp_path), name_lbl);
	return __ReadMNIST(path_img, path_lbl, padding, alpha);
}
#endif  //MNIST_7E2_1_1C_MNIST_HPP_INCLUDED