#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include <libudev.h>
#include "alsa_utils.h"
#include "udev_utils.h"
#include "env_utils.h"

static void print_help()
{
    printf("aconnectd - ALSA sequencer auto subscription\n");
    printf("Usage:\n");
    printf(" * Connection/disconnection between two ports\n");
    printf("   aconnectd [-options] sender receiver\n");
    printf("     -h,--help         show usage\n");
    printf("     -v,--vendor       usb device vendor id\n");
    printf("     -p,--product      usb device product id\n");
    printf("     -s,--src-port     source client port (defaults to 0)\n");
    printf("     -d,--dest-port    destination client port (defaults to 0)\n");
    printf("Examples:\n");
    printf("    aconnectd -v 0f54 -p 0101 -s 0 -d 0 \"USB-MIDI\" \"Midi Through\":\n");
}

int main(int argc, char *argv[]) 
{
    int opt;

    const char *config_file = "/etc/opt/aconnectd/config.env";

    // Load environment variables from the file
    load_env_from_file(config_file);
    char *src_client_vendor = getenv("SRC_CLIENT_VENDOR");
    char *src_client_product = getenv("SRC_CLIENT_PRODUCT");
    char *src_client_name = getenv("SRC_CLIENT_NAME");
    char *_src_client_port = getenv("SRC_CLIENT_PORT");
    char *dest_client_name = getenv("DEST_CLIENT_NAME");
    char *_dest_client_port = getenv("DEST_CLIENT_PORT");

    int src_client_port = 0; 
    int dest_client_port = 0;

    if (_src_client_port != NULL) 
    {
        src_client_port = atoi(_src_client_port);
    }

    if (_dest_client_port != NULL) 
    {
        dest_client_port = atoi(_dest_client_port);
    }

    while((opt = getopt(argc, argv, "hv:p:s:d:")) != -1) 
    { 
        switch(opt) 
        { 
            case 'v': 
                src_client_vendor = strdup(optarg);
                break; 
            case 'p': 
                src_client_product = strdup(optarg);
                break; 
            case 's': 
                src_client_port = atoi(optarg);
                break;
            case 'd': 
                dest_client_port = atoi(optarg);
                break; 
            case 'h':
                print_help();
                return 0;
            default:
                return 1;
        } 
    } 

    // Ensure vendor id and product id are passed
    if (src_client_vendor == NULL || src_client_product == NULL)
    {
        fprintf(stderr, "-v and -p are mandatory options.\n");
        return 1;
    }

    if(argc - optind == 2) 
    {
        // Retrieve sender and receiver
        src_client_name = argv[optind];
        dest_client_name = argv[optind + 1];
    }

    if (src_client_name == NULL || dest_client_name == NULL)
    {
        // Ensure sender and receiver are passed
        fprintf(stderr, "both sender and receiver must be specified.\n");
        return 1;
    }

    for (;;) 
    {
        snd_seq_t *seq_handle;

        int src_client_exist = 0;
        int dest_client_exist = 0;

        fprintf(stdout, "Opening handle to ALSA sequencer.\n");
        if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) 
        {
            fprintf(stderr, "Error: Cannot open ALSA sequencer.\n");
            return 1;
        }

        int src_client_id = get_client_id_by_name(seq_handle, src_client_name);
        if (src_client_id > -1) 
        {
            printf("Found Client '%s' with ID %d.\n", src_client_name, src_client_id); 
            
            if (check_client_port(seq_handle, src_client_id, src_client_port)) 
            {
                src_client_exist = 1;
            } 
            else 
            {
                fprintf(stderr, "Error: Client '%s' with MIDI port %d does not exist.\n", src_client_name, src_client_port);
            }
        }
        else 
        {
            fprintf(stderr, "Error: Client '%s' does not exist.\n", src_client_name);
            src_client_exist = 0;
        }

        int dest_client_id = get_client_id_by_name(seq_handle, dest_client_name);
        if (dest_client_id > -1) 
        {
            printf("Found Client '%s' with ID %d.\n", dest_client_name, dest_client_id);

            if (check_client_port(seq_handle, dest_client_id, dest_client_port)) 
            {
                dest_client_exist = 1;
            }
            else 
            {
                fprintf(stderr, "Error: Client '%s' with MIDI port %d does not exist.\n", dest_client_name, dest_client_port);
            }
        }
        else 
        {
            fprintf(stderr, "Error: Client '%s' does not exist.\n", dest_client_name);
            dest_client_exist = 0;
        }

        if (src_client_exist && dest_client_exist) 
        {
            if (check_subscription(seq_handle, src_client_id, src_client_port, dest_client_id, dest_client_port)) 
            {
                printf("Subscription found: '%d:%d' connecting to '%d:%d'.\n", src_client_id, src_client_port, dest_client_id, dest_client_port);
            } 
            else 
            {
                printf("Subscription not found: '%d:%d' connecting to '%d:%d'.\n", src_client_id, src_client_port, dest_client_id, dest_client_port);
                // Create the connection
                if (!create_subscription(seq_handle, src_client_id, src_client_port, dest_client_id, dest_client_port)) 
                {
                    fprintf(stderr, "Error: Unable to create subscription: '%d:%d' connecting to '%d:%d'.\n", src_client_id, src_client_port, dest_client_id, dest_client_port);
                    return 1;
                }

                printf("Successfully routed MIDI from %d:%d to %d:%d\n", src_client_id, src_client_port, dest_client_id, dest_client_port);
            }
        }
        
        printf("Closing handle to ALSA sequencer.\n");
        if (snd_seq_close(seq_handle) < 0) 
        {
            fprintf(stderr, "Error: Cannot close ALSA sequencer.\n");
            return 1;
        }

        printf("Listening for \"add\" udev event for src client device with idVendor %s and idProduct %s...\n", src_client_vendor, src_client_product);
        listen_for_device_add_events(src_client_vendor, src_client_product);
    }
    
    return 0;
}
