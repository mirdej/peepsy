###############################################################################
# Configure the following variables according to your AVR.
# Program the device with
#     make fuse    			# to set the clock generator, boot section size etc.
#     make flash   			# to load the boot loader into flash
#     make lock    			# to protect the boot loader from overwriting
#     make disablereset		# for ATtiny85 target - to use external reset line for IO (CAUTION: this is not easy to enable again, see README) 

F_CPU = 1000000
DEVICE = attiny85

PROGRAMMER = -c USBasp
#PROGRAMMER = -c stk500hvsp -P avrdoper
#PROGRAMMER = -c stk500v2 -P avrdoper
# PROGRAMMER contains AVRDUDE options to address your programmer


FUSEOPT = -U lfuse:w:0x62:m -U hfuse:w:0x5f:m -U efuse:w:0xff:m
# Tools:
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
CC = avr-gcc


fuse:
	$(AVRDUDE) $(FUSEOPT)
	
