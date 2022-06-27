/*=============================================================================
This file has been adapted from the implementation 
(available at, Public Domain https://github.com/KULeuven-COSIC/SABER) 
of "Saber: Module-LWR based key exchange, CPA-secure encryption and CCA-secure KEM"
by : Jan-Pieter D'Anvers, Angshuman Karmakar, Sujoy Sinha Roy, and Frederik Vercauteren
Jose Maria Bermudo Mera, Michiel Van Beirendonck, Andrea Basso. 

 * Copyright (c) 2020 by Cryptographic Engineering Research Group (CERG)
 * ECE Department, George Mason University
 * Fairfax, VA, U.S.A.
 * Author: Duc Tri Nguyen
=============================================================================*/

#ifndef INDCPA_H
#define INDCPA_H

#include "poly.h"

void indcpa_keypair(unsigned char *pk, unsigned char *sk);

void GenMatrix(polyvec *a, const unsigned char *seed);

void indcpa_client(unsigned char *pk, unsigned char *b_prime, unsigned char *c, unsigned char *key);

void indcpa_server(unsigned char *pk, unsigned char *b_prime, unsigned char *c, unsigned char *key);

void indcpa_kem_keypair(unsigned char *pk, unsigned char *sk);
void indcpa_kem_enc(unsigned char *message, unsigned char *noiseseed, const unsigned char *pk,
                    unsigned char *ciphertext);
void indcpa_kem_dec(const unsigned char *sk, const unsigned char *ciphertext,
                    unsigned char *message_dec);

#endif
