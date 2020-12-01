#ifndef BITCOIN_CHAINPARAMSSEEDS_H
#define BITCOIN_CHAINPARAMSSEEDS_H
/**
 * List of fixed seed nodes for the bitcoin network
 * AUTOGENERATED by contrib/seeds/generate-seeds.py
 *
 * Each line contains a 16-byte IPv6 address and a port.
 * IPv4 as well as onion addresses are wrapped inside a IPv6 address accordingly.
 */
static SeedSpec6 pnSeed6_main[] = {
    
    {{0xfd,0x87,0xd8,0x7e,0xeb,0x43,0xb7,0xb0,0xd1,0x08,0x71,0xcd,0xe9,0x2f,0xeb,0x1c}, 9894}
};

static SeedSpec6 pnSeed6_test[] = {
    
    {{0xfd,0x87,0xd8,0x7e,0xeb,0x43,0xab,0x47,0x92,0xd9,0xb7,0x3b,0x2d,0x6d,0xaa,0x1f}, 51474}
};
#endif // BITCOIN_CHAINPARAMSSEEDS_H