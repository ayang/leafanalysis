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
	PHOTOINFO* pPhoto = new PHOTOINFO;
	if( pPhoto == NULL ) return false;
	pPhoto->sFilePath = szPath;
	pPhoto->sFileName = szFileName;

	size_t origsize = wcslen(szPath) + 1;
    const size_t newsize = 1024;
    size_t convertedChars = 0;
    char nstring[newsize];
    wcstombs_s(&convertedChars, nstring, newsize, szPath, _TRUNCATE);

	pPhoto->image = cv::imread(nstring);
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
