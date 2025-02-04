def GetBit(num, bit):
    return (num >> bit) & 1
    
def ExtendTo(num, bit):
    return num << bit 
    
def Sign(num):
    return GetBit(num, 7)
    
def Zero(num):
    return num == 0

def Parity(num):
   high = 1
   for b in range(8):
       high += GetBit(num, b)
   return high % 2
   
for i in range(16):
    for j in range(16):
        num = i * 16 + j
        packedFlags = 0
        
        packedFlags |= ExtendTo(Sign(num), 7)
        packedFlags |= ExtendTo(Zero(num), 6)
        packedFlags |= ExtendTo(Parity(num), 2)
        
        print(packedFlags, end=", ")
    print()

68, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4, 
0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0, 
0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0, 
4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4, 
0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0, 
4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4, 
4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4, 
0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0, 
128, 132, 132, 128, 132, 128, 128, 132, 132, 128, 128, 132, 128, 132, 132, 128, 
132, 128, 128, 132, 128, 132, 132, 128, 128, 132, 132, 128, 132, 128, 128, 132, 
132, 128, 128, 132, 128, 132, 132, 128, 128, 132, 132, 128, 132, 128, 128, 132, 
128, 132, 132, 128, 132, 128, 128, 132, 132, 128, 128, 132, 128, 132, 132, 128, 
132, 128, 128, 132, 128, 132, 132, 128, 128, 132, 132, 128, 132, 128, 128, 132, 
128, 132, 132, 128, 132, 128, 128, 132, 132, 128, 128, 132, 128, 132, 132, 128, 
128, 132, 132, 128, 132, 128, 128, 132, 132, 128, 128, 132, 128, 132, 132, 128, 
132, 128, 128, 132, 128, 132, 132, 128, 128, 132, 132, 128, 132, 128, 128, 132, 