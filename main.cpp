#ifndef BMPLIB_H
#define BMPLIB_H

const int SIZE = 256;
int filefail=0;

int readGSBMP(const char* filename, unsigned char image[][SIZE],int &filefail);
int writeGSBMP(const char* filename, unsigned char outputImage[][SIZE]);
void showGSBMP(unsigned char inputImage[][SIZE]);

#endif

#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "unistd.h"

using namespace std;
using std::cout;
using std::cin;
using std::endl;

typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;

//#define BMP_BIG_ENDIAN
#define BYTE_SWAP(num) (((num>>24)&0xff) | ((num<<8)&&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000))

typedef struct
{
    uint8    bfType1;
    uint8    bfType2;
    uint32   bfSize;
    uint16    bfReserved1;
    uint16    bfReserved2;
    uint32   bfOffBits;
    uint32   biSize;          // size of structure, in bytes
    uint32   biWidth;         // bitmap width, in pixels
    uint32   biHeight;        // bitmap height, in pixels
    uint16    biPlanes;        // see below
    uint16    biBitCount;      // see below
    uint32   biCompression;   // see below
    uint32   biSizeImage;     // see below
    uint32   biXPelsPerMeter; // see below
    uint32   biYPelsPerMeter; // see below
    uint32   biClrUsed;       // see below
    uint32   biClrImportant;  // see below
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;
uint8 tempGSImage[SIZE][SIZE];
void write_hdr(uint8 *hdr, int *hdr_idx, uint32 data, uint32 size)
{
    if(size == 1)
    {
        hdr[*hdr_idx] = (uint8) data;
        (*hdr_idx) += 1;
    }
    else if(size == 2)
    {
        hdr[*hdr_idx] = (uint8) (data & 0x00ff);
        (*hdr_idx) += 1;
        hdr[*hdr_idx] = (uint8) ((data & 0xff00) >> 8);
        (*hdr_idx) += 1;
    }
    else if(size == 4)
    {
        hdr[*hdr_idx] = (uint8) (data & 0x000000ff);
        (*hdr_idx) += 1;
        hdr[*hdr_idx] = (uint8) ((data & 0x0000ff00) >> 8);
        (*hdr_idx) += 1;
        hdr[*hdr_idx] = (uint8) ((data & 0x00ff0000) >> 16);
        (*hdr_idx) += 1;
        hdr[*hdr_idx] = (uint8) ((data & 0xff000000) >> 24);
        (*hdr_idx) += 1;
    }
    else
    {
        printf("Illegal size in write_hdr...consult your instructor\n");
    }
}

int readGSBMP(const char* filename, unsigned char inputImage[][SIZE],int &filefail)
{
    uint8 type[2];
    int headersize = 0;

    int i,j;

    //BITMAPFILEHEADER bfh;

    /* file pointer */
    FILE *file;

    /* read input bmp into the data matrix */
    if (!(file=fopen(filename,"rb")))
    {
        cout << "Cannot open file: " << filename <<endl;
        return(1);
    }

    fread(type, sizeof(unsigned char), 0x2, file);
    if(type[0] != 'B' and type[1] != 'M')
    {
        cout << "Not a BMP file" << endl;
        return(1);
    }
    fseek(file, 8, SEEK_CUR);
    fread(&headersize, sizeof(uint32), 1, file);
#ifdef BMP_BIG_ENDIAN
    headersize = BYTE_SWAP(headersize);
#endif
    //cout << "Header size is " << headersize << endl;

    fseek(file, headersize, SEEK_SET);
    fread(tempGSImage, sizeof(uint8), SIZE*SIZE, file);
    //  cout << (int)tempGSImage[0][0][0] << "," << (int)tempGSImage[0][0][1] << "," << (int)tempGSImage[0][0][2] << endl;
    fclose(file);

    for(i=0; i < SIZE; i++)
    {
        for(j=0; j < SIZE; j++)
        {
            inputImage[SIZE-1-i][j] = tempGSImage[i][j];
            //	inputImage[SIZE-1-i][SIZE-1-j] = tempGSImage[i][j];
            //cerr << tempGSImage[i][j][k] << ",";
        }
        //cerr << endl;
    }
    return 0;
}


int writeGSBMP(const char* filename, unsigned char outputImage[][SIZE])
{
    uint8 hdr[54];
    int hdr_idx = 0;

    int i,j;

    BITMAPFILEHEADER bfh;

    // file pointer
    FILE *file;

    bfh.bfType1 = 'B';
    bfh.bfType2 = 'M';
    // 0x10436 = 2^16 + 1024 for color def + 0x36 for header
    bfh.bfSize = 0x010436; //0x36;

    bfh.bfReserved1 = 0x0;
    bfh.bfReserved2 = 0x0;
    // 0x0436
    bfh.bfOffBits = 0x436; //0x36;

    bfh.biSize = 0x28;
    bfh.biWidth = SIZE;
    bfh.biHeight = SIZE;
    bfh.biPlanes = 1;
    bfh.biBitCount = 8;
    bfh.biCompression  = 0;
    bfh.biSizeImage = SIZE*SIZE;
    bfh.biXPelsPerMeter = 0; //2400;
    bfh.biYPelsPerMeter = 0; //2400;
    bfh.biClrUsed = SIZE; // 0;
    bfh.biClrImportant = SIZE; // 0;

    write_hdr(hdr, &hdr_idx, bfh.bfType1, sizeof(uint8));
    write_hdr(hdr, &hdr_idx, bfh.bfType2, sizeof(uint8));
    write_hdr(hdr, &hdr_idx, bfh.bfSize, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.bfReserved1, sizeof(uint16));
    write_hdr(hdr, &hdr_idx, bfh.bfReserved2, sizeof(uint16));
    write_hdr(hdr, &hdr_idx, bfh.bfOffBits, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biSize, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biWidth, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biHeight, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biPlanes, sizeof(uint16));
    write_hdr(hdr, &hdr_idx, bfh.biBitCount, sizeof(uint16));
    write_hdr(hdr, &hdr_idx, bfh.biCompression, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biSizeImage, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biXPelsPerMeter, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biYPelsPerMeter, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biClrUsed, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biClrImportant, sizeof(uint32));


    for(i=0; i < SIZE; i++)
    {
        for(j=0; j < SIZE; j++)
        {
            //tempGSImage[SIZE-1-i][SIZE-1-j] = outputImage[i][j];
            tempGSImage[SIZE-1-i][j] = outputImage[i][j];
        }
    }

    // write result bmp file
    if (!(file=fopen(filename,"wb")))
    {
        cout << "Cannot open file: " << filename << endl;
        return(1);
    }
    uint8 z = 0;
    fwrite(&hdr, sizeof(unsigned char), 0x36, file);
    for(i=0; i < SIZE; i++)
    {
        uint8 x = (uint8) i;
        //cout << (int)x << endl;
        fwrite(&x, sizeof(uint8), 0x01, file);
        fwrite(&x, sizeof(uint8), 0x01, file);
        fwrite(&x, sizeof(uint8), 0x01, file);
        fwrite(&z, sizeof(uint8), 0x01, file);

    }
    fwrite(tempGSImage, sizeof(unsigned char), SIZE*SIZE, file);
    fclose(file);

    return 0;
}

int shows = 0;
void show()
{
    system("eog --single-window /tmp/bmplib.bmp &");

    // wait longer on the first show, OS takes time to start eog
    //if (shows == 0) sleep(1);

    // generally, wait 0.2 seconds = 200000 milliseconds
    usleep(200000);
    shows++;
}

void showGSBMP(unsigned char inputImage[][SIZE])
{
    writeGSBMP("/tmp/bmplib.bmp", inputImage);
    show();
}
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
using namespace std;
unsigned char image[SIZE][SIZE];
unsigned char image100[SIZE][SIZE];
bool flag=true;

void loadImage ();
void saveImage ();
void BlackAndWhite();
void InvertImage();
void RotateImage();
void DarkenLighten();
void EnlargeImage();
void shrink();
void FlipHorizontally();
void FlipVertically();
void SketchPhoto();
void ShuffleImage();
void loadImage100();
void MergeImage();
void MirrorImageLeftH();
void MirrorImageLowH();
void MirrorImageRightH();
void MirrorImageUpH();

int main()
{
    char choice,choice2;
    int counter=0;
    cout<<"Ahlan ya user ya habibi."<<endl;
    while(true)
    {
        cout<<endl<<"---------------------------------------------------------------"<<endl;
        if(counter==0)
        {
            loadImage();
            if(filefail==1)
            {
                filefail=0;
                continue;
            }
            cout<<endl<<"---------------------------------------------------------------"<<endl;
        }
        cout<<"Please select a filter to apply or 0 to exit."<<endl;
        cout<<"1-Black & White Filter.\n2-Invert Filter.\n3-Merge Filter."<<endl;
        cout<<"4-Flip Image.\n5-Rotate Image.\n6-Darken and Lighten Image."<<endl;
        cout<<"7-Detect Image Edges.\n8-Enlarge Image.\n9-Shrink Image.\na-Mirror 1/2 Image.\nb-Shuffle Image.\n";
        cout<<"s-Save the image to a file.\n0-Exit."<<endl;
        cout<<">>";
        cin>>choice;
        while(choice != '1'&&choice!='2'&&choice!='3'&choice!='4'&&choice!='5'&&choice!='6'&&choice!='7'&&choice!='8'&&choice!='9'&&choice!='s'&&choice!='0'&&choice!='a'&&choice!='b')
        {
            cout<<"Please enter a valid choice:"<<endl<<">>";
            cin>>choice;
        }
        if(choice=='0')
        {
            cout<<"Thank you for using this program.";
            break;
        }
        else if(choice=='s')
        {
            saveImage();
        }
        else if(choice=='1')
        {
            BlackAndWhite();
        }
        else if(choice=='2')
        {
            InvertImage();
        }
        else if(choice=='3')
        {
            MergeImage();
        }
        else if(choice=='4')
        {
            string choice6;
            cout<<"Flip (h)orizontally or (v)ertically ?\n>>";
            cin>>choice6;
            while(choice6!="h"&&choice6!="v")
            {
                cout<<"Please choose h to flip horizontally or v to flip vertically:\n>>";
                cin>>choice6;
            }
            if(choice6=="h")
            {
                FlipHorizontally();
            }
            else if(choice6=="v")
            {
                FlipVertically();
            }
        }
        else if(choice=='5')
        {
            RotateImage();
        }
        else if(choice=='6')
        {
            DarkenLighten();

        }
        else if(choice=='7')
        {
            SketchPhoto();
        }
        else if(choice=='8')
        {
            EnlargeImage();
        }
        else if(choice=='9')
        {
            shrink();
        }
        else if(choice=='a'){
            string choose4="";
            cout<<"Mirror (l)eft, (r)ight, (u)pper, (d)own side?\n>>";
            cin>>choose4;
            while(choose4!="l"&&choose4!="r"&&choose4!="u"&&choose4!="d"){
                cout<<"Please choose l, r, u, or d to mirror left, right,upper, or down.\n>>";
                cin>>choose4;
            }
            if(choose4=="l"){
                MirrorImageLeftH();
            }
            else if(choose4=="r"){
                MirrorImageRightH();
            }
            else if(choose4=="u"){
                MirrorImageUpH();
            }
            else if(choose4=="d"){
                MirrorImageLowH();
            }
        }
        else if(choice=='b')
        {
            ShuffleImage();
        }
        cout<<"Do you want to continue with this photo?\n1-Yes\n2-No\n>>";
        cin>>choice2;
        while(choice2!='1'&&choice2!='2')
        {
            cout<<"Please choose 1 or 2 :\n>>";
            cin>>choice2;
        }
        if(choice2=='1')
        {
            counter++;
        }
        else
        {
            counter=0;
            cout<<"Do you want to save your work?\n1-Save.\n2-Don't save.\n>>";
            char choose2,choose3;
            cin>>choose2;
            while(choose2!='1'&&choose2!='2')
            {
                cout<<"Please choose 1 or 2:\n>>";
                cin>>choose2;
            }
            if(choose2=='1')
            {
                saveImage();
            }
            cout<<"Do you want to exit this program?\n1-Yes.\n2-No\n>>";
            cin>>choose3;
            while(choose3!='1'&&choose3!='2')
            {
                cout<<"Please choose 1 or 2:\n>>";
                cin>>choose3;
            }
            if(choose3=='1')
            {
                cout<<"Thank you for using this program."<<endl;
                break;
            }
        }

    }
    return 0;
}
void loadImage100()
{
    char imageFileName[100];
    // Get gray scale image file name
    cout << "Enter the source image file name: ";
    cin >> imageFileName;
    // Add to it .bmp extension and load image
    strcat (imageFileName, ".bmp");
    readGSBMP(imageFileName, image100,filefail);
}
void loadImage ()
{
    char imageFileName[100];
    // Get gray scale image file name
    cout << "Please enter file name of the image to process:"<<endl<<">>";
    cin >> imageFileName;
    // Add to it .bmp extension and load image
    strcat (imageFileName, ".bmp");
    readGSBMP(imageFileName, image,filefail);
}
void saveImage ()
{
    char imageFileName[100];
    // Get gray scale image target file name
    cout << "Enter the target image file name: ";
    cin >> imageFileName;
    // Add to it .bmp extension and load image
    strcat (imageFileName, ".bmp");
    writeGSBMP(imageFileName, image);
}
void BlackAndWhite()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j< SIZE; j++)
        {
            if(image[i][j]>127)
            {
                image[i][j]=255;
            }
            else
            {
                image[i][j]=0;
            }
        }
    }
}
void InvertImage()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j< SIZE; j++)
        {
            image[i][j]=255-image[i][j];
        }
    }
}
void DarkenLighten()
{
    unsigned char newimage[SIZE][SIZE];
    char x;
    cout<<"Do you want to (d)arken or (l)ighten?\n>>"<<endl;
    cin>>x;
    while(x!='d'&&x!='l')
    {
        cout<<"Please choose d to darken or l to lighten:\n>>";
        cin>>x;
    }
    if(x=='d')
    {
        for(int i=0; i<SIZE; i++)
        {
            for(int j=0; j<SIZE; j++)
            {
                newimage[i][j]=(image[i][j]*0.5);
            }
        }
    }
    else
    {
        for(int i=0; i<SIZE; i++)
        {
            for(int j=0; j<SIZE; j++)
            {
                ///darkest color
                if((image[i][j]>=0)&&(image[i][j]<=64))
                {
                    newimage[i][j]=(image[i][j])+96;
                }
                /// tends to grey
                else if((image[i][j]>64)&&(image[i][j]<127))
                {
                    newimage[i][j]=(image[i][j])+64;
                }
                ///lighter than grey
                else if((image[i][j]>=127)&&(image[i][j]<191))
                {
                    newimage[i][j]=(image[i][j])+32;
                }
                ///tends to white
                else
                {
                    newimage[i][j]=(image[i][j]);
                }
            }
        }
    }
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            image[i][j]=newimage[i][j];
        }
    }


}
void RotateImage()
{
    unsigned char image2[SIZE][SIZE];
    int Rotate=1;
    int n=0;
    int s=SIZE-1;
    string choice4="";
    cout<<"Rotate the image by:\n1-90.\n2-180.\n3-270.\n>>";
    cin>>choice4;
    while(choice4!="1"&&choice4!="90"&&choice4!="2"&&choice4!="180"&&choice4!="3"&&choice4!="270")
    {
        cout<<"Please choose 1, 2, or 3:\n>>";
        cin>>choice4;
    }
    if(choice4=="1"||choice4=="90")
    {
        n=1;
    }
    else if(choice4=="2"||choice4=="180")
    {
        n=2;
    }
    else if(choice4=="3"||choice4=="270")
    {
        n=3;
    }
    for(int r=1; r<=Rotate*n; r++)
    {
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j< SIZE; j++)
            {
                image2[j][s]=image[i][j];
            }
            s--;
        }
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j< SIZE; j++)
            {
                image[i][j]=image2[i][j];
            }
        }
        s=SIZE-1;
    }
    n=0;
}
void EnlargeImage()
{
    unsigned char image3[SIZE][SIZE];
    string choice5="";
    cout<<"Which quarter to enlarge 1, 2, 3, or 4?"<<endl<<">>";
    cin>>choice5;
    while(choice5!="1"&&choice5!="2"&&choice5!="3"&&choice5!="4")
    {
        cout<<"Please choose 1, 2, 3, or 4:\n>>";
        cin>>choice5;
    }
    if(choice5=="1")
    {
        for (int i = 0,m=0; i < SIZE/2,m<SIZE; i++,m+=2)
        {
            for (int j = 0,n=0; j< SIZE/2,n<SIZE; j++,n+=2)
            {
                image3[m][n]=image[i][j];
                image3[m+1][n]=image[i][j];
                image3[m][n+1]=image[i][j];
                image3[m+1][n+1]=image[i][j];
            }
        }
    }
    else if(choice5=="2")
    {
        for (int i = 0,m=0; i < SIZE/2,m<SIZE; i++,m+=2)
        {
            for (int j = SIZE/2,n=0; j< SIZE,n<SIZE; j++,n+=2)
            {
                image3[m][n]=image[i][j];
                image3[m+1][n]=image[i][j];
                image3[m][n+1]=image[i][j];
                image3[m+1][n+1]=image[i][j];
            }
        }
    }
    else if(choice5=="3")
    {
        for (int i = SIZE/2,m=0; i < SIZE/2,m<SIZE; i++,m+=2)
        {
            for (int j = 0,n=0; j< SIZE/2,n<SIZE; j++,n+=2)
            {
                image3[m][n]=image[i][j];
                image3[m+1][n]=image[i][j];
                image3[m][n+1]=image[i][j];
                image3[m+1][n+1]=image[i][j];
            }
        }
    }
    else if(choice5=="4")
    {
        for (int i = SIZE/2,m=0; i < SIZE,m<SIZE; i++,m+=2)
        {
            for (int j = SIZE/2,n=0; j< SIZE,n<SIZE; j++,n+=2)
            {
                image3[m][n]=image[i][j];
                image3[m+1][n]=image[i][j];
                image3[m][n+1]=image[i][j];
                image3[m+1][n+1]=image[i][j];
            }
        }
    }
    for(int k=0; k<SIZE; k++)
    {
        for(int l=0; l<SIZE; l++)
        {
            image[k][l]=image3[k][l];
        }
    }
}
void shrink() {
    unsigned char newimage[SIZE][SIZE];
    int n,x=0,c=0;
    string num;
    cout<<"Shrink to:\n1) 1/2.\n2) 1/3.\n3) 1/4.\n>>"<<endl;
    cin>>num;
    while(num!="1"&&num!="2"&&num!="3"){
        cout<<"Please choose 1, 2, or 3:\n>>";
        cin>>num;
    }
    if(num=="1"){
for (int i=0,k=0; i < SIZE/2 ; i++, k+=2) {
         for (int j=0,l=0 ; j<SIZE/2  ;  j++,l+=2) {
               newimage[i][j]=image[k][l];
}
}
for(int i=0;i<SIZE;i++){
    for(int j=0;j<SIZE;j++){
        if(i<SIZE/2&&j<SIZE/2){
            image[i][j]=newimage[i][j];
        }
        else{
            image[i][j]=255;
        }
    }
}
    }
 else if(num=="2"){
    for (int i=0,k=0; i < SIZE/3 ; i++, k+=3) {
         for (int j=0,l=0 ; j<SIZE/3  ;  j++,l+=3) {
               newimage[i][j]=image[k][l];
}
}
for(int i=0;i<SIZE;i++){
    for(int j=0;j<SIZE;j++){
        if(i<SIZE/3&&j<SIZE/3){
            image[i][j]=newimage[i][j];
        }
        else{
            image[i][j]=255;
        }
    }
}
    }
  else if(num=="3"){
     for (int i=0,k=0; i < SIZE/4 ; i++, k+=4) {
         for (int j=0,l=0 ; j<SIZE/4 ;  j++,l+=4) {
               newimage[i][j]=image[k][l];
}
}
for(int i=0;i<SIZE;i++){
    for(int j=0;j<SIZE;j++){
        if(i<SIZE/4&&j<SIZE/4){
            image[i][j]=newimage[i][j];
        }
        else{
            image[i][j]=255;
        }
    }
}
}
}
void SketchPhoto()
{
    BlackAndWhite();
    for(int i=0; i<SIZE; i++)
    {
        for(int j=0; j<SIZE; j++)
        {
            if(image[i][j]-image[i][j+1]!=0||image[i][j]-image[i+1][j]!=0)
            {
                image[i][j]=0;
            }
            else
            {
                image[i][j]=255;
            }
        }
    }
}
void FlipHorizontally()
{
    unsigned char TempImage[SIZE][SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j< SIZE/2; j++)
        {
            TempImage[i][j]=image[i][j];
            image[i][j]=image[i][SIZE-j-1];
            image[i][SIZE-j-1]=TempImage[i][j];
        }
    }
}
void FlipVertically()

{
    unsigned char TempImage[SIZE][SIZE];
    char temp;
    int s=SIZE-1;
    for (int i = 0; i < SIZE/2; i++)
    {
        for (int j = 0; j< SIZE; j++)
        {
            TempImage[i][j]=image[i][j];
            image[i][j]=image[SIZE-i-1][j];
            image[SIZE-i-1][j]=TempImage[i][j];
        }
    }
}
void ShuffleImage()
{
    unsigned char image2[SIZE][SIZE];
    for(int i=0; i<SIZE; i++)
    {
        for(int j=0; j<SIZE; j++)
        {
            image2[i][j]=image[i][j];
        }
    }
    char choose;
    int k=1,o,p;
    cout<<"New order of quarters?\n>>";
    while(true)
    {
        int m,n;
        if(k==5)
        {
            break;
        }
        if(k==1)
        {
            o=0,p=0;
        }
        else if(k==2)
        {
            o=0;
            p=SIZE/2;
        }
        else if(k==3)
        {
            o=SIZE/2;
            p=0;
        }
        else if(k==4)
        {
            o=SIZE/2;
            p=SIZE/2;
        }
        k++;
        cin>>choose;
        cin.ignore();
        if(choose=='1')
        {
            for (int i = 0,m=o; i < SIZE/2,m<SIZE; i++,m++)
            {
                for (int j = 0,n=p; j< SIZE/2,n<SIZE; j++,n++)
                {
                    image2[m][n]=image[i][j];
                }
            }
        }
        else if(choose=='2')
        {
            for (int i = 0,m=o; i < SIZE/2,m<SIZE; i++,m++)
            {
                for (int j = SIZE/2,n=p; j< SIZE,n<SIZE; j++,n++)
                {
                    image2[m][n]=image[i][j];
                }
            }
        }
        else if(choose=='3')
        {
            for (int i = SIZE/2,m=o; i < SIZE,m<SIZE; i++,m++)
            {
                for (int j = 0,n=p; j< SIZE/2,n<SIZE; j++,n++)
                {
                    image2[m][n]=image[i][j];
                }
            }
        }
        else if(choose=='4')
        {
            for (int i = SIZE/2,m=o; i < SIZE,m<SIZE; i++,m++)
            {
                for (int j = SIZE/2,n=p; j< SIZE,n<SIZE; j++,n++)
                {
                    image2[m][n]=image[i][j];
                }
            }
        }
    }
    for(int i=0; i<SIZE; i++)
    {
        for(int j=0; j<SIZE; j++)
        {
            image[i][j]=image2[i][j];
        }
    }
}
void MergeImage()
{
    unsigned char newimage[SIZE][SIZE];
    loadImage100();
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            newimage[i][j]=(image[i][j]+image100[i][j])/2;
        }
    }
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            image[i][j]=newimage[i][j];
        }
    }
}
void MirrorImageRightH()
{
    unsigned char TempImage[SIZE][SIZE];

    for (int i = 0; i < (SIZE); i++)
    {
        for (int j = 0; j< (SIZE/2); j++)
        {
            image[i][j] = image[i][SIZE-1-j];

        }
    }



}
void MirrorImageLeftH()
{
    unsigned char TempImage[SIZE][SIZE];

    for (int i = 0; i < (SIZE); i++)
    {
        for (int j = 0; j< (SIZE/2); j++)
        {
            image[i][SIZE-1-j] = image[i][j];

        }
    }



}
void MirrorImageUpH()
{
    unsigned char TempImage[SIZE][SIZE];

    for (int i = 0; i < (SIZE/2); i++)
    {
        for (int j = 0; j< (SIZE); j++)
        {
            image[SIZE-1-i][j] = image[i][j];

        }
    }



}
void MirrorImageLowH()
{
    unsigned char TempImage[SIZE][SIZE];

    for (int i = 0; i < (SIZE/2); i++)
    {
        for (int j = 0; j< (SIZE); j++)
        {
            image[i][j] = image[SIZE-1-i][j];

        }
    }



}

