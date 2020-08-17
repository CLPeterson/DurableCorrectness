//petra/compile: make CPPFLAGS="-DUSE_PMEM_ALLOCATOR -DPWB_IS_CLWB -DROMULUS_LR_PTM"

#include <stdio.h>
#include <stdlib.h>
#include <string>

#define NUM_THRDS "48"

FILE *pfile;

int main(int argc,char* argv[]) 
{ 
	system("sudo [ -f ../compile/output.txt ] && rm ../compile/output.txt");
	system("[ -f output.txt ] && rm output.txt");

	std::string command;

	std::string executable("sudo ../compile/src/./transRomulus");
	std::string test_size;
	std::string key_range;
	std::string data_structure;

	std::string cleanup("sudo rm /mnt/pmem0/*");
	//std::string cleanup("sudo rm /dev/shm/*");

	system(cleanup.c_str());
	
	//for(unsigned int i = 0; i < 3; i++)
	for(unsigned int i = 0; i < 6; i++)
	{	
		test_size.clear();
		key_range.clear();
		if (i == 0)
		{
			test_size.append(" 10");
			key_range.append(" 100");
		} else if (i == 1)
		{
			test_size.append(" 100");
			key_range.append(" 1000");
		} else if (i == 2)
		{
			test_size.append(" 1000");
			key_range.append(" 1000");
		} else if (i == 3)
		{
			test_size.append(" 10000");
			key_range.append(" 10000");
		} else if (i == 4)
		{
			test_size.append(" 100000");
			key_range.append(" 100000");
		} else if (i == 5)
		{
			test_size.append(" 1000000");
			key_range.append(" 100000");
		}
		
		for(unsigned int j = 0; j < 4; j++)
		{
			data_structure.clear();
			if(j == 0)
			{
				data_structure.append(" 0 ");
			} else if (j == 1)
			{
				data_structure.append(" 1 ");
			} else if (j == 2)
			{
				data_structure.append(" 2 ");
			} else if (j == 3)
			{
				data_structure.append(" 3 ");
			} 
			command.clear();
			command.append(executable);
			command.append(data_structure);
			command.append(NUM_THRDS);
			command.append(test_size);
			command.append(" 4 ");
			command.append(key_range);
			command.append(" 33 33");
			system(command.c_str());
			system(cleanup.c_str());

		}
		system("sudo chmod 777 output.txt");
		pfile = fopen("output.txt", "a");
		if (pfile == NULL) {
		    perror("fopen()");
		    return EXIT_FAILURE;
		}
		fprintf(pfile, "\n\n\n\n\n");
		fclose(pfile);

	}

	system("cp output.txt outputRomulus.txt");

	return 0; 
}

