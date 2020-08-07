#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#define METHOD_LENGTH 0.5

typedef struct Method
{
    long int invocation;
	long int response;
	char method_call [80];
}Method;

int main(int argc,char* argv[]) 
{
	FILE * readFile;
	FILE * writeFile;

	int numThrds;

	readFile = fopen("thrd_lists.txt","r");

	writeFile = fopen("VSVVisual.tex","w");

	//long int invocation, response;
	//char str [80];

	long int min_invocation = LLONG_MAX;
	long int min_method_call = LLONG_MAX;
	
	fscanf(readFile, "%d", &numThrds);

	Method* method_list[numThrds];

	int numMethods[numThrds];

	int pos[numThrds];

	for(int i = 0; i < numThrds; i++)
	{
		pos[i] = 0;

		fscanf(readFile, "%d", &numMethods[i]);
		printf("numMethods[%d] = %d\n", i, numMethods[i]);

		method_list[i] = (Method*) malloc(numMethods[i] * sizeof(Method));

		for(int j = 0; j < numMethods[i]; j++)
		{
			fscanf(readFile, "%lu", &method_list[i][j].invocation);
			fscanf(readFile, "%lu", &method_list[i][j].response);
			fscanf(readFile, "%s", method_list[i][j].method_call);

			if(j == 0 && method_list[i][j].invocation < min_invocation)
			{
				min_invocation = method_list[i][j].invocation;
			}
			
			if((method_list[i][j].response - method_list[i][j].invocation) < min_method_call)
			{
				min_method_call = method_list[i][j].response - method_list[i][j].invocation;
			}
		}
	}

/*
	for(int i = 0; i < numThrds; i++)
	{
		printf("Thread %d\n", i);
		for(int j = 0; j < numMethods[i]; j++)
		{
			printf("invocation = %ld, response = %ld\n", method_list[i][j].invocation, method_list[i][j].response);
		}
	}
*/

	fprintf(writeFile,  "\\documentclass{article}\n");
	fprintf(writeFile,  "\\usepackage[utf8]{inputenc}\n");
	fprintf(writeFile,  "\\usepackage{tikz}\n");
	fprintf(writeFile,  "\\usepackage[legalpaper, landscape, margin=0.5in]{geometry}\n");
	fprintf(writeFile,  "\\title{VSV-Visualization}\n");
	fprintf(writeFile,  "\\date{}\n");
	fprintf(writeFile,  "\\begin{document}\n");
	fprintf(writeFile,  "\\maketitle\n");

	float scale_factor = (float) METHOD_LENGTH/(min_method_call);
	float x_offset = (float) min_invocation * scale_factor;
	bool stop = false;

	printf("scale_factor = %.5f, x_offset = %0.5f\n", scale_factor, x_offset);

	while(!stop)
	{
		fprintf(writeFile,  "\\begin{figure}[h!]\n");
		fprintf(writeFile,  "\\centering\n");
		fprintf(writeFile,  "\\begin{tikzpicture}[draw=black, scale=0.75, transform shape]\n");
		
		stop = true;

		for(int i = 0; i < numThrds; i++)
		{
			int y_offset = (numThrds - i);
			fprintf(writeFile,  "\n\\node[align=left] at (-0.5,%d) {P%d};\n", y_offset, i);		

			for(int j = pos[i]; j < numMethods[i]; j++)
			{
				
				if((float) (method_list[i][j].response*scale_factor - x_offset) > 40)
				{
					//x_offset = (float) (invocation*scale_factor);
					stop = false;
					printf("Thread %d stopped,  val = %.5f, response = %ld, x_offset = %.5f, j = %d\n", i, (method_list[i][j].response*scale_factor - x_offset), method_list[i][j].response, x_offset, j);
					
					if((float) (method_list[i][j].invocation*scale_factor - x_offset) < 40)
					{
						fprintf(writeFile,  "\\draw [thick] (%.5f,%d) -- (%d,%d);\n", (float) (method_list[i][j].invocation*scale_factor - x_offset), y_offset, 40, y_offset);
			
						fprintf(writeFile,  "\\node[align=center, above] at (%.5f,%d) {$%s$};%% x_offset = %.5f\n", (float) ((method_list[i][j].invocation*scale_factor - x_offset) + 40)/2 , y_offset, method_list[i][j].method_call, x_offset);
						method_list[i][j].invocation = (long int) (40 + x_offset) / scale_factor;
					}			


					break;
				} 
				pos[i] = j + 1;

				fprintf(writeFile,  "\\draw [thick] (%.5f,%d) -- (%.5f,%d);\n", (float) (method_list[i][j].invocation*scale_factor - x_offset), y_offset, (float) (method_list[i][j].response*scale_factor - x_offset), y_offset);
			
				fprintf(writeFile,  "\\node[align=center, above] at (%.5f,%d) {$%s$};%% x_offset = %.5f\n", (float) ((method_list[i][j].invocation + (method_list[i][j].response-method_list[i][j].invocation)/2)*scale_factor - x_offset), y_offset, method_list[i][j].method_call, x_offset);
			
			}
		}
		fprintf(writeFile,  "\\end{tikzpicture}\n");
		fprintf(writeFile,  "\\caption{Concurrent History}\n");
		fprintf(writeFile,  "\\end{figure}\n");

		if(!stop)
		{
			min_invocation = LLONG_MAX;
			for(int i = 0; i < numThrds; i++)
			{
				printf("pos[%d] = %d\n", i, pos[i]);
				//if(pos[i] < (numMethods[i] - 1) && method_list[i][pos[i]].invocation < min_invocation)
				if(pos[i] < numMethods[i] && method_list[i][pos[i]].invocation < min_invocation)
				{
					min_invocation = method_list[i][pos[i]].invocation;
				}
			} 
			
			x_offset = (float) min_invocation * scale_factor;
			printf("min_invocation = %ld, x_offset = %.2f, scale_factor = %.5f\n", min_invocation, x_offset, scale_factor);
		}
		
	}

	fprintf(writeFile,  "\\end{document}\n");

	return 0; 
} 
