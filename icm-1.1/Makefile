
DIRS = icc icm msg

ifeq (${PREFIX},)
ifneq (${LR_HOST_ROOT},)
PREFIX=${LR_HOST_ROOT}
else
ifneq (${LR_ROOT},)
PREFIX=${LR_ROOT}
endif
endif
endif

all:
	@ for dir in ${DIRS}; do (cd $${dir}; ${MAKE}) ; done
	
clean:
	@ for dir in ${DIRS}; do (cd $${dir}; ${MAKE} clean) ; done

install:
	cp ./lib/*.so ${PREFIX}/lib
	mkdir -p ${PREFIX}/include/icm
	cp -r * ${PREFIX}/include/icm
	cp ./bin/icmmsg ${PREFIX}/bin
	cp ./conf/icmmsg.ini ${PREFIX}/conf
