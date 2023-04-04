#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;


// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader
typedef struct tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;


// https://docs.microsoft.com/pl-pl/previous-versions/dd183376(v=vs.85)
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;


void read_file_header(FILE *fp, BITMAPFILEHEADER *bfh) {
  fread(&bfh->bfType, sizeof(bfh->bfType), 1, fp);
  fread(&bfh->bfSize, sizeof(bfh->bfSize), 1, fp);
  fread(&bfh->bfReserved1, sizeof(bfh->bfReserved1), 1, fp);
  fread(&bfh->bfReserved2, sizeof(bfh->bfReserved2), 1, fp);
  fread(&bfh->bfOffBits, sizeof(bfh->bfOffBits), 1, fp);
}


void read_info_header(FILE *fp, BITMAPINFOHEADER *bih) {
  fread(&bih->biSize, sizeof(bih->biSize), 1, fp);
  fread(&bih->biWidth, sizeof(bih->biWidth), 1, fp);
  fread(&bih->biHeight, sizeof(bih->biHeight), 1, fp);
  fread(&bih->biPlanes, sizeof(bih->biPlanes), 1, fp);
  fread(&bih->biBitCount, sizeof(bih->biBitCount), 1, fp);
  fread(&bih->biCompression, sizeof(bih->biCompression), 1, fp);
  fread(&bih->biSizeImage, sizeof(bih->biSizeImage), 1, fp);
  fread(&bih->biXPelsPerMeter, sizeof(bih->biXPelsPerMeter), 1, fp);
  fread(&bih->biYPelsPerMeter, sizeof(bih->biYPelsPerMeter), 1, fp);
  fread(&bih->biClrUsed, sizeof(bih->biClrUsed), 1, fp);
  fread(&bih->biClrImportant, sizeof(bih->biClrImportant), 1, fp);
}


void print_file_header(BITMAPFILEHEADER *bfh) {
  printf("BITMAPFILEHEADER:\n");
  printf("  bfType:\t\t%X\n", bfh->bfType);
  printf("  bfSize:\t\t%d\n", bfh->bfSize);
  printf("  bfReserved2:\t\t%d\n", bfh->bfReserved2);
  printf("  bfReserved1:\t\t%d\n", bfh->bfReserved1);
  printf("  bfOffBits:\t\t%d\n", bfh->bfOffBits);
}


void print_info_header(BITMAPINFOHEADER *bih) {
  printf("\nBITMAPINFOHEADER:\n");
  printf("  biSize:\t\t%d\n", bih->biSize);
  printf("  biWidth:\t\t%d\n", bih->biWidth);
  printf("  biHeight:\t\t%d\n", bih->biHeight);
  printf("  biPlanes:\t\t%d\n", bih->biPlanes);
  printf("  biBitCount:\t\t%d\n", bih->biBitCount);
  printf("  biCompression:\t%d\n", bih->biCompression);
  printf("  biSizeImage:\t\t%d\n", bih->biSizeImage);
  printf("  biXPelsPerMeter:\t%d\n", bih->biXPelsPerMeter);
  printf("  biYPelsPerMeter:\t%d\n", bih->biYPelsPerMeter);
  printf("  biClrUsed:\t\t%d\n", bih->biClrUsed);
  printf("  biClrImportant:\t%d\n\n", bih->biClrImportant);
}


void print_histogram(BITMAPINFOHEADER *bih, unsigned char *image, int row_size) {
  int intervals[17] = {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 256};
  long pixel_bgr_count[16 * 3] = {0};
  char colors[3] = {'B', 'G', 'R'};

  for (int i = 0; i < bih->biHeight; i++) {
    for (int j = 0; j < bih->biWidth; j++) { // iterating over pixels
      for(int k = 0; k < 16; k++) { // iterating over interval values
        if((image[(i * row_size) + (j * 3)] < intervals[k+1]) && (image[(i * row_size) + (j * 3)] >= intervals[k]))  { // count blue
          pixel_bgr_count[k] = pixel_bgr_count[k] + 1;
          
        }  
        if((image[(i * row_size) + (j * 3) + 1] < intervals[k+1]) && (image[(i * row_size) + (j * 3) + 1] >= intervals[k]))  { //count green
          pixel_bgr_count[k + 16] = pixel_bgr_count[k + 16] + 1;
          
        }
        if((image[(i * row_size) + (j * 3) + 2] < intervals[k+1]) && (image[(i * row_size) + (j * 3) + 2] >= intervals[k]))  { //count red
          pixel_bgr_count[k + 32] = pixel_bgr_count[k + 32] + 1;
        }  
      }
    }
  }
  for(int i = 0; i < 3; i++) { // printing the intervals and percentages
    printf("%c:\n", colors[i]);
    for(int j = 0; j < 16; j++) {
      float percentage = ((float)pixel_bgr_count[j + i * 16] / (bih->biHeight * row_size / 3)) * 100.0;
      printf("  %d - %d:\t%.2f %%\n", intervals[j], (intervals[j+1] - 1), percentage);
    }
  }
}


void write_file_header(FILE *fp, BITMAPFILEHEADER *bfh) {
  fwrite(&bfh->bfType, sizeof(bfh->bfType), 1, fp);
  fwrite(&bfh->bfSize, sizeof(bfh->bfSize), 1, fp);
  fwrite(&bfh->bfReserved1, sizeof(bfh->bfReserved1), 1, fp);
  fwrite(&bfh->bfReserved2, sizeof(bfh->bfReserved2), 1, fp);
  fwrite(&bfh->bfOffBits, sizeof(bfh->bfOffBits), 1, fp);
}


void write_info_header(FILE *fp, BITMAPINFOHEADER *bih) {
  fwrite(&bih->biSize, sizeof(bih->biSize), 1, fp);
  fwrite(&bih->biWidth, sizeof(bih->biWidth), 1, fp);
  fwrite(&bih->biHeight, sizeof(bih->biHeight), 1, fp);
  fwrite(&bih->biPlanes, sizeof(bih->biPlanes), 1, fp);
  fwrite(&bih->biBitCount, sizeof(bih->biBitCount), 1, fp);
  fwrite(&bih->biCompression, sizeof(bih->biCompression), 1, fp);
  fwrite(&bih->biSizeImage, sizeof(bih->biSizeImage), 1, fp);
  fwrite(&bih->biXPelsPerMeter, sizeof(bih->biXPelsPerMeter), 1, fp);
  fwrite(&bih->biYPelsPerMeter, sizeof(bih->biYPelsPerMeter), 1, fp);
  fwrite(&bih->biClrUsed, sizeof(bih->biClrUsed), 1, fp);
  fwrite(&bih->biClrImportant, sizeof(bih->biClrImportant), 1, fp);
}


void write_pixels(BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih, FILE *fp, unsigned char *image, int row_size) {
  // write to bmp row by row
  for (int i = 0; i < bih->biHeight; i++) {
    fseek(fp, bfh->bfOffBits + i * row_size, SEEK_SET); // seek to the beginning of the current row
    fwrite(&image[i * row_size], row_size, 1, fp); // write the current row
  }
}


void grayscale(BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih, unsigned char *image, int row_size, const char* file) {
  FILE* gray_file = fopen(file, "wb");
  if(gray_file == NULL) {
    printf("Failed to create grayscale image\n");
    return;
  }
  unsigned char *gray_image = (unsigned char*) malloc(row_size * bih->biHeight); // allocate memory for the grayscale pixels

  for(int i = 0; i < bih->biHeight; i++) { // modify the values of pixels
    for(int j = 0; j < bih->biWidth; j++) {
      gray_image[(i * row_size) + (j * 3)]  = (image[(i * row_size) + (j * 3)] + image[(i * row_size) + (j * 3) + 1] + image[(i * row_size) + (j * 3) + 2]) / 3; // blue
      gray_image[(i * row_size) + (j * 3) + 1]  = (image[(i * row_size) + (j * 3)] + image[(i * row_size) + (j * 3) + 1] + image[(i * row_size) + (j * 3) + 2]) / 3; // green
      gray_image[(i * row_size) + (j * 3) + 2]  = (image[(i * row_size) + (j * 3)] + image[(i * row_size) + (j * 3) + 1] + image[(i * row_size) + (j * 3) + 2]) / 3; // red
    }
  }

  write_file_header(gray_file, bfh);
  write_info_header(gray_file, bih);
  write_pixels(bfh, bih, gray_file, gray_image, row_size);

  free(gray_image);
  fclose(gray_file);
}


int main(int argc, char *argv[]) {
  char *in = argv[1]; // stores the path to input bmp file
  char *out = argv[2]; // store the path to output bmp file

  FILE *fp;
  BITMAPFILEHEADER bfh;
  BITMAPINFOHEADER bih;

  fp = fopen(in, "rb");  // open bmp file in binary read mode
  if (fp == NULL) {
    printf("ERROR: Failed to open BMP file.\n");
    return 1;
  }

// read and print file and info header
 read_file_header(fp, &bfh);
 read_info_header(fp, &bih);
 print_file_header(&bfh);
 print_info_header(&bih);

// histogram
  if ((bih.biCompression != 0) || (bih.biBitCount != 24)) {
    printf("Histogram calculation is not supported for this image.");
    return 1;
  }

  int row_size = ((bih.biWidth * bih.biBitCount + 31) / 32) * 4; 
  unsigned char *image = (unsigned char*) malloc(row_size * bih.biHeight); // allocate space to load in pixel data

  // read bmp row by row
  for (int i = 0; i < bih.biHeight; i++) {
    fseek(fp, bfh.bfOffBits + i * row_size, SEEK_SET); // seek to the beginning of the current row
    fread(&image[i * row_size], row_size, 1, fp); // read the current row
  }

  fclose(fp);

  print_histogram(&bih, image, row_size);

// grayscale
  grayscale(&bfh, &bih, image, row_size, out);


  free(image);
  
  return 0;
}