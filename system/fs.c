#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#ifdef FS
#include <fs.h>

int get_free_block();
static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;
extern int dev0;
char block_cache[512];
#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2
#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;
#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int
fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}

int fs_open(char *filename, int flags) 
{
    struct directory dir=fsd.root_dir; 
    int x=0;
    int flag=0;
    int index=0;
    int flag2=0; 
    int temp=0;
    if(dir.numentries==0)
    {
        //there are no files in the system
       printf("no files in the system to open\n");
        return SYSERR;
    }
    else
    {
        //check if file the user is trying to open exist in the file-system directory
        for(x=0;x<dir.numentries;x++)
        {
            if(strncmp(filename, dir.entry[x].name,strlen(filename))==0)
            {
                //file found in the system so open it
                flag=1;
                index=x;
                break;
            }
        }
        if(flag==1)
        {
            if(next_open_fd==-1)
            {
                //next_open_fd not set
                printf("next_open_fd error\n");
                return SYSERR;
            }
            else
            {
                if(oft[next_open_fd].state == FSTATE_OPEN)
                {
                    //file is already open
                    printf("The file is already open");
                    return SYSERR;
                }
                else
                {
                    //open the file here
                    //updating the metadata of the file-table
                    oft[next_open_fd].state=FSTATE_OPEN;
                    oft[next_open_fd].fileptr=0;
                        //dirent in file-table struct
                        oft[next_open_fd].de=(void*)getmem(sizeof(struct dirent));
                        oft[next_open_fd].de->inode_num=dir.entry[index].inode_num;
                        strncpy(oft[next_open_fd].de->name,filename,strlen(filename));
                        
                        //inode in the file-table struct
                      //  oft[next_open_fd].in=getmem(sizeof(struct inode));
                        /*oft[next_open_fd].in->id=
                        oft[next_open_fd].in->type=
                        oft[next_open_fd].in->nlink=
                        oft[next_open_fd].in->device=
                        oft[next_open_fd].in->size=
                        oft[next_open_fd].in->blocks=*/
                    
                    //get the inode 
                    fs_get_inode_by_num(dev0, dir.entry[index].inode_num, &oft[next_open_fd].in);
                    
                    //change the next_open_fd to the next closed state file
                    temp=next_open_fd;
                    for(x=0;x<NUM_FD;x++)
                    {
                        if(oft[x].state==FSTATE_CLOSED)
                        {
                            next_open_fd=x;
                            flag2=1;
                        }
                    }
                    if(flag2==0)
                    {
                        //no closed state file found
                        next_open_fd=-1;
                        return temp;
                    }
                    else
                    {
                        //do nothing
                        return temp;
                    }
                }
            }
        }
        else
        {
            //file requested to open not found in the file-system
            printf("no file found in the system with this name\n");
            return SYSERR;
        }
    } 
}

int fs_create(char *filename, int mode) 
{
    int x;
    struct directory dir=fsd.root_dir; // getting the root_dir for te directory structure
    struct inode *i_node=(void*)getmem(sizeof(struct inode));
    int flag=0;
    if(mode==O_CREAT)
    {
        if(fsd.root_dir.numentries==0)
        {
            //no file in the file system simply create a new file and dont check for the same name in the structure
            flag=1;           
        }
        else
        {   
            //check if file name exist in the fsd.root_dir
            for(x=0;x<dir.numentries;x++)
            {
                if(strncmp(filename,fsd.root_dir.entry[x].name,strlen(filename))==0)
                {
                    //file wit same name found and thus cannot create a file
                    printf("Cannot create file please enter a new file_name\n");
                    flag=0;
                    break;
                }
                else
                {
                    flag=1;
                }
            }
        }
        
        if(flag==1)
        {
            //setting the inode metadata and setting it
            i_node->id=fsd.inodes_used++;
            i_node->type=INODE_TYPE_FILE;
            i_node->nlink=1;
            i_node->device=dev0;
            i_node->size=0;
            for(x=0;x<INODEBLOCKS;x++)
            {
                i_node->blocks[x]=-1;
            }
            fs_put_inode_by_num(dev0, fsd.inodes_used-1, i_node);
            
            //now we update the root directory entry by setting the inode_num and filename
            fsd.root_dir.entry[fsd.root_dir.numentries].inode_num=fsd.inodes_used-1;
            strncpy(fsd.root_dir.entry[fsd.root_dir.numentries].name,filename,strlen(filename));
            fsd.root_dir.numentries=fsd.root_dir.numentries+1;
            //printf("%d\n",dir.numentries);            
            return fs_open(filename,O_RDWR);
        }
        else
        {
            printf("flag error\n");
            return SYSERR;
        }
    }
    else
    {
        printf("mode error\n");
        return SYSERR; //wrong mode
    }
}

int fs_seek(int fd, int offset) 
{
  int ptr;
  ptr = oft[fd].fileptr;
  ptr =ptr+offset;
  if(ptr>oft[fd].in.size)
  {
    //fileptr size exceeded the inode size
    return SYSERR;
  }
  else
  {
    if(ptr<0)
    {
      //pointer goes below zero no data to seek
      return SYSERR;
    }
    oft[fd].fileptr = ptr;
    return OK;  
  }
  
}

int fs_read(int fd, void *buf, int nbytes)
{
  char *buffer=buf;
  //get the index and the block number to start reading the file from
  int i_node_block_index=oft[fd].fileptr/dev0_blocksize;
  int i_node_block_number=oft[fd].in.blocks[i_node_block_index];
  int i_node_offset=oft[fd].fileptr % dev0_blocksize;
  if(oft[fd].state==FSTATE_CLOSED)
  {
    //the file is in closed state cannot read it
    printf("Closed state file\n");
    return SYSERR;
  }
  else
  {
      int set=0;
      int temp=0;
      while(nbytes>set)
      {
        //if the fileptr exceeds the inode size means it has read the last block of that file
        if(oft[fd].fileptr>=oft[fd].in.size)
        {
          return set;
        }

        //check if the actual read block size has exceeded the bytes requested to be read, if its true we set temp which is the length of the block size to be read, if the size doesnt exceed we simply set the device blocksize - the offset given by the user
        if(dev0_blocksize-i_node_offset > nbytes)
        {
          temp=nbytes;
        }
        else
        {
          temp=dev0_blocksize-i_node_offset;
        }
//	void *character=(void *)buf[set];
        int block_read=bs_bread(dev0, i_node_block_number, i_node_offset, &buffer[set], temp);
        if(block_read==SYSERR)
        {
          printf("Error in reading block");
          oft[fd].fileptr=oft[fd].fileptr+set;
          return set;
        }
        else
        {
          //increasing the iteration and filepointer by the blocksize read till now!!
          //and we set the offset to 0 as only for the first read the offset is required after that it is seqential
          set=set+temp;
          oft[fd].fileptr=oft[fd].fileptr+temp;
          nbytes=nbytes-temp;
          i_node_block_index=i_node_block_index+1;
          i_node_offset=0;
          if(i_node_block_number==-1)
          {
            //failed to read the block
            printf("Failed to read the block\n");
            return SYSERR;
          }
          else
          {
            //continue the loop
          }
        }

      }
      return set;
    }
    
}

int fs_write(int fd, void *buf, int nbytes) 
{
    char *buffer=buf;
    if(oft[fd].state==FSTATE_CLOSED)
    {
        //the file is closed cannot write in it
        printf("The file is closed\n");
        return SYSERR;
    }
    else
    {
        //set the inode size equal to the filepointer of the file
        //struct inode i_node=oft[fd].in;
	//struct inode i_node=(void*)getmem(sizeof(struct inode));
        //i_node.size = oft[fd].fileptr; 
        
        //calculate the inode block index and the offset size
        int i_node_block_index=oft[fd].fileptr/dev0_blocksize;
        int i_node_offset=oft[fd].fileptr%dev0_blocksize;
        int i_node_block_number=oft[fd].in.blocks[i_node_block_index];
        
        if(i_node_block_number==-1)
        {
            //need more free blocks
            i_node_block_number=get_free_block();
            oft[fd].in.blocks[i_node_block_index]=i_node_block_number;
        }
        int temp=0;
        int buffer_counter=0;
        while(nbytes>0)
        {
            //set the mask bit accoding to the i node block number calculated in each iteration that way all the mask will be set according to the i node block number
            fs_setmaskbit(i_node_block_number);
            //same explaination as read
            if(dev0_blocksize-i_node_offset>nbytes)
            {
                temp=nbytes;
            }
            else
            {
                temp=dev0_blocksize-i_node_offset;
            }
            
            int block_write=bs_bwrite(dev0, i_node_block_number, i_node_offset, &buffer[buffer_counter], temp);
            if(block_write!=SYSERR)
            {
                nbytes=nbytes-temp;
                buffer_counter=buffer_counter+temp;
                i_node_block_index=i_node_block_index+1;
                oft[fd].in.blocks[i_node_block_index]=get_free_block(); //get the next free block in the blocks array of inode
                i_node_block_number=oft[fd].in.blocks[i_node_block_index];
                i_node_offset=0;//offset is only used to start writing the first initial block
            }
            else
            {
                printf("Failed to write in the block");
                oft[fd].in.size=oft[fd].in.size+buffer_counter;
                return buffer_counter;
            }
        }
        int increament=oft[fd].in.size;
        oft[fd].in.size=increament+buffer_counter; //increase the size of the inode and the filepointer
        oft[fd].fileptr=increament+buffer_counter;
        return buffer_counter;
    }
}
int fs_close(int fd) 
{
  //free the memory and change the state to closed state
  freemem((char*)oft[fd].de,sizeof(struct dirent));
  oft[fd].state = FSTATE_CLOSED;
  return OK;
}
int get_free_block(){
    int x;
    int flag=1;
    int index=0;
    for(x=0;x<dev0_numblocks;x++)
    {
        if(fs_getmaskbit(x)==0)
        {
	    index=x;
	    flag=1;
            break;
        }
    }
    if(flag==1)
    {
	//free block found 
	return index;	
    }	
    else
    {
   	 printf("No free blocks available\n");
	 return -1;
    }
}
#endif
