// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  char name[10]; /* kmem----(no less than 4) */
} kmem[NCPU];

void
kinit()
{
  int i;
  for(i=0;i<NCPU;i++){
	/* 分配名字 */
  	snprintf(kmem[i].name, sizeof(kmem[i].name), "kmem_%d", i);
	initlock(&kmem[i].lock, kmem[i].name);
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  push_off(); /* turn interrupts on */
  int cpu_id = cpuid();

  acquire(&kmem[cpu_id].lock);
  r->next = kmem[cpu_id].freelist;
  kmem[cpu_id].freelist = r;
  release(&kmem[cpu_id].lock);

  pop_off(); /* recover */
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *now;
  push_off();
  int cpu_id = cpuid();

  acquire(&kmem[cpu_id].lock);
  now = kmem[cpu_id].freelist;
  if(now){
    /* has free-list */
    kmem[cpu_id].freelist = now->next;
  }else{
    /* steal from other cpus */
	  int success = 0;
	  int i;
	  for(i=0;i<NCPU;i++){
		  if(i==cpu_id)continue;
		  acquire(&kmem[i].lock);
		  struct run *p = kmem[i].freelist;
		  if(p){
			  success = 1;
			  /* 存在空闲链表，可以偷 */
			  struct run *seeker = p;
			  struct run *pre = p;
			  /* 定位到中间
			   * fp 为底部
			   * 拆分下来：pre 和 p 中间
			   * */
			  while(seeker && seeker->next){
				  seeker = seeker->next->next;
				  pre = p;
				  p = p->next;
			  }
			  kmem[cpu_id].freelist = kmem[i].freelist;
			  if(p==kmem[i].freelist){
				  /* 说明：没有进 while: p->next 为空；
				   * 说明：被偷的只有一个；
				   * 此时 p 和 pre 还是一样的
				   * */
				kmem[i].freelist = 0;
			  }else{
			  	kmem[i].freelist = p;
				pre->next = 0;
			  }
		  }
		  release(&kmem[i].lock);
		  if(success){
		  	now = kmem[cpu_id].freelist;
			kmem[cpu_id].freelist = now->next;
			break;
		  }
	  }
  }
    release(&kmem[cpu_id].lock);/* release the lock --> r should be free */
    pop_off();
  if(now)
    memset((char*)now, 5, PGSIZE); // fill with junk
  return (void*)now;
}
