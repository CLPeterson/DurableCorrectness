# PETRA
## Build:
Install dependencies:

`sudo apt-get install libboost-all-dev libgoogle-perftools-dev libtool m4 automake cmake libtbb-dev libgsl0-dev` <br />
`cd src/petra/` <br />
`mkdir compile` <br />
`cd dev` <br />
`bash bootstrap.sh` <br />
`cd ../compile` <br />
`../dev/configure` <br />
`make CPPFLAGS="-DUSE_DRAM_ALLOCATOR -DPWB_IS_CLFLUSH -DROMULUS_LR_PTM -O0 -g"` <br />

## Options:
setType: { 0="TransList" , 1="TransSkip", 2="TransMDList", 3="TransMap" } <br />
numThread: Number of Threads <br />
testSize: Test Size <br />
tranSize: Transaction Size <br />
keyRange: Key Range <br />
insertion: Insertion Percent <br />
deletion: Deletion Percent <br />
-v: verbose <br />

## Run:
`cd src/petra/compile` <br />
`./src/trans <setType> <numThread> <testSize> <tranSize> <keyRange> <insertion> <deletion>` 

For example, to test TransSkip with 16 threads, test size is 100, transaction size is 4 operations, key range is 1000, insert percent is 50, and delete percent is 25 with verbose flag: <br />
`./src/trans 1 16 100 4 1000 50 25 -v`

# Durable Sets
## Build:

`cd src/Link-Free/` <br />
`make`

## Options:
setType: { 0="LinkFreeList" , 1="LinkFreeSkip", 2="LinkFreeHash" } <br />
numThread: Number of Threads <br />
testSize: Test Size <br />
keyRange: Key Range <br />
insertion: Insertion Percent <br />
deletion: Deletion Percent <br />
-v: verbose <br />

## Run:
`cd src/Link-Free/` <br />
`./main <setType> <numThread> <testSize> <tranSize> <keyRange> <insertion> <deletion>`

For example, to test LinkFreeHash with 16 threads, test size is 100, key range is 1000, insert percent is 50, and delete percent is 25: <br />
`./main 2 16 100 1 1000 50 25`

# To Use VSV-D for Arbitrary User Code:
Place vsv.cc, vsv.h, timehelper.cc, timehelper.h, and threadbarrier.h in directory named common

Update Makefile

OBJ = common/vsv.o common/timehelper.o

common/vsv.o: common/vsv.cc common/vsv.h <br />
&nbsp;&nbsp;&nbsp;&nbsp;g++ -o common/vsv.o -c common/vsv.cc -ltbb -ggdb -O0 -g

common/timehelper.o: common/timehelper.cc common/timehelper.h <br />
&nbsp;&nbsp;&nbsp;&nbsp;g++ -o common/timehelper.o -c common/timehelper.cc

List $(OBJ) -ltbb in line in Makefile that creates the executable

Include vsv.h in the driver file for the experimental evaluation: <br />
`#include "../common/vsv.h"`

Call the following commands at the beginning of the main method to initialize the command line arguments. These functions must be configured by the user based on the format of the command line arguments for the executable. <br />
`vsv_args(argc, argv);` <br />
`vsv_init();`

Call the following command at the end of the worker thread body. <br />
`vsv_exit();`

Call the following command in the main method. <br />
`vsv_startup();`
* Calling vsv_startup(); immediately afte launching worker threads will execute the verification thread while program is running
* Calling vsv_startup(); after threads have joined will execute the verification thread after the program is finished.

Call the following command to join the verification thread and finalize verification results. <br />
`vsv_shutdown();`
