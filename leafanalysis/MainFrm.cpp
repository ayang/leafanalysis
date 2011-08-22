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

	m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LVS_ICON | LVS_ALIGNTOP | LVS_AUTOARRANGE | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
	m_property.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LBS_OWNERDRAWVARIABLE, WS_EX_CLIENTEDGE);
	m_result.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY, WS_EX_CLIENTEDGE);

	CFont* font = new CFont();
	font->CreateFont(18,0,0,0,FW_NORMAL, 0, 0, 0,
		DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"Arial");
	m_result.SetFont(*font, TRUE);

	m_property.SetExtendedListStyle(PLS_EX_CATEGORIZED);
	m_property.AddItem( PropCreateCategory(_T("ͼ������")) );
	m_property.AddItem( PropCreateSimple(_T("�ļ���"), _T("")) );
	m_property.AddItem( PropCreateSimple(_T("���"), 0L) );
	m_property.AddItem( PropCreateSimple(_T("�߶�"), 0L) );
	m_property.AddItem( PropCreateCategory(_T("ҶƬ����")) );
	m_property.AddItem( PropCreateSimple(_T("ҶƬ���"), 0L) );
	m_property.AddItem( PropCreateSimple(_T("�������"), 0L) );
	m_property.AddItem( PropCreateSimple(_T("��Բ������"), _T("0")) );

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

	CString temp = L"====================================== ͳ�ƽ�� ===================================\r\n" \
		L"�걾������\t%d\r\n" \
		L"ҶƬ���(����)��\tƽ�� %d, ��� %d, ��С %d\r\n" \
		L"�������(����)��\tƽ�� %d, ��� %d, ��С %d\r\n" \
		L"��Բ��������\tƽ�� %2.2f%%, ��� %2.2f%%, ��С %2.2f%%\r\n\r\n";
	CString message, text;
	message.Format(temp, count, leafarea[0], leafarea[1], leafarea[2], 
		lesionsarea[0], lesionsarea[1], lesionsarea[2], 
		relative[0]*100, relative[1]*100, relative[2]*100
		);
	m_result.Clear();
	m_result.AppendText(message);
	return 0;
}

LRESULT	CMainFrame::OnSelectionChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	PHOTOINFO *pinfo = (PHOTOINFO*)lParam;
	m_property.SetItemValue(m_property.FindProperty(_T("�ļ���")), &CComVariant(pinfo->sFileName));
	m_property.SetItemValue(m_property.FindProperty(_T("���")), &CComVariant(pinfo->image.cols));
	m_property.SetItemValue(m_property.FindProperty(_T("�߶�")), &CComVariant(pinfo->image.rows));
	m_property.SetItemValue(m_property.FindProperty(_T("ҶƬ���")), &CComVariant(pinfo->leafArea));
	m_property.SetItemValue(m_property.FindProperty(_T("�������")), &CComVariant(pinfo->lesionsArea));
	CString svalue;
	svalue.Format(_T("%2.2f%%"), pinfo->lesionsArea * 100 / (float)pinfo->leafArea);
	m_property.SetItemValue(m_property.FindProperty(_T("��Բ������")), &CComVariant(svalue));
	
	return 0;
}

void CMainFrame::UpdateLayout(BOOL bResizeBars)
{
	if(!m_view.IsWindow())
		return;
	CRect rcClient;
	GetClientRect(&rcClient);
	UpdateBarsPosition(rcClient, bResizeBars);

	CRect rcProperty = rcClient;
	rcProperty.left = rcProperty.right - 200;
	rcClient.right = rcProperty.left;

	CRect rcResult = rcClient;
	rcResult.top = rcResult.bottom - 180;
	rcClient.bottom = rcResult.top;

	m_property.SetWindowPos(NULL, &rcProperty, SWP_NOZORDER | SWP_NOACTIVATE);
	m_view.SetWindowPos(NULL, &rcClient, SWP_NOZORDER | SWP_NOACTIVATE);
	m_result.SetWindowPos(NULL, &rcResult, SWP_NOZORDER | SWP_NOACTIVATE);
	m_result.Invalidate();
}
