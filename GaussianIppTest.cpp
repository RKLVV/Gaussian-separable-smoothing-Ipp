// GaussianIppTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ippi.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void testCode()
{
	Ipp16u src1[9 * 9] = {
		0, 1, 2, 3, 4, 5, 6, 7, 0,
		1, 2, 3, 4, 5, 6, 7, 0, 1,
		2, 3, 4, 5, 6, 7, 0, 1, 2,
		3, 4, 5, 6, 7, 0, 1, 2, 3,
		4, 5, 6, 7, 0, 1, 2, 3, 4,
		5, 6, 7, 0, 1, 2, 3, 4, 5,
		6, 7, 0, 1, 2, 3, 4, 5, 6,
		7, 0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7, 0
	};

	ofstream InputFile("C:\\Image\\TestOutput\\InputImg.raw", ios::out | ios::binary);

	char buf[sizeof(Ipp16u)];
	for (int i = 0; i < (9 * 9); i++)
	{
		memcpy(buf, &src1[i], sizeof(Ipp16u));
		InputFile.write((char*)&buf, sizeof(buf));
	}
	InputFile.close();

	Ipp16u dst1[9 * 9];
	IppiSize roiSize1 = { 9, 9 };
	Ipp32f Ker1[] = { 1, 2, 1 };

	IppStatus result;
	//result = ippiFilterGauss_16u_C1R(src1, 9 * sizeof(Ipp16u), dst1, 9 * sizeof(Ipp16u), roiSize1, ippMskSize3x3);
	result = ippiFilterColumn32f_16u_C1R((Ipp16u*)src1 + 9, 9 * sizeof(Ipp16u), (Ipp16u*)dst1 + 9, 9 * sizeof(Ipp16u), roiSize1, (Ipp32f*)Ker1, 5, 1);

	cout << endl << "Result: " << result << endl;

	// Write processed image to output
	ofstream binaryFile("C:\\Image\\TestOutput\\SmoothImg.raw", ios::out | ios::binary);

	for (int i = 0; i < (9 * 9); i++)
	{
		memcpy(buf, &dst1[i], sizeof(Ipp16u));
		binaryFile.write((char*)&buf, sizeof(buf));
	}
}

// Returns Normalized 2D kernel
void GaussKer1D(Ipp32f* KernArr, int rad)
{
	int size = rad * 5;
	float sum = 0;

	for (int i = -size; i < (size + 1); i++)
	{
		KernArr[i + size] = exp((-1 * ((float)i*(float)i)) / (2 * rad*rad));
		sum += KernArr[i + size];
	}

	for (int i = -size; i < (size + 1); i++)
	{
		KernArr[i + size] = KernArr[i + size] / sum;
	}
}

// Reads RAW image file into source and init dst
void readRawImage(Ipp16u *src, Ipp16u *dst)
{
	// Read image file
	ifstream myImage("C:\\Image\\image023.raw", ios::binary);

	// Store value into buffer
	short value;
	int i = 0;
	char buf[sizeof(short)];
	while (myImage.read(buf, sizeof(buf)))
	{
		memcpy(&value, buf, sizeof(Ipp16u));
		src[i] = value;
		dst[i] = 0;
		i++;
	}

	// number of values read
	cout << "Total count: " << i << " values"<< endl;
}

// Copy buffer data
void copyBuffer(Ipp16u *src, Ipp16u *dst, int size)
{
	for (int i = 0; i < (size); i++)
	{
		dst[i] = src[i];
	}
}

// Dumo D Kernel
void dumpKernel(Ipp32f *ker1D, int kernSize)
{
	int i = 0;
	std::cout << "Kernel = [ ";
	for (i = 0; i < kernSize; i++)
	{
		std::cout << " " << ker1D[i];
	}
	std::cout << "] \n" << "Kernel size : " << i << endl;
}

//Dump image to file
void dumpImage(Ipp16u *src,int size, string filename)
{
	ofstream MyFile(filename, ios::out | ios::binary);

	char buf[sizeof(Ipp16u)];
	for (int i = 0; i < (size); i++)
	{
		memcpy(buf, &src[i], sizeof(Ipp16u));
		MyFile.write((char*)&buf, sizeof(buf));
	}
	MyFile.close();
}

int _tmain(int argc, _TCHAR* argv[])
{
	int debug = std::stoi(argv[4]);

	// Test Ipp with test matrix
	if (debug > 1)
	{
		testCode();
		return 0;
	}

	///1.***************************************************************************************************************************************************
	// Read radius to generate kernel
	int rad = std::stoi(argv[3]);
	int kernSize = (rad * 10) + 1;
	Ipp32f *ker1D = new Ipp32f[kernSize];
	GaussKer1D(ker1D, rad);

	//Dump Kernel
	if (debug)
		dumpKernel(ker1D,kernSize);

	///2.***************************************************************************************************************************************************
	// Read image size
	int w = std::stoi(argv[1]);
	int h = std::stoi(argv[2]);
	int size = w * h;

	// Allocate buffers for input and output images
	//Ipp16u *dst = new Ipp16u[size*sizeof(Ipp16u)];
	Ipp16u *src = (Ipp16u *)malloc(size * sizeof(Ipp16u));
	Ipp16u *dst = (Ipp16u *)malloc(size * sizeof(Ipp16u));

	// read image into src and zero dst
	cout << endl << "Reading input... " << endl;
	readRawImage(src,dst);

	//Dump Read Image
	if (debug)
		dumpImage(src, size, "C:\\Image\\TestOutput\\InputImg.raw");

	///3.***************************************************************************************************************************************************
	// setup Ipp stuff
	IppiSize roiSize = { w, h };
	int srcStep = w;// *sizeof(Ipp16u);
	int dstStep = w;// *sizeof(Ipp16u);
	IppStatus result;

	Ipp32f Ker1[] = { 1, 2, 1 };

	cout << endl << "Column Conv start... " << endl;
	result = ippiFilterColumn32f_16u_C1R((const Ipp16u*)src, srcStep, (Ipp16u*)dst, dstStep, roiSize, ker1D, kernSize, 1);
	cout << endl << "Result: " << result << endl;

	cout << endl << "Row Conv start... " << endl;
	result = ippiFilterRow32f_16u_C1R((const Ipp16u*)dst, srcStep, (Ipp16u*)dst, dstStep, roiSize, ker1D, kernSize, 2);
	cout << endl << "Result: " << result << endl;
	
	///4.***************************************************************************************************************************************************
	// Write processed image to output
	cout << endl << "Writing output... " << endl;
	//copyBuffer(src,dst,size);

	dumpImage(dst, size, "C:\\Image\\TestOutput\\SmoothImg.raw");

	cout << endl << "Done! " << endl;

	return 0;
}

// Some notes:
//ippiFilterColumn32f_16u_C1R(src, srcStep, dst, dstStep, roiSize, ker1D, rad * 5, 1);
//ippiFilterRow32f_16u_C1R(dst, srcStep, dst, dstStep, roiSize, ker1D, rad * 5, 1);

//ippiFilterGauss_16u_C1R(src, w, dst, w, roiSize, ippMskSize5x5);
//result = ippiFilterRow32f_16u_C1R(dst1, 9, dst1, 9, roiSize1, Ker1, 5, 1);