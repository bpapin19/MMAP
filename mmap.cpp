#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <chrono>
using namespace std;
using namespace std::chrono;
#define OUTPUT_MODE 0700

int main(int argc, char** argv)
{
	/* Make sure the command line is correct */
	if(argc < 2)
	{
		cout<<"FILE NAME missing\n";

		exit(1);
	}
  auto start = high_resolution_clock::now();
	/* Open the specified file */
	int fd = open(argv[1], O_RDWR);
	int out_fd = open(argv[2], O_RDWR); //creat causing seg fault, ouotput file must already exist
  //int out_fd = creat(argv[2], OUTPUT_MODE); //create output file
  int offset = 0;
  int count = 0;
  int filesize;
  char* data = NULL;
  char* target = NULL;

	if(fd < 0)
	{
		cout<<"\n"<<"input file cannot be opened"<<"\n";
		exit(1);
	}

  if (out_fd < 0) {
      cout << "\n" << "output file cannot be created" << "\n";
      exit(1);        // if it cannot be created, exit
  }

	struct stat stats;
	if (stat(argv[1], &stats)==0) {
      filesize = stats.st_size;
      cout<<endl<<"file size "<<stats.st_size;
  } else {
	    cout<<"Unable to get file properties.\n";
    }

	/* Get the page size  */
	int pagesize = getpagesize();
  ftruncate(out_fd, filesize);

	cout<<endl<<"page size is " <<pagesize<<"\n";

		/* Print the whole file character-by-character */
      for (int pageCount = 0; pageCount < (filesize/pagesize); ++pageCount){
        //for(int fIndex = 0; fIndex < pagesize; ++fIndex) {
        offset = pagesize * pageCount;
        /* map the file into memory */
        data = (char*)mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);

        /* Did the mapping succeed ? */
      	if(!data)
      	{
      		cout<<"\n"<<"input mapping did not succeed"<<"\n";
      		exit(1);
        }
        //ftruncate(out_fd, pagesize++);
        target = (char*)mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, out_fd, offset);

        /* Did the mapping succeed ? */
        if (!target)
        {
            cout << "\n" << "output mapping did not succeed" << "\n";
            exit(1);
        }
        // cout << "data: ";
        // for (int i = 0; i < pagesize; ++i){
        //    cout << data[i];
        // }
        // cout << "target: ";
        // for (int i = 0; i < pagesize; ++i){
        //   cout << target[i];
        // }
				// cout << *data << endl;
				// cout << *data << endl;
				// cout << pageCount << endl;
        memcpy(target, data, pagesize);
        // write data to output file
        //write(out_fd, data, pagesize);



      /* Unmap the shared memory region */
      munmap(data, pagesize);
      munmap(target, pagesize);
    }

	/* Close the file */
	close(fd);
  //close(out_fd);
  auto stop = high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;
	return 0;
}
