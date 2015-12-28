
// BmpShowView.cpp : CBmpShowView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "BmpShow.h"
#endif

#include "BmpShowDoc.h"
#include "BmpShowView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBmpShowView

IMPLEMENT_DYNCREATE(CBmpShowView, CView)

BEGIN_MESSAGE_MAP(CBmpShowView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CBmpShowView ����/�Ҹ�

CBmpShowView::CBmpShowView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CBmpShowView::~CBmpShowView()
{
}

BOOL CBmpShowView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CBmpShowView �׸���

void CBmpShowView::OnDraw(CDC* /*pDC*/)
{
	CBmpShowDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CBmpShowView �μ�

BOOL CBmpShowView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CBmpShowView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CBmpShowView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CBmpShowView ����

#ifdef _DEBUG
void CBmpShowView::AssertValid() const
{
	CView::AssertValid();
}

void CBmpShowView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBmpShowDoc* CBmpShowView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBmpShowDoc)));
	return (CBmpShowDoc*)m_pDocument;
}
#endif //_DEBUG


// CBmpShowView �޽��� ó����

#define DIB_HEADER_MARKER ((WORD) ('M' << 8) | 'B')

void CBmpShowView::OnLButtonDown(UINT nFlags, CPoint point)
{
	FILE* fp = NULL;
	fopen_s(&fp, "lenna.bmp", "rb");
	if (!fp)
		return;

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;

	// BITMAPFILEHEADER �б�
	if (fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, fp) != 1) {
		fclose(fp);
		return;
	}

	// BMP �������� ��Ÿ���� 'BM' ��Ŀ�� �ִ��� Ȯ��
	if (bmfh.bfType != DIB_HEADER_MARKER) {
		fclose(fp);
		return;
	}

	// BITMAPINFOHEADER �б�
	if (fread(&bmih, sizeof(BITMAPINFOHEADER), 1, fp) != 1) {
		fclose(fp);
		return;
	}

	LONG nWidth = bmih.biWidth;
	LONG nHeight = bmih.biHeight;
	WORD nBitCount = bmih.biBitCount;

	DWORD dwWidthStep = (DWORD)((nWidth * nBitCount / 8 + 3) & ~3);
	DWORD dwSizeImage = nHeight * dwWidthStep;

	// DIB ���� ��ü ũ�� ��� (BITMAPINFOHEADER + ���� ���̺� + �ȼ� ������)
	DWORD dwDibSize;
	if (nBitCount == 24)
		dwDibSize = sizeof(BITMAPINFOHEADER) + dwSizeImage;
	else
		dwDibSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << nBitCount) + dwSizeImage;

	// ���Ϸκ��� DIB ������ �о ������ �޸� ���� �Ҵ�
	BYTE* pDib = new BYTE[dwDibSize];

	if (pDib == NULL)
	{
		fclose(fp);
		return;
	}

	// ���Ϸκ��� Packed-DIB ũ�⸸ŭ�� �б�
	fseek(fp, sizeof(BITMAPFILEHEADER), SEEK_SET);
	if (fread(pDib, sizeof(BYTE), dwDibSize, fp) != dwDibSize)
	{
		delete[] pDib;
		pDib = NULL;
		fclose(fp);
		return;
	}

	// �ȼ� ������ ���� ��ġ ���
	LPVOID lpvBits;
	if (nBitCount == 24)
		lpvBits = pDib + sizeof(BITMAPINFOHEADER);
	else
		lpvBits = pDib + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << nBitCount);

	// DIB ȭ�� ���
	CClientDC dc(this);
	::SetDIBitsToDevice(dc.m_hDC, point.x, point.y, nWidth, nHeight, 0, 0, 0,
		nHeight, lpvBits, (BITMAPINFO*)pDib, DIB_RGB_COLORS);

	// �޸� ���� �� ���� �ݱ�
	delete[] pDib;
	fclose(fp);

	CView::OnLButtonDown(nFlags, point);
}
