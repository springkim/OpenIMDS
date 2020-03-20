/*
*  stl10.h
*  OpenIMDS
*
*  Created by kimbomm on 2018. 09. 12...
*  Copyright 2018 kimbomm. All rights reserved.
*
*/
#if !defined(OPENIMDS_7E2_1_1E_STL10_H_INCLUDED)
#define OPENIMDS_7E2_1_1E_STL10_H_INCLUDED
#include"utils.h"
#ifdef __cplusplus
namespace {
#endif
	static inline IMDSImage __ReadSTL10(char* image_file, char* label_file, int padding, bool bias, int N,IMDSFormat format) {
		assert(image_file != NULL);
		assert(label_file != NULL);
		assert(padding >= 0);
		FILE* fp = fopen(image_file, "rb");
		FILE* fp2 = fopen(label_file, "rb");
		assert(fp != NULL);
		assert(fp2 != NULL);
		IMDSImage imgs;
		const int W = 96;
		const int H = 96;
		imgs.w = W + padding * 2;
		imgs.h = H + padding * 2;
		imgs.c = 3;
		imgs.n = N;
		imgs.label = (int*)calloc(imgs.n, sizeof(int));
		imgs.image = (float**)calloc(imgs.n, sizeof(float*));
		imgs.format = format;
		imgs.original = true;
		unsigned char* lbl = (unsigned char*)calloc(N, sizeof(unsigned char));
		fread(lbl, 1, N, fp2);
		fclose(fp2);

		unsigned char row[96 * 96 * 3];
		for (int i = 0; i < N; i++) {
			fread(row, 1, 96 * 96 * 3, fp);	//RRGGBB
			imgs.label[i] = (int)lbl[i] - 1;
			imgs.image[i] = (float*)calloc((W + padding * 2)*(H + padding * 2) * 3 + bias, sizeof(float));
			for (int y = 0; y < H; y++) {
				for (int x = 0; x < W; x++) {
					for (int c = 0; c < 3; c++) {//current: RRGGBB
						switch (format) {
						case RRGGBB: {
							imgs.image[i][(y + padding)*imgs.w + (x + padding) + c * imgs.w*imgs.h] = row[x*W + y + c * W*H];
						}break;
						case BBGGRR: {
							imgs.image[i][(y + padding)*imgs.w + (x + padding) + c * imgs.w*imgs.h] = row[x*W + y + (2 - c) * W*H];
						}break;
						case RGBRGB: {
							imgs.image[i][(y + padding)*imgs.w * 3 + (x + padding) * 3 + c] = row[x*W + y + c * W*H];
						}break;
						case BGRBGR: {
							imgs.image[i][(y + padding)*imgs.w * 3 + (x + padding) * 3 + c] = row[x*W + y + (2-c) * W*H];
						}break;
						}
					}
				}
			}
		}
		free(lbl);
		fclose(fp);
		return imgs;
	}
	static inline char* __DownloadSTL10(char* tmp_path, char* url_img, char* name_img, int file_size) {
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
				fprintf(stderr, "STL10 image Download failure!\n");
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
				fprintf(stderr, "STL10 image Download failure!\n");
				exit(EXIT_FAILURE);
			}
		}
#endif
		return tmp_path;
	}
#ifdef __cplusplus
}
#endif
static inline IMDSImage GetSTL10TrainData(int padding _IMDSCPPDV(0), bool bias _IMDSCPPDV(true), IMDSFormat format _IMDSCPPDV(RRGGBB)) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	IMDSImage train;
	char* url[] = {
		"https://github.com/springkim/OpenIMDS/releases/download/STL10/train_X.bin"
		,"https://github.com/springkim/OpenIMDS/releases/download/STL10/train_y.bin"
	};
	train.c = 3;
	train.w = 96 + padding * 2;
	train.h = 96 + padding * 2;
	train.n = 5000;
	train.image = (float**)calloc(train.n, sizeof(float*));
	train.label = (int*)calloc(train.n, sizeof(int));
	char* name_img = "openimds_stl10_train_image.bin";
	char* name_lbl = "openimds_stl10_train_label.bin";
	__DownloadSTL10(tmp_path, url[0],name_img , 138240000);
	__DownloadSTL10(tmp_path, url[1], name_lbl, 5000);

	char path_img[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_img, tmp_path), name_img);
	char path_lbl[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_lbl, tmp_path), name_lbl);
	return __ReadSTL10(path_img, path_lbl, padding, bias, train.n,format);
}
static inline IMDSImage GetSTL10ValidData(int padding _IMDSCPPDV(0), bool bias _IMDSCPPDV(true), IMDSFormat format _IMDSCPPDV(RRGGBB)) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	IMDSImage valid;
	char* url[] = {
		"https://github.com/springkim/OpenIMDS/releases/download/STL10/test_X.bin"
		,"https://github.com/springkim/OpenIMDS/releases/download/STL10/test_y.bin"
	};
	valid.c = 3;
	valid.w = 96 + padding * 2;
	valid.h = 96 + padding * 2;
	valid.n = 8000;
	valid.image = (float**)calloc(valid.n, sizeof(float*));
	valid.label = (int*)calloc(valid.n, sizeof(int));
	char* name_img = "openimds_stl10_valid_image.bin";
	char* name_lbl = "openimds_stl10_valid_label.bin";
	__DownloadSTL10(tmp_path, url[0], name_img, 221184000);
	__DownloadSTL10(tmp_path, url[1], name_lbl, 8000);

	char path_img[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_img, tmp_path), name_img);
	char path_lbl[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_lbl, tmp_path), name_lbl);
	return __ReadSTL10(path_img, path_lbl, padding, bias, valid.n,format);
}
static inline char* GetSTL10Class(int index) {
	switch (index) {
		case 0:return "airplane";
		case 1:return "bird";
		case 2:return "car";
		case 3:return "cat";
		case 4:return "deer";
		case 5:return "dog";
		case 6:return "horse";
		case 7:return "monkey";
		case 8:return "ship";
		case 9: return "truck";
		default: return "NULL";
	}
}
#endif  //OPENIMDS_7E2_1_1E_CIFAR10_H_INCLUDED