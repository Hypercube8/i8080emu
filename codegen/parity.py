def parity(num):
   high = 1
   for b in range(8):
       high += num >> b & 1
   return high % 2
   
for i in range(16):
    for j in range(16):
        num = i * 16 + j
        print(f"{parity(num)}", end=", ")
    print()
