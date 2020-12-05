#pragma once

#define range(val, minval, maxval)			min(max(val, minval), maxval)

// Double
void SetWindowDouble(CWnd *pWndTarget, double dVal, int nDigits = 3);
void SetDlgItemDouble(CWnd *pWnd, int nID, double dVal, int nDigits = 3);
void SetDlgItemDoubleMulti(CWnd *pWnd, int nID, double dVal, int nDigits = 3);
double GetWindowDouble(CWnd *pWndTarget);
double GetDlgItemDouble(CWnd *pWnd, int nID);
double GetDlgItemDoubleMulti(CWnd *pWnd, int nID);
void EnableWindowEx(CWnd *pWnd, int nID, bool bVal);
void EnableWindowEx(CWnd *pWnd, bool bVal);
void ShowWindowEx(CWnd *pWnd, int nID, bool bVal);
// Int
void SetDlgItemIntEx(CWnd *pWnd, int nID, int nVal, int nDigits = 1); // 음수까지 설정 가능하다. nDigit is front zero count
int GetDlgItemIntEx(CWnd *pWnd, int nID);
void SetDlgItemIntMulti(CWnd *pWnd, int nID, int nVal);
int	GetDlgItemIntMulti(CWnd *pWnd, int nID);

// CString
void SetDlgItemTextEx(CWnd *pWnd, int nID, CString strVal);
CString GetDlgItemTextEx(CWnd *pWnd, int nID);

// Combo
int GetDlgItemCombo(CWnd *pWnd, int nID);
void SetDlgItemCombo(CWnd *pWnd, int nID, int nVal);

// CheckBox
bool GetDlgItemCheckBox(CWnd *pWnd, int nID);
void SetDlgItemCheckBox(CWnd *pWnd, int nID, BOOL bCheck);

// Range
BOOL isRangeInner(int value, int limit_L, int limit_H, const char* pMsg);
BOOL isRangeInner(double value, double limit_L, double limit_H, const char* pMsg);

// Child Windows ( Button, Edit, Static ) Set Font 
void ChildWindowSetFont(CWnd *pWnd, CFont *pFont);
void ChildWindowEnable(CWnd *pWnd, BOOL bEnable);

void MoveToCenter(CWnd *pWnd);

// Sleep with processing windows message.
// return TRUE if pBreakCondition is TRUE
// return FALSE if dwMilliseconds is out
BOOL SleepWithMessaging(DWORD dwMilliseconds, BOOL *pBreakCondition = NULL);
BOOL SleepWithBreak(DWORD dwMilliseconds, BOOL *pBreakCondition = NULL, BOOL bCheckValue = TRUE);


CString gStringFormat(LPCTSTR fmtstr, ...);