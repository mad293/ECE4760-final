/**
* @brief Partial list of possible commands.
*/
#include <inttypes.h>
enum commands {
 NOTE_ON=(0b1001),
 NOTE_OFF=(0b1000),
 PITCH=(0b1110),
 INSTRUMENT=(0b1100),
 CONTROL=(0b1011),
};


/**
* @brief Partial list of the controllers that we can possibly change.
*/
enum control_parameters {
  VOLUME=(0x07),
  MODULO=(0x01),
  PAN=(0x0A),
  BALANCE=(0x08),
};

/**
* @brief send a byte to the serial stream.
*
* @param byte Data byte
*/
void send_byte(uint8_t byte);

/**
* @brief Bend the pitch of the note playing on the specified channel.
*
* @param channel Channel
* @param value 2000 is no pitch bend a 14 bit value. We take the 16 bit number and use the 14 most significant bits, this allows for easier two's compliment arethmetic.
*/
void pitch_bend(uint8_t channel, int16_t value);

/**
*
* @brief Change the instrument on the specified channel.
* @param channel Channel
* @param instrument New instrument code
*/
void change_instrument(uint8_t channel, uint8_t instrument);

/**
* @brief  Change the volume on the specified channel
*
* @param chan Channel
* @param vol New volume
*/
void change_volume(uint8_t chan, uint8_t vol);

/**
* @brief sends a control command to the midi device. See the enum control parameters for a (partial) list of the available commands
*
* @param channel Channel to change the control on.
* @param controller Byte representing the controller you wish to change.
* @param value New value for the controller.
*/
void send_controller_command(uint8_t channel, uint8_t controller,uint8_t value);

/**
* @brief Turns a note on a certain channel
*
* @param channel
* @param vel
* @param pitch
*/
void send_note(uint8_t channel, uint8_t vel, uint8_t pitch);

/**
* @brief Turns a note off on a certain channel
*
* @param channel MIDI-channel
* @param vel Velocity of the note
* @param pitch Pitch of the note
*/
void send_note_off(uint8_t channel, uint8_t vel, uint8_t pitch);
