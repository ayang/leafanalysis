#pragma once

#include <opencv2/opencv.hpp>

struct PHOTOINFO
{
   WTL::CString sFileName;
   WTL::CString sFilePath;
   cv::Mat image;
   int leafArea;
   int lesionsArea;
   cv::Mat leafMask;
   cv::Mat lesionsMask;

   void Calculate()
   {
		std::vector<cv::Mat> bgr(3);
		cv::Mat tmp;
	   	cv::split(this->image, bgr);
		cv::divide(bgr[2], bgr[1], tmp, 250);
		cv::threshold(tmp, tmp, 200, 255, cv::THRESH_BINARY);
		cv::threshold(bgr[0], this->leafMask, 160, 255, cv::THRESH_BINARY_INV);
		cv::bitwise_and(tmp, this->leafMask, this->lesionsMask);
		cv::erode(this->lesionsMask, this->lesionsMask, cv::Mat());
		cv::dilate(this->lesionsMask, this->lesionsMask, cv::Mat());
		leafArea = cv::countNonZero(this->leafMask);
		lesionsArea = cv::countNonZero(this->lesionsMask);
   }
};

class CPhotoEngine
{
public:
	CPhotoEngine(void);
	~CPhotoEngine(void);

private:
	ATL::CSimpleValArray<PHOTOINFO*> photos;

public:

	PHOTOINFO* GetImageInfo(int iIndex) const
	{
		ATLASSERT(iIndex>=0 && iIndex<photos.GetSize());
		if( iIndex < 0 || iIndex >= photos.GetSize() ) return NULL;
		return photos[iIndex];
	}

	int GetImageCount() const
	{
		return photos.GetSize();
	}
private:
	int _PopulateList(LPCTSTR szPath);
public:
	bool AddImageFile(LPCTSTR szPath, LPCTSTR szFileName);
	int AddDirectory(LPCTSTR szPath);

	void RemoveItem(int index);
};

extern CPhotoEngine _Photos;