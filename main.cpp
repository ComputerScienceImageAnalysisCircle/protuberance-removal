/**
 *
 * This software is intented to remove proturbance
 * in bronchial walls on 2D CT images. 
 *
 * Author:	Patryk Zabkiewicz
 * Date:	24.05.2011
 * Email:	pzabkiewicz@gmail.com
 *
 * License:	GPL 3.0
 *
 * This software is distributed as is. NO WARRANTY provided.
 *
 * Dependencies:
 *  - OpenCV
 *
*/


#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

using namespace std;

#define PI 3.14159265
#define ILO 360
#define PROCENT 45

struct Sciana {
    int grubosc;
    int kat;
    int promien;
    Sciana *next;
};

void AddToList(Sciana*& head, int grubosc, int kat, int promien);


int main(int argc, char** argv) {

    if(argc < 3)  {  cout << endl << "Parametry: nazwa-pliku srodek-x srodek-y [nazwa-pliku-wynikowego]" << endl << endl; return 1;}

    char* filename = argv[1];
    char* sr_x = argv[2];
    char* sr_y = argv[3];

    char z = (-1);
    int r = 0, kat = 0, zmiana = 0;
    Sciana* scianka = NULL;


    IplImage* img0 = 0, *img1 = 0;


    //wczytanie obrazu
    img0 = cvLoadImage(filename, 0);



//    //segmentacja otworu
//    for(int k=0; k<img0->width; k++) {
//        for(int j=0; j<img0->height; j++) {
//            uchar piksel = CV_IMAGE_ELEM(img0, uchar, cursor.y, cursor.x);

//            if (z < 0) {
//                if (piksel > 240) {
//                    zmiana++;
//                    z = z * (-1);
//                }

//            } else {
//                if (piksel <= 240) {
//                    zmiana++;
//                    z = z * (-1);
//                }
//            }
//        }
//    }


    //szukanie srodka otworu


    //skanowanie radarem i mierzenie sciany
    CvPoint srodek = cvPoint(atoi(sr_x), atoi(sr_y));
    CvPoint cursor = cvPoint(atoi(sr_x), atoi(sr_y));

    int grubosc = 0;

    for (kat = 0; kat < ILO; kat++) {

        while (zmiana < 2) {
            //int* piksel = ((int*) (img0->imageData + i* img0->widthStep))[j*img0->nChannels+0];
            //wykorzystuje makro do odczytania wartosci piksela

            uchar piksel = CV_IMAGE_ELEM(img0, uchar, cursor.y, cursor.x);

            if (z < 0) {
                if (piksel > 240) {
                    zmiana++;
                    z = z * (-1);
                }

            } else {
                grubosc++;

                if (piksel <= 240) {
                    zmiana++;
                    z = z * (-1);
                }
            }

            //nowe wspolzedne kursora
            r++;
            cursor.x = (int) (srodek.x + r * cos(2 * PI * kat / ILO));
            cursor.y = (int) (srodek.y + r * sin(2 * PI * kat / ILO));

            if(cursor.x <= 0 || cursor.y <= 0 || cursor.x >= img0->width || cursor.y >= img0->height) {
                zmiana = 2;
                z = -1;
            }
        }

        cursor.x = srodek.x;
        cursor.y = srodek.y;

        AddToList(scianka,grubosc,kat,r);

        grubosc=0;
        zmiana = 0;
        r=0;
    }




    //sprawdzanie gdzie sciana jest grubsza w jakims miejscu

    float srednia = (float)scianka->grubosc;
    float suma = (float)scianka->grubosc;
    float roznica = 0.0;
    Sciana* zgrubienie = NULL;
    int ile=2,i=0;


    while(scianka != NULL) {

        roznica = srednia - scianka->grubosc;

        if( abs(roznica) < ((srednia*PROCENT)/100) ) {
            suma += scianka->grubosc;
            srednia = suma / (float)ile;
            ile++;
        } else {
            AddToList(zgrubienie,scianka->grubosc,scianka->kat,scianka->promien);
        }

        i++;
        scianka = scianka->next;
    }

    cout << "Srednia: " << srednia << endl;



    //wyczyszczenie zgrubienia
    img1 = cvCloneImage(img0);
    Sciana *tmp = zgrubienie; //zeby nie zgubic poczatku

        while(zgrubienie != NULL) {

            r = zgrubienie->promien - zgrubienie->grubosc -1;

            while(r < zgrubienie->promien) {
                cursor.x = (int) (srodek.x + r * cos(2 * PI * zgrubienie->kat / ILO));
                cursor.y = (int) (srodek.y + r * sin(2 * PI * zgrubienie->kat / ILO));

                uchar* dst = &CV_IMAGE_ELEM( img1, uchar, cursor.y, cursor.x );
                *dst = 0;

               if(cursor.x >= 1 || cursor.y >= 1 || cursor.x <= img0->width-1 || cursor.y <= img0->height-1) {
                //gumka musi byc wieksza niz jeden piksel
                dst = &CV_IMAGE_ELEM( img1, uchar, cursor.y, cursor.x+1 );
                *dst = 0;
                dst = &CV_IMAGE_ELEM( img1, uchar, cursor.y, cursor.x-1 );
                *dst = 0;
                dst = &CV_IMAGE_ELEM( img1, uchar, cursor.y+1, cursor.x );
                *dst = 0;
                dst = &CV_IMAGE_ELEM( img1, uchar, cursor.y-1, cursor.x );
                *dst = 0;
                }

                r++;
            }

        zgrubienie = zgrubienie->next;
        }




    //namalowanie nowej sciany
        zgrubienie = tmp;

        while(zgrubienie != NULL) {

            r = zgrubienie->promien - zgrubienie->grubosc;
            int cel = r+(int)srednia;

            while(r < cel) {
                cursor.x = (int) (srodek.x + r * cos(2 * PI * zgrubienie->kat / ILO));
                cursor.y = (int) (srodek.y + r * sin(2 * PI * zgrubienie->kat / ILO));

                uchar* dst = &CV_IMAGE_ELEM( img1, uchar, cursor.y, cursor.x );
                *dst = 253;

                if(cursor.x >= 1 || cursor.y >= 1 || cursor.x <= img0->width-1 || cursor.y <= img0->height-1) {
                //pedzel musi byc wiekszy niz jeden piksel
                dst = &CV_IMAGE_ELEM( img1, uchar, cursor.y, cursor.x+1 );
                *dst = 253;
                dst = &CV_IMAGE_ELEM( img1, uchar, cursor.y, cursor.x-1 );
                *dst = 253;
                dst = &CV_IMAGE_ELEM( img1, uchar, cursor.y+1, cursor.x );
                *dst = 253;
                dst = &CV_IMAGE_ELEM( img1, uchar, cursor.y-1, cursor.x );
                *dst = 253;
                }

                r++;
            }

        zgrubienie = zgrubienie->next;
        }

    //zapis obrazka lub wyswietlenie
        if(strlen(argv[4])>0)
    cvSaveImage(argv[4], img1);
    else
    {
        cvNamedWindow( "oryginal", 1 );
        cvNamedWindow( "wycienta krawedz", 1 );
        cvShowImage( "oryginal", img0 );
        cvShowImage( "wycienta krawedz", img1 );
    }

    return 0;
}

void AddToList(Sciana*& head, int grubosc, int kat, int promien)
{
    Sciana* TMP = new Sciana;
    TMP->grubosc = grubosc;
    TMP->kat = kat;
    TMP->promien = promien;
    TMP->next = head;
    head = TMP;
}
