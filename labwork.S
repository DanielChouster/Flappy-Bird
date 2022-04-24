   # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #
.global delay
.global time2string

delay:
move $t1, $a0 # load ms in $t1
li $t3, 1 # just a constant 1

while_loop:
li $t0, 0 # int i = 0;
ble $t1, $zero, end # if (ms <= 0) -> end of loop
sub $t1, $t1, 1 # ms = ms - 1;
for_loop:
slti $t2, $t0, 3800 # check if i < constant
bne $t2, $t3, while_loop # end the for loop and go back to the while loop if i == constant
addi $t0, $t0, 1 # i = i + 1;
j for_loop 
end: jr $ra
nop
	
 
time2string: 
PUSH $ra
#start
move $t6, $a0              #save a0 in t6   adress
move $a0, $a1              #save a1 in a0   time
addi $t6, $t6, 0x5         #set first adress to end
andi $t5,$t5,0x00          # Clear all bits
sb  $t5,($t6)              #copy last char to mem output
li $t4,4                   #set  cycle

cycle_start:
sub $t6, $t6, 0x1         #dec mem 
jal hexasc
sb $v0,($t6)               #copy first char to mem output
srl $a0,$a0,4
sub $t4, $t4, 0x1         #dec cycle
bne $t4, 2, cycle_1        #continue while
#copy ":"
sub $t6, $t6, 0x1         #dec mem 
li $t5,0x003A      # Copy ":"
sb  $t5,($t6)            #copy last char to mem output

cycle_1:
bne $t4, 0, cycle_start         #continue while

#end
POP $ra
jr $ra
nop

  # you can write your code for subroutine "hexasc" below this line
  hexasc:
  andi $t0, $a0, 0xf # Clear all bits except the last four 
  addi $t1, $zero, 9 # $t1 = 9
  slt $t2, $t1, $t0 # if (9 < $a0) {$t2 = 1}
  beq $t2, 1, if_bigger
  addi $t0, $t0, 0x30 #add 48 because 0 starts with that number.
  j return_value
  nop
  if_bigger: addi $t0, $t0, 0x37 #add 55 because 10 + 55 = 65 = 0x41 which corresponds to letter A
  j return_value
  nop
  return_value: move $v0, $t0
  jr $ra
  nop  


