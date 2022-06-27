#!/usr/bin/python2
# -*- coding: utf-8 -*-

SUBTERRANEAN_IO_BITS = [((176**i) % 257) for i in range(0, 33)]

def print_previous_pi_round_io():
    temp_str = ''
    for i, position in enumerate(SUBTERRANEAN_IO_BITS):
        index = 12*position % 257
        temp_str = "assign " + "a_after_theta" + "[" + str(index) + "]" + " = "
        temp_str = temp_str + "a_after_chi_iota" + "[" + str(index) + "]" + " ^ "
        temp_str = temp_str + "a_after_chi_iota" + "[" + str((index + 3)%257) + "]" + " ^ "
        temp_str = temp_str + "a_after_chi_iota" + "[" + str((index + 8)%257) + "]" + " ^ "
        temp_str = temp_str + "i" + "[" + str(i) + "]" + ";"
        print(temp_str)

def print_previous_pi_round_empty():
    temp_str = ''
    for position in range(256):
        if (not (position in SUBTERRANEAN_IO_BITS)):
            index = 12*position % 257
            temp_str = "assign " + "a_after_theta" + "[" + str(index) + "]" + " = "
            temp_str = temp_str + "a_after_chi_iota" + "[" + str(index) + "]" + " ^ "
            temp_str = temp_str + "a_after_chi_iota" + "[" + str((index + 3)%257) + "]" + " ^ "
            temp_str = temp_str + "a_after_chi_iota" + "[" + str((index + 8)%257) + "]" + ";"
            print(temp_str)

def print_buffer_out_round():
    temp_str = ''
    for i, position in enumerate(SUBTERRANEAN_IO_BITS):
        index = position
        temp_str = "assign " + "state_with_buffer_in_for_buffer_out" + "[" + str(i) + "]" + " = "
        temp_str = temp_str + "(" + "permutation_state" + "[" + str(index) + "]" + " ^ "
        temp_str = temp_str + "permutation_state" + "[" + str(257-index) + "]" + ")" + " ^ "
        temp_str = temp_str + "(" + "buffer_in" + "[" + str(i) + "]" + " & "
        temp_str = temp_str + "buffer_in_mask_for_buffer_out" + "[" + str(i) + "]" + ")" + ";"
        print(temp_str)

print_previous_pi_round_io()
print('')
print_previous_pi_round_empty()
print('')
print('')
print_buffer_out_round()