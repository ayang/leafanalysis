// leafanalysisView.cpp : implementation of the CLeafanalysisView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include <vector>

#include "PhotoEngine.h"
#include "leafanalysisView.h"

BOOL CLeafanalysisView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}


LRESULT CLeafanalysisView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LRESULT lRes = DefWindowProc();

	ModifyStyle(0, LVS_ALIGNTOP | LVS_SHAREIMAGELISTS);

	SetBkColor(RGB(238,243,250));
	SetTextBkColor(RGB(238,243,250));
	SetViewType(LV_VIEW_ICON);
	SetExtendedListViewStyle(LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
	if( IsThemingSupported() ) SetWindowTheme(m_hWnd, L"Explorer", NULL);

	_PopulateList();

	return lRes;

	return 0;
}

Gdiplus::Bitmap* BitmapfromCvImage(IplImage* cvimg)
{
	BITMAPINFO binfo = {0};
	binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	binfo.bmiHeader.biPlanes = 1;
	binfo.bmiHeader.biBitCount = cvimg->depth * cvimg->nChannels;
	binfo.bmiHeader.biHeight = cvimg->height;
	if(cvimg->origin == 0) binfo.bmiHeader.biHeight = - binfo.bmiHeader.biHeight;
	binfo.bmiHeader.biWidth = cvimg->width;

	Gdiplus::Bitmap *pbmp = Gdiplus::Bitmap::FromBITMAPINFO(&binfo, cvimg->imageData);

	return pbmp;
}

Gdiplus::Bitmap* BitmapfromMat(cv::Mat image)
{
	BITMAPINFO binfo = {0};
	binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	binfo.bmiHeader.biPlanes = 1;
	binfo.bmiHeader.biBitCount = image.elemSize()*8;
	binfo.bmiHeader.biHeight = -image.rows;
	binfo.bmiHeader.biWidth = image.cols;

	Gdiplus::Bitmap *pbmp = Gdiplus::Bitmap::FromBITMAPINFO(&binfo, image.data);

	return pbmp;
}

void CLeafanalysisView::_PopulateList(void)
{
	int cyIcon = 18 + (int)(m_cxIcon * 0.7);                                        // Height is proportionally less
	int cyOffset = 16;                                                              // Nice large gap at top
	double dblScale = 1.0;                                                          // Thumb is full scale
	dblScale = 0.8;                          // Thumb with text is slightly smaller

	SetRedraw(FALSE);

	DeleteAllItems();
	while( DeleteColumn(0) ) /* */;

	if( !m_Images.IsNull() ) m_Images.Destroy();
	m_Images.Create(m_cxIcon, cyIcon, ILC_COLOR32, _Photos.GetImageCount(), 0);
	SetImageList(m_Images, LVSIL_NORMAL);

	SetIconSpacing(m_cxIcon + 20, cyIcon + 26);

	SetView(LV_VIEW_ICON);

	for( int iIndex = 0; iIndex < _Photos.GetImageCount(); iIndex++ ) 
	{
		PHOTOINFO* pInfo = _Photos.GetImageInfo(iIndex);

		// Create thumbnail icon for imagelist
		CIcon icon;
		int cxThumb = (int)(m_cxIcon * dblScale);
		int cyThumb = (int)(cyIcon * dblScale);

		CvSize thumbsize;
		thumbsize.width = cxThumb;
		thumbsize.height = cyIcon;
		cv::Mat thumb = cv::Mat();
		cv::resize(pInfo->image, thumb, thumbsize, 0, 0, cv::INTER_AREA);
		//cv::namedWindow("image");cv::imshow("image", thumb);cv::waitKey();cv::destroyWindow("image");
		Gdiplus::Bitmap* pThumb = BitmapfromMat(pInfo->image);
		pThumb->GetHICON(&icon.m_hIcon);

		//Gdiplus::Image* pThumb = pInfo->pImage->GetThumbnailImage(cxThumb, cyIcon);

		//if( pThumb != NULL ) {
		//	Gdiplus::Bitmap bm24bpp(cxThumb, cyThumb); 
		//	Gdiplus::Graphics g(&bm24bpp);
		//	Gdiplus::Pen penShadow1(Gdiplus::Color(183,187,192));
		//	Gdiplus::SolidBrush brBack(Gdiplus::Color(0,238,243,250));
		//	Gdiplus::Rect rcThumb(0, 0, cxThumb, cyIcon);
		//	g.FillRectangle(&brBack, rcThumb);
		//	g.DrawRectangle(&penShadow1, 2, cyOffset + 1, cxThumb - 3, cyThumb - cyOffset - 2);
		//	g.DrawImage(pThumb, 0, cyOffset, cxThumb - 2, cyThumb - cyOffset - 2); 
		//	Gdiplus::Pen penWhite(Gdiplus::Color(255,255,255));
		//	Gdiplus::Pen penGrey(Gdiplus::Color(228,228,228));
		//	g.DrawRectangle(&penGrey, 0, cyOffset, cxThumb - 2, cyThumb - cyOffset - 2);
		//	g.DrawRectangle(&penWhite, 1, cyOffset + 1, cxThumb - 4, cyThumb - cyOffset - 4);
		//	g.DrawRectangle(&penWhite, 2, cyOffset + 2, cxThumb - 6, cyThumb - cyOffset - 6);
		//	bm24bpp.GetHICON(&icon.m_hIcon);
		//	delete pThumb;
		//}
		m_Images.AddIcon(icon);

		// Insert list item...
		LVITEM item = { 0 };
		item.iItem = iIndex;
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		item.iImage = iIndex;
		item.pszText = pInfo->sFileName.GetBuffer(50);
		item.lParam = (LPARAM) pInfo;
		item.iSubItem = 0;
		InsertItem(&item);
	}

	SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	SetRedraw(TRUE);
	Invalidate();

}

LRESULT CLeafanalysisView::OnSampleAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
#define FILENAMES_BUFFER_SIZE 1024
	TCHAR* buf_file = new TCHAR[FILENAMES_BUFFER_SIZE];
	CString filepath;

	CFileDialog fd (TRUE, NULL, NULL,
		OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, 
		L"JPEG图像 (*.jpg) \0 *.jpg\0\0", m_hWnd
		); 
	fd.m_ofn.nMaxFile = FILENAMES_BUFFER_SIZE;
	fd.m_ofn.lpstrFile = buf_file;
	memset(buf_file, 0, sizeof(TCHAR)*FILENAMES_BUFFER_SIZE);

	if (fd.DoModal () == IDOK) 
	{
		if(*fd.m_szFileTitle != 0)
		{
			_Photos.AddImageFile(buf_file, fd.m_szFileTitle);
		}
		else
		{
			LPTSTR dirname = buf_file;
			LPTSTR filename = dirname + lstrlen(dirname) + 1;
			while(*filename != 0)
			{
				filepath.Format(_T("%s\\%s"), dirname, filename);
				_Photos.AddImageFile(filepath, filename);
				filename += lstrlen(filename) + 1;
			}
		}

		_PopulateList();
	}

	return 0;
}

LRESULT CLeafanalysisView::OnSampleAddDirectory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFolderDialog fldDlg ( m_hWnd, _T("选择保存标本图片的文件夹"),
		BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE );

	if ( IDOK == fldDlg.DoModal() )
	{
		_Photos.AddDirectory(fldDlg.m_szFolderPath);
		_PopulateList();
	}


	return 0;
}

LRESULT CLeafanalysisView::OnItemChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	PHOTOINFO* pinfo;
	LPNMITEMACTIVATE pactivate = (LPNMITEMACTIVATE)pnmh;

	pinfo = _Photos.GetImageInfo(pactivate->iItem);

	SendMessage(_hMainFrame, WM_USER_SELECTION_CHANGE, 0, (LPARAM)pinfo);

	return 0;
}


LRESULT CLeafanalysisView::OnSampleRemove(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int selectedindex = GetNextItem(-1, LVNI_SELECTED);
	while (selectedindex != -1)
	{
		_Photos.RemoveItem(selectedindex);
		selectedindex = GetNextItem(selectedindex, LVNI_SELECTED);
	}
	_PopulateList();

	return 0;
}


LRESULT CLeafanalysisView::OnItemActivate(LPNMHDR pnmh)
{

	return 0;
}
