import math

class PixelMapper:

    def __init__(self):
        pass

    def map(self,index):
        #print index

        section = 0
        clusterindex = 0
        pixindex = 0

        row = int(math.floor(index/32.0))
        coll = index%32;

        if row > 31:
            section = 1

        if coll < 16:
            clusterindex = int(math.floor(row%32/4.0) * 16 + coll%16)
        else:
            clusterindex = int(math.floor(row%32/4.0) * 16 + coll%16 + 128)

        pixindex = row%4

        return (section,clusterindex,pixindex) #section, index, 4 pix cluster index

    def generate(self):
        f = open("pixelmap.h","w")
        f.write("char pixellookup[2048][3] = {\n")
        for i in range(2048):
            a,b,c = self.map(i)
            f.write("{"+str(a)+","+str(b)+","+str(c)+"},\n")
        f.write("};")
        f.close()



if __name__ == "__main__":
    m = PixelMapper()
    m.generate()