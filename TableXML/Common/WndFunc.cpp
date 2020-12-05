#include "stdafx.h"
#include "WndFunc.h"
#include <float.h>

#define CHTEXT_MAX 600

//////////////////////////////////////////////////////////////////////////
//
// Double
//
//////////////////////////////////////////////////////////////////////////
void SetWindowDouble(CWnd *pWndTarget, double dVal, int nDigits)
{
	char chText[CHTEXT_MAX];
	char chFormat[CHTEXT_MAX];
	sprintf_s(chFormat, CHTEXT_MAX, "%%.%dlf", nDigits);
	sprintf_s(chText, CHTEXT_MAX, chFormat, dVal);
	pWndTarget->SetWindowText(chText);
}

void SetDlgItemDouble(CWnd *pWnd, int nID, double dVal, int nDigits)
{
	char chText[CHTEXT_MAX];
	char chFormat[CHTEXT_MAX];
	sprintf_s(chFormat, CHTEXT_MAX, "%%.%dlf", nDigits);
	sprintf_s(chText, CHTEXT_MAX, chFormat, dVal);
	CString strTmp;
	pWnd->GetDlgItem(nID)->GetWindowTextA(strTmp);
	if(strTmp.Compare(chText) != 0)
		pWnd->SetDlgItemText(nID, chText);
}

void SetDlgItemDoubleMulti(CWnd *pWnd, int nID, double dVal, int nDigits)
{
	char chText[CHTEXT_MAX];
	if (dVal == DBL_MIN)
		sprintf_s(chText, CHTEXT_MAX, "*");
	else
	{
		char chFormat[CHTEXT_MAX];
		sprintf_s(chFormat, CHTEXT_MAX, "%%.%dlf", nDigits);
		sprintf_s(chText, CHTEXT_MAX, chFormat/*"%.3lf"*/, dVal);
	}
	CString strTmp;
	pWnd->GetDlgItem(nID)->GetWindowTextA(strTmp);
	if(strTmp.Compare(chText) != 0)
		pWnd->SetDlgItemText(nID, chText);
}

double GetWindowDouble(CWnd *pWndTarget)
{
	char chText[CHTEXT_MAX];
	pWndTarget->GetWindowText(chText, CHTEXT_MAX);
	return atof(chText);
}

double GetDlgItemDouble(CWnd *pWnd, int nID)
{
	char chText[CHTEXT_MAX];
	pWnd->GetDlgItemText(nID, chText, CHTEXT_MAX);
	return atof(chText);
}

double GetDlgItemDoubleMulti(CWnd *pWnd, int nID)
{
	double dVal;
	char chText[CHTEXT_MAX];
	pWnd->GetDlgItemText(nID, chText, CHTEXT_MAX);
	if (strcmp(chText, "*") == 0)
		dVal = DBL_MIN;
	else
		dVal = atof(chText);

	return dVal;
}
void EnableWindowEx(CWnd *pWnd, int nID, bool bVal)
{
	
	ASSERT(pWnd);
	CWnd *pDlg = pWnd->GetDlgItem(nID);
	ASSERT(pDlg);

	bool bCurEnable = pDlg->IsWindowEnabled() ? true : false;
	if(bCurEnable != bVal)
		pDlg->EnableWindow(bVal);
	
}

void EnableWindowEx(CWnd *pWnd, bool bVal)
{
	try
	{
		ASSERT(pWnd);
		BOOL bCur = pWnd->IsWindowEnabled();
		if(bCur != (BOOL)bVal)
			pWnd->EnableWindow(bVal);
	} catch(...){ASSERT(FALSE);}
}

void ShowWindowEx(CWnd *pWnd, int nID, bool bVal)
{
	try
	{
		ASSERT(pWnd);
		CWnd *pDlg = pWnd->GetDlgItem(nID);
		ASSERT(pDlg);
		BOOL bCur = pDlg->IsWindowVisible();
		if(bCur != (BOOL)bVal)
			pDlg->ShowWindow(bVal);
	} catch(...){ASSERT(FALSE);}
}
//////////////////////////////////////////////////////////////////////////
//
// Int
//
//////////////////////////////////////////////////////////////////////////
void SetDlgItemIntEx(CWnd *pWnd, int nID, int nVal, int nDigits)
{
	char chText[CHTEXT_MAX];
	char chFormat[CHTEXT_MAX];
	sprintf_s(chFormat, CHTEXT_MAX, "%%.%dd", nDigits);
	sprintf_s(chText, CHTEXT_MAX, chFormat, nVal);
	CString strTmp;
	pWnd->GetDlgItem(nID)->GetWindowTextA(strTmp);
	if(strTmp.Compare(chText) != 0)
		pWnd->SetDlgItemText(nID, chText);
}

int GetDlgItemIntEx(CWnd *pWnd, int nID)
{
	char chText[CHTEXT_MAX];
	pWnd->GetDlgItemText(nID, chText, CHTEXT_MAX);
	return atoi(chText);
}

void SetDlgItemIntMulti(CWnd *pWnd, int nID, int nVal)
{
	char chText[CHTEXT_MAX];
	if (nVal == INT_MIN)
		sprintf_s(chText, CHTEXT_MAX, "*");
	else
		sprintf_s(chText, CHTEXT_MAX, "%d", nVal);

	CString strTmp;
	pWnd->GetDlgItem(nID)->GetWindowTextA(strTmp);
	if(strTmp.Compare(chText) != 0)
		pWnd->SetDlgItemText(nID, chText);
}

int	GetDlgItemIntMulti(CWnd *pWnd, int nID)
{
	int nVal;
	char chText[CHTEXT_MAX];
	pWnd->GetDlgItemText(nID, chText, CHTEXT_MAX);
	if (strcmp(chText, "*") == 0)
		nVal = INT_MIN;
	else
		nVal = atoi(chText);

	return nVal;
}

void SetDlgItemTextEx(CWnd *pWnd, int nID, CString strVal)
{
	CString strTmp;
	pWnd->GetDlgItem(nID)->GetWindowTextA(strTmp);
	if(strTmp.Compare(strVal) != 0)
		pWnd->SetDlgItemText(nID, strVal);
}

CString GetDlgItemTextEx(CWnd *pWnd, int nID)
{
	CString str;
	pWnd->GetDlgItemText(nID, str);
	return str;
}

//////////////////////////////////////////////////////////////////////////
//
// Combo
//
//////////////////////////////////////////////////////////////////////////
int GetDlgItemCombo(CWnd *pWnd, int nID)
{
	CComboBox *pCombo = (CComboBox*)pWnd->GetDlgItem(nID);
	return pCombo->GetCurSel();
}

void SetDlgItemCombo(CWnd *pWnd, int nID, int nVal)
{
	CComboBox *pCombo = (CComboBox*)pWnd->GetDlgItem(nID);
	pCombo->SetCurSel(nVal);
}


//////////////////////////////////////////////////////////////////////////
//
// Checkbox
//
//////////////////////////////////////////////////////////////////////////
bool GetDlgItemCheckBox(CWnd *pWnd, int nID)
{
	CButton *pBtn = (CButton*)pWnd->GetDlgItem(nID);
	return pBtn->GetCheck() == BST_CHECKED;
}

void SetDlgItemCheckBox(CWnd *pWnd, int nID, BOOL bCheck)
{
	CButton *pBtn = (CButton*)pWnd->GetDlgItem(nID);
	pBtn->SetCheck(bCheck ? BST_CHECKED : BST_UNCHECKED);
}


//////////////////////////////////////////////////////////////////////////
//
// Range
//
//////////////////////////////////////////////////////////////////////////
BOOL isRangeInner(int value, int limit_L, int limit_H, const char* pMsg)
{
	if (value >= limit_L && value <= limit_H)
		return TRUE;
	else 
	{
		if (pMsg)
		{
			CString msgStr;
			msgStr.Format("%s Value is over the range (%d ~ %d)", pMsg, limit_L, limit_H);
			AfxMessageBox(msgStr, MB_ICONWARNING | MB_OK); 
		}
		return FALSE;
	}
}

BOOL isRangeInner(double value, double limit_L, double limit_H, const char* pMsg)
{
	if (value >= limit_L && value <= limit_H)
		return TRUE;
	else 
	{
		if (pMsg)
		{
			CString msgStr;
			msgStr.Format("%s Value is over the range (%f ~ %f)", pMsg, limit_L, limit_H);
			AfxMessageBox(msgStr, MB_ICONWARNING | MB_OK); 
		}
		return FALSE;
	}
}

// Child Windows ( Button, Edit, Static ) Set Font 
void ChildWindowSetFont(CWnd *pWnd, CFont *pFont)
{
	static int nButton = 0;
	CWnd *pPrevWnd = pWnd->GetWindow( GW_CHILD );
	while( pPrevWnd )
	{
		char className[100];
		::GetClassName(
			pPrevWnd->m_hWnd,           // handle to window
			className,  // class name
			100       // size of class name buffer
			);

		if (lstrcmp(className,"Button") == 0)
		{
			CButton *pBtn = (CButton*)pPrevWnd;
			pBtn->SetFont(pFont);
		}
		else if (lstrcmp(className,"Edit") == 0)
		{
			CEdit *pBtn = (CEdit*)pPrevWnd;
			pBtn->SetFont(pFont);
		}
		else if (lstrcmp(className,"Static") == 0)
		{
			CStatic *pBtn = (CStatic*)pPrevWnd;
			pBtn->SetFont(pFont);
		};
		pPrevWnd = pPrevWnd->GetWindow( GW_HWNDNEXT );
	}	
}


// Child Windows ( Button, Edit, Static ) Enable
void ChildWindowEnable(CWnd *pWnd, BOOL bEnable)
{
	static int nButton = 0;
	CWnd *pPrevWnd = pWnd->GetWindow( GW_CHILD );
	while( pPrevWnd )
	{
		char className[100];
		::GetClassName(
			pPrevWnd->m_hWnd,           // handle to window
			className,  // class name
			100       // size of class name buffer
			);

		if (lstrcmp(className,"Button") == 0)
		{
			CButton *pBtn = (CButton*)pPrevWnd;
			pBtn->EnableWindow(bEnable);
		}
		else if (lstrcmp(className,"Edit") == 0)
		{
			CEdit *pBtn = (CEdit*)pPrevWnd;
			pBtn->EnableWindow(bEnable);
		}
		else if (lstrcmp(className,"Static") == 0)
		{
			CStatic *pBtn = (CStatic*)pPrevWnd;
			pBtn->EnableWindow(bEnable);
		};
		pPrevWnd = pPrevWnd->GetWindow( GW_HWNDNEXT );
	}	
}

void MoveToCenter(CWnd *pWnd)
{
	CRect rtDesktop;
	pWnd->GetDesktopWindow()->GetClientRect(rtDesktop);
	CRect rtThis;
	pWnd->GetClientRect(rtThis);
	pWnd->SetWindowPos(NULL, (rtDesktop.right - rtThis.right)/2, 
		(rtDesktop.bottom - rtThis.bottom)/2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}


BOOL SleepWithMessaging(DWORD dwMilliseconds, BOOL *pBreakCondition)
{
	DWORD tick = GetTickCount();
	while(GetTickCount() - tick < dwMilliseconds)
	{
		if (pBreakCondition)
		{
			if (*pBreakCondition == TRUE)
			{
				return TRUE;
			}
		}
		MSG msg;

		if (::PeekMessage(&msg, (HWND)NULL, (UINT)NULL, (UINT)NULL, PM_REMOVE))
		{
			::TranslateMessage((LPMSG)&msg);
			::DispatchMessage((LPMSG)&msg);
		}
	}
	return FALSE;
}

BOOL SleepWithBreak(DWORD dwMilliseconds, BOOL *pBreakCondition, BOOL bCheckValue)
{
	DWORD tick = GetTickCount();
	while(GetTickCount() - tick < dwMilliseconds)
	{
		if (pBreakCondition)
		{
			if (*pBreakCondition == bCheckValue)
			{
				return TRUE;
			}
		}
		Sleep(1);
	}
	return FALSE;
}

CString gStringFormat(LPCTSTR fmtstr, ...)
{
	TCHAR buffer[4096];
	ZeroMemory(buffer, sizeof(buffer));
	try
	{
		va_list argptr;
		va_start(argptr, fmtstr);
		_vstprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR), fmtstr, argptr);
		va_end(argptr);
	}
	catch (...)
	{
		//...
	}

	CString str;
	str.SetString(buffer);
	return str;
}

