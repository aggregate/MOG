BIN               := MOG

#CUDA_INSTALL_PATH ?= /usr/local/cuda-5.0
CUDA_INSTALL_PATH ?= /opt/cuda
#CUDA_SDK_PATH ?= $(HOME)/NVIDIA_GPU_Computing_SDK

NVCC ?= $(CUDA_INSTALL_PATH)/bin/nvcc
CC = g++
INCD = -I"$(CUDA_SDK_PATH)/C/common/inc" -I"$(CUDA_INSTALL_PATH)/include" -I"$(CUDA_INSTALL_PATH)/samples/common/inc"
LIBS = -L"$(CUDA_INSTALL_PATH)/lib64" -lcudart

# CUDA source files (compiled with nvcc)
CU_SOURCES      := mog.cu
CU_DEPS         := \
        op.h \
        do.h \
        pr.h \
        mog.h \
        mogas
CU_OBJS           := $(patsubst %.cu, %.cu.o, $(CU_SOURCES))

$(BIN): $(CU_OBJS)
	$(CC) -o $(BIN) $(CU_OBJS) $(INCD) $(LIBS)

mog.cu.o: mog.cu $(CU_DEPS)
	$(NVCC) -c -o $@ $< $(INCD)

clean:
	rm -f $(BIN) *.o *.cu.o
