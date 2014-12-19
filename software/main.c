#include "ledmirror.h"
#include "utils.h"

void test_ImageDraw(int framecounter){

    int frc = framecounter%(1900/32);
	
    int offset = frc * 32;
    int xoffset = frc%32-15; 

    int image[20] = { 15 + offset + xoffset, 
                      16 + offset + xoffset, 
                      47 + offset + xoffset, 
                      48 + offset + xoffset, 
                      77 + offset + xoffset,
                      78 + offset + xoffset,
                      79 + offset + xoffset,
                      80 + offset + xoffset,
                      81 + offset + xoffset,
                      82 + offset + xoffset, 
                      109 + offset + xoffset,
                      110 + offset + xoffset,
                      111 + offset + xoffset,
                      112 + offset + xoffset,
                      113 + offset + xoffset,
                      114 + offset + xoffset, 
                      143 + offset + xoffset,
                      144 + offset + xoffset,
                      175 + offset + xoffset,
                      176 + offset + xoffset };

    displayImage(image,NELEMS(image));

}

int main(int argc, const char **argv)
{
    int exitcode = ledmirror_run();
    return exitcode;
}

