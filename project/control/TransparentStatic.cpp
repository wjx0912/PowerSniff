/////////////////////////////
// Hyperlink.cpp

#include "stdafx.h"
#include "TransparentStatic.h"

CTransparentStatic::CTransparentStatic()
{
}

CTransparentStatic::~CTransparentStatic()
{
}

LRESULT CTransparentStatic::OnMessageReflect(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	// Messages such as WM_CTLCOLORSTATIC are reflected back to the CWnd object that created them.
	if (uMsg ==  WM_CTLCOLORSTATIC)
	{
		CDC dc((HDC)wParam);
		dc.SetBkMode(TRANSPARENT);

		// Return a NULL brush for a transparent background
		return (LRESULT)GetStockObject(NULL_BRUSH);
	}
	return 0L;
}

