# PS4Debug (A PS4 Debugging Payload)
My custom fork of PS4Debug includes my own tweaks and modifications, and allows building a single PS4Debug payload binary compatible with multiple PS4 OFWs. This eliminates the need to maintain five or more separate projects for different OFWs. I am the sole maintainer of this version and will be responsible for adding new features, implementing code improvements, fixing issues, and more.

## News
### Recreating ctn123's Console Scan Feature
<details>
<summary>See Explaination + Code</summary>

Developer [ctn123](https://github.com/ctn123) described the console scan feature as essentially "Read memory, compare bytes, rinse, repeat," and I assume that once all process memory sections have been scanned, data is sent back. Based on this description, I've outlined a plan to replicate this functionality:

1. **Memory Comparison:** Allocate a buffer using the `pfmalloc` (malloc with prefaulting) function to hold a maximum of 10,000 (uint64_t-based) memory addresses. If the comparing function `CompareProcScanValues` succeeds, append the memory address to the buffer. Repeat this process until all memory sections of the process have been scanned.

2. **Data Transmission:** Once all memory sections have been scanned, loop through the array containing the uint64_t-based address values. Send back the addresses one by one using the `net_send_data` function until all valid addresses have been transmitted.

3. **End Flag:** Send back an end flag to mark the completion of data transmission.

Check out [debugger/source/proc.c](https://github.com/a0zhar/PS4DebugV2/blob/9022062adf644a9f63bd490e5db00e96f3dedc3a/debugger/source/proc.c#L352) to view the current implementation of this feature.

</details>

<details>
<summary>See Usage Instructions</summary>

You need to send the value of **0xBDAA000D** to the PS4Debug server if you want it to use the `proc_console_scan_handle` function instead of `proc_scan_handle`
</details>



## Credits
- ctn123 - For explaining the logic behind his Console Scan Feature in Discord DM's. 
- DeathRGH - For his frame4 Repo (A Multi-Firmware Compatible Version of PS4Debug)
- GiantPluto - For the initial 6.72 Compatible version of this repo.
- jogolden
