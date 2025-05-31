#include <stdio.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include "alsa_utils.h"

//https://github.com/bear24rw/alsa-utils/blob/master/seq/aconnect/aconnect.c

int get_client_id_by_name(snd_seq_t *seq_handle, const char *client_name) 
{
    snd_seq_client_info_t *client_info;
    snd_seq_client_info_alloca(&client_info);

    snd_seq_client_info_set_client(client_info, -1); // Start from the first client
    while (snd_seq_query_next_client(seq_handle, client_info) >= 0) 
    {
        if (strcmp(snd_seq_client_info_get_name(client_info), client_name) == 0) 
        {
            return snd_seq_client_info_get_client(client_info); // Return the client ID
        }
    }
    return -1; // Client not found
}

int check_client_port(snd_seq_t *seq_handle, int client_id, int port) 
{
    snd_seq_port_info_t *port_info;
    snd_seq_port_info_alloca(&port_info);

    // Iterate over all ports of the client
    snd_seq_port_info_set_client(port_info, client_id);
    snd_seq_port_info_set_port(port_info, -1);

    while (snd_seq_query_next_port(seq_handle, port_info) >= 0) 
    {
        int found_port = snd_seq_port_info_get_port(port_info);
        fprintf(stderr, "Found Client ID:port %d:%d.\n", client_id, found_port);
        if (found_port == port) 
        {
            return 1; // Client and port exist
        }
    }

    return 0; // Client or port does not exist
}

int check_subscription(snd_seq_t *seq_handle, int src_client_id, int src_client_port, int dest_client_id, int dest_client_port) 
{
    // Check if the port is connected to the destination client and port
    snd_seq_addr_t src_addr = {src_client_id, src_client_port};
    const snd_seq_addr_t *tmp_addr;

    snd_seq_query_subscribe_t *subs;
    snd_seq_query_subscribe_alloca(&subs);
    snd_seq_query_subscribe_set_root(subs, &src_addr);
    snd_seq_query_subscribe_set_type(subs, SND_SEQ_QUERY_SUBS_READ);
    snd_seq_query_subscribe_set_index(subs, 0);

    while (snd_seq_query_port_subscribers(seq_handle, subs) >= 0) 
    {
        tmp_addr = snd_seq_query_subscribe_get_addr(subs);
        fprintf(stderr, "Found Subscriber %d:%d Connect To %d:%d.\n", src_addr.client, src_addr.port, tmp_addr->client, tmp_addr->port);

        if (tmp_addr->client == dest_client_id && tmp_addr->port == dest_client_port) 
        {
            return 1; // Connection exists
        }

        snd_seq_query_subscribe_set_index(subs, snd_seq_query_subscribe_get_index(subs) + 1);
    }

    return 0; // Connection does not exist
}

int create_subscription(snd_seq_t *seq_handle, int src_client_id, int src_client_port, int dest_client_id, int dest_client_port) 
{
    // Create a port subscription
    snd_seq_port_subscribe_t *subscription;
    snd_seq_port_subscribe_alloca(&subscription);

    // Set the source (sender) client and port
    snd_seq_addr_t sender;
    sender.client = src_client_id;
    sender.port = src_client_port;
    snd_seq_port_subscribe_set_sender(subscription, &sender);

    // Set the destination (receiver) client and port
    snd_seq_addr_t receiver;
    receiver.client = dest_client_id;
    receiver.port = dest_client_port;
    snd_seq_port_subscribe_set_dest(subscription, &receiver);

    // Subscribe the ports
    if (snd_seq_subscribe_port(seq_handle, subscription) < 0) 
    {
        fprintf(stderr, "Error: Unable to subscribe ports.\n");
        return 0;
    }

    return 1;
}