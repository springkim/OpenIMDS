OpenIMDS
========

#### Open Image Datasets

This simple header only C library can read popular classification datasets.

## 1.	Classification datasets

We are using `IMDSImage` type for each datasets. It has width,height,the number of images,images and labels only.

### MNIST(mnist.h)

<img src="https://i.imgur.com/9BcVOYQ.png" width="256">

The MNIST database of handwritten digits, available from this page, has a training set of 60,000 examples, and a test set of 10,000 examples. It is a subset of a larger set available from NIST. The digits have been size-normalized and centered in a fixed-size image.

It is a good database for people who want to try learning techniques and pattern recognition methods on real-world data while spending minimal efforts on preprocessing and formatting.

This script automatically download MNIST datasets to your temporary directory at once. So you just need below 2 functions.

The default parameters are only available in C++. It automatically disable in C.
```c  
IMDSImage GetMnistTrainData(int padding=0,float alpha=1.0F,bool bias=true);
```

```c
IMDSImage GetMnistValidData(int padding=0,float alpha=1.0F,bool bias=true);`
```

*	Parameters

	*	padding : The top,bottom,left and right padding of image.
	*	alpha : Multiply this value to each pixel.
	*	bias : Adding bias input on image if this value set true.

###### Example

```cpp
IMDSImage train= GetMnistTrainData(0, 1 / 255.0F,true);
IMDSImage valid= GetMnistValidData(0, 1 / 255.0F,true);
```
### Fashion(mnist.h)

<img src="https://i.imgur.com/MtTIit7.png" width="256">

**[Fashion-MNIST](https://github.com/zalandoresearch/fashion-mnist)** is a dataset of Zalando's article images—consisting of a training set of 60,000 examples and a test set of 10,000 examples. Each example is a 28x28 grayscale image, associated with a label from 10 classes. We intend Fashion-MNIST to serve as a direct drop-in replacement for the original MNIST dataset for benchmarking machine learning algorithms. It shares the same image size and structure of training and testing splits.

```c
IMDSImage GetFashionTrainData(int padding=0,float alpha=1.0F,bool bias=true)
```

```c
IMDSImage GetFashionValidData(int padding=0,float alpha=1.0F,bool bias=true)
```

### Cifar-10(cifar10.h)

<img src="https://i.imgur.com/X7Tv9dQ.png" width="256">

The CIFAR-10 dataset consists of 60000 32x32 colour images in 10 classes, with 6000 images per class. There are 50000 training images and 10000 test images.

```c
IMDSImage GetCifar10TrainData(int padding=0,float alpha=1.0F,bool bias=true)
```

```c
IMDSImage GetCifar10ValidData(int padding=0,float alpha=1.0F,bool bias=true)
```

```c
char* GetCifar10Class(int index)
```
