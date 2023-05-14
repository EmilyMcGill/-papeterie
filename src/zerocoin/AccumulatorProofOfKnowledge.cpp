/**
 * @file       AccumulatorProofOfKnowledge.cpp
 *
 * @brief      AccumulatorProofOfKnowledge class for the Zerocoin library.
 *
 * @author     Ian Miers, Christina Garman and Matthew Green
 * @date       June 2013
 *
 * @copyright  Copyright 2013 Ian Miers, Christina Garman and Matthew Green
 * @license    This project is released under the MIT license.
 **/

#include "Zerocoin.h"

namespace libzerocoin {

AccumulatorProofOfKnowledge::AccumulatorProofOfKnowledge(const AccumulatorAndProofParams* p): params(p) {}

AccumulatorProofOfKnowledge::AccumulatorProofOfKnowledge(const AccumulatorAndProofParams* p,
        const Commitment& commitmentToCoin, const AccumulatorWitness& witness,
        Accumulator& a): params(p) {

	Bignum sg = params->accumulatorPoKCommitmentGroup.g;
	Bignum sh = params->accumulatorPoKCommitmentGroup.h;

	Bignum g_n = params->accumulatorQRNCommitmentGroup.g;
	Bignum h_n = params->accumulatorQRNCommitmentGroup.h;

	Bignum e = commitmentToCoin.getContents();
	Bignum r = commitmentToCoin.getRandomness();

	Bignum r_1 = Bignum::randBignum(params->accumulatorModulus/4);
	Bignum r_2 = Bignum::randBignum(params->accumulatorModulus/4);
	Bignum r_3 = Bignum::randBignum(params->accumulatorModulus/4);

	this->C_e = g_n.pow_mod(e, params->accumulatorModulus) * h_n.pow_mod(r_1, params->accumulatorModulus);
	this->C_u = witness.getValue() * h_n.pow_mod(r_2, params->accumulatorModulus);
	this->C_r = g_n.pow_mod(r_2, params->accumulatorModulus) * h_n.pow_mod(r_3, params->accumulatorModulus);

	Bignum r_alpha = Bignum::randBignum(params->maxCoinValue * Bignum(2).pow(params->k_prime + params->k_dprime));
	if(!(Bignum::randBignum(Bignum(3)) % 2)) {
		r_alpha = 0-r_alpha;
	}

	Bignum r_gamma = Bignum::randBignum(params->accumulatorPoKCommitmentGroup.modulus);
	Bignum r_phi = Bignum::randBignum(params->accumulatorPoKCommitmentGroup.modulus);
	Bignum r_psi = Bignum::randBignum(params->accumulatorPoKCommitmentGroup.modulus);
	Bignum r_sigma = Bignum::randBignum(params->accumulatorPoKCommitmentGroup.modulus);
	Bignum r_xi = Bignum::randBignum(params->accumulatorPoKCommitmentGroup.modulus);

	Bignum r_epsilon =  Bignum::randBignum((params->accumulatorModulus/4) * Bignum(2).pow(params->k_prime + params->k_dprime));
	if(!(Bignum::randBignum(Bignum(3)) % 2)) {
		r_epsilon = 0-r_epsilon;
	}
	Bignum r_eta = Bignum::randBignum((params->accumulatorModulus/4) * Bignum(2).pow(params->k_prime + params->k_dprime));
	if(!(Bignum::randBignum(Bignum(3)) % 2)) {
		r_eta = 0-r_eta;
	}
	Bignum r_zeta = Bignum::randBignum((params->accumulatorModulus/4) * Bignum(2).pow(params->k_prime + params->k_dprime));
	if(!(Bignum::randBignum(Bignum(3)) % 2)) {
		r_zeta = 0-r_zeta;
	}

	Bignum r_beta = Bignum::randBignum((params->accumulatorModulus/4) * params->accumulatorPoKCommitmentGroup.modulus * Bignum(2).pow(params->k_prime + params->k_dprime));
	if(!(Bignum::randBignum(Bignum(3)) % 2)) {
		r_beta = 0-r_beta;
	}
	Bignum r_delta = Bignum::randBignum((params->accumulatorModulus/4) * params->accumulatorPoKCommitmentGroup.modulus * Bignum(2).pow(params->k_prime + params->k_dprime));
	if(!(Bignum::randBignum(Bignum(3)) % 2)) {
		r_delta = 0-r_delta;
	}

	this->st_1 = (sg.pow_mod(r_alpha, params->accumulatorPoKCommitmentGroup.modulus) * sh.pow_mod(r_phi, params->accumulatorPoKCommitmentGroup.modulus)) % params->accumulatorPoKCommitmentGroup.modulus;
	this->st_2 = (((commitmentToCoin.getCommitmentValue() * sg.inverse(params->accumulatorPoKCommitmentGroup.modulus)).pow_mod(r_gamma, params->accumulatorPoKCommitmentGroup.modulus)) * sh.pow_mod(r_psi, params->accumulatorPoKCommitmentGroup.modulus)) % params->accumulatorPoKCommitmentGroup.modulus;
	this->st_3 = ((sg * commitmentToCoin.getCommitmentValue()).pow_mod(r_sigma, params->accumulatorPoKCommitmentGroup.modulus) * sh.pow_mod(r_xi, params->accumulatorPoKCommitmentGroup.modulus)) % params->accumulatorPoKCommitmentGroup.modulus;

	this->t_1 = (h_n.pow_mod(r_zeta, params->accumulatorModulus) * g_n.pow_mod(r_epsilon, params->accumulatorModulus)) % params->accumulatorModulus;
	this->t_2 = (h_n.pow_mod(r_eta, params->accumulatorModulus) * g_n.pow_mod(r_alpha, params->accumulatorModulus)) % params->accumulatorModulus;
	this->t_3 = (C_u.pow_mod(r_alpha, params->accumulatorModulus) * ((h_n.inverse(params->accumulatorModulus)).pow_mod(r_beta, params->accumulatorModulus))) % params->accumulatorModulus;
	this->t_4 = (C_r.pow_mod(r_alpha, params->accumulatorModulus) * ((h_n.inverse(params->accumulatorModulus)).pow_mod(r_delta, params->accumulatorModulus)) * ((g_n.inverse(params->accumulatorModulus)).pow_mod(r_beta, params->accumulatorModulus))) % params->accumulatorModulus;

	CHashWriter hasher(0,0);
	hasher << *params << sg << sh << g_n << h_n << commitmentT