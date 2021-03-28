#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include <linux/kallsyms.h>
MODULE_LICENSE("GPL");

const char* FILEPATH="/proc/version";
char MAPPATH[128] ="/boot/System.map-";

typedef asmlinkage long (*system_call_ptr_t)(unsigned long p1, void* p2, int* p3,int* p4, unsigned long p5);
static sys_call_ptr_t clone_add;
int countery = 0;
sys_call_ptr_t* sysCall;


static asmlinkage long forkyy(unsigned long p1, void* p2, int* p3,int* p4, unsigned long p5)
{
    
    countery=countery+1;
   printk("The fork called: %d \n", countery);

long trial;
//return 
	trial= clone_add(p1, p2, p3, p4, p5);
return trial;
}

struct myfile{
struct file* f; 
mm_segment_t fs; 
loff_t pos;
};


/*
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
*/
/*
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
} */  
/*
void close_file(struct myfile *myfileptr) 
{
    filp_close(myfileptr->f, NULL);
}
*/

inline void writerFunc(unsigned long holder2)
{
    asm volatile("mov %0,%%cr0": "+r" (holder2), "+m" (__force_order));
}

static void disableWrite(void)
{
  unsigned long reg_cr0 = read_cr0();
  set_bit(16, &reg_cr0);
  writerFunc(reg_cr0);
}

static void enableWrite(void)
{
  unsigned long reg_cr0 = read_cr0();
  clear_bit(16, &reg_cr0);
  writerFunc(reg_cr0);
} 


int init_module(void){

		sysCall= kallsyms_lookup_name("sys_call_table");
		printk(KERN_ALERT " Fork Address: %px \n", sysCall[2]);
		clone_add = sysCall[__NR_clone];
    	enableWrite();
    	sysCall[__NR_clone] = (sys_call_ptr_t)forkyy;
		disableWrite();

return 0; 
}

void cleanup_module(void){
	enableWrite();
    sysCall[__NR_clone] = clone_add;
disableWrite();
	printk(KERN_INFO " Tamam keda\n");
}



