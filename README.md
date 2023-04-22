# COMPSCI 377 Exercise: Memory Allocation

## Purpose

This exercise is designed to cover how to perform a page walk.

## Setup

Once you have logged in to Edlab, you can clone this repo using

```bash
git clone https://github.com/umass-cs-377/377-lab-pagewalk.git
```

Then you can use `cd` to open the directory you just cloned:

```bash
cd F22-377-pagewalk
```

This repository includes a `Makefile` that allows you to locally compile and run all the sample code listed in this tutorial. You can compile them by running `make`. Feel free to modify the source files yourself, after making changes you can run `make` again to build new binaries from your modified files. You can also use `make clean` to remove all the built files, this command is usually used when something went wrong during the compilation so that you can start fresh.

## Page Walk

In this assignment, you are asked to implement a simple `page walk` simulation. A `page walk` refers to the process of translating the virtual address to the physical address. 

Let's start by describing the memory we have to work with. In this example, we have a very small memory space of $2^8 = 256$ bytes (recall, a byte is 8 bits). We divide the address space into $2^2 = 4$ chunks called pages. In total, we have $\frac{2^8}{2^2} = 2^6 = 64$ pages in our memory. For this lab, we will use 3-level paging scheme. 


### Terminology 
Throughout the lab, you'll see the terms `MSB` and `LSB`. `MSB` stands for `Most Significant Bit(s)`. This is the leftmost bit(s). For example, the binary number `10000000` has an `MSB` of `1`. Similarly, `LBS` stands for `Least Significant Bit(s)`. For example, the binary number `00000001` has a `LBS` of `1`.

For the rest of the page table, we used the Terminology used in the GNU/Linx. 

##### PGD (Page Global Directory)
The PGD is the entry point for every virtual to physical translation. When a program starts, the OS assigns the process a PGD address, and stores the address of the page into the register `cr3` register on x86. Each entry in the PGD provides the staring address of the next-level called PMD.

##### PMD (Page Middle Directory)
The PMD is the second level of the indirection. This has the same structure as the PGD, but (in our example) consists of at most 4 different pages. The addresses of each of these pages is stored in the PGD entries. Each value in the PMD stores the address of the next-level PTE.

##### PTE (Page Table Entry)
The PTE is the lowest level of indirection. Each value in PTE stores a physical address of a virtual address.

### Perform a Page Walk
##### PGD Offset
Say you are given a virtual address with a binary representation `01111011`; two most significant bits (MSB) gives the offset in the PGD. To get the offset, we first need to mask out (set to 0) all but the 2 MSB by binary `AND` the address with the mask like: `01111011 & 11000000` to get `01000000`. Next, we need to bitshift the virtual address to get `00000001`, which gives the offset in the PGD. Looking at the entry, we get the address of PMD. 

##### Validating the PMD
In order to check if the PMD is valid, we need to look at the least significant bit (LSB). If the value is `0`, that means the table isn't used yet, so the program tried to access an invalid address. However, if the value is `1`, that means the PMD is valid. To get the address of the PMD, we look at the 6 MSB. Remember, indexing the memory by pages needs $2^6$ bits.


We repeat the process for the other levels for the PMD, but this time we need to get the offset from the bits `5,6` (i.e., `01111011 & 00110000`). This pattern is similar when accessing the offset in the PTE. 

```cpp
#include <stdlib.h>    //Required for malloc, free
#include <iostream>    //Required for cout
#include <bitset>      //Required for printing binary rep
#include <arpa/inet.h> //Required for htonl

#define page_entry(page, offset) \
   *(((uint8_t *)&(page)) + (offset))


#define PGD 20          // PGD is at mem 20 our `cr3`
#define VALID_MASK 0x01 // 00000001b

#define PGD_MASK  0xC0 // 11000000b
#define PMD_MASK  0x30 // 00110000b
#define PTE_MASK  0x0C // 00001100b
#define PAGE_MASK 0x03 // 00000011b

using namespace std; 

unsigned int mem[64];

void init_memory(unsigned int *mem){
   // initlize the memory. 
   mem[PGD] = htonl(0x00CD0000);
   mem[51]  = htonl(0x00000019);
   mem[6]   = htonl(0x00005900);
   mem[22]  = htonl(0xCAFEFACE);
}


int virt_to_pgd(int pfn, uint8_t vaddr){

   /**
    * Given a virtual address (XX******)b, the 2 MSB (represented by XX) gives
    * the offest in the PGD. To retrieve the offset we
    *    1) need to mask out the 6 LSB (represented by ******) to get 
    *       XX000000b. 
    *    2) bitshift the 2 MSB by 6 to get (000000XX)b
    */
   int offset = (vaddr & PGD_MASK) >> 6;

   /**
    * Now that we have the offset, we can retrieve the PMD by calling the 
    * page_entry(pfn, offset)
    */
   uint8_t pmd = page_entry(mem[pfn], offset);

   /**
    * if the LSB in PMD is 1 (i.e. *******1)b than the PMD is valid, so we
    * can return the PMD bit shifted by 2 to get the page frame number (PFN)
    * of the PMD
    */
   if ((pmd & VALID_MASK) == 1)
      return pmd >> 2;

   /**
    * otherwise we return -1
    */
   return -1;
}

int virt_to_pmd(int pfn, uint8_t vaddr){
   // TODO
   // get the offest stored at (**XX****)b
   // get the PTE stored at the offset
   // check if the PTE is valid
   // return the 6 MSB if valid
   

   return -1;
}

int virt_to_pte(int pfn, uint8_t vaddr){
   // Get the offest stored at (****XX**)b
   // get the frame number stored at the offset
   // check if the frame number is valid
   // return the 6 MSB if valid

   return -1;
}

int virt_to_phys(int pfn, uint8_t vaddr){
   // 1) Get the offest stored at (******XX)b
   // 2) Get the vlaue stores at the offset and return
   return -1; 
}

void page_walk(uint8_t vaddr){

   //do page walk

   // 1) get the PMD address
   int pmd = virt_to_pgd(PGD, vaddr);
   if (pmd == -1) {
      cout << "\tsegfault: Page Middle Directory does not exist" << endl;
      return; 
   }

   // 2) get the PTE address
   int pte = virt_to_pmd(pmd, vaddr);
   if (pte == -1) {
      cout << "\tsegfault: Page Table Entry does not exist" << endl;
      return;
   }

   // 3) get the frame number
   int phs = virt_to_pte(pte, vaddr);
   if (phs == -1) {
      cout << "\tsegfault: Invalid physical memory" << endl;
      return;
   }

   // 4) get the value at that location
   int val = virt_to_phys(phs, vaddr);

   cout << "\tPAGE WALK COMPLETED: value stored=" << val << endl;
   return; 
}

int main(void) {
   
   init_memory(mem);

   cout << "TEST: virt to phys should work" << endl;
   page_walk(0x7B); //01111011b
   
   cout << "TEST: invalid physical adrress (PTE flag = 0)" << endl;
   page_walk(0x7C); //01111100b

   cout << "TEST: invalid PTE (PMD flag = 0)" << endl;
   page_walk(0x6C); //01111100b
   
   cout << "TEST: invalid PMD (PGD flag = 0)" << endl;
   page_walk(0xB4); //10110100b
}
```


To see how Linux (used to) implement the page table walk [this article explains it well](https://www.kernel.org/doc/gorman/html/understand/understand006.html)
