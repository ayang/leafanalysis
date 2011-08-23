// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "leafanalysisView.h"
#include "PropertyList.h"
#include "PhotoEngine.h"
#include "MainFrm.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	if(m_view.IsWindow())
		return m_view.PreTranslateMessage(pMsg);
	return FALSE;
	//return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
	m_CmdBar.SetAlphaImages(true);
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME1);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	CImageList iltoolbar;
	iltoolbar.CreateFromImage(IDB_BITMAP1, 48, 48, RGB(255,255,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	CToolBarCtrl wndToolbar(hWndToolBar);
	wndToolbar.SetImageList(iltoolbar.m_hImageList);

	CreateSimpleStatusBar();

	m_hWndClient = m_vsplitter.Create(m_hWnd, rcDefault);
	m_hsplitter.Create(m_vsplitter, rcDefault);
	m_paneProperty.Create(m_vsplitter, _T("属性"));
	m_paneResult.Create(m_hsplitter,  _T("结果统计"));
	m_view.Create(m_hsplitter, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LVS_ICON | LVS_ALIGNTOP | LVS_AUTOARRANGE | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
	m_property.Create(m_paneProperty, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LBS_OWNERDRAWVARIABLE, WS_EX_CLIENTEDGE);
	m_result.Create(m_paneResult, rcDefault, NULL,  WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL, WS_EX_CLIENTEDGE);

	m_paneResult.SetClient(m_result);
	m_paneProperty.SetClient(m_property);

	m_hsplitter.SetSplitterPanes(m_view, m_paneResult);
	m_vsplitter.SetSplitterPanes(m_hsplitter, m_paneProperty);

	UpdateLayout();
	m_vsplitter.SetSplitterExtendedStyle(SPLIT_RIGHTALIGNED);
	m_hsplitter.SetSplitterExtendedStyle(SPLIT_BOTTOMALIGNED);

	RECT rect;
	m_vsplitter.GetSplitterRect(&rect);
	m_vsplitter.SetSplitterPos(rect.right - 200);
	m_hsplitter.SetSplitterPos(rect.bottom - 200);

	m_paneProperty.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);
	m_paneResult.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);


	m_font = AtlCreateControlFont();
	m_result.SetFont(m_font);


	m_property.SetExtendedListStyle(PLS_EX_CATEGORIZED);
	m_property.AddItem( PropCreateCategory(_T("图像属性")) );
	m_property.AddItem( PropCreateSimple(_T("文件名"), _T("")) );
	m_property.AddItem( PropCreateSimple(_T("宽度"), 0L) );
	m_property.AddItem( PropCreateSimple(_T("高度"), 0L) );
	m_property.AddItem( PropCreateCategory(_T("叶片病斑")) );
	m_property.AddItem( PropCreateSimple(_T("叶片面积"), 0L) );
	m_property.AddItem( PropCreateSimple(_T("病斑面积"), 0L) );
	m_property.AddItem( PropCreateSimple(_T("相对病斑面积"), _T("0")) );

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document

	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnSampleCompute(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int count = _Photos.GetImageCount();
	if (count == 0)
		return 0;

	int leafarea[3] = {0, 0, INT_MAX};
	int lesionsarea[3] = {0, 0, INT_MAX};
	float relative[3] = {0.0, 0.0, 1.1};
	for (int i = 0; i < count; i++)
	{
		PHOTOINFO *info = _Photos.GetImageInfo(i);
		leafarea[0] += info->leafArea;
		if (leafarea[1] < info->leafArea)
		{
			leafarea[1] = info->leafArea;
		}
		if (leafarea[2] > info->leafArea)
		{
			leafarea[2] = info->leafArea;
		}

		lesionsarea[0] += info->lesionsArea;
		if (lesionsarea[1] < info->lesionsArea)
		{
			lesionsarea[1] = info->lesionsArea;
		}
		if (lesionsarea[2] > info->lesionsArea)
		{
			lesionsarea[2] = info->lesionsArea;
		}

		relative[0] += info->lesionsArea / (float)info->leafArea;
		if (relative[1] < info->lesionsArea / (float)info->leafArea)
		{
			relative[1] = info->lesionsArea / (float)info->leafArea;
		}
		if (relative[2] > info->lesionsArea / (float)info->leafArea)
		{
			relative[2] = info->lesionsArea / (float)info->leafArea;
		}
	}
	leafarea[0] /= count;
	lesionsarea[0] /= count;
	relative[0] /= count;

	CString temp = L"====================================== 统计结果 ===================================\r\n" \
		L"标本数量：\t%d\r\n" \
		L"叶片面积(像素)：\t平均 %d, 最大 %d, 最小 %d\r\n" \
		L"病斑面积(像素)：\t平均 %d, 最大 %d, 最小 %d\r\n" \
		L"相对病斑面积：\t平均 %2.2f%%, 最大 %2.2f%%, 最小 %2.2f%%\r\n\r\n";
	CString message, text;
	message.Format(temp, count, leafarea[0], leafarea[1], leafarea[2], 
		lesionsarea[0], lesionsarea[1], lesionsarea[2], 
		relative[0]*100, relative[1]*100, relative[2]*100
		);
	m_result.AppendText(message);
	return 0;
}

LRESULT	CMainFrame::OnSelectionChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	PHOTOINFO *pinfo = (PHOTOINFO*)lParam;
	m_property.SetItemValue(m_property.FindProperty(_T("文件名")), &CComVariant(pinfo->sFileName));
	m_property.SetItemValue(m_property.FindProperty(_T("宽度")), &CComVariant(pinfo->image.cols));
	m_property.SetItemValue(m_property.FindProperty(_T("高度")), &CComVariant(pinfo->image.rows));
	m_property.SetItemValue(m_property.FindProperty(_T("叶片面积")), &CComVariant(pinfo->leafArea));
	m_property.SetItemValue(m_property.FindProperty(_T("病斑面积")), &CComVariant(pinfo->lesionsArea));
	CString svalue;
	svalue.Format(_T("%2.2f%%"), pinfo->lesionsArea * 100 / (float)pinfo->leafArea);
	m_property.SetItemValue(m_property.FindProperty(_T("相对病斑面积")), &CComVariant(svalue));m_property.Invalidate();
	
	return 0;
}

//void CMainFrame::UpdateLayout(BOOL bResizeBars)
//{
	//if(!m_view.IsWindow())
	//	return;
	//CRect rcClient;
	//GetClientRect(&rcClient);
	//UpdateBarsPosition(rcClient, bResizeBars);

	//CRect rcProperty = rcClient;
	//rcProperty.left = rcProperty.right - 200;
	//rcClient.right = rcProperty.left;

	//CRect rcResult = rcClient;
	//rcResult.top = rcResult.bottom - 180;
	//rcClient.bottom = rcResult.top;

	//m_property.SetWindowPos(NULL, &rcProperty, SWP_NOZORDER | SWP_NOACTIVATE);
	//m_result.SetWindowPos(NULL, &rcResult, SWP_NOZORDER | SWP_NOACTIVATE);
	//m_view.SetWindowPos(NULL, &rcClient, SWP_NOZORDER | SWP_NOACTIVATE);
//}
