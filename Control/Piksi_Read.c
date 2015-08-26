
#include "Piksi_Read.h"

u32 serialread(u8 *buf, u32 n, void* context) {
    context_t ctx = *(context_t *)context;
    return read(ctx.fd, buf, n);
}

void sbp_pos_llh_callback(u16 sender_id, u8 len, u8 msg[], void *context)
{
  pos_llh = *(msg_pos_llh_t *)msg;
}
void sbp_baseline_ned_callback(u16 sender_id, u8 len, u8 msg[], void *context)
{
  baseline_ned = *(msg_baseline_ned_t *)msg;
}
void sbp_vel_ned_callback(u16 sender_id, u8 len, u8 msg[], void *context)
{
  vel_ned = *(msg_vel_ned_t *)msg;
}
void sbp_dops_callback(u16 sender_id, u8 len, u8 msg[], void *context)
{
  dops = *(msg_dops_t *)msg;
}
void sbp_gps_time_callback(u16 sender_id, u8 len, u8 msg[], void *context)
{
  gps_time = *(msg_gps_time_t *)msg;
}

/*
 * Set up SwiftNav Binary Protocol (SBP) nodes; the sbp_process function will
 * search through these to find the callback for a particular message ID.
 * Example: sbp_pos_llh_callback is registered with sbp_state, and is associated
 * with both a unique sbp_msg_callbacks_node_t and the message ID SBP_POS_LLH.
 * When a valid SBP message with the ID SBP_POS_LLH comes through the UART, written
 * to the FIFO, and then parsed by sbp_process, sbp_pos_llh_callback is called
 * with the data carried by that message.
 */
void sbp_setup(sbp_state_t *sbp_state, context_t *context)
{
  /* SBP parser state must be initialized before sbp_process is called. */
  sbp_state_init(sbp_state);
  sbp_state_set_io_context(sbp_state, context);
  /* Register a node and callback, and associate them with a specific message ID. */
  sbp_register_callback(sbp_state, SBP_MSG_GPS_TIME, &sbp_gps_time_callback,
                        NULL, &gps_time_node);
  sbp_register_callback(sbp_state, SBP_MSG_POS_LLH, &sbp_pos_llh_callback,
                        NULL, &pos_llh_node);
  sbp_register_callback(sbp_state, SBP_MSG_BASELINE_NED, &sbp_baseline_ned_callback,
                        NULL, &baseline_ned_node);
  sbp_register_callback(sbp_state, SBP_MSG_VEL_NED, &sbp_vel_ned_callback,
                        NULL, &vel_ned_node);
  sbp_register_callback(sbp_state, SBP_MSG_DOPS, &sbp_dops_callback,
                        NULL, &dops_node);
}

int openSerial(char *path, speed_t baud) {
    printf("Opening %s\b\n", path);
    int fd = open(path, O_RDWR | O_NOCTTY);
    struct termios toptions;
    bzero(&toptions, sizeof(toptions));

    if (tcgetattr(fd, &toptions) < 0) {
        perror("tcgetattr");
        return -1;
    }
    toptions.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;
    toptions.c_iflag = IGNPAR;
    toptions.c_oflag = 0;
    toptions.c_lflag = 0;
	
    struct serial_struct ss; 
    ioctl(fd, TIOCGSERIAL, &ss); 
    ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
    ss.custom_divisor = ss.baud_base / baud; 
    ioctl(fd, TIOCSSERIAL, &ss);
	
    cfsetispeed(&toptions, B38400);
    cfsetospeed(&toptions, B38400);
    toptions.c_cc[VMIN]     = 5;   // blocking read until 5 chars received
    toptions.c_cc[VTIME]    = 0;   // inter-character timer unused
    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &toptions) < 0) {
        perror("tcsetattr");
        return -1;
    }
    return fd;
}

int OpenPiksi(char *path, int baud) {
    context.path = path;
    context.fd = openSerial(path, baud);
    sbp_setup(&sbp_state, &context);
    return context.fd;
}

void ProcessPiksi() {
    sbp_process(&sbp_state, &serialread);
}
