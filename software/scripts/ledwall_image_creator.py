from PIL import Image, ImageOps
import math
import os, os.path

class LedwallImageCreator:
    def __init__(self,picdir):
        self.picdir = picdir

    def compute(self, pixel):
        r,g,b,a = pixel
        if a == 0:
            return 0
        #val = round(math.sqrt(math.pow(r,2) + math.pow(g,2) + math.pow(b,2))   )
        val =  0.299*r + 0.587*g + 0.114*b
        if val < 1.0:
            val = 1
        return val

    def processImages(self):
        for root, dirs, files in os.walk(self.picdir):
            for i in dirs:
                pass
                #retDirs.append(os.path.join(root,i))
            self.outputText = '#ifndef ANIMATION_H\n#define ANIMATION_H\n'    
            myFiles =[]
            for imFile in files:
                if '.png' in imFile:
                    self.processImage(imFile) 
        self.outputText +='#endif'
        fp = open('animation.h','wb')
        fp.write(self.outputText)
        fp.close()

    def processImage(self,fileName):
        imPath = os.path.join(self.picdir,fileName)
        print ('imPath', imPath)
        im = Image.open(imPath)
        w,h = im.size
        im = ImageOps.mirror(im)
        pix = im.load()
        output = []
        for i in range(64*32):
            output.append(0)
        if w != 32:
            print('invalid image width, should be 32 px')
        if h != 64:
            print('invalid image height, should be 64 px')
        for y in range(64):
            for x in range(32):
                pixelVal = pix[x,y]
                val = int(round(self.compute(pixelVal)))
                output[y*32+x] = val

        frameName = fileName.split('.')[0]
        self.outputText += 'static unsigned int '+frameName+'[2048] =  {'
        for val in output:
            self.outputText += str(val)+', '

        self.outputText = self.outputText[:-2]+'};\n'
            
   
if __name__ == '__main__':
    lwImCr = LedwallImageCreator('images')
    lwImCr.processImages()

