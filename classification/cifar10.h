/*
*  cifar10.h
*  openimds
*
*  Created by spring on 2020. 3. 20...
*  Copyright 2020 spring. All rights reserved.
*
*/
#if !defined(OPENIMDS_7E2_1_1E_CIFAR10_H_INCLUDED)
#define OPENIMDS_7E2_1_1E_CIFAR10_H_INCLUDED

#include"utils.h"
#ifdef __cplusplus
namespace {
#endif
	static inline void __ReadCIFAR10(IMDSImage* imgs,char* image_file, int padding, bool bias, IMDSFormat format,int idx) {
		assert(image_file != NULL);
		assert(padding >= 0);
		FILE* fp = fopen(image_file, "rb");
		assert(fp != NULL);
		const int W = 32;
		const int H = 32;
		unsigned char row[3073];
		for (int i = 0; i < 10000; i++) {
			int local = 10000 * idx + i;
			fread(row, 1, 3073, fp);
			imgs->label[local] = (int)row[0];
			imgs->image[local] = imgs->images+(imgs->image_size * 10000 * idx + imgs->image_size*i);
			for (int y = 0; y < H; y++) {
				for (int x = 0; x < W; x++) {
					for (int c = 0; c < 3; c++) {//current: RRGGBB
						switch (format) {
						case RRGGBB: {
							imgs->image[local][(y + padding)*imgs->w + (x + padding) + c * imgs->w*imgs->h] = row[y*W + x + c * W*H + 1];
						}break;
						case BBGGRR: {
							imgs->image[local][(y + padding)*imgs->w + (x + padding) + c * imgs->w*imgs->h] = row[y*W + x + (2 - c) * W*H + 1];
						}break;
						case RGBRGB: {
							imgs->image[local][(y + padding)*imgs->w * 3 + (x + padding) * 3 + c] = row[y*W + x + c * W*H + 1];
						}break;
						case BGRBGR: {
							imgs->image[local][(y + padding)*imgs->w * 3 + (x + padding) * 3 + c] = row[y*W + x + (2 - c) * W*H + 1];
						}break;
						}
					}
				}
			}
		}
		fclose(fp);
	}
	static inline char* __DownloadCifar10(char* tmp_path, char* url_img, char* name_img, int file_size) {
#if defined(_WIN32) || defined(_WIN64)
		SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, tmp_path);
		strcat(tmp_path, "\\OpenIMDS\\");
		_mkdir(tmp_path);
		size_t len = strlen(tmp_path);
		assert(len != 0);
		char img_file[MAX_PATH + 1] = { 0 };
		strcat(strcpy(img_file, tmp_path), name_img);
		int size = 0;
		FILE* fp = fopen(img_file, "rb");
		if (fp) {
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			fclose(fp);
		}
		if (PathFileExistsA(img_file) == FALSE || size != file_size) {
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
		if (access(img_file, F_OK) != 0 || size != file_size) {
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
#ifdef __cplusplus
}
#endif
static inline IMDSImage GetCifar10TrainData(int padding _IMDSCPPDV(0), bool bias _IMDSCPPDV(true), IMDSFormat format _IMDSCPPDV(RRGGBB)) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	char name_img[MAX_PATH + 1] = { 0 };
	
	char* url[5] = {
		"https://github.com/springkim/OpenIMDS/releases/download/Cifar10/openimds_cifar10_train_1.bin"
		,"https://github.com/springkim/OpenIMDS/releases/download/Cifar10/openimds_cifar10_train_2.bin"
		,"https://github.com/springkim/OpenIMDS/releases/download/Cifar10/openimds_cifar10_train_3.bin"
		,"https://github.com/springkim/OpenIMDS/releases/download/Cifar10/openimds_cifar10_train_4.bin"
		,"https://github.com/springkim/OpenIMDS/releases/download/Cifar10/openimds_cifar10_train_5.bin"
	};
	IMDSImage imds;
	imds.c = 3;
	imds.w = 32 + padding * 2;
	imds.h = 32 + padding * 2;
	imds.n = 50000;
	imds.image = (float**)calloc(imds.n, sizeof(float*));
	imds.label = (int*)calloc(imds.n, sizeof(int));
	imds.image_size = (imds.w + padding * 2)*(imds.h + padding * 2) * 3 + bias;
	imds.images = (float*)calloc(imds.n*imds.image_size, sizeof(float));
	imds.format = format;
	imds.original = true;
	for (int i = 1; i <= 5; i++) {
		sprintf(name_img, "openimds_cifar10_train_%d.bin", i);
		__DownloadCifar10(tmp_path, url[i - 1], name_img, 30730000);
		char path_img[MAX_PATH + 1] = { 0 };
		strcat(strcpy(path_img, tmp_path), name_img);
		__ReadCIFAR10(&imds,path_img, padding, bias, format,i-1);
	}
	return imds;
}
static inline IMDSImage GetCifar10ValidData(int padding _IMDSCPPDV(0), bool bias _IMDSCPPDV(true), IMDSFormat format _IMDSCPPDV(RRGGBB)) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	char* name_img = "openimds_cifar10_test.bin";
	char* url = "https://github.com/springkim/OpenIMDS/releases/download/Cifar10/openimds_cifar10_test.bin";
	IMDSImage imds;
	imds.c = 3;
	imds.w = 32 + padding * 2;
	imds.h = 32 + padding * 2;
	imds.n = 10000;
	imds.image = (float**)calloc(imds.n, sizeof(float*));
	imds.label = (int*)calloc(imds.n, sizeof(int));
	imds.image_size = (imds.w + padding * 2)*(imds.h + padding * 2) * 3 + bias;
	imds.images = (float*)calloc(imds.n*imds.image_size, sizeof(float));
	imds.format = format;
	imds.original = true;
	__DownloadCifar10(tmp_path, url, name_img, 30730000);
	char path_img[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_img, tmp_path), name_img);
	__ReadCIFAR10(&imds,path_img, padding, bias, format,0);
	return imds;
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