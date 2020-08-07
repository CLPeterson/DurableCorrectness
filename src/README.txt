Makefile can be updated at petra/dev/src/Makefile.am

make CPPFLAGS="-DUSE_DRAM_ALLOCATOR -DPWB_IS_CLWB -DROMULUS_LR_PTM"
./src/trans 0 2 10000 1 10000 50 50

For Debug:
make CPPFLAGS="-DUSE_DRAM_ALLOCATOR -DPWB_IS_CLWB -DROMULUS_LR_PTM -O0 -g"

Configure Makefile at: petra/dev/src/Makefile.am 

For PETRA or Romulus:
make CPPFLAGS="-DUSE_PMEM_ALLOCATOR -DPWB_IS_CLWB -DROMULUS_LR_PTM"

For PETRA or PMDK:
make CPPFLAGS="-DUSE_PMEM_ALLOCATOR -DPWB_IS_CLWB -DPMDK_PTM"

For Debug:
make CPPFLAGS="-DUSE_PMEM_ALLOCATOR -DPWB_IS_CLWB -DROMULUS_LR_PTM -O0 -g"

To Test Persistent Memory:
sudo ./src/trans 0 2 10000 1 10000 50 50
sudo rm /mnt/pmem0/*

ListBench.cpp
LinkFree/LinkFreeList.h
include/BenchUtils.h
SLBench.cpp

cout << "  -a     algorithm" << endl;
cout << "  -p     thread num" << endl;
cout << "  -d     duration" << endl;
cout << "  -R     lookup ratio (0~100)" << endl;
cout << "  -M     key range" << endl;
cout << "  -I     iteration number" << endl;
cout << "  -t     test number" << endl;

./list -a LinkFreeList -p 2 -R 0 -M 5 -I 5 -t 5
BenchUtils.h: runBench()


Log-free data structures
nv-lf-structures/common/Makefile.common
commented out line 188

libnvram/Makefile
commented out line 72
