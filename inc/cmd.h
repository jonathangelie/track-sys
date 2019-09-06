#ifndef CMD_HEADER
#define CMD_HEADER

/**
 * @brief Command initialization
 * @param withUi set to True if program started from UI
 *
 * @return 0 for success, otherwise please check @ref error_code
 */

error_code cmd_init(bool withUi);
/**
 * @brief parsing command line
 *
 * @return 0 for success, otherwise please check @ref error_code
 */

error_code cmd_read(void);

error_code cmd_execute(uint8_t *line, uint8_t len);

#endif /* CMD_HEADER */
