
#define NOTE_ON (0b1001)
#define NOTE_OFF (0b1000)
#define PITCH (0b1110)
#define INSTRUMENT (0b1100)
#define CONTROL (0b1011)

#define VOLUME  (0x07)
#define MODULO  (0x01)
#define PAN     (0x0A)
#define BALANCE (0x08)

void write_test_note(void);

void send_byte(uint8_t byte);

void pitch_bend(uint8_t channel, int16_t value);

void change_instrument(uint8_t channel, uint8_t instrument);

void change_volume(uint8_t chan, uint8_t vol);

void send_controller_command(uint8_t channel, uint8_t controller,uint8_t value);

void send_note(uint8_t channel, uint8_t vel, uint8_t pitch);

void send_note_off(uint8_t channel, uint8_t vel, uint8_t pitch);
