

#include "../include/proc.h"
#include "../include/compare.h"

int proc_list_handle(int fd, struct cmd_packet* packet) {
   void* data;
   uint64_t num;
   uint32_t length;

   sys_proc_list(NULL, &num);

   if (num > 0) {
      length = sizeof(struct proc_list_entry) * num;
      data = pfmalloc(length);
      if (!data) {
         net_send_status(fd, CMD_DATA_NULL);
         return 1;
      }

      sys_proc_list(data, &num);

      net_send_status(fd, CMD_SUCCESS);
      net_send_data(fd, &num, sizeof(uint32_t));
      net_send_data(fd, data, length);

      free(data);

      return 0;
   }

   net_send_status(fd, CMD_DATA_NULL);
   return 1;
}

int proc_read_handle(int fd, struct cmd_packet* packet) {
   struct cmd_proc_read_packet* rp;
   void* data;
   uint64_t left;
   uint64_t address;

   rp = (struct cmd_proc_read_packet*)packet->data;

   if (rp) {
      // allocate a small buffer
      data = pfmalloc(NET_MAX_LENGTH);
      if (!data) {
         net_send_status(fd, CMD_DATA_NULL);
         return 1;
      }

      net_send_status(fd, CMD_SUCCESS);

      left = rp->length;
      address = rp->address;

      // send by chunks
      while (left > 0) {
         memset(data, NULL, NET_MAX_LENGTH);

         if (left > NET_MAX_LENGTH) {
            sys_proc_rw(rp->pid, address, data, NET_MAX_LENGTH, 0);
            net_send_data(fd, data, NET_MAX_LENGTH);

            address += NET_MAX_LENGTH;
            left -= NET_MAX_LENGTH;
         }
         else {
            sys_proc_rw(rp->pid, address, data, left, 0);
            net_send_data(fd, data, left);

            address += left;
            left -= left;
         }
      }

      free(data);

      return 0;
   }

   net_send_status(fd, CMD_DATA_NULL);

   return 1;
}

int proc_write_handle(int fd, struct cmd_packet* packet) {
   struct cmd_proc_write_packet* wp;
   void* data;
   uint64_t left;
   uint64_t address;

   wp = (struct cmd_proc_write_packet*)packet->data;

   if (wp) {
      // only allocate a small buffer
      data = pfmalloc(NET_MAX_LENGTH);
      if (!data) {
         net_send_status(fd, CMD_DATA_NULL);
         return 1;
      }

      net_send_status(fd, CMD_SUCCESS);

      left = wp->length;
      address = wp->address;

      // write in chunks
      while (left > 0) {
         if (left > NET_MAX_LENGTH) {
            net_recv_data(fd, data, NET_MAX_LENGTH, 1);
            sys_proc_rw(wp->pid, address, data, NET_MAX_LENGTH, 1);

            address += NET_MAX_LENGTH;
            left -= NET_MAX_LENGTH;
         }
         else {
            net_recv_data(fd, data, left, 1);
            sys_proc_rw(wp->pid, address, data, left, 1);

            address += left;
            left -= left;
         }
      }

      net_send_status(fd, CMD_SUCCESS);

      free(data);

      return 0;
   }

   net_send_status(fd, CMD_DATA_NULL);

   return 1;
}

int proc_maps_handle(int fd, struct cmd_packet* packet) {
   struct cmd_proc_maps_packet* mp;
   struct sys_proc_vm_map_args args;
   uint32_t size;
   uint32_t num;

   mp = (struct cmd_proc_maps_packet*)packet->data;

   if (mp) {
      memset(&args, NULL, sizeof(args));

      if (sys_proc_cmd(mp->pid, SYS_PROC_VM_MAP, &args)) {
         net_send_status(fd, CMD_ERROR);
         return 1;
      }

      size = args.num * sizeof(struct proc_vm_map_entry);

      args.maps = (struct proc_vm_map_entry*)pfmalloc(size);
      if (!args.maps) {
         net_send_status(fd, CMD_DATA_NULL);
         return 1;
      }

      if (sys_proc_cmd(mp->pid, SYS_PROC_VM_MAP, &args)) {
         free(args.maps);
         net_send_status(fd, CMD_ERROR);
         return 1;
      }

      net_send_status(fd, CMD_SUCCESS);
      num = (uint32_t)args.num;
      net_send_data(fd, &num, sizeof(uint32_t));
      net_send_data(fd, args.maps, size);

      free(args.maps);

      return 0;
   }

   net_send_status(fd, CMD_ERROR);

   return 1;
}

int proc_install_handle(int fd, struct cmd_packet* packet) {
   struct cmd_proc_install_packet* ip;
   struct sys_proc_install_args args;
   struct cmd_proc_install_response resp;

   ip = (struct cmd_proc_install_packet*)packet->data;

   if (!ip) {
      net_send_status(fd, CMD_DATA_NULL);
      return 1;
   }

   args.stubentryaddr = NULL;
   sys_proc_cmd(ip->pid, SYS_PROC_INSTALL, &args);

   if (!args.stubentryaddr) {
      net_send_status(fd, CMD_DATA_NULL);
      return 1;
   }

   resp.rpcstub = args.stubentryaddr;

   net_send_status(fd, CMD_SUCCESS);
   net_send_data(fd, &resp, CMD_PROC_INSTALL_RESPONSE_SIZE);

   return 0;
}

int proc_call_handle(int fd, struct cmd_packet* packet) {
   struct cmd_proc_call_packet* cp;
   struct sys_proc_call_args args;
   struct cmd_proc_call_response resp;

   cp = (struct cmd_proc_call_packet*)packet->data;

   if (!cp) {
      net_send_status(fd, CMD_DATA_NULL);
      return 1;
   }

   // copy over the arguments for the call
   args.pid = cp->pid;
   args.rpcstub = cp->rpcstub;
   args.rax = NULL;
   args.rip = cp->rpc_rip;
   args.rdi = cp->rpc_rdi;
   args.rsi = cp->rpc_rsi;
   args.rdx = cp->rpc_rdx;
   args.rcx = cp->rpc_rcx;
   args.r8 = cp->rpc_r8;
   args.r9 = cp->rpc_r9;

   sys_proc_cmd(cp->pid, SYS_PROC_CALL, &args);

   resp.pid = cp->pid;
   resp.rpc_rax = args.rax;

   net_send_status(fd, CMD_SUCCESS);
   net_send_data(fd, &resp, CMD_PROC_CALL_RESPONSE_SIZE);

   return 0;
}

int proc_elf_handle(int fd, struct cmd_packet* packet) {
   struct cmd_proc_elf_packet* ep;
   struct sys_proc_elf_args args;
   void* elf;

   ep = (struct cmd_proc_elf_packet*)packet->data;

   if (ep) {
      elf = pfmalloc(ep->length);
      if (!elf) {
         net_send_status(fd, CMD_DATA_NULL);
         return 1;
      }

      net_send_status(fd, CMD_SUCCESS);

      net_recv_data(fd, elf, ep->length, 1);

      args.elf = elf;

      if (sys_proc_cmd(ep->pid, SYS_PROC_ELF, &args)) {
         free(elf);
         net_send_status(fd, CMD_ERROR);
         return 1;
      }

      free(elf);

      net_send_status(fd, CMD_SUCCESS);

      return 0;
   }

   net_send_status(fd, CMD_ERROR);

   return 1;
}

int proc_protect_handle(int fd, struct cmd_packet* packet) {
   struct cmd_proc_protect_packet* pp;
   struct sys_proc_protect_args args;

   pp = (struct cmd_proc_protect_packet*)packet->data;

   if (pp) {
      args.address = pp->address;
      args.length = pp->length;
      args.prot = pp->newprot;
      sys_proc_cmd(pp->pid, SYS_PROC_PROTECT, &args);

      net_send_status(fd, CMD_SUCCESS);
   }

   net_send_status(fd, CMD_DATA_NULL);

   return 0;
}

// Function will depending on the type set for the value to use in scanning
// return the related variable size: 
// basically the value of sizeof(<value type>)
size_t GetSizeOfProcScanValue(enum cmd_proc_scan_valuetype valType) {
   switch (valType) {
      // In case variable type is signed/unsigned 8bit integer
      case SCAN_TYPE_U8:
      case SCAN_TYPE_I8:
         return 1;

         // In case variable type is signed/unsigned 16bit integer
      case SCAN_TYPE_U16:
      case SCAN_TYPE_I16:
         return 2;

         // In case variable type is signed/unsigned 32bit integer or floating point
      case SCAN_TYPE_U32:
      case SCAN_TYPE_I32:
      case SCAN_TYPE_FLOAT:
         return 4;

         // In case variable type is signed/unsigned 64bit integer or double
      case SCAN_TYPE_U64:
      case SCAN_TYPE_I64:
      case SCAN_TYPE_DOUBLE:
         return 8;

         // In case variable type is byte array, string, or none of the above cases
      case SCAN_TYPE_BYTE_ARRAY:
      case SCAN_TYPE_STRING:
      default: return 0;
   };
}

int CompareProcScanValues(enum cmd_proc_scan_comparetype cmpType, enum cmd_proc_scan_valuetype valType, size_t valTypeLength, BYTE* pScanValue, BYTE* pMemoryValue, BYTE* pExtraValue) {
   switch (cmpType) {
      case SCAN_CMP_EXACT_MATCH:          return compare_value_exact(pScanValue, pMemoryValue, valTypeLength);
      case SCAN_CMP_FUZZY:          return compare_value_fuzzy(valType, pScanValue, pMemoryValue);
      case SCAN_CMP_GREATER_THAN:          return compare_value_bigger_than(valType, pScanValue, pMemoryValue);
      case SCAN_CMP_SMALLER_THAN:         return compare_value_smaller_than(valType, pScanValue, pMemoryValue);
      case SCAN_CMP_RANGE_BETWEEN:        return compare_value_between(valType, pScanValue, pMemoryValue, pExtraValue);
      case SCAN_CMP_VALUE_INCREASED:      return compare_value_increased(valType, pScanValue, pMemoryValue, pExtraValue);
      case SCAM_CMP_INCREASED_BY:    return compare_value_increased_by(valType, pScanValue, pMemoryValue, pExtraValue);
      case SCAM_CMP_VALUE_DECREASED:      return compare_value_decreased(valType, pScanValue, pMemoryValue, pExtraValue);
      case SCAM_CMP_DECREASED_BY:    return compare_value_decreased_by(valType, pScanValue, pMemoryValue, pExtraValue);
      case SCAM_CMP_VALUE_CHANGED:        return compare_value_changed(valType, pScanValue, pMemoryValue, pExtraValue);
      case SCAM_CMP_VALUE_UNCHANGED:      return compare_value_unchanged(valType, pScanValue, pMemoryValue, pExtraValue);
      case SCAM_CMP_UNKNOWN_VALUE: return TRUE;
   };

   return FALSE;
}

// Structure type definitions
typedef struct cmd_proc_scan_packet PROC_SCAN_PKT_T, * PPROC_SCAN_PKT;

// Variable type definitions
typedef unsigned char* PBYTE;

// The Default maximum number of addresses 10000
#define INIT_MAX_ADDR_COUNT 10000

// A Helper function for the Console based scanning function which is
// used to ensure that no unused and unessecary memory is allocated 
// for the list of valid addresses. Basically this function is used to
// ensure that we dont hold any memory we dont use (not 2 be wasteful).
int release_unused_addr_list_memory(uint64_t** ptrAddrList, size_t addrCount, size_t totalSz) {
   // Calculate the minimum memory size needed for our address list
   // buffer to be able to contain <addrCount> number of uint64_t 
   // address values
   size_t min_mem_size = addrCount * sizeof(uint64_t);

   // Attempt to resize the address list, using the calculated size
   // and check if pfrealloc returned error, and handle it
   uint64_t* new_resized_mem = (uint64_t*)pfrealloc(
      *ptrAddrList, // pointer to memory of our <valid_addresses>
      min_mem_size, // minimum needed size
      totalSz       // current size of the allocated memory
   );
   if (new_resized_mem == NULL)
      return -1;

   // If pfrealloc doesn't fail, assign the new resized memory to
   // the <ptrAddrList> variable, and return success code
   *ptrAddrList = new_resized_mem;
   return 1;
}

// Helper function for the console based scan, which will resize the
// buffer containing the addresses using the initial size  
int ResizeTheAddrListBuffer(uint64_t** pAddressMem, size_t* pCurrentSz) {
   // Check if the provided buffer is invalid, and handle it
   if (*pAddressMem == NULL) return -1;

   // Calculate the new memory size first calculating the size used when 
   // initially allocating array of uint64_t addresses
   size_t new_size = *pCurrentSz + (INIT_MAX_ADDR_COUNT * sizeof(uint64_t));

   // Resize the memory, and return error code if pfrealloc fails
   uint64_t* new_resized_mem = (uint64_t*)pfrealloc(*pAddressMem, new_size, *pCurrentSz);
   if (new_resized_mem == NULL)
      return -1;

   // Otherwise, we assign the pointer to the expanded memory to the
   // address collection buffer, update the current size variable and 
   // return success code
   *pAddressMem = new_resized_mem;
   *pCurrentSz = new_size;
   return 1;
}

// Sends scan results over network connection to PS4Cheater/Reaper
// @param fd Network socket descriptor
// @param addresses Array of matched addresses
// @param count Number of valid addresses
void Send_Process_Scan_Results(int ps4cheater_fd, uint64_t *addresses, size_t count) {
   for (size_t i = 0; i < count; i++) {
       net_send_data(
         ps4cheater_fd, 
         &addresses[i], 
         sizeof(uint64_t)
      );
   }
   uint64_t endflag = 0xFFFFFFFFFFFFFFFF;
   net_send_data(ps4cheater_fd, &endflag, sizeof(uint64_t));
}

int Retrieve_Process_Memory_Map(int pid, struct sys_proc_vm_map_args* vm_map) {
   memset(vm_map, 0, sizeof(*vm_map));
   // Query for the process ID's memory map
   if (sys_proc_cmd(pid, SYS_PROC_VM_MAP, vm_map))
      return 1;

   // Calculate the size of the memory map
   size_t size = vm_map->num * sizeof(struct proc_vm_map_entry);
   vm_map->maps = (struct proc_vm_map_entry*)pfmalloc(size);
   if (!vm_map->maps)
      return 1;

   return sys_proc_cmd(pid, SYS_PROC_VM_MAP, vm_map);
}

// This replaces the previous version! It's console scan recreation
int proc_scan_handle(int fd, struct cmd_packet* packet) {
   struct cmd_proc_scan_packet* sp;
   size_t scanValSize;
   PBYTE data;

   // Extract the data from the RPC packet, and check if the data 
   // pointer isn't valid, and return early if so
   if (!(sp = (struct cmd_proc_scan_packet*)packet->data)) {
      // Send status indicating null data
      net_send_status(fd, CMD_DATA_NULL);
      return 1;
   }

   // Calculate the length of the value to be scanned
   if (!(scanValSize = GetSizeOfProcScanValue(sp->valueType)))
      scanValSize = sp->lenData;


   // Allocate memory to store received data
   if ((data = (PBYTE)pfmalloc(sp->lenData)) == NULL) {
      net_send_status(fd, CMD_DATA_NULL);
      return 1;
   }

   // Notify successful data reception
   net_send_status(fd, CMD_SUCCESS);

   // Receive data from the network
   net_recv_data(fd, data, sp->lenData, 1);
   
   // Memory map retrieval
   struct sys_proc_vm_map_args args;
   if (Retrieve_Process_Memory_Map(sp->pid, &args)) {
      free(data);
      net_send_status(fd, CMD_ERROR);
      return 1;
   }

   // Retrieve the process memory map
   if (sys_proc_cmd(sp->pid, SYS_PROC_VM_MAP, &args)) {
      free(args.maps);
      free(data);
      net_send_status(fd, CMD_ERROR);
      return 1;
   }

   // Notify successful memory map retrieval
   net_send_status(fd, CMD_SUCCESS);

   // Initialize variables for scanning
   unsigned char* pExtraValue = scanValSize == sp->lenData ? NULL : &data[scanValSize];
   unsigned char* scanBuffer = (unsigned char*)pfmalloc(PAGE_SIZE);
   if (scanBuffer == NULL) {
      free(args.maps);
      free(data);
      net_send_status(fd, CMD_DATA_NULL);
      return 1;
   }

   // Counter used to increase the <valid_addresses> current array index
   // everytime a valid address is found
   size_t addressCount = 0;
   size_t init_memSize = INIT_MAX_ADDR_COUNT * sizeof(uint64_t);
   // Allocate memory to hold <INIT_MAX_ADDR_COUNT> number of offsets
   uint64_t* valid_addresses = (uint64_t*)pfmalloc(init_memSize);
   if (valid_addresses == NULL) {
      free(scanBuffer);
      free(args.maps);
      free(data);
      net_send_status(fd, CMD_DATA_NULL);
      return 1;
   }

   uprintf("scan start");
   // Loop through each memory section of the process
   for (size_t i = 0; i < args.num; i++) {
      // Skip sections that cannot be read
      if ((args.maps[i].prot & PROT_READ) != PROT_READ)
         continue;

      // Calculate section start address and length
      uint64_t sectionStartAddr = args.maps[i].start;
      size_t sectionLen = args.maps[i].end - sectionStartAddr;

      // Iterate through the memory section
      for (uint64_t j = 0; j < sectionLen; j += scanValSize) {
         // If the current offset is at a page boundary, read the next page
         if (j == 0 || !(j % PAGE_SIZE)) {
            sys_proc_rw(
               sp->pid,
               sectionStartAddr,
               scanBuffer,
               PAGE_SIZE,
               FALSE
            );
         }

         // Calculate the scan offset and current address
         uint64_t scanOffset = j % PAGE_SIZE;
         uint64_t curAddress = sectionStartAddr + j;

         // Run Comparison on the found value
         if (CompareProcScanValues(
            sp->compareType,
            sp->valueType,
            scanValSize,
            data,
            scanBuffer + scanOffset,
            pExtraValue)) {

            // First before appending the new address to the address collection
            // we check if the current index in <valid_addresses> is equal to or
            // will be equal to/greater than <INIT_MAX_ADDR_COUNT>, and if true
            // we try to resize <valid_addresses> to be able to hold 
            // <INIT_MAX_ADDR_COUNT> additional addresses 
            if (addressCount + 1 >= INIT_MAX_ADDR_COUNT || addressCount == INIT_MAX_ADDR_COUNT) {
               // Attempt to resize <valid_addresses> and check if the helper
               // function returns error code, and if true, handle it
               if (ResizeTheAddrListBuffer(&valid_addresses, &init_memSize) == -1) {
                  free(scanBuffer);
                  free(args.maps);
                  free(data);
                  free(valid_addresses);
                  net_send_status(fd, CMD_DATA_NULL);
                  return 1;
               }
            }

            // Append the new found address to the <valid_addresses> then
            // we increment <addressCount> by 1 afterwards
            valid_addresses[addressCount++] = curAddress;
         }
      }
   }

   // Notify the end of the scanning process
   uprintf("scan done");
   Send_Process_Scan_Results(fd, valid_addresses, addressCount);

   // Free allocated memory
   free(scanBuffer);
   free(valid_addresses);
   free(args.maps);
   free(data);

   return 0;
}


int proc_info_handle(int fd, struct cmd_packet* packet) {
   struct cmd_proc_info_packet* ip;
   struct sys_proc_info_args args;
   struct cmd_proc_info_response resp;

   ip = (struct cmd_proc_info_packet*)packet->data;

   if (ip) {
      sys_proc_cmd(ip->pid, SYS_PROC_INFO, &args);

      resp.pid = args.pid;
      memcpy(resp.name, args.name, sizeof(resp.name));
      memcpy(resp.path, args.path, sizeof(resp.path));
      memcpy(resp.titleid, args.titleid, sizeof(resp.titleid));
      memcpy(resp.contentid, args.contentid, sizeof(resp.contentid));

      net_send_status(fd, CMD_SUCCESS);
      net_send_data(fd, &resp, CMD_PROC_INFO_RESPONSE_SIZE);
      return 0;
   }

   net_send_status(fd, CMD_DATA_NULL);

   return 0;
}

int proc_alloc_handle(int fd, struct cmd_packet* packet) {
   struct cmd_proc_alloc_packet* ap;
   struct sys_proc_alloc_args args;
   struct cmd_proc_alloc_response resp;

   ap = (struct cmd_proc_alloc_packet*)packet->data;

   if (ap) {
      args.length = ap->length;
      sys_proc_cmd(ap->pid, SYS_PROC_ALLOC, &args);

      resp.address = args.address;

      net_send_status(fd, CMD_SUCCESS);
      net_send_data(fd, &resp, CMD_PROC_ALLOC_RESPONSE_SIZE);
      return 0;
   }

   net_send_status(fd, CMD_DATA_NULL);

   return 0;
}

int proc_free_handle(int fd, struct cmd_packet* packet) {
   struct cmd_proc_free_packet* fp;
   struct sys_proc_free_args args;

   fp = (struct cmd_proc_free_packet*)packet->data;

   if (fp) {
      args.address = fp->address;
      args.length = fp->length;
      sys_proc_cmd(fp->pid, SYS_PROC_FREE, &args);

      net_send_status(fd, CMD_SUCCESS);
      return 0;
   }

   net_send_status(fd, CMD_DATA_NULL);

   return 0;
}

int proc_handle(int fd, struct cmd_packet* packet) {
   switch (packet->cmd) {
      case CMD_PROC_LIST:         return proc_list_handle(fd, packet);
      case CMD_PROC_READ:         return proc_read_handle(fd, packet);
      case CMD_PROC_WRITE:        return proc_write_handle(fd, packet);
      case CMD_PROC_MAPS:         return proc_maps_handle(fd, packet);
      case CMD_PROC_INTALL:       return proc_install_handle(fd, packet);
      case CMD_PROC_CALL:         return proc_call_handle(fd, packet);
      case CMD_PROC_ELF:          return proc_elf_handle(fd, packet);
      case CMD_PROC_PROTECT:      return proc_protect_handle(fd, packet);
      case CMD_PROC_SCAN:         return proc_scan_handle(fd, packet);
      case CMD_PROC_INFO:         return proc_info_handle(fd, packet);
      case CMD_PROC_ALLOC:        return proc_alloc_handle(fd, packet);
      case CMD_PROC_FREE:         return proc_free_handle(fd, packet);
      default: return -1;
   };
}
