/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }
    
    int n = atoi("agrv[1]");
        
    if (n < 1 && n > 100)
    {
        printf("n shoud be in the range of 1 to 100\n");
        return 2;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // update BITMAPFILEHEADER and BITMAPINFOHEADER 
    
    //resizing Width
    int newWidth = bi.biWidth * n;
    
    //resizing Height 
    int newHeight = abs(bi.biHeight) * n;
    
    //updating padding 
    int oldpadding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int newpadding =  (4 - (newWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    //Updating ImageSize 
    bi.biSizeImage = abs(newHeight) * ((newWidth * sizeof(RGBTRIPLE)) + newpadding);
    
    //Updating Size
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // resize verticaly 
        for (int y = 0; y < n; y++)
        {
        
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // resize horizontaly 
                for (int x  = 0; x < n; x ++)
                {
                    
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);                
                }
            }

            // skip over padding in infile
            fseek(inptr, oldpadding, SEEK_CUR);

            // then add it back to outfile, if needed 
            for (int k = 0; k < newpadding; k++)
            {
                fputc(0x00, outptr);
            }            
        }

    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
