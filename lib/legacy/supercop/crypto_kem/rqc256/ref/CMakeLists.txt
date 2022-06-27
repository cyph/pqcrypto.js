
add_library(rqc OBJECT rqc_parsing.c rqc_pke.c rqc_kem.c)

target_include_directories(rqc PUBLIC ${SHA2_DIR})
target_include_directories(rqc PUBLIC ${FIPS202_DIR})
target_include_directories(rqc PUBLIC ${RANDOMBYTES_DIR})
target_include_directories(rqc PUBLIC ${SEEDEXPANDER_DIR})
target_include_directories(rqc PUBLIC ${CORE_DIR})
target_include_directories(rqc PUBLIC ${GABIDULIN_DIR})
target_include_directories(rqc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

