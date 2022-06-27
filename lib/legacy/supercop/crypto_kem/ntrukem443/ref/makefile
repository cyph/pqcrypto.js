TARGET := ntru-kem-443

LIBS := -lm -lcrypto

C_SRCS += \
  NTRUEncrypt.c \
  PQCgenKAT_kem.c \
  kem.c \
  packing.c \
  param.c \
  ../common/rng.c \
  poly.c \
  ../common/crypto_hash_sha512.c \
  ../common/crypto_stream.c \
  ../common/fastrandombytes.c \
  ../common/shred.c 

OBJS += \
  NTRUEncrypt.o \
  PQCgenKAT_kem.o \
  kem.o \
  packing.o \
  param.o \
  ../common/rng.o \
  poly.o \
  ../common/crypto_hash_sha512.o \
  ../common/crypto_stream.o \
  ../common/fastrandombytes.o \
  ../common/shred.o 

C_DEPS += \
  NTRUEncrypt.d \
  PQCgenKAT_kem.d \
  kem.d \
  packing.d \
  param.d \
  ../common/rng.d \
  poly.d \
  ../common/crypto_hash_sha512.d \
  ../common/crypto_stream.d \
  ../common/fastrandombytes.d \
  ../common/shred.d 



%.o: %.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/include -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

poly/%.o: poly/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/include -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rng/%.o: rng/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/include -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

RM := rm -rf

# All Target
all: $(TARGET)

# Tool invocations
$(TARGET): $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	gcc  -o $(TARGET) $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) $(TARGET)
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

