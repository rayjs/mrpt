/* +---------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)               |
   |                          http://www.mrpt.org/                             |
   |                                                                           |
   | Copyright (c) 2005-2017, Individual contributors, see AUTHORS file        |
   | See: http://www.mrpt.org/Authors - All rights reserved.                   |
   | Released under BSD License. See details in http://www.mrpt.org/License    |
   +---------------------------------------------------------------------------+ */

#include <mrpt/poses/CPose3DInterpolator.h>
#include <mrpt/random/RandomGenerators.h>
#include <mrpt/system/datetime.h>

#include "common.h"

using mrpt::utils::DEG2RAD;

// ------------------------------------------------------
//				Benchmark Pose Interpolation
// ------------------------------------------------------

// XX ======================
template <bool INSERT_AT_END, bool BENCHMARK_INSERT_nQUERY>
double pose_interp3D_test_insert(int a1, int a2)
{
	const long N = 400000;
	mrpt::utils::CTicTac tictac;

	mrpt::poses::CPose3D a(1.0,2.0,3.0,DEG2RAD(10),DEG2RAD(50),DEG2RAD(-30));
	
	mrpt::poses::CPose3DInterpolator pose_path;
	const auto t0 = mrpt::system::now();
	const auto dt = mrpt::system::secondsToTimestamp(0.25);
	auto t = t0;

	std::vector<mrpt::system::TTimeStamp> Ats(N);
	for (long i = 0; i < N; i++) {
		if (INSERT_AT_END) {
			Ats[i] = dt;
		}
		else {
			Ats[i] = mrpt::system::secondsToTimestamp(mrpt::random::randomGenerator.drawUniform(-5.0, 5.0));
		}
	}

	if (BENCHMARK_INSERT_nQUERY)
	{
		tictac.Tic();
		for (long i = 0; i < N; i++)
		{
			pose_path.insert(t, a);
			t += Ats[i]; // negligible cost...
		}
		return tictac.Tac() / N;
	}
	else
	{
		for (long i = 0; i < N; i++)
		{
			pose_path.insert(t, a);
			t += 2*Ats[i];
		}
		t = t0 + mrpt::system::secondsToTimestamp(4.512);
		
		mrpt::poses::CPose3D p;
		bool valid;
		tictac.Tic();
		for (long i = 0; i < N; i++)
		{
			pose_path.interpolate(t, p, valid);
			t += Ats[i]; // negligible cost...
		}
		dummy_do_nothing_with_string( mrpt::format("%f %s",p.x(),valid? "YES":"NO") );
		return tictac.Tac() / N;
	}
}

// ------------------------------------------------------
// register_tests_pose_interp
// ------------------------------------------------------
void register_tests_pose_interp()
{
	mrpt::random::randomGenerator.randomize(1234);

	lstTests.push_back( TestData("CPose3DInterpolator: insert pose at end", &pose_interp3D_test_insert<true,true>) );
	lstTests.push_back( TestData("CPose3DInterpolator: insert pose random", &pose_interp3D_test_insert<false,true>) );
	lstTests.push_back( TestData("CPose3DInterpolator: query", &pose_interp3D_test_insert<true, false>));

}