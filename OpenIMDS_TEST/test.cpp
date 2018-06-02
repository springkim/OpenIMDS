#include"../classification/mnist.h"
#include<opencv2/opencv.hpp>

int main(){
	IMDSImage train=GetMnistTrainData();

	IMDSImage mini_batch = GetRandomMiniBatch(train, 64);

	for(int i=0;i<mini_batch.n;i++){
		cv::Mat img(mini_batch.h, mini_batch.w,CV_32FC1, mini_batch.image[i]);
		cv::imshow("image",img);
		cv::waitKey();
	}
	return 0;
}
