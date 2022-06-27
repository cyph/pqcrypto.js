#!/usr/bin/env python3

# Python port of genkat_aead.c.
#
# Authors, hereby denoted as "the implementer":
#     Kévin Le Gouguec,
#     2019.
#
# For more information, feedback or questions, refer to our website:
# https://paclido.fr/lilliput-ae
#
# To the extent possible under law, the implementer has waived all copyright
# and related or neighboring rights to the source code in this file.
# http://creativecommons.org/publicdomain/zero/1.0/

"""Python port of the genkat_aead.c program."""

from os import path
from sys import argv

import crypto_aead


class DecryptionError(Exception):
    def __init__(self):
        super().__init__('crypto_aead.decrypt did not recover the plaintext')


MAX_MESSAGE_LENGTH = 32
MAX_ASSOCIATED_DATA_LENGTH = 32


def print_bstr(output, label, buf):
    print('{l} = {b}'.format(l=label, b=buf.hex().upper()), file=output)


def generate_test_vectors(output_dir):
    count = 1
    filename = 'LWC_AEAD_KAT_{key}_{npub}.txt'.format(
        key=crypto_aead.KEYBYTES*8, npub=crypto_aead.NPUBBYTES*8
    )

    npub = bytes(range(crypto_aead.NPUBBYTES))
    key = bytes(range(crypto_aead.KEYBYTES))

    with open(path.join(output_dir, filename), 'w') as output:

        for mlen in range(MAX_MESSAGE_LENGTH+1):
            for adlen in range(MAX_ASSOCIATED_DATA_LENGTH+1):

                msg = bytes(range(mlen))
                ad = bytes(range(adlen))

                print('Count = {c}'.format(c=count), file=output)
                count += 1

                print_bstr(output, 'Key', key)
                print_bstr(output, 'Nonce', npub)
                print_bstr(output, 'PT', msg)
                print_bstr(output, 'AD', ad)

                ct = crypto_aead.encrypt(msg, ad, npub, key)

                print_bstr(output, 'CT', ct)

                msg2 = crypto_aead.decrypt(ct, ad, npub, key)

                if msg != msg2:
                    raise DecryptionError()

                print(file=output)


def main(argv):
    output_dir = path.curdir
    if len(argv) > 1:
        output_dir = argv[1]

    generate_test_vectors(output_dir)


if __name__ == '__main__':
    main(argv)
