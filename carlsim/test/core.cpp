#include "gtest/gtest.h"
#include "carlsim_tests.h"

#include <carlsim.h>
#include <vector>

#if defined(WIN32) || defined(WIN64)
#include <periodic_spikegen.h>
#endif

/// **************************************************************************************************************** ///
/// CORE FUNCTIONALITY
/// **************************************************************************************************************** ///

TEST(CORE, getGroupGrid3D) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	CARLsim* sim = new CARLsim("CORE.getGroupGrid3D",CPU_MODE,SILENT,0,42);
	Grid3D grid(2,3,4);
	int g2=sim->createGroup("excit2", grid, EXCITATORY_NEURON);
	sim->setNeuronParameters(g2, 0.02f, 0.2f, -65.0f, 8.0f);
	int g1=sim->createSpikeGeneratorGroup("excit", grid, EXCITATORY_NEURON);
	sim->connect(g1,g2,"full",RangeWeight(0.1), 1.0, RangeDelay(1));
	sim->setupNetwork(); // need SETUP state for this function to work

	for (int g=g1; g<g2; g++) {
		Grid3D getGrid = sim->getGroupGrid3D(g);
		EXPECT_EQ(getGrid.x, grid.x);
		EXPECT_EQ(getGrid.y, grid.y);
		EXPECT_EQ(getGrid.z, grid.z);
		EXPECT_EQ(getGrid.N, grid.N);
	}

	delete sim;
}

TEST(CORE, getGroupIdFromString) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	CARLsim* sim = new CARLsim("CORE.getGroupIdFromString",CPU_MODE,SILENT,0,42);
	int g2=sim->createGroup("bananahama", Grid3D(1,2,3), INHIBITORY_NEURON);
	sim->setNeuronParameters(g2, 0.02f, 0.2f, -65.0f, 8.0f);
	int g1=sim->createSpikeGeneratorGroup("excit", Grid3D(2,3,4), EXCITATORY_NEURON);
	sim->connect(g1,g2,"full",RangeWeight(0.1), 1.0, RangeDelay(1));
	sim->setupNetwork(); // need SETUP state for this function to work

	EXPECT_EQ(sim->getGroupId("excit"), g1);
	EXPECT_EQ(sim->getGroupId("bananahama"), g2);
	EXPECT_EQ(sim->getGroupId("invalid group name"), -1); // group not found

	delete sim;
}


// This test creates a group on a grid and makes sure that the returned 3D location of each neuron is correct
TEST(CORE, getNeuronLocation3D) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	CARLsim* sim = new CARLsim("CORE.getNeuronLocation3D",CPU_MODE,SILENT,0,42);
	Grid3D grid(2,3,4);
	int g2=sim->createGroup("excit2", grid, EXCITATORY_NEURON);
	sim->setNeuronParameters(g2, 0.02f, 0.2f, -65.0f, 8.0f);
	int g1=sim->createSpikeGeneratorGroup("excit", grid, EXCITATORY_NEURON);
	sim->connect(g1,g2,"full",RangeWeight(0.1), 1.0, RangeDelay(1));
	sim->setupNetwork(); // need SETUP state for getNeuronLocation3D to work

	// make sure the 3D location that is returned is correct
	for (int grp=0; grp<=1; grp++) {
		// do for both spike gen and RS group

		int x=0,y=0,z=0;
		for (int neurId=grp*grid.N; neurId<(grp+1)*grid.N; neurId++) {
			Point3D loc = sim->getNeuronLocation3D(neurId);
			EXPECT_FLOAT_EQ(loc.x, x-(grid.x-1)/2.0f);
			EXPECT_FLOAT_EQ(loc.y, y-(grid.y-1)/2.0f);
			EXPECT_FLOAT_EQ(loc.z, z-(grid.z-1)/2.0f);

			x++;
			if (x==grid.x) {
				x=0;
				y++;
			}
			if (y==grid.y) {
				x=0;
				y=0;
				z++;
			}
		}
	}

	delete sim;
}

/*
// \FIXME deactivate for now because we don't want to instantiate CpuSNN

// tests whether a point lies on a grid
TEST(CORE, isPoint3DonGrid) {
	CpuSNN snn("CORE.isPoint3DonGrid", CPU_MODE, SILENT, 0, 42);
	EXPECT_FALSE(snn.isPoint3DonGrid(Point3D(-1,-1,-1), Grid3D(10,5,2)));
	EXPECT_FALSE(snn.isPoint3DonGrid(Point3D(0.5,0.5,0.5), Grid3D(10,5,2)));
	EXPECT_FALSE(snn.isPoint3DonGrid(Point3D(10,5,2), Grid3D(10,5,2)));

	EXPECT_TRUE(snn.isPoint3DonGrid(Point3D(0,0,0), Grid3D(10,5,2)));
	EXPECT_TRUE(snn.isPoint3DonGrid(Point3D(0.0,0.0,0.0), Grid3D(10,5,2)));
	EXPECT_TRUE(snn.isPoint3DonGrid(Point3D(1,1,1), Grid3D(10,5,2)));
	EXPECT_TRUE(snn.isPoint3DonGrid(Point3D(9,4,1), Grid3D(10,5,2)));
	EXPECT_TRUE(snn.isPoint3DonGrid(Point3D(9.0,4.0,1.0), Grid3D(10,5,2)));
}
*/

// \TODO: using external current, make sure the Izhikevich model is correctly implemented
// Run izhikevich.org MATLAB script to find number of spikes as a function of neuron type,
// input current, and time period. Build test case to reproduce the exact numbers.

TEST(CORE, setExternalCurrent) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	CARLsim * sim;
	int nNeur = 10;

#ifdef __CPU_ONLY__
	int numModes = 1;
#else
	int numModes = 2;
#endif

	for (int hasCOBA=0; hasCOBA<=1; hasCOBA++) {
		for (int isGPUmode=0; isGPUmode<numModes; isGPUmode++) {
			sim = new CARLsim("CORE.setExternalCurrent", isGPUmode?GPU_MODE:CPU_MODE, SILENT, 0, 42);
			int g1=sim->createGroup("excit1", nNeur, EXCITATORY_NEURON);
			sim->setNeuronParameters(g1, 0.02f, 0.2f, -65.0f, 8.0f);
			int g0=sim->createSpikeGeneratorGroup("input0", nNeur, EXCITATORY_NEURON);
			sim->connect(g0,g1,"full",RangeWeight(0.1),1.0f,RangeDelay(1));
			sim->setConductances(hasCOBA>0);
			sim->setupNetwork();
//			fprintf(stderr, "setExternalCurrent %s %s\n",hasCOBA?"COBA":"CUBA",isGPUmode?"GPU":"CPU");

			SpikeMonitor* SM = sim->setSpikeMonitor(g1,"NULL");

			// run for a bunch, observe zero spikes since ext current should be zero by default
			SM->startRecording();
			sim->runNetwork(1,0);
			SM->stopRecording();
			EXPECT_EQ(SM->getPopNumSpikes(), 0);

			// set current, observe spikes
			std::vector<float> current(nNeur,7.0f);
			sim->setExternalCurrent(g1, current);
			SM->startRecording();
			sim->runNetwork(0,500);
			SM->stopRecording();
			EXPECT_GT(SM->getPopNumSpikes(), 0); // should be >0 in all cases
			for (int i=0; i<nNeur; i++) {
				EXPECT_EQ(SM->getNeuronNumSpikes(i), 8); // but actually should be ==8
			}

			// (intentionally) forget to reset current, observe spikes
			SM->startRecording();
			sim->runNetwork(0,500);
			SM->stopRecording();
			EXPECT_GT(SM->getPopNumSpikes(), 0); // should be >0 in all cases
			for (int i=0; i<nNeur; i++) {
				EXPECT_EQ(SM->getNeuronNumSpikes(i), 8); // but actually should be ==8
			}

			// reset current to zero
			sim->setExternalCurrent(g1, 0.0f);
			SM->startRecording();
			sim->runNetwork(0,500);
			SM->stopRecording();
			EXPECT_EQ(SM->getPopNumSpikes(), 0);

			// use convenience function to achieve same result as above
			sim->setExternalCurrent(g1, 7.0f);
			SM->startRecording();
			sim->runNetwork(0,500);
			SM->stopRecording();
			EXPECT_GT(SM->getPopNumSpikes(), 0); // should be >0 in all cases
			for (int i=0; i<nNeur; i++) {
				EXPECT_EQ(SM->getNeuronNumSpikes(i), 8); // but actually should be ==8
			}

			delete sim;
		}
	}
}

TEST(CORE, biasWeights) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	CARLsim* sim;
	int nNeur = 10;
	int *nSpkHighWt = new int[nNeur];
	memset(nSpkHighWt, 0, nNeur*sizeof(int));

#ifdef __CPU_ONLY__
	int numModes = 1;
#else
	int numModes = 2;
#endif

	for (int isGPUmode=0; isGPUmode<numModes; isGPUmode++) {
		sim = new CARLsim("CORE.biasWeights",isGPUmode?GPU_MODE:CPU_MODE,SILENT,0,42);
		int g1=sim->createGroup("excit", nNeur, EXCITATORY_NEURON);
		sim->setNeuronParameters(g1, 0.02f, 0.2f,-65.0f,8.0f);
		int c1=sim->connect(g1, g1, "one-to-one", RangeWeight(0.5f), 1.0f, RangeDelay(1));
		sim->setConductances(true);
		sim->setupNetwork();

		// ---- run network for a while with input current and high weight
		//      observe much spiking

		SpikeMonitor* SM = sim->setSpikeMonitor(g1,"NULL");
		sim->setExternalCurrent(g1, 7.0f);

		SM->startRecording();
		sim->runNetwork(2,0);
		SM->stopRecording();

		for (int neurId=0; neurId<nNeur; neurId++) {	
			nSpkHighWt[neurId] = SM->getNeuronNumSpikes(neurId);
		}


		// ---- run network for a while with zero weight (but still current injection)
		//      observe less spiking
		sim->biasWeights(c1, -0.25f, false);

		SM->startRecording();
		sim->runNetwork(2,0);
		SM->stopRecording();

		for (int neurId=0; neurId<nNeur; neurId++) {
			EXPECT_LT(SM->getNeuronNumSpikes(neurId), nSpkHighWt[neurId]);
		}

		delete sim;
	}

	delete[] nSpkHighWt;
}

TEST(CORE, scaleWeights) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	CARLsim* sim;
	int nNeur = 10;
	int *nSpkHighWt = new int[nNeur];
	memset(nSpkHighWt, 0, nNeur*sizeof(int));

#ifdef __CPU_ONLY__
	int numModes = 1;
#else
	int numModes = 2;
#endif

	for (int isGPUmode=0; isGPUmode<numModes; isGPUmode++) {
		sim = new CARLsim("CORE.scaleWeights",isGPUmode?GPU_MODE:CPU_MODE,SILENT,0,42);
		int g1=sim->createGroup("excit", nNeur, EXCITATORY_NEURON);
		sim->setNeuronParameters(g1, 0.02f, 0.2f,-65.0f,8.0f);
		int c1=sim->connect(g1, g1, "one-to-one", RangeWeight(0.5f), 1.0f, RangeDelay(1));
		sim->setConductances(true);
		sim->setupNetwork();

		// ---- run network for a while with input current and high weight
		//      observe much spiking

		SpikeMonitor* SM = sim->setSpikeMonitor(g1,"NULL");
		sim->setExternalCurrent(g1, 7.0f);

		SM->startRecording();
		sim->runNetwork(2,0);
		SM->stopRecording();

		for (int neurId=0; neurId<nNeur; neurId++) {	
			nSpkHighWt[neurId] = SM->getNeuronNumSpikes(neurId);
		}


		// ---- run network for a while with zero weight (but still current injection)
		//      observe less spiking
		sim->scaleWeights(c1, 0.5f, false);

		SM->startRecording();
		sim->runNetwork(2,0);
		SM->stopRecording();

		for (int neurId=0; neurId<nNeur; neurId++) {
			EXPECT_LT(SM->getNeuronNumSpikes(neurId), nSpkHighWt[neurId]);
		}

		delete sim;
	}

	delete[] nSpkHighWt;
}

TEST(CORE, setWeight) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	CARLsim* sim;
	int nNeur = 10;
	int *nSpkHighWt = new int[nNeur];
	memset(nSpkHighWt, 0, nNeur*sizeof(int));

#ifdef __CPU_ONLY__
	int numModes = 1;
#else
	int numModes = 2;
#endif

	for (int isGPUmode=0; isGPUmode<numModes; isGPUmode++) {
		sim = new CARLsim("CORE.setWeight",isGPUmode?GPU_MODE:CPU_MODE,SILENT,0,42);
		int g1=sim->createGroup("excit", nNeur, EXCITATORY_NEURON);
		sim->setNeuronParameters(g1, 0.02f, 0.2f,-65.0f,8.0f);
		int c1=sim->connect(g1, g1, "one-to-one", RangeWeight(0.5f), 1.0f, RangeDelay(1));
		sim->setConductances(true);
		sim->setupNetwork();

		// ---- run network for a while with input current and high weight
		//      observe much spiking

		SpikeMonitor* SM = sim->setSpikeMonitor(g1,"NULL");
		sim->setExternalCurrent(g1, 7.0f);

		SM->startRecording();
		sim->runNetwork(2,0);
		SM->stopRecording();

		for (int neurId=0; neurId<nNeur; neurId++) {	
			nSpkHighWt[neurId] = SM->getNeuronNumSpikes(neurId);
			sim->setWeight(c1, neurId, neurId, 0.0f, false);
		}


		// ---- run network for a while with zero weight (but still current injection)
		//      observe less spiking

		SM->startRecording();
		sim->runNetwork(2,0);
		SM->stopRecording();

		for (int neurId=0; neurId<nNeur; neurId++) {
			EXPECT_LT(SM->getNeuronNumSpikes(neurId), nSpkHighWt[neurId]);
		}

		delete sim;
	}

	delete[] nSpkHighWt;
}

TEST(CORE, getDelayRange) {
	CARLsim* sim;
	int nNeur = 10;
	int minDelay = 1;
	int maxDelay = 10;

#ifdef __CPU_ONLY__
	int numModes = 1;
#else
	int numModes = 2;
#endif

	for (int isGPUmode=0; isGPUmode<numModes; isGPUmode++) {
		sim = new CARLsim("CORE.getDelayRange",isGPUmode?GPU_MODE:CPU_MODE,SILENT,0,42);
		int g1=sim->createGroup("excit", nNeur, EXCITATORY_NEURON);
		sim->setNeuronParameters(g1, 0.02f, 0.2f,-65.0f,8.0f);
		int c1=sim->connect(g1, g1, "one-to-one", RangeWeight(0.5f), 1.0f, RangeDelay(minDelay,maxDelay));

		// config state right after connect
		RangeDelay delay = sim->getDelayRange(c1);
		EXPECT_EQ(delay.min, minDelay);
		EXPECT_EQ(delay.max, maxDelay);

		sim->setConductances(true);
		sim->setupNetwork();

		// setup state: still valid
		delay = sim->getDelayRange(c1);
		EXPECT_EQ(delay.min, minDelay);
		EXPECT_EQ(delay.max, maxDelay);

		sim->runNetwork(1,0);

		// exe state: still valid
		delay = sim->getDelayRange(c1);
		EXPECT_EQ(delay.min, minDelay);
		EXPECT_EQ(delay.max, maxDelay);

		delete sim;
	}
}

TEST(CORE, getWeightRange) {
	CARLsim* sim;
	int nNeur = 10;
	float minWt = 0.0f;
	float initWt = 1.25f;
	float maxWt = 10.0f;

#ifdef __CPU_ONLY__
	int numModes = 1;
#else
	int numModes = 2;
#endif

	for (int isGPUmode=0; isGPUmode<numModes; isGPUmode++) {
		sim = new CARLsim("CORE.getWeightRange",isGPUmode?GPU_MODE:CPU_MODE,SILENT,0,42);
		int g1=sim->createGroup("excit", nNeur, EXCITATORY_NEURON);
		sim->setNeuronParameters(g1, 0.02f, 0.2f,-65.0f,8.0f);
		int c1=sim->connect(g1, g1, "one-to-one", RangeWeight(minWt,initWt,maxWt), 1.0f, RangeDelay(1), RadiusRF(-1),
			SYN_PLASTIC);

		// config state right after connect
		RangeWeight wt = sim->getWeightRange(c1);
		EXPECT_EQ(wt.min, minWt);
		EXPECT_EQ(wt.init, initWt);
		EXPECT_EQ(wt.max, maxWt);

		sim->setConductances(true);
		sim->setupNetwork();

		// setup state: still valid
		wt = sim->getWeightRange(c1);
		EXPECT_EQ(wt.min, minWt);
		EXPECT_EQ(wt.init, initWt);
		EXPECT_EQ(wt.max, maxWt);

		sim->runNetwork(1,0);

		// exe state: still valid
		wt = sim->getWeightRange(c1);
		EXPECT_EQ(wt.min, minWt);
		EXPECT_EQ(wt.init, initWt);
		EXPECT_EQ(wt.max, maxWt);

		delete sim;
	}
}


// make sure bookkeeping for number of groups is correct during CONFIG
TEST(CORE, numGroups) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	CARLsim sim("CORE.numGroups", CPU_MODE, SILENT, 0, 42);
	EXPECT_EQ(sim.getNumGroups(), 0);

	int nLoops = 4;
	int nNeur = 10;
	for (int i=0; i<nLoops; i++) {
		sim.createGroup("regexc", nNeur, EXCITATORY_NEURON);
		EXPECT_EQ(sim.getNumGroups(), i*4+1);
		sim.createGroup("reginh", nNeur, INHIBITORY_NEURON);
		EXPECT_EQ(sim.getNumGroups(), i*4+2);
		sim.createSpikeGeneratorGroup("genexc", nNeur, EXCITATORY_NEURON);
		EXPECT_EQ(sim.getNumGroups(), i*4+3);
		sim.createSpikeGeneratorGroup("geninh", nNeur, INHIBITORY_NEURON);
		EXPECT_EQ(sim.getNumGroups(), i*4+4);
	}
}

// make sure bookkeeping for number of neurons is correct during CONFIG
TEST(CORE, numNeurons) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	CARLsim sim("CORE.numNeurons", CPU_MODE, SILENT, 0, 42);
	EXPECT_EQ(sim.getNumNeurons(), 0);
	EXPECT_EQ(sim.getNumNeuronsRegExc(), 0);
	EXPECT_EQ(sim.getNumNeuronsRegInh(), 0);
	EXPECT_EQ(sim.getNumNeuronsGenExc(), 0);
	EXPECT_EQ(sim.getNumNeuronsGenInh(), 0);

	int nLoops = 4;
	int nNeur = 10;

	for (int i=0; i<nLoops; i++) {
		sim.createGroup("regexc", nNeur, EXCITATORY_NEURON);
		EXPECT_EQ(sim.getNumNeurons(), i*4*nNeur + nNeur);
		EXPECT_EQ(sim.getNumNeuronsRegExc(), i*nNeur + nNeur);
		EXPECT_EQ(sim.getNumNeuronsRegInh(), i*nNeur);
		EXPECT_EQ(sim.getNumNeuronsGenExc(), i*nNeur);
		EXPECT_EQ(sim.getNumNeuronsGenInh(), i*nNeur);
		EXPECT_EQ(sim.getNumNeurons(), sim.getNumNeuronsRegExc() + sim.getNumNeuronsRegInh()
			+ sim.getNumNeuronsGenExc() + sim.getNumNeuronsGenInh());
		EXPECT_EQ(sim.getNumNeuronsReg(), sim.getNumNeuronsRegExc() + sim.getNumNeuronsRegInh());
		EXPECT_EQ(sim.getNumNeuronsGen(), sim.getNumNeuronsGenExc() + sim.getNumNeuronsGenInh());

		sim.createGroup("reginh", nNeur, INHIBITORY_NEURON);
		EXPECT_EQ(sim.getNumNeurons(), i*4*nNeur + 2*nNeur);
		EXPECT_EQ(sim.getNumNeuronsRegExc(), i*nNeur + nNeur);
		EXPECT_EQ(sim.getNumNeuronsRegInh(), i*nNeur + nNeur);
		EXPECT_EQ(sim.getNumNeuronsGenExc(), i*nNeur);
		EXPECT_EQ(sim.getNumNeuronsGenInh(), i*nNeur);
		EXPECT_EQ(sim.getNumNeurons(), sim.getNumNeuronsRegExc() + sim.getNumNeuronsRegInh()
			+ sim.getNumNeuronsGenExc() + sim.getNumNeuronsGenInh());
		EXPECT_EQ(sim.getNumNeuronsReg(), sim.getNumNeuronsRegExc() + sim.getNumNeuronsRegInh());
		EXPECT_EQ(sim.getNumNeuronsGen(), sim.getNumNeuronsGenExc() + sim.getNumNeuronsGenInh());

		sim.createSpikeGeneratorGroup("genexc", nNeur, EXCITATORY_NEURON);
		EXPECT_EQ(sim.getNumNeurons(), i*4*nNeur + 3*nNeur);
		EXPECT_EQ(sim.getNumNeuronsRegExc(), i*nNeur + nNeur);
		EXPECT_EQ(sim.getNumNeuronsRegInh(), i*nNeur + nNeur);
		EXPECT_EQ(sim.getNumNeuronsGenExc(), i*nNeur + nNeur);
		EXPECT_EQ(sim.getNumNeuronsGenInh(), i*nNeur);
		EXPECT_EQ(sim.getNumNeurons(), sim.getNumNeuronsRegExc() + sim.getNumNeuronsRegInh()
			+ sim.getNumNeuronsGenExc() + sim.getNumNeuronsGenInh());
		EXPECT_EQ(sim.getNumNeuronsReg(), sim.getNumNeuronsRegExc() + sim.getNumNeuronsRegInh());
		EXPECT_EQ(sim.getNumNeuronsGen(), sim.getNumNeuronsGenExc() + sim.getNumNeuronsGenInh());

		sim.createSpikeGeneratorGroup("geninh", nNeur, INHIBITORY_NEURON);
		EXPECT_EQ(sim.getNumNeurons(), i*4*nNeur + 4*nNeur);
		EXPECT_EQ(sim.getNumNeuronsRegExc(), i*nNeur + nNeur);
		EXPECT_EQ(sim.getNumNeuronsRegInh(), i*nNeur + nNeur);
		EXPECT_EQ(sim.getNumNeuronsGenExc(), i*nNeur + nNeur);
		EXPECT_EQ(sim.getNumNeuronsGenInh(), i*nNeur + nNeur);
		EXPECT_EQ(sim.getNumNeurons(), sim.getNumNeuronsRegExc() + sim.getNumNeuronsRegInh()
			+ sim.getNumNeuronsGenExc() + sim.getNumNeuronsGenInh());
		EXPECT_EQ(sim.getNumNeuronsReg(), sim.getNumNeuronsRegExc() + sim.getNumNeuronsRegInh());
		EXPECT_EQ(sim.getNumNeuronsGen(), sim.getNumNeuronsGenExc() + sim.getNumNeuronsGenInh());
	}
}

TEST(CORE, startStopTestingPhase) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	CARLsim* sim;

#ifdef __CPU_ONLY__
	int numModes = 1;
#else
	int numModes = 2;
#endif

	// run twice, once with expected start/stop order, once with a bunch of additional (but
	// irrelevant start/stop calls)
	for (int run=0; run<=1; run++) {
		for (int mode=0; mode<numModes; mode++) {
			sim = new CARLsim("CORE.startStopTestingPhase",mode?GPU_MODE:CPU_MODE,SILENT,0,42);

			int gExc = sim->createGroup("output", 1, EXCITATORY_NEURON);
			sim->setNeuronParameters(gExc, 0.02f, 0.2f, -65.0f, 8.0f); // RS
			int gIn = sim->createSpikeGeneratorGroup("input", 10, EXCITATORY_NEURON);

			int cInExc  = sim->connect(gIn, gExc, "full", RangeWeight(0.0f, 0.5f, 0.5f), 1.0f, RangeDelay(1), 
				RadiusRF(-1), SYN_PLASTIC);

			// set E-STDP to be STANDARD (without neuromodulatory influence) with an EXP_CURVE type.
			sim->setESTDP(gExc, true, STANDARD, ExpCurve(2e-4f,20.0f, -6.6e-5f,60.0f));
			sim->setHomeostasis(gExc, true, 1.0f, 10.0f);  // homeo scaling factor, avg time scale
			sim->setHomeoBaseFiringRate(gExc, 35.0f, 0.0f); // target firing, target firing st.d.

			sim->setConductances(true);
			sim->setupNetwork();
			ConnectionMonitor* CM = sim->setConnectionMonitor(gIn, gExc, "NULL");

			PoissonRate PR(10);
			PR.setRates(50.0f);
			sim->setSpikeRate(gIn, &PR);

			// training: expect weight changes due to STDP
			if (run==1) {
				sim->startTesting(); // testing function calls in SETUP_STATE
				sim->stopTesting();
			}
			sim->runNetwork(1,0);
			double wtChange = CM->getTotalAbsWeightChange();
			EXPECT_GT(CM->getTotalAbsWeightChange(), 0);
			EXPECT_EQ(CM->getTimeMsCurrentSnapshot(), 1000);
			EXPECT_EQ(CM->getTimeMsLastSnapshot(), 0);
			EXPECT_EQ(CM->getTimeMsSinceLastSnapshot(), 1000);

			// testing: expect no weight changes
			sim->startTesting();
			if (run==1) {
				sim->runNetwork(5,0);
				sim->startTesting(); // start after start: redundant
				sim->runNetwork(5,0);
			} else {
				sim->runNetwork(10,0);
			}
			EXPECT_FLOAT_EQ(CM->getTotalAbsWeightChange(), 0.0f);
			EXPECT_EQ(CM->getTimeMsCurrentSnapshot(), 11000);
			EXPECT_EQ(CM->getTimeMsLastSnapshot(), 1000);
			EXPECT_EQ(CM->getTimeMsSinceLastSnapshot(), 10000);

			// some more training: expect weight changes
			sim->stopTesting();
			CM->takeSnapshot();
			sim->runNetwork(5,0);
			EXPECT_GT(CM->getTotalAbsWeightChange(), 0);
			EXPECT_EQ(CM->getTimeMsCurrentSnapshot(), 16000);
			EXPECT_EQ(CM->getTimeMsLastSnapshot(), 11000);
			EXPECT_EQ(CM->getTimeMsSinceLastSnapshot(), 5000);

			delete sim;
		}
	}
}

TEST(CORE, saveLoadSimulation) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	float tauPlus = 20.0f;
	float tauMinus = 20.0f;
	float alphaPlus = 0.1f;
	float alphaMinus = 0.15f;
	int gPre, gPost;
	ConnectionMonitor* cmSave;
	ConnectionMonitor* cmLoad;
    // all neurons get input of 6 Hz.
	PeriodicSpikeGenerator spkGenG0(6.0f);
	std::vector<std::vector<float> > weightsSave;
	std::vector<std::vector<float> > weightsLoad;

#ifdef __CPU_ONLY__
	int numModes = 1;
#else
	int numModes = 2;
#endif

	for (int mode=0; mode<numModes; mode++) {
		for (int coba=0; coba<=1; coba++) {
			for (int connType = 0; connType < 5; connType++) {
				for (int loadSim=0; loadSim<=1; loadSim++) {
					// Run and save simulation ------------------------------ //
					CARLsim* sim = new CARLsim("CORE.saveSimulation", mode?GPU_MODE:CPU_MODE, SILENT, 0, 42);
					FILE* simFid = NULL;
					PoissonRate* poisRate = new PoissonRate(100, false);

					gPost = sim->createGroup("post-ex", Grid3D(10, 10), EXCITATORY_NEURON);
					sim->setNeuronParameters(gPost, 0.02f, 0.2f, -65.0f, 8.0f);
					gPre = sim->createSpikeGeneratorGroup("pre-ex", Grid3D(10, 10), EXCITATORY_NEURON);

					switch (connType) {
					case 0:
						if (coba > 0) {
							sim->connect(gPre, gPost, "full", RangeWeight(0.0, 2.0f/100, 20.0f/100), 1.0f, RangeDelay(1, 20),
									RadiusRF(-1), SYN_PLASTIC);
							sim->setSTDP(gPost, true, STANDARD, alphaPlus/100, tauPlus, alphaMinus/100, tauMinus);
							sim->setConductances(true);
						} else {
							sim->connect(gPre, gPost, "full", RangeWeight(0.0, 2.0f, 20.0f), 1.0f, RangeDelay(1, 20),
									RadiusRF(-1), SYN_PLASTIC);
							sim->setSTDP(gPost, true, STANDARD, alphaPlus, tauPlus, alphaMinus, tauMinus);
							sim->setConductances(false);
						}
						break;
					case 1:
						if (coba > 0) {
							sim->connect(gPre, gPost, "full-no-direct", RangeWeight(0.0, 2.0f/100, 20.0f/100), 1.0f, RangeDelay(1, 20),
									RadiusRF(-1), SYN_PLASTIC);
							sim->setSTDP(gPost, true, STANDARD, alphaPlus/100, tauPlus, alphaMinus/100, tauMinus);
							sim->setConductances(true);
						} else {
							sim->connect(gPre, gPost, "full-no-direct", RangeWeight(0.0, 2.0f, 20.0f), 1.0f, RangeDelay(1, 20),
									RadiusRF(-1), SYN_PLASTIC);
							sim->setSTDP(gPost, true, STANDARD, alphaPlus, tauPlus, alphaMinus, tauMinus);
							sim->setConductances(false);
						}
						break;
					case 2:
						if (coba > 0) {
							sim->connect(gPre, gPost, "one-to-one", RangeWeight(0.0, 10.0f/100, 20.0f/100), 1.0f, RangeDelay(1, 20),
									RadiusRF(-1), SYN_PLASTIC);
							sim->setSTDP(gPost, true, STANDARD, alphaPlus/100, tauPlus, alphaMinus/100, tauMinus);
							sim->setConductances(true);
						} else {
							sim->connect(gPre, gPost, "one-to-one", RangeWeight(0.0, 10.0f, 20.0f), 1.0f, RangeDelay(1, 20),
									RadiusRF(-1), SYN_PLASTIC);
							sim->setSTDP(gPost, true, STANDARD, alphaPlus, tauPlus, alphaMinus, tauMinus);
							sim->setConductances(false);
						}
						break;
					case 3:
						if (coba > 0) {
							sim->connect(gPre, gPost, "random", RangeWeight(0.0, 5.0f/100, 20.0f/100), 0.2f, RangeDelay(1, 20),
									RadiusRF(-1), SYN_PLASTIC);
							sim->setSTDP(gPost, true, STANDARD, alphaPlus/100, tauPlus, alphaMinus/100, tauMinus);
							sim->setConductances(true);
						} else {
							sim->connect(gPre, gPost, "random", RangeWeight(0.0, 5.0f, 20.0f), 0.2f, RangeDelay(1, 20),
									RadiusRF(-1), SYN_PLASTIC);
							sim->setSTDP(gPost, true, STANDARD, alphaPlus, tauPlus, alphaMinus, tauMinus);
							sim->setConductances(false);
						}
						break;
					case 4:
						if (coba > 0) {
							sim->connect(gPre, gPost, "gaussian", RangeWeight(0.0, 5.0f/100, 20.0f/100), 0.4f, RangeDelay(1, 20),
									RadiusRF(8, 8, 0), SYN_PLASTIC);
							sim->setSTDP(gPost, true, STANDARD, alphaPlus/100, tauPlus, alphaMinus/100, tauMinus);
							sim->setConductances(true);
						} else {
							sim->connect(gPre, gPost, "gaussian", RangeWeight(0.0, 5.0f, 20.0f), 0.4f, RangeDelay(1, 20),
									RadiusRF(8, 8, 0), SYN_PLASTIC);
							sim->setSTDP(gPost, true, STANDARD, alphaPlus, tauPlus, alphaMinus, tauMinus);
							sim->setConductances(false);
						}
						break;
					default:
						EXPECT_TRUE(false);
						break;
					}

					if (loadSim) {
					// load previous simulation
						simFid = fopen("results/sim.dat", "rb");
						sim->loadSimulation(simFid);
					}

					sim->setupNetwork();

					poisRate->setRates(10.0f);
					sim->setSpikeRate(gPre, poisRate);

					if (!loadSim) {
						// first run: save network at the end
						cmSave = sim->setConnectionMonitor(gPre, gPost, "NULL");
						sim->runNetwork(20, 0, false, false);

						weightsSave = cmSave->takeSnapshot();
						sim->saveSimulation("results/sim.dat", true);
					} else {
						// second run: load simulation
						cmLoad = sim->setConnectionMonitor(gPre, gPost, "NULL");
						sim->runNetwork(0, 2, false, false);
						weightsLoad = cmLoad->takeSnapshot();

						// test weights we saved are the same as weights we loaded
						for (int i = 0; i < sim->getGroupNumNeurons(gPre); i++) {
							for (int j = 0; j < sim->getGroupNumNeurons(gPost); j++) {
								if(isnan(weightsSave[i][j]))
									EXPECT_TRUE(isnan(weightsLoad[i][j]));
								else {						
									EXPECT_FLOAT_EQ(weightsSave[i][j], weightsLoad[i][j]);
									//printf("(%f,%f) ", weightsSave[i][j], weightsLoad[i][j]);
								}
							}
						}
					}

					// close sim.dat
					if (simFid != NULL) fclose(simFid);
					delete poisRate;
					delete sim;
				}
			}
		}
	}
}

// repeat a config phase where we forget to call setNeuronParameters on one group: if that group is a regular
// group, we expect the simulation to break upon calling setupNetwork
TEST(CORE, setNeuronParameters) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

#ifdef __CPU_ONLY__
	int numModes = 1;
#else
	int numModes = 2;
#endif

	for (int runs=0; runs<5; runs++) {
		// don't call setNeuronParams on one group picked at random
		int numGroups = 10;
		int forgetGroup = rand() % numGroups;

		for (int mode=0; mode<numModes; mode++) {
			for (int coba=0; coba<=1; coba++) {
				CARLsim* sim = new CARLsim("CORE.setNeuronParameters", mode?GPU_MODE:CPU_MODE, SILENT, 0, 42);

				for (int g=0; g<numGroups; g++) {
					if (1.0 * rand() / RAND_MAX < 0.5) {
						sim->createSpikeGeneratorGroup("name", 1, EXCITATORY_NEURON);
					} else {
						sim->createGroup("name", 1, EXCITATORY_NEURON);
					}
				}
				sim->setConductances(coba>0);

				for (int g=0; g<numGroups; g++) {
					if (g != forgetGroup && !sim->isPoissonGroup(g)) {
						sim->setNeuronParameters(g, 0.1f, 0.2f, 0.3f, 0.4f);
					}
				}

				if (!sim->isPoissonGroup(forgetGroup)) {
					// we forgot setNeuronParams
					EXPECT_DEATH({sim->setupNetwork();},"");
				}

				delete sim;
			}
		}
	}
}
