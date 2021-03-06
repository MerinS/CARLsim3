/* 
 * Copyright (c) 2014 Regents of the University of California. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. The names of its contributors may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * *********************************************************************************************** *
 * CARLsim
 * created by: 		(MDR) Micah Richert, (JN) Jayram M. Nageswaran
 * maintained by:	(MA) Mike Avery <averym@uci.edu>, (MB) Michael Beyeler <mbeyeler@uci.edu>,
 *					(KDC) Kristofor Carlson <kdcarlso@uci.edu>
 *					(TSC) Ting-Shuo Chou <tingshuc@uci.edu>
 *
 * CARLsim available from http://socsci.uci.edu/~jkrichma/CARLsim/
 * Ver 11/26/2014
 */

#include <periodic_spikegen.h>

#include <user_errors.h>	// fancy error messages
#include <algorithm>		// std::find
#include <vector>			// std::vector
#include <cassert>			// assert

PeriodicSpikeGenerator::PeriodicSpikeGenerator(float rate, bool spikeAtZero) {
	assert(rate>0);
	rate_ = rate;	  // spike rate
	isi_ = 1000/rate; // inter-spike interval in ms
	spikeAtZero_ = spikeAtZero;

	checkFiringRate();
}

unsigned int PeriodicSpikeGenerator::nextSpikeTime(CARLsim* sim, int grpId, int nid, unsigned int currentTime, 
	unsigned int lastScheduledSpikeTime, unsigned int endOfTimeSlice) {
//		fprintf(stderr,"currentTime: %u lastScheduled: %u\n",currentTime,lastScheduledSpikeTime);

	if (spikeAtZero_) {
		// insert spike at t=0 for each neuron (keep track of neuron IDs to avoid getting stuck in infinite loop)
		if (std::find(nIdFiredAtZero_.begin(), nIdFiredAtZero_.end(), nid)==nIdFiredAtZero_.end()) {
			// spike at t=0 has not been scheduled yet for this neuron
			nIdFiredAtZero_.push_back(nid);
			return 0;
		}
	}

	// periodic spiking according to ISI
	return lastScheduledSpikeTime+isi_;
}

void PeriodicSpikeGenerator::checkFiringRate() {
	UserErrors::assertTrue(rate_>0, UserErrors::MUST_BE_POSITIVE, "PeriodicSpikeGenerator", "Firing rate");
}