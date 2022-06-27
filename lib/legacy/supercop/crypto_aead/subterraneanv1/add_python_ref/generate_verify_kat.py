#!/usr/bin/python2
# -*- coding: utf-8 -*-

from subterranean_bit import *

import binascii

def init_buffer(number_bytes):
    value = bytearray(number_bytes)
    for i in range(number_bytes):
        value[i] = i % 256
    return value
    
def generate_hash_test(test_file_name = "LWC_HASH_KAT_256_p.txt", number_of_tests = 1024, hash_size_bytes = 32):
    out_file = open(test_file_name, 'w')
    messages = init_buffer(number_of_tests)
    for count in range(number_of_tests+1):
        hash_out = crypto_hash(messages[:count], hash_size_bytes)
        out_file.write("Count = " + str(count) + '\n')
        out_file.write("Msg = " + (binascii.hexlify(messages[:count])).upper() + '\n')
        out_file.write("MD = " + (binascii.hexlify(hash_out)).upper() + '\n')
        out_file.write("\n")
    out_file.close()

def verify_hash_test(test_file_name = "LWC_HASH_KAT_256_p.txt", hash_size_bytes = 32):
    read_file = open(test_file_name, 'r')
    current_line = read_file.readline()
    while(current_line != ''):
        count_str = (current_line.split('=')[1]).strip()
        count = int(count_str)
        current_line = read_file.readline()
        message_str = (current_line.split('=')[1]).strip()
        message = bytearray.fromhex(message_str)
        current_line = read_file.readline()
        expected_hash_str = (current_line.split('=')[1]).strip()
        expected_hash = bytearray.fromhex(expected_hash_str)
        hash_out = crypto_hash(message, hash_size_bytes)
        if(hash_out != expected_hash):
            print("Count = " + str(count) + '\n')
            print("Msg = " + message_str + '\n')
            print("MD = " + expected_hash_str + '\n')
            print("MD = " + (binascii.hexlify(hash_out)).upper() + '\n')
            print("\n")
            break
        current_line = read_file.readline() # There is one blank line between tests
        current_line = read_file.readline()
    read_file.close()

def generate_aead_test(test_file_name = "LWC_AEAD_KAT_128_128_p.txt", number_of_tests_m = 32, number_of_tests_ad = 32, tag_bytes = 16, nonce_bytes = 16, key_bytes = 16):
    out_file = open(test_file_name, 'w')
    messages = init_buffer(number_of_tests_m)
    associated_datas = init_buffer(number_of_tests_ad)
    nonce = init_buffer(nonce_bytes)
    key = init_buffer(key_bytes)
    count = 0
    for i in range(number_of_tests_m+1):
        for j in range(number_of_tests_ad+1):
            ciphertext = crypto_aead_encrypt(messages[:i], associated_datas[:j], None, nonce, key, tag_bytes)
            out_file.write("Count = " + str(count) + '\n')
            out_file.write("Key = " + (binascii.hexlify(key)).upper() + '\n')
            out_file.write("Nonce = " + (binascii.hexlify(nonce)).upper() + '\n')
            out_file.write("PT = " + (binascii.hexlify(messages[:i])).upper() + '\n')
            out_file.write("AD = " + (binascii.hexlify(associated_datas[:j])).upper() + '\n')
            out_file.write("CT = " + (binascii.hexlify(ciphertext)).upper() + '\n')
            out_file.write("\n")
            count += 1
    out_file.close()

def verify_aead_test(test_file_name = "LWC_AEAD_KAT_128_128_p.txt", tag_bytes = 16):
    read_file = open(test_file_name, 'r')
    current_line = read_file.readline()
    while(current_line != ''):
        count_str = (current_line.split('=')[1]).strip()
        count = int(count_str)
        current_line = read_file.readline()
        key_str = (current_line.split('=')[1]).strip()
        key = bytearray.fromhex(key_str)
        current_line = read_file.readline()
        nonce_str = (current_line.split('=')[1]).strip()
        nonce = bytearray.fromhex(nonce_str)
        current_line = read_file.readline()
        message_str = (current_line.split('=')[1]).strip()
        message = bytearray.fromhex(message_str)
        current_line = read_file.readline()
        associated_data_str = (current_line.split('=')[1]).strip()
        associated_data = bytearray.fromhex(associated_data_str)
        current_line = read_file.readline()
        ciphertext_str = (current_line.split('=')[1]).strip()
        ciphertext = bytearray.fromhex(ciphertext_str)
        message = crypto_aead_decrypt(ciphertext, associated_data, None, nonce, key, tag_bytes)
        if(message == None):
            print("Count = " + str(count) + '\n')
            print("Key = " + key_str + '\n')
            print("Nonce = " + nonce_str + '\n')
            print("PT = " + message_str + '\n')
            print("AD = " + associated_data_str + '\n')
            print("CT = " + ciphertext_str + '\n')
            print("\n")
            break
        current_line = read_file.readline() # There is one blank line between tests
        current_line = read_file.readline()
    read_file.close()

#generate_hash_test()
verify_hash_test(test_file_name = "LWC_HASH_KAT_256.txt")
#generate_aead_test()
verify_aead_test(test_file_name = "LWC_AEAD_KAT_128_128.txt")