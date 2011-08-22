// leafanalysisView.h : interface of the CLeafanalysisView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "atlctrls.h"

class CLeafanalysisView : public CWindowImpl<CLeafanalysisView, CListViewCtrl>, public WTL::CTheme
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CLeafanalysisView)		
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		COMMAND_ID_HANDLER(ID_SAMPLE_ADD, OnSampleAdd)
		COMMAND_ID_HANDLER(ID_SAMPLE_ADD_DIRECTORY, OnSampleAddDirectory)
		REFLECTED_NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnItemChanged)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(LVN_ITEMACTIVATE, OnItemActivate)
		COMMAND_ID_HANDLER(ID_SAMPLE_REMOVE, OnSampleRemove)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
private:
	void _PopulateList(void);
	WTL::CImageList m_Images;
	static const int m_cxIcon = 128;
public:
	LRESULT OnSampleAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSampleAddDirectory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnSampleRemove(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnItemActivate(LPNMHDR pnmh);
};
