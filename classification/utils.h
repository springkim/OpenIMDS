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
static inline IMDSImage GetRandomMiniBatch(IMDSImage imds, int batch_size) {
	IMDSImage mini_batch;
	mini_batch.w = imds.w;
	mini_batch.h = imds.h;
	mini_batch.c = imds.c;
	mini_batch.n = batch_size;
	mini_batch.image = (float**)malloc(batch_size * sizeof(float*));
	mini_batch.label = (int*)malloc(batch_size * sizeof(int));
	mini_batch.format = imds.format;
	mini_batch.original = false;
	for (int i = 0; i < batch_size; i++) {
		int idx = rand() % imds.n;
		mini_batch.image[i] = imds.image[idx];
		mini_batch.label[i] = imds.label[idx];
	}
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