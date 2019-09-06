#ifndef EVENT_HEADER
#define EVENT_HEADER


enum event_type {
	event_simulation_stopped = 0,
};
/**
 * @brief Sedning an event
 * @param evt event type
 * @param data optional data
 * @param data_len data length
 *
 * @return 0 for success, otherwise please check @ref error_code
 */

error_code event_send(enum event_type evt, uint8_t *data, uint8_t data_len);

#endif /* EVENT_HEADER */
