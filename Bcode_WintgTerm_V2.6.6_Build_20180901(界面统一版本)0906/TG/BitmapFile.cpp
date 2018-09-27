//	MURUGAN@NDR	(2002/05/16)
//	CBitmapFile class cpp file implementation
//	Functions to create a BMP/JPEG file from a window and desktop

#include "stdafx.h"
#include "BitmapFile.h"


#ifdef __cplusplus
	extern "C" {
#endif // __cplusplus

#include "jpeglib.h"

#ifdef __cplusplus
	}
#endif // __cplusplus

#include <setjmp.h>

struct ima_error_mgr
{
  struct  jpeg_error_mgr pub;	//	"public" fields
  jmp_buf setjmp_buffer;		//	for return to caller
};


inline int Grey( int r, int g, int b )
{
    return ((( b * 11 ) + ( g * 59 ) + ( r * 30 )) / 100 );
}


CBitmapFile::CBitmapFile()
{
}


CBitmapFile::~CBitmapFile()
{
}


BOOL CBitmapFile::SaveDesktopAsFile(LPTSTR szFile, int nFileType)
{
	CString		strFileName, strError = "";
	CWnd* pWnd = CWnd::GetDesktopWindow(); 
	CBitmap 	bitmap;
	CWindowDC	dc(pWnd);
	CDC 		memDC;
	CRect		rect;

	strFileName = (CString)szFile;
	memDC.CreateCompatibleDC(&dc); 

	pWnd->GetWindowRect(rect);

	bitmap.CreateCompatibleBitmap(&dc, rect.Width(),rect.Height() );
	
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.BitBlt(0, 0, rect.Width(),rect.Height(), &dc, 0, 0, SRCCOPY); 

	// Create logical palette if device support a palette
	CPalette pal;
	if( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE )
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		pLP->palVersion = 0x300;

		pLP->palNumEntries = 
			GetSystemPaletteEntries( dc, 0, 255, pLP->palPalEntry );

		// Create the palette
		pal.CreatePalette( pLP );

		delete[] pLP;
	}


	// Convert the bitmap to a DIB
	HANDLE hDIB = DDBToDIB( bitmap, BI_RGB, &pal );

	if( hDIB == NULL )
		return FALSE;

//	DrawGreyScale( &memDC, hDIB );

	memDC.SelectObject(pOldBitmap);

	// Write it to file
	// Write it to file
	if(nFileType == BMP_FILE)
		WriteDIB( szFile, hDIB );
	else
		JpegFromDib(hDIB, 100, strFileName, &strError);

	// Free the memory allocated by DDBToDIB for the DIB
	GlobalFree( hDIB );
	return TRUE;
}


BOOL CBitmapFile::SaveWindowAsFile(LPTSTR szFile, CWnd *pWnd, int nFileType)
{
	CString		strFileName, strError = "";
	CBitmap 	bitmap;
	CWindowDC	dc(pWnd);
	CDC 		memDC;
	CRect		rect;

	strFileName = (CString)szFile;
	memDC.CreateCompatibleDC(&dc); 

	pWnd->GetWindowRect(rect);

	bitmap.CreateCompatibleBitmap(&dc, rect.Width(),rect.Height() );
	
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.BitBlt(0, 0, rect.Width(),rect.Height(), &dc, 0, 0, SRCCOPY); 

	// Create logical palette if device support a palette
	CPalette pal;
	if( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE )
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		pLP->palVersion = 0x300;

		pLP->palNumEntries = 
			GetSystemPaletteEntries( dc, 0, 255, pLP->palPalEntry );

		// Create the palette
		pal.CreatePalette( pLP );

		delete[] pLP;
	}

	memDC.SelectObject(pOldBitmap);

	// Convert the bitmap to a DIB
	HANDLE hDIB = DDBToDIB( bitmap, BI_RGB, &pal );

	if( hDIB == NULL )
		return FALSE;

	// Write it to file
	if(nFileType == BMP_FILE)
		WriteDIB( szFile, hDIB );
	else
		JpegFromDib(hDIB, 100, strFileName, &strError);

	// Free the memory allocated by DDBToDIB for the DIB
	GlobalFree( hDIB );

	return TRUE;
}

// DDBToDIB		- Creates a DIB from a DDB
// bitmap		- Device dependent bitmap
// dwCompression	- Type of compression - see BITMAPINFOHEADER
// pPal			- Logical palette
HANDLE CBitmapFile::DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal , int nWidth) 
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;


	ASSERT( bitmap.GetSafeHandle() );

	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE) pPal->GetSafeHandle();
	if (hPal==NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize		= sizeof(BITMAPINFOHEADER);
	if(nWidth == 0)
	    bi.biWidth		= bm.bmWidth;
	else
		bi.biWidth		= nWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;			//bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 ) 
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device m_context to get the DIB from
	hDC = GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB){
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
			(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0){
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
						* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
				0L,				// Start scan line
				(DWORD)bi.biHeight,		// # of scan lines
				(LPBYTE)lpbi 			// address for bitmap bits
				+ (bi.biSize + nColors * sizeof(RGBQUAD)),
				(LPBITMAPINFO)lpbi,		// address of bitmapinfo
				(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);
		
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	ReleaseDC(NULL,hDC);
	return hDIB;
}


BOOL CBitmapFile::WriteDIB( LPTSTR szFile, HANDLE hDIB)
{
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;

	if (!hDIB)
		return FALSE;

	CFile file;
	if( !file.Open( szFile, CFile::modeWrite|CFile::modeCreate) )
		return FALSE;

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = 1 << lpbi->biBitCount;

	// Fill in the fields of the file header 
	hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize		= GlobalSize (hDIB) + sizeof( hdr );
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= (DWORD) (sizeof( hdr ) + lpbi->biSize +
						nColors * sizeof(RGBQUAD));

	// Write the file header 
	file.Write( &hdr, sizeof(hdr) );

	// Write the DIB header and the bits 
	file.Write( lpbi, GlobalSize(hDIB) );

	return TRUE;
}


// DrawGreyScale    - Draws a bitmap in Grey scale
// pDC              - Pointer to target device m_context
// hDIB             - Handle of device-independent bitmap
//
void CBitmapFile::DrawGreyScale( CDC *pDC, HANDLE hDIB )
{
    CPalette pal;
    CPalette *pOldPalette = 0;

    BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
    
    int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 
                1 << bmInfo.bmiHeader.biBitCount;
    
    int nWidth = bmInfo.bmiHeader.biWidth;
    int nHeight = bmInfo.bmiHeader.biHeight;
    
    // Compute the address of the bitmap bits
    LPVOID lpDIBBits;
    if( bmInfo.bmiHeader.biBitCount > 8 )
        lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + 
            bmInfo.bmiHeader.biClrUsed) + 
            ((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
    else
        lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
    
    // Create the palette if needed
    if( pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE && nColors <= 256 )
    {
        // The device supports a palette and bitmap has color table
        
        // Allocate memory for a palette
        UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
        LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

        pLP->palVersion = 0x300;
        pLP->palNumEntries = nColors;
        
        for( int i=0; i < nColors; i++)
        {
            int nGrey = Grey( bmInfo.bmiColors[i].rgbRed, bmInfo.bmiColors[i].rgbGreen,
                bmInfo.bmiColors[i].rgbBlue );
            pLP->palPalEntry[i].peRed = nGrey;
            pLP->palPalEntry[i].peGreen = nGrey;
            pLP->palPalEntry[i].peBlue = nGrey;
            pLP->palPalEntry[i].peFlags = 0;
        }
        
        pal.CreatePalette( pLP );
        
        delete[] pLP;

        // Select the palette
        pOldPalette = pDC->SelectPalette(&pal, FALSE);
        pDC->RealizePalette();
    }
    else if((pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) == 0 && nColors <= 256 )
    {
        // Device does not supports palettes but bitmap has a color table
        // Modify the bitmaps color table directly
        // Note : This ends up changing the DIB. If that is not acceptable then
        // copy the DIB and then change the copy rather than the original

        for( int i=0; i < nColors; i++)
        {
            int nGrey = Grey( bmInfo.bmiColors[i].rgbRed, bmInfo.bmiColors[i].rgbGreen,
                bmInfo.bmiColors[i].rgbBlue );
            bmInfo.bmiColors[i].rgbRed = nGrey;
            bmInfo.bmiColors[i].rgbGreen = nGrey;
            bmInfo.bmiColors[i].rgbBlue = nGrey;
        }
    }
    else
    {
        // Modify the bitmaps pixels directly
        // Note : This ends up changing the DIB. If that is not acceptable then
        // copy the DIB and then change the copy rather than the original
        if ( bmInfo.bmiHeader.biBitCount == 24 )
        {
            BYTE *dst=(BYTE*)lpDIBBits;
            int Size=nWidth*nHeight;

            while ( Size-- )
            {
                int nGrey = Grey( dst[2], dst[1], dst[0] );

                dst[0]=(BYTE)nGrey;
                dst[1]=(BYTE)nGrey;
                dst[2]=(BYTE)nGrey;
                dst+=4;
            }
        }
        else if ( bmInfo.bmiHeader.biBitCount == 16 )
        {
            WORD *dst=(WORD*)lpDIBBits;
            int Size=nWidth*nHeight;

            while ( Size-- )
            {
                BYTE b = (BYTE)((*dst)&(0x1F));
                BYTE g = (BYTE)(((*dst)>>5)&(0x1F));
                BYTE r = (BYTE)(((*dst)>>10)&(0x1F));

                int nGrey = Grey( r, g, b );

                *dst++ = ((WORD)(((BYTE)(nGrey)|((WORD)((BYTE)(nGrey))<<5))|(((DWORD)(BYTE)(nGrey))<<10)));
            }
        }
    }

    // Draw the image 
    ::SetDIBitsToDevice(pDC->m_hDC,    // hDC
        0,                             // XDest
        0,                             // YDest
        nWidth,                        // nDestWidth
        nHeight,                       // nDestHeight
        0,                             // XSrc
        0,                             // YSrc
        0,                             // nStartScan
        nHeight,                       // nNumScans
        lpDIBBits,                     // lpBits
        (LPBITMAPINFO)hDIB,            // lpBitsInfo
        DIB_RGB_COLORS);               // wUsage
    
    
    if ( pOldPalette )
        pDC->SelectPalette(pOldPalette, FALSE);
}

////////////////////////////////////////////////////////////////////////////
//This function takes the contents of a DIB
//and turns it into a JPEG file.
//
//The DIB may be monochrome, 16-color, 256-color, or 24-bit color.
//
//Any functions or data items beginning with "jpeg_" belong to jpeg.lib,
//and are not included here.
//
//The function assumes 3 color components per pixel.
/////////////////////////////////////////////////////////////////////////////
BOOL CBitmapFile::JpegFromDib(	HANDLE     hDib,     //Handle to DIB
								int        nQuality, //JPEG quality (0-100)
								CString    csJpeg,   //Pathname to jpeg file
								CString*   pcsMsg)   //Error msg to return
{
    //Basic sanity checks...
    if (nQuality < 0 || nQuality > 100 ||
        hDib   == NULL ||
        pcsMsg == NULL ||
        csJpeg == "")
    {
        if (pcsMsg != NULL)
            *pcsMsg = "Invalid input data";

        return FALSE;
    }

    *pcsMsg = "";

    byte *buf2 = 0;

    //Use libjpeg functions to write scanlines to disk in JPEG format
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr       jerr;

    FILE*      pOutFile;     //Target file 
    int        nSampsPerRow; //Physical row width in image buffer 
    JSAMPARRAY jsmpArray;    //Pixel RGB buffer for JPEG file

    cinfo.err = jpeg_std_error(&jerr); //Use default error handling (ugly!)

    jpeg_create_compress(&cinfo);

    if ((pOutFile = fopen(csJpeg, "wb")) == NULL)
    {
        *pcsMsg = "Cannot open ";
		*pcsMsg += csJpeg;
        jpeg_destroy_compress(&cinfo);
        return FALSE;
    }

    jpeg_stdio_dest(&cinfo, pOutFile);

    LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)hDib;

    cinfo.image_width      = lpbi->biWidth;  //Image width and height, in pixels 
    cinfo.image_height     = lpbi->biHeight;
    cinfo.input_components = 3;              //Color components per pixel
                                             //(RGB_PIXELSIZE - see jmorecfg.h)
    cinfo.in_color_space   = JCS_RGB; 	     //Colorspace of input image

    jpeg_set_defaults(&cinfo);

    jpeg_set_quality(&cinfo,
                     nQuality, //Quality: 0-100 scale
                     TRUE);    //Limit to baseline-JPEG values

    jpeg_start_compress(&cinfo, TRUE);

    //JSAMPLEs per row in output buffer
    nSampsPerRow = cinfo.image_width * cinfo.input_components; 

    //Allocate array of pixel RGB values
    jsmpArray = (*cinfo.mem->alloc_sarray)
                ((j_common_ptr) &cinfo,
                 JPOOL_IMAGE,
                 nSampsPerRow,
                 cinfo.image_height);

    if (DibToSamps(hDib,
                   nSampsPerRow,
                   cinfo,
                   jsmpArray,
                   pcsMsg))
    {
        //Write the array of scan lines to the JPEG file
        (void)jpeg_write_scanlines(&cinfo,
                                   jsmpArray,
                                   cinfo.image_height);
    }

    jpeg_finish_compress(&cinfo); //Always finish

    fclose(pOutFile);

    jpeg_destroy_compress(&cinfo); //Free resources

    if (*pcsMsg != "")
        return FALSE;

    else
        return TRUE;
}

////////////////////////////////////////////////////////////////
//This function fills a jsmpArray with the RGB values
//for the CBitmap.
//
//It has been improved to handle all legal bitmap formats.
//
//A jsmpArray is a big array of RGB values, 3 bytes per value.
//
//Note that rows of pixels are processed bottom to top:
//The data in the jsamp array must be arranged top to bottom.
////////////////////////////////////////////////////////////////
BOOL CBitmapFile::DibToSamps(HANDLE		hDib,
							int			nSampsPerRow,
							struct		jpeg_compress_struct cinfo,
							JSAMPARRAY	jsmpPixels,
							CString*	pcsMsg)
{
   //Sanity...
   if (hDib == NULL    ||
     nSampsPerRow <= 0 || pcsMsg == NULL) 
   { 
     if (pcsMsg !=NULL) 
        *pcsMsg="Invalid input data"; 
     return FALSE; 
   } 

   int r=0, p=0, q=0, b=0, n=0, 
       nUnused=0, nBytesWide=0, nUsed=0, nLastBits=0, nLastNibs=0, nCTEntries=0,
       nRow=0, nByte=0, nPixel=0;
   BYTE bytCTEnt=0;
   LPBITMAPINFOHEADER pbBmHdr= (LPBITMAPINFOHEADER)hDib; //The bit count tells you the format of the bitmap: //Decide how many entries will be in the color table (if any) 

   switch (pbBmHdr->biBitCount)
   {
      case 1:
         nCTEntries = 2;   //Monochrome
         break;

      case 4:
         nCTEntries = 16;  //16-color
         break;

      case 8:
         nCTEntries = 256; //256-color
         break;

      case 16:
      case 24:
      case 32:
         nCTEntries = 0;   //No color table needed
         break;

      default:
         *pcsMsg = "Invalid bitmap bit count";
         return FALSE; //Unsupported format
   }

   //Point to the color table and pixels
   DWORD     dwCTab = (DWORD)pbBmHdr + pbBmHdr->biSize;
   LPRGBQUAD pCTab  = (LPRGBQUAD)(dwCTab);
   LPSTR     lpBits = (LPSTR)pbBmHdr +
                      (WORD)pbBmHdr->biSize +
                      (WORD)(nCTEntries * sizeof(RGBQUAD));

   //Different formats for the image bits
   LPBYTE   lpPixels = (LPBYTE)  lpBits;
   RGBQUAD* pRgbQs   = (RGBQUAD*)lpBits;
   WORD*    wPixels  = (WORD*)   lpBits;

   //Set up the jsamps according to the bitmap's format.
   //Note that rows are processed bottom to top, because
   //that's how bitmaps are created.
   switch (pbBmHdr->biBitCount)
   {
      case 1:
         nUsed      = (pbBmHdr->biWidth + 7) / 8;
         nUnused    = (((nUsed + 3) / 4) * 4) - nUsed;
         nBytesWide = nUsed + nUnused;
         nLastBits  = 8 - ((nUsed * 8) - pbBmHdr->biWidth);

         for (r=0; r < pbBmHdr->biHeight; r++)
         {
            for (p=0, q=0; p < nUsed; p++) 
            { 
               nRow=(pbBmHdr->biHeight-r-1) * nBytesWide;
               nByte =  nRow + p;

               int nBUsed = (p < (nUsed >> 1) ) ? 8 : nLastBits;
//										^^		Not sure about this symbol Might be problem			   
			   for(b=0; b < nBUsed;b++) 
               { 
                  bytCTEnt = lpPixels[nByte] << b; 
                  bytCTEnt = bytCTEnt >> 7;

                  jsmpPixels[r][q+0] = pCTab[bytCTEnt].rgbRed;
                  jsmpPixels[r][q+1] = pCTab[bytCTEnt].rgbGreen;
                  jsmpPixels[r][q+2] = pCTab[bytCTEnt].rgbBlue;

                  q += 3;
               }
            }
         }
         break;

      case 4:
         nUsed      = (pbBmHdr->biWidth + 1) / 2;
         nUnused    = (((nUsed + 3) / 4) * 4) - nUsed;
         nBytesWide = nUsed + nUnused;
         nLastNibs  = 2 - ((nUsed * 2) - pbBmHdr->biWidth);

         for (r=0; r < pbBmHdr->biHeight;r++)
         {
            for (p=0,q=0; p < nUsed;p++) 
            { 
				nRow=(pbBmHdr->biHeight-r-1) * nBytesWide;
				nByte = nRow + p;

				int nNibbles = (p  >> (4- (n * 4)) );
//								   ^^		Not sure about this symbol. Might be problem.			   

				jsmpPixels[r][q+0] = pCTab[bytCTEnt].rgbRed;
				jsmpPixels[r][q+1] = pCTab[bytCTEnt].rgbGreen;
				jsmpPixels[r][q+2] = pCTab[bytCTEnt].rgbBlue;

				q += 3;
            }
         }
         break;

      default:
      case 8: //Each byte is a pointer to a pixel color
         nUnused = (((pbBmHdr->biWidth + 3) / 4) * 4) -
                   pbBmHdr->biWidth;

         for (r=0;r < pbBmHdr->biHeight; r++)
         {
            for (p=0,q=0; p < pbBmHdr->biWidth; p++,q+=3)
            {
               nRow   = (pbBmHdr->biHeight-r-1) * (pbBmHdr->biWidth + nUnused);
               nPixel =  nRow + p;

               jsmpPixels[r][q+0] = pCTab[lpPixels[nPixel]].rgbRed;
               jsmpPixels[r][q+1] = pCTab[lpPixels[nPixel]].rgbGreen;
               jsmpPixels[r][q+2] = pCTab[lpPixels[nPixel]].rgbBlue;
            }
         }
         break;

      case 16: //Hi-color (16 bits per pixel)
         for (r=0;r < pbBmHdr->biHeight; r++)
         {
            for (p=0,q=0; p < pbBmHdr->biWidth; p++,q+=3)
            {
               nRow    = (pbBmHdr->biHeight-r-1) * pbBmHdr->biWidth;
               nPixel  = nRow + p;

               RGBQUAD quad = QuadFromWord(wPixels[nPixel]);

               jsmpPixels[r][q+0] = quad.rgbRed;
               jsmpPixels[r][q+1] = quad.rgbGreen;
               jsmpPixels[r][q+2] = quad.rgbBlue;
            }
         }
         break;

      case 24:
         nBytesWide =  (pbBmHdr->biWidth*3);
         nUnused    =  (((nBytesWide + 3) / 4) * 4) -
                       nBytesWide;
         nBytesWide += nUnused;

         for (r=0; r < pbBmHdr->biHeight; r++)
         {
            for (p=0,q=0;p < (nBytesWide-nUnused); p+=3,q+=3)
            { 
               nRow = (pbBmHdr->biHeight-r-1) * nBytesWide;
               nPixel  = nRow + p;

               jsmpPixels[r][q+0] = lpPixels[nPixel+2]; //Red
               jsmpPixels[r][q+1] = lpPixels[nPixel+1]; //Green
               jsmpPixels[r][q+2] = lpPixels[nPixel+0]; //Blue
            }
         }
         break;

      case 32:
         for (r=0; r < pbBmHdr->biHeight; r++)
         {
            for (p=0,q=0; p < pbBmHdr->biWidth; p++,q+=3)
            {
               nRow    = (pbBmHdr->biHeight-r-1) *
                          pbBmHdr->biWidth;
               nPixel  = nRow + p;

               jsmpPixels[r][q+0] = pRgbQs[nPixel].rgbRed;
               jsmpPixels[r][q+1] = pRgbQs[nPixel].rgbGreen;
               jsmpPixels[r][q+2] = pRgbQs[nPixel].rgbBlue;
            }
         }
         break;
   }   //end switch

return TRUE;
}
 
////////////////////////////////////////
//This function turns a 16-bit pixel
//into an RGBQUAD value.
////////////////////////////////////////
RGBQUAD CBitmapFile::QuadFromWord(WORD b16)
{
   BYTE bytVals[] =
   {
     0,  16, 24, 32,  40, 48, 56, 64,
     72, 80, 88, 96, 104,112,120,128,
     136,144,152,160,168,176,184,192,
     200,208,216,224,232,240,248,255
   };

   WORD wR = b16;
   WORD wG = b16;
   WORD wB = b16;

   wR <<= 1; wR >>= 11;
   wG <<= 6; wG >>= 11;
   wB <<= 11; wB >>= 11;

   RGBQUAD rgb;

   rgb.rgbReserved = 0;
   rgb.rgbBlue     = bytVals[wB];
   rgb.rgbGreen    = bytVals[wG];
   rgb.rgbRed      = bytVals[wR];

   return rgb;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CBitmapFile::SaveBitmapAsJpgFile(CDC* dc, CBitmap *bmp, LPTSTR szFile, int nWidth)
{
	CString	strFileName, strError = "";
	strFileName = CString(szFile);
		
	// Create logical palette if device support a palette
	CPalette pal;
	if( dc->GetDeviceCaps(RASTERCAPS) & RC_PALETTE )
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		pLP->palVersion = 0x300;
		
		pLP->palNumEntries = 
			GetSystemPaletteEntries(*(CWindowDC*)dc, 0, 255, pLP->palPalEntry );
		
		// Create the palette
		pal.CreatePalette( pLP );
		
		delete[] pLP;
	}
	
	
	// Convert the bitmap to a DIB
	HANDLE hDIB = DDBToDIB( *bmp, BI_RGB, &pal, nWidth);
	
	if( hDIB == NULL )
		return FALSE;
		
	JpegFromDib(hDIB, 75, strFileName, &strError);
	
	GlobalFree( hDIB );

	return TRUE;
}
