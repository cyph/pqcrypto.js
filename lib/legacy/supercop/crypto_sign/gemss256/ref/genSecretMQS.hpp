#ifndef _GENSECRETMQS_HPP
#define _GENSECRETMQS_HPP

#include "config_HFE.h"

#include <NTL/mat_GF2E.h>
#include <NTL/GF2EX.h>

using namespace NTL;

void genSecretMQS(mat_GF2E& Q, const GF2EX& F, const UINT* F2);

#endif

