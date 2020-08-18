#include <stdio.h>
#include <stdlib.h>
#include <string>

#define NUM_THRDS "48"

FILE *pfile;

int main(int argc,char* argv[]) 
{ 
	system("[ -f ../output.txt ] && rm ../output.txt");
	system("[ -f output.txt ] && rm output.txt");

	std::string command;

	std::string executable("../");
	std::string test_size;
	std::string key_range;
	std::string data_structure;
	
	//for(unsigned int i = 0; i < 4; i++)
	for(unsigned int i = 0; i < 5; i++)
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
			key_range.append(" 100");
		} else if (i == 2)
		{
			test_size.append(" 1000");
			key_range.append(" 1000");
		} else if (i == 3)
		{
			test_size.append(" 10000");
			key_range.append(" 1000");
		} else if (i == 4)
		{
			test_size.append(" 25000");
			key_range.append(" 1000");
		} 
		
		//First parameter set to 0 since it is irrelevant for this test
		for(unsigned int j = 0; j < 4; j++)
		{
			data_structure.clear();
			if(j == 0)
			{
				data_structure.append("./main-lf-linkedlist 0 ");
			} else if (j == 1)
			{
				data_structure.append("./main-lf-skiplist 0 ");
			} else if (j == 2)
			{
				data_structure.append("./main-lf-bst 0 ");
			} else if (j == 3)
			{
				data_structure.append("./main-hashtable 0 ");
			} 
			command.clear();
			command.append(executable);
			command.append(data_structure);
			command.append(NUM_THRDS);
			command.append(test_size);
			command.append(" 1 ");
			command.append(key_range);
			command.append(" 33 33");
			system(command.c_str());

		}
		pfile = fopen("output.txt", "a");
		fprintf(pfile, "\n\n\n\n\n");
		fclose(pfile);

	}
	
	return 0; 
}

