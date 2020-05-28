/*
*  utils.h
*  openimds
*
*  Created by spring on 2020. 3. 20...
*  Copyright 2020 spring. All rights reserved.
*
*/
#if !defined(OPENIMDS_7E4_3_14_UTILS_H_INCLUDED)
#define OPENIMDS_7E4_3_14_UTILS_H_INCLUDED
#include"types.h"
#ifndef _IMDSCPPDV	//IMDS c++ default value
#ifdef __cplusplus
#define _IMDSCPPDV(VALUE)	=(VALUE)
#else
#define _IMDSCPPDV(VALUE)
#endif
#endif

static inline void ReleaseIMDSImage(IMDSImage* imgs) {
	free(imgs->images);
	if (imgs->image) {

			free(imgs->image);
			imgs->image = NULL;
		
	}
	if (imgs->label) {
		free(imgs->label);
		imgs->label = NULL;
	}
	imgs->n = imgs->w = imgs->h = imgs->c = 0;
}
static inline int* GetRandomMiniBatchSchedule(IMDSImage imds, int batch_size) {
	int over =batch_size -  imds.n%batch_size;
	int* indexes = (int*)calloc(imds.n + over, sizeof(int));
	for (int i = 0; i < imds.n; i++)
		indexes[i] = i;
	for (int i = 0; i < imds.n; i++) {
		int r = rand() % imds.n;
		int t = indexes[i];
		indexes[i] = indexes[r];
		indexes[r] = t;
	}
	for (int i = imds.n; i < imds.n + over; i++) {
		indexes[i] = rand() % imds.n;
	}
	return indexes;
}
static inline int GetRandomMiniBatchIter(IMDSImage imds, int batch_size) {
	return (int)ceil((float)imds.n / batch_size);
}
static inline IMDSImage GetRandomMiniBatch(IMDSImage imds, int batch_size,int** schedule) {
	IMDSImage mini_batch;
	mini_batch.w = imds.w;
	mini_batch.h = imds.h;
	mini_batch.c = imds.c;
	mini_batch.n = batch_size;
	mini_batch.image = NULL;
	mini_batch.images = (float*)calloc(batch_size*imds.image_size, sizeof(float));
	mini_batch.label = (int*)calloc(batch_size , sizeof(int));
	mini_batch.image_size = imds.image_size;
	mini_batch.format = imds.format;
	for (int i = 0; i < batch_size; i++) {
		int idx = (*schedule)[i];
		memcpy(mini_batch.images + i * imds.image_size, imds.image[idx],imds.image_size*sizeof(float));
		mini_batch.label[i] = imds.label[idx];
	}
	(*schedule) += batch_size;
	return mini_batch;
}
static inline void Normalize(IMDSImage imds, float mean, float stddev) {
	for (int i = 0; i < imds.n; i++) {
		for (int j = 0; j < imds.c*imds.h*imds.w; j++) {
			imds.image[i][j] /= 255.0;
			imds.image[i][j] = (imds.image[i][j] - mean) / stddev;
		}
	}
}
#endif  //OPENIMDS_7E4_3_14_UTILS_H_INCLUDED