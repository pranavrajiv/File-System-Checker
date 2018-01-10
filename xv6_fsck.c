#include<string.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<sys/mman.h>


#define IPB           (BSIZE / sizeof(struct dinode))
#define ROOTINO 1  // root i-number
#define BSIZE 512  // block size
#define BBLOCK(b, ninodes) (b/BPB + (ninodes)/IPB + 3)
#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)
#define DIRSIZ 14 


// File system super block
struct superblock {
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
};

// On-disk inode structure
struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEV only)
  short minor;          // Minor device number (T_DEV only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[NDIRECT+1];   // Data block addresses
};

struct dirent {
    ushort inum;
    char name[DIRSIZ];
};














bool checkerFourteenFun(int parentInodeIndex, void * img_ptr,int index)
{


    struct superblock *sb;
    sb= (struct superblock *) (img_ptr + BSIZE);
    
    struct dinode *dip =  (struct dinode *) (img_ptr + BSIZE*2);

    for(int i=0;i<parentInodeIndex;i++)
        dip++;
   
   
   for(int i=0;i<sb->nblocks;i++)
   {


    if(dip->addrs[0]!=0)
    {
    
        struct dirent *checkerNineDirect =  (struct dirent *)(img_ptr + dip->addrs[0]*BSIZE);
        checkerNineDirect++;
        if(checkerNineDirect->inum == index)
            return true;
        else
        {
            printf("checkerNineDirect->inum = %d and index ===================================================================== %d \n",checkerNineDirect->inum,index);
            
            dip =  (struct dinode *) (img_ptr + BSIZE*2);
            int k;
            for(k=0;i<checkerNineDirect->inum;k++)
            {
                dip++;
            }
            index = k;

        }
        
    }


    }

    struct dinode *dipCp =  (struct dinode *) (img_ptr + BSIZE*2);
    for(int i =0;i<index;i++)
        dipCp++;

        if(dipCp->type ==0)
        return true;
    
    //printf("index ===================================================================== %d \n",index);
    return false;
}







bool checkerThirteenFun(int parentInodeIndex, void * img_ptr, int inodeIndex)
{

    if(inodeIndex==1)
        return true;


    struct dinode *dip =  (struct dinode *) (img_ptr + BSIZE*2);
    
    for(int i=0;i<parentInodeIndex;i++)
        dip++;
        
        
        
        
        for(int j=0;j<12;j++)
            if(dip->addrs[j]!=0)
            {

                struct dirent *checkerNineDirect =  (struct dirent *)(img_ptr + dip->addrs[j]*BSIZE);
            
                int k=0;
            
                if(j==0)
                {
                    k=2;
                    checkerNineDirect++;
                    checkerNineDirect++;
                }
            
                
                for(;k<BSIZE/sizeof(struct dirent);k++)
                {

                   if(checkerNineDirect->inum == inodeIndex)
                     return true;   
                        
                    checkerNineDirect++;
                }  
            }
        
        
        
        
        
        
        if(dip->addrs[12]!=0)
        {  
              
            int *checkerNineIndirectPart1 =  (int *)(img_ptr + dip->addrs[12]*BSIZE);
            for(int j=0;j<NINDIRECT;j++)
              {
               
               if(*checkerNineIndirectPart1==0) 
               {
                    checkerNineIndirectPart1++;
                    continue;
               }

                struct dirent *checkerNineIndirectPart2 =  (struct dirent *)(img_ptr + (*checkerNineIndirectPart1) *BSIZE);
                
                for(int k=0;k<BSIZE/sizeof(struct dirent);k++)
                {
                    
                    
                    if(checkerNineIndirectPart2->inum ==  inodeIndex)
                       return true;


                    checkerNineIndirectPart2++;
            
                }
                checkerNineIndirectPart1++;
             } 
        }
        
        return false;
}


bool checkerTwelveFun(int inodeUsed[],int size,void * img_ptr)
{

    struct dinode *dip =  (struct dinode *) (img_ptr + BSIZE*2);
    dip++;
    dip++;


    for(int i=2;i<size;i++)
    {
        if(dip->type == 1)
            if(inodeUsed[i]>1)
                return false;
        dip++;
    }


    return true;
}



bool checkerElevenFun(int inodeUsed[],int size,void * img_ptr)
{

    struct dinode *dip =  (struct dinode *) (img_ptr + BSIZE*2);
    dip++;
    dip++;

    for(int i=2;i<size;i++)
    {
        if(inodeUsed[i]!=-1)        
            if(inodeUsed[i]!=dip->nlink)
                return false;
        dip++;
    }
    return true;
}





bool checkerTenFun(int inodeIndex,void * img_ptr)
{

    if(inodeIndex ==0 || inodeIndex ==1)
        return true;
    
    struct dinode *dip =  (struct dinode *) (img_ptr + BSIZE*2);

    for(int i=0;i<inodeIndex;i++)
        dip++;
    
    if(dip->type ==0)
        return false;

    return true;
}


bool bitInUse (char * x,int blockNo)
{

    int val;
    if(blockNo%8 !=0)
        val =  (x[blockNo/8]>>(blockNo%8))&1;
    else
        val = x[blockNo/8]&1;

    
    if(val==1)
        return true;
    else
        return false;

}



int
main(int argc, char *argv[])
{

    if(argc !=2)
    {
        fprintf( stderr,"Usage: xv6_fsck <file_system_image>.\n");       
        exit(1);
    }


    int fd;

    if((fd=open(argv[1],O_RDONLY)) <= -1)
    {
        fprintf( stderr, "image not found.\n");
        exit(1);
    }
    
   
    int rc;
    struct stat sbuf;
    rc = fstat(fd, &sbuf);
    assert(rc == 0);
    

    void *img_ptr = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE,fd,0);
    assert(img_ptr != MAP_FAILED);

    struct superblock *sb;
    sb= (struct superblock *) (img_ptr + BSIZE);
    
    struct dinode *dip =  (struct dinode *) (img_ptr + BSIZE*2);
    

    int inodeStartIndex= (sb->ninodes/ ( BSIZE/sizeof(struct dinode)))+3;
    int inodeEndIndex= inodeStartIndex + sb->nblocks-1;

    int usedBlocksArray[sb->size];
    int usedDirectArray[sb->size];
    int usedIndirectArray[sb->size];
    int inodeUsed[sb->ninodes];
    
    int checkerTen =1;
    int checkerThirteen = 1;    
    int checkerFourteen = 1;
    //bitmap address
    char * bmp= (char *)(img_ptr + BSIZE*2 + (sb->ninodes/ ( BSIZE/sizeof(struct dinode)) )*BSIZE + BSIZE);
    
    for(int i =0;i< sb->ninodes; i++)
    {
        //populate the inodeUsed array for test Checker 9 part1
        inodeUsed[0]=-1;
        if(dip->type== 0)
            inodeUsed[i]=-1;
     
        //test Badroot
        if(i==1)
            if(dip->type!= 1)
            {
                fprintf( stderr,"ERROR: root directory does not exist.\n");
                exit(1);
            }
     
     
        // test Badinode
        if(dip->type!= 0 && dip->type!= 1 && dip->type!= 2 && dip->type!= 3)
        {
            fprintf( stderr,"ERROR: bad inode.\n");
            exit(1);
        }
    
        // test BadDirect
        for(int j=0;j<12;j++)
            if(dip->addrs[j]!=0)
                if(!(dip->addrs[j]>=inodeStartIndex && dip->addrs[j]<= inodeEndIndex) )
                {
                    fprintf( stderr,"ERROR: bad direct address in inode.\n");
                    exit(1);
                }
       
       // test BadInDirect Part1
        if(dip->addrs[12]!=0) 
            if(!(dip->addrs[12]>=inodeStartIndex && dip->addrs[12]<= inodeEndIndex) )
            {
                fprintf( stderr,"ERROR: bad indirect address in inode.\n");
                exit(1);
            }


        // test BadInDirect Par2
        int * indirectAddress = (int *)(img_ptr + dip->addrs[12]*BSIZE);
        for(int k = 0;k<NINDIRECT && dip->addrs[12]!=0;k++)
        {
            if(*indirectAddress!=0)
                if(!(*indirectAddress >=inodeStartIndex && *indirectAddress <= inodeEndIndex) )
                {
                    fprintf( stderr,"ERROR: bad indirect address in inode.\n");
                    exit(1);
                }
                
            indirectAddress++;
                
                
        }



        //test directory not formatted
        if(i>0)
            if(dip->type==1)
                if(dip->addrs[0]!=0)
                {
                    struct dirent *dirNotFormatTestVar =  (struct dirent *)(img_ptr + dip->addrs[0]*BSIZE);
            
                    if(strcmp(".",dirNotFormatTestVar->name)!=0 || dirNotFormatTestVar->inum != i)
                    {
                        fprintf( stderr,"ERROR: directory not properly formatted.\n");
                        exit(1);
                    }
               
                    dirNotFormatTestVar++;
                
   
   
                    //checkerThirteenFun
                    if(checkerThirteenFun(dirNotFormatTestVar->inum,img_ptr,i)==false)
                        checkerThirteen =0;

                    //checkerFourteen
                    //if(checkerFourteenFun(dirNotFormatTestVar->inum,img_ptr,i)==false)
                      //  checkerFourteen =0;


                    if(strcmp("..",dirNotFormatTestVar->name)!=0)
                    {
                        fprintf( stderr,"ERROR: directory not properly formatted.\n");
                        exit(1);

                    }   
            
                }   



        //test Badroot2
        if(i==1)
        {
            struct dirent *badRoot2TestVar =  (struct dirent *)(img_ptr + dip->addrs[0]*BSIZE);
            if(badRoot2TestVar->inum!=1)
            {
                fprintf( stderr,"ERROR: root directory does not exist.\n");
                exit(1);
            }

            badRoot2TestVar++;
            if(badRoot2TestVar->inum!=1)
            {
                fprintf( stderr,"ERROR: root directory does not exist.\n");
                exit(1);
            }
        }
    

    
    //Test address used by inode but marked free in bitmap.
    if(dip->type== 1 || dip->type== 2 || dip->type== 3)
    {  

        for(int j=0;j<13;j++)
            if(dip->addrs[j]!=0)
            {
               //populate the usedBlocksArray[IPB+2+sb->nblocks] for test Checker 6 and more part 1
               usedBlocksArray[dip->addrs[j]] =1;
                
               //populate the usedDirectArray[IPB+2+sb->nblocks] for test Checker 7
                usedDirectArray[dip->addrs[j]]++;
                
                if(bitInUse(bmp,dip->addrs[j])==false)
                {
                    fprintf( stderr,"ERROR: address used by inode but marked free in bitmap.\n");
                    exit(1);
                }
            }


    
        int * indirectAddress = (int *)(img_ptr + dip->addrs[12]*BSIZE);
        for(int k = 0;k<NINDIRECT && dip->addrs[12]!=0;k++)
        {
            if(*indirectAddress!=0)
            {
                
               //populate the usedBlocksArray[IPB+2+sb->nblocks] for test Checker 6 and more part 2
               usedBlocksArray[*indirectAddress] =1;
               
                
               //populate the usedIndirectArray[IPB+2+sb->nblocks] for test Checker 8
                usedIndirectArray[*indirectAddress]++;

               if(bitInUse(bmp,*indirectAddress)==false)
                {
                    fprintf( stderr,"ERROR: address used by inode but marked free in bitmap.\n");
                    exit(1);
                }
            }

            indirectAddress++;
                
        } 
    
    
    
    }


    //populate the inodeUsed for test Checker 9
    if(dip->type==1)
    {
    
        for(int j=0;j<12;j++)
            if(dip->addrs[j]!=0)
            {

                struct dirent *checkerNineDirect =  (struct dirent *)(img_ptr + dip->addrs[j]*BSIZE);
            
                int k=0;
            
                if(j==0)
                {
                    k=2;
                    checkerNineDirect++;
                    checkerNineDirect++;
                }
            
                
                for(;k<BSIZE/sizeof(struct dirent);k++)
                {
                    //if(checkerNineDirect->inum == 0)
                      //  break;
                    
                    if(checkerNineDirect->inum != 0)
                    {
                        inodeUsed[checkerNineDirect->inum]++;

                        //populate the inodeUsed for test Checker 10
                        if(checkerTenFun(checkerNineDirect->inum,img_ptr)==false)
                            checkerTen =0;
                   }
                    checkerNineDirect++;
                }  
            }
        
        
        if(dip->addrs[12]!=0)
        {  
              
            int *checkerNineIndirectPart1 =  (int *)(img_ptr + dip->addrs[12]*BSIZE);
            for(int j=0;j<NINDIRECT;j++)
              {
               
               if(*checkerNineIndirectPart1==0) 
               {
                    checkerNineIndirectPart1++;
                    continue;
               }

                struct dirent *checkerNineIndirectPart2 =  (struct dirent *)(img_ptr + (*checkerNineIndirectPart1) *BSIZE);
                
                for(int k=0;k<BSIZE/sizeof(struct dirent);k++)
                {
                    //if(checkerNineIndirectPart2->inum == 0)
                      //  break;
                    
                    if(checkerNineIndirectPart2->inum != 0)
                    {
                        inodeUsed[checkerNineIndirectPart2->inum]++;
                        
                        //populate the inodeUsed for test Checker 10
                        if(checkerTenFun(checkerNineIndirectPart2->inum,img_ptr)==false)
                            checkerTen =0;
                    }
                    checkerNineIndirectPart2++;
            
                }
                checkerNineIndirectPart1++;
             } 
        }
    
    
    
    
    
    
    }

    dip++;
}
    
    //Checker 6, Test to bitmap marks block in use but it is not in use.
    for(int i=sb->size-sb->nblocks;i<sb->size;i++)
        if((bitInUse(bmp,i)==true)&&(usedBlocksArray[i]==0))
        {
            fprintf( stderr,"ERROR: bitmap marks block in use but it is not in use.\n");
            exit(1);
        }


    //Checker 7
    for(int i=0;i<sb->size;i++)
        if(usedDirectArray[i]>1)
        {
            fprintf( stderr,"ERROR: direct address used more than once.\n");
            exit(1);
        }

    //Checker 8
    for(int i=0;i<sb->size;i++)
        if(usedIndirectArray[i]>1)
        {
            fprintf( stderr,"ERROR: indirect address used more than once.\n");
            exit(1);
        }
    
   
    //Checker 9
    for(int i=2;i<sb->ninodes;i++)
        if(inodeUsed[i]==0)
        {
           //fprintf(stderr,"inode used for i =%d \n",i);
            fprintf( stderr,"ERROR: inode marked use but not found in a directory.\n");
            exit(1);
        }


    //Checker 10
    if(checkerTen ==0)
    {
        fprintf( stderr,"ERROR: inode referred to in directory but marked free.\n");
        exit(1);
    }

    
    //Checker 12
    if(checkerTwelveFun(inodeUsed,sb->ninodes,img_ptr) ==false)
    {
        fprintf( stderr,"ERROR: directory appears more than once in file system.\n");
        exit(1);
    }


    //Checker 11
    if(checkerElevenFun(inodeUsed,sb->ninodes,img_ptr) ==false)
    {
        fprintf( stderr,"ERROR: bad reference count for file.\n");
        exit(1);
    }

    //checker 13
    if(checkerThirteen ==0)
    {
        fprintf( stderr,"ERROR: parent directory mismatch.\n");
        exit(1);
    }

    
    //checker 14
    if(checkerFourteen ==0)
    {
        fprintf( stderr,"ERROR: inaccessible directory exists.\n");
        exit(1);
    }
    
    
    return 0;
}
