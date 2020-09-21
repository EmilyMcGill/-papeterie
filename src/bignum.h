// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_BIGNUM_H
#define BITCOIN_BIGNUM_H

#include "serialize.h"
#include "uint256.h"
#include "version.h"

#include <openssl/bn.h>

#include <stdexcept>
#include <vector>

#include <stdint.h>

/** Errors thrown by the bignum class */
class bignum_error : public std::runtime_error
{
public:
    explicit bignum_error(const std::string& str) : std::runtime_error(str) {}
};


/** RAII encapsulated BN_CTX (OpenSSL bignum context) */
class CAutoBN_CTX
{
protected:
    BN_CTX* pctx;
    BN_CTX* operator=(BN_CTX* pnew) { return pctx = pnew; }

public:
    CAutoBN_CTX()
    {
        pctx = BN_CTX_new();
        if (pctx == NULL)
            throw bignum_error("CAutoBN_CTX : BN_CTX_new() returned NULL");
    }

    ~CAutoBN_CTX()
    {
        if (pctx != NULL)
            BN_CTX_free(pctx);
    }

    operator BN_CTX*() { return pctx; }
    BN_CTX& operator*() { return *pctx; }
    BN_CTX** operator&() { return &pctx; }
    bool operator!() { return (pctx == NULL); }
};


/** C++ wrapper for BIGNUM (OpenSSL bignum) */
class CBigNum : public BIGNUM
{
public:
    CBigNum()
    {
        BN_init(this);
    }

    CBigNum(const CBigNum& b)
    {
        BN_init(this);
        if (!BN_copy(this, &b))
        {
            BN_clear_free(this);
            throw bignum_error("CBigNum::CBigNum(const CBigNum&) : BN_copy failed");
        }
    }

    CBigNum& operator=(const CBigNum& b)
    {
        if (!BN_copy(this, &b))
            throw bignum_error("CBigNum::operator= : BN_copy failed");
        return (*this);
    }

    ~CBigNum()
    {
        BN_clear_free(this);
    }

    //CBigNum(char n) is not portable.  Use 'signed char' or 'unsigned char'.
    CBigNum(signed char n)        { BN_init(this); if (n >= 0) setulong(n); else setint64(n); }
    CBigNum(short n)              { BN_init(this); if (n >= 0) setulong(n); else setint64(n); }
    CBigNum(int n)                { BN_init(this); if (n >= 0) setulong(n); else setint64(n); }
    CBigNum(long n)               { BN_init(this); if (n >= 0) setulong(n); else setint64(n); }
    CBigNum(long long n)          { BN_init(this); setint64(n); }
    CBigNum(unsigned char n)      { BN_init(this); setulong(n); }
    CBigNum(unsigned short n)     { BN_init(this); setulong(n); }
    CBigNum(unsigned int n)       { BN_init(this); setulong(n); }
    CBigNum(unsigned long n)      { BN_init(this); setulong(n); }
    CBigNum(unsigned long long n) { BN_init(this); setuint64(n); }
    explicit CBigNum(uint256 n)   { BN_init(this); setuint256(n); }

    explicit CBigNum(const std::vector<unsigned char>& vch)
    {
        BN_init(this);
        setvch(vch);
    }

    /** Generates a cryptographically secure random number between zero and range exclusive
    * i.e. 0 < returned number < range
    * @param range The upper bound on the number.
    * @return
    */
    static CBigNum  randBignum(const CBigNum& range) {
        CBigNum ret;
        if(!BN_rand_range(&ret, &range)){
            throw bignum_error("CBigNum:rand element : BN_rand_range failed");
        }
        return ret;
    }

    /** Generates a cryptographically secure random k-bit number
    * @param k The bit length of the number.
    * @return
    */
    static CBigNum RandKBitBigum(const uint32_t k){
        CBigNum ret;
        if(!BN_rand(&ret, k, -1, 0)){
            throw bignum_error("CBigNum:rand element : BN_rand failed");
        }
        return ret;
    }

    /**Returns the size in bits of the underlying bignum.
     *
     * @return the size
     */
    int bitSize() const{
        return  BN_num_bits(this);
    }


    void setulong(unsigned long n)
    {
        if (!BN_set_word(this, n))
            throw bignum_error("CBigNum conversion from unsigned long : BN_set_word failed");
    }

    unsigned long getulong() const
    {
        return BN_get_word(this);
    }

    unsigned int getuint() const
    {
        return BN_get_word(this);
    }

    int getint() const
    {
        unsigned long n = BN_get_word(this);
        if (!BN_is_negative(this))
            return (n > (unsigned long)std::numeric_limits<int>::max() ? std::numeric_limits<int>::max() : n);
        else
            return (n > (unsigned long)std::numeric_limits<int>::max() ? std::numeric_limits<int>::min() : -(int)n);
    }

    void setint64(int64_t sn)
    {
        unsigned char pch[sizeof(sn) + 6];
        unsigned char* p = pch + 4;
        bool fNegative;
        uint64_t n;

        if (sn < (int64_t)0)
        {
            // Since the minimum signed integer cannot be represented as positive so long as its type is signed, and it's not well-defined what happens if you make it unsigned before negating it, we instead increment the negative integer by 1, convert it, then increment the (now positive) unsigned integer by 1 to compensate
            n = -(sn + 1);
            ++n;
            fNegative = true;
        } else {
            n = sn;
            fNegative = false;
        }

        bool fLeadingZeroes = true;
        for (int i = 0; i < 8; i++)
        {
            unsigned char c = (n >> 56) & 0xff;
            n <<= 8;
            if (fLeadingZeroes)
            {
                if (c == 0)
                    continue;
                if (c & 0x80)
                    *p++ = (fNegative ? 0x80 : 0);
                else if (fNegative)
                    c |= 0x80;
                fLeadingZeroes = false;
            }
            *p++ = c;
        }
        unsigned int nSize = p - (pch + 4);
        pch[0] = (nSize >> 24) & 0xff;
        pch[1] = (nSize >> 16) & 0xff;
        pch[2] = (nSize >> 8) & 0xff;
        pch[3] = (nSize) & 0xff;
        BN_mpi2bn(pch, p - pch, this);
    }

    uint64_t getuint64()
    {
        unsigned int nSize = BN_bn2mpi(this, NULL);
        if (nSize < 4)
            return 0;
        std::vector<unsigned char> vch(nSize);
        BN_bn2mpi(this, &vch[0]);
        if (vch.size() > 4)
            vch[4] &= 0x7f;
        uint64_t n = 0;
        for (unsigned int i = 0, j = vch.size()-1; i < sizeof(n) && j >= 4; i++, j--)
            ((unsigned char*)&n)[i] = vch[j];
        return n;
    }

    void setuint64(uint64_t n)
    {
        unsigned char pch[sizeof(n) + 6];
        unsigned char* p = pch + 4;
        bool fLeadingZeroes = true;
        for (int i = 0; i < 8; i++)
        {
            unsigned char c = (n >> 56) & 0xff;
            n <<= 8;
            if (fLeadingZeroes)
            {
                if (c == 0)
                    continue;
                if (c & 0x80)
                    *p++ = 0;
                fLeadingZeroes = false;
            }
            *p++ = c;
        }
        unsigned int nSize = p - (pch + 4);
        pch[0] = (nSize >> 24) & 0xff;
        pch[1] = (nSize >> 16) & 0xff;
        pch[2] = (nSize >> 8) & 0xff;
        pch[3] = (nSize) & 0xff;
        BN_mpi2bn(pch, p - pch, this);
    }

    void setuint256(uint256 n)
    {
        unsigned char pch[sizeof(n) + 6];
        unsigned char* p = pch + 4;
        bool fLeadingZeroes = true;
        unsigned char* pbegin = (unsigned char*)&n;
        unsigned char* psrc = pbegin + sizeof(n);
        while (psrc != pbegin)
        {
            unsigned char c = *(--psrc);
            if (fLeadingZeroes)
            {
                if (c == 0)
                    continue;
                if (c & 0x80)
                    *p++ = 0;
                fLeadingZeroes = false;
            }
            *p++ = c;
        }
        unsigned int nSize = p - (pch + 4);
        pch[0] = (nSize >> 24) & 0xff;
        pch[1] = (nSize >> 16) & 0xff;
        pch[2] = (nSize >> 8) & 0xff;
        pch[3] = (nSize >> 0) & 0xff;
        BN_mpi2bn(pch, p - pch, this);
    }

    uint256 getuint256() const
    {
        unsigned int nSize = BN_bn2mpi(this, NULL);
        if (nSize < 4)
            return 0;
        std::vector<unsigned char> vch(nSize);
        BN_bn2mpi(this, &vch[0]);
        if (vch.size() > 4)
            vch[4] &= 0x7f;
        uint256 n = 0;
        for (unsigned int i = 0, j = vch.size()-1; i < sizeof(n) && j >= 4; i++, j--)
            ((unsigned char*)&n)[i] = vch[j];
        return n;
    }


    void setvch(const std::vector<unsigned char>& vch)
    {
        std::vector<unsigned char> vch2(vch.size() + 4);
        unsigned int nSize = vch.size();
        // BIGNUM's byte stream format expects 4 bytes of
        // big endian size data info at the front
        vch2[0] = (nSize >> 24) & 0xff;
        vch2[1] = (nSize >> 16) & 0xff;
        vch2[2] = (nSize >> 8) & 0xff;
        vch2[3] = (nSize >> 0) & 0xff;
        // swap data to big endian
        reverse_copy(vch.begin(), vch.end(), vch2.begin() + 4);
        BN_mpi2bn(&vch2[