#include <stdio.h>
#include <stdlib.h>
#include "ledmirror.h"
#include <bcm2835.h>
#include <unistd.h>

void testLeds(){
   //test pixel loop
   int i;
   int j = 0;
   int realimagewidth = 32;
   int rowcount = 0;
   for(i=0; i < 2*(imagewidth*imageheight/4); i++){
     int p4 = 0;
     int p3 = 90;
     int p2 = 120;
     int p1 = 240;
     int packed_4_pix = pack(p1,p2,p3,p4);

     char output_buffer2[3];
     output_buffer2[0] = 0x00;
     output_buffer2[1] = i;
     output_buffer2[2] = packed_4_pix;
     bcm2835_spi_transfern(&output_buffer2[0], 3);

     if (i%imagewidth == (imagewidth-1)){
        j = j + realimagewidth*4;
        rowcount ++;
        usleep(200000);
     }
     usleep(500000);
   }

   for(i=0; i < 2*(imagewidth*imageheight/4); i++){
     int p4 = 0;
     int p3 = 90;
     int p2 = 120;
     int p1 = 240;
     int packed_4_pix = pack(p1,p2,p3,p4);

     char output_buffer2[3];
     output_buffer2[0] = 0x01;
     output_buffer2[1] = i;
     output_buffer2[2] = packed_4_pix;
     bcm2835_spi_transfern(&output_buffer2[0], 3);

     if (i%imagewidth == (imagewidth-1)){
        j = j + realimagewidth*4;
        rowcount ++;
        usleep(200000);
     }
     usleep(500000);
   }

   
}


void printBin(int n){
   printf("num %i :",n);
   while (n) {
       if (n & 1)
           printf("1");
       else
           printf("0");

       n >>= 1;
   }
   printf("\n");
}

void clear(){
   //test pixel loop
   int i;
   int j = 0;
   int realimagewidth = 32;
   int rowcount = 0;
   for(i=0; i < 2*(imagewidth*imageheight/4); i++){
     int p4 = 0;
     int p3 = 0;
     int p2 = 0;
     int p1 = 0;
     int packed_4_pix = pack(p1,p2,p3,p4);

     char output_buffer2[3];
     output_buffer2[0] = 0x00;
     output_buffer2[1] = i;
     output_buffer2[2] = packed_4_pix;
     bcm2835_spi_transfern(&output_buffer2[0], 3);

     if (i%imagewidth == (imagewidth-1)){
        j = j + realimagewidth*4;
        rowcount ++;
     }
   }

   for(i=0; i < 2*(imagewidth*imageheight/4); i++){
     int p4 = 0;
     int p3 = 0;
     int p2 = 0;
     int p1 = 0;
     int packed_4_pix = pack(p1,p2,p3,p4);

     char output_buffer2[3];
     output_buffer2[0] = 0x01;
     output_buffer2[1] = i;
     output_buffer2[2] = packed_4_pix;
     bcm2835_spi_transfern(&output_buffer2[0], 3);

     if (i%imagewidth == (imagewidth-1)){
        j = j + realimagewidth*4;
        rowcount ++;
     }
   }

   printf("total rows %u\n",rowcount);
}
