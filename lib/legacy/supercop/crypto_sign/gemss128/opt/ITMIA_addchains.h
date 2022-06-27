#ifndef _ITMIA_ADDCHAINS_H
#define _ITMIA_ADDCHAINS_H


/* This file contains the addition chains for ITMIA.
    We read the bits of n-1 from higher bits to lower bits.
    The chain always starts with {1,2}. */


#include "parameters_HFE.h"

#if(HFEn==3)
    #define ITOH3U
    #define P_ITOH3U
    #define VP_ITOH3U
    #define PMS_ITOH3U
#elif(HFEn==4)
    #define M_ITOH4(V,MS) \
        V##ODD_INIT;

    #define ITOH4U M_ITOH4(,)
    #define P_ITOH4U M_ITOH4(P,)
    #define VP_ITOH4U M_ITOH4(VP,)
    #define PMS_ITOH4U M_ITOH4(P,_MULTISQR)
#elif(HFEn==5)
    #define M_ITOH5(V,MS)\
        V##EVEN_CASE(2);

    #define ITOH5U M_ITOH5(,)
    #define P_ITOH5U M_ITOH5(P,)
    #define VP_ITOH5U M_ITOH5(VP,)
    #define PMS_ITOH5U M_ITOH5(P,_MULTISQR)
#elif(HFEn==6)
    #define M_ITOH6(V,MS)\
        V##ODD_CASE(2);

    #define ITOH6U M_ITOH6(,)
    #define P_ITOH6U M_ITOH6(P,)
    #define VP_ITOH6U M_ITOH6(VP,)
    #define PMS_ITOH6U M_ITOH6(P,_MULTISQR)
#elif(HFEn==7)
    #define M_ITOH7(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);

    #define ITOH7U M_ITOH7(,)
    #define P_ITOH7U M_ITOH7(P,)
    #define VP_ITOH7U M_ITOH7(VP,)
    #define PMS_ITOH7U M_ITOH7(P,_MULTISQR)
#elif(HFEn==8)
    #define M_ITOH8(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);

    #define ITOH8U M_ITOH8(,)
    #define P_ITOH8U M_ITOH8(P,)
    #define VP_ITOH8U M_ITOH8(VP,)
    #define PMS_ITOH8U M_ITOH8(P,_MULTISQR)
#elif(HFEn==9)
    #define M_ITOH9(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);

    #define ITOH9U M_ITOH9(,)
    #define P_ITOH9U M_ITOH9(P,)
    #define VP_ITOH9U M_ITOH9(VP,)
    #define PMS_ITOH9U M_ITOH9(P,_MULTISQR)
#elif(HFEn==10)
    #define M_ITOH10(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);

    #define ITOH10U M_ITOH10(,)
    #define P_ITOH10U M_ITOH10(P,)
    #define VP_ITOH10U M_ITOH10(VP,)
    #define PMS_ITOH10U M_ITOH10(P,_MULTISQR)
#elif(HFEn==11)
    #define M_ITOH11(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);

    #define ITOH11U M_ITOH11(,)
    #define P_ITOH11U M_ITOH11(P,)
    #define VP_ITOH11U M_ITOH11(VP,)
    #define PMS_ITOH11U M_ITOH11(P,_MULTISQR)
#elif(HFEn==12)
    #define M_ITOH12(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);

    #define ITOH12U M_ITOH12(,)
    #define P_ITOH12U M_ITOH12(P,)
    #define VP_ITOH12U M_ITOH12(VP,)
    #define PMS_ITOH12U M_ITOH12(P,_MULTISQR)
#elif(HFEn==13)
    #define M_ITOH13(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);

    #define ITOH13U M_ITOH13(,)
    #define P_ITOH13U M_ITOH13(P,)
    #define VP_ITOH13U M_ITOH13(VP,)
    #define PMS_ITOH13U M_ITOH13(P,_MULTISQR)
#elif(HFEn==14)
    #define M_ITOH14(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);

    #define ITOH14U M_ITOH14(,)
    #define P_ITOH14U M_ITOH14(P,)
    #define VP_ITOH14U M_ITOH14(VP,)
    #define PMS_ITOH14U M_ITOH14(P,_MULTISQR)
#elif(HFEn==15)
    #define M_ITOH15(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);

    #define ITOH15U M_ITOH15(,)
    #define P_ITOH15U M_ITOH15(P,)
    #define VP_ITOH15U M_ITOH15(VP,)
    #define PMS_ITOH15U M_ITOH15(P,_MULTISQR)
#elif(HFEn==16)
    #define M_ITOH16(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);

    #define ITOH16U M_ITOH16(,)
    #define P_ITOH16U M_ITOH16(P,)
    #define VP_ITOH16U M_ITOH16(VP,)
    #define PMS_ITOH16U M_ITOH16(P,_MULTISQR)
#elif(HFEn==17)
    #define M_ITOH17(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);

    #define ITOH17U M_ITOH17(,)
    #define P_ITOH17U M_ITOH17(P,)
    #define VP_ITOH17U M_ITOH17(VP,)
    #define PMS_ITOH17U M_ITOH17(P,_MULTISQR)
#elif(HFEn==18)
    #define M_ITOH18(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);

    #define ITOH18U M_ITOH18(,)
    #define P_ITOH18U M_ITOH18(P,)
    #define VP_ITOH18U M_ITOH18(VP,)
    #define PMS_ITOH18U M_ITOH18(P,_MULTISQR)
#elif(HFEn==19)
    #define M_ITOH19(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);

    #define ITOH19U M_ITOH19(,)
    #define P_ITOH19U M_ITOH19(P,)
    #define VP_ITOH19U M_ITOH19(VP,)
    #define PMS_ITOH19U M_ITOH19(P,_MULTISQR)
#elif(HFEn==20)
    #define M_ITOH20(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);

    #define ITOH20U M_ITOH20(,)
    #define P_ITOH20U M_ITOH20(P,)
    #define VP_ITOH20U M_ITOH20(VP,)
    #define PMS_ITOH20U M_ITOH20(P,_MULTISQR)
#elif(HFEn==21)
    #define M_ITOH21(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);

    #define ITOH21U M_ITOH21(,)
    #define P_ITOH21U M_ITOH21(P,)
    #define VP_ITOH21U M_ITOH21(VP,)
    #define PMS_ITOH21U M_ITOH21(P,_MULTISQR)
#elif(HFEn==22)
    #define M_ITOH22(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);

    #define ITOH22U M_ITOH22(,)
    #define P_ITOH22U M_ITOH22(P,)
    #define VP_ITOH22U M_ITOH22(VP,)
    #define PMS_ITOH22U M_ITOH22(P,_MULTISQR)
#elif(HFEn==23)
    #define M_ITOH23(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);

    #define ITOH23U M_ITOH23(,)
    #define P_ITOH23U M_ITOH23(P,)
    #define VP_ITOH23U M_ITOH23(VP,)
    #define PMS_ITOH23U M_ITOH23(P,_MULTISQR)
#elif(HFEn==24)
    #define M_ITOH24(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);

    #define ITOH24U M_ITOH24(,)
    #define P_ITOH24U M_ITOH24(P,)
    #define VP_ITOH24U M_ITOH24(VP,)
    #define PMS_ITOH24U M_ITOH24(P,_MULTISQR)
#elif(HFEn==25)
    #define M_ITOH25(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);

    #define ITOH25U M_ITOH25(,)
    #define P_ITOH25U M_ITOH25(P,)
    #define VP_ITOH25U M_ITOH25(VP,)
    #define PMS_ITOH25U M_ITOH25(P,_MULTISQR)
#elif(HFEn==26)
    #define M_ITOH26(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);

    #define ITOH26U M_ITOH26(,)
    #define P_ITOH26U M_ITOH26(P,)
    #define VP_ITOH26U M_ITOH26(VP,)
    #define PMS_ITOH26U M_ITOH26(P,_MULTISQR)
#elif(HFEn==27)
    #define M_ITOH27(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);

    #define ITOH27U M_ITOH27(,)
    #define P_ITOH27U M_ITOH27(P,)
    #define VP_ITOH27U M_ITOH27(VP,)
    #define PMS_ITOH27U M_ITOH27(P,_MULTISQR)
#elif(HFEn==28)
    #define M_ITOH28(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);

    #define ITOH28U M_ITOH28(,)
    #define P_ITOH28U M_ITOH28(P,)
    #define VP_ITOH28U M_ITOH28(VP,)
    #define PMS_ITOH28U M_ITOH28(P,_MULTISQR)
#elif(HFEn==29)
    #define M_ITOH29(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);

    #define ITOH29U M_ITOH29(,)
    #define P_ITOH29U M_ITOH29(P,)
    #define VP_ITOH29U M_ITOH29(VP,)
    #define PMS_ITOH29U M_ITOH29(P,_MULTISQR)
#elif(HFEn==30)
    #define M_ITOH30(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);

    #define ITOH30U M_ITOH30(,)
    #define P_ITOH30U M_ITOH30(P,)
    #define VP_ITOH30U M_ITOH30(VP,)
    #define PMS_ITOH30U M_ITOH30(P,_MULTISQR)
#elif(HFEn==31)
    #define M_ITOH31(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);

    #define ITOH31U M_ITOH31(,)
    #define P_ITOH31U M_ITOH31(P,)
    #define VP_ITOH31U M_ITOH31(VP,)
    #define PMS_ITOH31U M_ITOH31(P,_MULTISQR)
#elif(HFEn==32)
    #define M_ITOH32(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);

    #define ITOH32U M_ITOH32(,)
    #define P_ITOH32U M_ITOH32(P,)
    #define VP_ITOH32U M_ITOH32(VP,)
    #define PMS_ITOH32U M_ITOH32(P,_MULTISQR)
#elif(HFEn==33)
    #define M_ITOH33(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);

    #define ITOH33U M_ITOH33(,)
    #define P_ITOH33U M_ITOH33(P,)
    #define VP_ITOH33U M_ITOH33(VP,)
    #define PMS_ITOH33U M_ITOH33(P,_MULTISQR)
#elif(HFEn==34)
    #define M_ITOH34(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);

    #define ITOH34U M_ITOH34(,)
    #define P_ITOH34U M_ITOH34(P,)
    #define VP_ITOH34U M_ITOH34(VP,)
    #define PMS_ITOH34U M_ITOH34(P,_MULTISQR)
#elif(HFEn==35)
    #define M_ITOH35(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);

    #define ITOH35U M_ITOH35(,)
    #define P_ITOH35U M_ITOH35(P,)
    #define VP_ITOH35U M_ITOH35(VP,)
    #define PMS_ITOH35U M_ITOH35(P,_MULTISQR)
#elif(HFEn==36)
    #define M_ITOH36(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);

    #define ITOH36U M_ITOH36(,)
    #define P_ITOH36U M_ITOH36(P,)
    #define VP_ITOH36U M_ITOH36(VP,)
    #define PMS_ITOH36U M_ITOH36(P,_MULTISQR)
#elif(HFEn==37)
    #define M_ITOH37(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);

    #define ITOH37U M_ITOH37(,)
    #define P_ITOH37U M_ITOH37(P,)
    #define VP_ITOH37U M_ITOH37(VP,)
    #define PMS_ITOH37U M_ITOH37(P,_MULTISQR)
#elif(HFEn==38)
    #define M_ITOH38(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);

    #define ITOH38U M_ITOH38(,)
    #define P_ITOH38U M_ITOH38(P,)
    #define VP_ITOH38U M_ITOH38(VP,)
    #define PMS_ITOH38U M_ITOH38(P,_MULTISQR)
#elif(HFEn==39)
    #define M_ITOH39(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);

    #define ITOH39U M_ITOH39(,)
    #define P_ITOH39U M_ITOH39(P,)
    #define VP_ITOH39U M_ITOH39(VP,)
    #define PMS_ITOH39U M_ITOH39(P,_MULTISQR)
#elif(HFEn==40)
    #define M_ITOH40(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);

    #define ITOH40U M_ITOH40(,)
    #define P_ITOH40U M_ITOH40(P,)
    #define VP_ITOH40U M_ITOH40(VP,)
    #define PMS_ITOH40U M_ITOH40(P,_MULTISQR)
#elif(HFEn==41)
    #define M_ITOH41(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);

    #define ITOH41U M_ITOH41(,)
    #define P_ITOH41U M_ITOH41(P,)
    #define VP_ITOH41U M_ITOH41(VP,)
    #define PMS_ITOH41U M_ITOH41(P,_MULTISQR)
#elif(HFEn==42)
    #define M_ITOH42(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);

    #define ITOH42U M_ITOH42(,)
    #define P_ITOH42U M_ITOH42(P,)
    #define VP_ITOH42U M_ITOH42(VP,)
    #define PMS_ITOH42U M_ITOH42(P,_MULTISQR)
#elif(HFEn==43)
    #define M_ITOH43(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);

    #define ITOH43U M_ITOH43(,)
    #define P_ITOH43U M_ITOH43(P,)
    #define VP_ITOH43U M_ITOH43(VP,)
    #define PMS_ITOH43U M_ITOH43(P,_MULTISQR)
#elif(HFEn==44)
    #define M_ITOH44(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);

    #define ITOH44U M_ITOH44(,)
    #define P_ITOH44U M_ITOH44(P,)
    #define VP_ITOH44U M_ITOH44(VP,)
    #define PMS_ITOH44U M_ITOH44(P,_MULTISQR)
#elif(HFEn==45)
    #define M_ITOH45(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);

    #define ITOH45U M_ITOH45(,)
    #define P_ITOH45U M_ITOH45(P,)
    #define VP_ITOH45U M_ITOH45(VP,)
    #define PMS_ITOH45U M_ITOH45(P,_MULTISQR)
#elif(HFEn==46)
    #define M_ITOH46(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);

    #define ITOH46U M_ITOH46(,)
    #define P_ITOH46U M_ITOH46(P,)
    #define VP_ITOH46U M_ITOH46(VP,)
    #define PMS_ITOH46U M_ITOH46(P,_MULTISQR)
#elif(HFEn==47)
    #define M_ITOH47(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);

    #define ITOH47U M_ITOH47(,)
    #define P_ITOH47U M_ITOH47(P,)
    #define VP_ITOH47U M_ITOH47(VP,)
    #define PMS_ITOH47U M_ITOH47(P,_MULTISQR)
#elif(HFEn==48)
    #define M_ITOH48(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);

    #define ITOH48U M_ITOH48(,)
    #define P_ITOH48U M_ITOH48(P,)
    #define VP_ITOH48U M_ITOH48(VP,)
    #define PMS_ITOH48U M_ITOH48(P,_MULTISQR)
#elif(HFEn==49)
    #define M_ITOH49(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);

    #define ITOH49U M_ITOH49(,)
    #define P_ITOH49U M_ITOH49(P,)
    #define VP_ITOH49U M_ITOH49(VP,)
    #define PMS_ITOH49U M_ITOH49(P,_MULTISQR)
#elif(HFEn==50)
    #define M_ITOH50(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);

    #define ITOH50U M_ITOH50(,)
    #define P_ITOH50U M_ITOH50(P,)
    #define VP_ITOH50U M_ITOH50(VP,)
    #define PMS_ITOH50U M_ITOH50(P,_MULTISQR)
#elif(HFEn==51)
    #define M_ITOH51(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);

    #define ITOH51U M_ITOH51(,)
    #define P_ITOH51U M_ITOH51(P,)
    #define VP_ITOH51U M_ITOH51(VP,)
    #define PMS_ITOH51U M_ITOH51(P,_MULTISQR)
#elif(HFEn==52)
    #define M_ITOH52(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);

    #define ITOH52U M_ITOH52(,)
    #define P_ITOH52U M_ITOH52(P,)
    #define VP_ITOH52U M_ITOH52(VP,)
    #define PMS_ITOH52U M_ITOH52(P,_MULTISQR)
#elif(HFEn==53)
    #define M_ITOH53(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);

    #define ITOH53U M_ITOH53(,)
    #define P_ITOH53U M_ITOH53(P,)
    #define VP_ITOH53U M_ITOH53(VP,)
    #define PMS_ITOH53U M_ITOH53(P,_MULTISQR)
#elif(HFEn==54)
    #define M_ITOH54(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);

    #define ITOH54U M_ITOH54(,)
    #define P_ITOH54U M_ITOH54(P,)
    #define VP_ITOH54U M_ITOH54(VP,)
    #define PMS_ITOH54U M_ITOH54(P,_MULTISQR)
#elif(HFEn==55)
    #define M_ITOH55(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);

    #define ITOH55U M_ITOH55(,)
    #define P_ITOH55U M_ITOH55(P,)
    #define VP_ITOH55U M_ITOH55(VP,)
    #define PMS_ITOH55U M_ITOH55(P,_MULTISQR)
#elif(HFEn==56)
    #define M_ITOH56(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);

    #define ITOH56U M_ITOH56(,)
    #define P_ITOH56U M_ITOH56(P,)
    #define VP_ITOH56U M_ITOH56(VP,)
    #define PMS_ITOH56U M_ITOH56(P,_MULTISQR)
#elif(HFEn==57)
    #define M_ITOH57(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);

    #define ITOH57U M_ITOH57(,)
    #define P_ITOH57U M_ITOH57(P,)
    #define VP_ITOH57U M_ITOH57(VP,)
    #define PMS_ITOH57U M_ITOH57(P,_MULTISQR)
#elif(HFEn==58)
    #define M_ITOH58(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);

    #define ITOH58U M_ITOH58(,)
    #define P_ITOH58U M_ITOH58(P,)
    #define VP_ITOH58U M_ITOH58(VP,)
    #define PMS_ITOH58U M_ITOH58(P,_MULTISQR)
#elif(HFEn==59)
    #define M_ITOH59(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);

    #define ITOH59U M_ITOH59(,)
    #define P_ITOH59U M_ITOH59(P,)
    #define VP_ITOH59U M_ITOH59(VP,)
    #define PMS_ITOH59U M_ITOH59(P,_MULTISQR)
#elif(HFEn==60)
    #define M_ITOH60(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);

    #define ITOH60U M_ITOH60(,)
    #define P_ITOH60U M_ITOH60(P,)
    #define VP_ITOH60U M_ITOH60(VP,)
    #define PMS_ITOH60U M_ITOH60(P,_MULTISQR)
#elif(HFEn==61)
    #define M_ITOH61(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);

    #define ITOH61U M_ITOH61(,)
    #define P_ITOH61U M_ITOH61(P,)
    #define VP_ITOH61U M_ITOH61(VP,)
    #define PMS_ITOH61U M_ITOH61(P,_MULTISQR)
#elif(HFEn==62)
    #define M_ITOH62(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);

    #define ITOH62U M_ITOH62(,)
    #define P_ITOH62U M_ITOH62(P,)
    #define VP_ITOH62U M_ITOH62(VP,)
    #define PMS_ITOH62U M_ITOH62(P,_MULTISQR)
#elif(HFEn==63)
    #define M_ITOH63(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);

    #define ITOH63U M_ITOH63(,)
    #define P_ITOH63U M_ITOH63(P,)
    #define VP_ITOH63U M_ITOH63(VP,)
    #define PMS_ITOH63U M_ITOH63(P,_MULTISQR)
#elif(HFEn==64)
    #define M_ITOH64(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);

    #define ITOH64U M_ITOH64(,)
    #define P_ITOH64U M_ITOH64(P,)
    #define VP_ITOH64U M_ITOH64(VP,)
    #define PMS_ITOH64U M_ITOH64(P,_MULTISQR)
#elif(HFEn==65)
    #define M_ITOH65(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);

    #define ITOH65U M_ITOH65(,)
    #define P_ITOH65U M_ITOH65(P,)
    #define VP_ITOH65U M_ITOH65(VP,)
    #define PMS_ITOH65U M_ITOH65(P,_MULTISQR)
#elif(HFEn==66)
    #define M_ITOH66(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);

    #define ITOH66U M_ITOH66(,)
    #define P_ITOH66U M_ITOH66(P,)
    #define VP_ITOH66U M_ITOH66(VP,)
    #define PMS_ITOH66U M_ITOH66(P,_MULTISQR)
#elif(HFEn==67)
    #define M_ITOH67(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);

    #define ITOH67U M_ITOH67(,)
    #define P_ITOH67U M_ITOH67(P,)
    #define VP_ITOH67U M_ITOH67(VP,)
    #define PMS_ITOH67U M_ITOH67(P,_MULTISQR)
#elif(HFEn==68)
    #define M_ITOH68(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);

    #define ITOH68U M_ITOH68(,)
    #define P_ITOH68U M_ITOH68(P,)
    #define VP_ITOH68U M_ITOH68(VP,)
    #define PMS_ITOH68U M_ITOH68(P,_MULTISQR)
#elif(HFEn==69)
    #define M_ITOH69(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);

    #define ITOH69U M_ITOH69(,)
    #define P_ITOH69U M_ITOH69(P,)
    #define VP_ITOH69U M_ITOH69(VP,)
    #define PMS_ITOH69U M_ITOH69(P,_MULTISQR)
#elif(HFEn==70)
    #define M_ITOH70(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);

    #define ITOH70U M_ITOH70(,)
    #define P_ITOH70U M_ITOH70(P,)
    #define VP_ITOH70U M_ITOH70(VP,)
    #define PMS_ITOH70U M_ITOH70(P,_MULTISQR)
#elif(HFEn==71)
    #define M_ITOH71(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);

    #define ITOH71U M_ITOH71(,)
    #define P_ITOH71U M_ITOH71(P,)
    #define VP_ITOH71U M_ITOH71(VP,)
    #define PMS_ITOH71U M_ITOH71(P,_MULTISQR)
#elif(HFEn==72)
    #define M_ITOH72(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);

    #define ITOH72U M_ITOH72(,)
    #define P_ITOH72U M_ITOH72(P,)
    #define VP_ITOH72U M_ITOH72(VP,)
    #define PMS_ITOH72U M_ITOH72(P,_MULTISQR)
#elif(HFEn==73)
    #define M_ITOH73(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##EVEN_CASE(36);

    #define ITOH73U M_ITOH73(,)
    #define P_ITOH73U M_ITOH73(P,)
    #define VP_ITOH73U M_ITOH73(VP,)
    #define PMS_ITOH73U M_ITOH73(P,_MULTISQR)
#elif(HFEn==74)
    #define M_ITOH74(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##ODD_CASE(36);

    #define ITOH74U M_ITOH74(,)
    #define P_ITOH74U M_ITOH74(P,)
    #define VP_ITOH74U M_ITOH74(VP,)
    #define PMS_ITOH74U M_ITOH74(P,_MULTISQR)
#elif(HFEn==75)
    #define M_ITOH75(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##EVEN_CASE(37);

    #define ITOH75U M_ITOH75(,)
    #define P_ITOH75U M_ITOH75(P,)
    #define VP_ITOH75U M_ITOH75(VP,)
    #define PMS_ITOH75U M_ITOH75(P,_MULTISQR)
#elif(HFEn==76)
    #define M_ITOH76(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##ODD_CASE(37);

    #define ITOH76U M_ITOH76(,)
    #define P_ITOH76U M_ITOH76(P,)
    #define VP_ITOH76U M_ITOH76(VP,)
    #define PMS_ITOH76U M_ITOH76(P,_MULTISQR)
#elif(HFEn==77)
    #define M_ITOH77(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##EVEN_CASE(38);

    #define ITOH77U M_ITOH77(,)
    #define P_ITOH77U M_ITOH77(P,)
    #define VP_ITOH77U M_ITOH77(VP,)
    #define PMS_ITOH77U M_ITOH77(P,_MULTISQR)
#elif(HFEn==78)
    #define M_ITOH78(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##ODD_CASE(38);

    #define ITOH78U M_ITOH78(,)
    #define P_ITOH78U M_ITOH78(P,)
    #define VP_ITOH78U M_ITOH78(VP,)
    #define PMS_ITOH78U M_ITOH78(P,_MULTISQR)
#elif(HFEn==79)
    #define M_ITOH79(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##EVEN_CASE(39);

    #define ITOH79U M_ITOH79(,)
    #define P_ITOH79U M_ITOH79(P,)
    #define VP_ITOH79U M_ITOH79(VP,)
    #define PMS_ITOH79U M_ITOH79(P,_MULTISQR)
#elif(HFEn==80)
    #define M_ITOH80(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##ODD_CASE(39);

    #define ITOH80U M_ITOH80(,)
    #define P_ITOH80U M_ITOH80(P,)
    #define VP_ITOH80U M_ITOH80(VP,)
    #define PMS_ITOH80U M_ITOH80(P,_MULTISQR)
#elif(HFEn==81)
    #define M_ITOH81(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##EVEN_CASE(40);

    #define ITOH81U M_ITOH81(,)
    #define P_ITOH81U M_ITOH81(P,)
    #define VP_ITOH81U M_ITOH81(VP,)
    #define PMS_ITOH81U M_ITOH81(P,_MULTISQR)
#elif(HFEn==82)
    #define M_ITOH82(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##ODD_CASE(40);

    #define ITOH82U M_ITOH82(,)
    #define P_ITOH82U M_ITOH82(P,)
    #define VP_ITOH82U M_ITOH82(VP,)
    #define PMS_ITOH82U M_ITOH82(P,_MULTISQR)
#elif(HFEn==83)
    #define M_ITOH83(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##EVEN_CASE(41);

    #define ITOH83U M_ITOH83(,)
    #define P_ITOH83U M_ITOH83(P,)
    #define VP_ITOH83U M_ITOH83(VP,)
    #define PMS_ITOH83U M_ITOH83(P,_MULTISQR)
#elif(HFEn==84)
    #define M_ITOH84(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##ODD_CASE(41);

    #define ITOH84U M_ITOH84(,)
    #define P_ITOH84U M_ITOH84(P,)
    #define VP_ITOH84U M_ITOH84(VP,)
    #define PMS_ITOH84U M_ITOH84(P,_MULTISQR)
#elif(HFEn==85)
    #define M_ITOH85(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##EVEN_CASE(42);

    #define ITOH85U M_ITOH85(,)
    #define P_ITOH85U M_ITOH85(P,)
    #define VP_ITOH85U M_ITOH85(VP,)
    #define PMS_ITOH85U M_ITOH85(P,_MULTISQR)
#elif(HFEn==86)
    #define M_ITOH86(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##ODD_CASE(42);

    #define ITOH86U M_ITOH86(,)
    #define P_ITOH86U M_ITOH86(P,)
    #define VP_ITOH86U M_ITOH86(VP,)
    #define PMS_ITOH86U M_ITOH86(P,_MULTISQR)
#elif(HFEn==87)
    #define M_ITOH87(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##EVEN_CASE(43);

    #define ITOH87U M_ITOH87(,)
    #define P_ITOH87U M_ITOH87(P,)
    #define VP_ITOH87U M_ITOH87(VP,)
    #define PMS_ITOH87U M_ITOH87(P,_MULTISQR)
#elif(HFEn==88)
    #define M_ITOH88(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##ODD_CASE(43);

    #define ITOH88U M_ITOH88(,)
    #define P_ITOH88U M_ITOH88(P,)
    #define VP_ITOH88U M_ITOH88(VP,)
    #define PMS_ITOH88U M_ITOH88(P,_MULTISQR)
#elif(HFEn==89)
    #define M_ITOH89(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##EVEN_CASE(44);

    #define ITOH89U M_ITOH89(,)
    #define P_ITOH89U M_ITOH89(P,)
    #define VP_ITOH89U M_ITOH89(VP,)
    #define PMS_ITOH89U M_ITOH89(P,_MULTISQR)
#elif(HFEn==90)
    #define M_ITOH90(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##ODD_CASE(44);

    #define ITOH90U M_ITOH90(,)
    #define P_ITOH90U M_ITOH90(P,)
    #define VP_ITOH90U M_ITOH90(VP,)
    #define PMS_ITOH90U M_ITOH90(P,_MULTISQR)
#elif(HFEn==91)
    #define M_ITOH91(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##EVEN_CASE(45);

    #define ITOH91U M_ITOH91(,)
    #define P_ITOH91U M_ITOH91(P,)
    #define VP_ITOH91U M_ITOH91(VP,)
    #define PMS_ITOH91U M_ITOH91(P,_MULTISQR)
#elif(HFEn==92)
    #define M_ITOH92(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##ODD_CASE(45);

    #define ITOH92U M_ITOH92(,)
    #define P_ITOH92U M_ITOH92(P,)
    #define VP_ITOH92U M_ITOH92(VP,)
    #define PMS_ITOH92U M_ITOH92(P,_MULTISQR)
#elif(HFEn==93)
    #define M_ITOH93(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##EVEN_CASE(46);

    #define ITOH93U M_ITOH93(,)
    #define P_ITOH93U M_ITOH93(P,)
    #define VP_ITOH93U M_ITOH93(VP,)
    #define PMS_ITOH93U M_ITOH93(P,_MULTISQR)
#elif(HFEn==94)
    #define M_ITOH94(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##ODD_CASE(46);

    #define ITOH94U M_ITOH94(,)
    #define P_ITOH94U M_ITOH94(P,)
    #define VP_ITOH94U M_ITOH94(VP,)
    #define PMS_ITOH94U M_ITOH94(P,_MULTISQR)
#elif(HFEn==95)
    #define M_ITOH95(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##EVEN_CASE(47);

    #define ITOH95U M_ITOH95(,)
    #define P_ITOH95U M_ITOH95(P,)
    #define VP_ITOH95U M_ITOH95(VP,)
    #define PMS_ITOH95U M_ITOH95(P,_MULTISQR)
#elif(HFEn==96)
    #define M_ITOH96(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##ODD_CASE(47);

    #define ITOH96U M_ITOH96(,)
    #define P_ITOH96U M_ITOH96(P,)
    #define VP_ITOH96U M_ITOH96(VP,)
    #define PMS_ITOH96U M_ITOH96(P,_MULTISQR)
#elif(HFEn==97)
    #define M_ITOH97(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##EVEN_CASE(48);

    #define ITOH97U M_ITOH97(,)
    #define P_ITOH97U M_ITOH97(P,)
    #define VP_ITOH97U M_ITOH97(VP,)
    #define PMS_ITOH97U M_ITOH97(P,_MULTISQR)
#elif(HFEn==98)
    #define M_ITOH98(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##ODD_CASE(48);

    #define ITOH98U M_ITOH98(,)
    #define P_ITOH98U M_ITOH98(P,)
    #define VP_ITOH98U M_ITOH98(VP,)
    #define PMS_ITOH98U M_ITOH98(P,_MULTISQR)
#elif(HFEn==99)
    #define M_ITOH99(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##EVEN_CASE(49);

    #define ITOH99U M_ITOH99(,)
    #define P_ITOH99U M_ITOH99(P,)
    #define VP_ITOH99U M_ITOH99(VP,)
    #define PMS_ITOH99U M_ITOH99(P,_MULTISQR)
#elif(HFEn==100)
    #define M_ITOH100(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##ODD_CASE(49);

    #define ITOH100U M_ITOH100(,)
    #define P_ITOH100U M_ITOH100(P,)
    #define VP_ITOH100U M_ITOH100(VP,)
    #define PMS_ITOH100U M_ITOH100(P,_MULTISQR)
#elif(HFEn==101)
    #define M_ITOH101(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##EVEN_CASE(50);

    #define ITOH101U M_ITOH101(,)
    #define P_ITOH101U M_ITOH101(P,)
    #define VP_ITOH101U M_ITOH101(VP,)
    #define PMS_ITOH101U M_ITOH101(P,_MULTISQR)
#elif(HFEn==102)
    #define M_ITOH102(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##ODD_CASE(50);

    #define ITOH102U M_ITOH102(,)
    #define P_ITOH102U M_ITOH102(P,)
    #define VP_ITOH102U M_ITOH102(VP,)
    #define PMS_ITOH102U M_ITOH102(P,_MULTISQR)
#elif(HFEn==103)
    #define M_ITOH103(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##EVEN_CASE(51);

    #define ITOH103U M_ITOH103(,)
    #define P_ITOH103U M_ITOH103(P,)
    #define VP_ITOH103U M_ITOH103(VP,)
    #define PMS_ITOH103U M_ITOH103(P,_MULTISQR)
#elif(HFEn==104)
    #define M_ITOH104(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##ODD_CASE(51);

    #define ITOH104U M_ITOH104(,)
    #define P_ITOH104U M_ITOH104(P,)
    #define VP_ITOH104U M_ITOH104(VP,)
    #define PMS_ITOH104U M_ITOH104(P,_MULTISQR)
#elif(HFEn==105)
    #define M_ITOH105(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##EVEN_CASE(52);

    #define ITOH105U M_ITOH105(,)
    #define P_ITOH105U M_ITOH105(P,)
    #define VP_ITOH105U M_ITOH105(VP,)
    #define PMS_ITOH105U M_ITOH105(P,_MULTISQR)
#elif(HFEn==106)
    #define M_ITOH106(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##ODD_CASE(52);

    #define ITOH106U M_ITOH106(,)
    #define P_ITOH106U M_ITOH106(P,)
    #define VP_ITOH106U M_ITOH106(VP,)
    #define PMS_ITOH106U M_ITOH106(P,_MULTISQR)
#elif(HFEn==107)
    #define M_ITOH107(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##EVEN_CASE(53);

    #define ITOH107U M_ITOH107(,)
    #define P_ITOH107U M_ITOH107(P,)
    #define VP_ITOH107U M_ITOH107(VP,)
    #define PMS_ITOH107U M_ITOH107(P,_MULTISQR)
#elif(HFEn==108)
    #define M_ITOH108(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##ODD_CASE(53);

    #define ITOH108U M_ITOH108(,)
    #define P_ITOH108U M_ITOH108(P,)
    #define VP_ITOH108U M_ITOH108(VP,)
    #define PMS_ITOH108U M_ITOH108(P,_MULTISQR)
#elif(HFEn==109)
    #define M_ITOH109(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##EVEN_CASE(54);

    #define ITOH109U M_ITOH109(,)
    #define P_ITOH109U M_ITOH109(P,)
    #define VP_ITOH109U M_ITOH109(VP,)
    #define PMS_ITOH109U M_ITOH109(P,_MULTISQR)
#elif(HFEn==110)
    #define M_ITOH110(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##ODD_CASE(54);

    #define ITOH110U M_ITOH110(,)
    #define P_ITOH110U M_ITOH110(P,)
    #define VP_ITOH110U M_ITOH110(VP,)
    #define PMS_ITOH110U M_ITOH110(P,_MULTISQR)
#elif(HFEn==111)
    #define M_ITOH111(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##EVEN_CASE(55);

    #define ITOH111U M_ITOH111(,)
    #define P_ITOH111U M_ITOH111(P,)
    #define VP_ITOH111U M_ITOH111(VP,)
    #define PMS_ITOH111U M_ITOH111(P,_MULTISQR)
#elif(HFEn==112)
    #define M_ITOH112(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##ODD_CASE(55);

    #define ITOH112U M_ITOH112(,)
    #define P_ITOH112U M_ITOH112(P,)
    #define VP_ITOH112U M_ITOH112(VP,)
    #define PMS_ITOH112U M_ITOH112(P,_MULTISQR)
#elif(HFEn==113)
    #define M_ITOH113(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##EVEN_CASE(56);

    #define ITOH113U M_ITOH113(,)
    #define P_ITOH113U M_ITOH113(P,)
    #define VP_ITOH113U M_ITOH113(VP,)
    #define PMS_ITOH113U M_ITOH113(P,_MULTISQR)
#elif(HFEn==114)
    #define M_ITOH114(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##ODD_CASE(56);

    #define ITOH114U M_ITOH114(,)
    #define P_ITOH114U M_ITOH114(P,)
    #define VP_ITOH114U M_ITOH114(VP,)
    #define PMS_ITOH114U M_ITOH114(P,_MULTISQR)
#elif(HFEn==115)
    #define M_ITOH115(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##EVEN_CASE(57);

    #define ITOH115U M_ITOH115(,)
    #define P_ITOH115U M_ITOH115(P,)
    #define VP_ITOH115U M_ITOH115(VP,)
    #define PMS_ITOH115U M_ITOH115(P,_MULTISQR)
#elif(HFEn==116)
    #define M_ITOH116(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##ODD_CASE(57);

    #define ITOH116U M_ITOH116(,)
    #define P_ITOH116U M_ITOH116(P,)
    #define VP_ITOH116U M_ITOH116(VP,)
    #define PMS_ITOH116U M_ITOH116(P,_MULTISQR)
#elif(HFEn==117)
    #define M_ITOH117(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##EVEN_CASE(58);

    #define ITOH117U M_ITOH117(,)
    #define P_ITOH117U M_ITOH117(P,)
    #define VP_ITOH117U M_ITOH117(VP,)
    #define PMS_ITOH117U M_ITOH117(P,_MULTISQR)
#elif(HFEn==118)
    #define M_ITOH118(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##ODD_CASE(58);

    #define ITOH118U M_ITOH118(,)
    #define P_ITOH118U M_ITOH118(P,)
    #define VP_ITOH118U M_ITOH118(VP,)
    #define PMS_ITOH118U M_ITOH118(P,_MULTISQR)
#elif(HFEn==119)
    #define M_ITOH119(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##EVEN_CASE(59);

    #define ITOH119U M_ITOH119(,)
    #define P_ITOH119U M_ITOH119(P,)
    #define VP_ITOH119U M_ITOH119(VP,)
    #define PMS_ITOH119U M_ITOH119(P,_MULTISQR)
#elif(HFEn==120)
    #define M_ITOH120(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##ODD_CASE(59);

    #define ITOH120U M_ITOH120(,)
    #define P_ITOH120U M_ITOH120(P,)
    #define VP_ITOH120U M_ITOH120(VP,)
    #define PMS_ITOH120U M_ITOH120(P,_MULTISQR)
#elif(HFEn==121)
    #define M_ITOH121(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##EVEN_CASE(60);

    #define ITOH121U M_ITOH121(,)
    #define P_ITOH121U M_ITOH121(P,)
    #define VP_ITOH121U M_ITOH121(VP,)
    #define PMS_ITOH121U M_ITOH121(P,_MULTISQR)
#elif(HFEn==122)
    #define M_ITOH122(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##ODD_CASE(60);

    #define ITOH122U M_ITOH122(,)
    #define P_ITOH122U M_ITOH122(P,)
    #define VP_ITOH122U M_ITOH122(VP,)
    #define PMS_ITOH122U M_ITOH122(P,_MULTISQR)
#elif(HFEn==123)
    #define M_ITOH123(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##EVEN_CASE(61);

    #define ITOH123U M_ITOH123(,)
    #define P_ITOH123U M_ITOH123(P,)
    #define VP_ITOH123U M_ITOH123(VP,)
    #define PMS_ITOH123U M_ITOH123(P,_MULTISQR)
#elif(HFEn==124)
    #define M_ITOH124(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##ODD_CASE(61);

    #define ITOH124U M_ITOH124(,)
    #define P_ITOH124U M_ITOH124(P,)
    #define VP_ITOH124U M_ITOH124(VP,)
    #define PMS_ITOH124U M_ITOH124(P,_MULTISQR)
#elif(HFEn==125)
    #define M_ITOH125(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##EVEN_CASE(62);

    #define ITOH125U M_ITOH125(,)
    #define P_ITOH125U M_ITOH125(P,)
    #define VP_ITOH125U M_ITOH125(VP,)
    #define PMS_ITOH125U M_ITOH125(P,_MULTISQR)
#elif(HFEn==126)
    #define M_ITOH126(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##ODD_CASE(62);

    #define ITOH126U M_ITOH126(,)
    #define P_ITOH126U M_ITOH126(P,)
    #define VP_ITOH126U M_ITOH126(VP,)
    #define PMS_ITOH126U M_ITOH126(P,_MULTISQR)
#elif(HFEn==127)
    #define M_ITOH127(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##EVEN_CASE(63);

    #define ITOH127U M_ITOH127(,)
    #define P_ITOH127U M_ITOH127(P,)
    #define VP_ITOH127U M_ITOH127(VP,)
    #define PMS_ITOH127U M_ITOH127(P,_MULTISQR)
#elif(HFEn==128)
    #define M_ITOH128(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##ODD_CASE(63);

    #define ITOH128U M_ITOH128(,)
    #define P_ITOH128U M_ITOH128(P,)
    #define VP_ITOH128U M_ITOH128(VP,)
    #define PMS_ITOH128U M_ITOH128(P,_MULTISQR)
#elif(HFEn==129)
    #define M_ITOH129(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##EVEN_CASE(64);

    #define ITOH129U M_ITOH129(,)
    #define P_ITOH129U M_ITOH129(P,)
    #define VP_ITOH129U M_ITOH129(VP,)
    #define PMS_ITOH129U M_ITOH129(P,_MULTISQR)
#elif(HFEn==130)
    #define M_ITOH130(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##ODD_CASE(64);

    #define ITOH130U M_ITOH130(,)
    #define P_ITOH130U M_ITOH130(P,)
    #define VP_ITOH130U M_ITOH130(VP,)
    #define PMS_ITOH130U M_ITOH130(P,_MULTISQR)
#elif(HFEn==131)
    #define M_ITOH131(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##EVEN_CASE(65);

    #define ITOH131U M_ITOH131(,)
    #define P_ITOH131U M_ITOH131(P,)
    #define VP_ITOH131U M_ITOH131(VP,)
    #define PMS_ITOH131U M_ITOH131(P,_MULTISQR)
#elif(HFEn==132)
    #define M_ITOH132(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##ODD_CASE(65);

    #define ITOH132U M_ITOH132(,)
    #define P_ITOH132U M_ITOH132(P,)
    #define VP_ITOH132U M_ITOH132(VP,)
    #define PMS_ITOH132U M_ITOH132(P,_MULTISQR)
#elif(HFEn==133)
    #define M_ITOH133(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##EVEN_CASE(66);

    #define ITOH133U M_ITOH133(,)
    #define P_ITOH133U M_ITOH133(P,)
    #define VP_ITOH133U M_ITOH133(VP,)
    #define PMS_ITOH133U M_ITOH133(P,_MULTISQR)
#elif(HFEn==134)
    #define M_ITOH134(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##ODD_CASE(66);

    #define ITOH134U M_ITOH134(,)
    #define P_ITOH134U M_ITOH134(P,)
    #define VP_ITOH134U M_ITOH134(VP,)
    #define PMS_ITOH134U M_ITOH134(P,_MULTISQR)
#elif(HFEn==135)
    #define M_ITOH135(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##EVEN_CASE(67);

    #define ITOH135U M_ITOH135(,)
    #define P_ITOH135U M_ITOH135(P,)
    #define VP_ITOH135U M_ITOH135(VP,)
    #define PMS_ITOH135U M_ITOH135(P,_MULTISQR)
#elif(HFEn==136)
    #define M_ITOH136(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##ODD_CASE(67);

    #define ITOH136U M_ITOH136(,)
    #define P_ITOH136U M_ITOH136(P,)
    #define VP_ITOH136U M_ITOH136(VP,)
    #define PMS_ITOH136U M_ITOH136(P,_MULTISQR)
#elif(HFEn==137)
    #define M_ITOH137(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##EVEN_CASE(68);

    #define ITOH137U M_ITOH137(,)
    #define P_ITOH137U M_ITOH137(P,)
    #define VP_ITOH137U M_ITOH137(VP,)
    #define PMS_ITOH137U M_ITOH137(P,_MULTISQR)
#elif(HFEn==138)
    #define M_ITOH138(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##ODD_CASE(68);

    #define ITOH138U M_ITOH138(,)
    #define P_ITOH138U M_ITOH138(P,)
    #define VP_ITOH138U M_ITOH138(VP,)
    #define PMS_ITOH138U M_ITOH138(P,_MULTISQR)
#elif(HFEn==139)
    #define M_ITOH139(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##EVEN_CASE(69);

    #define ITOH139U M_ITOH139(,)
    #define P_ITOH139U M_ITOH139(P,)
    #define VP_ITOH139U M_ITOH139(VP,)
    #define PMS_ITOH139U M_ITOH139(P,_MULTISQR)
#elif(HFEn==140)
    #define M_ITOH140(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##ODD_CASE(69);

    #define ITOH140U M_ITOH140(,)
    #define P_ITOH140U M_ITOH140(P,)
    #define VP_ITOH140U M_ITOH140(VP,)
    #define PMS_ITOH140U M_ITOH140(P,_MULTISQR)
#elif(HFEn==141)
    #define M_ITOH141(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##EVEN_CASE(70);

    #define ITOH141U M_ITOH141(,)
    #define P_ITOH141U M_ITOH141(P,)
    #define VP_ITOH141U M_ITOH141(VP,)
    #define PMS_ITOH141U M_ITOH141(P,_MULTISQR)
#elif(HFEn==142)
    #define M_ITOH142(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##ODD_CASE(70);

    #define ITOH142U M_ITOH142(,)
    #define P_ITOH142U M_ITOH142(P,)
    #define VP_ITOH142U M_ITOH142(VP,)
    #define PMS_ITOH142U M_ITOH142(P,_MULTISQR)
#elif(HFEn==143)
    #define M_ITOH143(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##EVEN_CASE(71);

    #define ITOH143U M_ITOH143(,)
    #define P_ITOH143U M_ITOH143(P,)
    #define VP_ITOH143U M_ITOH143(VP,)
    #define PMS_ITOH143U M_ITOH143(P,_MULTISQR)
#elif(HFEn==144)
    #define M_ITOH144(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##ODD_CASE(71);

    #define ITOH144U M_ITOH144(,)
    #define P_ITOH144U M_ITOH144(P,)
    #define VP_ITOH144U M_ITOH144(VP,)
    #define PMS_ITOH144U M_ITOH144(P,_MULTISQR)
#elif(HFEn==145)
    #define M_ITOH145(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##EVEN_CASE(36);\
        V##EVEN_CASE(72);

    #define ITOH145U M_ITOH145(,)
    #define P_ITOH145U M_ITOH145(P,)
    #define VP_ITOH145U M_ITOH145(VP,)
    #define PMS_ITOH145U M_ITOH145(P,_MULTISQR)
#elif(HFEn==146)
    #define M_ITOH146(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##EVEN_CASE(36);\
        V##ODD_CASE(72);

    #define ITOH146U M_ITOH146(,)
    #define P_ITOH146U M_ITOH146(P,)
    #define VP_ITOH146U M_ITOH146(VP,)
    #define PMS_ITOH146U M_ITOH146(P,_MULTISQR)
#elif(HFEn==147)
    #define M_ITOH147(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##ODD_CASE(36);\
        V##EVEN_CASE(73);

    #define ITOH147U M_ITOH147(,)
    #define P_ITOH147U M_ITOH147(P,)
    #define VP_ITOH147U M_ITOH147(VP,)
    #define PMS_ITOH147U M_ITOH147(P,_MULTISQR)
#elif(HFEn==148)
    #define M_ITOH148(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##ODD_CASE(36);\
        V##ODD_CASE(73);

    #define ITOH148U M_ITOH148(,)
    #define P_ITOH148U M_ITOH148(P,)
    #define VP_ITOH148U M_ITOH148(VP,)
    #define PMS_ITOH148U M_ITOH148(P,_MULTISQR)
#elif(HFEn==149)
    #define M_ITOH149(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##EVEN_CASE(37);\
        V##EVEN_CASE(74);

    #define ITOH149U M_ITOH149(,)
    #define P_ITOH149U M_ITOH149(P,)
    #define VP_ITOH149U M_ITOH149(VP,)
    #define PMS_ITOH149U M_ITOH149(P,_MULTISQR)
#elif(HFEn==150)
    #define M_ITOH150(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##EVEN_CASE(37);\
        V##ODD_CASE(74);

    #define ITOH150U M_ITOH150(,)
    #define P_ITOH150U M_ITOH150(P,)
    #define VP_ITOH150U M_ITOH150(VP,)
    #define PMS_ITOH150U M_ITOH150(P,_MULTISQR)
#elif(HFEn==151)
    #define M_ITOH151(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##ODD_CASE(37);\
        V##EVEN_CASE(75);

    #define ITOH151U M_ITOH151(,)
    #define P_ITOH151U M_ITOH151(P,)
    #define VP_ITOH151U M_ITOH151(VP,)
    #define PMS_ITOH151U M_ITOH151(P,_MULTISQR)
#elif(HFEn==152)
    #define M_ITOH152(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##ODD_CASE(37);\
        V##ODD_CASE(75);

    #define ITOH152U M_ITOH152(,)
    #define P_ITOH152U M_ITOH152(P,)
    #define VP_ITOH152U M_ITOH152(VP,)
    #define PMS_ITOH152U M_ITOH152(P,_MULTISQR)
#elif(HFEn==153)
    #define M_ITOH153(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##EVEN_CASE(38);\
        V##EVEN_CASE(76);

    #define ITOH153U M_ITOH153(,)
    #define P_ITOH153U M_ITOH153(P,)
    #define VP_ITOH153U M_ITOH153(VP,)
    #define PMS_ITOH153U M_ITOH153(P,_MULTISQR)
#elif(HFEn==154)
    #define M_ITOH154(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##EVEN_CASE(38);\
        V##ODD_CASE(76);

    #define ITOH154U M_ITOH154(,)
    #define P_ITOH154U M_ITOH154(P,)
    #define VP_ITOH154U M_ITOH154(VP,)
    #define PMS_ITOH154U M_ITOH154(P,_MULTISQR)
#elif(HFEn==155)
    #define M_ITOH155(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##ODD_CASE(38);\
        V##EVEN_CASE(77);

    #define ITOH155U M_ITOH155(,)
    #define P_ITOH155U M_ITOH155(P,)
    #define VP_ITOH155U M_ITOH155(VP,)
    #define PMS_ITOH155U M_ITOH155(P,_MULTISQR)
#elif(HFEn==156)
    #define M_ITOH156(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##ODD_CASE(38);\
        V##ODD_CASE(77);

    #define ITOH156U M_ITOH156(,)
    #define P_ITOH156U M_ITOH156(P,)
    #define VP_ITOH156U M_ITOH156(VP,)
    #define PMS_ITOH156U M_ITOH156(P,_MULTISQR)
#elif(HFEn==157)
    #define M_ITOH157(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##EVEN_CASE(39);\
        V##EVEN_CASE(78);

    #define ITOH157U M_ITOH157(,)
    #define P_ITOH157U M_ITOH157(P,)
    #define VP_ITOH157U M_ITOH157(VP,)
    #define PMS_ITOH157U M_ITOH157(P,_MULTISQR)
#elif(HFEn==158)
    #define M_ITOH158(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##EVEN_CASE(39);\
        V##ODD_CASE(78);

    #define ITOH158U M_ITOH158(,)
    #define P_ITOH158U M_ITOH158(P,)
    #define VP_ITOH158U M_ITOH158(VP,)
    #define PMS_ITOH158U M_ITOH158(P,_MULTISQR)
#elif(HFEn==159)
    #define M_ITOH159(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##ODD_CASE(39);\
        V##EVEN_CASE(79);

    #define ITOH159U M_ITOH159(,)
    #define P_ITOH159U M_ITOH159(P,)
    #define VP_ITOH159U M_ITOH159(VP,)
    #define PMS_ITOH159U M_ITOH159(P,_MULTISQR)
#elif(HFEn==160)
    #define M_ITOH160(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##ODD_CASE(39);\
        V##ODD_CASE(79);

    #define ITOH160U M_ITOH160(,)
    #define P_ITOH160U M_ITOH160(P,)
    #define VP_ITOH160U M_ITOH160(VP,)
    #define PMS_ITOH160U M_ITOH160(P,_MULTISQR)
#elif(HFEn==161)
    #define M_ITOH161(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##EVEN_CASE(40);\
        V##EVEN_CASE(80);

    #define ITOH161U M_ITOH161(,)
    #define P_ITOH161U M_ITOH161(P,)
    #define VP_ITOH161U M_ITOH161(VP,)
    #define PMS_ITOH161U M_ITOH161(P,_MULTISQR)
#elif(HFEn==162)
    #define M_ITOH162(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##EVEN_CASE(40);\
        V##ODD_CASE(80);

    #define ITOH162U M_ITOH162(,)
    #define P_ITOH162U M_ITOH162(P,)
    #define VP_ITOH162U M_ITOH162(VP,)
    #define PMS_ITOH162U M_ITOH162(P,_MULTISQR)
#elif(HFEn==163)
    #define M_ITOH163(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##ODD_CASE(40);\
        V##EVEN_CASE(81);

    #define ITOH163U M_ITOH163(,)
    #define P_ITOH163U M_ITOH163(P,)
    #define VP_ITOH163U M_ITOH163(VP,)
    #define PMS_ITOH163U M_ITOH163(P,_MULTISQR)
#elif(HFEn==164)
    #define M_ITOH164(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##ODD_CASE(40);\
        V##ODD_CASE(81);

    #define ITOH164U M_ITOH164(,)
    #define P_ITOH164U M_ITOH164(P,)
    #define VP_ITOH164U M_ITOH164(VP,)
    #define PMS_ITOH164U M_ITOH164(P,_MULTISQR)
#elif(HFEn==165)
    #define M_ITOH165(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##EVEN_CASE(41);\
        V##EVEN_CASE(82);

    #define ITOH165U M_ITOH165(,)
    #define P_ITOH165U M_ITOH165(P,)
    #define VP_ITOH165U M_ITOH165(VP,)
    #define PMS_ITOH165U M_ITOH165(P,_MULTISQR)
#elif(HFEn==166)
    #define M_ITOH166(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##EVEN_CASE(41);\
        V##ODD_CASE(82);

    #define ITOH166U M_ITOH166(,)
    #define P_ITOH166U M_ITOH166(P,)
    #define VP_ITOH166U M_ITOH166(VP,)
    #define PMS_ITOH166U M_ITOH166(P,_MULTISQR)
#elif(HFEn==167)
    #define M_ITOH167(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##ODD_CASE(41);\
        V##EVEN_CASE(83);

    #define ITOH167U M_ITOH167(,)
    #define P_ITOH167U M_ITOH167(P,)
    #define VP_ITOH167U M_ITOH167(VP,)
    #define PMS_ITOH167U M_ITOH167(P,_MULTISQR)
#elif(HFEn==168)
    #define M_ITOH168(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##ODD_CASE(41);\
        V##ODD_CASE(83);

    #define ITOH168U M_ITOH168(,)
    #define P_ITOH168U M_ITOH168(P,)
    #define VP_ITOH168U M_ITOH168(VP,)
    #define PMS_ITOH168U M_ITOH168(P,_MULTISQR)
#elif(HFEn==169)
    #define M_ITOH169(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##EVEN_CASE(42);\
        V##EVEN_CASE(84);

    #define ITOH169U M_ITOH169(,)
    #define P_ITOH169U M_ITOH169(P,)
    #define VP_ITOH169U M_ITOH169(VP,)
    #define PMS_ITOH169U M_ITOH169(P,_MULTISQR)
#elif(HFEn==170)
    #define M_ITOH170(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##EVEN_CASE(42);\
        V##ODD_CASE(84);

    #define ITOH170U M_ITOH170(,)
    #define P_ITOH170U M_ITOH170(P,)
    #define VP_ITOH170U M_ITOH170(VP,)
    #define PMS_ITOH170U M_ITOH170(P,_MULTISQR)
#elif(HFEn==171)
    #define M_ITOH171(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##ODD_CASE(42);\
        V##EVEN_CASE(85);

    #define ITOH171U M_ITOH171(,)
    #define P_ITOH171U M_ITOH171(P,)
    #define VP_ITOH171U M_ITOH171(VP,)
    #define PMS_ITOH171U M_ITOH171(P,_MULTISQR)
#elif(HFEn==172)
    #define M_ITOH172(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##ODD_CASE(42);\
        V##ODD_CASE(85);

    #define ITOH172U M_ITOH172(,)
    #define P_ITOH172U M_ITOH172(P,)
    #define VP_ITOH172U M_ITOH172(VP,)
    #define PMS_ITOH172U M_ITOH172(P,_MULTISQR)
#elif(HFEn==173)
    #define M_ITOH173(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##EVEN_CASE(43);\
        V##EVEN_CASE(86);

    #define ITOH173U M_ITOH173(,)
    #define P_ITOH173U M_ITOH173(P,)
    #define VP_ITOH173U M_ITOH173(VP,)
    #define PMS_ITOH173U M_ITOH173(P,_MULTISQR)
#elif(HFEn==174)
    #define M_ITOH174(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##EVEN_CASE(43);\
        V##ODD_CASE(86);

    #define ITOH174U M_ITOH174(,)
    #define P_ITOH174U M_ITOH174(P,)
    #define VP_ITOH174U M_ITOH174(VP,)
    #define PMS_ITOH174U M_ITOH174(P,_MULTISQR)
#elif(HFEn==175)
    #define M_ITOH175(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##ODD_CASE(43);\
        V##EVEN_CASE(87);

    #define ITOH175U M_ITOH175(,)
    #define P_ITOH175U M_ITOH175(P,)
    #define VP_ITOH175U M_ITOH175(VP,)
    #define PMS_ITOH175U M_ITOH175(P,_MULTISQR)
#elif(HFEn==176)
    #define M_ITOH176(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##ODD_CASE(43);\
        V##ODD_CASE(87);

    #define ITOH176U M_ITOH176(,)
    #define P_ITOH176U M_ITOH176(P,)
    #define VP_ITOH176U M_ITOH176(VP,)
    #define PMS_ITOH176U M_ITOH176(P,_MULTISQR)
#elif(HFEn==177)
    #define M_ITOH177(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##EVEN_CASE(44);\
        V##EVEN_CASE(88);

    #define ITOH177U M_ITOH177(,)
    #define P_ITOH177U M_ITOH177(P,)
    #define VP_ITOH177U M_ITOH177(VP,)
    #define PMS_ITOH177U M_ITOH177(P,_MULTISQR)
#elif(HFEn==178)
    #define M_ITOH178(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##EVEN_CASE(44);\
        V##ODD_CASE(88);

    #define ITOH178U M_ITOH178(,)
    #define P_ITOH178U M_ITOH178(P,)
    #define VP_ITOH178U M_ITOH178(VP,)
    #define PMS_ITOH178U M_ITOH178(P,_MULTISQR)
#elif(HFEn==179)
    #define M_ITOH179(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##ODD_CASE(44);\
        V##EVEN_CASE(89);

    #define ITOH179U M_ITOH179(,)
    #define P_ITOH179U M_ITOH179(P,)
    #define VP_ITOH179U M_ITOH179(VP,)
    #define PMS_ITOH179U M_ITOH179(P,_MULTISQR)
#elif(HFEn==180)
    #define M_ITOH180(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##ODD_CASE(44);\
        V##ODD_CASE(89);

    #define ITOH180U M_ITOH180(,)
    #define P_ITOH180U M_ITOH180(P,)
    #define VP_ITOH180U M_ITOH180(VP,)
    #define PMS_ITOH180U M_ITOH180(P,_MULTISQR)
#elif(HFEn==181)
    #define M_ITOH181(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##EVEN_CASE(45);\
        V##EVEN_CASE(90);

    #define ITOH181U M_ITOH181(,)
    #define P_ITOH181U M_ITOH181(P,)
    #define VP_ITOH181U M_ITOH181(VP,)
    #define PMS_ITOH181U M_ITOH181(P,_MULTISQR)
#elif(HFEn==182)
    #define M_ITOH182(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##EVEN_CASE(45);\
        V##ODD_CASE(90);

    #define ITOH182U M_ITOH182(,)
    #define P_ITOH182U M_ITOH182(P,)
    #define VP_ITOH182U M_ITOH182(VP,)
    #define PMS_ITOH182U M_ITOH182(P,_MULTISQR)
#elif(HFEn==183)
    #define M_ITOH183(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##ODD_CASE(45);\
        V##EVEN_CASE(91);

    #define ITOH183U M_ITOH183(,)
    #define P_ITOH183U M_ITOH183(P,)
    #define VP_ITOH183U M_ITOH183(VP,)
    #define PMS_ITOH183U M_ITOH183(P,_MULTISQR)
#elif(HFEn==184)
    #define M_ITOH184(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##ODD_CASE(45);\
        V##ODD_CASE(91);

    #define ITOH184U M_ITOH184(,)
    #define P_ITOH184U M_ITOH184(P,)
    #define VP_ITOH184U M_ITOH184(VP,)
    #define PMS_ITOH184U M_ITOH184(P,_MULTISQR)
#elif(HFEn==185)
    #define M_ITOH185(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##EVEN_CASE(46);\
        V##EVEN_CASE(92);

    #define ITOH185U M_ITOH185(,)
    #define P_ITOH185U M_ITOH185(P,)
    #define VP_ITOH185U M_ITOH185(VP,)
    #define PMS_ITOH185U M_ITOH185(P,_MULTISQR)
#elif(HFEn==186)
    #define M_ITOH186(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##EVEN_CASE(46);\
        V##ODD_CASE(92);

    #define ITOH186U M_ITOH186(,)
    #define P_ITOH186U M_ITOH186(P,)
    #define VP_ITOH186U M_ITOH186(VP,)
    #define PMS_ITOH186U M_ITOH186(P,_MULTISQR)
#elif(HFEn==187)
    #define M_ITOH187(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##ODD_CASE(46);\
        V##EVEN_CASE(93);

    #define ITOH187U M_ITOH187(,)
    #define P_ITOH187U M_ITOH187(P,)
    #define VP_ITOH187U M_ITOH187(VP,)
    #define PMS_ITOH187U M_ITOH187(P,_MULTISQR)
#elif(HFEn==188)
    #define M_ITOH188(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##ODD_CASE(46);\
        V##ODD_CASE(93);

    #define ITOH188U M_ITOH188(,)
    #define P_ITOH188U M_ITOH188(P,)
    #define VP_ITOH188U M_ITOH188(VP,)
    #define PMS_ITOH188U M_ITOH188(P,_MULTISQR)
#elif(HFEn==189)
    #define M_ITOH189(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##EVEN_CASE(47);\
        V##EVEN_CASE(94);

    #define ITOH189U M_ITOH189(,)
    #define P_ITOH189U M_ITOH189(P,)
    #define VP_ITOH189U M_ITOH189(VP,)
    #define PMS_ITOH189U M_ITOH189(P,_MULTISQR)
#elif(HFEn==190)
    #define M_ITOH190(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##EVEN_CASE(47);\
        V##ODD_CASE(94);

    #define ITOH190U M_ITOH190(,)
    #define P_ITOH190U M_ITOH190(P,)
    #define VP_ITOH190U M_ITOH190(VP,)
    #define PMS_ITOH190U M_ITOH190(P,_MULTISQR)
#elif(HFEn==191)
    #define M_ITOH191(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##ODD_CASE(47);\
        V##EVEN_CASE(95);

    #define ITOH191U M_ITOH191(,)
    #define P_ITOH191U M_ITOH191(P,)
    #define VP_ITOH191U M_ITOH191(VP,)
    #define PMS_ITOH191U M_ITOH191(P,_MULTISQR)
#elif(HFEn==192)
    #define M_ITOH192(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##ODD_CASE(47);\
        V##ODD_CASE(95);

    #define ITOH192U M_ITOH192(,)
    #define P_ITOH192U M_ITOH192(P,)
    #define VP_ITOH192U M_ITOH192(VP,)
    #define PMS_ITOH192U M_ITOH192(P,_MULTISQR)
#elif(HFEn==193)
    #define M_ITOH193(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##EVEN_CASE(48);\
        V##EVEN_CASE(96);

    #define ITOH193U M_ITOH193(,)
    #define P_ITOH193U M_ITOH193(P,)
    #define VP_ITOH193U M_ITOH193(VP,)
    #define PMS_ITOH193U M_ITOH193(P,_MULTISQR)
#elif(HFEn==194)
    #define M_ITOH194(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##EVEN_CASE(48);\
        V##ODD_CASE(96);

    #define ITOH194U M_ITOH194(,)
    #define P_ITOH194U M_ITOH194(P,)
    #define VP_ITOH194U M_ITOH194(VP,)
    #define PMS_ITOH194U M_ITOH194(P,_MULTISQR)
#elif(HFEn==195)
    #define M_ITOH195(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##ODD_CASE(48);\
        V##EVEN_CASE(97);

    #define ITOH195U M_ITOH195(,)
    #define P_ITOH195U M_ITOH195(P,)
    #define VP_ITOH195U M_ITOH195(VP,)
    #define PMS_ITOH195U M_ITOH195(P,_MULTISQR)
#elif(HFEn==196)
    #define M_ITOH196(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##ODD_CASE(48);\
        V##ODD_CASE(97);

    #define ITOH196U M_ITOH196(,)
    #define P_ITOH196U M_ITOH196(P,)
    #define VP_ITOH196U M_ITOH196(VP,)
    #define PMS_ITOH196U M_ITOH196(P,_MULTISQR)
#elif(HFEn==197)
    #define M_ITOH197(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##EVEN_CASE(49);\
        V##EVEN_CASE(98);

    #define ITOH197U M_ITOH197(,)
    #define P_ITOH197U M_ITOH197(P,)
    #define VP_ITOH197U M_ITOH197(VP,)
    #define PMS_ITOH197U M_ITOH197(P,_MULTISQR)
#elif(HFEn==198)
    #define M_ITOH198(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##EVEN_CASE(49);\
        V##ODD_CASE(98);

    #define ITOH198U M_ITOH198(,)
    #define P_ITOH198U M_ITOH198(P,)
    #define VP_ITOH198U M_ITOH198(VP,)
    #define PMS_ITOH198U M_ITOH198(P,_MULTISQR)
#elif(HFEn==199)
    #define M_ITOH199(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##ODD_CASE(49);\
        V##EVEN_CASE(99);

    #define ITOH199U M_ITOH199(,)
    #define P_ITOH199U M_ITOH199(P,)
    #define VP_ITOH199U M_ITOH199(VP,)
    #define PMS_ITOH199U M_ITOH199(P,_MULTISQR)
#elif(HFEn==200)
    #define M_ITOH200(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##ODD_CASE(49);\
        V##ODD_CASE(99);

    #define ITOH200U M_ITOH200(,)
    #define P_ITOH200U M_ITOH200(P,)
    #define VP_ITOH200U M_ITOH200(VP,)
    #define PMS_ITOH200U M_ITOH200(P,_MULTISQR)
#elif(HFEn==201)
    #define M_ITOH201(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##EVEN_CASE(50);\
        V##EVEN_CASE(100);

    #define ITOH201U M_ITOH201(,)
    #define P_ITOH201U M_ITOH201(P,)
    #define VP_ITOH201U M_ITOH201(VP,)
    #define PMS_ITOH201U M_ITOH201(P,_MULTISQR)
#elif(HFEn==202)
    #define M_ITOH202(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##EVEN_CASE(50);\
        V##ODD_CASE(100);

    #define ITOH202U M_ITOH202(,)
    #define P_ITOH202U M_ITOH202(P,)
    #define VP_ITOH202U M_ITOH202(VP,)
    #define PMS_ITOH202U M_ITOH202(P,_MULTISQR)
#elif(HFEn==203)
    #define M_ITOH203(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##ODD_CASE(50);\
        V##EVEN_CASE(101);

    #define ITOH203U M_ITOH203(,)
    #define P_ITOH203U M_ITOH203(P,)
    #define VP_ITOH203U M_ITOH203(VP,)
    #define PMS_ITOH203U M_ITOH203(P,_MULTISQR)
#elif(HFEn==204)
    #define M_ITOH204(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##ODD_CASE(50);\
        V##ODD_CASE(101);

    #define ITOH204U M_ITOH204(,)
    #define P_ITOH204U M_ITOH204(P,)
    #define VP_ITOH204U M_ITOH204(VP,)
    #define PMS_ITOH204U M_ITOH204(P,_MULTISQR)
#elif(HFEn==205)
    #define M_ITOH205(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##EVEN_CASE(51);\
        V##EVEN_CASE(102);

    #define ITOH205U M_ITOH205(,)
    #define P_ITOH205U M_ITOH205(P,)
    #define VP_ITOH205U M_ITOH205(VP,)
    #define PMS_ITOH205U M_ITOH205(P,_MULTISQR)
#elif(HFEn==206)
    #define M_ITOH206(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##EVEN_CASE(51);\
        V##ODD_CASE(102);

    #define ITOH206U M_ITOH206(,)
    #define P_ITOH206U M_ITOH206(P,)
    #define VP_ITOH206U M_ITOH206(VP,)
    #define PMS_ITOH206U M_ITOH206(P,_MULTISQR)
#elif(HFEn==207)
    #define M_ITOH207(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##ODD_CASE(51);\
        V##EVEN_CASE(103);

    #define ITOH207U M_ITOH207(,)
    #define P_ITOH207U M_ITOH207(P,)
    #define VP_ITOH207U M_ITOH207(VP,)
    #define PMS_ITOH207U M_ITOH207(P,_MULTISQR)
#elif(HFEn==208)
    #define M_ITOH208(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##ODD_CASE(51);\
        V##ODD_CASE(103);

    #define ITOH208U M_ITOH208(,)
    #define P_ITOH208U M_ITOH208(P,)
    #define VP_ITOH208U M_ITOH208(VP,)
    #define PMS_ITOH208U M_ITOH208(P,_MULTISQR)
#elif(HFEn==209)
    #define M_ITOH209(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##EVEN_CASE(52);\
        V##EVEN_CASE(104);

    #define ITOH209U M_ITOH209(,)
    #define P_ITOH209U M_ITOH209(P,)
    #define VP_ITOH209U M_ITOH209(VP,)
    #define PMS_ITOH209U M_ITOH209(P,_MULTISQR)
#elif(HFEn==210)
    #define M_ITOH210(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##EVEN_CASE(52);\
        V##ODD_CASE(104);

    #define ITOH210U M_ITOH210(,)
    #define P_ITOH210U M_ITOH210(P,)
    #define VP_ITOH210U M_ITOH210(VP,)
    #define PMS_ITOH210U M_ITOH210(P,_MULTISQR)
#elif(HFEn==211)
    #define M_ITOH211(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##ODD_CASE(52);\
        V##EVEN_CASE(105);

    #define ITOH211U M_ITOH211(,)
    #define P_ITOH211U M_ITOH211(P,)
    #define VP_ITOH211U M_ITOH211(VP,)
    #define PMS_ITOH211U M_ITOH211(P,_MULTISQR)
#elif(HFEn==212)
    #define M_ITOH212(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##ODD_CASE(52);\
        V##ODD_CASE(105);

    #define ITOH212U M_ITOH212(,)
    #define P_ITOH212U M_ITOH212(P,)
    #define VP_ITOH212U M_ITOH212(VP,)
    #define PMS_ITOH212U M_ITOH212(P,_MULTISQR)
#elif(HFEn==213)
    #define M_ITOH213(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##EVEN_CASE(53);\
        V##EVEN_CASE(106);

    #define ITOH213U M_ITOH213(,)
    #define P_ITOH213U M_ITOH213(P,)
    #define VP_ITOH213U M_ITOH213(VP,)
    #define PMS_ITOH213U M_ITOH213(P,_MULTISQR)
#elif(HFEn==214)
    #define M_ITOH214(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##EVEN_CASE(53);\
        V##ODD_CASE(106);

    #define ITOH214U M_ITOH214(,)
    #define P_ITOH214U M_ITOH214(P,)
    #define VP_ITOH214U M_ITOH214(VP,)
    #define PMS_ITOH214U M_ITOH214(P,_MULTISQR)
#elif(HFEn==215)
    #define M_ITOH215(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##ODD_CASE(53);\
        V##EVEN_CASE(107);

    #define ITOH215U M_ITOH215(,)
    #define P_ITOH215U M_ITOH215(P,)
    #define VP_ITOH215U M_ITOH215(VP,)
    #define PMS_ITOH215U M_ITOH215(P,_MULTISQR)
#elif(HFEn==216)
    #define M_ITOH216(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##ODD_CASE(53);\
        V##ODD_CASE(107);

    #define ITOH216U M_ITOH216(,)
    #define P_ITOH216U M_ITOH216(P,)
    #define VP_ITOH216U M_ITOH216(VP,)
    #define PMS_ITOH216U M_ITOH216(P,_MULTISQR)
#elif(HFEn==217)
    #define M_ITOH217(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##EVEN_CASE(54);\
        V##EVEN_CASE(108);

    #define ITOH217U M_ITOH217(,)
    #define P_ITOH217U M_ITOH217(P,)
    #define VP_ITOH217U M_ITOH217(VP,)
    #define PMS_ITOH217U M_ITOH217(P,_MULTISQR)
#elif(HFEn==218)
    #define M_ITOH218(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##EVEN_CASE(54);\
        V##ODD_CASE(108);

    #define ITOH218U M_ITOH218(,)
    #define P_ITOH218U M_ITOH218(P,)
    #define VP_ITOH218U M_ITOH218(VP,)
    #define PMS_ITOH218U M_ITOH218(P,_MULTISQR)
#elif(HFEn==219)
    #define M_ITOH219(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##ODD_CASE(54);\
        V##EVEN_CASE(109);

    #define ITOH219U M_ITOH219(,)
    #define P_ITOH219U M_ITOH219(P,)
    #define VP_ITOH219U M_ITOH219(VP,)
    #define PMS_ITOH219U M_ITOH219(P,_MULTISQR)
#elif(HFEn==220)
    #define M_ITOH220(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##ODD_CASE(54);\
        V##ODD_CASE(109);

    #define ITOH220U M_ITOH220(,)
    #define P_ITOH220U M_ITOH220(P,)
    #define VP_ITOH220U M_ITOH220(VP,)
    #define PMS_ITOH220U M_ITOH220(P,_MULTISQR)
#elif(HFEn==221)
    #define M_ITOH221(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##EVEN_CASE(55);\
        V##EVEN_CASE(110);

    #define ITOH221U M_ITOH221(,)
    #define P_ITOH221U M_ITOH221(P,)
    #define VP_ITOH221U M_ITOH221(VP,)
    #define PMS_ITOH221U M_ITOH221(P,_MULTISQR)
#elif(HFEn==222)
    #define M_ITOH222(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##EVEN_CASE(55);\
        V##ODD_CASE(110);

    #define ITOH222U M_ITOH222(,)
    #define P_ITOH222U M_ITOH222(P,)
    #define VP_ITOH222U M_ITOH222(VP,)
    #define PMS_ITOH222U M_ITOH222(P,_MULTISQR)
#elif(HFEn==223)
    #define M_ITOH223(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##ODD_CASE(55);\
        V##EVEN_CASE(111);

    #define ITOH223U M_ITOH223(,)
    #define P_ITOH223U M_ITOH223(P,)
    #define VP_ITOH223U M_ITOH223(VP,)
    #define PMS_ITOH223U M_ITOH223(P,_MULTISQR)
#elif(HFEn==224)
    #define M_ITOH224(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##ODD_CASE(55);\
        V##ODD_CASE(111);

    #define ITOH224U M_ITOH224(,)
    #define P_ITOH224U M_ITOH224(P,)
    #define VP_ITOH224U M_ITOH224(VP,)
    #define PMS_ITOH224U M_ITOH224(P,_MULTISQR)
#elif(HFEn==225)
    #define M_ITOH225(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##EVEN_CASE(56);\
        V##EVEN_CASE(112);

    #define ITOH225U M_ITOH225(,)
    #define P_ITOH225U M_ITOH225(P,)
    #define VP_ITOH225U M_ITOH225(VP,)
    #define PMS_ITOH225U M_ITOH225(P,_MULTISQR)
#elif(HFEn==226)
    #define M_ITOH226(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##EVEN_CASE(56);\
        V##ODD_CASE(112);

    #define ITOH226U M_ITOH226(,)
    #define P_ITOH226U M_ITOH226(P,)
    #define VP_ITOH226U M_ITOH226(VP,)
    #define PMS_ITOH226U M_ITOH226(P,_MULTISQR)
#elif(HFEn==227)
    #define M_ITOH227(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##ODD_CASE(56);\
        V##EVEN_CASE(113);

    #define ITOH227U M_ITOH227(,)
    #define P_ITOH227U M_ITOH227(P,)
    #define VP_ITOH227U M_ITOH227(VP,)
    #define PMS_ITOH227U M_ITOH227(P,_MULTISQR)
#elif(HFEn==228)
    #define M_ITOH228(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##ODD_CASE(56);\
        V##ODD_CASE(113);

    #define ITOH228U M_ITOH228(,)
    #define P_ITOH228U M_ITOH228(P,)
    #define VP_ITOH228U M_ITOH228(VP,)
    #define PMS_ITOH228U M_ITOH228(P,_MULTISQR)
#elif(HFEn==229)
    #define M_ITOH229(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##EVEN_CASE(57);\
        V##EVEN_CASE(114);

    #define ITOH229U M_ITOH229(,)
    #define P_ITOH229U M_ITOH229(P,)
    #define VP_ITOH229U M_ITOH229(VP,)
    #define PMS_ITOH229U M_ITOH229(P,_MULTISQR)
#elif(HFEn==230)
    #define M_ITOH230(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##EVEN_CASE(57);\
        V##ODD_CASE(114);

    #define ITOH230U M_ITOH230(,)
    #define P_ITOH230U M_ITOH230(P,)
    #define VP_ITOH230U M_ITOH230(VP,)
    #define PMS_ITOH230U M_ITOH230(P,_MULTISQR)
#elif(HFEn==231)
    #define M_ITOH231(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##ODD_CASE(57);\
        V##EVEN_CASE(115);

    #define ITOH231U M_ITOH231(,)
    #define P_ITOH231U M_ITOH231(P,)
    #define VP_ITOH231U M_ITOH231(VP,)
    #define PMS_ITOH231U M_ITOH231(P,_MULTISQR)
#elif(HFEn==232)
    #define M_ITOH232(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##ODD_CASE(57);\
        V##ODD_CASE(115);

    #define ITOH232U M_ITOH232(,)
    #define P_ITOH232U M_ITOH232(P,)
    #define VP_ITOH232U M_ITOH232(VP,)
    #define PMS_ITOH232U M_ITOH232(P,_MULTISQR)
#elif(HFEn==233)
    #define M_ITOH233(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##EVEN_CASE(58);\
        V##EVEN_CASE(116);

    #define ITOH233U M_ITOH233(,)
    #define P_ITOH233U M_ITOH233(P,)
    #define VP_ITOH233U M_ITOH233(VP,)
    #define PMS_ITOH233U M_ITOH233(P,_MULTISQR)
#elif(HFEn==234)
    #define M_ITOH234(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##EVEN_CASE(58);\
        V##ODD_CASE(116);

    #define ITOH234U M_ITOH234(,)
    #define P_ITOH234U M_ITOH234(P,)
    #define VP_ITOH234U M_ITOH234(VP,)
    #define PMS_ITOH234U M_ITOH234(P,_MULTISQR)
#elif(HFEn==235)
    #define M_ITOH235(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##ODD_CASE(58);\
        V##EVEN_CASE(117);

    #define ITOH235U M_ITOH235(,)
    #define P_ITOH235U M_ITOH235(P,)
    #define VP_ITOH235U M_ITOH235(VP,)
    #define PMS_ITOH235U M_ITOH235(P,_MULTISQR)
#elif(HFEn==236)
    #define M_ITOH236(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##ODD_CASE(58);\
        V##ODD_CASE(117);

    #define ITOH236U M_ITOH236(,)
    #define P_ITOH236U M_ITOH236(P,)
    #define VP_ITOH236U M_ITOH236(VP,)
    #define PMS_ITOH236U M_ITOH236(P,_MULTISQR)
#elif(HFEn==237)
    #define M_ITOH237(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##EVEN_CASE(59);\
        V##EVEN_CASE(118);

    #define ITOH237U M_ITOH237(,)
    #define P_ITOH237U M_ITOH237(P,)
    #define VP_ITOH237U M_ITOH237(VP,)
    #define PMS_ITOH237U M_ITOH237(P,_MULTISQR)
#elif(HFEn==238)
    #define M_ITOH238(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##EVEN_CASE(59);\
        V##ODD_CASE(118);

    #define ITOH238U M_ITOH238(,)
    #define P_ITOH238U M_ITOH238(P,)
    #define VP_ITOH238U M_ITOH238(VP,)
    #define PMS_ITOH238U M_ITOH238(P,_MULTISQR)
#elif(HFEn==239)
    #define M_ITOH239(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##ODD_CASE(59);\
        V##EVEN_CASE(119);

    #define ITOH239U M_ITOH239(,)
    #define P_ITOH239U M_ITOH239(P,)
    #define VP_ITOH239U M_ITOH239(VP,)
    #define PMS_ITOH239U M_ITOH239(P,_MULTISQR)
#elif(HFEn==240)
    #define M_ITOH240(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##ODD_CASE(59);\
        V##ODD_CASE(119);

    #define ITOH240U M_ITOH240(,)
    #define P_ITOH240U M_ITOH240(P,)
    #define VP_ITOH240U M_ITOH240(VP,)
    #define PMS_ITOH240U M_ITOH240(P,_MULTISQR)
#elif(HFEn==241)
    #define M_ITOH241(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##EVEN_CASE(60);\
        V##EVEN_CASE(120);

    #define ITOH241U M_ITOH241(,)
    #define P_ITOH241U M_ITOH241(P,)
    #define VP_ITOH241U M_ITOH241(VP,)
    #define PMS_ITOH241U M_ITOH241(P,_MULTISQR)
#elif(HFEn==242)
    #define M_ITOH242(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##EVEN_CASE(60);\
        V##ODD_CASE(120);

    #define ITOH242U M_ITOH242(,)
    #define P_ITOH242U M_ITOH242(P,)
    #define VP_ITOH242U M_ITOH242(VP,)
    #define PMS_ITOH242U M_ITOH242(P,_MULTISQR)
#elif(HFEn==243)
    #define M_ITOH243(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##ODD_CASE(60);\
        V##EVEN_CASE(121);

    #define ITOH243U M_ITOH243(,)
    #define P_ITOH243U M_ITOH243(P,)
    #define VP_ITOH243U M_ITOH243(VP,)
    #define PMS_ITOH243U M_ITOH243(P,_MULTISQR)
#elif(HFEn==244)
    #define M_ITOH244(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##ODD_CASE(60);\
        V##ODD_CASE(121);

    #define ITOH244U M_ITOH244(,)
    #define P_ITOH244U M_ITOH244(P,)
    #define VP_ITOH244U M_ITOH244(VP,)
    #define PMS_ITOH244U M_ITOH244(P,_MULTISQR)
#elif(HFEn==245)
    #define M_ITOH245(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##EVEN_CASE(61);\
        V##EVEN_CASE(122);

    #define ITOH245U M_ITOH245(,)
    #define P_ITOH245U M_ITOH245(P,)
    #define VP_ITOH245U M_ITOH245(VP,)
    #define PMS_ITOH245U M_ITOH245(P,_MULTISQR)
#elif(HFEn==246)
    #define M_ITOH246(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##EVEN_CASE(61);\
        V##ODD_CASE(122);

    #define ITOH246U M_ITOH246(,)
    #define P_ITOH246U M_ITOH246(P,)
    #define VP_ITOH246U M_ITOH246(VP,)
    #define PMS_ITOH246U M_ITOH246(P,_MULTISQR)
#elif(HFEn==247)
    #define M_ITOH247(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##ODD_CASE(61);\
        V##EVEN_CASE(123);

    #define ITOH247U M_ITOH247(,)
    #define P_ITOH247U M_ITOH247(P,)
    #define VP_ITOH247U M_ITOH247(VP,)
    #define PMS_ITOH247U M_ITOH247(P,_MULTISQR)
#elif(HFEn==248)
    #define M_ITOH248(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##ODD_CASE(61);\
        V##ODD_CASE(123);

    #define ITOH248U M_ITOH248(,)
    #define P_ITOH248U M_ITOH248(P,)
    #define VP_ITOH248U M_ITOH248(VP,)
    #define PMS_ITOH248U M_ITOH248(P,_MULTISQR)
#elif(HFEn==249)
    #define M_ITOH249(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##EVEN_CASE(62);\
        V##EVEN_CASE(124);

    #define ITOH249U M_ITOH249(,)
    #define P_ITOH249U M_ITOH249(P,)
    #define VP_ITOH249U M_ITOH249(VP,)
    #define PMS_ITOH249U M_ITOH249(P,_MULTISQR)
#elif(HFEn==250)
    #define M_ITOH250(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##EVEN_CASE(62);\
        V##ODD_CASE(124);

    #define ITOH250U M_ITOH250(,)
    #define P_ITOH250U M_ITOH250(P,)
    #define VP_ITOH250U M_ITOH250(VP,)
    #define PMS_ITOH250U M_ITOH250(P,_MULTISQR)
#elif(HFEn==251)
    #define M_ITOH251(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##ODD_CASE(62);\
        V##EVEN_CASE(125);

    #define ITOH251U M_ITOH251(,)
    #define P_ITOH251U M_ITOH251(P,)
    #define VP_ITOH251U M_ITOH251(VP,)
    #define PMS_ITOH251U M_ITOH251(P,_MULTISQR)
#elif(HFEn==252)
    #define M_ITOH252(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##ODD_CASE(62);\
        V##ODD_CASE(125);

    #define ITOH252U M_ITOH252(,)
    #define P_ITOH252U M_ITOH252(P,)
    #define VP_ITOH252U M_ITOH252(VP,)
    #define PMS_ITOH252U M_ITOH252(P,_MULTISQR)
#elif(HFEn==253)
    #define M_ITOH253(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##EVEN_CASE(63);\
        V##EVEN_CASE(126);

    #define ITOH253U M_ITOH253(,)
    #define P_ITOH253U M_ITOH253(P,)
    #define VP_ITOH253U M_ITOH253(VP,)
    #define PMS_ITOH253U M_ITOH253(P,_MULTISQR)
#elif(HFEn==254)
    #define M_ITOH254(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##EVEN_CASE(63);\
        V##ODD_CASE(126);

    #define ITOH254U M_ITOH254(,)
    #define P_ITOH254U M_ITOH254(P,)
    #define VP_ITOH254U M_ITOH254(VP,)
    #define PMS_ITOH254U M_ITOH254(P,_MULTISQR)
#elif(HFEn==255)
    #define M_ITOH255(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##ODD_CASE(63);\
        V##EVEN_CASE(127);

    #define ITOH255U M_ITOH255(,)
    #define P_ITOH255U M_ITOH255(P,)
    #define VP_ITOH255U M_ITOH255(VP,)
    #define PMS_ITOH255U M_ITOH255(P,_MULTISQR)
#elif(HFEn==256)
    #define M_ITOH256(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##ODD_CASE(63);\
        V##ODD_CASE(127);

    #define ITOH256U M_ITOH256(,)
    #define P_ITOH256U M_ITOH256(P,)
    #define VP_ITOH256U M_ITOH256(VP,)
    #define PMS_ITOH256U M_ITOH256(P,_MULTISQR)
#elif(HFEn==257)
    #define M_ITOH257(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##EVEN_CASE(64);\
        V##EVEN_CASE(128);

    #define ITOH257U M_ITOH257(,)
    #define P_ITOH257U M_ITOH257(P,)
    #define VP_ITOH257U M_ITOH257(VP,)
    #define PMS_ITOH257U M_ITOH257(P,_MULTISQR)
#elif(HFEn==258)
    #define M_ITOH258(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##EVEN_CASE(64);\
        V##ODD_CASE(128);

    #define ITOH258U M_ITOH258(,)
    #define P_ITOH258U M_ITOH258(P,)
    #define VP_ITOH258U M_ITOH258(VP,)
    #define PMS_ITOH258U M_ITOH258(P,_MULTISQR)
#elif(HFEn==259)
    #define M_ITOH259(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##ODD_CASE(64);\
        V##EVEN_CASE(129);

    #define ITOH259U M_ITOH259(,)
    #define P_ITOH259U M_ITOH259(P,)
    #define VP_ITOH259U M_ITOH259(VP,)
    #define PMS_ITOH259U M_ITOH259(P,_MULTISQR)
#elif(HFEn==260)
    #define M_ITOH260(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##ODD_CASE(64);\
        V##ODD_CASE(129);

    #define ITOH260U M_ITOH260(,)
    #define P_ITOH260U M_ITOH260(P,)
    #define VP_ITOH260U M_ITOH260(VP,)
    #define PMS_ITOH260U M_ITOH260(P,_MULTISQR)
#elif(HFEn==261)
    #define M_ITOH261(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##EVEN_CASE(65);\
        V##EVEN_CASE(130);

    #define ITOH261U M_ITOH261(,)
    #define P_ITOH261U M_ITOH261(P,)
    #define VP_ITOH261U M_ITOH261(VP,)
    #define PMS_ITOH261U M_ITOH261(P,_MULTISQR)
#elif(HFEn==262)
    #define M_ITOH262(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##EVEN_CASE(65);\
        V##ODD_CASE(130);

    #define ITOH262U M_ITOH262(,)
    #define P_ITOH262U M_ITOH262(P,)
    #define VP_ITOH262U M_ITOH262(VP,)
    #define PMS_ITOH262U M_ITOH262(P,_MULTISQR)
#elif(HFEn==263)
    #define M_ITOH263(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##ODD_CASE(65);\
        V##EVEN_CASE(131);

    #define ITOH263U M_ITOH263(,)
    #define P_ITOH263U M_ITOH263(P,)
    #define VP_ITOH263U M_ITOH263(VP,)
    #define PMS_ITOH263U M_ITOH263(P,_MULTISQR)
#elif(HFEn==264)
    #define M_ITOH264(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##ODD_CASE(65);\
        V##ODD_CASE(131);

    #define ITOH264U M_ITOH264(,)
    #define P_ITOH264U M_ITOH264(P,)
    #define VP_ITOH264U M_ITOH264(VP,)
    #define PMS_ITOH264U M_ITOH264(P,_MULTISQR)
#elif(HFEn==265)
    #define M_ITOH265(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##EVEN_CASE(66);\
        V##EVEN_CASE(132);

    #define ITOH265U M_ITOH265(,)
    #define P_ITOH265U M_ITOH265(P,)
    #define VP_ITOH265U M_ITOH265(VP,)
    #define PMS_ITOH265U M_ITOH265(P,_MULTISQR)
#elif(HFEn==266)
    #define M_ITOH266(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##EVEN_CASE(66);\
        V##ODD_CASE(132);

    #define ITOH266U M_ITOH266(,)
    #define P_ITOH266U M_ITOH266(P,)
    #define VP_ITOH266U M_ITOH266(VP,)
    #define PMS_ITOH266U M_ITOH266(P,_MULTISQR)
#elif(HFEn==267)
    #define M_ITOH267(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##ODD_CASE(66);\
        V##EVEN_CASE(133);

    #define ITOH267U M_ITOH267(,)
    #define P_ITOH267U M_ITOH267(P,)
    #define VP_ITOH267U M_ITOH267(VP,)
    #define PMS_ITOH267U M_ITOH267(P,_MULTISQR)
#elif(HFEn==268)
    #define M_ITOH268(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##ODD_CASE(66);\
        V##ODD_CASE(133);

    #define ITOH268U M_ITOH268(,)
    #define P_ITOH268U M_ITOH268(P,)
    #define VP_ITOH268U M_ITOH268(VP,)
    #define PMS_ITOH268U M_ITOH268(P,_MULTISQR)
#elif(HFEn==269)
    #define M_ITOH269(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##EVEN_CASE(67);\
        V##EVEN_CASE(134);

    #define ITOH269U M_ITOH269(,)
    #define P_ITOH269U M_ITOH269(P,)
    #define VP_ITOH269U M_ITOH269(VP,)
    #define PMS_ITOH269U M_ITOH269(P,_MULTISQR)
#elif(HFEn==270)
    #define M_ITOH270(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##EVEN_CASE(67);\
        V##ODD_CASE(134);

    #define ITOH270U M_ITOH270(,)
    #define P_ITOH270U M_ITOH270(P,)
    #define VP_ITOH270U M_ITOH270(VP,)
    #define PMS_ITOH270U M_ITOH270(P,_MULTISQR)
#elif(HFEn==271)
    #define M_ITOH271(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##ODD_CASE(67);\
        V##EVEN_CASE(135);

    #define ITOH271U M_ITOH271(,)
    #define P_ITOH271U M_ITOH271(P,)
    #define VP_ITOH271U M_ITOH271(VP,)
    #define PMS_ITOH271U M_ITOH271(P,_MULTISQR)
#elif(HFEn==272)
    #define M_ITOH272(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##ODD_CASE(67);\
        V##ODD_CASE(135);

    #define ITOH272U M_ITOH272(,)
    #define P_ITOH272U M_ITOH272(P,)
    #define VP_ITOH272U M_ITOH272(VP,)
    #define PMS_ITOH272U M_ITOH272(P,_MULTISQR)
#elif(HFEn==273)
    #define M_ITOH273(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##EVEN_CASE(68);\
        V##EVEN_CASE(136);

    #define ITOH273U M_ITOH273(,)
    #define P_ITOH273U M_ITOH273(P,)
    #define VP_ITOH273U M_ITOH273(VP,)
    #define PMS_ITOH273U M_ITOH273(P,_MULTISQR)
#elif(HFEn==274)
    #define M_ITOH274(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##EVEN_CASE(68);\
        V##ODD_CASE(136);

    #define ITOH274U M_ITOH274(,)
    #define P_ITOH274U M_ITOH274(P,)
    #define VP_ITOH274U M_ITOH274(VP,)
    #define PMS_ITOH274U M_ITOH274(P,_MULTISQR)
#elif(HFEn==275)
    #define M_ITOH275(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##ODD_CASE(68);\
        V##EVEN_CASE(137);

    #define ITOH275U M_ITOH275(,)
    #define P_ITOH275U M_ITOH275(P,)
    #define VP_ITOH275U M_ITOH275(VP,)
    #define PMS_ITOH275U M_ITOH275(P,_MULTISQR)
#elif(HFEn==276)
    #define M_ITOH276(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##ODD_CASE(68);\
        V##ODD_CASE(137);

    #define ITOH276U M_ITOH276(,)
    #define P_ITOH276U M_ITOH276(P,)
    #define VP_ITOH276U M_ITOH276(VP,)
    #define PMS_ITOH276U M_ITOH276(P,_MULTISQR)
#elif(HFEn==277)
    #define M_ITOH277(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##EVEN_CASE(69);\
        V##EVEN_CASE(138);

    #define ITOH277U M_ITOH277(,)
    #define P_ITOH277U M_ITOH277(P,)
    #define VP_ITOH277U M_ITOH277(VP,)
    #define PMS_ITOH277U M_ITOH277(P,_MULTISQR)
#elif(HFEn==278)
    #define M_ITOH278(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##EVEN_CASE(69);\
        V##ODD_CASE(138);

    #define ITOH278U M_ITOH278(,)
    #define P_ITOH278U M_ITOH278(P,)
    #define VP_ITOH278U M_ITOH278(VP,)
    #define PMS_ITOH278U M_ITOH278(P,_MULTISQR)
#elif(HFEn==279)
    #define M_ITOH279(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##ODD_CASE(69);\
        V##EVEN_CASE(139);

    #define ITOH279U M_ITOH279(,)
    #define P_ITOH279U M_ITOH279(P,)
    #define VP_ITOH279U M_ITOH279(VP,)
    #define PMS_ITOH279U M_ITOH279(P,_MULTISQR)
#elif(HFEn==280)
    #define M_ITOH280(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##ODD_CASE(69);\
        V##ODD_CASE(139);

    #define ITOH280U M_ITOH280(,)
    #define P_ITOH280U M_ITOH280(P,)
    #define VP_ITOH280U M_ITOH280(VP,)
    #define PMS_ITOH280U M_ITOH280(P,_MULTISQR)
#elif(HFEn==281)
    #define M_ITOH281(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##EVEN_CASE(70);\
        V##EVEN_CASE(140);

    #define ITOH281U M_ITOH281(,)
    #define P_ITOH281U M_ITOH281(P,)
    #define VP_ITOH281U M_ITOH281(VP,)
    #define PMS_ITOH281U M_ITOH281(P,_MULTISQR)
#elif(HFEn==282)
    #define M_ITOH282(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##EVEN_CASE(70);\
        V##ODD_CASE(140);

    #define ITOH282U M_ITOH282(,)
    #define P_ITOH282U M_ITOH282(P,)
    #define VP_ITOH282U M_ITOH282(VP,)
    #define PMS_ITOH282U M_ITOH282(P,_MULTISQR)
#elif(HFEn==283)
    #define M_ITOH283(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##ODD_CASE(70);\
        V##EVEN_CASE(141);

    #define ITOH283U M_ITOH283(,)
    #define P_ITOH283U M_ITOH283(P,)
    #define VP_ITOH283U M_ITOH283(VP,)
    #define PMS_ITOH283U M_ITOH283(P,_MULTISQR)
#elif(HFEn==284)
    #define M_ITOH284(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##ODD_CASE(70);\
        V##ODD_CASE(141);

    #define ITOH284U M_ITOH284(,)
    #define P_ITOH284U M_ITOH284(P,)
    #define VP_ITOH284U M_ITOH284(VP,)
    #define PMS_ITOH284U M_ITOH284(P,_MULTISQR)
#elif(HFEn==285)
    #define M_ITOH285(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##EVEN_CASE(71);\
        V##EVEN_CASE(142);

    #define ITOH285U M_ITOH285(,)
    #define P_ITOH285U M_ITOH285(P,)
    #define VP_ITOH285U M_ITOH285(VP,)
    #define PMS_ITOH285U M_ITOH285(P,_MULTISQR)
#elif(HFEn==286)
    #define M_ITOH286(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##EVEN_CASE(71);\
        V##ODD_CASE(142);

    #define ITOH286U M_ITOH286(,)
    #define P_ITOH286U M_ITOH286(P,)
    #define VP_ITOH286U M_ITOH286(VP,)
    #define PMS_ITOH286U M_ITOH286(P,_MULTISQR)
#elif(HFEn==287)
    #define M_ITOH287(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##ODD_CASE(71);\
        V##EVEN_CASE(143);

    #define ITOH287U M_ITOH287(,)
    #define P_ITOH287U M_ITOH287(P,)
    #define VP_ITOH287U M_ITOH287(VP,)
    #define PMS_ITOH287U M_ITOH287(P,_MULTISQR)
#elif(HFEn==288)
    #define M_ITOH288(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##ODD_CASE(71);\
        V##ODD_CASE(143);

    #define ITOH288U M_ITOH288(,)
    #define P_ITOH288U M_ITOH288(P,)
    #define VP_ITOH288U M_ITOH288(VP,)
    #define PMS_ITOH288U M_ITOH288(P,_MULTISQR)
#elif(HFEn==289)
    #define M_ITOH289(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##EVEN_CASE(36);\
        V##EVEN_CASE(72);\
        V##EVEN_CASE(144);

    #define ITOH289U M_ITOH289(,)
    #define P_ITOH289U M_ITOH289(P,)
    #define VP_ITOH289U M_ITOH289(VP,)
    #define PMS_ITOH289U M_ITOH289(P,_MULTISQR)
#elif(HFEn==290)
    #define M_ITOH290(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##EVEN_CASE(36);\
        V##EVEN_CASE(72);\
        V##ODD_CASE(144);

    #define ITOH290U M_ITOH290(,)
    #define P_ITOH290U M_ITOH290(P,)
    #define VP_ITOH290U M_ITOH290(VP,)
    #define PMS_ITOH290U M_ITOH290(P,_MULTISQR)
#elif(HFEn==291)
    #define M_ITOH291(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##EVEN_CASE(36);\
        V##ODD_CASE(72);\
        V##EVEN_CASE(145);

    #define ITOH291U M_ITOH291(,)
    #define P_ITOH291U M_ITOH291(P,)
    #define VP_ITOH291U M_ITOH291(VP,)
    #define PMS_ITOH291U M_ITOH291(P,_MULTISQR)
#elif(HFEn==292)
    #define M_ITOH292(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##EVEN_CASE(36);\
        V##ODD_CASE(72);\
        V##ODD_CASE(145);

    #define ITOH292U M_ITOH292(,)
    #define P_ITOH292U M_ITOH292(P,)
    #define VP_ITOH292U M_ITOH292(VP,)
    #define PMS_ITOH292U M_ITOH292(P,_MULTISQR)
#elif(HFEn==293)
    #define M_ITOH293(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##ODD_CASE(36);\
        V##EVEN_CASE(73);\
        V##EVEN_CASE(146);

    #define ITOH293U M_ITOH293(,)
    #define P_ITOH293U M_ITOH293(P,)
    #define VP_ITOH293U M_ITOH293(VP,)
    #define PMS_ITOH293U M_ITOH293(P,_MULTISQR)
#elif(HFEn==294)
    #define M_ITOH294(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##ODD_CASE(36);\
        V##EVEN_CASE(73);\
        V##ODD_CASE(146);

    #define ITOH294U M_ITOH294(,)
    #define P_ITOH294U M_ITOH294(P,)
    #define VP_ITOH294U M_ITOH294(VP,)
    #define PMS_ITOH294U M_ITOH294(P,_MULTISQR)
#elif(HFEn==295)
    #define M_ITOH295(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##ODD_CASE(36);\
        V##ODD_CASE(73);\
        V##EVEN_CASE(147);

    #define ITOH295U M_ITOH295(,)
    #define P_ITOH295U M_ITOH295(P,)
    #define VP_ITOH295U M_ITOH295(VP,)
    #define PMS_ITOH295U M_ITOH295(P,_MULTISQR)
#elif(HFEn==296)
    #define M_ITOH296(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##EVEN_CASE(18);\
        V##ODD_CASE(36);\
        V##ODD_CASE(73);\
        V##ODD_CASE(147);

    #define ITOH296U M_ITOH296(,)
    #define P_ITOH296U M_ITOH296(P,)
    #define VP_ITOH296U M_ITOH296(VP,)
    #define PMS_ITOH296U M_ITOH296(P,_MULTISQR)
#elif(HFEn==297)
    #define M_ITOH297(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##EVEN_CASE(37);\
        V##EVEN_CASE(74);\
        V##EVEN_CASE(148);

    #define ITOH297U M_ITOH297(,)
    #define P_ITOH297U M_ITOH297(P,)
    #define VP_ITOH297U M_ITOH297(VP,)
    #define PMS_ITOH297U M_ITOH297(P,_MULTISQR)
#elif(HFEn==298)
    #define M_ITOH298(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##EVEN_CASE(37);\
        V##EVEN_CASE(74);\
        V##ODD_CASE(148);

    #define ITOH298U M_ITOH298(,)
    #define P_ITOH298U M_ITOH298(P,)
    #define VP_ITOH298U M_ITOH298(VP,)
    #define PMS_ITOH298U M_ITOH298(P,_MULTISQR)
#elif(HFEn==299)
    #define M_ITOH299(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##EVEN_CASE(37);\
        V##ODD_CASE(74);\
        V##EVEN_CASE(149);

    #define ITOH299U M_ITOH299(,)
    #define P_ITOH299U M_ITOH299(P,)
    #define VP_ITOH299U M_ITOH299(VP,)
    #define PMS_ITOH299U M_ITOH299(P,_MULTISQR)
#elif(HFEn==300)
    #define M_ITOH300(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##EVEN_CASE(37);\
        V##ODD_CASE(74);\
        V##ODD_CASE(149);

    #define ITOH300U M_ITOH300(,)
    #define P_ITOH300U M_ITOH300(P,)
    #define VP_ITOH300U M_ITOH300(VP,)
    #define PMS_ITOH300U M_ITOH300(P,_MULTISQR)
#elif(HFEn==301)
    #define M_ITOH301(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##ODD_CASE(37);\
        V##EVEN_CASE(75);\
        V##EVEN_CASE(150);

    #define ITOH301U M_ITOH301(,)
    #define P_ITOH301U M_ITOH301(P,)
    #define VP_ITOH301U M_ITOH301(VP,)
    #define PMS_ITOH301U M_ITOH301(P,_MULTISQR)
#elif(HFEn==302)
    #define M_ITOH302(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##ODD_CASE(37);\
        V##EVEN_CASE(75);\
        V##ODD_CASE(150);

    #define ITOH302U M_ITOH302(,)
    #define P_ITOH302U M_ITOH302(P,)
    #define VP_ITOH302U M_ITOH302(VP,)
    #define PMS_ITOH302U M_ITOH302(P,_MULTISQR)
#elif(HFEn==303)
    #define M_ITOH303(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##ODD_CASE(37);\
        V##ODD_CASE(75);\
        V##EVEN_CASE(151);

    #define ITOH303U M_ITOH303(,)
    #define P_ITOH303U M_ITOH303(P,)
    #define VP_ITOH303U M_ITOH303(VP,)
    #define PMS_ITOH303U M_ITOH303(P,_MULTISQR)
#elif(HFEn==304)
    #define M_ITOH304(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##EVEN_CASE(9);\
        V##ODD_CASE(18);\
        V##ODD_CASE(37);\
        V##ODD_CASE(75);\
        V##ODD_CASE(151);

    #define ITOH304U M_ITOH304(,)
    #define P_ITOH304U M_ITOH304(P,)
    #define VP_ITOH304U M_ITOH304(VP,)
    #define PMS_ITOH304U M_ITOH304(P,_MULTISQR)
#elif(HFEn==305)
    #define M_ITOH305(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##EVEN_CASE(38);\
        V##EVEN_CASE(76);\
        V##EVEN_CASE(152);

    #define ITOH305U M_ITOH305(,)
    #define P_ITOH305U M_ITOH305(P,)
    #define VP_ITOH305U M_ITOH305(VP,)
    #define PMS_ITOH305U M_ITOH305(P,_MULTISQR)
#elif(HFEn==306)
    #define M_ITOH306(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##EVEN_CASE(38);\
        V##EVEN_CASE(76);\
        V##ODD_CASE(152);

    #define ITOH306U M_ITOH306(,)
    #define P_ITOH306U M_ITOH306(P,)
    #define VP_ITOH306U M_ITOH306(VP,)
    #define PMS_ITOH306U M_ITOH306(P,_MULTISQR)
#elif(HFEn==307)
    #define M_ITOH307(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##EVEN_CASE(38);\
        V##ODD_CASE(76);\
        V##EVEN_CASE(153);

    #define ITOH307U M_ITOH307(,)
    #define P_ITOH307U M_ITOH307(P,)
    #define VP_ITOH307U M_ITOH307(VP,)
    #define PMS_ITOH307U M_ITOH307(P,_MULTISQR)
#elif(HFEn==308)
    #define M_ITOH308(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##EVEN_CASE(38);\
        V##ODD_CASE(76);\
        V##ODD_CASE(153);

    #define ITOH308U M_ITOH308(,)
    #define P_ITOH308U M_ITOH308(P,)
    #define VP_ITOH308U M_ITOH308(VP,)
    #define PMS_ITOH308U M_ITOH308(P,_MULTISQR)
#elif(HFEn==309)
    #define M_ITOH309(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##ODD_CASE(38);\
        V##EVEN_CASE(77);\
        V##EVEN_CASE(154);

    #define ITOH309U M_ITOH309(,)
    #define P_ITOH309U M_ITOH309(P,)
    #define VP_ITOH309U M_ITOH309(VP,)
    #define PMS_ITOH309U M_ITOH309(P,_MULTISQR)
#elif(HFEn==310)
    #define M_ITOH310(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##ODD_CASE(38);\
        V##EVEN_CASE(77);\
        V##ODD_CASE(154);

    #define ITOH310U M_ITOH310(,)
    #define P_ITOH310U M_ITOH310(P,)
    #define VP_ITOH310U M_ITOH310(VP,)
    #define PMS_ITOH310U M_ITOH310(P,_MULTISQR)
#elif(HFEn==311)
    #define M_ITOH311(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##ODD_CASE(38);\
        V##ODD_CASE(77);\
        V##EVEN_CASE(155);

    #define ITOH311U M_ITOH311(,)
    #define P_ITOH311U M_ITOH311(P,)
    #define VP_ITOH311U M_ITOH311(VP,)
    #define PMS_ITOH311U M_ITOH311(P,_MULTISQR)
#elif(HFEn==312)
    #define M_ITOH312(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##EVEN_CASE(19);\
        V##ODD_CASE(38);\
        V##ODD_CASE(77);\
        V##ODD_CASE(155);

    #define ITOH312U M_ITOH312(,)
    #define P_ITOH312U M_ITOH312(P,)
    #define VP_ITOH312U M_ITOH312(VP,)
    #define PMS_ITOH312U M_ITOH312(P,_MULTISQR)
#elif(HFEn==313)
    #define M_ITOH313(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##EVEN_CASE(39);\
        V##EVEN_CASE(78);\
        V##EVEN_CASE(156);

    #define ITOH313U M_ITOH313(,)
    #define P_ITOH313U M_ITOH313(P,)
    #define VP_ITOH313U M_ITOH313(VP,)
    #define PMS_ITOH313U M_ITOH313(P,_MULTISQR)
#elif(HFEn==314)
    #define M_ITOH314(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##EVEN_CASE(39);\
        V##EVEN_CASE(78);\
        V##ODD_CASE(156);

    #define ITOH314U M_ITOH314(,)
    #define P_ITOH314U M_ITOH314(P,)
    #define VP_ITOH314U M_ITOH314(VP,)
    #define PMS_ITOH314U M_ITOH314(P,_MULTISQR)
#elif(HFEn==315)
    #define M_ITOH315(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##EVEN_CASE(39);\
        V##ODD_CASE(78);\
        V##EVEN_CASE(157);

    #define ITOH315U M_ITOH315(,)
    #define P_ITOH315U M_ITOH315(P,)
    #define VP_ITOH315U M_ITOH315(VP,)
    #define PMS_ITOH315U M_ITOH315(P,_MULTISQR)
#elif(HFEn==316)
    #define M_ITOH316(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##EVEN_CASE(39);\
        V##ODD_CASE(78);\
        V##ODD_CASE(157);

    #define ITOH316U M_ITOH316(,)
    #define P_ITOH316U M_ITOH316(P,)
    #define VP_ITOH316U M_ITOH316(VP,)
    #define PMS_ITOH316U M_ITOH316(P,_MULTISQR)
#elif(HFEn==317)
    #define M_ITOH317(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##ODD_CASE(39);\
        V##EVEN_CASE(79);\
        V##EVEN_CASE(158);

    #define ITOH317U M_ITOH317(,)
    #define P_ITOH317U M_ITOH317(P,)
    #define VP_ITOH317U M_ITOH317(VP,)
    #define PMS_ITOH317U M_ITOH317(P,_MULTISQR)
#elif(HFEn==318)
    #define M_ITOH318(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##ODD_CASE(39);\
        V##EVEN_CASE(79);\
        V##ODD_CASE(158);

    #define ITOH318U M_ITOH318(,)
    #define P_ITOH318U M_ITOH318(P,)
    #define VP_ITOH318U M_ITOH318(VP,)
    #define PMS_ITOH318U M_ITOH318(P,_MULTISQR)
#elif(HFEn==319)
    #define M_ITOH319(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##ODD_CASE(39);\
        V##ODD_CASE(79);\
        V##EVEN_CASE(159);

    #define ITOH319U M_ITOH319(,)
    #define P_ITOH319U M_ITOH319(P,)
    #define VP_ITOH319U M_ITOH319(VP,)
    #define PMS_ITOH319U M_ITOH319(P,_MULTISQR)
#elif(HFEn==320)
    #define M_ITOH320(V,MS)\
        V##EVEN_CASE(2);\
        V##ODD_CASE(4);\
        V##ODD_CASE(9);\
        V##ODD_CASE(19);\
        V##ODD_CASE(39);\
        V##ODD_CASE(79);\
        V##ODD_CASE(159);

    #define ITOH320U M_ITOH320(,)
    #define P_ITOH320U M_ITOH320(P,)
    #define VP_ITOH320U M_ITOH320(VP,)
    #define PMS_ITOH320U M_ITOH320(P,_MULTISQR)
#elif(HFEn==321)
    #define M_ITOH321(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##EVEN_CASE(40);\
        V##EVEN_CASE(80);\
        V##EVEN_CASE(160);

    #define ITOH321U M_ITOH321(,)
    #define P_ITOH321U M_ITOH321(P,)
    #define VP_ITOH321U M_ITOH321(VP,)
    #define PMS_ITOH321U M_ITOH321(P,_MULTISQR)
#elif(HFEn==322)
    #define M_ITOH322(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##EVEN_CASE(40);\
        V##EVEN_CASE(80);\
        V##ODD_CASE(160);

    #define ITOH322U M_ITOH322(,)
    #define P_ITOH322U M_ITOH322(P,)
    #define VP_ITOH322U M_ITOH322(VP,)
    #define PMS_ITOH322U M_ITOH322(P,_MULTISQR)
#elif(HFEn==323)
    #define M_ITOH323(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##EVEN_CASE(40);\
        V##ODD_CASE(80);\
        V##EVEN_CASE(161);

    #define ITOH323U M_ITOH323(,)
    #define P_ITOH323U M_ITOH323(P,)
    #define VP_ITOH323U M_ITOH323(VP,)
    #define PMS_ITOH323U M_ITOH323(P,_MULTISQR)
#elif(HFEn==324)
    #define M_ITOH324(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##EVEN_CASE(40);\
        V##ODD_CASE(80);\
        V##ODD_CASE(161);

    #define ITOH324U M_ITOH324(,)
    #define P_ITOH324U M_ITOH324(P,)
    #define VP_ITOH324U M_ITOH324(VP,)
    #define PMS_ITOH324U M_ITOH324(P,_MULTISQR)
#elif(HFEn==325)
    #define M_ITOH325(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##ODD_CASE(40);\
        V##EVEN_CASE(81);\
        V##EVEN_CASE(162);

    #define ITOH325U M_ITOH325(,)
    #define P_ITOH325U M_ITOH325(P,)
    #define VP_ITOH325U M_ITOH325(VP,)
    #define PMS_ITOH325U M_ITOH325(P,_MULTISQR)
#elif(HFEn==326)
    #define M_ITOH326(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##ODD_CASE(40);\
        V##EVEN_CASE(81);\
        V##ODD_CASE(162);

    #define ITOH326U M_ITOH326(,)
    #define P_ITOH326U M_ITOH326(P,)
    #define VP_ITOH326U M_ITOH326(VP,)
    #define PMS_ITOH326U M_ITOH326(P,_MULTISQR)
#elif(HFEn==327)
    #define M_ITOH327(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##ODD_CASE(40);\
        V##ODD_CASE(81);\
        V##EVEN_CASE(163);

    #define ITOH327U M_ITOH327(,)
    #define P_ITOH327U M_ITOH327(P,)
    #define VP_ITOH327U M_ITOH327(VP,)
    #define PMS_ITOH327U M_ITOH327(P,_MULTISQR)
#elif(HFEn==328)
    #define M_ITOH328(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##EVEN_CASE(20);\
        V##ODD_CASE(40);\
        V##ODD_CASE(81);\
        V##ODD_CASE(163);

    #define ITOH328U M_ITOH328(,)
    #define P_ITOH328U M_ITOH328(P,)
    #define VP_ITOH328U M_ITOH328(VP,)
    #define PMS_ITOH328U M_ITOH328(P,_MULTISQR)
#elif(HFEn==329)
    #define M_ITOH329(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##EVEN_CASE(41);\
        V##EVEN_CASE(82);\
        V##EVEN_CASE(164);

    #define ITOH329U M_ITOH329(,)
    #define P_ITOH329U M_ITOH329(P,)
    #define VP_ITOH329U M_ITOH329(VP,)
    #define PMS_ITOH329U M_ITOH329(P,_MULTISQR)
#elif(HFEn==330)
    #define M_ITOH330(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##EVEN_CASE(41);\
        V##EVEN_CASE(82);\
        V##ODD_CASE(164);

    #define ITOH330U M_ITOH330(,)
    #define P_ITOH330U M_ITOH330(P,)
    #define VP_ITOH330U M_ITOH330(VP,)
    #define PMS_ITOH330U M_ITOH330(P,_MULTISQR)
#elif(HFEn==331)
    #define M_ITOH331(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##EVEN_CASE(41);\
        V##ODD_CASE(82);\
        V##EVEN_CASE(165);

    #define ITOH331U M_ITOH331(,)
    #define P_ITOH331U M_ITOH331(P,)
    #define VP_ITOH331U M_ITOH331(VP,)
    #define PMS_ITOH331U M_ITOH331(P,_MULTISQR)
#elif(HFEn==332)
    #define M_ITOH332(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##EVEN_CASE(41);\
        V##ODD_CASE(82);\
        V##ODD_CASE(165);

    #define ITOH332U M_ITOH332(,)
    #define P_ITOH332U M_ITOH332(P,)
    #define VP_ITOH332U M_ITOH332(VP,)
    #define PMS_ITOH332U M_ITOH332(P,_MULTISQR)
#elif(HFEn==333)
    #define M_ITOH333(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##ODD_CASE(41);\
        V##EVEN_CASE(83);\
        V##EVEN_CASE(166);

    #define ITOH333U M_ITOH333(,)
    #define P_ITOH333U M_ITOH333(P,)
    #define VP_ITOH333U M_ITOH333(VP,)
    #define PMS_ITOH333U M_ITOH333(P,_MULTISQR)
#elif(HFEn==334)
    #define M_ITOH334(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##ODD_CASE(41);\
        V##EVEN_CASE(83);\
        V##ODD_CASE(166);

    #define ITOH334U M_ITOH334(,)
    #define P_ITOH334U M_ITOH334(P,)
    #define VP_ITOH334U M_ITOH334(VP,)
    #define PMS_ITOH334U M_ITOH334(P,_MULTISQR)
#elif(HFEn==335)
    #define M_ITOH335(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##ODD_CASE(41);\
        V##ODD_CASE(83);\
        V##EVEN_CASE(167);

    #define ITOH335U M_ITOH335(,)
    #define P_ITOH335U M_ITOH335(P,)
    #define VP_ITOH335U M_ITOH335(VP,)
    #define PMS_ITOH335U M_ITOH335(P,_MULTISQR)
#elif(HFEn==336)
    #define M_ITOH336(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##EVEN_CASE(10);\
        V##ODD_CASE(20);\
        V##ODD_CASE(41);\
        V##ODD_CASE(83);\
        V##ODD_CASE(167);

    #define ITOH336U M_ITOH336(,)
    #define P_ITOH336U M_ITOH336(P,)
    #define VP_ITOH336U M_ITOH336(VP,)
    #define PMS_ITOH336U M_ITOH336(P,_MULTISQR)
#elif(HFEn==337)
    #define M_ITOH337(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##EVEN_CASE(42);\
        V##EVEN_CASE(84);\
        V##EVEN_CASE(168);

    #define ITOH337U M_ITOH337(,)
    #define P_ITOH337U M_ITOH337(P,)
    #define VP_ITOH337U M_ITOH337(VP,)
    #define PMS_ITOH337U M_ITOH337(P,_MULTISQR)
#elif(HFEn==338)
    #define M_ITOH338(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##EVEN_CASE(42);\
        V##EVEN_CASE(84);\
        V##ODD_CASE(168);

    #define ITOH338U M_ITOH338(,)
    #define P_ITOH338U M_ITOH338(P,)
    #define VP_ITOH338U M_ITOH338(VP,)
    #define PMS_ITOH338U M_ITOH338(P,_MULTISQR)
#elif(HFEn==339)
    #define M_ITOH339(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##EVEN_CASE(42);\
        V##ODD_CASE(84);\
        V##EVEN_CASE(169);

    #define ITOH339U M_ITOH339(,)
    #define P_ITOH339U M_ITOH339(P,)
    #define VP_ITOH339U M_ITOH339(VP,)
    #define PMS_ITOH339U M_ITOH339(P,_MULTISQR)
#elif(HFEn==340)
    #define M_ITOH340(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##EVEN_CASE(42);\
        V##ODD_CASE(84);\
        V##ODD_CASE(169);

    #define ITOH340U M_ITOH340(,)
    #define P_ITOH340U M_ITOH340(P,)
    #define VP_ITOH340U M_ITOH340(VP,)
    #define PMS_ITOH340U M_ITOH340(P,_MULTISQR)
#elif(HFEn==341)
    #define M_ITOH341(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##ODD_CASE(42);\
        V##EVEN_CASE(85);\
        V##EVEN_CASE(170);

    #define ITOH341U M_ITOH341(,)
    #define P_ITOH341U M_ITOH341(P,)
    #define VP_ITOH341U M_ITOH341(VP,)
    #define PMS_ITOH341U M_ITOH341(P,_MULTISQR)
#elif(HFEn==342)
    #define M_ITOH342(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##ODD_CASE(42);\
        V##EVEN_CASE(85);\
        V##ODD_CASE(170);

    #define ITOH342U M_ITOH342(,)
    #define P_ITOH342U M_ITOH342(P,)
    #define VP_ITOH342U M_ITOH342(VP,)
    #define PMS_ITOH342U M_ITOH342(P,_MULTISQR)
#elif(HFEn==343)
    #define M_ITOH343(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##ODD_CASE(42);\
        V##ODD_CASE(85);\
        V##EVEN_CASE(171);

    #define ITOH343U M_ITOH343(,)
    #define P_ITOH343U M_ITOH343(P,)
    #define VP_ITOH343U M_ITOH343(VP,)
    #define PMS_ITOH343U M_ITOH343(P,_MULTISQR)
#elif(HFEn==344)
    #define M_ITOH344(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##EVEN_CASE(21);\
        V##ODD_CASE(42);\
        V##ODD_CASE(85);\
        V##ODD_CASE(171);

    #define ITOH344U M_ITOH344(,)
    #define P_ITOH344U M_ITOH344(P,)
    #define VP_ITOH344U M_ITOH344(VP,)
    #define PMS_ITOH344U M_ITOH344(P,_MULTISQR)
#elif(HFEn==345)
    #define M_ITOH345(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##EVEN_CASE(43);\
        V##EVEN_CASE(86);\
        V##EVEN_CASE(172);

    #define ITOH345U M_ITOH345(,)
    #define P_ITOH345U M_ITOH345(P,)
    #define VP_ITOH345U M_ITOH345(VP,)
    #define PMS_ITOH345U M_ITOH345(P,_MULTISQR)
#elif(HFEn==346)
    #define M_ITOH346(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##EVEN_CASE(43);\
        V##EVEN_CASE(86);\
        V##ODD_CASE(172);

    #define ITOH346U M_ITOH346(,)
    #define P_ITOH346U M_ITOH346(P,)
    #define VP_ITOH346U M_ITOH346(VP,)
    #define PMS_ITOH346U M_ITOH346(P,_MULTISQR)
#elif(HFEn==347)
    #define M_ITOH347(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##EVEN_CASE(43);\
        V##ODD_CASE(86);\
        V##EVEN_CASE(173);

    #define ITOH347U M_ITOH347(,)
    #define P_ITOH347U M_ITOH347(P,)
    #define VP_ITOH347U M_ITOH347(VP,)
    #define PMS_ITOH347U M_ITOH347(P,_MULTISQR)
#elif(HFEn==348)
    #define M_ITOH348(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##EVEN_CASE(43);\
        V##ODD_CASE(86);\
        V##ODD_CASE(173);

    #define ITOH348U M_ITOH348(,)
    #define P_ITOH348U M_ITOH348(P,)
    #define VP_ITOH348U M_ITOH348(VP,)
    #define PMS_ITOH348U M_ITOH348(P,_MULTISQR)
#elif(HFEn==349)
    #define M_ITOH349(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##ODD_CASE(43);\
        V##EVEN_CASE(87);\
        V##EVEN_CASE(174);

    #define ITOH349U M_ITOH349(,)
    #define P_ITOH349U M_ITOH349(P,)
    #define VP_ITOH349U M_ITOH349(VP,)
    #define PMS_ITOH349U M_ITOH349(P,_MULTISQR)
#elif(HFEn==350)
    #define M_ITOH350(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##ODD_CASE(43);\
        V##EVEN_CASE(87);\
        V##ODD_CASE(174);

    #define ITOH350U M_ITOH350(,)
    #define P_ITOH350U M_ITOH350(P,)
    #define VP_ITOH350U M_ITOH350(VP,)
    #define PMS_ITOH350U M_ITOH350(P,_MULTISQR)
#elif(HFEn==351)
    #define M_ITOH351(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##ODD_CASE(43);\
        V##ODD_CASE(87);\
        V##EVEN_CASE(175);

    #define ITOH351U M_ITOH351(,)
    #define P_ITOH351U M_ITOH351(P,)
    #define VP_ITOH351U M_ITOH351(VP,)
    #define PMS_ITOH351U M_ITOH351(P,_MULTISQR)
#elif(HFEn==352)
    #define M_ITOH352(V,MS)\
        V##ODD_CASE(2);\
        V##EVEN_CASE(5);\
        V##ODD_CASE(10);\
        V##ODD_CASE(21);\
        V##ODD_CASE(43);\
        V##ODD_CASE(87);\
        V##ODD_CASE(175);

    #define ITOH352U M_ITOH352(,)
    #define P_ITOH352U M_ITOH352(P,)
    #define VP_ITOH352U M_ITOH352(VP,)
    #define PMS_ITOH352U M_ITOH352(P,_MULTISQR)
#elif(HFEn==353)
    #define M_ITOH353(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##EVEN_CASE(44);\
        V##EVEN_CASE(88);\
        V##EVEN_CASE(176);

    #define ITOH353U M_ITOH353(,)
    #define P_ITOH353U M_ITOH353(P,)
    #define VP_ITOH353U M_ITOH353(VP,)
    #define PMS_ITOH353U M_ITOH353(P,_MULTISQR)
#elif(HFEn==354)
    #define M_ITOH354(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##EVEN_CASE(44);\
        V##EVEN_CASE(88);\
        V##ODD_CASE(176);

    #define ITOH354U M_ITOH354(,)
    #define P_ITOH354U M_ITOH354(P,)
    #define VP_ITOH354U M_ITOH354(VP,)
    #define PMS_ITOH354U M_ITOH354(P,_MULTISQR)
#elif(HFEn==355)
    #define M_ITOH355(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##EVEN_CASE(44);\
        V##ODD_CASE(88);\
        V##EVEN_CASE(177);

    #define ITOH355U M_ITOH355(,)
    #define P_ITOH355U M_ITOH355(P,)
    #define VP_ITOH355U M_ITOH355(VP,)
    #define PMS_ITOH355U M_ITOH355(P,_MULTISQR)
#elif(HFEn==356)
    #define M_ITOH356(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##EVEN_CASE(44);\
        V##ODD_CASE(88);\
        V##ODD_CASE(177);

    #define ITOH356U M_ITOH356(,)
    #define P_ITOH356U M_ITOH356(P,)
    #define VP_ITOH356U M_ITOH356(VP,)
    #define PMS_ITOH356U M_ITOH356(P,_MULTISQR)
#elif(HFEn==357)
    #define M_ITOH357(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##ODD_CASE(44);\
        V##EVEN_CASE(89);\
        V##EVEN_CASE(178);

    #define ITOH357U M_ITOH357(,)
    #define P_ITOH357U M_ITOH357(P,)
    #define VP_ITOH357U M_ITOH357(VP,)
    #define PMS_ITOH357U M_ITOH357(P,_MULTISQR)
#elif(HFEn==358)
    #define M_ITOH358(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##ODD_CASE(44);\
        V##EVEN_CASE(89);\
        V##ODD_CASE(178);

    #define ITOH358U M_ITOH358(,)
    #define P_ITOH358U M_ITOH358(P,)
    #define VP_ITOH358U M_ITOH358(VP,)
    #define PMS_ITOH358U M_ITOH358(P,_MULTISQR)
#elif(HFEn==359)
    #define M_ITOH359(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##ODD_CASE(44);\
        V##ODD_CASE(89);\
        V##EVEN_CASE(179);

    #define ITOH359U M_ITOH359(,)
    #define P_ITOH359U M_ITOH359(P,)
    #define VP_ITOH359U M_ITOH359(VP,)
    #define PMS_ITOH359U M_ITOH359(P,_MULTISQR)
#elif(HFEn==360)
    #define M_ITOH360(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##EVEN_CASE(22);\
        V##ODD_CASE(44);\
        V##ODD_CASE(89);\
        V##ODD_CASE(179);

    #define ITOH360U M_ITOH360(,)
    #define P_ITOH360U M_ITOH360(P,)
    #define VP_ITOH360U M_ITOH360(VP,)
    #define PMS_ITOH360U M_ITOH360(P,_MULTISQR)
#elif(HFEn==361)
    #define M_ITOH361(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##EVEN_CASE(45);\
        V##EVEN_CASE(90);\
        V##EVEN_CASE(180);

    #define ITOH361U M_ITOH361(,)
    #define P_ITOH361U M_ITOH361(P,)
    #define VP_ITOH361U M_ITOH361(VP,)
    #define PMS_ITOH361U M_ITOH361(P,_MULTISQR)
#elif(HFEn==362)
    #define M_ITOH362(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##EVEN_CASE(45);\
        V##EVEN_CASE(90);\
        V##ODD_CASE(180);

    #define ITOH362U M_ITOH362(,)
    #define P_ITOH362U M_ITOH362(P,)
    #define VP_ITOH362U M_ITOH362(VP,)
    #define PMS_ITOH362U M_ITOH362(P,_MULTISQR)
#elif(HFEn==363)
    #define M_ITOH363(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##EVEN_CASE(45);\
        V##ODD_CASE(90);\
        V##EVEN_CASE(181);

    #define ITOH363U M_ITOH363(,)
    #define P_ITOH363U M_ITOH363(P,)
    #define VP_ITOH363U M_ITOH363(VP,)
    #define PMS_ITOH363U M_ITOH363(P,_MULTISQR)
#elif(HFEn==364)
    #define M_ITOH364(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##EVEN_CASE(45);\
        V##ODD_CASE(90);\
        V##ODD_CASE(181);

    #define ITOH364U M_ITOH364(,)
    #define P_ITOH364U M_ITOH364(P,)
    #define VP_ITOH364U M_ITOH364(VP,)
    #define PMS_ITOH364U M_ITOH364(P,_MULTISQR)
#elif(HFEn==365)
    #define M_ITOH365(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##ODD_CASE(45);\
        V##EVEN_CASE(91);\
        V##EVEN_CASE(182);

    #define ITOH365U M_ITOH365(,)
    #define P_ITOH365U M_ITOH365(P,)
    #define VP_ITOH365U M_ITOH365(VP,)
    #define PMS_ITOH365U M_ITOH365(P,_MULTISQR)
#elif(HFEn==366)
    #define M_ITOH366(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##ODD_CASE(45);\
        V##EVEN_CASE(91);\
        V##ODD_CASE(182);

    #define ITOH366U M_ITOH366(,)
    #define P_ITOH366U M_ITOH366(P,)
    #define VP_ITOH366U M_ITOH366(VP,)
    #define PMS_ITOH366U M_ITOH366(P,_MULTISQR)
#elif(HFEn==367)
    #define M_ITOH367(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##ODD_CASE(45);\
        V##ODD_CASE(91);\
        V##EVEN_CASE(183);

    #define ITOH367U M_ITOH367(,)
    #define P_ITOH367U M_ITOH367(P,)
    #define VP_ITOH367U M_ITOH367(VP,)
    #define PMS_ITOH367U M_ITOH367(P,_MULTISQR)
#elif(HFEn==368)
    #define M_ITOH368(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##EVEN_CASE(11);\
        V##ODD_CASE(22);\
        V##ODD_CASE(45);\
        V##ODD_CASE(91);\
        V##ODD_CASE(183);

    #define ITOH368U M_ITOH368(,)
    #define P_ITOH368U M_ITOH368(P,)
    #define VP_ITOH368U M_ITOH368(VP,)
    #define PMS_ITOH368U M_ITOH368(P,_MULTISQR)
#elif(HFEn==369)
    #define M_ITOH369(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##EVEN_CASE(46);\
        V##EVEN_CASE(92);\
        V##EVEN_CASE(184);

    #define ITOH369U M_ITOH369(,)
    #define P_ITOH369U M_ITOH369(P,)
    #define VP_ITOH369U M_ITOH369(VP,)
    #define PMS_ITOH369U M_ITOH369(P,_MULTISQR)
#elif(HFEn==370)
    #define M_ITOH370(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##EVEN_CASE(46);\
        V##EVEN_CASE(92);\
        V##ODD_CASE(184);

    #define ITOH370U M_ITOH370(,)
    #define P_ITOH370U M_ITOH370(P,)
    #define VP_ITOH370U M_ITOH370(VP,)
    #define PMS_ITOH370U M_ITOH370(P,_MULTISQR)
#elif(HFEn==371)
    #define M_ITOH371(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##EVEN_CASE(46);\
        V##ODD_CASE(92);\
        V##EVEN_CASE(185);

    #define ITOH371U M_ITOH371(,)
    #define P_ITOH371U M_ITOH371(P,)
    #define VP_ITOH371U M_ITOH371(VP,)
    #define PMS_ITOH371U M_ITOH371(P,_MULTISQR)
#elif(HFEn==372)
    #define M_ITOH372(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##EVEN_CASE(46);\
        V##ODD_CASE(92);\
        V##ODD_CASE(185);

    #define ITOH372U M_ITOH372(,)
    #define P_ITOH372U M_ITOH372(P,)
    #define VP_ITOH372U M_ITOH372(VP,)
    #define PMS_ITOH372U M_ITOH372(P,_MULTISQR)
#elif(HFEn==373)
    #define M_ITOH373(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##ODD_CASE(46);\
        V##EVEN_CASE(93);\
        V##EVEN_CASE(186);

    #define ITOH373U M_ITOH373(,)
    #define P_ITOH373U M_ITOH373(P,)
    #define VP_ITOH373U M_ITOH373(VP,)
    #define PMS_ITOH373U M_ITOH373(P,_MULTISQR)
#elif(HFEn==374)
    #define M_ITOH374(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##ODD_CASE(46);\
        V##EVEN_CASE(93);\
        V##ODD_CASE(186);

    #define ITOH374U M_ITOH374(,)
    #define P_ITOH374U M_ITOH374(P,)
    #define VP_ITOH374U M_ITOH374(VP,)
    #define PMS_ITOH374U M_ITOH374(P,_MULTISQR)
#elif(HFEn==375)
    #define M_ITOH375(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##ODD_CASE(46);\
        V##ODD_CASE(93);\
        V##EVEN_CASE(187);

    #define ITOH375U M_ITOH375(,)
    #define P_ITOH375U M_ITOH375(P,)
    #define VP_ITOH375U M_ITOH375(VP,)
    #define PMS_ITOH375U M_ITOH375(P,_MULTISQR)
#elif(HFEn==376)
    #define M_ITOH376(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##EVEN_CASE(23);\
        V##ODD_CASE(46);\
        V##ODD_CASE(93);\
        V##ODD_CASE(187);

    #define ITOH376U M_ITOH376(,)
    #define P_ITOH376U M_ITOH376(P,)
    #define VP_ITOH376U M_ITOH376(VP,)
    #define PMS_ITOH376U M_ITOH376(P,_MULTISQR)
#elif(HFEn==377)
    #define M_ITOH377(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##EVEN_CASE(47);\
        V##EVEN_CASE(94);\
        V##EVEN_CASE(188);

    #define ITOH377U M_ITOH377(,)
    #define P_ITOH377U M_ITOH377(P,)
    #define VP_ITOH377U M_ITOH377(VP,)
    #define PMS_ITOH377U M_ITOH377(P,_MULTISQR)
#elif(HFEn==378)
    #define M_ITOH378(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##EVEN_CASE(47);\
        V##EVEN_CASE(94);\
        V##ODD_CASE(188);

    #define ITOH378U M_ITOH378(,)
    #define P_ITOH378U M_ITOH378(P,)
    #define VP_ITOH378U M_ITOH378(VP,)
    #define PMS_ITOH378U M_ITOH378(P,_MULTISQR)
#elif(HFEn==379)
    #define M_ITOH379(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##EVEN_CASE(47);\
        V##ODD_CASE(94);\
        V##EVEN_CASE(189);

    #define ITOH379U M_ITOH379(,)
    #define P_ITOH379U M_ITOH379(P,)
    #define VP_ITOH379U M_ITOH379(VP,)
    #define PMS_ITOH379U M_ITOH379(P,_MULTISQR)
#elif(HFEn==380)
    #define M_ITOH380(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##EVEN_CASE(47);\
        V##ODD_CASE(94);\
        V##ODD_CASE(189);

    #define ITOH380U M_ITOH380(,)
    #define P_ITOH380U M_ITOH380(P,)
    #define VP_ITOH380U M_ITOH380(VP,)
    #define PMS_ITOH380U M_ITOH380(P,_MULTISQR)
#elif(HFEn==381)
    #define M_ITOH381(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##ODD_CASE(47);\
        V##EVEN_CASE(95);\
        V##EVEN_CASE(190);

    #define ITOH381U M_ITOH381(,)
    #define P_ITOH381U M_ITOH381(P,)
    #define VP_ITOH381U M_ITOH381(VP,)
    #define PMS_ITOH381U M_ITOH381(P,_MULTISQR)
#elif(HFEn==382)
    #define M_ITOH382(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##ODD_CASE(47);\
        V##EVEN_CASE(95);\
        V##ODD_CASE(190);

    #define ITOH382U M_ITOH382(,)
    #define P_ITOH382U M_ITOH382(P,)
    #define VP_ITOH382U M_ITOH382(VP,)
    #define PMS_ITOH382U M_ITOH382(P,_MULTISQR)
#elif(HFEn==383)
    #define M_ITOH383(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##ODD_CASE(47);\
        V##ODD_CASE(95);\
        V##EVEN_CASE(191);

    #define ITOH383U M_ITOH383(,)
    #define P_ITOH383U M_ITOH383(P,)
    #define VP_ITOH383U M_ITOH383(VP,)
    #define PMS_ITOH383U M_ITOH383(P,_MULTISQR)
#elif(HFEn==384)
    #define M_ITOH384(V,MS)\
        V##ODD_CASE(2);\
        V##ODD_CASE(5);\
        V##ODD_CASE(11);\
        V##ODD_CASE(23);\
        V##ODD_CASE(47);\
        V##ODD_CASE(95);\
        V##ODD_CASE(191);

    #define ITOH384U M_ITOH384(,)
    #define P_ITOH384U M_ITOH384(P,)
    #define VP_ITOH384U M_ITOH384(VP,)
    #define PMS_ITOH384U M_ITOH384(P,_MULTISQR)
#elif(HFEn==385)
    #define M_ITOH385(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##EVEN_CASE(48);\
        V##EVEN_CASE(96);\
        V##EVEN_CASE(192);

    #define ITOH385U M_ITOH385(,)
    #define P_ITOH385U M_ITOH385(P,)
    #define VP_ITOH385U M_ITOH385(VP,)
    #define PMS_ITOH385U M_ITOH385(P,_MULTISQR)
#elif(HFEn==386)
    #define M_ITOH386(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##EVEN_CASE(48);\
        V##EVEN_CASE(96);\
        V##ODD_CASE(192);

    #define ITOH386U M_ITOH386(,)
    #define P_ITOH386U M_ITOH386(P,)
    #define VP_ITOH386U M_ITOH386(VP,)
    #define PMS_ITOH386U M_ITOH386(P,_MULTISQR)
#elif(HFEn==387)
    #define M_ITOH387(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##EVEN_CASE(48);\
        V##ODD_CASE(96);\
        V##EVEN_CASE(193);

    #define ITOH387U M_ITOH387(,)
    #define P_ITOH387U M_ITOH387(P,)
    #define VP_ITOH387U M_ITOH387(VP,)
    #define PMS_ITOH387U M_ITOH387(P,_MULTISQR)
#elif(HFEn==388)
    #define M_ITOH388(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##EVEN_CASE(48);\
        V##ODD_CASE(96);\
        V##ODD_CASE(193);

    #define ITOH388U M_ITOH388(,)
    #define P_ITOH388U M_ITOH388(P,)
    #define VP_ITOH388U M_ITOH388(VP,)
    #define PMS_ITOH388U M_ITOH388(P,_MULTISQR)
#elif(HFEn==389)
    #define M_ITOH389(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##ODD_CASE(48);\
        V##EVEN_CASE(97);\
        V##EVEN_CASE(194);

    #define ITOH389U M_ITOH389(,)
    #define P_ITOH389U M_ITOH389(P,)
    #define VP_ITOH389U M_ITOH389(VP,)
    #define PMS_ITOH389U M_ITOH389(P,_MULTISQR)
#elif(HFEn==390)
    #define M_ITOH390(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##ODD_CASE(48);\
        V##EVEN_CASE(97);\
        V##ODD_CASE(194);

    #define ITOH390U M_ITOH390(,)
    #define P_ITOH390U M_ITOH390(P,)
    #define VP_ITOH390U M_ITOH390(VP,)
    #define PMS_ITOH390U M_ITOH390(P,_MULTISQR)
#elif(HFEn==391)
    #define M_ITOH391(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##ODD_CASE(48);\
        V##ODD_CASE(97);\
        V##EVEN_CASE(195);

    #define ITOH391U M_ITOH391(,)
    #define P_ITOH391U M_ITOH391(P,)
    #define VP_ITOH391U M_ITOH391(VP,)
    #define PMS_ITOH391U M_ITOH391(P,_MULTISQR)
#elif(HFEn==392)
    #define M_ITOH392(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##EVEN_CASE(24);\
        V##ODD_CASE(48);\
        V##ODD_CASE(97);\
        V##ODD_CASE(195);

    #define ITOH392U M_ITOH392(,)
    #define P_ITOH392U M_ITOH392(P,)
    #define VP_ITOH392U M_ITOH392(VP,)
    #define PMS_ITOH392U M_ITOH392(P,_MULTISQR)
#elif(HFEn==393)
    #define M_ITOH393(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##EVEN_CASE(49);\
        V##EVEN_CASE(98);\
        V##EVEN_CASE(196);

    #define ITOH393U M_ITOH393(,)
    #define P_ITOH393U M_ITOH393(P,)
    #define VP_ITOH393U M_ITOH393(VP,)
    #define PMS_ITOH393U M_ITOH393(P,_MULTISQR)
#elif(HFEn==394)
    #define M_ITOH394(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##EVEN_CASE(49);\
        V##EVEN_CASE(98);\
        V##ODD_CASE(196);

    #define ITOH394U M_ITOH394(,)
    #define P_ITOH394U M_ITOH394(P,)
    #define VP_ITOH394U M_ITOH394(VP,)
    #define PMS_ITOH394U M_ITOH394(P,_MULTISQR)
#elif(HFEn==395)
    #define M_ITOH395(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##EVEN_CASE(49);\
        V##ODD_CASE(98);\
        V##EVEN_CASE(197);

    #define ITOH395U M_ITOH395(,)
    #define P_ITOH395U M_ITOH395(P,)
    #define VP_ITOH395U M_ITOH395(VP,)
    #define PMS_ITOH395U M_ITOH395(P,_MULTISQR)
#elif(HFEn==396)
    #define M_ITOH396(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##EVEN_CASE(49);\
        V##ODD_CASE(98);\
        V##ODD_CASE(197);

    #define ITOH396U M_ITOH396(,)
    #define P_ITOH396U M_ITOH396(P,)
    #define VP_ITOH396U M_ITOH396(VP,)
    #define PMS_ITOH396U M_ITOH396(P,_MULTISQR)
#elif(HFEn==397)
    #define M_ITOH397(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##ODD_CASE(49);\
        V##EVEN_CASE(99);\
        V##EVEN_CASE(198);

    #define ITOH397U M_ITOH397(,)
    #define P_ITOH397U M_ITOH397(P,)
    #define VP_ITOH397U M_ITOH397(VP,)
    #define PMS_ITOH397U M_ITOH397(P,_MULTISQR)
#elif(HFEn==398)
    #define M_ITOH398(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##ODD_CASE(49);\
        V##EVEN_CASE(99);\
        V##ODD_CASE(198);

    #define ITOH398U M_ITOH398(,)
    #define P_ITOH398U M_ITOH398(P,)
    #define VP_ITOH398U M_ITOH398(VP,)
    #define PMS_ITOH398U M_ITOH398(P,_MULTISQR)
#elif(HFEn==399)
    #define M_ITOH399(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##ODD_CASE(49);\
        V##ODD_CASE(99);\
        V##EVEN_CASE(199);

    #define ITOH399U M_ITOH399(,)
    #define P_ITOH399U M_ITOH399(P,)
    #define VP_ITOH399U M_ITOH399(VP,)
    #define PMS_ITOH399U M_ITOH399(P,_MULTISQR)
#elif(HFEn==400)
    #define M_ITOH400(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##EVEN_CASE(12);\
        V##ODD_CASE(24);\
        V##ODD_CASE(49);\
        V##ODD_CASE(99);\
        V##ODD_CASE(199);

    #define ITOH400U M_ITOH400(,)
    #define P_ITOH400U M_ITOH400(P,)
    #define VP_ITOH400U M_ITOH400(VP,)
    #define PMS_ITOH400U M_ITOH400(P,_MULTISQR)
#elif(HFEn==401)
    #define M_ITOH401(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##EVEN_CASE(50);\
        V##EVEN_CASE(100);\
        V##EVEN_CASE(200);

    #define ITOH401U M_ITOH401(,)
    #define P_ITOH401U M_ITOH401(P,)
    #define VP_ITOH401U M_ITOH401(VP,)
    #define PMS_ITOH401U M_ITOH401(P,_MULTISQR)
#elif(HFEn==402)
    #define M_ITOH402(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##EVEN_CASE(50);\
        V##EVEN_CASE(100);\
        V##ODD_CASE(200);

    #define ITOH402U M_ITOH402(,)
    #define P_ITOH402U M_ITOH402(P,)
    #define VP_ITOH402U M_ITOH402(VP,)
    #define PMS_ITOH402U M_ITOH402(P,_MULTISQR)
#elif(HFEn==403)
    #define M_ITOH403(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##EVEN_CASE(50);\
        V##ODD_CASE(100);\
        V##EVEN_CASE(201);

    #define ITOH403U M_ITOH403(,)
    #define P_ITOH403U M_ITOH403(P,)
    #define VP_ITOH403U M_ITOH403(VP,)
    #define PMS_ITOH403U M_ITOH403(P,_MULTISQR)
#elif(HFEn==404)
    #define M_ITOH404(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##EVEN_CASE(50);\
        V##ODD_CASE(100);\
        V##ODD_CASE(201);

    #define ITOH404U M_ITOH404(,)
    #define P_ITOH404U M_ITOH404(P,)
    #define VP_ITOH404U M_ITOH404(VP,)
    #define PMS_ITOH404U M_ITOH404(P,_MULTISQR)
#elif(HFEn==405)
    #define M_ITOH405(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##ODD_CASE(50);\
        V##EVEN_CASE(101);\
        V##EVEN_CASE(202);

    #define ITOH405U M_ITOH405(,)
    #define P_ITOH405U M_ITOH405(P,)
    #define VP_ITOH405U M_ITOH405(VP,)
    #define PMS_ITOH405U M_ITOH405(P,_MULTISQR)
#elif(HFEn==406)
    #define M_ITOH406(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##ODD_CASE(50);\
        V##EVEN_CASE(101);\
        V##ODD_CASE(202);

    #define ITOH406U M_ITOH406(,)
    #define P_ITOH406U M_ITOH406(P,)
    #define VP_ITOH406U M_ITOH406(VP,)
    #define PMS_ITOH406U M_ITOH406(P,_MULTISQR)
#elif(HFEn==407)
    #define M_ITOH407(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##ODD_CASE(50);\
        V##ODD_CASE(101);\
        V##EVEN_CASE(203);

    #define ITOH407U M_ITOH407(,)
    #define P_ITOH407U M_ITOH407(P,)
    #define VP_ITOH407U M_ITOH407(VP,)
    #define PMS_ITOH407U M_ITOH407(P,_MULTISQR)
#elif(HFEn==408)
    #define M_ITOH408(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##EVEN_CASE(25);\
        V##ODD_CASE(50);\
        V##ODD_CASE(101);\
        V##ODD_CASE(203);

    #define ITOH408U M_ITOH408(,)
    #define P_ITOH408U M_ITOH408(P,)
    #define VP_ITOH408U M_ITOH408(VP,)
    #define PMS_ITOH408U M_ITOH408(P,_MULTISQR)
#elif(HFEn==409)
    #define M_ITOH409(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##EVEN_CASE(51);\
        V##EVEN_CASE(102);\
        V##EVEN_CASE(204);

    #define ITOH409U M_ITOH409(,)
    #define P_ITOH409U M_ITOH409(P,)
    #define VP_ITOH409U M_ITOH409(VP,)
    #define PMS_ITOH409U M_ITOH409(P,_MULTISQR)
#elif(HFEn==410)
    #define M_ITOH410(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##EVEN_CASE(51);\
        V##EVEN_CASE(102);\
        V##ODD_CASE(204);

    #define ITOH410U M_ITOH410(,)
    #define P_ITOH410U M_ITOH410(P,)
    #define VP_ITOH410U M_ITOH410(VP,)
    #define PMS_ITOH410U M_ITOH410(P,_MULTISQR)
#elif(HFEn==411)
    #define M_ITOH411(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##EVEN_CASE(51);\
        V##ODD_CASE(102);\
        V##EVEN_CASE(205);

    #define ITOH411U M_ITOH411(,)
    #define P_ITOH411U M_ITOH411(P,)
    #define VP_ITOH411U M_ITOH411(VP,)
    #define PMS_ITOH411U M_ITOH411(P,_MULTISQR)
#elif(HFEn==412)
    #define M_ITOH412(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##EVEN_CASE(51);\
        V##ODD_CASE(102);\
        V##ODD_CASE(205);

    #define ITOH412U M_ITOH412(,)
    #define P_ITOH412U M_ITOH412(P,)
    #define VP_ITOH412U M_ITOH412(VP,)
    #define PMS_ITOH412U M_ITOH412(P,_MULTISQR)
#elif(HFEn==413)
    #define M_ITOH413(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##ODD_CASE(51);\
        V##EVEN_CASE(103);\
        V##EVEN_CASE(206);

    #define ITOH413U M_ITOH413(,)
    #define P_ITOH413U M_ITOH413(P,)
    #define VP_ITOH413U M_ITOH413(VP,)
    #define PMS_ITOH413U M_ITOH413(P,_MULTISQR)
#elif(HFEn==414)
    #define M_ITOH414(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##ODD_CASE(51);\
        V##EVEN_CASE(103);\
        V##ODD_CASE(206);

    #define ITOH414U M_ITOH414(,)
    #define P_ITOH414U M_ITOH414(P,)
    #define VP_ITOH414U M_ITOH414(VP,)
    #define PMS_ITOH414U M_ITOH414(P,_MULTISQR)
#elif(HFEn==415)
    #define M_ITOH415(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##ODD_CASE(51);\
        V##ODD_CASE(103);\
        V##EVEN_CASE(207);

    #define ITOH415U M_ITOH415(,)
    #define P_ITOH415U M_ITOH415(P,)
    #define VP_ITOH415U M_ITOH415(VP,)
    #define PMS_ITOH415U M_ITOH415(P,_MULTISQR)
#elif(HFEn==416)
    #define M_ITOH416(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##EVEN_CASE(6);\
        V##ODD_CASE(12);\
        V##ODD_CASE(25);\
        V##ODD_CASE(51);\
        V##ODD_CASE(103);\
        V##ODD_CASE(207);

    #define ITOH416U M_ITOH416(,)
    #define P_ITOH416U M_ITOH416(P,)
    #define VP_ITOH416U M_ITOH416(VP,)
    #define PMS_ITOH416U M_ITOH416(P,_MULTISQR)
#elif(HFEn==417)
    #define M_ITOH417(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##EVEN_CASE(52);\
        V##EVEN_CASE(104);\
        V##EVEN_CASE(208);

    #define ITOH417U M_ITOH417(,)
    #define P_ITOH417U M_ITOH417(P,)
    #define VP_ITOH417U M_ITOH417(VP,)
    #define PMS_ITOH417U M_ITOH417(P,_MULTISQR)
#elif(HFEn==418)
    #define M_ITOH418(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##EVEN_CASE(52);\
        V##EVEN_CASE(104);\
        V##ODD_CASE(208);

    #define ITOH418U M_ITOH418(,)
    #define P_ITOH418U M_ITOH418(P,)
    #define VP_ITOH418U M_ITOH418(VP,)
    #define PMS_ITOH418U M_ITOH418(P,_MULTISQR)
#elif(HFEn==419)
    #define M_ITOH419(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##EVEN_CASE(52);\
        V##ODD_CASE(104);\
        V##EVEN_CASE(209);

    #define ITOH419U M_ITOH419(,)
    #define P_ITOH419U M_ITOH419(P,)
    #define VP_ITOH419U M_ITOH419(VP,)
    #define PMS_ITOH419U M_ITOH419(P,_MULTISQR)
#elif(HFEn==420)
    #define M_ITOH420(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##EVEN_CASE(52);\
        V##ODD_CASE(104);\
        V##ODD_CASE(209);

    #define ITOH420U M_ITOH420(,)
    #define P_ITOH420U M_ITOH420(P,)
    #define VP_ITOH420U M_ITOH420(VP,)
    #define PMS_ITOH420U M_ITOH420(P,_MULTISQR)
#elif(HFEn==421)
    #define M_ITOH421(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##ODD_CASE(52);\
        V##EVEN_CASE(105);\
        V##EVEN_CASE(210);

    #define ITOH421U M_ITOH421(,)
    #define P_ITOH421U M_ITOH421(P,)
    #define VP_ITOH421U M_ITOH421(VP,)
    #define PMS_ITOH421U M_ITOH421(P,_MULTISQR)
#elif(HFEn==422)
    #define M_ITOH422(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##ODD_CASE(52);\
        V##EVEN_CASE(105);\
        V##ODD_CASE(210);

    #define ITOH422U M_ITOH422(,)
    #define P_ITOH422U M_ITOH422(P,)
    #define VP_ITOH422U M_ITOH422(VP,)
    #define PMS_ITOH422U M_ITOH422(P,_MULTISQR)
#elif(HFEn==423)
    #define M_ITOH423(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##ODD_CASE(52);\
        V##ODD_CASE(105);\
        V##EVEN_CASE(211);

    #define ITOH423U M_ITOH423(,)
    #define P_ITOH423U M_ITOH423(P,)
    #define VP_ITOH423U M_ITOH423(VP,)
    #define PMS_ITOH423U M_ITOH423(P,_MULTISQR)
#elif(HFEn==424)
    #define M_ITOH424(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##EVEN_CASE(26);\
        V##ODD_CASE(52);\
        V##ODD_CASE(105);\
        V##ODD_CASE(211);

    #define ITOH424U M_ITOH424(,)
    #define P_ITOH424U M_ITOH424(P,)
    #define VP_ITOH424U M_ITOH424(VP,)
    #define PMS_ITOH424U M_ITOH424(P,_MULTISQR)
#elif(HFEn==425)
    #define M_ITOH425(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##EVEN_CASE(53);\
        V##EVEN_CASE(106);\
        V##EVEN_CASE(212);

    #define ITOH425U M_ITOH425(,)
    #define P_ITOH425U M_ITOH425(P,)
    #define VP_ITOH425U M_ITOH425(VP,)
    #define PMS_ITOH425U M_ITOH425(P,_MULTISQR)
#elif(HFEn==426)
    #define M_ITOH426(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##EVEN_CASE(53);\
        V##EVEN_CASE(106);\
        V##ODD_CASE(212);

    #define ITOH426U M_ITOH426(,)
    #define P_ITOH426U M_ITOH426(P,)
    #define VP_ITOH426U M_ITOH426(VP,)
    #define PMS_ITOH426U M_ITOH426(P,_MULTISQR)
#elif(HFEn==427)
    #define M_ITOH427(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##EVEN_CASE(53);\
        V##ODD_CASE(106);\
        V##EVEN_CASE(213);

    #define ITOH427U M_ITOH427(,)
    #define P_ITOH427U M_ITOH427(P,)
    #define VP_ITOH427U M_ITOH427(VP,)
    #define PMS_ITOH427U M_ITOH427(P,_MULTISQR)
#elif(HFEn==428)
    #define M_ITOH428(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##EVEN_CASE(53);\
        V##ODD_CASE(106);\
        V##ODD_CASE(213);

    #define ITOH428U M_ITOH428(,)
    #define P_ITOH428U M_ITOH428(P,)
    #define VP_ITOH428U M_ITOH428(VP,)
    #define PMS_ITOH428U M_ITOH428(P,_MULTISQR)
#elif(HFEn==429)
    #define M_ITOH429(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##ODD_CASE(53);\
        V##EVEN_CASE(107);\
        V##EVEN_CASE(214);

    #define ITOH429U M_ITOH429(,)
    #define P_ITOH429U M_ITOH429(P,)
    #define VP_ITOH429U M_ITOH429(VP,)
    #define PMS_ITOH429U M_ITOH429(P,_MULTISQR)
#elif(HFEn==430)
    #define M_ITOH430(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##ODD_CASE(53);\
        V##EVEN_CASE(107);\
        V##ODD_CASE(214);

    #define ITOH430U M_ITOH430(,)
    #define P_ITOH430U M_ITOH430(P,)
    #define VP_ITOH430U M_ITOH430(VP,)
    #define PMS_ITOH430U M_ITOH430(P,_MULTISQR)
#elif(HFEn==431)
    #define M_ITOH431(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##ODD_CASE(53);\
        V##ODD_CASE(107);\
        V##EVEN_CASE(215);

    #define ITOH431U M_ITOH431(,)
    #define P_ITOH431U M_ITOH431(P,)
    #define VP_ITOH431U M_ITOH431(VP,)
    #define PMS_ITOH431U M_ITOH431(P,_MULTISQR)
#elif(HFEn==432)
    #define M_ITOH432(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##EVEN_CASE(13);\
        V##ODD_CASE(26);\
        V##ODD_CASE(53);\
        V##ODD_CASE(107);\
        V##ODD_CASE(215);

    #define ITOH432U M_ITOH432(,)
    #define P_ITOH432U M_ITOH432(P,)
    #define VP_ITOH432U M_ITOH432(VP,)
    #define PMS_ITOH432U M_ITOH432(P,_MULTISQR)
#elif(HFEn==433)
    #define M_ITOH433(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##EVEN_CASE(54);\
        V##EVEN_CASE(108);\
        V##EVEN_CASE(216);

    #define ITOH433U M_ITOH433(,)
    #define P_ITOH433U M_ITOH433(P,)
    #define VP_ITOH433U M_ITOH433(VP,)
    #define PMS_ITOH433U M_ITOH433(P,_MULTISQR)
#elif(HFEn==434)
    #define M_ITOH434(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##EVEN_CASE(54);\
        V##EVEN_CASE(108);\
        V##ODD_CASE(216);

    #define ITOH434U M_ITOH434(,)
    #define P_ITOH434U M_ITOH434(P,)
    #define VP_ITOH434U M_ITOH434(VP,)
    #define PMS_ITOH434U M_ITOH434(P,_MULTISQR)
#elif(HFEn==435)
    #define M_ITOH435(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##EVEN_CASE(54);\
        V##ODD_CASE(108);\
        V##EVEN_CASE(217);

    #define ITOH435U M_ITOH435(,)
    #define P_ITOH435U M_ITOH435(P,)
    #define VP_ITOH435U M_ITOH435(VP,)
    #define PMS_ITOH435U M_ITOH435(P,_MULTISQR)
#elif(HFEn==436)
    #define M_ITOH436(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##EVEN_CASE(54);\
        V##ODD_CASE(108);\
        V##ODD_CASE(217);

    #define ITOH436U M_ITOH436(,)
    #define P_ITOH436U M_ITOH436(P,)
    #define VP_ITOH436U M_ITOH436(VP,)
    #define PMS_ITOH436U M_ITOH436(P,_MULTISQR)
#elif(HFEn==437)
    #define M_ITOH437(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##ODD_CASE(54);\
        V##EVEN_CASE(109);\
        V##EVEN_CASE(218);

    #define ITOH437U M_ITOH437(,)
    #define P_ITOH437U M_ITOH437(P,)
    #define VP_ITOH437U M_ITOH437(VP,)
    #define PMS_ITOH437U M_ITOH437(P,_MULTISQR)
#elif(HFEn==438)
    #define M_ITOH438(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##ODD_CASE(54);\
        V##EVEN_CASE(109);\
        V##ODD_CASE(218);

    #define ITOH438U M_ITOH438(,)
    #define P_ITOH438U M_ITOH438(P,)
    #define VP_ITOH438U M_ITOH438(VP,)
    #define PMS_ITOH438U M_ITOH438(P,_MULTISQR)
#elif(HFEn==439)
    #define M_ITOH439(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##ODD_CASE(54);\
        V##ODD_CASE(109);\
        V##EVEN_CASE(219);

    #define ITOH439U M_ITOH439(,)
    #define P_ITOH439U M_ITOH439(P,)
    #define VP_ITOH439U M_ITOH439(VP,)
    #define PMS_ITOH439U M_ITOH439(P,_MULTISQR)
#elif(HFEn==440)
    #define M_ITOH440(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##EVEN_CASE(27);\
        V##ODD_CASE(54);\
        V##ODD_CASE(109);\
        V##ODD_CASE(219);

    #define ITOH440U M_ITOH440(,)
    #define P_ITOH440U M_ITOH440(P,)
    #define VP_ITOH440U M_ITOH440(VP,)
    #define PMS_ITOH440U M_ITOH440(P,_MULTISQR)
#elif(HFEn==441)
    #define M_ITOH441(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##EVEN_CASE(55);\
        V##EVEN_CASE(110);\
        V##EVEN_CASE(220);

    #define ITOH441U M_ITOH441(,)
    #define P_ITOH441U M_ITOH441(P,)
    #define VP_ITOH441U M_ITOH441(VP,)
    #define PMS_ITOH441U M_ITOH441(P,_MULTISQR)
#elif(HFEn==442)
    #define M_ITOH442(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##EVEN_CASE(55);\
        V##EVEN_CASE(110);\
        V##ODD_CASE(220);

    #define ITOH442U M_ITOH442(,)
    #define P_ITOH442U M_ITOH442(P,)
    #define VP_ITOH442U M_ITOH442(VP,)
    #define PMS_ITOH442U M_ITOH442(P,_MULTISQR)
#elif(HFEn==443)
    #define M_ITOH443(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##EVEN_CASE(55);\
        V##ODD_CASE(110);\
        V##EVEN_CASE(221);

    #define ITOH443U M_ITOH443(,)
    #define P_ITOH443U M_ITOH443(P,)
    #define VP_ITOH443U M_ITOH443(VP,)
    #define PMS_ITOH443U M_ITOH443(P,_MULTISQR)
#elif(HFEn==444)
    #define M_ITOH444(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##EVEN_CASE(55);\
        V##ODD_CASE(110);\
        V##ODD_CASE(221);

    #define ITOH444U M_ITOH444(,)
    #define P_ITOH444U M_ITOH444(P,)
    #define VP_ITOH444U M_ITOH444(VP,)
    #define PMS_ITOH444U M_ITOH444(P,_MULTISQR)
#elif(HFEn==445)
    #define M_ITOH445(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##ODD_CASE(55);\
        V##EVEN_CASE(111);\
        V##EVEN_CASE(222);

    #define ITOH445U M_ITOH445(,)
    #define P_ITOH445U M_ITOH445(P,)
    #define VP_ITOH445U M_ITOH445(VP,)
    #define PMS_ITOH445U M_ITOH445(P,_MULTISQR)
#elif(HFEn==446)
    #define M_ITOH446(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##ODD_CASE(55);\
        V##EVEN_CASE(111);\
        V##ODD_CASE(222);

    #define ITOH446U M_ITOH446(,)
    #define P_ITOH446U M_ITOH446(P,)
    #define VP_ITOH446U M_ITOH446(VP,)
    #define PMS_ITOH446U M_ITOH446(P,_MULTISQR)
#elif(HFEn==447)
    #define M_ITOH447(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##ODD_CASE(55);\
        V##ODD_CASE(111);\
        V##EVEN_CASE(223);

    #define ITOH447U M_ITOH447(,)
    #define P_ITOH447U M_ITOH447(P,)
    #define VP_ITOH447U M_ITOH447(VP,)
    #define PMS_ITOH447U M_ITOH447(P,_MULTISQR)
#elif(HFEn==448)
    #define M_ITOH448(V,MS)\
        V##ODD_INIT;\
        V##EVEN_CASE(3);\
        V##ODD_CASE(6);\
        V##ODD_CASE(13);\
        V##ODD_CASE(27);\
        V##ODD_CASE(55);\
        V##ODD_CASE(111);\
        V##ODD_CASE(223);

    #define ITOH448U M_ITOH448(,)
    #define P_ITOH448U M_ITOH448(P,)
    #define VP_ITOH448U M_ITOH448(VP,)
    #define PMS_ITOH448U M_ITOH448(P,_MULTISQR)
#elif(HFEn==449)
    #define M_ITOH449(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##EVEN_CASE(56);\
        V##EVEN_CASE(112);\
        V##EVEN_CASE(224);

    #define ITOH449U M_ITOH449(,)
    #define P_ITOH449U M_ITOH449(P,)
    #define VP_ITOH449U M_ITOH449(VP,)
    #define PMS_ITOH449U M_ITOH449(P,_MULTISQR)
#elif(HFEn==450)
    #define M_ITOH450(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##EVEN_CASE(56);\
        V##EVEN_CASE(112);\
        V##ODD_CASE(224);

    #define ITOH450U M_ITOH450(,)
    #define P_ITOH450U M_ITOH450(P,)
    #define VP_ITOH450U M_ITOH450(VP,)
    #define PMS_ITOH450U M_ITOH450(P,_MULTISQR)
#elif(HFEn==451)
    #define M_ITOH451(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##EVEN_CASE(56);\
        V##ODD_CASE(112);\
        V##EVEN_CASE(225);

    #define ITOH451U M_ITOH451(,)
    #define P_ITOH451U M_ITOH451(P,)
    #define VP_ITOH451U M_ITOH451(VP,)
    #define PMS_ITOH451U M_ITOH451(P,_MULTISQR)
#elif(HFEn==452)
    #define M_ITOH452(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##EVEN_CASE(56);\
        V##ODD_CASE(112);\
        V##ODD_CASE(225);

    #define ITOH452U M_ITOH452(,)
    #define P_ITOH452U M_ITOH452(P,)
    #define VP_ITOH452U M_ITOH452(VP,)
    #define PMS_ITOH452U M_ITOH452(P,_MULTISQR)
#elif(HFEn==453)
    #define M_ITOH453(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##ODD_CASE(56);\
        V##EVEN_CASE(113);\
        V##EVEN_CASE(226);

    #define ITOH453U M_ITOH453(,)
    #define P_ITOH453U M_ITOH453(P,)
    #define VP_ITOH453U M_ITOH453(VP,)
    #define PMS_ITOH453U M_ITOH453(P,_MULTISQR)
#elif(HFEn==454)
    #define M_ITOH454(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##ODD_CASE(56);\
        V##EVEN_CASE(113);\
        V##ODD_CASE(226);

    #define ITOH454U M_ITOH454(,)
    #define P_ITOH454U M_ITOH454(P,)
    #define VP_ITOH454U M_ITOH454(VP,)
    #define PMS_ITOH454U M_ITOH454(P,_MULTISQR)
#elif(HFEn==455)
    #define M_ITOH455(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##ODD_CASE(56);\
        V##ODD_CASE(113);\
        V##EVEN_CASE(227);

    #define ITOH455U M_ITOH455(,)
    #define P_ITOH455U M_ITOH455(P,)
    #define VP_ITOH455U M_ITOH455(VP,)
    #define PMS_ITOH455U M_ITOH455(P,_MULTISQR)
#elif(HFEn==456)
    #define M_ITOH456(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##EVEN_CASE(28);\
        V##ODD_CASE(56);\
        V##ODD_CASE(113);\
        V##ODD_CASE(227);

    #define ITOH456U M_ITOH456(,)
    #define P_ITOH456U M_ITOH456(P,)
    #define VP_ITOH456U M_ITOH456(VP,)
    #define PMS_ITOH456U M_ITOH456(P,_MULTISQR)
#elif(HFEn==457)
    #define M_ITOH457(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##EVEN_CASE(57);\
        V##EVEN_CASE(114);\
        V##EVEN_CASE(228);

    #define ITOH457U M_ITOH457(,)
    #define P_ITOH457U M_ITOH457(P,)
    #define VP_ITOH457U M_ITOH457(VP,)
    #define PMS_ITOH457U M_ITOH457(P,_MULTISQR)
#elif(HFEn==458)
    #define M_ITOH458(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##EVEN_CASE(57);\
        V##EVEN_CASE(114);\
        V##ODD_CASE(228);

    #define ITOH458U M_ITOH458(,)
    #define P_ITOH458U M_ITOH458(P,)
    #define VP_ITOH458U M_ITOH458(VP,)
    #define PMS_ITOH458U M_ITOH458(P,_MULTISQR)
#elif(HFEn==459)
    #define M_ITOH459(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##EVEN_CASE(57);\
        V##ODD_CASE(114);\
        V##EVEN_CASE(229);

    #define ITOH459U M_ITOH459(,)
    #define P_ITOH459U M_ITOH459(P,)
    #define VP_ITOH459U M_ITOH459(VP,)
    #define PMS_ITOH459U M_ITOH459(P,_MULTISQR)
#elif(HFEn==460)
    #define M_ITOH460(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##EVEN_CASE(57);\
        V##ODD_CASE(114);\
        V##ODD_CASE(229);

    #define ITOH460U M_ITOH460(,)
    #define P_ITOH460U M_ITOH460(P,)
    #define VP_ITOH460U M_ITOH460(VP,)
    #define PMS_ITOH460U M_ITOH460(P,_MULTISQR)
#elif(HFEn==461)
    #define M_ITOH461(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##ODD_CASE(57);\
        V##EVEN_CASE(115);\
        V##EVEN_CASE(230);

    #define ITOH461U M_ITOH461(,)
    #define P_ITOH461U M_ITOH461(P,)
    #define VP_ITOH461U M_ITOH461(VP,)
    #define PMS_ITOH461U M_ITOH461(P,_MULTISQR)
#elif(HFEn==462)
    #define M_ITOH462(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##ODD_CASE(57);\
        V##EVEN_CASE(115);\
        V##ODD_CASE(230);

    #define ITOH462U M_ITOH462(,)
    #define P_ITOH462U M_ITOH462(P,)
    #define VP_ITOH462U M_ITOH462(VP,)
    #define PMS_ITOH462U M_ITOH462(P,_MULTISQR)
#elif(HFEn==463)
    #define M_ITOH463(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##ODD_CASE(57);\
        V##ODD_CASE(115);\
        V##EVEN_CASE(231);

    #define ITOH463U M_ITOH463(,)
    #define P_ITOH463U M_ITOH463(P,)
    #define VP_ITOH463U M_ITOH463(VP,)
    #define PMS_ITOH463U M_ITOH463(P,_MULTISQR)
#elif(HFEn==464)
    #define M_ITOH464(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##EVEN_CASE(14);\
        V##ODD_CASE(28);\
        V##ODD_CASE(57);\
        V##ODD_CASE(115);\
        V##ODD_CASE(231);

    #define ITOH464U M_ITOH464(,)
    #define P_ITOH464U M_ITOH464(P,)
    #define VP_ITOH464U M_ITOH464(VP,)
    #define PMS_ITOH464U M_ITOH464(P,_MULTISQR)
#elif(HFEn==465)
    #define M_ITOH465(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##EVEN_CASE(58);\
        V##EVEN_CASE(116);\
        V##EVEN_CASE(232);

    #define ITOH465U M_ITOH465(,)
    #define P_ITOH465U M_ITOH465(P,)
    #define VP_ITOH465U M_ITOH465(VP,)
    #define PMS_ITOH465U M_ITOH465(P,_MULTISQR)
#elif(HFEn==466)
    #define M_ITOH466(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##EVEN_CASE(58);\
        V##EVEN_CASE(116);\
        V##ODD_CASE(232);

    #define ITOH466U M_ITOH466(,)
    #define P_ITOH466U M_ITOH466(P,)
    #define VP_ITOH466U M_ITOH466(VP,)
    #define PMS_ITOH466U M_ITOH466(P,_MULTISQR)
#elif(HFEn==467)
    #define M_ITOH467(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##EVEN_CASE(58);\
        V##ODD_CASE(116);\
        V##EVEN_CASE(233);

    #define ITOH467U M_ITOH467(,)
    #define P_ITOH467U M_ITOH467(P,)
    #define VP_ITOH467U M_ITOH467(VP,)
    #define PMS_ITOH467U M_ITOH467(P,_MULTISQR)
#elif(HFEn==468)
    #define M_ITOH468(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##EVEN_CASE(58);\
        V##ODD_CASE(116);\
        V##ODD_CASE(233);

    #define ITOH468U M_ITOH468(,)
    #define P_ITOH468U M_ITOH468(P,)
    #define VP_ITOH468U M_ITOH468(VP,)
    #define PMS_ITOH468U M_ITOH468(P,_MULTISQR)
#elif(HFEn==469)
    #define M_ITOH469(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##ODD_CASE(58);\
        V##EVEN_CASE(117);\
        V##EVEN_CASE(234);

    #define ITOH469U M_ITOH469(,)
    #define P_ITOH469U M_ITOH469(P,)
    #define VP_ITOH469U M_ITOH469(VP,)
    #define PMS_ITOH469U M_ITOH469(P,_MULTISQR)
#elif(HFEn==470)
    #define M_ITOH470(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##ODD_CASE(58);\
        V##EVEN_CASE(117);\
        V##ODD_CASE(234);

    #define ITOH470U M_ITOH470(,)
    #define P_ITOH470U M_ITOH470(P,)
    #define VP_ITOH470U M_ITOH470(VP,)
    #define PMS_ITOH470U M_ITOH470(P,_MULTISQR)
#elif(HFEn==471)
    #define M_ITOH471(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##ODD_CASE(58);\
        V##ODD_CASE(117);\
        V##EVEN_CASE(235);

    #define ITOH471U M_ITOH471(,)
    #define P_ITOH471U M_ITOH471(P,)
    #define VP_ITOH471U M_ITOH471(VP,)
    #define PMS_ITOH471U M_ITOH471(P,_MULTISQR)
#elif(HFEn==472)
    #define M_ITOH472(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##EVEN_CASE(29);\
        V##ODD_CASE(58);\
        V##ODD_CASE(117);\
        V##ODD_CASE(235);

    #define ITOH472U M_ITOH472(,)
    #define P_ITOH472U M_ITOH472(P,)
    #define VP_ITOH472U M_ITOH472(VP,)
    #define PMS_ITOH472U M_ITOH472(P,_MULTISQR)
#elif(HFEn==473)
    #define M_ITOH473(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##EVEN_CASE(59);\
        V##EVEN_CASE(118);\
        V##EVEN_CASE(236);

    #define ITOH473U M_ITOH473(,)
    #define P_ITOH473U M_ITOH473(P,)
    #define VP_ITOH473U M_ITOH473(VP,)
    #define PMS_ITOH473U M_ITOH473(P,_MULTISQR)
#elif(HFEn==474)
    #define M_ITOH474(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##EVEN_CASE(59);\
        V##EVEN_CASE(118);\
        V##ODD_CASE(236);

    #define ITOH474U M_ITOH474(,)
    #define P_ITOH474U M_ITOH474(P,)
    #define VP_ITOH474U M_ITOH474(VP,)
    #define PMS_ITOH474U M_ITOH474(P,_MULTISQR)
#elif(HFEn==475)
    #define M_ITOH475(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##EVEN_CASE(59);\
        V##ODD_CASE(118);\
        V##EVEN_CASE(237);

    #define ITOH475U M_ITOH475(,)
    #define P_ITOH475U M_ITOH475(P,)
    #define VP_ITOH475U M_ITOH475(VP,)
    #define PMS_ITOH475U M_ITOH475(P,_MULTISQR)
#elif(HFEn==476)
    #define M_ITOH476(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##EVEN_CASE(59);\
        V##ODD_CASE(118);\
        V##ODD_CASE(237);

    #define ITOH476U M_ITOH476(,)
    #define P_ITOH476U M_ITOH476(P,)
    #define VP_ITOH476U M_ITOH476(VP,)
    #define PMS_ITOH476U M_ITOH476(P,_MULTISQR)
#elif(HFEn==477)
    #define M_ITOH477(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##ODD_CASE(59);\
        V##EVEN_CASE(119);\
        V##EVEN_CASE(238);

    #define ITOH477U M_ITOH477(,)
    #define P_ITOH477U M_ITOH477(P,)
    #define VP_ITOH477U M_ITOH477(VP,)
    #define PMS_ITOH477U M_ITOH477(P,_MULTISQR)
#elif(HFEn==478)
    #define M_ITOH478(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##ODD_CASE(59);\
        V##EVEN_CASE(119);\
        V##ODD_CASE(238);

    #define ITOH478U M_ITOH478(,)
    #define P_ITOH478U M_ITOH478(P,)
    #define VP_ITOH478U M_ITOH478(VP,)
    #define PMS_ITOH478U M_ITOH478(P,_MULTISQR)
#elif(HFEn==479)
    #define M_ITOH479(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##ODD_CASE(59);\
        V##ODD_CASE(119);\
        V##EVEN_CASE(239);

    #define ITOH479U M_ITOH479(,)
    #define P_ITOH479U M_ITOH479(P,)
    #define VP_ITOH479U M_ITOH479(VP,)
    #define PMS_ITOH479U M_ITOH479(P,_MULTISQR)
#elif(HFEn==480)
    #define M_ITOH480(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##EVEN_CASE(7);\
        V##ODD_CASE(14);\
        V##ODD_CASE(29);\
        V##ODD_CASE(59);\
        V##ODD_CASE(119);\
        V##ODD_CASE(239);

    #define ITOH480U M_ITOH480(,)
    #define P_ITOH480U M_ITOH480(P,)
    #define VP_ITOH480U M_ITOH480(VP,)
    #define PMS_ITOH480U M_ITOH480(P,_MULTISQR)
#elif(HFEn==481)
    #define M_ITOH481(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##EVEN_CASE(60);\
        V##EVEN_CASE(120);\
        V##EVEN_CASE(240);

    #define ITOH481U M_ITOH481(,)
    #define P_ITOH481U M_ITOH481(P,)
    #define VP_ITOH481U M_ITOH481(VP,)
    #define PMS_ITOH481U M_ITOH481(P,_MULTISQR)
#elif(HFEn==482)
    #define M_ITOH482(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##EVEN_CASE(60);\
        V##EVEN_CASE(120);\
        V##ODD_CASE(240);

    #define ITOH482U M_ITOH482(,)
    #define P_ITOH482U M_ITOH482(P,)
    #define VP_ITOH482U M_ITOH482(VP,)
    #define PMS_ITOH482U M_ITOH482(P,_MULTISQR)
#elif(HFEn==483)
    #define M_ITOH483(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##EVEN_CASE(60);\
        V##ODD_CASE(120);\
        V##EVEN_CASE(241);

    #define ITOH483U M_ITOH483(,)
    #define P_ITOH483U M_ITOH483(P,)
    #define VP_ITOH483U M_ITOH483(VP,)
    #define PMS_ITOH483U M_ITOH483(P,_MULTISQR)
#elif(HFEn==484)
    #define M_ITOH484(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##EVEN_CASE(60);\
        V##ODD_CASE(120);\
        V##ODD_CASE(241);

    #define ITOH484U M_ITOH484(,)
    #define P_ITOH484U M_ITOH484(P,)
    #define VP_ITOH484U M_ITOH484(VP,)
    #define PMS_ITOH484U M_ITOH484(P,_MULTISQR)
#elif(HFEn==485)
    #define M_ITOH485(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##ODD_CASE(60);\
        V##EVEN_CASE(121);\
        V##EVEN_CASE(242);

    #define ITOH485U M_ITOH485(,)
    #define P_ITOH485U M_ITOH485(P,)
    #define VP_ITOH485U M_ITOH485(VP,)
    #define PMS_ITOH485U M_ITOH485(P,_MULTISQR)
#elif(HFEn==486)
    #define M_ITOH486(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##ODD_CASE(60);\
        V##EVEN_CASE(121);\
        V##ODD_CASE(242);

    #define ITOH486U M_ITOH486(,)
    #define P_ITOH486U M_ITOH486(P,)
    #define VP_ITOH486U M_ITOH486(VP,)
    #define PMS_ITOH486U M_ITOH486(P,_MULTISQR)
#elif(HFEn==487)
    #define M_ITOH487(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##ODD_CASE(60);\
        V##ODD_CASE(121);\
        V##EVEN_CASE(243);

    #define ITOH487U M_ITOH487(,)
    #define P_ITOH487U M_ITOH487(P,)
    #define VP_ITOH487U M_ITOH487(VP,)
    #define PMS_ITOH487U M_ITOH487(P,_MULTISQR)
#elif(HFEn==488)
    #define M_ITOH488(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##EVEN_CASE(30);\
        V##ODD_CASE(60);\
        V##ODD_CASE(121);\
        V##ODD_CASE(243);

    #define ITOH488U M_ITOH488(,)
    #define P_ITOH488U M_ITOH488(P,)
    #define VP_ITOH488U M_ITOH488(VP,)
    #define PMS_ITOH488U M_ITOH488(P,_MULTISQR)
#elif(HFEn==489)
    #define M_ITOH489(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##EVEN_CASE(61);\
        V##EVEN_CASE(122);\
        V##EVEN_CASE(244);

    #define ITOH489U M_ITOH489(,)
    #define P_ITOH489U M_ITOH489(P,)
    #define VP_ITOH489U M_ITOH489(VP,)
    #define PMS_ITOH489U M_ITOH489(P,_MULTISQR)
#elif(HFEn==490)
    #define M_ITOH490(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##EVEN_CASE(61);\
        V##EVEN_CASE(122);\
        V##ODD_CASE(244);

    #define ITOH490U M_ITOH490(,)
    #define P_ITOH490U M_ITOH490(P,)
    #define VP_ITOH490U M_ITOH490(VP,)
    #define PMS_ITOH490U M_ITOH490(P,_MULTISQR)
#elif(HFEn==491)
    #define M_ITOH491(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##EVEN_CASE(61);\
        V##ODD_CASE(122);\
        V##EVEN_CASE(245);

    #define ITOH491U M_ITOH491(,)
    #define P_ITOH491U M_ITOH491(P,)
    #define VP_ITOH491U M_ITOH491(VP,)
    #define PMS_ITOH491U M_ITOH491(P,_MULTISQR)
#elif(HFEn==492)
    #define M_ITOH492(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##EVEN_CASE(61);\
        V##ODD_CASE(122);\
        V##ODD_CASE(245);

    #define ITOH492U M_ITOH492(,)
    #define P_ITOH492U M_ITOH492(P,)
    #define VP_ITOH492U M_ITOH492(VP,)
    #define PMS_ITOH492U M_ITOH492(P,_MULTISQR)
#elif(HFEn==493)
    #define M_ITOH493(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##ODD_CASE(61);\
        V##EVEN_CASE(123);\
        V##EVEN_CASE(246);

    #define ITOH493U M_ITOH493(,)
    #define P_ITOH493U M_ITOH493(P,)
    #define VP_ITOH493U M_ITOH493(VP,)
    #define PMS_ITOH493U M_ITOH493(P,_MULTISQR)
#elif(HFEn==494)
    #define M_ITOH494(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##ODD_CASE(61);\
        V##EVEN_CASE(123);\
        V##ODD_CASE(246);

    #define ITOH494U M_ITOH494(,)
    #define P_ITOH494U M_ITOH494(P,)
    #define VP_ITOH494U M_ITOH494(VP,)
    #define PMS_ITOH494U M_ITOH494(P,_MULTISQR)
#elif(HFEn==495)
    #define M_ITOH495(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##ODD_CASE(61);\
        V##ODD_CASE(123);\
        V##EVEN_CASE(247);

    #define ITOH495U M_ITOH495(,)
    #define P_ITOH495U M_ITOH495(P,)
    #define VP_ITOH495U M_ITOH495(VP,)
    #define PMS_ITOH495U M_ITOH495(P,_MULTISQR)
#elif(HFEn==496)
    #define M_ITOH496(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##EVEN_CASE(15);\
        V##ODD_CASE(30);\
        V##ODD_CASE(61);\
        V##ODD_CASE(123);\
        V##ODD_CASE(247);

    #define ITOH496U M_ITOH496(,)
    #define P_ITOH496U M_ITOH496(P,)
    #define VP_ITOH496U M_ITOH496(VP,)
    #define PMS_ITOH496U M_ITOH496(P,_MULTISQR)
#elif(HFEn==497)
    #define M_ITOH497(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##EVEN_CASE(62);\
        V##EVEN_CASE(124);\
        V##EVEN_CASE(248);

    #define ITOH497U M_ITOH497(,)
    #define P_ITOH497U M_ITOH497(P,)
    #define VP_ITOH497U M_ITOH497(VP,)
    #define PMS_ITOH497U M_ITOH497(P,_MULTISQR)
#elif(HFEn==498)
    #define M_ITOH498(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##EVEN_CASE(62);\
        V##EVEN_CASE(124);\
        V##ODD_CASE(248);

    #define ITOH498U M_ITOH498(,)
    #define P_ITOH498U M_ITOH498(P,)
    #define VP_ITOH498U M_ITOH498(VP,)
    #define PMS_ITOH498U M_ITOH498(P,_MULTISQR)
#elif(HFEn==499)
    #define M_ITOH499(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##EVEN_CASE(62);\
        V##ODD_CASE(124);\
        V##EVEN_CASE(249);

    #define ITOH499U M_ITOH499(,)
    #define P_ITOH499U M_ITOH499(P,)
    #define VP_ITOH499U M_ITOH499(VP,)
    #define PMS_ITOH499U M_ITOH499(P,_MULTISQR)
#elif(HFEn==500)
    #define M_ITOH500(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##EVEN_CASE(62);\
        V##ODD_CASE(124);\
        V##ODD_CASE(249);

    #define ITOH500U M_ITOH500(,)
    #define P_ITOH500U M_ITOH500(P,)
    #define VP_ITOH500U M_ITOH500(VP,)
    #define PMS_ITOH500U M_ITOH500(P,_MULTISQR)
#elif(HFEn==501)
    #define M_ITOH501(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##ODD_CASE(62);\
        V##EVEN_CASE(125);\
        V##EVEN_CASE(250);

    #define ITOH501U M_ITOH501(,)
    #define P_ITOH501U M_ITOH501(P,)
    #define VP_ITOH501U M_ITOH501(VP,)
    #define PMS_ITOH501U M_ITOH501(P,_MULTISQR)
#elif(HFEn==502)
    #define M_ITOH502(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##ODD_CASE(62);\
        V##EVEN_CASE(125);\
        V##ODD_CASE(250);

    #define ITOH502U M_ITOH502(,)
    #define P_ITOH502U M_ITOH502(P,)
    #define VP_ITOH502U M_ITOH502(VP,)
    #define PMS_ITOH502U M_ITOH502(P,_MULTISQR)
#elif(HFEn==503)
    #define M_ITOH503(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##ODD_CASE(62);\
        V##ODD_CASE(125);\
        V##EVEN_CASE(251);

    #define ITOH503U M_ITOH503(,)
    #define P_ITOH503U M_ITOH503(P,)
    #define VP_ITOH503U M_ITOH503(VP,)
    #define PMS_ITOH503U M_ITOH503(P,_MULTISQR)
#elif(HFEn==504)
    #define M_ITOH504(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##EVEN_CASE(31);\
        V##ODD_CASE(62);\
        V##ODD_CASE(125);\
        V##ODD_CASE(251);

    #define ITOH504U M_ITOH504(,)
    #define P_ITOH504U M_ITOH504(P,)
    #define VP_ITOH504U M_ITOH504(VP,)
    #define PMS_ITOH504U M_ITOH504(P,_MULTISQR)
#elif(HFEn==505)
    #define M_ITOH505(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##EVEN_CASE(63);\
        V##EVEN_CASE(126);\
        V##EVEN_CASE(252);

    #define ITOH505U M_ITOH505(,)
    #define P_ITOH505U M_ITOH505(P,)
    #define VP_ITOH505U M_ITOH505(VP,)
    #define PMS_ITOH505U M_ITOH505(P,_MULTISQR)
#elif(HFEn==506)
    #define M_ITOH506(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##EVEN_CASE(63);\
        V##EVEN_CASE(126);\
        V##ODD_CASE(252);

    #define ITOH506U M_ITOH506(,)
    #define P_ITOH506U M_ITOH506(P,)
    #define VP_ITOH506U M_ITOH506(VP,)
    #define PMS_ITOH506U M_ITOH506(P,_MULTISQR)
#elif(HFEn==507)
    #define M_ITOH507(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##EVEN_CASE(63);\
        V##ODD_CASE(126);\
        V##EVEN_CASE(253);

    #define ITOH507U M_ITOH507(,)
    #define P_ITOH507U M_ITOH507(P,)
    #define VP_ITOH507U M_ITOH507(VP,)
    #define PMS_ITOH507U M_ITOH507(P,_MULTISQR)
#elif(HFEn==508)
    #define M_ITOH508(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##EVEN_CASE(63);\
        V##ODD_CASE(126);\
        V##ODD_CASE(253);

    #define ITOH508U M_ITOH508(,)
    #define P_ITOH508U M_ITOH508(P,)
    #define VP_ITOH508U M_ITOH508(VP,)
    #define PMS_ITOH508U M_ITOH508(P,_MULTISQR)
#elif(HFEn==509)
    #define M_ITOH509(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##ODD_CASE(63);\
        V##EVEN_CASE(127);\
        V##EVEN_CASE(254);

    #define ITOH509U M_ITOH509(,)
    #define P_ITOH509U M_ITOH509(P,)
    #define VP_ITOH509U M_ITOH509(VP,)
    #define PMS_ITOH509U M_ITOH509(P,_MULTISQR)
#elif(HFEn==510)
    #define M_ITOH510(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##ODD_CASE(63);\
        V##EVEN_CASE(127);\
        V##ODD_CASE(254);

    #define ITOH510U M_ITOH510(,)
    #define P_ITOH510U M_ITOH510(P,)
    #define VP_ITOH510U M_ITOH510(VP,)
    #define PMS_ITOH510U M_ITOH510(P,_MULTISQR)
#elif(HFEn==511)
    #define M_ITOH511(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##ODD_CASE(63);\
        V##ODD_CASE(127);\
        V##EVEN_CASE(255);

    #define ITOH511U M_ITOH511(,)
    #define P_ITOH511U M_ITOH511(P,)
    #define VP_ITOH511U M_ITOH511(VP,)
    #define PMS_ITOH511U M_ITOH511(P,_MULTISQR)
#elif(HFEn==512)
    #define M_ITOH512(V,MS)\
        V##ODD_INIT;\
        V##ODD_CASE(3);\
        V##ODD_CASE(7);\
        V##ODD_CASE(15);\
        V##ODD_CASE(31);\
        V##ODD_CASE(63);\
        V##ODD_CASE(127);\
        V##ODD_CASE(255);

    #define ITOH512U M_ITOH512(,)
    #define P_ITOH512U M_ITOH512(P,)
    #define VP_ITOH512U M_ITOH512(VP,)
    #define PMS_ITOH512U M_ITOH512(P,_MULTISQR)
#elif(HFEn==513)
    #define M_ITOH513(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##EVEN_CASE(64);\
        V##EVEN_CASE(128);\
        V##EVEN_CASE(256);

    #define ITOH513U M_ITOH513(,)
    #define P_ITOH513U M_ITOH513(P,)
    #define VP_ITOH513U M_ITOH513(VP,)
    #define PMS_ITOH513U M_ITOH513(P,_MULTISQR)
#elif(HFEn==514)
    #define M_ITOH514(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##EVEN_CASE(64);\
        V##EVEN_CASE(128);\
        V##ODD_CASE(256);

    #define ITOH514U M_ITOH514(,)
    #define P_ITOH514U M_ITOH514(P,)
    #define VP_ITOH514U M_ITOH514(VP,)
    #define PMS_ITOH514U M_ITOH514(P,_MULTISQR)
#elif(HFEn==515)
    #define M_ITOH515(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##EVEN_CASE(64);\
        V##ODD_CASE(128);\
        V##EVEN_CASE(257);

    #define ITOH515U M_ITOH515(,)
    #define P_ITOH515U M_ITOH515(P,)
    #define VP_ITOH515U M_ITOH515(VP,)
    #define PMS_ITOH515U M_ITOH515(P,_MULTISQR)
#elif(HFEn==516)
    #define M_ITOH516(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##EVEN_CASE(64);\
        V##ODD_CASE(128);\
        V##ODD_CASE(257);

    #define ITOH516U M_ITOH516(,)
    #define P_ITOH516U M_ITOH516(P,)
    #define VP_ITOH516U M_ITOH516(VP,)
    #define PMS_ITOH516U M_ITOH516(P,_MULTISQR)
#elif(HFEn==517)
    #define M_ITOH517(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##ODD_CASE(64);\
        V##EVEN_CASE(129);\
        V##EVEN_CASE(258);

    #define ITOH517U M_ITOH517(,)
    #define P_ITOH517U M_ITOH517(P,)
    #define VP_ITOH517U M_ITOH517(VP,)
    #define PMS_ITOH517U M_ITOH517(P,_MULTISQR)
#elif(HFEn==518)
    #define M_ITOH518(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##ODD_CASE(64);\
        V##EVEN_CASE(129);\
        V##ODD_CASE(258);

    #define ITOH518U M_ITOH518(,)
    #define P_ITOH518U M_ITOH518(P,)
    #define VP_ITOH518U M_ITOH518(VP,)
    #define PMS_ITOH518U M_ITOH518(P,_MULTISQR)
#elif(HFEn==519)
    #define M_ITOH519(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##ODD_CASE(64);\
        V##ODD_CASE(129);\
        V##EVEN_CASE(259);

    #define ITOH519U M_ITOH519(,)
    #define P_ITOH519U M_ITOH519(P,)
    #define VP_ITOH519U M_ITOH519(VP,)
    #define PMS_ITOH519U M_ITOH519(P,_MULTISQR)
#elif(HFEn==520)
    #define M_ITOH520(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##EVEN_CASE(32);\
        V##ODD_CASE(64);\
        V##ODD_CASE(129);\
        V##ODD_CASE(259);

    #define ITOH520U M_ITOH520(,)
    #define P_ITOH520U M_ITOH520(P,)
    #define VP_ITOH520U M_ITOH520(VP,)
    #define PMS_ITOH520U M_ITOH520(P,_MULTISQR)
#elif(HFEn==521)
    #define M_ITOH521(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##EVEN_CASE(65);\
        V##EVEN_CASE(130);\
        V##EVEN_CASE(260);

    #define ITOH521U M_ITOH521(,)
    #define P_ITOH521U M_ITOH521(P,)
    #define VP_ITOH521U M_ITOH521(VP,)
    #define PMS_ITOH521U M_ITOH521(P,_MULTISQR)
#elif(HFEn==522)
    #define M_ITOH522(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##EVEN_CASE(65);\
        V##EVEN_CASE(130);\
        V##ODD_CASE(260);

    #define ITOH522U M_ITOH522(,)
    #define P_ITOH522U M_ITOH522(P,)
    #define VP_ITOH522U M_ITOH522(VP,)
    #define PMS_ITOH522U M_ITOH522(P,_MULTISQR)
#elif(HFEn==523)
    #define M_ITOH523(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##EVEN_CASE(65);\
        V##ODD_CASE(130);\
        V##EVEN_CASE(261);

    #define ITOH523U M_ITOH523(,)
    #define P_ITOH523U M_ITOH523(P,)
    #define VP_ITOH523U M_ITOH523(VP,)
    #define PMS_ITOH523U M_ITOH523(P,_MULTISQR)
#elif(HFEn==524)
    #define M_ITOH524(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##EVEN_CASE(65);\
        V##ODD_CASE(130);\
        V##ODD_CASE(261);

    #define ITOH524U M_ITOH524(,)
    #define P_ITOH524U M_ITOH524(P,)
    #define VP_ITOH524U M_ITOH524(VP,)
    #define PMS_ITOH524U M_ITOH524(P,_MULTISQR)
#elif(HFEn==525)
    #define M_ITOH525(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##ODD_CASE(65);\
        V##EVEN_CASE(131);\
        V##EVEN_CASE(262);

    #define ITOH525U M_ITOH525(,)
    #define P_ITOH525U M_ITOH525(P,)
    #define VP_ITOH525U M_ITOH525(VP,)
    #define PMS_ITOH525U M_ITOH525(P,_MULTISQR)
#elif(HFEn==526)
    #define M_ITOH526(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##ODD_CASE(65);\
        V##EVEN_CASE(131);\
        V##ODD_CASE(262);

    #define ITOH526U M_ITOH526(,)
    #define P_ITOH526U M_ITOH526(P,)
    #define VP_ITOH526U M_ITOH526(VP,)
    #define PMS_ITOH526U M_ITOH526(P,_MULTISQR)
#elif(HFEn==527)
    #define M_ITOH527(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##ODD_CASE(65);\
        V##ODD_CASE(131);\
        V##EVEN_CASE(263);

    #define ITOH527U M_ITOH527(,)
    #define P_ITOH527U M_ITOH527(P,)
    #define VP_ITOH527U M_ITOH527(VP,)
    #define PMS_ITOH527U M_ITOH527(P,_MULTISQR)
#elif(HFEn==528)
    #define M_ITOH528(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##EVEN_CASE(16);\
        V##ODD_CASE(32);\
        V##ODD_CASE(65);\
        V##ODD_CASE(131);\
        V##ODD_CASE(263);

    #define ITOH528U M_ITOH528(,)
    #define P_ITOH528U M_ITOH528(P,)
    #define VP_ITOH528U M_ITOH528(VP,)
    #define PMS_ITOH528U M_ITOH528(P,_MULTISQR)
#elif(HFEn==529)
    #define M_ITOH529(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##EVEN_CASE(66);\
        V##EVEN_CASE(132);\
        V##EVEN_CASE(264);

    #define ITOH529U M_ITOH529(,)
    #define P_ITOH529U M_ITOH529(P,)
    #define VP_ITOH529U M_ITOH529(VP,)
    #define PMS_ITOH529U M_ITOH529(P,_MULTISQR)
#elif(HFEn==530)
    #define M_ITOH530(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##EVEN_CASE(66);\
        V##EVEN_CASE(132);\
        V##ODD_CASE(264);

    #define ITOH530U M_ITOH530(,)
    #define P_ITOH530U M_ITOH530(P,)
    #define VP_ITOH530U M_ITOH530(VP,)
    #define PMS_ITOH530U M_ITOH530(P,_MULTISQR)
#elif(HFEn==531)
    #define M_ITOH531(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##EVEN_CASE(66);\
        V##ODD_CASE(132);\
        V##EVEN_CASE(265);

    #define ITOH531U M_ITOH531(,)
    #define P_ITOH531U M_ITOH531(P,)
    #define VP_ITOH531U M_ITOH531(VP,)
    #define PMS_ITOH531U M_ITOH531(P,_MULTISQR)
#elif(HFEn==532)
    #define M_ITOH532(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##EVEN_CASE(66);\
        V##ODD_CASE(132);\
        V##ODD_CASE(265);

    #define ITOH532U M_ITOH532(,)
    #define P_ITOH532U M_ITOH532(P,)
    #define VP_ITOH532U M_ITOH532(VP,)
    #define PMS_ITOH532U M_ITOH532(P,_MULTISQR)
#elif(HFEn==533)
    #define M_ITOH533(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##ODD_CASE(66);\
        V##EVEN_CASE(133);\
        V##EVEN_CASE(266);

    #define ITOH533U M_ITOH533(,)
    #define P_ITOH533U M_ITOH533(P,)
    #define VP_ITOH533U M_ITOH533(VP,)
    #define PMS_ITOH533U M_ITOH533(P,_MULTISQR)
#elif(HFEn==534)
    #define M_ITOH534(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##ODD_CASE(66);\
        V##EVEN_CASE(133);\
        V##ODD_CASE(266);

    #define ITOH534U M_ITOH534(,)
    #define P_ITOH534U M_ITOH534(P,)
    #define VP_ITOH534U M_ITOH534(VP,)
    #define PMS_ITOH534U M_ITOH534(P,_MULTISQR)
#elif(HFEn==535)
    #define M_ITOH535(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##ODD_CASE(66);\
        V##ODD_CASE(133);\
        V##EVEN_CASE(267);

    #define ITOH535U M_ITOH535(,)
    #define P_ITOH535U M_ITOH535(P,)
    #define VP_ITOH535U M_ITOH535(VP,)
    #define PMS_ITOH535U M_ITOH535(P,_MULTISQR)
#elif(HFEn==536)
    #define M_ITOH536(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##EVEN_CASE(33);\
        V##ODD_CASE(66);\
        V##ODD_CASE(133);\
        V##ODD_CASE(267);

    #define ITOH536U M_ITOH536(,)
    #define P_ITOH536U M_ITOH536(P,)
    #define VP_ITOH536U M_ITOH536(VP,)
    #define PMS_ITOH536U M_ITOH536(P,_MULTISQR)
#elif(HFEn==537)
    #define M_ITOH537(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##EVEN_CASE(67);\
        V##EVEN_CASE(134);\
        V##EVEN_CASE(268);

    #define ITOH537U M_ITOH537(,)
    #define P_ITOH537U M_ITOH537(P,)
    #define VP_ITOH537U M_ITOH537(VP,)
    #define PMS_ITOH537U M_ITOH537(P,_MULTISQR)
#elif(HFEn==538)
    #define M_ITOH538(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##EVEN_CASE(67);\
        V##EVEN_CASE(134);\
        V##ODD_CASE(268);

    #define ITOH538U M_ITOH538(,)
    #define P_ITOH538U M_ITOH538(P,)
    #define VP_ITOH538U M_ITOH538(VP,)
    #define PMS_ITOH538U M_ITOH538(P,_MULTISQR)
#elif(HFEn==539)
    #define M_ITOH539(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##EVEN_CASE(67);\
        V##ODD_CASE(134);\
        V##EVEN_CASE(269);

    #define ITOH539U M_ITOH539(,)
    #define P_ITOH539U M_ITOH539(P,)
    #define VP_ITOH539U M_ITOH539(VP,)
    #define PMS_ITOH539U M_ITOH539(P,_MULTISQR)
#elif(HFEn==540)
    #define M_ITOH540(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##EVEN_CASE(67);\
        V##ODD_CASE(134);\
        V##ODD_CASE(269);

    #define ITOH540U M_ITOH540(,)
    #define P_ITOH540U M_ITOH540(P,)
    #define VP_ITOH540U M_ITOH540(VP,)
    #define PMS_ITOH540U M_ITOH540(P,_MULTISQR)
#elif(HFEn==541)
    #define M_ITOH541(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##ODD_CASE(67);\
        V##EVEN_CASE(135);\
        V##EVEN_CASE(270);

    #define ITOH541U M_ITOH541(,)
    #define P_ITOH541U M_ITOH541(P,)
    #define VP_ITOH541U M_ITOH541(VP,)
    #define PMS_ITOH541U M_ITOH541(P,_MULTISQR)
#elif(HFEn==542)
    #define M_ITOH542(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##ODD_CASE(67);\
        V##EVEN_CASE(135);\
        V##ODD_CASE(270);

    #define ITOH542U M_ITOH542(,)
    #define P_ITOH542U M_ITOH542(P,)
    #define VP_ITOH542U M_ITOH542(VP,)
    #define PMS_ITOH542U M_ITOH542(P,_MULTISQR)
#elif(HFEn==543)
    #define M_ITOH543(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##ODD_CASE(67);\
        V##ODD_CASE(135);\
        V##EVEN_CASE(271);

    #define ITOH543U M_ITOH543(,)
    #define P_ITOH543U M_ITOH543(P,)
    #define VP_ITOH543U M_ITOH543(VP,)
    #define PMS_ITOH543U M_ITOH543(P,_MULTISQR)
#elif(HFEn==544)
    #define M_ITOH544(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##EVEN_CASE(8);\
        V##ODD_CASE(16);\
        V##ODD_CASE(33);\
        V##ODD_CASE(67);\
        V##ODD_CASE(135);\
        V##ODD_CASE(271);

    #define ITOH544U M_ITOH544(,)
    #define P_ITOH544U M_ITOH544(P,)
    #define VP_ITOH544U M_ITOH544(VP,)
    #define PMS_ITOH544U M_ITOH544(P,_MULTISQR)
#elif(HFEn==545)
    #define M_ITOH545(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##EVEN_CASE(68);\
        V##EVEN_CASE(136);\
        V##EVEN_CASE(272);

    #define ITOH545U M_ITOH545(,)
    #define P_ITOH545U M_ITOH545(P,)
    #define VP_ITOH545U M_ITOH545(VP,)
    #define PMS_ITOH545U M_ITOH545(P,_MULTISQR)
#elif(HFEn==546)
    #define M_ITOH546(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##EVEN_CASE(68);\
        V##EVEN_CASE(136);\
        V##ODD_CASE(272);

    #define ITOH546U M_ITOH546(,)
    #define P_ITOH546U M_ITOH546(P,)
    #define VP_ITOH546U M_ITOH546(VP,)
    #define PMS_ITOH546U M_ITOH546(P,_MULTISQR)
#elif(HFEn==547)
    #define M_ITOH547(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##EVEN_CASE(68);\
        V##ODD_CASE(136);\
        V##EVEN_CASE(273);

    #define ITOH547U M_ITOH547(,)
    #define P_ITOH547U M_ITOH547(P,)
    #define VP_ITOH547U M_ITOH547(VP,)
    #define PMS_ITOH547U M_ITOH547(P,_MULTISQR)
#elif(HFEn==548)
    #define M_ITOH548(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##EVEN_CASE(68);\
        V##ODD_CASE(136);\
        V##ODD_CASE(273);

    #define ITOH548U M_ITOH548(,)
    #define P_ITOH548U M_ITOH548(P,)
    #define VP_ITOH548U M_ITOH548(VP,)
    #define PMS_ITOH548U M_ITOH548(P,_MULTISQR)
#elif(HFEn==549)
    #define M_ITOH549(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##ODD_CASE(68);\
        V##EVEN_CASE(137);\
        V##EVEN_CASE(274);

    #define ITOH549U M_ITOH549(,)
    #define P_ITOH549U M_ITOH549(P,)
    #define VP_ITOH549U M_ITOH549(VP,)
    #define PMS_ITOH549U M_ITOH549(P,_MULTISQR)
#elif(HFEn==550)
    #define M_ITOH550(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##ODD_CASE(68);\
        V##EVEN_CASE(137);\
        V##ODD_CASE(274);

    #define ITOH550U M_ITOH550(,)
    #define P_ITOH550U M_ITOH550(P,)
    #define VP_ITOH550U M_ITOH550(VP,)
    #define PMS_ITOH550U M_ITOH550(P,_MULTISQR)
#elif(HFEn==551)
    #define M_ITOH551(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##ODD_CASE(68);\
        V##ODD_CASE(137);\
        V##EVEN_CASE(275);

    #define ITOH551U M_ITOH551(,)
    #define P_ITOH551U M_ITOH551(P,)
    #define VP_ITOH551U M_ITOH551(VP,)
    #define PMS_ITOH551U M_ITOH551(P,_MULTISQR)
#elif(HFEn==552)
    #define M_ITOH552(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##EVEN_CASE(34);\
        V##ODD_CASE(68);\
        V##ODD_CASE(137);\
        V##ODD_CASE(275);

    #define ITOH552U M_ITOH552(,)
    #define P_ITOH552U M_ITOH552(P,)
    #define VP_ITOH552U M_ITOH552(VP,)
    #define PMS_ITOH552U M_ITOH552(P,_MULTISQR)
#elif(HFEn==553)
    #define M_ITOH553(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##EVEN_CASE(69);\
        V##EVEN_CASE(138);\
        V##EVEN_CASE(276);

    #define ITOH553U M_ITOH553(,)
    #define P_ITOH553U M_ITOH553(P,)
    #define VP_ITOH553U M_ITOH553(VP,)
    #define PMS_ITOH553U M_ITOH553(P,_MULTISQR)
#elif(HFEn==554)
    #define M_ITOH554(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##EVEN_CASE(69);\
        V##EVEN_CASE(138);\
        V##ODD_CASE(276);

    #define ITOH554U M_ITOH554(,)
    #define P_ITOH554U M_ITOH554(P,)
    #define VP_ITOH554U M_ITOH554(VP,)
    #define PMS_ITOH554U M_ITOH554(P,_MULTISQR)
#elif(HFEn==555)
    #define M_ITOH555(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##EVEN_CASE(69);\
        V##ODD_CASE(138);\
        V##EVEN_CASE(277);

    #define ITOH555U M_ITOH555(,)
    #define P_ITOH555U M_ITOH555(P,)
    #define VP_ITOH555U M_ITOH555(VP,)
    #define PMS_ITOH555U M_ITOH555(P,_MULTISQR)
#elif(HFEn==556)
    #define M_ITOH556(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##EVEN_CASE(69);\
        V##ODD_CASE(138);\
        V##ODD_CASE(277);

    #define ITOH556U M_ITOH556(,)
    #define P_ITOH556U M_ITOH556(P,)
    #define VP_ITOH556U M_ITOH556(VP,)
    #define PMS_ITOH556U M_ITOH556(P,_MULTISQR)
#elif(HFEn==557)
    #define M_ITOH557(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##ODD_CASE(69);\
        V##EVEN_CASE(139);\
        V##EVEN_CASE(278);

    #define ITOH557U M_ITOH557(,)
    #define P_ITOH557U M_ITOH557(P,)
    #define VP_ITOH557U M_ITOH557(VP,)
    #define PMS_ITOH557U M_ITOH557(P,_MULTISQR)
#elif(HFEn==558)
    #define M_ITOH558(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##ODD_CASE(69);\
        V##EVEN_CASE(139);\
        V##ODD_CASE(278);

    #define ITOH558U M_ITOH558(,)
    #define P_ITOH558U M_ITOH558(P,)
    #define VP_ITOH558U M_ITOH558(VP,)
    #define PMS_ITOH558U M_ITOH558(P,_MULTISQR)
#elif(HFEn==559)
    #define M_ITOH559(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##ODD_CASE(69);\
        V##ODD_CASE(139);\
        V##EVEN_CASE(279);

    #define ITOH559U M_ITOH559(,)
    #define P_ITOH559U M_ITOH559(P,)
    #define VP_ITOH559U M_ITOH559(VP,)
    #define PMS_ITOH559U M_ITOH559(P,_MULTISQR)
#elif(HFEn==560)
    #define M_ITOH560(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##EVEN_CASE(17);\
        V##ODD_CASE(34);\
        V##ODD_CASE(69);\
        V##ODD_CASE(139);\
        V##ODD_CASE(279);

    #define ITOH560U M_ITOH560(,)
    #define P_ITOH560U M_ITOH560(P,)
    #define VP_ITOH560U M_ITOH560(VP,)
    #define PMS_ITOH560U M_ITOH560(P,_MULTISQR)
#elif(HFEn==561)
    #define M_ITOH561(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##EVEN_CASE(70);\
        V##EVEN_CASE(140);\
        V##EVEN_CASE(280);

    #define ITOH561U M_ITOH561(,)
    #define P_ITOH561U M_ITOH561(P,)
    #define VP_ITOH561U M_ITOH561(VP,)
    #define PMS_ITOH561U M_ITOH561(P,_MULTISQR)
#elif(HFEn==562)
    #define M_ITOH562(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##EVEN_CASE(70);\
        V##EVEN_CASE(140);\
        V##ODD_CASE(280);

    #define ITOH562U M_ITOH562(,)
    #define P_ITOH562U M_ITOH562(P,)
    #define VP_ITOH562U M_ITOH562(VP,)
    #define PMS_ITOH562U M_ITOH562(P,_MULTISQR)
#elif(HFEn==563)
    #define M_ITOH563(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##EVEN_CASE(70);\
        V##ODD_CASE(140);\
        V##EVEN_CASE(281);

    #define ITOH563U M_ITOH563(,)
    #define P_ITOH563U M_ITOH563(P,)
    #define VP_ITOH563U M_ITOH563(VP,)
    #define PMS_ITOH563U M_ITOH563(P,_MULTISQR)
#elif(HFEn==564)
    #define M_ITOH564(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##EVEN_CASE(70);\
        V##ODD_CASE(140);\
        V##ODD_CASE(281);

    #define ITOH564U M_ITOH564(,)
    #define P_ITOH564U M_ITOH564(P,)
    #define VP_ITOH564U M_ITOH564(VP,)
    #define PMS_ITOH564U M_ITOH564(P,_MULTISQR)
#elif(HFEn==565)
    #define M_ITOH565(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##ODD_CASE(70);\
        V##EVEN_CASE(141);\
        V##EVEN_CASE(282);

    #define ITOH565U M_ITOH565(,)
    #define P_ITOH565U M_ITOH565(P,)
    #define VP_ITOH565U M_ITOH565(VP,)
    #define PMS_ITOH565U M_ITOH565(P,_MULTISQR)
#elif(HFEn==566)
    #define M_ITOH566(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##ODD_CASE(70);\
        V##EVEN_CASE(141);\
        V##ODD_CASE(282);

    #define ITOH566U M_ITOH566(,)
    #define P_ITOH566U M_ITOH566(P,)
    #define VP_ITOH566U M_ITOH566(VP,)
    #define PMS_ITOH566U M_ITOH566(P,_MULTISQR)
#elif(HFEn==567)
    #define M_ITOH567(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##ODD_CASE(70);\
        V##ODD_CASE(141);\
        V##EVEN_CASE(283);

    #define ITOH567U M_ITOH567(,)
    #define P_ITOH567U M_ITOH567(P,)
    #define VP_ITOH567U M_ITOH567(VP,)
    #define PMS_ITOH567U M_ITOH567(P,_MULTISQR)
#elif(HFEn==568)
    #define M_ITOH568(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##EVEN_CASE(35);\
        V##ODD_CASE(70);\
        V##ODD_CASE(141);\
        V##ODD_CASE(283);

    #define ITOH568U M_ITOH568(,)
    #define P_ITOH568U M_ITOH568(P,)
    #define VP_ITOH568U M_ITOH568(VP,)
    #define PMS_ITOH568U M_ITOH568(P,_MULTISQR)
#elif(HFEn==569)
    #define M_ITOH569(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##EVEN_CASE(71);\
        V##EVEN_CASE(142);\
        V##EVEN_CASE(284);

    #define ITOH569U M_ITOH569(,)
    #define P_ITOH569U M_ITOH569(P,)
    #define VP_ITOH569U M_ITOH569(VP,)
    #define PMS_ITOH569U M_ITOH569(P,_MULTISQR)
#elif(HFEn==570)
    #define M_ITOH570(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##EVEN_CASE(71);\
        V##EVEN_CASE(142);\
        V##ODD_CASE(284);

    #define ITOH570U M_ITOH570(,)
    #define P_ITOH570U M_ITOH570(P,)
    #define VP_ITOH570U M_ITOH570(VP,)
    #define PMS_ITOH570U M_ITOH570(P,_MULTISQR)
#elif(HFEn==571)
    #define M_ITOH571(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##EVEN_CASE(71);\
        V##ODD_CASE(142);\
        V##EVEN_CASE(285);

    #define ITOH571U M_ITOH571(,)
    #define P_ITOH571U M_ITOH571(P,)
    #define VP_ITOH571U M_ITOH571(VP,)
    #define PMS_ITOH571U M_ITOH571(P,_MULTISQR)
#elif(HFEn==572)
    #define M_ITOH572(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##EVEN_CASE(71);\
        V##ODD_CASE(142);\
        V##ODD_CASE(285);

    #define ITOH572U M_ITOH572(,)
    #define P_ITOH572U M_ITOH572(P,)
    #define VP_ITOH572U M_ITOH572(VP,)
    #define PMS_ITOH572U M_ITOH572(P,_MULTISQR)
#elif(HFEn==573)
    #define M_ITOH573(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##ODD_CASE(71);\
        V##EVEN_CASE(143);\
        V##EVEN_CASE(286);

    #define ITOH573U M_ITOH573(,)
    #define P_ITOH573U M_ITOH573(P,)
    #define VP_ITOH573U M_ITOH573(VP,)
    #define PMS_ITOH573U M_ITOH573(P,_MULTISQR)
#elif(HFEn==574)
    #define M_ITOH574(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##ODD_CASE(71);\
        V##EVEN_CASE(143);\
        V##ODD_CASE(286);

    #define ITOH574U M_ITOH574(,)
    #define P_ITOH574U M_ITOH574(P,)
    #define VP_ITOH574U M_ITOH574(VP,)
    #define PMS_ITOH574U M_ITOH574(P,_MULTISQR)
#elif(HFEn==575)
    #define M_ITOH575(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##ODD_CASE(71);\
        V##ODD_CASE(143);\
        V##EVEN_CASE(287);

    #define ITOH575U M_ITOH575(,)
    #define P_ITOH575U M_ITOH575(P,)
    #define VP_ITOH575U M_ITOH575(VP,)
    #define PMS_ITOH575U M_ITOH575(P,_MULTISQR)
#elif(HFEn==576)
    #define M_ITOH576(V,MS)\
        V##EVEN_CASE(2);\
        V##EVEN_CASE(4);\
        V##ODD_CASE(8);\
        V##ODD_CASE(17);\
        V##ODD_CASE(35);\
        V##ODD_CASE(71);\
        V##ODD_CASE(143);\
        V##ODD_CASE(287);

    #define ITOH576U M_ITOH576(,)
    #define P_ITOH576U M_ITOH576(P,)
    #define VP_ITOH576U M_ITOH576(VP,)
    #define PMS_ITOH576U M_ITOH576(P,_MULTISQR)
#else
#endif



#endif
