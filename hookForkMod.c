#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
MODULE_LICENSE("GPL");

const char* FILEPATH="/proc/version";
char MAPPATH[128] ="/boot/System.map-";

struct myfile{
struct file* f; 
mm_segment_t fs; 
loff_t pos;
};



struct myfile* open_file (const char *filePath) 
{
    struct myfile *mfptr;
    mfptr= kmalloc(sizeof(struct myfile), GFP_KERNEL);
    mfptr->fs = get_fs();
    set_fs(get_ds());
   mfptr-> f = filp_open(filePath,0, 0);
    set_fs(mfptr->fs);
    return mfptr;
}


int read_from_file_until(struct myfile *mfptr, unsigned char* buf,unsigned long  vlen, char c) 
{
   int ret;
   mfptr-> fs = get_fs();
   set_fs(get_ds());
int i;    
    for (i=0;i<vlen;i++){
   	 ret = vfs_read(mfptr->f, buf+i ,1 , &(mfptr->pos));
	 if (buf[i]==c){
	 buf[i]=NULL;
	 break;
	 }
	}
    set_fs(mfptr->fs);
    return ret;
}   

void close_file(struct myfile *myfileptr) 
{
    filp_close(myfileptr->f, NULL);
}

int init_module(void){
struct myfile *mfptr;

mfptr= kmalloc(sizeof(struct myfile), GFP_KERNEL);
mfptr= open_file(FILEPATH);

	
	
	char* rbuffer;
	rbuffer= kmalloc(sizeof(char),GFP_KERNEL);
	mfptr->pos=0;
	read_from_file_until(mfptr,rbuffer,1024, ' ');
	read_from_file_until(mfptr,rbuffer,1024, ' ');
	read_from_file_until(mfptr,rbuffer,1024, ' ');
	printk(KERN_ALERT "Kernel_Version: %s\n", rbuffer);
	close_file(mfptr);
	
	
	strncat(MAPPATH, rbuffer, 20);
	printk(KERN_ALERT "file: %s\n", MAPPATH);
	mfptr=open_file(MAPPATH);
	
		
		char* rbuffer2;
		rbuffer2= kmalloc(sizeof(char),GFP_KERNEL);
		mfptr->pos=0;
		int i; 
		while (true){
		
		read_from_file_until(mfptr,rbuffer2,1024, '\n');
		if (strstr (rbuffer2, "sys_call_table")!= NULL)
			break;	
		}
		char Address [17];
		strncpy(Address,rbuffer2,16);
		Address[16] ='\0';
		printk(KERN_ALERT "FirstLineinsysmap: %s\n", rbuffer2);
		printk(KERN_ALERT "FirstLineAddress: %s\n", Address);	
		sys_call_ptr_t* sysCall;
		unsigned long holder; 
		sscanf(Address,"%lx", &holder);
		sysCall= (sys_call_ptr_t *) holder;
		printk(KERN_ALERT " Fork Address: %px \n", sysCall[2]);

kfree(rbuffer2);
kfree(mfptr);
kfree(rbuffer);	
return 0; 
}

void cleanup_module(void){
	printk(KERN_INFO " Tamam keda\n");
}



