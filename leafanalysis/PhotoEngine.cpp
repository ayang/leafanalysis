#include "StdAfx.h"
#include "PhotoEngine.h"

CPhotoEngine::CPhotoEngine(void)
{
}

CPhotoEngine::~CPhotoEngine(void)
{
}

int CPhotoEngine::_PopulateList(LPCTSTR szPath)
{
	const int count = 0;
	BOOL bRes;
	CFindFile ff;
	CString sPattern;
	sPattern.Format(_T("%s\\*.jpg"), szPath);
	for( bRes = ff.FindFile(sPattern); bRes; bRes = ff.FindNextFile() )
	{
		if( ff.IsDirectory() ) continue;
		AddImageFile(ff.GetFilePath(), ff.GetFileName());
	}

	return 0;
}

bool CPhotoEngine::AddImageFile(LPCTSTR szPath, LPCTSTR szFileName)
{
	const int maxsize = 800;
	PHOTOINFO* pPhoto = new PHOTOINFO;
	if( pPhoto == NULL ) return false;
	pPhoto->sFilePath = szPath;
	pPhoto->sFileName = szFileName;

	size_t origsize = wcslen(szPath) + 1;
    const size_t newsize = 1024;
    size_t convertedChars = 0;
    char nstring[newsize];
    wcstombs_s(&convertedChars, nstring, newsize, szPath, _TRUNCATE);

	cv::Mat image = cv::imread(nstring);
	cv::Size imgsize(image.cols, image.rows);
	if(imgsize.width > maxsize)
	{
		imgsize.height *= maxsize / (float)imgsize.width;
		imgsize.width = maxsize;
	}
	if(imgsize.height > maxsize)
	{
		imgsize.width *= maxsize / (float)imgsize.height;
		imgsize.height = maxsize;
	}
	cv::Mat image2;
	cv::resize(image, image2, imgsize, 0, 0, cv::INTER_AREA);
	if(imgsize.height > imgsize.width)
	{
		cv::transpose(image2, image2);
		cv::flip(image2, image2, 0);
	}
	pPhoto->image = image2.clone();
	pPhoto->Calculate();
	photos.Add(pPhoto);

	return true;
}

int CPhotoEngine::AddDirectory(LPCTSTR szPath)
{
	int count = _PopulateList(szPath);
	return count;
}

void CPhotoEngine::RemoveItem(int index)
{
	photos.RemoveAt(index);
}
