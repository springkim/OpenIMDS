#include"../classification/mnist.h"
#include"../classification/cifar10.h"
#include<opencv2/opencv.hpp>
void CifarTest() {
	IMDSImage train = GetCifar10TrainData();
	IMDSImage mini_batch = GetRandomMiniBatch(train, 64);
	for (int i = 0; i<mini_batch.n; i++) {
		cv::Mat img(mini_batch.h, mini_batch.w, CV_32FC3, mini_batch.image[i]);
		img.convertTo(img, CV_8UC3);
		cv::resize(img, img, cv::Size(500, 500));
		cv::imshow("image", img);
		cv::waitKey();
	}
}
void FashionTest() {
	IMDSImage train = GetFashionTrainData();
	IMDSImage mini_batch = GetRandomMiniBatch(train, 64);
	for (int i = 0; i<mini_batch.n; i++) {
		cv::Mat img(mini_batch.h, mini_batch.w, CV_32FC1, mini_batch.image[i]);
		img.convertTo(img, CV_8UC1);
		cv::resize(img, img, cv::Size(500, 500));
		cv::imshow("image", img);
		cv::waitKey();
	}
}
int main(){
	FashionTest();
	return 0;
}
