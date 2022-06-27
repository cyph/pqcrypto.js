/**
 * \file lrpc.h
 * \brief Fonctions to decode the error support using LRPC codes
 *
 */

#ifndef RBC_LRPC_H
#define RBC_LRPC_H

#include "rbc_vspace.h"
#include "rbc_qre.h"

unsigned int rbc_lrpc_RSR(rbc_vspace E, const uint32_t E_expected_dim, const rbc_vspace F, const uint32_t F_dim, const rbc_qre ec, const uint32_t ec_size);
unsigned int rbc_lrpc_RSR_constant_time(rbc_vspace E, const uint32_t E_expected_dim, const rbc_vspace F, const uint32_t F_dim, const rbc_qre ec, const uint32_t ec_size);

#endif

