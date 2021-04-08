#include "testbench.h"
#include <systemc>

Testbench::Testbench(sc_module_name n)
    : sc_module(n), initiator("initiator"){
    
    SC_THREAD(transfer_data);
}

int Testbench::read_bmp(){
    FILE* fp_s = fopen("input.bmp", "rb");
    if (fp_s == nullptr) {
        std::cerr << "fopen fp_s error" << std::endl;
        return -1;
    }

    fseek(fp_s, 10, SEEK_SET);
    assert(fread(&rgb_raw_data_offset, sizeof(unsigned int), 1, fp_s));

    fseek(fp_s, 18, SEEK_SET);
    assert(fread(&width, sizeof(unsigned int), 1, fp_s));
    assert(fread(&height, sizeof(unsigned int), 1, fp_s));

    fseek(fp_s, 28, SEEK_SET);
    assert(fread(&bit_per_pixel, sizeof(unsigned short), 1, fp_s));
    byte_per_pixel = bit_per_pixel / 8;

    fseek(fp_s, rgb_raw_data_offset, SEEK_SET);

    size_t size = width * height * byte_per_pixel;
    image_s = reinterpret_cast<unsigned char *>(new void *[size]);
    if (image_s == nullptr) {
        std::cerr << "allocate image_s error" << std::endl;
        return -1;
    }

    image_t = reinterpret_cast<unsigned char *>(new void *[size]);
    if (image_t == nullptr) {
        std::cerr << "allocate image_t error" << std::endl;
        return -1;
    }

    assert(fread(image_s, sizeof(unsigned char),
                (size_t)(long)width * height * byte_per_pixel, fp_s));
    fclose(fp_s);

    return 0;
}
int Testbench::write_bmp(){
    unsigned int file_size = 0; // file size
    FILE* fp_t = fopen("output.bmp", "wb");
    if (fp_t == nullptr) {
        std::cerr << "fopen fname_t error" << std::endl;
        return -1;
    }
    file_size = width * height * byte_per_pixel + rgb_raw_data_offset;
    header[2] = (unsigned char)(file_size & 0x000000ff);
    header[3] = (file_size >> 8) & 0x000000ff;
    header[4] = (file_size >> 16) & 0x000000ff;
    header[5] = (file_size >> 24) & 0x000000ff;
    header[18] = width & 0x000000ff;
    header[19] = (width >> 8) & 0x000000ff;
    header[20] = (width >> 16) & 0x000000ff;
    header[21] = (width >> 24) & 0x000000ff;
    header[22] = height & 0x000000ff;
    header[23] = (height >> 8) & 0x000000ff;
    header[24] = (height >> 16) & 0x000000ff;
    header[25] = (height >> 24) & 0x000000ff;
    header[28] = bit_per_pixel;
    fwrite(header, sizeof(unsigned char), rgb_raw_data_offset, fp_t);
    fwrite(image_t, sizeof(unsigned char),
            (size_t)(long)width * height * byte_per_pixel, fp_t);

    fclose(fp_t);

    return 0;
}
void Testbench::transfer_data(){
    int offset = 0;
    int i, j, x, y;
    int filterHeight = 3, filterWidth = 3;
    int cnt = 0;
    unsigned char R, G, B;

    wait(5 * CLOCK_PERIOD, SC_NS);
    for (y = 0; y != height; ++y) {
        for (x = 0; x != width; ++x) {
            for (i=-1 ; i<filterHeight-1 ; ++i) {
                for (j=-1 ; j<filterWidth-1 ; ++j) {
                    if(0<=y+i && y+i<height && 0<=x+j && x+j<width) {
                        R = (*(image_s + byte_per_pixel * (width * (y+i) + x + j + offset) + 2));
						G = (*(image_s + byte_per_pixel * (width * (y+i) + x + j + offset) + 1));
						B = (*(image_s + byte_per_pixel * (width * (y+i) + x + j + offset) + 0));
                    }
					else{
 						R = (0);
						G = (0);
						B = (0);
					}
                    unsigned char buffer[4];
                    unsigned char mask[4];
                    // Prepare data
                    buffer[0] = R;
                    buffer[1] = G;
                    buffer[2] = B;
                    mask[0] = 0xff;
                    mask[1] = 0xff;
                    mask[2] = 0xff;
                    mask[3] = 0;
                    // send data by socket
                    //cout << "write :" << (int)buffer[0] << endl;
                    initiator.write_to_socket(FILTER_MM_BASE + FILTER_R_ADDR, mask, buffer, 4);
          
                    cnt += 3;
                }
            }
            
            unsigned char buffer[4];
            unsigned char mask[4];
            initiator.read_from_socket(FILTER_MM_BASE + FILTER_RESULT_ADDR, mask, buffer, 4);
            
			*(image_t + byte_per_pixel * (width * y + x) + 2) = buffer[0];
			*(image_t + byte_per_pixel * (width * y + x) + 1) = buffer[1];
			*(image_t + byte_per_pixel * (width * y + x) + 0) = buffer[2];
        }
    }

    cout << "total sent: " << cnt << " pixels" << endl;
    sc_stop();
}
