#include "win32/ClientDeviceContext.h"
#include "win32/Splitter.h"

#define CLSNAME		_T("CSplitter")

using namespace Framework;
using namespace Framework::Win32;

CSplitter::CSplitter(HWND hParent, const RECT& Rect, HCURSOR nCursor, unsigned int nEdgePosition)
{
	m_nChild[0] = NULL;
	m_nChild[1] = NULL;

	m_nCursor		= nCursor;
	m_nEdgePosition = nEdgePosition;

	if(!DoesWindowClassExist(CLSNAME))
	{
		WNDCLASSEX w;
		memset(&w, 0, sizeof(WNDCLASSEX));
		w.cbSize		= sizeof(WNDCLASSEX);
		w.lpfnWndProc	= CWindow::WndProc;
		w.lpszClassName	= CLSNAME;
		w.hbrBackground	= NULL;
		w.hInstance		= GetModuleHandle(NULL);
		w.hCursor		= LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&w);
	}

	Create(NULL, CLSNAME, _T(""), WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN, &Rect, hParent, NULL);
	SetClassPtr();
}

CSplitter::~CSplitter()
{

}

void CSplitter::SetChild(unsigned int nIndex, HWND hWnd)
{
	m_nChild[nIndex] = hWnd;
	ResizeChild(nIndex);
}

void CSplitter::SetEdgePosition(double nFraction)
{
	short nX, nY;
	RECT ClientRect;

	GetClientRect(&ClientRect);
	nX = (short)((double)ClientRect.right * nFraction);
	nY = (short)((double)ClientRect.bottom * nFraction);

	UpdateEdgePosition(nX, nY);

	ResizeChild(0);
	ResizeChild(1);
}

long CSplitter::OnSize(unsigned int nX, unsigned int nY, unsigned int nType)
{
	ResizeChild(0);
	ResizeChild(1);
	return TRUE;
}

long CSplitter::OnMouseMove(WPARAM wParam, int nX, int nY)
{
	if(GetCapture() == m_hWnd)
	{
		UpdateEdgePosition(nX, nY);

		ResizeChild(0);
		ResizeChild(1);
	}

	SetCursor(m_nCursor);
	return TRUE;
}

long CSplitter::OnLeftButtonDown(int nX, int nY)
{
	SetCapture(m_hWnd);
	SetCursor(m_nCursor);

	return TRUE;
}

long CSplitter::OnLeftButtonUp(int nX, int nY)
{
	ReleaseCapture();
	return TRUE;
}

long CSplitter::OnNotify(WPARAM wParam, NMHDR* pH)
{
	return (long)SendMessage(GetParent(), WM_NOTIFY, wParam, (LPARAM)pH);
}

void CSplitter::ResizeChild(unsigned int nIndex)
{
	if(m_nChild[nIndex] == NULL) return;

	RECT PaneRect;

	GetPaneRect(&PaneRect, nIndex);

	SetWindowPos(m_nChild[nIndex], NULL, PaneRect.left, PaneRect.top,
		PaneRect.right - PaneRect.left,
		PaneRect.bottom - PaneRect.top,
		SWP_NOZORDER);
}

void CSplitter::Paint(HDC hDC)
{
	CDeviceContext PaintDC(hDC);
	RECT EdgeRect;

	GetEdgeRect(&EdgeRect);
	FillRect(PaintDC, &EdgeRect, (HBRUSH)COLOR_WINDOW);
}