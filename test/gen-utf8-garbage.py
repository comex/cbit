import random, sys
u = ''
for i in range(1000000):
    length = random.randint(1, 4)
    lo, hi = [(0, 0x7f), (0x80, 0x7ff), (0x800, 0xffff), (0x10000, 0x10ffff)][length-1]
    while True:
        num = random.randint(lo, hi)
        if 0xd800 <= num <= 0xdfff: continue
        break
    sys.stdout.write(chr(num))
    
