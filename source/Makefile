################################
##############ZeOS #############
################################
########## Makefile ############
################################

# package dev86 is required
AS86 = as86 -0 -a
LD86 = ld86 -0

HOSTCFLAGS = -Wall -Wstrict-prototypes -g
HOSTCC = gcc
CC = gcc
AS = as --32
LD = ld
OBJCOPY = objcopy -O binary -R .note -R .comment -S

INCLUDEDIR = include

CFLAGS = -m32 -O2 -g -fno-omit-frame-pointer -ffreestanding -Wall -I$(INCLUDEDIR) -fno-PIC
ASMFLAGS = -I$(INCLUDEDIR)
LDFLAGS = -g -melf_i386

SYSOBJ = \
	interrupt.o \
	entry.o \
	sys_call_table.o \
	io.o \
	tty.o \
	topbar.o \
	sched.o \
	sys.o \
	mm.o \
	devices.o \
	utils.o \
	hardware.o \
	list.o \
	systemwrap.o \
	stats.o \
	cyclic_buffer.o \
	ps2.o \
        

LIBZEOS = -L . -l zeos

#add to USROBJ any object files required to complete the user program
USROBJ = \
	libc.o \
	wrappers.o \
	adam/adam.o \
	adam/adamlib.o \
        adam/walls.o \
	# libjp.a \

all:zeos.bin

zeos.bin: bootsect system build user
	$(OBJCOPY) system system.out
	$(OBJCOPY) user user.out
	./build bootsect system.out user.out > zeos.bin

build: build.c
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

bootsect: bootsect.o
	$(LD86) -s -o $@ $<

bootsect.o: bootsect.s
	$(AS86) -o $@ $<

bootsect.s: bootsect.S
	$(CPP) $(ASMFLAGS) -traditional $< -o $@

entry.s: entry.S $(INCLUDEDIR)/asm.h $(INCLUDEDIR)/segment.h
	$(CPP) $(ASMFLAGS) -o $@ $<

sys_call_table.s: sys_call_table.S $(INCLUDEDIR)/asm.h $(INCLUDEDIR)/segment.h
	$(CPP) $(ASMFLAGS) -o $@ $<

wrappers.s: wrappers.S $(INCLUDEDIR)/asm.h
	$(CPP) $(ASMFLAGS) -o $@ $<

systemwrap.s: systemwrap.S $(INCLUDEDIR)/asm.h
	$(CPP) $(ASMFLAGS) -o $@ $<


user.o:user.c $(INCLUDEDIR)/libc.h

interrupt.o:interrupt.c $(INCLUDEDIR)/interrupt.h $(INCLUDEDIR)/segment.h $(INCLUDEDIR)/types.h

tty.o:tty.c $(INCLUDEDIR)/tty.h

io.o:io.c $(INCLUDEDIR)/io.h

ps2.o:ps2.c $(INCLUDEDIR)/ps2.h

topbar.o:topbar.c $(INCLUDEDIR)/topbar.h

sched.o:sched.c $(INCLUDEDIR)/sched.h

stats.o:stats.c $(INCLUDEDIR)/stats.h

libc.o:libc.c $(INCLUDEDIR)/libc.h

mm.o:mm.c $(INCLUDEDIR)/types.h $(INCLUDEDIR)/mm.h

sys.o:sys.c $(INCLUDEDIR)/devices.h

utils.o:utils.c $(INCLUDEDIR)/utils.h

cyclic_buffer.o:cyclic_buffer.c $(INCLUDEDIR)/cyclic_buffer.h

# ADAM
adam.o:adam/adam.c $(INCLUDEDIR)/adam.h

adamlib.o:adam/adamlib.c $(INCLUDEDIR)/adamlib.h

walls.o:adam/walls.c $(INCLUDEDIR)/adam.h

system.o:system.c $(INCLUDEDIR)/hardware.h system.lds $(SYSOBJ) $(INCLUDEDIR)/segment.h $(INCLUDEDIR)/types.h $(INCLUDEDIR)/interrupt.h $(INCLUDEDIR)/system.h $(INCLUDEDIR)/sched.h $(INCLUDEDIR)/mm.h $(INCLUDEDIR)/io.h $(INCLUDEDIR)/mm_address.h 


system: system.o system.lds $(SYSOBJ)
	$(LD) $(LDFLAGS) -T system.lds -o $@ $< $(SYSOBJ) $(LIBZEOS)

user: user.o user.lds $(USROBJ) 
	$(LD) $(LDFLAGS) -T user.lds -o $@ $< $(USROBJ)


clean:
	rm -f *.o adam/*.o *.s bochsout.txt parport.out system.out system bootsect zeos.bin user user.out *~ build 

disk: zeos.bin
	dd if=zeos.bin of=/dev/fd0

emul: zeos.bin
	bochs -q -f .bochsrc

gdb: zeos.bin
	bochs -q -f .bochsrc_gdb &
	gdb -tui -x .gdbcmd system

emuldbg: zeos.bin
	bochs_nogdb -q -f .bochsrc

emuldbg_gui: zeos.bin
	bochs_nogdb -q -f .bochsrc_gui

test: zeos.bin
	bochs_nogdb -q -f .bochsrc_test




