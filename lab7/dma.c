#define INPUT_COMMAND   0x42
#define OUTPUT_COMMAND  0x41
#define OUTPUT_8B       0xC0
#define INPUT_8B        0xC8
#define B_MODE          0x00

void init_dspdma(void)
{

    unsigned int samp_rate = 11025;

    unsigned char HB = ((samp_rate&0xff00) >> 8);
    unsigned char LB = (samp_rate&0x00ff);

    //set dsp transfer sampling rate
    write_dsp(INPUT_COMMAND);
    write_dsp(HB);
    write_dsp(LB);

    //send I/O command followed by the transfer mode, and the data transfer count
    unsigned int wlength = (1024*64)-1;
    unsigned char WL_HB = ((wLength & 0xff00) >> 8);
    unsigned char WL_LB = (wlength & 0x00ff);
    write_dsp(INPUT_8B);
    write_dsp(B_MODE);
    write_dsp(WL_LB);
    write_dsp(WL_HB);

}

ssize_t sb16drv_write(...)
{
    unsigned int amount_allocted;
    unsigned int amount_returned;
    unsigned int max;

    max = 64*1024;

    amount_returned = copy_from_user(dma_buffer, buf, count);
    amount_allocated = count - amount_returned;
    
    *f_pos += amount_allocated

    if(*f_pos == max) { //buffer full
        start_play();
    } else if (f_pos > max) {
        return -ESPC //whatever that weird constant was from onebyte_write
    }

    return amount;  //returns the amount allocated
}
