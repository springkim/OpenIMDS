#include"../classification/mnist.h"
#include"../classification/cifar10.h"
#include"../classification/stl10.h"
#include"../classification/types.h"
#include"../classification/utils.h"


#include<opencv2/opencv.hpp>
void rrggbb2bgrbgr(float* in, float* out, int w, int h, int c) {
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			out[y*w * 3 + x * 3 + 2] = in[0 * w*h + y * w + x];
			out[y*w * 3 + x * 3 + 1] = in[1 * w*h + y * w + x];
			out[y*w * 3 + x * 3 + 0] = in[2 * w*h + y * w + x];
		}
	}
}
void CifarTest() {
	IMDSImage train = GetCifar10TrainData();
	IMDSImage mini_batch = GetRandomMiniBatch(train, 64);
	for (int i = 0; i < mini_batch.n; i++) {
		float* p = new float[mini_batch.w*mini_batch.h*mini_batch.c];
		rrggbb2bgrbgr(mini_batch.image[i], p, mini_batch.w, mini_batch.h, mini_batch.c);
		cv::Mat img(mini_batch.h, mini_batch.w, CV_32FC3, p);
		img.convertTo(img, CV_8UC3);
		cv::resize(img, img, cv::Size(500, 500));
		cv::imshow("image", img);
		cv::waitKey();
		delete[] p;
	}
}
void FashionTest() {
	IMDSImage train = GetFashionTrainData();
	IMDSImage mini_batch = GetRandomMiniBatch(train, 64);
	for (int i = 0; i < mini_batch.n; i++) {
		cv::Mat img(mini_batch.h, mini_batch.w, CV_32FC1, mini_batch.image[i]);
		img.convertTo(img, CV_8UC1);
		cv::resize(img, img, cv::Size(500, 500));
		cv::imshow("image", img);
		cv::waitKey();
	}
}
void STL10Test() {
	IMDSImage train = GetSTL10ValidData();
	for (int i = 0; i < train.n; i++) {
		float* p = new float[train.w*train.h*train.c];
		rrggbb2bgrbgr(train.image[i], p, train.w, train.h, train.c);
		cv::Mat img(train.h, train.w, CV_32FC3, p);
		img.convertTo(img, CV_8UC3);
		cv::resize(img, img, cv::Size(500, 500));
		cv::imshow("image", img);
		std::cout << GetSTL10Class(train.label[i]) << std::endl;
		cv::waitKey();
		delete[] p;
	}
}
void MnistTest() {
	IMDSImage train = GetMnistTrainData();
	IMDSImage mini_batch = GetRandomMiniBatch(train, 64);
	for (int i = 0; i < mini_batch.n; i++) {
		cv::Mat img(mini_batch.h, mini_batch.w, CV_32FC1, mini_batch.image[i]);
		img.convertTo(img, CV_8UC1);
		cv::resize(img, img, cv::Size(500, 500));
		cv::imshow("image", img);
		cv::waitKey();
	}
}
int main() {
	STL10Test();
	return 0;
}
