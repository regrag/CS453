
KERNEL_SOURCE=/lib/modules/`uname -r`/build
all:
	 make -C ${KERNEL_SOURCE} M=`pwd` modules

obj-m += booga.o

clean:
	/bin/rm --force .booga* booga.o booga.mod.c booga.mod.o booga.ko Modules.*
	/bin/rm -fr .tmp_versions/



