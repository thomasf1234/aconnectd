#ifndef ALSA_UTILS_H
#define ALSA_UTILS_H

#include <alsa/asoundlib.h>

/**
 * Returns the ID of an ALSA client.
 * 
 * @param seq_handle The ALSA sequence handler.
 * @param client_name The name of the ALSA client to check.
 * @return ID if the client exist, -1 otherwise.
 */
int get_client_id_by_name(snd_seq_t *seq_handle, const char *client_name);

/**
 * Checks if a specific ALSA client and port exist.
 * 
 * @param seq_handle The ALSA sequence handler.
 * @param client_id The ID of the ALSA client to check.
 * @param port The port number to check.
 * @return 1 if the client port exists, 0 otherwise.
 */
int check_client_port(snd_seq_t *seq_handle, int client_id, int port);

/**
 * Checks if a specific connection exists between two ALSA clients and ports.
 * 
 * @param seq_handle The ALSA sequence handler.
 * @param src_client_id The ID of the source client.
 * @param src_client_port The port number of the source client.
 * @param dest_client_id The ID of the destination client.
 * @param dest_client_port The port number of the destination client.
 * @return 1 if the connection exists, 0 otherwise.
 */
int check_subscription(snd_seq_t *seq_handle, int src_client_id, int src_client_port, int dest_client_id, int dest_client_port);

/**
 * Create a subscription to source client  two ALSA clients and ports.
 * 
 * @param seq_handle The ALSA sequence handler.
 * @param src_client_id The ID of the source client.
 * @param src_client_port The port number of the source client.
 * @param dest_client_id The ID of the destination client.
 * @param dest_client_port The port number of the destination client.
 * @return 1 if the subscription is created, 0 otherwise.
 */
int create_subscription(snd_seq_t *seq_handle, int src_client_id, int src_client_port, int dest_client_id, int dest_client_port);

#endif

