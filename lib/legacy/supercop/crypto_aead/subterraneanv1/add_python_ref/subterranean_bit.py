#!/usr/bin/python2
# -*- coding: utf-8 -*-

SUBTERRANEAN_SIZE = 257

def bits_to_hex_string(in_bits):
    string_state = ""
    if(len(in_bits)%8 == 0):
        final_position = len(in_bits) - 8
    else:
        final_position = len(in_bits) - (len(in_bits)%8)
    for i in range(0, final_position, 8):
        temp_value = in_bits[i] + 2*in_bits[i+1] + 4*in_bits[i+2] + 8*in_bits[i+3] + 16*in_bits[i+4] + 32*in_bits[i+5] + 64*in_bits[i+6] + 128*in_bits[i+7]
        string_state = string_state + "{0:02x}".format(temp_value)
    mult_factor = 1
    temp_value = 0
    for i in range(final_position, len(in_bits)):
        temp_value += mult_factor*in_bits[i]
        mult_factor *= 2
    string_state = string_state + "{0:02x}".format(temp_value)
    return string_state

def bytearray_to_bits(value):
    value_bits = [((int(value[i//8]) & (1 << (i%8))) >> (i%8)) for i in range(len(value)*8)]
    return value_bits

def bits_to_bytearray(value):
    value_bytearray = bytearray((len(value)+7)//8)
    if(len(value) != 0):
        if(len(value)%8 == 0):
            final_position = len(value) - 8
        else:
            final_position = len(value) - (len(value)%8)
        j = 0
        for i in range(0, final_position, 8):
            value_bytearray[j] = value[i] + 2*value[i+1] + 4*value[i+2] + 8*value[i+3] + 16*value[i+4] + 32*value[i+5] + 64*value[i+6] + 128*value[i+7]
            j += 1
        mult_factor = 1
        value_bytearray[j] = 0
        for i in range(final_position, len(value)):
            value_bytearray[j] += mult_factor*value[i]
            mult_factor *= 2
    return value_bytearray

def subterranean_round(state_bits):
    # Chi step
    new_state_bits = [state_bits[i] ^ ((1 ^ state_bits[(i+1) % SUBTERRANEAN_SIZE]) & state_bits[(i+2) % SUBTERRANEAN_SIZE]) for i in range(SUBTERRANEAN_SIZE)]
    # Iota step
    new_state_bits[0] ^= 1
    # Theta step
    new_state_bits = [new_state_bits[i] ^ new_state_bits[(i+3) % SUBTERRANEAN_SIZE] ^ new_state_bits[(i+8) % SUBTERRANEAN_SIZE] for i in range(SUBTERRANEAN_SIZE)]
    # Pi step
    new_state_bits = [new_state_bits[(12*i) % SUBTERRANEAN_SIZE] for i in range(SUBTERRANEAN_SIZE)]
    return new_state_bits

def subterranean_init():
    state = [0 for i in range(SUBTERRANEAN_SIZE)]
    return state

def subterranean_duplex(state, sigma):
    # s <= R(s)
    new_state = subterranean_round(state)
    # sbar <= sbar + sigma
    index_j = 1;
    for i in range(len(sigma)):
        new_state[index_j] ^= sigma[i]
        index_j = (176*index_j) % SUBTERRANEAN_SIZE
    # sbar <= sbar + (1||0*)
    new_state[index_j] ^= 1
    return new_state

def subterranean_extract(state):
    value_out = [0 for i in range(32)]
    # value_out <= extract
    index_j = 1;
    for i in range(32):
        value_out[i] = state[index_j] ^ state[SUBTERRANEAN_SIZE-index_j]
        index_j = (176*index_j) % SUBTERRANEAN_SIZE
    return value_out

def subterranean_absorb_unkeyed(state, value_in):
    new_state = [state[i] for i in range(len(state))]
    i = 0
    while(i < len(value_in) - 7):
        new_state = subterranean_duplex(new_state, value_in[i:i+8])
        new_state = subterranean_duplex(new_state, [])
        i += 8
    new_state = subterranean_duplex(new_state, value_in[i:])
    new_state = subterranean_duplex(new_state, [])
    return new_state

def subterranean_absorb_keyed(state, value_in):
    new_state = [state[i] for i in range(len(state))]
    i = 0
    while(i < len(value_in) - 31):
        new_state = subterranean_duplex(new_state, value_in[i:i+32])
        i += 32
    new_state = subterranean_duplex(new_state, value_in[i:])
    return new_state

def subterranean_absorb_encrypt(state, value_in):
    new_state = [state[i] for i in range(len(state))]
    # Y <= *
    value_out = []
    i = 0
    while(i < len(value_in) - 31):
        temp_value_state = subterranean_extract(new_state)
        temp_value_out = [value_in[i+j] ^ temp_value_state[j] for j in range(32)]
        new_state = subterranean_duplex(new_state, value_in[i:i+32])
        value_out = value_out + temp_value_out
        i += 32
    temp_value_state = subterranean_extract(new_state)
    temp_value_out = [value_in[i+j] ^ temp_value_state[j] for j in range(len(value_in)-i)]
    new_state = subterranean_duplex(new_state, value_in[i:])
    value_out = value_out + temp_value_out
    return new_state, value_out

def subterranean_absorb_decrypt(state, value_in):
    new_state = [state[i] for i in range(len(state))]
    # Y <= *
    value_out = []
    i = 0
    while(i < len(value_in) - 31):
        temp_value_state = subterranean_extract(new_state)
        temp_value_out = [value_in[i+j] ^ temp_value_state[j] for j in range(32)]
        new_state = subterranean_duplex(new_state, temp_value_out)
        value_out = value_out + temp_value_out
        i += 32
    temp_value_state = subterranean_extract(new_state)
    temp_value_out = [value_in[i+j] ^ temp_value_state[j] for j in range(len(value_in)-i)]
    new_state = subterranean_duplex(new_state, temp_value_out)
    value_out = value_out + temp_value_out
    return new_state, value_out

def subterranean_blank(state, r_calls):
    new_state = [state[i] for i in range(len(state))]
    for i in range(r_calls):
        new_state = subterranean_duplex(new_state, [])
    return new_state

def subterranean_squeeze(state, size_l):
    new_state = [state[i] for i in range(len(state))]
    # Z <= *
    Z = []
    i = 0
    while(i < size_l - 32):
        temp_value_out = subterranean_extract(new_state)
        new_state = subterranean_duplex(new_state, [])
        Z = Z + temp_value_out
        i += 32
    temp_value_out = subterranean_extract(new_state)
    new_state = subterranean_duplex(new_state, [])
    Z = Z + temp_value_out[:size_l]
    return new_state, Z

def subterranean_xof_init():
    # S <= Subterranean()
    state = subterranean_init()
    return state

def subterranean_xof_update(state, message):
    new_state = subterranean_absorb_unkeyed(state, message)
    return new_state
    
def subterranean_xof_finalize(state, size_l):
    # S.blank(8)
    new_state = subterranean_blank(state, 8)
    # Z <= S.squeeze(l)
    new_state, z = subterranean_squeeze(new_state, size_l)
    return new_state, z
    
def subterranean_xof_direct(message, size_l):
    # S <= Subterranean()
    state = subterranean_init()
    # Only one single message to absorb
    
    state = subterranean_absorb_unkeyed(state, message)
    # S.blank(8)
    state = subterranean_blank(state, 8)
    # Z <= S.squeeze(l)
    state, z = subterranean_squeeze(state, size_l)
    return z

def subterranean_deck_init(key):
    # S <= Subterranean()
    state = subterranean_init()
    # S.absorb(K,MAC)
    state = subterranean_absorb_keyed(state, key)
    return state

def subterranean_deck_update(state, message):
    new_state = subterranean_absorb_keyed(state, message)
    return new_state
    
def subterranean_deck_finalize(state, size_l):
    # S.blank(8)
    new_state = subterranean_blank(state, 8)
    # Z <= S.squeeze(l)
    new_state, z = subterranean_squeeze(new_state, size_l)
    return new_state, z

def subterranean_deck_direct(key, message, size_l):
    # S <= Subterranean()
    state = subterranean_init()
    # S.absorb(K,MAC)
    state = subterranean_absorb_keyed(key)
    # Only one single message to absorb
    state = subterranean_absorb_keyed(state, message)
    # S.blank(8)
    state = subterranean_blank(state, 8)
    # Z <= S.squeeze(l)
    z = subterranean_squeeze(state, size_l)
    return z
    
def subterranean_SAE_start(key, nonce):
    # S <= Subterranean()
    state = subterranean_init()
    # S.absorb(K)
    state = subterranean_absorb_keyed(state, key)
    # S.absorb(N)
    state = subterranean_absorb_keyed(state, nonce)
    # S.blank(8)
    state = subterranean_blank(state, 8)
    return state

def subterranean_SAE_wrap_encrypt(state, associated_data, text, tag_length):
    # S.absorb(A,MAC)
    new_state = subterranean_absorb_keyed(state, associated_data)
    # Y <= S.absorb(X,op)
    new_state, new_text = subterranean_absorb_encrypt(new_state, text)
    # S.blank(8)
    new_state = subterranean_blank(new_state, 8)
    # T <= S.squeeze(tau)
    new_state, new_tag = subterranean_squeeze(new_state, tag_length)
    return new_state, new_text, new_tag

def subterranean_SAE_wrap_decrypt(state, associated_data, text, tag, tag_length):
    # S.absorb(A,MAC)
    new_state = subterranean_absorb_keyed(state, associated_data)
    # Y <= S.absorb(X,op)
    new_state, new_text = subterranean_absorb_decrypt(new_state, text)
    # S.blank(8)
    new_state = subterranean_blank(new_state, 8)
    # T <= S.squeeze(tau)
    new_state, new_tag = subterranean_squeeze(new_state, tag_length)
    # if op = decrypt AND (tag != new_tag) then (Y,T) = (*,*)
    if(tag != new_tag):
        new_text = []
        new_tag = []
    return new_state, new_text, new_tag

def subterranean_SAE_direct_encrypt(key, nonce, associated_data, text, tag_length):
    # S <= Subterranean()
    state = subterranean_init()
    # S.absorb(K)
    state = subterranean_absorb_keyed(state, key)
    # S.absorb(N)
    state = subterranean_absorb_keyed(state, nonce)
    # S.blank(8)
    state = subterranean_blank(state, 8)
    # S.absorb(A,MAC)
    state = subterranean_absorb_keyed(state, associated_data)
    # Y <= S.absorb(X,op)
    state, new_text = subterranean_absorb_encrypt(state, text)
    # S.blank(8)
    state = subterranean_blank(state, 8)
    # T <= S.squeeze(tau)
    state, new_tag = subterranean_squeeze(state, tag_length)
    return new_text, new_tag

def subterranean_SAE_direct_decrypt(key, nonce, associated_data, text, tag, tag_length):
    # S <= Subterranean()
    state = subterranean_init()
    # S.absorb(K)
    state = subterranean_absorb_keyed(state, key)
    # S.absorb(N)
    state = subterranean_absorb_keyed(state, nonce)
    # S.blank(8)
    state = subterranean_blank(state, 8)
    # S.absorb(A,MAC)
    state = subterranean_absorb_keyed(state, associated_data)
    # Y <= S.absorb(X,op)
    state, new_text = subterranean_absorb_decrypt(state, text)
    # S.blank(8)
    state = subterranean_blank(state, 8)
    # T <= S.squeeze(tau)
    state, new_tag = subterranean_squeeze(state, tag_length)
    # if op = decrypt AND (tag != new_tag) then (Y,T) = (*,*)
    if(tag != new_tag):
        print(tag)
        print(new_tag)
        new_text = []
        new_tag = []
    return new_text, new_tag

def crypto_hash(m, out_length_bytes):
    m_bits = bytearray_to_bits(m)
    hash_bits = subterranean_xof_direct(m_bits, out_length_bytes*8)
    hash_out = bits_to_bytearray(hash_bits)
    return hash_out

def crypto_aead_encrypt(m, ad, nsec, npub, k, t_length_bytes):
    m_bits = bytearray_to_bits(m)
    ad_bits = bytearray_to_bits(ad)
    npub_bits = bytearray_to_bits(npub)
    k_bits = bytearray_to_bits(k)
    c_bits, t_bits = subterranean_SAE_direct_encrypt(k_bits, npub_bits, ad_bits, m_bits, t_length_bytes*8)
    joined_c_bits = c_bits + t_bits
    joined_c = bits_to_bytearray(joined_c_bits)
    return joined_c

def crypto_aead_decrypt(c, ad, nsec, npub, k, t_length_bytes):
    joined_c_bits = bytearray_to_bits(c)
    c_bits = joined_c_bits[:len(joined_c_bits)-t_length_bytes*8]
    t_prime_bits = joined_c_bits[len(joined_c_bits)-t_length_bytes*8:]
    ad_bits = bytearray_to_bits(ad)
    npub_bits = bytearray_to_bits(npub)
    k_bits = bytearray_to_bits(k)
    m_bits, t_bits = subterranean_SAE_direct_decrypt(k_bits, npub_bits, ad_bits, c_bits, t_prime_bits, t_length_bytes*8)
    if(t_bits == []):
        return None
    m = bits_to_bytearray(m_bits)
    return m