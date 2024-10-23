A = 0x51
B = 0x41
C = 0x21
D = 0x01
for _ in range(6):
     print("A =",f"{hex(A)}",end=" ")
     print("B =",f"{hex(B)}",end=" ")
     print("C =",f"{hex(C)}",end=" ")
     print("D =",f"{hex(D)}",end=" ")
     F0 = ~((A & ~B) | (~B & D))
     print("F0 =",f"{hex(0xFF+F0+1)}",end=" ")
     F1 = (A | ~C) & (B | ~D)
     print("F1 =",f"{hex(0xFF+F1+1)}")
     A += 0x01
     B += 0x02
     C += 0x03
     D += 0x04