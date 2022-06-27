#ifndef _GENSECRETMQS_GF2_H
#define _GENSECRETMQS_GF2_H

#include "prefix_name.h"
#include "MQS_gf2.h"
#include "gf2nx.h"


int PREFIX_NAME(genSecretMQS_gf2)(mqsnv_gf2n MQS_res, cst_sparse_monic_gf2nx F);
#define genSecretMQS_gf2 PREFIX_NAME(genSecretMQS_gf2)

#endif

