/**
* @file       Params.cpp
*
* @brief      Parameter class for Zerocoin.
*
* @author     Ian Miers, Christina Garman and Matthew Green
* @date       June 2013
*
* @copyright  Copyright 2013 Ian Miers, Christina Garman and Matthew Green
* @license    This project is released under the MIT license.
**/
#include "Zerocoin.h"

namespace libzerocoin {

Params::Params(Bignum N, uint32_t securityLevel) {
	this->zkp_hash_len = securityLevel;
	this->zkp_iterations = securityLevel;

	this->accumulatorParams.k_prime = ACCPROOF_KPRIME;
	this->accumulatorParams.k_dprime = ACCPROOF_KDPRIME;

	// Generate the parameters
	CalculateParams(*this, N, ZEROCOIN_PROTOCOL_VERSION, securityLevel);

	this->accumulatorParams.initialized = true;
	this->initialized = true;
}

AccumulatorAndProofParams::AccumulatorAndProofParams() {
	this->initialized = false;
}

IntegerGroupParams::IntegerGroupParams() {
	this->initialized = false;
}

Bignum IntegerGroupParams::randomElement() const {
	// The generator of th