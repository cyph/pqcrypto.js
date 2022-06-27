#include <stdlib.h>
#include <stdbool.h>
#include "crypto_aead.h"
#include "api.h"
#include "common.h"

uint64_t sk[72];

bool need_to_fulfill = false;
bool msg_not_integral = false;
bool cipher_not_integral = false;
bool isValidDec = true;

void add_2sum_128(uint64_t input1[2], uint64_t input2[2], uint64_t output[2]) 
{
	unsigned char *input_bytes1, *input_bytes2, *output_bytes;
	
	input_bytes1 = (unsigned char *)malloc(16 * sizeof(unsigned char));
	input_bytes2 = (unsigned char *)malloc(16 * sizeof(unsigned char));
	output_bytes = (unsigned char *)malloc(16 * sizeof(unsigned char));

	cast_uint64_to_uint8_array(input_bytes1, input1[0]);
	cast_uint64_to_uint8_array(input_bytes1 + 8, input1[1]);
	cast_uint64_to_uint8_array(input_bytes2, input2[0]);
	cast_uint64_to_uint8_array(input_bytes2 + 8, input2[1]);

	unsigned char carry = 0;
	uint32_t tmp = 0;

	for (int i = 8; i < 16; i++) {
		tmp = 0;
		tmp = input_bytes1[i] + input_bytes2[i] + carry;
		output_bytes[i] = tmp & 0x000000ff;
		carry = (tmp >> 8) & 0x000000ff;
	}
	for (int i = 0; i < 8; i++) {
		tmp = 0;
		tmp = input_bytes1[i] + input_bytes2[i] + carry;
		output_bytes[i] = tmp & 0x000000ff;
		carry = (tmp >> 8) & 0x000000ff;
	}

	cast_uint8_array_to_uint64(output, output_bytes);
	cast_uint8_array_to_uint64(output + 1, output_bytes + 8);

	free(input_bytes1);
	free(input_bytes2);
	free(output_bytes);
}

//2x ^ y, 3x ^ y
void tho(uint64_t input1[2], uint64_t input2[2], uint64_t output[2]) 
{
	add_2sum_128(input1, input1, output);
	output[0] ^= input2[0];
	output[1] ^= input2[1];
}

void tho_y(uint64_t input1[2], uint64_t input2[2], uint64_t output[2]) 
{
	uint64_t tmp[2];
	add_2sum_128(input1, input1, tmp);
	add_2sum_128(input1, tmp, output);
	output[0] ^= input2[0];
	output[1] ^= input2[1];
}

void tho_reverse_y(uint64_t input1[2], uint64_t input2[2], uint64_t output[2]) 
{
	uint64_t tmp[2];
	add_2sum_128(input1, input1, tmp);
	add_2sum_128(input1, tmp, output);
	output[0] ^= input2[0];
	output[1] ^= input2[1];
}

/* -------------- transform functions ---------------- */

void cast_uint8_array_to_uint64(uint64_t *dest, const unsigned char *src)
{
	*dest = (uint64_t)src[7] << 56 |
		(uint64_t)src[6] << 48 |
		(uint64_t)src[5] << 40 |
		(uint64_t)src[4] << 32 |
		(uint64_t)src[3] << 24 |
		(uint64_t)src[2] << 16 |
		(uint64_t)src[1] << 8 |
		(uint64_t)src[0];
}

void cast_uint64_to_uint8_array(unsigned char *dest, uint64_t src)
{
	dest[0] = (unsigned char)(src & 0x00000000000000ff);
	dest[1] = (unsigned char)((src & 0x000000000000ff00) >> 8);
	dest[2] = (unsigned char)((src & 0x0000000000ff0000) >> 16);
	dest[3] = (unsigned char)((src & 0x00000000ff000000) >> 24);
	dest[4] = (unsigned char)((src & 0x000000ff00000000) >> 32);
	dest[5] = (unsigned char)((src & 0x0000ff0000000000) >> 40);
	dest[6] = (unsigned char)((src & 0x00ff000000000000) >> 48);
	dest[7] = (unsigned char)((src & 0xff00000000000000) >> 56);
}

/* -------------- transform functions ---------------- */

/* key shedule of simon 128 */

void key_schedule(uint64_t* key, uint64_t* dest)
{
	for (uint32_t i = 0; i < KEY_WORDS; i++) {
		dest[i] = key[i];
	}
	for (uint32_t i = KEY_WORDS; i < ROUNDS; i++) {
		uint64_t y = RROT(dest[i - 1], 3);
		/* in case the change of word size */
		if (KEY_WORDS == 4) {
			y ^= dest[i - 3];
		}
		dest[i] = RROT(y, 1) ^ dest[i - KEY_WORDS] ^ y ^ ENC_CONSTANT ^ Z_VECTOR[Z_VECTOR_CH][(i - KEY_WORDS) % 62];
	}
}

/* simon round function */

void simon_round(uint64_t* p1, uint64_t* p2, uint64_t k)
{
	uint64_t p1_tmp = *p1;
	*p1 = *p2 ^ ENTIRE_ROTATE(*p1) ^ k;
	*p2 = p1_tmp;
}

/* encryption */

void simon_encrypt(uint64_t p[2], uint64_t c[2], uint64_t k[ROUNDS])
{
	c[0] = p[0];
	c[1] = p[1];
	for (uint32_t i = 0; i < ROUNDS; i++) {
		simon_round(&c[0], &c[1], k[i]);
	}
}

/* decryption */

void simon_decrypt(uint64_t c[2], uint64_t p[2], uint64_t k[ROUNDS])
{
	p[0] = c[0];
	p[1] = c[1];
	for (uint32_t i = ROUNDS; i > 0; i--) {
		simon_backwards(&p[0], &p[1], k[i - 1]);
	}
}

void simon_backwards(uint64_t* p1, uint64_t* p2, uint64_t k)
{
	uint64_t p2_tmp = *p2;
	*p2 = *p1 ^ ENTIRE_ROTATE(*p2) ^ k;
	*p1 = p2_tmp;
}

/* decryption */

void generate_s0_star(uint64_t nonce[2], uint64_t s0_star[2]) 
{  
	simon_encrypt(nonce, s0_star, sk);
}

void calculate_s0(const unsigned char *helper, uint64_t helper_byte_length, uint64_t *nonce, uint64_t *s0) 
{
	if(helper_byte_length == 0)
	{
		generate_s0_star(nonce, s0) ;
	}
	else
	{
		uint64_t helper_block_length = (uint64_t)(helper_byte_length / BYTE_SIZE_PER_BLOCK);
		if (helper_byte_length % BYTE_SIZE_PER_BLOCK != 0) 
		{
			helper_block_length++;
		}
		uint64_t *helper_ablocks = (uint64_t *)malloc(helper_block_length * 2 * sizeof(uint64_t));

		fill_plaintext(helper, helper_byte_length, helper_ablocks);
	
		ae_encrypt_part_one(helper_ablocks, helper_block_length, s0, nonce);
		free(helper_ablocks);
	}
}


void copy_message(const unsigned char *src, unsigned char *dest, uint64_t src_byte_length, uint64_t require_byte_length) 
{
	uint64_t i = 0;
	for (; i < src_byte_length; i++) {
		dest[i] = src[i];
	}
	if (require_byte_length > 0) {
		dest[i++] = 0x80;
		for (; i < src_byte_length + require_byte_length; i++) {
			dest[i] = 0x00;
		}
	}
}

/*  message for encryption and src_length for the byte length  */

void fill_plaintext(const unsigned char *message, uint64_t src_byte_length, uint64_t* block_messages) 
{
	uint64_t total_block = (uint64_t)(src_byte_length / BYTE_SIZE_PER_BLOCK);
	unsigned char *filled_message;

	if (src_byte_length % BYTE_SIZE_PER_BLOCK != 0) 
	{
		need_to_fulfill = true;

		uint64_t left_byte_length = src_byte_length - total_block * BYTE_SIZE_PER_BLOCK;
		uint64_t require_byte_length = BYTE_SIZE_PER_BLOCK - left_byte_length;

		total_block++;
		filled_message = (unsigned char *)malloc(total_block * BYTE_SIZE_PER_BLOCK * sizeof(unsigned char));

		copy_message(message, filled_message, src_byte_length, require_byte_length);
	}
	else
	{
		filled_message = (unsigned char *)malloc(total_block * BYTE_SIZE_PER_BLOCK * sizeof(unsigned char));
		copy_message(message, filled_message, src_byte_length, 0);
	}

	//block_messages = (uint64_t *)malloc(total_block * 2 * sizeof(uint64_t));

	for (uint64_t i = 0; i < total_block * 2; i++) 
	{
		cast_uint8_array_to_uint64(&block_messages[i], filled_message + (i * BYTE_SIZE_PER_BLOCK) / 2);
	}

	free(filled_message);
	return;
}

/* encryption of helper array and output s0 */

void ae_encrypt_part_one(uint64_t *block_messages, uint64_t block_length, uint64_t s_output[2], uint64_t nonce[2]) 
{

	uint64_t s0_star[2], s_tmp_input[2], s_tmp_output[2], s_tmp_enc[2], a_enc[2];
	uint64_t ya[2], add_res[2];

	generate_s0_star(nonce, s0_star);

	s_tmp_input[0] = s0_star[0];
	s_tmp_input[1] = s0_star[1];

	a_enc[0] = *block_messages++;
	a_enc[1] = *block_messages++;

	for (uint64_t i = 0; i < block_length - 1; i++) 
	{
		simon_encrypt(a_enc, s_tmp_enc, sk);
		tho(s_tmp_input, s_tmp_enc, s_tmp_output);
		s_tmp_input[0] = s_tmp_output[0];
		s_tmp_input[1] = s_tmp_output[1];
		a_enc[0] = *block_messages++;
		a_enc[1] = *block_messages++;
	}

	simon_encrypt(a_enc, ya, sk);

	if (need_to_fulfill) 
	{
		add_2sum_128(s_tmp_input, s_tmp_input, add_res);
		add_2sum_128(s_tmp_input, add_res, s_output);
	}
	else 
	{
		add_2sum_128(s_tmp_input, s_tmp_input, s_output);
	}

	s_output[0] ^= ya[0];
	s_output[1] ^= ya[1];
}

/* fill up the messages */

void fill_messages(const unsigned char *message, uint64_t src_byte_length, uint64_t* block_messages, uint64_t msg_block) 
{
	uint64_t *ptr = block_messages;
	uint64_t i = src_byte_length;

	for (i = 0; i < msg_block - 1; i++) 
	{
		/* copy the former 8 bytes */
		*ptr++ = *((uint64_t*)(message + i * 8));
		/* concat the number */
		*ptr++ = i + 1;
	}
	return;
}

/* encryption of message and ouput cipher */

void ae_encrypt_part_two(uint64_t s0[2], uint64_t *block_messages, uint64_t block_length, uint64_t s_last[2], uint64_t* cipher_output) 
{
	uint64_t  *ptr = block_messages;
	uint64_t s_tmp_input[2], s_tmp_output[2], msg_tmp_input[2], pre_enc[2], after_enc[2];

	s_tmp_input[0] = s0[0];
	s_tmp_input[1] = s0[1];

	for (uint64_t i = 0; i < block_length - 1; i++) {
		msg_tmp_input[0] = *ptr++;
		msg_tmp_input[1] = *ptr++;

		tho(s_tmp_input, msg_tmp_input, s_tmp_output);
		tho_y(s_tmp_input, msg_tmp_input, pre_enc);

		s_tmp_input[0] = s_tmp_output[0];
		s_tmp_input[1] = s_tmp_output[1];

		if (i == block_length - 3) {
			s_last[0] = s_tmp_output[0];
			s_last[1] = s_tmp_output[1];
		}

		simon_encrypt(pre_enc, after_enc, sk);

		cipher_output[2 * i] = after_enc[0];
		cipher_output[2 * i + 1] = after_enc[1];
	}

	return;
}

void ae_encrypt_byte(const unsigned char *message, uint64_t src_byte_length, uint64_t nonce[2],const unsigned char *helper, uint64_t helper_byte_length, unsigned char *cipher) 
{
	//printf("need");
	uint64_t s0[2], s_last[2];
	calculate_s0(helper, helper_byte_length, nonce, s0);

	uint64_t msg_block_length = (uint64_t)(src_byte_length / MSG_BLOCK_SIZE);
	if (src_byte_length % MSG_BLOCK_SIZE != 0) 
	{
		msg_block_length++;
	}

	uint64_t *block_messages = (uint64_t *)malloc(msg_block_length * 2 * sizeof(uint64_t));
	fill_messages(message, src_byte_length, block_messages, msg_block_length);
		
	uint64_t *cipher_messages = (uint64_t *)malloc(msg_block_length * 2 * sizeof(uint64_t));
	ae_encrypt_part_two(s0, block_messages, msg_block_length, s_last, cipher_messages);
	
	uint64_t msg_left_byte = src_byte_length % MSG_BLOCK_SIZE;
	uint64_t cipher_before_last[2];

	cipher_before_last[0] = cipher_messages[(msg_block_length - 2) * 2];
	cipher_before_last[1] = cipher_messages[(msg_block_length - 2) * 2 + 1];

	/* byte length of Z */
	uint64_t msg_require_byte = MSG_BLOCK_SIZE - msg_left_byte;
	unsigned char *padding_byte = (unsigned char *)malloc(MSG_BLOCK_SIZE * sizeof(unsigned char));
	cast_uint64_to_uint8_array(padding_byte, cipher_before_last[1]);

	for (uint64_t i = 0; i < msg_left_byte; i++) {
		padding_byte[i] = message[(msg_block_length - 1) * MSG_BLOCK_SIZE + i];
	}

	uint64_t msg_last[2];
	cast_uint8_array_to_uint64(msg_last, padding_byte);
	msg_last[1] = 0x00;

	uint64_t msg_len_before_enc[2];
	msg_len_before_enc[0] = src_byte_length * 8;
	msg_len_before_enc[1] = 0;

	uint64_t msg_len_after_enc[2];

	simon_encrypt(msg_len_before_enc, msg_len_after_enc, sk);

	uint64_t msg_last_after_tho[2];

	tho_y(s_last, msg_last, msg_last_after_tho);
	msg_last_after_tho[0] ^= msg_len_after_enc[0];
	msg_last_after_tho[1] ^= msg_len_after_enc[1];

	uint64_t cipher_last[2];

	simon_encrypt(msg_last_after_tho, cipher_last, sk);

	cipher_messages[(msg_block_length - 1) * 2] = cipher_last[0];
	cipher_messages[(msg_block_length - 1) * 2 + 1] = cipher_last[1];

	uint64_t cipher_output_byte_length = msg_block_length * BYTE_SIZE_PER_BLOCK - msg_require_byte;

	//cipher = (unsigned char *)malloc(cipher_output_byte_length * sizeof(unsigned char));
	unsigned char *cipher_transform_bytes = (unsigned char *)malloc(msg_block_length * BYTE_SIZE_PER_BLOCK * sizeof(unsigned char));

	for (uint64_t i = 0; i < msg_block_length; i++) {
		cast_uint64_to_uint8_array(&cipher_transform_bytes[2 * i * 8], cipher_messages[i * 2]);
		cast_uint64_to_uint8_array(&cipher_transform_bytes[(2 * i + 1) * 8], cipher_messages[i * 2 + 1]);
	}

	uint64_t index = 0;

	for (uint64_t i = 0; i < cipher_output_byte_length - BYTE_SIZE_PER_BLOCK; i++) {
		cipher[index++] = cipher_transform_bytes[i];
	}

	for (uint64_t i = (msg_block_length - 1) * BYTE_SIZE_PER_BLOCK; i < msg_block_length * BYTE_SIZE_PER_BLOCK; i++) {
		cipher[index++] = cipher_transform_bytes[i];
	}

	/*printf("\n\n output cipher bytes: \n\n");

	for (int i = 0; i < cipher_output_byte_length; i++) {
		printf("0x%02x, ", cipher_output_bytes[i]);
	}

	printf("\n\n");*/
	free(padding_byte);
	free(cipher_transform_bytes);
	free(block_messages);
	free(cipher_messages);
	return;
}

void ae_decrypt_byte(const unsigned char *cipher, uint64_t cipher_byte_length, uint64_t nonce[2],const unsigned char *helper, uint64_t helper_byte_length, unsigned char* msg_output_bytes) 
{
	//printf("need dec");

	uint64_t s0[2];
	calculate_s0(helper, helper_byte_length, nonce, s0);

	uint64_t cipher_block_length = (uint64_t)(cipher_byte_length / BYTE_SIZE_PER_BLOCK);

	if (cipher_byte_length % BYTE_SIZE_PER_BLOCK != 0) {
		cipher_block_length++;
		cipher_not_integral = true;
	}

	uint64_t cipher_left_length = cipher_byte_length % BYTE_SIZE_PER_BLOCK;

	uint64_t *cipher_blocks = (uint64_t *)malloc(cipher_block_length * 2 * sizeof(uint64_t));

	for (uint64_t i = 0; i < cipher_block_length - 2; i++) {
		cast_uint8_array_to_uint64(&cipher_blocks[i * 2], &cipher[2 * i * 8]);
		cast_uint8_array_to_uint64(&cipher_blocks[i * 2 + 1], &cipher[(2 * i + 1) * 8]);
	}

	uint64_t s_tmp_input[2], s_tmp_output[2];
	uint64_t cipher_before_dec[2], cipher_after_dec[2], msg_dec[2];
	uint64_t *ptr = cipher_blocks, *msg_output;
 	
	msg_output= (uint64_t *)malloc(cipher_block_length * 2 * sizeof(uint64_t));
	s_tmp_input[0] = s0[0];
	s_tmp_input[1] = s0[1];

	for (uint64_t i = 0; i < cipher_block_length - 2; i++) {
		cipher_before_dec[0] = *ptr++;
		cipher_before_dec[1] = *ptr++;
		simon_decrypt(cipher_before_dec, cipher_after_dec, sk);

		tho_reverse_y(s_tmp_input, cipher_after_dec, msg_dec);
		tho(s_tmp_input, msg_dec, s_tmp_output);

		s_tmp_input[0] = s_tmp_output[0];
		s_tmp_input[1] = s_tmp_output[1];

		msg_output[2 * i] = msg_dec[0];
		msg_output[2 * i + 1] = msg_dec[1];
	}

	/* recover the last two blocks */

	uint64_t cipher_last[2], msg_last[2], msg_pre_last[2];
	cast_uint8_array_to_uint64(&cipher_last[0], &cipher[cipher_byte_length - BYTE_SIZE_PER_BLOCK]);
	cast_uint8_array_to_uint64(&cipher_last[1], &cipher[cipher_byte_length - BYTE_SIZE_PER_BLOCK / 2]);

	uint64_t cipher_length_before_enc[2], cipher_length_after_enc[2];
	cipher_length_before_enc[0] = (cipher_byte_length - cipher_block_length * MSG_BLOCK_SIZE) * 8;
	cipher_length_before_enc[1] = 0;
	simon_encrypt(cipher_length_before_enc, cipher_length_after_enc, sk);

	uint64_t cipher_last_dec[2];
	simon_decrypt(cipher_last, cipher_last_dec, sk);

	cipher_last_dec[0] ^= cipher_length_after_enc[0];
	cipher_last_dec[1] ^= cipher_length_after_enc[1];
	tho_reverse_y(s_tmp_output, cipher_last_dec, msg_last);

	/* get the length of Z */
	uint64_t z_length = BYTE_SIZE_PER_BLOCK - cipher_left_length;

	/* recover the block before last */
	unsigned char *cipher_pre_last_bytes = (unsigned char *)malloc(BYTE_SIZE_PER_BLOCK * sizeof(unsigned char));
	uint64_t index = 0;
	for (uint64_t i = cipher_byte_length - BYTE_SIZE_PER_BLOCK - cipher_left_length; i < cipher_byte_length - BYTE_SIZE_PER_BLOCK; i++) {
		cipher_pre_last_bytes[index++] = cipher[i];
	}

	unsigned char *msg_pre_half = (unsigned char *)malloc(MSG_BLOCK_SIZE * sizeof(unsigned char));

	cast_uint64_to_uint8_array(msg_pre_half, msg_last[0]);

	for (uint64_t i = MSG_BLOCK_SIZE - z_length; i < MSG_BLOCK_SIZE; i++) {
		cipher_pre_last_bytes[index++] = msg_pre_half[i];
	}

	uint64_t cipher_pre_last_dec[2], cipher_pre_last[2];
	cast_uint8_array_to_uint64(&cipher_pre_last[0], cipher_pre_last_bytes);
	cast_uint8_array_to_uint64(&cipher_pre_last[1], cipher_pre_last_bytes + 8);
	simon_decrypt(cipher_pre_last, cipher_pre_last_dec, sk);

	tho_reverse_y(s_tmp_output, cipher_pre_last_dec, msg_pre_last);

	msg_output[2 * (cipher_block_length - 2)] = msg_pre_last[0];
	msg_output[2 * (cipher_block_length - 2) + 1] = msg_pre_last[1];
	msg_output[2 * (cipher_block_length - 1)] = msg_last[0];
	msg_output[2 * (cipher_block_length - 1) + 1] = msg_last[1];

	/* output plaintext bytes */
	uint64_t msg_byte_len = cipher_byte_length - cipher_block_length * MSG_BLOCK_SIZE;
	unsigned char *msg_transform_bytes = (unsigned char *)malloc(cipher_block_length * BYTE_SIZE_PER_BLOCK * sizeof(unsigned char));
	//msg_output_bytes = (unsigned char *)malloc(msg_byte_len * sizeof(unsigned char));
	unsigned char *valid_bytes = (unsigned char *)malloc((cipher_block_length - 1) * MSG_BLOCK_SIZE * sizeof(unsigned char));

	for (uint64_t i = 0; i < cipher_block_length; i++) {
		cast_uint64_to_uint8_array(&msg_transform_bytes[2 * i * 8], msg_output[i * 2]);
		cast_uint64_to_uint8_array(&msg_transform_bytes[(2 * i + 1) * 8], msg_output[i * 2 + 1]);
	}

	uint64_t index_byte = 0;
	uint64_t count = 1;
	uint64_t tmp;

	for (uint64_t i = 0; i < cipher_block_length - 1; i++) {
		uint64_t j = i * BYTE_SIZE_PER_BLOCK;
		for (uint64_t k = 0; k < MSG_BLOCK_SIZE; k++) {
			msg_output_bytes[index_byte] = msg_transform_bytes[j + k];
			valid_bytes[index_byte] = msg_transform_bytes[j + k + MSG_BLOCK_SIZE];
			//printf("%02x ", valid_bytes[index_byte]);
			index_byte++;
		}
		cast_uint8_array_to_uint64(&tmp, &valid_bytes[i * MSG_BLOCK_SIZE]);
		if (tmp != count) {
			isValidDec = false;
		}
		count++;
	}
	cast_uint8_array_to_uint64(&tmp, &msg_transform_bytes[(cipher_block_length - 1) * BYTE_SIZE_PER_BLOCK + MSG_BLOCK_SIZE]);
	if (tmp != 0) {
		isValidDec = false;
	}
	uint64_t msg_left_len = msg_byte_len % MSG_BLOCK_SIZE;

	for (uint64_t i = (cipher_block_length - 1) * BYTE_SIZE_PER_BLOCK; i < (cipher_block_length - 1) * BYTE_SIZE_PER_BLOCK + msg_left_len; i++) {
		msg_output_bytes[index_byte++] = msg_transform_bytes[i];
	}

	free(cipher_blocks);
	free(msg_output);
	free(cipher_pre_last_bytes);
	free(msg_pre_half);
	free(msg_transform_bytes);
	free(valid_bytes);
	return;
}

void ae_encrypt_byte_nopad(const unsigned char *message, uint64_t src_byte_length, uint64_t nonce[2],const unsigned char *helper, uint64_t helper_byte_length, unsigned char *cipher) {

	uint64_t s0[2], s_last[2];
	calculate_s0(helper, helper_byte_length, nonce, s0);

	uint64_t msg_block_length = (uint64_t)(src_byte_length / MSG_BLOCK_SIZE);
	
	uint64_t msg_block_length1 = (uint64_t)((src_byte_length + 1) / MSG_BLOCK_SIZE);
	if ((src_byte_length + 1) % MSG_BLOCK_SIZE != 0) 
	{
		msg_block_length1++;
	}

	uint64_t *block_messages = (uint64_t *)malloc(msg_block_length1 * 2 * sizeof(uint64_t));

	fill_messages(message, src_byte_length + 1, block_messages, msg_block_length1);
	
	uint64_t *cipher_messages = (uint64_t *)malloc(msg_block_length * 2 * sizeof(uint64_t));
	ae_encrypt_part_two(s0, block_messages, msg_block_length, s_last, cipher_messages);

//	uint64_t cipher_before_last[2];
//	cipher_before_last[0] = cipher_messages[(msg_block_length - 2) * 2];
//	cipher_before_last[1] = cipher_messages[(msg_block_length - 2) * 2 + 1];

	unsigned char *padding_byte = (unsigned char *)malloc(MSG_BLOCK_SIZE * sizeof(unsigned char));

	for (uint64_t i = 0; i < MSG_BLOCK_SIZE; i++) {
		padding_byte[i] = message[(msg_block_length - 1) * MSG_BLOCK_SIZE + i];
	}

	uint64_t msg_last[2];
	cast_uint8_array_to_uint64(msg_last, padding_byte);
	msg_last[1] = 0x00;

	uint64_t msg_len_before_enc[2];
	msg_len_before_enc[0] = src_byte_length * 8;
	msg_len_before_enc[1] = 0;

	uint64_t msg_len_after_enc[2];

	simon_encrypt(msg_len_before_enc, msg_len_after_enc, sk);

	uint64_t msg_last_after_tho[2];

	tho_y(s_last, msg_last, msg_last_after_tho);

	msg_last_after_tho[0] ^= msg_len_after_enc[0];
	msg_last_after_tho[1] ^= msg_len_after_enc[1];

	uint64_t cipher_last[2];

	simon_encrypt(msg_last_after_tho, cipher_last, sk);

	cipher_messages[(msg_block_length - 1) * 2] = cipher_last[0];
	cipher_messages[(msg_block_length - 1) * 2 + 1] = cipher_last[1];

	//cipher = (unsigned char *)malloc(msg_block_length * BYTE_SIZE_PER_BLOCK * sizeof(unsigned char));

	for (uint64_t i = 0; i < msg_block_length; i++) {
		cast_uint64_to_uint8_array(&cipher[2 * i * 8], cipher_messages[i * 2]);
		cast_uint64_to_uint8_array(&cipher[(2 * i + 1) * 8], cipher_messages[i * 2 + 1]);
	}

	//printf("\n\n output ciphers: \n\n");

	//for (uint64_t i = 0; i < msg_block_length * BYTE_SIZE_PER_BLOCK; i++) {
	//	printf("0x%02x, ", cipher_transform_bytes[i]);
	//}

	//printf("\n\n");
	free(padding_byte);
	free(block_messages);
	free(cipher_messages);
	return;
}

void ae_decrypt_byte_nopad(const unsigned char *cipher, uint64_t cipher_byte_length, uint64_t nonce[2],const unsigned char *helper, uint64_t helper_byte_length, unsigned char* msg_output_bytes) 
{
	uint64_t s0[2];
	calculate_s0(helper, helper_byte_length, nonce, s0);

	uint64_t cipher_block_length = (uint64_t)(cipher_byte_length / BYTE_SIZE_PER_BLOCK);

	uint64_t *cipher_blocks = (uint64_t *)malloc(cipher_block_length * 2 * sizeof(uint64_t));

	for (uint64_t i = 0; i < cipher_block_length - 2; i++) {
		cast_uint8_array_to_uint64(&cipher_blocks[i * 2], &cipher[2 * i * 8]);
		cast_uint8_array_to_uint64(&cipher_blocks[i * 2 + 1], &cipher[(2 * i + 1) * 8]);
	}

	uint64_t s_tmp_input[2], s_tmp_output[2];
	uint64_t cipher_before_dec[2], cipher_after_dec[2], msg_dec[2];
	uint64_t *ptr = cipher_blocks, *msg_output;

	msg_output = (uint64_t *)malloc(cipher_block_length * 2 * sizeof(uint64_t));

	s_tmp_input[0] = s0[0];
	s_tmp_input[1] = s0[1];

	for (uint64_t i = 0; i < cipher_block_length - 2; i++) {
		cipher_before_dec[0] = *ptr++;
		cipher_before_dec[1] = *ptr++;
		simon_decrypt(cipher_before_dec, cipher_after_dec, sk);

		tho_reverse_y(s_tmp_input, cipher_after_dec, msg_dec);
		tho(s_tmp_input, msg_dec, s_tmp_output);

		s_tmp_input[0] = s_tmp_output[0];
		s_tmp_input[1] = s_tmp_output[1];

		msg_output[2 * i] = msg_dec[0];
		msg_output[2 * i + 1] = msg_dec[1];
	}

	/* recover the last two blocks */

	uint64_t cipher_last[2], msg_last[2], msg_pre_last[2];
	cast_uint8_array_to_uint64(&cipher_last[0], &cipher[cipher_byte_length - BYTE_SIZE_PER_BLOCK]);
	cast_uint8_array_to_uint64(&cipher_last[1], &cipher[cipher_byte_length - BYTE_SIZE_PER_BLOCK / 2]);

	uint64_t cipher_length_before_enc[2], cipher_length_after_enc[2];
	cipher_length_before_enc[0] = (cipher_byte_length - cipher_block_length * MSG_BLOCK_SIZE) * 8;
	cipher_length_before_enc[1] = 0;
	simon_encrypt(cipher_length_before_enc, cipher_length_after_enc, sk);

	uint64_t cipher_last_dec[2];
	simon_decrypt(cipher_last, cipher_last_dec, sk);

	cipher_last_dec[0] ^= cipher_length_after_enc[0];
	cipher_last_dec[1] ^= cipher_length_after_enc[1];

	tho_reverse_y(s_tmp_output, cipher_last_dec, msg_last);

	/* recover the block before last */
	unsigned char *cipher_pre_last_bytes = (unsigned char *)malloc(BYTE_SIZE_PER_BLOCK * sizeof(unsigned char));
	uint64_t index = 0;
	for (uint64_t i = cipher_byte_length - BYTE_SIZE_PER_BLOCK * 2; i < cipher_byte_length - BYTE_SIZE_PER_BLOCK; i++) {
		cipher_pre_last_bytes[index++] = cipher[i];
	}

	uint64_t cipher_pre_last_dec[2], cipher_pre_last[2];
	cast_uint8_array_to_uint64(&cipher_pre_last[0], cipher_pre_last_bytes);
	cast_uint8_array_to_uint64(&cipher_pre_last[1], cipher_pre_last_bytes + 8);
	simon_decrypt(cipher_pre_last, cipher_pre_last_dec, sk);

	tho_reverse_y(s_tmp_output, cipher_pre_last_dec, msg_pre_last);

	msg_output[2 * (cipher_block_length - 2)] = msg_pre_last[0];
	msg_output[2 * (cipher_block_length - 2) + 1] = msg_pre_last[1];
	msg_output[2 * (cipher_block_length - 1)] = msg_last[0];
	msg_output[2 * (cipher_block_length - 1) + 1] = msg_last[1];

	/* output plaintext */
	uint64_t msg_byte_len = cipher_byte_length - cipher_block_length * MSG_BLOCK_SIZE;
	unsigned char *msg_transform_bytes = (unsigned char *)malloc(cipher_block_length * BYTE_SIZE_PER_BLOCK * sizeof(unsigned char));
	//msg_output_bytes = (unsigned char *)malloc(msg_byte_len * sizeof(unsigned char));
	unsigned char *valid_bytes = (unsigned char *)malloc(msg_byte_len * sizeof(unsigned char));

	for (uint64_t i = 0; i < cipher_block_length; i++) {
		cast_uint64_to_uint8_array(&msg_transform_bytes[2 * i * 8], msg_output[i * 2]);
		cast_uint64_to_uint8_array(&msg_transform_bytes[(2 * i + 1) * 8], msg_output[i * 2 + 1]);
	}

	uint64_t index_byte = 0;
	uint64_t count = 1;
	uint64_t tmp;

	for (uint64_t i = 0; i < cipher_block_length; i++) {
		uint64_t j = i * BYTE_SIZE_PER_BLOCK;
		for (uint64_t k = 0; k < MSG_BLOCK_SIZE; k++) {
			msg_output_bytes[index_byte] = msg_transform_bytes[j + k];
			valid_bytes[index_byte] = msg_transform_bytes[j + k + MSG_BLOCK_SIZE];
			//printf("%02x ", valid_bytes[index_byte]);
			index_byte++;
		}
		cast_uint8_array_to_uint64(&tmp, &valid_bytes[i * MSG_BLOCK_SIZE]);
		if (i < (cipher_block_length - 1) && tmp != count) {
			isValidDec = false;
		}
		if (i == (cipher_block_length - 1) && tmp != 0) {
			isValidDec = false;
		}
		count++;
	}

	free(cipher_blocks);
	free(cipher_pre_last_bytes);
	free(msg_transform_bytes);
	free(valid_bytes);
	free(msg_output);

	return;
}

void encrypt_8_byte(const unsigned char *message, uint64_t nonce[2], const unsigned char *helper, uint64_t helper_byte_length, unsigned char *cipher) 
{
	//printf("enc 8 here");
	uint64_t s0[2], s_last[2];
	calculate_s0(helper, helper_byte_length, nonce, s0);
	uint64_t msg_b1[2], msg_b2[2], msg_tho_b1[2], cipher_b1[2], msg_tho_b2[2], cipher_b2[2], msg_len_before[2], msg_len_after[2];

	cast_uint8_array_to_uint64(&msg_b1[0], message);
	msg_b1[1] = 1;

	s_last[0] = s0[0];
	s_last[1] = s0[1];
	tho_y(s0, msg_b1, msg_tho_b1);

	simon_encrypt(msg_tho_b1, cipher_b1, sk);

	msg_b2[0] = cipher_b1[1];
	msg_b2[1] = 0;

	msg_len_before[0] = 64;
	msg_len_before[1] = 0;

	simon_encrypt(msg_len_before, msg_len_after, sk);

	tho_y(s_last, msg_b2, msg_tho_b2);
	msg_tho_b2[0] ^= msg_len_after[0];
	msg_tho_b2[1] ^= msg_len_after[1];
	simon_encrypt(msg_tho_b2, cipher_b2, sk);

	//cipher = (unsigned char *)malloc(24 * sizeof(unsigned char));
	cast_uint64_to_uint8_array(cipher, cipher_b1[0]);
	cast_uint64_to_uint8_array(cipher + 8, cipher_b2[0]);
	cast_uint64_to_uint8_array(cipher + 16, cipher_b2[1]);


	return;
}

void decrypt_8_byte(const unsigned char *cipher, uint64_t nonce[2], const unsigned char *helper, uint64_t helper_byte_length, unsigned char *msg) 
{
	//printf("dec 8 here\n");
	uint64_t s0[2], s_last[2];
	calculate_s0(helper, helper_byte_length, nonce, s0);
	uint64_t msg_b1[2], msg_b2[2], cipher_b1[2], cipher_b2[2], msg_len_before[2], msg_len_after[2], cipher_b1_dec[2], cipher_b2_dec[2];

	cast_uint8_array_to_uint64(&cipher_b1[0], cipher);
	cast_uint8_array_to_uint64(&cipher_b2[0], cipher + 8);
	cast_uint8_array_to_uint64(&cipher_b2[1], cipher + 16);

	msg_len_before[0] = 64;
	msg_len_before[1] = 0;

	simon_encrypt(msg_len_before, msg_len_after, sk);

	simon_decrypt(cipher_b2, cipher_b2_dec, sk);
	cipher_b2_dec[0] ^= msg_len_after[0];
	cipher_b2_dec[1] ^= msg_len_after[1];

	tho_reverse_y(s0, cipher_b2_dec, msg_b2);
	s_last[0] = s0[0];
	s_last[1] = s0[1];
	cipher_b1[1] = msg_b2[0];

	simon_decrypt(cipher_b1, cipher_b1_dec, sk);
	tho_reverse_y(s_last, cipher_b1_dec, msg_b1);

	//msg = (unsigned char *)malloc(8 * sizeof(unsigned char));
	cast_uint64_to_uint8_array(msg, msg_b1[0]);
	
	if (msg_b1[1] != 1 || msg_b2[1] != 0) {
		isValidDec = false;
	}

	return;
}

void encrypt_16_byte(const unsigned char *message, uint64_t src_byte_length, uint64_t nonce[2],const unsigned char *helper, uint64_t helper_byte_length, unsigned char *cipher) {

	uint64_t s0[2];
	calculate_s0(helper, helper_byte_length, nonce, s0);

	uint64_t msg_left_byte = src_byte_length - MSG_BLOCK_SIZE;
	uint64_t msg_require_byte = MSG_BLOCK_SIZE - msg_left_byte;

	unsigned char *padding_byte = (unsigned char *)malloc(MSG_BLOCK_SIZE * sizeof(unsigned char));

	uint64_t msg_b1[2], msg_b2[2], msg_tho_b1[2], cipher_b1[2], msg_tho_b2[2], cipher_b2[2], msg_len_before[2], msg_len_after[2];
	cast_uint8_array_to_uint64(&msg_b1[0], message);
	msg_b1[1] = 1;

	tho_y(s0, msg_b1, msg_tho_b1);
	simon_encrypt(msg_tho_b1, cipher_b1, sk);

	cast_uint64_to_uint8_array(padding_byte, cipher_b1[1]);
	for (uint64_t i = 0; i < msg_left_byte; i++) {
		padding_byte[i] = message[8 + i];
	}
	cast_uint8_array_to_uint64(&msg_b2[0], padding_byte);

	msg_b2[1] = 0;

	msg_len_before[0] = src_byte_length * 8;
	msg_len_before[1] = 0;
	simon_encrypt(msg_len_before, msg_len_after, sk);
	tho_y(s0, msg_b2, msg_tho_b2);
	msg_tho_b2[0] ^= msg_len_after[0];
	msg_tho_b2[1] ^= msg_len_after[1];

	simon_encrypt(msg_tho_b2, cipher_b2, sk);
	unsigned char *ciphers = (unsigned char *)malloc(32 * sizeof(unsigned char));
	cast_uint64_to_uint8_array(ciphers, cipher_b1[0]);
	cast_uint64_to_uint8_array(ciphers + 8, cipher_b1[1]);
	cast_uint64_to_uint8_array(ciphers + 16, cipher_b2[0]);
	cast_uint64_to_uint8_array(ciphers + 24, cipher_b2[1]);

	//cipher = (unsigned char *)malloc((32 - msg_require_byte) * sizeof(unsigned char));
	int index = 0;
	for (uint64_t i = 0; i < 16 - msg_require_byte; i++) {
		cipher[index++] = ciphers[i];
	}
	for (uint64_t i = 16; i < 32; i++) {
		cipher[index++] = ciphers[i];
	}

	free(padding_byte);
	free(ciphers);

	return;
}

void decrypt_16_byte(const unsigned char *cipher, uint64_t cipher_byte_length, uint64_t nonce[2],const unsigned char *helper, uint64_t helper_byte_length, unsigned char *msg_output) 
{
	uint64_t s0[2];
	calculate_s0(helper, helper_byte_length, nonce, s0);

	uint64_t cipher_left_byte = cipher_byte_length - 16;
	uint64_t z_byte = 16 - cipher_left_byte;

	uint64_t msg_b1[2], msg_b2[2], msg_tho_b1[2], cipher_b1[2], msg_tho_b2[2], cipher_b2[2], msg_len_before[2], msg_len_after[2];
	cast_uint8_array_to_uint64(&cipher_b2[0], cipher + cipher_left_byte);
	cast_uint8_array_to_uint64(&cipher_b2[1], cipher + cipher_left_byte + 8);
	msg_len_before[0] = 8 * (16 - z_byte);
	msg_len_before[1] = 0;
	simon_encrypt(msg_len_before, msg_len_after, sk);
	simon_decrypt(cipher_b2, msg_tho_b2, sk);
	msg_tho_b2[0] ^= msg_len_after[0];
	msg_tho_b2[1] ^= msg_len_after[1];
	tho_reverse_y(s0, msg_tho_b2, msg_b2);

	unsigned char *cipher_byte2 = (unsigned char *)malloc(16 * sizeof(unsigned char));
	int index = 0;
	for (uint64_t i = 0; i < cipher_left_byte; i++) {
		cipher_byte2[index++] = cipher[i];
	}
	unsigned char *msg_byte2 = (unsigned char *)malloc(8 * sizeof(unsigned char));
	cast_uint64_to_uint8_array(msg_byte2, msg_b2[0]);
	for (uint64_t i = 8 - z_byte; i < 8; i++) {
		cipher_byte2[index++] = msg_byte2[i];
	}
	cast_uint8_array_to_uint64(&cipher_b1[0], cipher_byte2);
	cast_uint8_array_to_uint64(&cipher_b1[1], cipher_byte2 + 8);

	simon_decrypt(cipher_b1, msg_tho_b1, sk);
	tho_reverse_y(s0, msg_tho_b1, msg_b1);
	unsigned char *msgs = (unsigned char *)malloc(16 * sizeof(unsigned char));
	cast_uint64_to_uint8_array(msgs, msg_b1[0]);
	cast_uint64_to_uint8_array(msgs + 8, msg_b2[0]);

	if (msg_b1[1] != 1 || msg_b2[1] != 0) {
		isValidDec = false;
	}

	//msg_output = (unsigned char *)malloc((16 - z_byte) * sizeof(unsigned char));
	for (uint64_t i = 0; i < 16 - z_byte; i++) {
		msg_output[i] = msgs[i];
	}

	free(cipher_byte2);
	free(msg_byte2);
	free(msgs);
	return;
}

void encrypt_short_byte(const unsigned char *message, uint64_t src_byte_length, uint64_t nonce[2], const unsigned char *helper, uint64_t helper_byte_length, unsigned char *cipher) {

	uint64_t s0[2];
	calculate_s0(helper, helper_byte_length, nonce, s0);

	uint64_t z_byte = 16 - src_byte_length;

	uint64_t msg_b1[2], msg_b2[2], msg_tho_b1[2], cipher_b1[2], msg_tho_b2[2], cipher_b2[2], msg_len_before[2], msg_len_after[2];
	unsigned char *padding_msg = (unsigned char *)malloc(8 * sizeof(unsigned char));
	for (uint64_t i = 0; i < src_byte_length; i++) 
	{
		padding_msg[i] = message[i];
	}
	for (uint64_t i = src_byte_length; i < 8; i++) 
	{
		padding_msg[i] = 0x00;
	}
	cast_uint8_array_to_uint64(&msg_b1[0], padding_msg);
	msg_b1[1] = 1;

	tho_y(s0, msg_b1, msg_tho_b1);
	simon_encrypt(msg_tho_b1, cipher_b1, sk);

	unsigned char *cipher_b1_trans = (unsigned char *)malloc(16 * sizeof(unsigned char));
	cast_uint64_to_uint8_array(cipher_b1_trans, cipher_b1[0]);
	cast_uint64_to_uint8_array(cipher_b1_trans + 8, cipher_b1[1]);
	unsigned char *msg_byte2 = (unsigned char *)malloc(16 * sizeof(unsigned char));
	for (uint64_t i = 0; i < z_byte; i++) 
	{
		msg_byte2[i] = cipher_b1_trans[16 - z_byte + i];
	}
	for (uint64_t i = z_byte; i < 16; i++) 
	{
		msg_byte2[i] = 0x00;
	}
	cast_uint8_array_to_uint64(&msg_b2[0], msg_byte2);
	cast_uint8_array_to_uint64(&msg_b2[1], msg_byte2 + 8);

	tho_y(s0, msg_b2, msg_tho_b2);

	msg_len_before[1] = src_byte_length * 8;
	msg_len_before[0] = 0;
	simon_encrypt(msg_len_before, msg_len_after, sk);
	msg_tho_b2[0] ^= msg_len_after[0];
	msg_tho_b2[1] ^= msg_len_after[1];

	simon_encrypt(msg_tho_b2, cipher_b2, sk);
	unsigned char *ciphers = (unsigned char *)malloc(32 * sizeof(unsigned char));
	cast_uint64_to_uint8_array(ciphers, cipher_b1[0]);
	cast_uint64_to_uint8_array(ciphers + 8, cipher_b1[1]);
	cast_uint64_to_uint8_array(ciphers + 16, cipher_b2[0]);
	cast_uint64_to_uint8_array(ciphers + 24, cipher_b2[1]);

	//cipher = (unsigned char *)malloc((32 - z_byte) * sizeof(unsigned char));
	int index = 0;
	for (uint64_t i = 0; i < 16 - z_byte; i++) {
		cipher[index++] = ciphers[i];
	}
	for (uint64_t i = 16; i < 32; i++) {
		cipher[index++] = ciphers[i];
	}
	free(padding_msg);
	free(cipher_b1_trans);
	free(msg_byte2);
	free(ciphers);

	return;
}

void decrypt_short_byte(const unsigned char *cipher, uint64_t cipher_byte_length, uint64_t nonce[2], const unsigned char *helper, uint64_t helper_byte_length, unsigned char *msg_output) {

	uint64_t s0[2];
	calculate_s0(helper, helper_byte_length, nonce, s0);

	uint64_t cipher_left_byte = cipher_byte_length - 16;
	uint64_t z_byte = 16 - cipher_left_byte;

	uint64_t msg_b1[2], msg_b2[2], msg_tho_b1[2], cipher_b1[2], msg_tho_b2[2], cipher_b2[2], msg_len_before[2], msg_len_after[2];

	cast_uint8_array_to_uint64(&cipher_b2[0], cipher + cipher_left_byte);
	cast_uint8_array_to_uint64(&cipher_b2[1], cipher + cipher_left_byte + 8);

	msg_len_before[1] = 8 * cipher_left_byte;
	msg_len_before[0] = 0;
	simon_encrypt(msg_len_before, msg_len_after, sk);
	simon_decrypt(cipher_b2, msg_tho_b2, sk);
	msg_tho_b2[0] ^= msg_len_after[0];
	msg_tho_b2[1] ^= msg_len_after[1];
	tho_reverse_y(s0, msg_tho_b2, msg_b2);

	unsigned char *cipher_byte, *msg_byte;

	cipher_byte = (unsigned char *)malloc(16 * sizeof(unsigned char));
	msg_byte = (unsigned char *)malloc(16 * sizeof(unsigned char));

	cast_uint64_to_uint8_array(msg_byte, msg_b2[0]);
	cast_uint64_to_uint8_array(msg_byte + 8, msg_b2[1]);

	for (uint64_t i = z_byte; i < 16; i++) 
	{
		if (msg_byte[i] != 0) {
			isValidDec = false;
			return;
		}
	}

	int index = 0;
	for (uint64_t i = 0; i < cipher_left_byte; i++) {
		cipher_byte[index++] = cipher[i];
	}
	for (uint64_t i = 0; i < z_byte; i++) {
		cipher_byte[index++] = msg_byte[i];
	}
	cast_uint8_array_to_uint64(&cipher_b1[0], cipher_byte);
	cast_uint8_array_to_uint64(&cipher_b1[1], cipher_byte + 8);
	simon_decrypt(cipher_b1, msg_tho_b1, sk);
	tho_reverse_y(s0, msg_tho_b1, msg_b1);

	if (msg_b1[1] != 1) {
		isValidDec = false;
		return;
	}

	unsigned char *msgs = (unsigned char *)malloc(8 * sizeof(unsigned char));
	cast_uint64_to_uint8_array(msgs, msg_b1[0]);
	//msg_output = (unsigned char *)malloc(cipher_left_byte * sizeof(unsigned char));
	for (uint64_t i = 0; i < cipher_left_byte; i++) {
		msg_output[i] = msgs[i];
	}
	for (uint64_t i = cipher_left_byte; i < MSG_BLOCK_SIZE; i++) {
		if (msgs[i] != 0) {
			isValidDec = false;
			return;
		}
	}
	free(cipher_byte);
	free(msg_byte);
	free(msgs);

	return;
}

int generate_keys(const unsigned char *k, uint64_t k_byte_length) 
{
	uint64_t *key;

	if (k_byte_length == 16) 
	{
		key = (uint64_t *)malloc(2 * sizeof(uint64_t));
		if (ROUNDS != 68 || KEY_WORDS != 2) 
		{
			return -1;
		}
	}
	else if (k_byte_length == 24) {
		key = (uint64_t *)malloc(3 * sizeof(uint64_t));
		if (ROUNDS != 69 || KEY_WORDS != 3) {
			return -1;
		}
	}
	else {
		key = (uint64_t *)malloc(4 * sizeof(uint64_t));
		if (ROUNDS != 72 || KEY_WORDS != 4) {
			return -1;
		}
	}

	for (uint64_t i = 0; i < KEY_WORDS; i++) {
		cast_uint8_array_to_uint64(&key[i], k + i * 8);
	}

	key_schedule(key, sk);
	free(key);
	return 0;
}

int crypto_aead_encrypt(
	unsigned char *c, unsigned long long *clen,
	const unsigned char *m, unsigned long long mlen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
)
{
	if(nsec == NULL)
	{
	    ;
	}	
	uint64_t k_byte_length = CRYPTO_KEYBYTES;
	if (CRYPTO_KEYBYTES != 16 && CRYPTO_KEYBYTES != 32 && CRYPTO_KEYBYTES != 24)
	{
		return -1;	//key length is wrong
	}

	int gen = generate_keys(k, k_byte_length);
	if (gen == -1)
	{
		return -2;	//key schedule fails
	}

	uint64_t nonce[2];
	cast_uint8_array_to_uint64(&nonce[0], npub);
	cast_uint8_array_to_uint64(&nonce[1], npub + 8);

	if (mlen == 0) 
	{
		*clen = 0;
		return 0;
	}

	if (mlen < 8) 
	{
		unsigned char *cipher = (unsigned char *)malloc((mlen + 16) * sizeof(unsigned char));
		encrypt_short_byte(m, mlen, nonce, ad, adlen, cipher);
		*clen = mlen + 16;
		for (uint64_t i = 0; i < *clen; i++) 
		{
			c[i] = cipher[i];
		}
		free(cipher);
		return 0;
	}
	if (mlen == 8) 
	{
		unsigned char *cipher = (unsigned char *)malloc((mlen + 16) * sizeof(unsigned char));
		encrypt_8_byte(m, nonce, ad, adlen, cipher);
		*clen = mlen + 16;
		for (uint64_t i = 0; i < *clen; i++) {
			c[i] = cipher[i];
		}
		free(cipher);
		return 0;
	}
	if (mlen <= 16) 
	{
		unsigned char *cipher = (unsigned char *)malloc((mlen + 16) * sizeof(unsigned char));
		encrypt_16_byte(m, mlen, nonce, ad, adlen, cipher);
		*clen = mlen + 16;
		for (uint64_t i = 0; i < *clen; i++) {
			c[i] = cipher[i];
		}
		free(cipher);
		return 0;
	}
	if (mlen % MSG_BLOCK_SIZE == 0) 
	{
		unsigned char *cipher = (unsigned char *)malloc((mlen * 2) * sizeof(unsigned char));
		ae_encrypt_byte_nopad(m, mlen, nonce, ad, adlen, cipher);
		*clen = 2 * mlen;
		for (uint64_t i = 0; i < *clen; i++) {
			c[i] = cipher[i];
		}
		free(cipher);
		return 0;
	}

	*clen = (mlen / 8) * 16 + (mlen % 8) + 8;
	unsigned char *cipher = (unsigned char *)malloc((*clen) * sizeof(unsigned char));
	ae_encrypt_byte(m, mlen, nonce, ad, adlen, cipher);
	for (uint64_t i = 0; i < *clen; i++) {
		c[i] = cipher[i];
	}
	free(cipher);
	return 0;
}


int crypto_aead_decrypt(
	unsigned char *m, unsigned long long *mlen,
	unsigned char *nsec,
	const unsigned char *c, unsigned long long clen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *npub,
	const unsigned char *k
)
{
	if(nsec == NULL)
	{
	    ;
	}	
	isValidDec = true;
	uint64_t k_byte_length = CRYPTO_KEYBYTES;
	unsigned char *msg;
	if (k_byte_length != 16 && k_byte_length != 32 && k_byte_length != 24)
	{
		return -2;
	}

	int gen = generate_keys(k, k_byte_length);

	if (gen == -1)
	{
		return -3;
	}

	uint64_t nonce[2];
	cast_uint8_array_to_uint64(&nonce[0], npub);
	cast_uint8_array_to_uint64(&nonce[1], npub + 8);

	if(clen == 0)
	{
		*mlen = 0;
		return 0;
	}

	uint64_t z_byte_length, cipher_block;
	z_byte_length = (BYTE_SIZE_PER_BLOCK - clen % BYTE_SIZE_PER_BLOCK) % BYTE_SIZE_PER_BLOCK;
	cipher_block = (uint64_t)(clen / BYTE_SIZE_PER_BLOCK) + (clen % BYTE_SIZE_PER_BLOCK > 0 ? 1 : 0);

	if (clen % BYTE_SIZE_PER_BLOCK > 0 && z_byte_length > 8) 
	{
		*mlen = clen % BYTE_SIZE_PER_BLOCK;
		msg = (unsigned char *)malloc((*mlen) * sizeof(unsigned char));
		decrypt_short_byte(c, clen, nonce, ad, adlen, msg);
		if (!isValidDec) 
		{
			free(msg);
			return -1;
		}
		for (uint64_t i = 0; i < *mlen; i++) 
		{
			m[i] = msg[i];
		}
		free(msg);
		return 0;
	}

	*mlen = clen - cipher_block * MSG_BLOCK_SIZE;

	if (*mlen == 8) 
	{
		msg = (unsigned char *)malloc(8 * sizeof(unsigned char));
		decrypt_8_byte(c, nonce, ad, adlen, msg);
		if (!isValidDec) 
		{
			free(msg);
			return -1;
		}
		for (uint64_t i = 0; i < *mlen; i++) {
			m[i] = msg[i];
		}
		free(msg);
		return 0;
	}

	if (*mlen <= 16) 
	{
		msg = (unsigned char *)malloc((*mlen) * sizeof(unsigned char));
		decrypt_16_byte(c, clen, nonce, ad, adlen, msg);
		if (!isValidDec) 
		{
			free(msg);
			return -1;
		}
		for (uint64_t i = 0; i < *mlen; i++) {
			m[i] = msg[i];
		}
		free(msg);
		return 0;
	}

	if (*mlen % MSG_BLOCK_SIZE == 0) 
	{
		msg = (unsigned char *)malloc((*mlen) * sizeof(unsigned char));
		ae_decrypt_byte_nopad(c, clen, nonce, ad, adlen, msg);
		if (!isValidDec) 
		{
			free(msg);
			return -1;
		}
		for (uint64_t i = 0; i < *mlen; i++) {
			m[i] = msg[i];
		}
		free(msg);
		return 0;
	}

	msg = (unsigned char *)malloc((*mlen) * sizeof(unsigned char));
	ae_decrypt_byte(c, clen, nonce, ad, adlen, msg);
	if (!isValidDec) 
	{
		free(msg);
		return -1;
	}
	for (uint64_t i = 0; i < *mlen; i++) 
	{
		m[i] = msg[i];
	}
	free(msg);
	return 0;
}
