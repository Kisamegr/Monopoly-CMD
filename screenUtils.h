
typedef enum {black,
              blue,
              green,
              cyan,
              red,
              magenta,
              yellow,
              white,
              gray,
              lightblue,
              lightgreen,
              lightcyan,
              lightred,
              lightmagenta,
              lightyellow,
              lightwhite} color;
/*To color xrisimopoieitai oste stin setColor na dilonetai apeytheias to onoma tou epithimitou xromatos*/


void gotoxy(int x, int y);
/*
I sinartisi metakinei ton kersora sti thesi (x,y) tis othonis. To x einai i stili kai sinithos pairnei 
times apo 0 mexri 79 kai to y einai i grammi pou sinithos kimainetai apo 0 mexri 24
P.x.  i  gotoxy(40,12); tha metakinisei ton kersora peripou sto kentro tis othonis.
*/


void clrscr(void);
/*
I sinartisi katharizei tin othoni kai metaferei ton kersora stin arxi (0,0)
*/


void setColor(color text, color back);
/*
I sinartisi kathorizei to xroma tis grammatoseiras kai to background gia tin epomeni sinartisi emfanisis stin othoni (p.x. printf())
To proto orisma einai to font color kai to deytero to background color. Ta 2 orismata pairnoun arithmitikes times apo 0 mexri 15 alla
boroun na xrisimopoiithoun ta onomata ton xromaton poy yparxoun sti dilosi tis enum color pio pano.

P.x.

setColor (green,white);  // i allios setColor(2,7);
printf("Hello");

tha typosei ti lexi Hello me prasinous xaraktires se aspro fonto.

*/
