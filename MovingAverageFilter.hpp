#pragma once

#include <deque>
#include <numeric>

class MovingAverageFilter {
private:
    std::deque<double> samples;
    size_t maxSamples;
    double sum;

public:
    MovingAverageFilter(size_t maxSamples) : maxSamples(maxSamples), sum(0) {}

    void pushSample(double newSample) {
        samples.push_back(newSample);
        sum += newSample;

        if (samples.size() > maxSamples) {
            sum -= samples.front();
            samples.pop_front();
        }
    }

    bool isReady(){
        return samples.size() >= maxSamples;
    }

    double average() const {
        if (samples.empty()) {
            return 0.0;  // or a value that signifies no data available
        }

        return sum / samples.size();
    }
};

