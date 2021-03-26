// Copies a BMP file
// I'v finished applying horizontal rescaling, vertical left.

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage:  scale_factor copy infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];
    int factor = atoi(argv[1]);

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }
    FILE *outcopy = fopen("imcopy.bmp", "w");

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    BITMAPFILEHEADER new_bf;
    BITMAPINFOHEADER new_bi;

    fseek(inptr, 0, SEEK_SET);

    fread(&new_bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    fread(&new_bi, sizeof(BITMAPINFOHEADER), 1, inptr);


    new_bi.biHeight *= factor;
    new_bi.biWidth *= factor;
    int new_padding = (4 - (new_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    new_bi.biSizeImage = (((sizeof(RGBTRIPLE) * new_bi.biWidth) + new_padding) * abs(new_bi.biHeight));
    new_bf.bfSize = new_bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }


    // write outfile's BITMAPFILEHEADER
    fwrite(&new_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&new_bi, sizeof(BITMAPINFOHEADER), 1, outptr);



    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int counter;

    long loc = 0;
    printf("%ld\n", loc);
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        for (int m = 0; m < factor; m++)
        {
            for (int j = 0; j < bi.biWidth; j++)
            {
                RGBTRIPLE temp;
                if (j == 0)
                {
                    loc = ftell(inptr);
                }
                fread(&temp, sizeof(RGBTRIPLE), 1, inptr);
                for (int k = 0; k < factor; k++)
                {
                    fwrite(&temp, sizeof(RGBTRIPLE), 1, outptr);
                }

            }
            for (int l = 0; l < new_padding; l++)
            {
                fputc(0x00, outptr);
            }
            if (m != factor - 1)
            {
                fseek(inptr, loc, SEEK_SET);
            }
        }
        fseek(inptr, padding, SEEK_CUR);



        // skip over padding, if any

        // then add it back (to demonstrate how)

    }


    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
