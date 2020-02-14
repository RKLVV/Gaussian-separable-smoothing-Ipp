// GaussianIppTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ippi.h>
#include <ipps.h>
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
void GaussKer1D(Ipp32f* KernArr, float rad, int factor)
{
	int size = int(rad * factor);
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
	ifstream myImage("C:\\Image\\GausInput.raw", ios::binary);

	// Store value into buffer
	short value;
	int i = 0;
	char buf[sizeof(short)];
	while (myImage.read(buf, sizeof(buf)))
	{
		memcpy(&value, buf, sizeof(Ipp16u));
		src[i] = value;
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
void dumpImage(Ipp16u *src, int size, string filename, int offset)
{
	ofstream MyFile(filename, ios::out | ios::binary);
	int i = 0;
	char buf[sizeof(Ipp16u)];
	for (i = offset; i < (size-offset); i++)
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
	// Init
	Ipp32f *ker1D;
	float rad = std::stof(argv[3]);
	int factor = 5;
	if (rad > 1.4)
		factor = 3;

	int kernSize = rad * factor * 2;
	if (kernSize % 2 == 0)
		kernSize++;

	IppStatus result;

	int w = std::stoi(argv[1]);
	int h = std::stoi(argv[2]);
	int size = w * h;
	int outSize = (w + kernSize - 1)*(h + kernSize - 1);

	// Allocate buffers for input and output images
	Ipp16u *src = (Ipp16u *)malloc(size * sizeof(Ipp16u));
	Ipp16u *temp = (Ipp16u *)malloc(outSize * sizeof(Ipp16u));
	Ipp16u *dst = (Ipp16u *)malloc(size * sizeof(Ipp16u));
	Ipp16u *Hp = (Ipp16u *)malloc(size * sizeof(Ipp16u));

	// ROI for src and dest images
	IppiSize roiSize1 = { w, h };
	IppiSize roiSize2 = { w + kernSize - 1, h + kernSize - 1 };

	// Stepsize
	int srcStep = w*sizeof(Ipp16u);
	int dstStep = w*sizeof(Ipp16u);

	///2.***************************************************************************************************************************************************
	// Read image size
	// read image into src and zero dst
	cout << endl << "Reading input... " << endl;
	readRawImage(src,dst);

	//Dump Read Image
	/*if (debug)
		dumpImage(src, size, "C:\\Image\\TestOutput\\InputImg.raw",0);*/

	///3.***************************************************************************************************************************************************
	// Generate Kernel
	ker1D = new Ipp32f[kernSize];
	GaussKer1D(ker1D, rad, factor);

	//Dump Kernel
	if (debug)
		dumpKernel(ker1D, kernSize);

	///4.***************************************************************************************************************************************************
	// Border pixels for Column
	int topBorderHeight = (kernSize - 1)/2;
	int leftBorderWidth = (kernSize - 1)/2;

	// Change dst step
	dstStep = (w + kernSize - 1)*sizeof(Ipp16u);

	cout << endl << "Column Border start... " << endl;
	
	result = ippiCopyReplicateBorder_16u_C1R(src, srcStep, roiSize1, temp, dstStep, roiSize2, topBorderHeight, leftBorderWidth);
	cout << endl << "Result: " << result << endl;

	if (debug)
		dumpImage(temp, outSize, "C:\\Image\\TestOutput\\BorImg.raw", 0);

	///5.***************************************************************************************************************************************************
	// Applyfilter
	// Change ROI
	roiSize1 = { w+kernSize-1, h };
	roiSize2 = { w, h+kernSize-1};
	
	// Change stepsize
	srcStep = (w + kernSize - 1)*sizeof(Ipp16u);
	dstStep = (w + kernSize - 1)*sizeof(Ipp16u);
	
	cout << endl << "Column Conv start... " << endl;
	result = ippiFilterColumn32f_16u_C1R(temp, srcStep, temp, dstStep, roiSize1, ker1D, kernSize, kernSize - 1);
	cout << endl << "Result: " << result << endl;

	if (debug)
		dumpImage(temp, outSize, "C:\\Image\\TestOutput\\ColImg.raw", 0);

	cout << endl << "Row Conv start... " << endl;
	result = ippiFilterRow32f_16u_C1R(temp, srcStep, temp, dstStep, roiSize2, ker1D, kernSize, kernSize - 1);
	cout << endl << "Result: " << result << endl;

	// Crop image
	// Change ROI
	roiSize1 = { w, h };

	// Change stepsize
	srcStep = (w + kernSize - 1)*sizeof(Ipp16u);
	dstStep = (w)*sizeof(Ipp16u);

	cout << endl << "Copying to final output: " << endl;
	result = ippiCopy_16u_C1R(temp,srcStep, dst,dstStep,roiSize1);
	cout << endl << "Result: " << result << endl;

	// HP image
	cout << endl << "Computing HP image: " << endl;
	result = ippsSub_16u_Sfs(src, dst, Hp, size, 1);
	cout << endl << "Result: " << result << endl;
	
	///4.***************************************************************************************************************************************************
	// Write processed image to output
	cout << endl << "Writing output... " << endl;

	dumpImage(src, size, "C:\\Image\\TestOutput\\InputImg.raw", 0);
	dumpImage(dst, size, "C:\\Image\\TestOutput\\SmoothImg.raw", 0);
	dumpImage(Hp, size, "C:\\Image\\TestOutput\\HighPass.raw", 0);

	cout << endl << "Done! " << endl;

	return 0;
}