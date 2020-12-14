/////////////////////////////
// Hyperlink.h

#ifndef TRANSPARENT_STATIC_H
#define TRANSPARENT_STATIC_H

class CTransparentStatic :	public CStatic
{
public:
	CTransparentStatic();
	virtual ~CTransparentStatic();

protected:
	virtual LRESULT OnMessageReflect(UINT uMsg, WPARAM wParam, LPARAM lParam);

};

#endif // HYPERLINK_H

