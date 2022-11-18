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

#define PGD_SHIFT 6
#define PMD_SHIFT 4
#define PTE_SHIFT 2

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
   int offset = (vaddr & PGD_MASK) >> PGD_SHIFT;

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
   // TODO
   // Get the offest stored at (****XX**)b
   // get the frame number stored at the offset
   // check if the frame number is valid
   // return the 6 MSB if valid

   return -1;
}

int virt_to_phys(int pfn, uint8_t vaddr){
   // TODO
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
