#include "stdafx.h"

#include <sampling.hpp>

using namespace Texturize;

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Synthesizer Settings implementation                                                     /////
///////////////////////////////////////////////////////////////////////////////////////////////////

SynthesisSettings::SynthesisSettings(unsigned int rngState) :
	SynthesisSettings(cv::Point2f(0.f, 0.f))
{
}

SynthesisSettings::SynthesisSettings(cv::Point2f seedCoords, int kernel, unsigned int rngState) :
	_seedKernel(kernel), _seedCoords(seedCoords), _rngState(rngState)
{
}

bool SynthesisSettings::validate() const
{
	bool valid = true;

	// The seed kernel must be odd and the coords must be valid in uv-space.
	valid = !valid || _seedKernel % 2 == 1;
	valid = !valid || _seedCoords.x >= 0.f && _seedCoords.x < 1.f;
	valid = !valid || _seedCoords.y >= 0.f && _seedCoords.y < 1.f;

	return valid;
}

SynthesisSettings SynthesisSettings::random(int kernel, unsigned int state)
{
	// Randomly generate the seed coords.
	cv::RNG rng = cv::RNG(static_cast<unsigned long int>(state));

	return SynthesisSettings(cv::Point2f(rng.uniform(0.f, 1.f), rng.uniform(0.f, 1.f)), kernel, state);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Pyramid Synthesizer Settings implementation                                             /////
///////////////////////////////////////////////////////////////////////////////////////////////////

PyramidSynthesisSettings::PyramidSynthesisSettings(float scale, float randomness, unsigned int rngState) :
	PyramidSynthesisSettings(scale, cv::Point2f(0.f, 0.f), randomness, rngState)
{
}

PyramidSynthesisSettings::PyramidSynthesisSettings(float scale, const std::vector<float>& randomness, unsigned int rngState) :
	PyramidSynthesisSettings(scale, cv::Point2f(0.f, 0.f), randomness, rngState)
{
}

PyramidSynthesisSettings::PyramidSynthesisSettings(float scale, RandomnessSelectorFunction fn, unsigned int rngState) :
	PyramidSynthesisSettings(scale, cv::Point2f(0.f, 0.f), fn, rngState)
{
}

PyramidSynthesisSettings::PyramidSynthesisSettings(float scale, cv::Point2f seedCoords, float randomness, int kernel, unsigned int rngState) :
	PyramidSynthesisSettings(scale, cv::Point2f(0.f, 0.f), [randomness](int, const cv::Mat&) { return randomness; }, rngState)
{
}

PyramidSynthesisSettings::PyramidSynthesisSettings(float scale, cv::Point2f seedCoords, const std::vector<float>& randomness, int kernel, unsigned int rngState) :
	PyramidSynthesisSettings(scale, cv::Point2f(0.f, 0.f), [randomness](int l, const cv::Mat&) { return l >= randomness.size() ? 0.5f : randomness[l]; }, rngState)
{
}

PyramidSynthesisSettings::PyramidSynthesisSettings(float scale, cv::Point2f seedCoords, RandomnessSelectorFunction fn, int kernel, unsigned int rngState) :
	SynthesisSettings(seedCoords, kernel, rngState), _scale(scale), _randomnessSelector(fn)
{
}

PyramidSynthesisSettings PyramidSynthesisSettings::random(float scale, float randomness, int kernel, unsigned int state)
{
	// Randomly generate the seed coords.
	cv::RNG rng = cv::RNG(state);

	return PyramidSynthesisSettings(scale, cv::Point2f(rng.uniform(0.f, 1.f), rng.uniform(0.f, 1.f)), randomness, kernel, state);
}

PyramidSynthesisSettings PyramidSynthesisSettings::random(float scale, const std::vector<float>& randomness, int kernel, unsigned int state)
{
	// Randomly generate the seed coords.
	cv::RNG rng = cv::RNG(state);

	return PyramidSynthesisSettings(scale, cv::Point2f(rng.uniform(0.f, 1.f), rng.uniform(0.f, 1.f)), randomness, kernel, state);
}

PyramidSynthesisSettings PyramidSynthesisSettings::random(float scale, RandomnessSelectorFunction fn, int kernel, unsigned int state)
{
	// Randomly generate the seed coords.
	cv::RNG rng = cv::RNG(state);

	return PyramidSynthesisSettings(scale, cv::Point2f(rng.uniform(0.f, 1.f), rng.uniform(0.f, 1.f)), fn, kernel, state);
}