/*
*  mnist.h
*  openimds
*
*  Created by spring on 2020. 3. 20...
*  Copyright 2020 spring. All rights reserved.
*
*/
#if !defined(MNIST_7E2_1_1C_MNIST_H_INCLUDED)
#define MNIST_7E2_1_1C_MNIST_H_INCLUDED
#include"utils.h"

#ifdef __cplusplus
namespace {
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
	static inline IMDSImage __ReadMNIST(char* image_file, char* label_file, int padding, bool bias, IMDSFormat format) {
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
		int image_size = (rows + padding * 2)*(cols + padding * 2) + bias;
		imgs.image_size = image_size;
		imgs.images = (float*)calloc(num_of_images*image_size, sizeof(float));
		imgs.label = (int*)calloc(num_of_images, sizeof(int));
		imgs.format = format;
		imgs.original = true;
		unsigned char* p = (unsigned char*)calloc(rows*cols, sizeof(unsigned char));
		for (int n = 0; n < num_of_images; n++) {
			imgs.image[n] = imgs.images + image_size * n;
			unsigned char label;
			fread(&label, sizeof(unsigned char), 1, fp_label);
			imgs.label[n] = label;
			fread(p, 1, rows*cols, fp_image);
			for (int y = 0; y < rows; y++) {
				for (int x = 0; x < cols; x++) {
					imgs.image[n][(y + padding)*(cols + padding * 2) + (x + padding)] = (float)p[y*cols + x];
				}
			}
		}
		free(p);
		fclose(fp_image);
		fclose(fp_label);
		return imgs;
	}
	static inline char* __DownloadMNIST(char* tmp_path, char* url_img, char* url_lbl, char* name_img, char* name_lbl, int img_size, int lbl_size) {
#if defined(_WIN32) || defined(_WIN64)
		SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, tmp_path);
		strcat(tmp_path, "\\OpenIMDS\\");
		_mkdir(tmp_path);
		size_t len = strlen(tmp_path);
		assert(len != 0);
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
		if (PathFileExistsA(img_file) == FALSE || img_file_size != img_size) {
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
		if (PathFileExistsA(lbl_file) == FALSE || lbl_file_size != lbl_size) {
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
		if (access(img_file, F_OK) != 0 || img_file_size != img_size) {
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
		if (access(lbl_file, F_OK) != 0 || lbl_file_size != lbl_size) {
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
#ifdef __cplusplus
}
#endif
static inline IMDSImage GetMnistTrainData(int padding _IMDSCPPDV(0), bool bias _IMDSCPPDV(true), IMDSFormat format _IMDSCPPDV(RRGGBB)) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	char* name_img = "openimds_mnist_train_image.bin";
	char* name_lbl = "openimds_mnist_train_label.bin";
	__DownloadMNIST(tmp_path, "https://github.com/springkim/OpenIMDS/releases/download/MNIST/openimds_mnist_train_image.bin"
		, "https://github.com/springkim/OpenIMDS/releases/download/MNIST/openimds_mnist_train_label.bin"
		, name_img, name_lbl, 47040016, 60008);
	char path_img[MAX_PATH + 1] = { 0 };
	char path_lbl[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_img, tmp_path), name_img);
	strcat(strcpy(path_lbl, tmp_path), name_lbl);
	return __ReadMNIST(path_img, path_lbl, padding, bias, format);
}
static inline IMDSImage GetMnistValidData(int padding _IMDSCPPDV(0), bool bias _IMDSCPPDV(true), IMDSFormat format _IMDSCPPDV(RRGGBB)) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	char* name_img = "openimds_mnist_valid_image.bin";
	char* name_lbl = "openimds_mnist_valid_label.bin";
	__DownloadMNIST(tmp_path, "https://github.com/springkim/OpenIMDS/releases/download/MNIST/openimds_mnist_valid_image.bin"
		, "https://github.com/springkim/OpenIMDS/releases/download/MNIST/openimds_mnist_valid_label.bin"
		, name_img, name_lbl, 7840016, 10008);
	char path_img[MAX_PATH + 1] = { 0 };
	char path_lbl[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_img, tmp_path), name_img);
	strcat(strcpy(path_lbl, tmp_path), name_lbl);
	return __ReadMNIST(path_img, path_lbl, padding, bias, format);
}
static inline IMDSImage GetFashionTrainData(int padding _IMDSCPPDV(0), bool bias _IMDSCPPDV(true), IMDSFormat format _IMDSCPPDV(RRGGBB)) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	char* name_img = "openimds_fashion_train_image.bin";
	char* name_lbl = "openimds_fashion_train_label.bin";
	__DownloadMNIST(tmp_path, "https://github.com/springkim/OpenIMDS/releases/download/fashion/train-images-idx3-ubyte"
		, "https://github.com/springkim/OpenIMDS/releases/download/fashion/train-labels-idx1-ubyte"
		, name_img, name_lbl, 47040016, 60008);
	char path_img[MAX_PATH + 1] = { 0 };
	char path_lbl[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_img, tmp_path), name_img);
	strcat(strcpy(path_lbl, tmp_path), name_lbl);
	return __ReadMNIST(path_img, path_lbl, padding, bias, format);
}
static inline IMDSImage GetFashionValidData(int padding _IMDSCPPDV(0), bool bias _IMDSCPPDV(true), IMDSFormat format _IMDSCPPDV(RRGGBB)) {
	char tmp_path[MAX_PATH + 1] = { 0 };
	char* name_img = "openimds_fashion_valid_image.bin";
	char* name_lbl = "openimds_fashion_valid_label.bin";
	__DownloadMNIST(tmp_path, "https://github.com/springkim/OpenIMDS/releases/download/fashion/t10k-images-idx3-ubyte"
		, "https://github.com/springkim/OpenIMDS/releases/download/fashion/t10k-labels-idx1-ubyte"
		, name_img, name_lbl, 7840016, 10008);
	char path_img[MAX_PATH + 1] = { 0 };
	char path_lbl[MAX_PATH + 1] = { 0 };
	strcat(strcpy(path_img, tmp_path), name_img);
	strcat(strcpy(path_lbl, tmp_path), name_lbl);
	return __ReadMNIST(path_img, path_lbl, padding, bias, format);
}
#endif  //MNIST_7E2_1_1C_MNIST_HPP_INCLUDED
