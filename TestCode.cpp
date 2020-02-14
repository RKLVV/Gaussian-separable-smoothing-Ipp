#include "stdafx.h"

//void testCode()
//{
//	Ipp16u src1[9 * 9] = {
//		0, 1, 2, 3, 4, 5, 6, 7, 0,
//		1, 2, 3, 4, 5, 6, 7, 0, 1,
//		2, 3, 4, 5, 6, 7, 0, 1, 2,
//		3, 4, 5, 6, 7, 0, 1, 2, 3,
//		4, 5, 6, 7, 0, 1, 2, 3, 4,
//		5, 6, 7, 0, 1, 2, 3, 4, 5,
//		6, 7, 0, 1, 2, 3, 4, 5, 6,
//		7, 0, 1, 2, 3, 4, 5, 6, 7,
//		0, 1, 2, 3, 4, 5, 6, 7, 0
//	};
//
//	ofstream InputFile("C:\\Image\\TestOutput\\InputImg.raw", ios::out | ios::binary);
//
//	char buf[sizeof(Ipp16u)];
//	for (int i = 0; i < (9 * 9); i++)
//	{
//		memcpy(buf, &src1[i], sizeof(Ipp16u));
//		InputFile.write((char*)&buf, sizeof(buf));
//	}
//	InputFile.close();
//
//	Ipp16u dst1[9 * 9];
//	IppiSize roiSize1 = { 9, 9 };
//	Ipp32f Ker1[] = { 1, 2, 1 };
//
//	IppStatus result;
//	//result = ippiFilterGauss_16u_C1R(src1, 9 * sizeof(Ipp16u), dst1, 9 * sizeof(Ipp16u), roiSize1, ippMskSize3x3);
//	result = ippiFilterColumn32f_16u_C1R((Ipp16u*)src1 + 9, 9 * sizeof(Ipp16u), (Ipp16u*)dst1 + 9, 9 * sizeof(Ipp16u), roiSize1, (Ipp32f*)Ker1, 5, 1);
//
//	cout << endl << "Result: " << result << endl;
//
//	// Write processed image to output
//	ofstream binaryFile("C:\\Image\\TestOutput\\SmoothImg.raw", ios::out | ios::binary);
//
//	for (int i = 0; i < (9 * 9); i++)
//	{
//		memcpy(buf, &dst1[i], sizeof(Ipp16u));
//		binaryFile.write((char*)&buf, sizeof(buf));
//	}
//}