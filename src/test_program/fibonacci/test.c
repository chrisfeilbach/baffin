#include <stdint.h>

// We need to write our own.
//
static int puts(const char* str) {
    // Dump this to the UART.
    volatile uint32_t* status_reg   = (volatile uint32_t*)0x4000000;
    volatile uint32_t* transmit_reg = (volatile uint32_t*)0x4000004;

    uint8_t c;
    int chars_transmitted = 0;
    while(c = *str++) {
        // Wait for the last byte to be transmitted.
	//
	while(*status_reg != 0);

	*transmit_reg = (uint32_t)c;
	chars_transmitted++;
    }

    // Transmit the newline.
    //
    while(*status_reg != 0);
    *transmit_reg = '\n';
    chars_transmitted++;

    return chars_transmitted;
}

static void int2str(char* buffer, uint32_t integer) { 
    // Clear the buffer. 
    // The largest int that can be represented in a 32-bit unsigned 
    // int is 4294967296, which is 10 chars. Then, we need an additional 
    // byte for the null terminator. 
    // 
    for(uint32_t i=0;i<11;i++) { 
        buffer[i] = 0; 
    } 
    uint32_t divisor = 1000000000; 
    uint32_t buffer_idx = 0; 
    // Figure out what largest decimal digit we need is. 
    // 
    while(divisor > integer) { 
        // Skip to the next potential digit. 
        // 
        divisor /= 10; 
    } 
 
    // The integer is now larger than the divisor, so we 
    // can start generating this string digit by digit. 
    // 
    while(divisor) { 
        buffer[buffer_idx++] = (integer / divisor) | 0x30; 
        integer = integer % divisor; 
        divisor /= 10; 
    } 
} 
 
int main(void) { 
    // Calculate the first 25 fibonacci numbers. 
    // 
    uint32_t nums[2]; 
    char     str_buffer[11]; 
    nums[0] = 1; 
    nums[1] = 1; 
    // Print out the first two guys. 
    // 
    int2str(str_buffer, nums[0]); 
    puts(str_buffer); 
    puts(str_buffer); 
    // Compute the rest. 
    // 
    for(uint32_t i=2;i<25;i++) { 
        uint32_t next = nums[0] + nums[1]; 
        nums[0] = nums[1]; 
        nums[1] = next; 
        int2str(str_buffer, nums[1]); 
        puts(str_buffer); 
    } 
    return 0; 
} 

void __attribute__((naked)) _start() {
    asm volatile("mov sp, #0x4200" :::);
    main();
    while(1);
}

void raise(void) {}
